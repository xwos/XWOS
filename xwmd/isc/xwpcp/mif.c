/**
 * @file
 * @brief 点对点通讯协议：应用接口层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/mif.h>

#define XWPCP_TXTHD_PRIORITY XWMDCFG_isc_xwpcp_TXTHD_PRIORITY
#define XWPCP_RXTHD_PRIORITY XWMDCFG_isc_xwpcp_RXTHD_PRIORITY

/**
 * @brief 发送线程的描述
 */
static __xwmd_rodata
const struct xwos_thd_desc xwpcp_rxthd_desc = {
        .name = "xwmd.isc.xwpcp.rxthd",
        .prio = XWPCP_RXTHD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thd_f)xwpcp_rxthd,
        .arg = NULL, /* TBD */
        .attr = XWOS_SKDATTR_PRIVILEGED,
};

/**
 * @brief 发送线程的描述
 */
static __xwmd_rodata
const struct xwos_thd_desc xwpcp_txthd_desc = {
        .name = "xwmd.isc.xwpcp.txthd",
        .prio = XWPCP_TXTHD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thd_f)xwpcp_txthd,
        .arg = NULL, /* TBD */
        .attr = XWOS_SKDATTR_PRIVILEGED,
};

/**
 * @brief 内存池名字
 */
static __xwmd_rodata
const char xwpcp_mempool_name[] = "xwpcp.mempool";

static __xwmd_code
void xwpcp_txcb_notify(struct xwpcp * xwpcp, xwpcp_txh_t txh, xwer_t rc, void * arg);

static __xwmd_code
xwer_t xwpcp_gc(void * obj);

/**
 * @brief XWPCP API: 启动XWPCP
 * @param xwpcp: (I) XWPCP对象的指针
 * @param name: (I) XWPCP实例的名字
 * @param hwifops: (I) 硬件接口抽象层操作函数集合
 * @param hwifcb: (I) 硬件接口控制块指针
 * @param mem: (I) 连续的内存块，大小必须为@ref XWPCP_MEMPOOL_SIZE
 * @param memsize: (I) 连续的内存块大小，值必须为@ref XWPCP_MEMPOOL_SIZE
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval --ENOMEM: 内存池台小
 * @retval -EPERM: XWPCP未初始化
 * @note
 * + 参数**mem**作为xwpcp发送和接收缓冲区，用户可以
 *   使用```xwu8_t xwpcp_mem[XWPCP_MEMPOOL_SIZE];```定义，也可增加修饰符
 *   将这段内存定义在DMA区域，并对齐到L1CacheLine，以便提高整体效率；
 * + 参数**memsize**作用是提醒用户**mem**的大小必须为@ref XWPCP_MEMPOOL_SIZE，
 *   API内部会做检查。
 * @note
 * + 同步/异步：同步
 * + 上下文：线程
 * + 重入性：不可重入
 */
__xwmd_api
xwer_t xwpcp_start(struct xwpcp * xwpcp, const char * name,
                   const struct xwpcp_hwifal_operation * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize)
{
        xwer_t rc;
        xwssq_t i, j;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((hwifops), "nullptr", -EFAULT);
        XWPCP_VALIDATE((hwifops->tx), "nullptr", -EFAULT);
        XWPCP_VALIDATE((hwifops->rx), "nullptr", -EFAULT);

        XWPCP_VALIDATE_FORCE((mem), "nullptr", -EFAULT);
        XWPCP_VALIDATE_FORCE((XWPCP_MEMPOOL_SIZE == memsize),
                             "memsize-error",
                             -ESIZE);

        xwpcplogf(DEBUG, "Starting XWPCP-%s ...\n", XWPCP_VERSION);

        xwos_object_construct(&xwpcp->xwobj);
        xwpcp->hwifst = XWPCP_HWIFST_CLOSED;
        xwpcp->hwifcb = NULL;
        xwpcp->name = name;
        xwpcp->hwifops = hwifops;
        xwpcp->txthd = NULL;
        xwpcp->rxthd = NULL;
        xwpcp->mempool = NULL;
        xwos_object_activate(&xwpcp->xwobj, xwpcp_gc);

        /* 创建内存池 */
        xwpcp->mempool = (struct xwmm_bma *)&xwpcp->mempool_bma_raw;
        rc = xwmm_bma_init(xwpcp->mempool, xwpcp_mempool_name,
                           (xwptr_t)mem, memsize,
                           XWPCP_MEMBLK_SIZE, XWPCP_MEMBLK_ODR);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Init bma ... [rc:%d]\n", rc);
                goto err_bma_init;
        }
        xwpcplogf(DEBUG, "Create BMA ... [OK]\n");

        /* 初始化发送状态机 */
        xwpcp->txq.cnt = 0;
        for (i = 0; i < (xwssq_t)XWPCP_MEMBLK_NUM; i++) {
                xwlib_bclst_init_node(&xwpcp->txq.car[i].node);
                xwpcp->txq.car[i].state = XWPCP_CRS_IDLE;
                xwpcp->txq.car[i].idx = i;
                xwpcp->txq.car[i].pri = XWPCP_INVALID_PRI;
                xwpcp->txq.car[i].slot = NULL;
        }
        memset(&xwpcp->txq.carbmp, 0, sizeof(xwpcp->txq.carbmp));
        for (i = 0; i < (xwssq_t)XWPCP_PRI_NUM; i++) {
                xwlib_bclst_init_head(&xwpcp->txq.q[i]);
        }
        memset(&xwpcp->txq.qnebmp, 0, sizeof(xwpcp->txq.qnebmp));
        xwos_splk_init(&xwpcp->txq.qlock);
        rc = xwos_sem_init(&xwpcp->txq.qsem, 0, XWPCP_MEMBLK_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Init TXQ semaphore ... [rc:%d]\n", rc);
                goto err_txqsem_init;
        }
        rc = xwos_mtx_init(&xwpcp->txq.csmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Init xwpcp->csmtx ... [rc:%d]\n", rc);
                goto err_csmtx_init;
        }
        rc = xwos_cond_init(&xwpcp->txq.cscond);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Init xwpcp->cscond ... [rc:%d]\n", rc);
                goto err_cscond_init;
        }
        xwpcp->txq.remote.ack = 0;
        xwpcp->txq.remote.id = 0;
        xwpcp->txq.tmp = NULL;
        xwos_splk_init(&xwpcp->txq.notiflk);

        /* 初始化接收状态机 */
        xwpcp->rxq.cnt = 0;
        for (i = 0; i < (xwssq_t)XWPCP_PORT_NUM; i++) {
                xwlib_bclst_init_head(&xwpcp->rxq.q[i]);
                xwos_splk_init(&xwpcp->rxq.lock[i]);
                rc = xwos_sem_init(&xwpcp->rxq.sem[i], 0, XWPCP_MEMBLK_NUM);
                if (__xwcc_unlikely(rc < 0)) {
                        xwpcplogf(ERR, "Init RXQ semaphore[%d] ... [rc:%d]\n",
                                  i, rc);
                        goto err_rxqsem_init;
                }
        }

        /* 打开硬件接口 */
        rc = xwpcp_hwifal_open(xwpcp, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open;
        }

        /* 创建线程 */
        rc = xwos_thd_create(&xwpcp->rxthd,
                             xwpcp_rxthd_desc.name,
                             xwpcp_rxthd_desc.func,
                             xwpcp,
                             xwpcp_rxthd_desc.stack_size,
                             xwpcp_rxthd_desc.prio,
                             xwpcp_rxthd_desc.attr);
        if (rc < 0) {
                goto err_rxthd_create;
        }

        rc = xwos_thd_create(&xwpcp->txthd,
                             xwpcp_txthd_desc.name,
                             xwpcp_txthd_desc.func,
                             xwpcp,
                             xwpcp_txthd_desc.stack_size,
                             xwpcp_txthd_desc.prio,
                             xwpcp_txthd_desc.attr);
        if (rc < 0) {
                goto err_txthd_create;
        }

        return XWOK;

err_txthd_create:
        xwos_thd_cancel(xwpcp->rxthd);
        xwos_thd_detach(xwpcp->rxthd);
        xwpcp->rxthd = NULL;
err_rxthd_create:
        xwpcp_hwifal_close(xwpcp);
err_hwifal_open:
err_rxqsem_init:
        for (j = i - 1; j >= 0; j--) {
                xwos_sem_destroy(&xwpcp->rxq.sem[j]);
        }
        xwos_cond_destroy(&xwpcp->txq.cscond);
err_cscond_init:
        xwos_mtx_destroy(&xwpcp->txq.csmtx);
err_csmtx_init:
        xwos_sem_destroy(&xwpcp->txq.qsem);
err_txqsem_init:
err_bma_init:
        xwos_object_rawput(&xwpcp->xwobj);
        return rc;
}

/**
 * @brief XWPCP API: 停止XWPCP
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwpcp_stop(struct xwpcp * xwpcp)
{
        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);

        return xwpcp_put(xwpcp);
}

/**
 * @brief XWPCP的垃圾回收函数
 * @param obj: (I) XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_gc(void * obj)
{
        struct xwpcp * xwpcp;
        struct xwpcp_carrier * car;
        union xwpcp_slot * slot;
        xwer_t rc, childrc;
        xwssq_t j;

        xwpcp = obj;
        if (xwpcp->txthd) {
                rc = xwos_thd_stop(xwpcp->txthd, &childrc);
                if (XWOK == rc) {
                        xwpcp->txthd = NULL;
                        xwpcplogf(INFO, "Stop XWPCP TX thread... [OK]\n");
                }
        }

        if (xwpcp->rxthd) {
                rc = xwos_thd_stop(xwpcp->rxthd, &childrc);
                if (XWOK == rc) {
                        xwpcp->rxthd = NULL;
                        xwpcplogf(INFO, "Stop XWPCP RX thread... [OK]\n");
                }
        }

        xwpcp_hwifal_close(xwpcp);

        /* 释放TXQ中的剩余帧 */
        do {
                rc = xwos_sem_trywait(&xwpcp->txq.qsem);
                if (rc < 0) {
                        break;
                }
                car = xwpcp_txq_choose(xwpcp);
                xwmm_bma_free(xwpcp->mempool, car->slot);
                car->slot = NULL;
                xwpcp_txq_carrier_free(xwpcp, car);
        } while (true);

        /* 释放RXQ中的剩余帧 */
        for (j = XWPCP_PORT_NUM - 1; j >= 0; j--) {
                do {
                        rc = xwos_sem_trywait(&xwpcp->rxq.sem[j]);
                        if (rc < 0) {
                                break;
                        }
                        slot = xwpcp_rxq_choose(xwpcp, (xwu8_t)j);
                        xwmm_bma_free(xwpcp->mempool, slot);
                } while (true);
        }

        for (j = XWPCP_PORT_NUM - 1; j >= 0; j--) {
                xwos_sem_destroy(&xwpcp->rxq.sem[j]);
        }
        xwos_cond_destroy(&xwpcp->txq.cscond);
        xwos_mtx_destroy(&xwpcp->txq.csmtx);
        xwos_sem_destroy(&xwpcp->txq.qsem);
        return XWOK;
}

/**
 * @brief 用于实现@ref xwpcp_tx()的回调函数参数结构体
 */
struct xwpcp_txcb_arg {
        struct xwos_splk splk;
        struct xwos_cond cond;
        xwer_t rc;
};

/**
 * @brief 用于实现@ref xwpcp_tx()的通知发送结果的回调函数
 * @param xwpcp: (I) XWPCP对象的指针
 * @param txh: (I) 发送句柄
 * @param rc: (I) 发送结果
 * @param arg: (I) 回调函数的参数（此处用于传递struct xwpcp_txcb_arg的指针）
 */
static __xwmd_code
void xwpcp_txcb_notify(struct xwpcp * xwpcp, xwpcp_txh_t txh, xwer_t rc, void * arg)
{
        struct xwpcp_carrier * car;
        struct xwpcp_txcb_arg * cbarg;

        XWOS_UNUSED(xwpcp);
        car = txh;
        xwpcplogf(DEBUG, "txh:%p, rc:%d\n", txh, rc);
        if (XWPCP_CRS_FINISH == car->state) {
                cbarg = arg;
                xwos_splk_lock(&cbarg->splk);
                cbarg->rc = rc;
                xwos_splk_unlock(&cbarg->splk);
                xwos_cond_unicast(&cbarg->cond);
        }
}

/**
 * @brief XWPCP API: 在限定的时间内，将一条用户数据加入到XWPCP的发送队列中，
 *                   并等待发送结果
 * @param xwpcp: (I) XWPCP对象的指针
 * @param data: (I) 数据缓冲区的指针
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示数据长度
 *              (O) 作为输出时，返回实际发送的数据长度
 * @param pri: (I) 用户数据的优先级
 * @param port: (I) 端口
 * @param qos: (I) 服务质量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWPCP未启动
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_tx(struct xwpcp * xwpcp,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwtm_t * xwtm)
{
        struct xwpcp_txcb_arg cbarg;
        union xwos_ulock ulk;
        xwpcp_txh_t txh;
        xwsq_t lkst;
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((data), "nullptr", -EFAULT);
        XWPCP_VALIDATE((size), "nullptr", -EFAULT);
        XWPCP_VALIDATE((*size <= XWPCP_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWPCP_VALIDATE((pri < XWPCP_PRI_NUM), "pri-invalid", -E2BIG);
        XWPCP_VALIDATE((port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);
        XWPCP_VALIDATE((qos < XWPCP_MSG_QOS_NUM), "qos-invalid", -EINVAL);
        XWPCP_VALIDATE((xwtm), "nullptr", -EFAULT);

        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        xwos_splk_init(&cbarg.splk);
        xwos_cond_init(&cbarg.cond);
        cbarg.rc = -EINPROGRESS;

        rc = xwpcp_eq_msg(xwpcp, data, *size, pri, port, qos,
                          xwpcp_txcb_notify, &cbarg, &txh);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwpcp_eq;
        }

        ulk.osal.splk = &cbarg.splk;
        xwos_splk_lock(&cbarg.splk);
        if (-EINPROGRESS == cbarg.rc) {
                rc = xwos_cond_timedwait(&cbarg.cond, ulk, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOK == rc) {
                        rc = cbarg.rc;
                        xwos_splk_unlock(&cbarg.splk);
                } else {
                        if (XWOS_LKST_LOCKED == lkst) {
                                xwos_splk_unlock(&cbarg.splk);
                        }/* else {} */
                        xwos_splk_lock(&xwpcp->txq.notiflk);
                        xwos_splk_lock(&cbarg.splk);
                        if (-EINPROGRESS == cbarg.rc) {
                                txh->slot->tx.ntfcb = NULL;
                                txh->slot->tx.cbarg = NULL;
                        } else {
                                rc = cbarg.rc;
                        }
                        xwos_splk_unlock(&cbarg.splk);
                        xwos_splk_unlock(&xwpcp->txq.notiflk);
                }
        } else {
                rc = cbarg.rc;
                xwos_splk_unlock(&cbarg.splk);
        }
        xwos_cond_destroy(&cbarg.cond);
        xwpcp_put(xwpcp);
        return rc;

err_xwpcp_eq:
        xwos_cond_destroy(&cbarg.cond);
        xwpcp_put(xwpcp);
err_ifnotrdy:
        return rc;
}

/**
 * @brief XWPCP API: 将一条用户数据加入到XWPCP的发送队列中
 * @param xwpcp: (I) XWPCP对象的指针
 * @param data: (I) 数据缓冲区的指针
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示数据长度
 *              (O) 作为输出时，返回实际入队的数据长度
 * @param pri: (I) 优先级
 * @param port: (I) 端口
 * @param qos: (I) 服务质量
 * @param ntfcb: (I) 通知发送结果的回调函数
 * @param cbarg: (I) 调用回调函数时的用户数据
 * @param txhbuf: (O) 指向缓冲区的指针，通过此缓冲区返回发送句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWPCP未启动
 * @note
 * - 此函数将用户数据放如发送队列就返回。当用户数据被XWPCP的发送线程成功发送
 *   （接收到远程端应答），注册的回调函数会被调用；
 * - 回调函数在XWPCP的发送线程的线程上下文中执行，如果在此函数中使用了会
 *   长时间阻塞线程的函数，会导致XWPCP停止发送。
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_eq(struct xwpcp * xwpcp,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwpcp_ntf_f ntfcb, void * cbarg,
                xwpcp_txh_t * txhbuf)
{
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((data), "nullptr", -EFAULT);
        XWPCP_VALIDATE((size), "nullptr", -EFAULT);
        XWPCP_VALIDATE((*size <= XWPCP_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWPCP_VALIDATE((pri < XWPCP_PRI_NUM), "pri-invalid", -E2BIG);
        XWPCP_VALIDATE((port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);
        XWPCP_VALIDATE((qos < XWPCP_MSG_QOS_NUM), "qos-invalid", -EINVAL);

        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }
        rc = xwpcp_eq_msg(xwpcp, data, *size, pri, port, qos, ntfcb, cbarg, txhbuf);
        xwpcp_put(xwpcp);

err_ifnotrdy:
        return rc;
}

/**
 * @brief XWPCP API: 中断发送
 * @param xwpcp: (I) XWPCP对象的指针
 * @param txh: (I) 发送句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EACCES: 消息帧已经正在发送
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwpcp_abort(struct xwpcp * xwpcp, xwpcp_txh_t txh)
{
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((txh), "nullptr", -EFAULT);

        XWOS_UNUSED(xwpcp);

        rc = xwaop_teq_then_write(xwu32, &txh->state,
                                  XWPCP_CRS_READY,
                                  XWPCP_CRS_ABORT,
                                  NULL);
        return rc;
}

/**
 * @brief XWPCP API: 获取发送状态
 * @param txh: (I) 发送句柄
 * @return 发送状态，取值：@ref xwpcp_carrier_state_em
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwsq_t xwpcp_get_txstate(xwpcp_txh_t txh)
{
        return txh->state;
}

/**
 * @brief XWPCP API: 接收消息，若接收队列为空，就限时等待
 * @param xwpcp: (I) XWPCP对象的指针
 * @param port: (I) 接收消息的端口
 * @param rxbuf: (O) 指向缓冲区的指针，此缓冲区用于接收消息
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示接收缓冲区的大小
 *              (O) 作为输出时，返回实际接收的消息大小
 * @param qos: (O) 指向缓冲区的指针，此缓冲区用于返回消息的QoS，可为NULL表示不关心QoS
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -EPERM: XWPCP未启动
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_rx(struct xwpcp * xwpcp, xwu8_t port,
                xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos,
                xwtm_t * xwtm)
{
        union xwpcp_slot * slot;
        xwu8_t * sdupos;
        xwsz_t bufsize;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((rxbuf), "nullptr", -EFAULT);
        XWPCP_VALIDATE((size), "nullptr", -EFAULT);
        XWPCP_VALIDATE((port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);
        XWPCP_VALIDATE((xwtm), "nullptr", -EFAULT);

        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        bufsize = *size;
        xwpcplogf(DEBUG, "port:%d, buffer size:0x%X\n", port, bufsize);
        rc = xwos_sem_timedwait(&xwpcp->rxq.sem[port], xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_timedwait;
        }

        slot = xwpcp_rxq_choose(xwpcp, port);
        sdupos = XWPCP_SDUPOS(&slot->rx.frm.head);
        sdusize = XWPCP_RXSDUSIZE(&slot->rx);

        if (bufsize < sdusize) {
                realsize = bufsize;
                xwpcplogf(WARNING,
                          "buffer size is too small(0x%X)!, expected size is 0x%X\n",
                          bufsize, sdusize);
        } else {
                realsize = sdusize;
        }
        memcpy(rxbuf, sdupos, realsize);
        if (qos) {
                *qos = slot->rx.frm.head.qos;
        }
        *size = realsize;

        xwmm_bma_free(xwpcp->mempool, slot);
        xwpcp_put(xwpcp);
        return XWOK;

err_sem_timedwait:
        xwpcp_put(xwpcp);
err_ifnotrdy:
        return rc;
}

/**
 * @brief XWPCP API: 尝试接收消息，若接收队列为空，立即返回错误码
 * @param xwpcp: (I) XWPCP对象的指针
 * @param port: (I) 接收消息的端口
 * @param rxbuf: (O) 指向缓冲区的指针，此缓冲区用于接收消息
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示接收缓冲区的大小
 *              (O) 作为输出时，返回实际接收的消息大小
 * @param qos: (O) 指向缓冲区的指针，此缓冲区用于返回消息的QoS，可为NULL表示不关心QoS
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENXIO: 端口0不允许发送用户数据
 * @retval -ENODEV: 端口号超出范围
 * @retval -ENODATA: 接收队列为空
 * @retval -EPERM: XWPCP未启动
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwpcp_try_rx(struct xwpcp * xwpcp, xwu8_t port,
                    xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos)
{
        union xwpcp_slot * slot;
        xwu8_t * sdupos;
        xwsz_t bufsize;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((rxbuf), "nullptr", -EFAULT);
        XWPCP_VALIDATE((size), "nullptr", -EFAULT);
        XWPCP_VALIDATE((port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);

        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        bufsize = *size;
        xwpcplogf(DEBUG, "port:%d, buffer size:0x%X\n", port, bufsize);
        rc = xwos_sem_trywait(&xwpcp->rxq.sem[port]);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_trywait;
        }

        slot = xwpcp_rxq_choose(xwpcp, port);
        sdupos = XWPCP_SDUPOS(&slot->rx.frm.head);
        sdusize = XWPCP_RXSDUSIZE(&slot->rx);

        if (bufsize < sdusize) {
                realsize = bufsize;
                xwpcplogf(WARNING,
                          "buffer size is too small(0x%X)!, expected size is 0x%X\n",
                          bufsize, sdusize);
        } else {
                realsize = sdusize;
        }
        memcpy(rxbuf, sdupos, realsize);
        if (qos) {
                *qos = slot->rx.frm.head.qos;
        }
        *size = realsize;

        xwmm_bma_free(xwpcp->mempool, slot);
        xwpcp_put(xwpcp);
        return XWOK;

err_sem_trywait:
        xwpcp_put(xwpcp);
err_ifnotrdy:
        return rc;
}
