/**
 * @file
 * @brief SOC间点对点通讯协议：应用接口层
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwmd/isc/xwssc/hwifal.h>
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mif.h>

#define XWSSC_TXTHD_PRIORITY XWMDCFG_isc_xwssc_TXTHD_PRIORITY
#define XWSSC_RXTHD_PRIORITY XWMDCFG_isc_xwssc_RXTHD_PRIORITY

static __xwmd_code
void xwssc_txcb_notify(struct xwssc * xwssc, xwssc_txh_t txh, xwer_t rc, void * arg);

static __xwmd_code
xwer_t xwssc_gc(void * obj);

/**
 * @brief XWSSC API: 启动XWSSC
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] name: XWSSC实例的名字
 * @param[in] hwifops: 硬件接口抽象层操作函数集合
 * @param[in] hwifcb: 硬件接口控制块指针
 * @param[in] mem: 连续的内存块
 * @param[in] memsize: 连续的内存块大小，值必须为 @ref XWSSC_MEMPOOL_SIZE
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存池台小
 * @retval -EPERM: XWSSC未初始化
 * @note
 * + 参数 `mem` 作为xwssc发送和接收缓冲区，用户可以使用 @ref XWSSC_DEF_MEMPOOL 定义，
 *   `XWSSC_DEF_MEMPOOL` 增加修饰符 `__xwcc_alignl1cache` ，可提高内存的访问的效率。
 * + 参数 `memsize` 作用是提醒用户 `mem` 的大小必须为 @ref XWSSC_MEMPOOL_SIZE ，
 *   API内部也会做检查。
 * @note
 * + 同步/异步：同步
 * + 上下文：线程
 * + 重入性：不可重入
 */
__xwmd_api
xwer_t xwssc_start(struct xwssc * xwssc, const char * name,
                   const struct xwssc_hwifal_operation * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize)
{
        struct xwos_thd_attr attr;
        xwer_t rc;
        xwssq_t i, j;

        XWSSC_VALIDATE((xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((hwifops), "nullptr", -EFAULT);
        XWSSC_VALIDATE((hwifops->tx), "nullptr", -EFAULT);
        XWSSC_VALIDATE((hwifops->rx), "nullptr", -EFAULT);

        XWSSC_VALIDATE_FORCE((mem), "nullptr", -EFAULT);
        XWSSC_VALIDATE_FORCE((XWSSC_MEMPOOL_SIZE == memsize),
                             "memsize-error",
                             -ESIZE);

        xwssclogf(DEBUG, "[API] Starting XWSSC-%s ...\n", XWSSC_VERSION);

        xwos_object_construct(&xwssc->xwobj);
        xwssc->hwifst = XWSSC_HWIFST_CLOSED;
        xwssc->hwifcb = NULL;
        xwssc->name = name;
        xwssc->hwifops = hwifops;
        xwssc->txthd = XWOS_THD_NILD;
        xwssc->rxthd = XWOS_THD_NILD;
        xwssc->mempool = NULL;
        xwos_object_activate(&xwssc->xwobj, xwssc_gc);

        /* 创建内存池 */
        xwssc->mempool = (struct xwmm_bma *)&xwssc->mempool_bma_raw;
        rc = xwmm_bma_init(xwssc->mempool, "xwssc.mempool",
                           (xwptr_t)mem, memsize,
                           XWSSC_MEMBLK_SIZE, XWSSC_MEMBLK_ODR);
        if (__xwcc_unlikely(rc < 0)) {
                xwssclogf(ERR, "[API] Init BMA ... <rc:%d>\n", rc);
                goto err_bma_init;
        }
        xwssclogf(DEBUG, "[API] Create BMA ... [OK]\n");

        /* 初始化发送状态机 */
        xwssc->txq.cnt = 0;
        for (i = 0; i < (xwssq_t)XWSSC_MEMBLK_NUM; i++) {
                xwlib_bclst_init_node(&xwssc->txq.car[i].node);
                xwssc->txq.car[i].state = XWSSC_CRS_IDLE;
                xwssc->txq.car[i].idx = i;
                xwssc->txq.car[i].pri = XWSSC_INVALID_PRI;
                xwssc->txq.car[i].slot = NULL;
        }
        memset((void *)&xwssc->txq.carbmp, 0, sizeof(xwssc->txq.carbmp));
        for (i = 0; i < (xwssq_t)XWSSC_PRI_NUM; i++) {
                xwlib_bclst_init_head(&xwssc->txq.q[i]);
        }
        memset(&xwssc->txq.qnebmp, 0, sizeof(xwssc->txq.qnebmp));
        xwos_splk_init(&xwssc->txq.qlock);
        rc = xwos_sem_init(&xwssc->txq.qsem, 0, XWSSC_MEMBLK_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwssclogf(ERR, "[API] Init TXQ semaphore ... <rc:%d>\n", rc);
                goto err_txqsem_init;
        }
        rc = xwos_mtx_init(&xwssc->txq.csmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwssclogf(ERR, "[API] Init xwssc->csmtx ... <rc:%d>\n", rc);
                goto err_csmtx_init;
        }
        rc = xwos_cond_init(&xwssc->txq.cscond);
        if (__xwcc_unlikely(rc < 0)) {
                xwssclogf(ERR, "[API] Init xwssc->cscond ... <rc:%d>\n", rc);
                goto err_cscond_init;
        }
        xwssc->txq.remote.ack = 0;
        xwssc->txq.remote.id = 0;
        xwssc->txq.tmp = NULL;
        xwos_splk_init(&xwssc->txq.notiflk);

        /* 初始化接收状态机 */
        xwssc->rxq.cnt = 0;
        for (i = 0; i < (xwssq_t)XWSSC_PORT_NUM; i++) {
                xwlib_bclst_init_head(&xwssc->rxq.q[i]);
                xwos_splk_init(&xwssc->rxq.lock[i]);
                rc = xwos_sem_init(&xwssc->rxq.sem[i], 0, XWSSC_MEMBLK_NUM);
                if (__xwcc_unlikely(rc < 0)) {
                        xwssclogf(ERR, "[API] Init RXQ semaphore[%d] ... <rc:%d>\n",
                                  i, rc);
                        goto err_rxqsem_init;
                }
        }

        /* 打开硬件接口 */
        rc = xwssc_hwifal_open(xwssc, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open;
        }

        /* 创建线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "xwmd.isc.xwssc.rxthd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = XWSSC_RXTHD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&xwssc->rxthd, &attr, (xwos_thd_f)xwssc_rxthd, xwssc);
        if (rc < 0) {
                goto err_rxthd_create;
        }

        xwos_thd_attr_init(&attr);
        attr.name = "xwmd.isc.xwssc.txthd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = XWSSC_TXTHD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&xwssc->txthd, &attr, (xwos_thd_f)xwssc_txthd, xwssc);
        if (rc < 0) {
                goto err_txthd_create;
        }

        return XWOK;

err_txthd_create:
        xwos_thd_detach(xwssc->rxthd);
        xwos_thd_quit(xwssc->rxthd);
        xwssc->rxthd = XWOS_THD_NILD;
err_rxthd_create:
        xwssc_hwifal_close(xwssc);
err_hwifal_open:
err_rxqsem_init:
        for (j = i - 1; j >= 0; j--) {
                xwos_sem_fini(&xwssc->rxq.sem[j]);
        }
        xwos_cond_fini(&xwssc->txq.cscond);
err_cscond_init:
        xwos_mtx_fini(&xwssc->txq.csmtx);
err_csmtx_init:
        xwos_sem_fini(&xwssc->txq.qsem);
err_txqsem_init:
err_bma_init:
        xwos_object_rawput(&xwssc->xwobj);
        return rc;
}

/**
 * @brief XWSSC API: 停止XWSSC
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwssc_stop(struct xwssc * xwssc)
{
        XWSSC_VALIDATE((xwssc), "nullptr", -EFAULT);

        return xwssc_put(xwssc);
}

/**
 * @brief XWSSC的垃圾回收函数
 * @param[in] obj: XWSSC对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_gc(void * obj)
{
        struct xwssc * xwssc;
        struct xwssc_carrier * car;
        union xwssc_slot * slot;
        xwer_t rc, childrc;
        xwssq_t j;

        xwssc = obj;
        if (xwssc->txthd.thd) {
                rc = xwos_thd_stop(xwssc->txthd, &childrc);
                if (XWOK == rc) {
                        xwssc->txthd = XWOS_THD_NILD;
                        xwssclogf(INFO, "[API] Stop XWSSC TX thread... [OK]\n");
                }
        }

        if (xwssc->rxthd.thd) {
                rc = xwos_thd_stop(xwssc->rxthd, &childrc);
                if (XWOK == rc) {
                        xwssc->rxthd = XWOS_THD_NILD;
                        xwssclogf(INFO, "[API] Stop XWSSC RX thread... [OK]\n");
                }
        }

        xwssc_hwifal_close(xwssc);

        /* 释放TXQ中的剩余帧 */
        do {
                rc = xwos_sem_trywait(&xwssc->txq.qsem);
                if (rc < 0) {
                        break;
                }
                car = xwssc_txq_choose(xwssc);
                xwmm_bma_free(xwssc->mempool, car->slot);
                car->slot = NULL;
                xwssc_txq_carrier_free(xwssc, car);
        } while (true);

        /* 释放RXQ中的剩余帧 */
        for (j = XWSSC_PORT_NUM - 1; j >= 0; j--) {
                do {
                        rc = xwos_sem_trywait(&xwssc->rxq.sem[j]);
                        if (rc < 0) {
                                break;
                        }
                        slot = xwssc_rxq_choose(xwssc, (xwu8_t)j);
                        xwmm_bma_free(xwssc->mempool, slot);
                } while (true);
        }

        for (j = XWSSC_PORT_NUM - 1; j >= 0; j--) {
                xwos_sem_fini(&xwssc->rxq.sem[j]);
        }
        xwos_cond_fini(&xwssc->txq.cscond);
        xwos_mtx_fini(&xwssc->txq.csmtx);
        xwos_sem_fini(&xwssc->txq.qsem);
        return XWOK;
}

/**
 * @brief 用于实现 @ref xwssc_tx() 的回调函数参数结构体
 * @details
 * 此结构体被锁 `xwssc->txq.notiflk` 保护。上锁顺序：
 * ① `xwssc->txq.notiflk`
 * ② `xwssc_txcb_arg.splk`
 */
struct xwssc_txcb_arg {
        struct xwos_splk splk;
        struct xwos_cond cond;
        xwer_t rc; /**< 发送结果，被锁 `splk` 保护 */
};

/**
 * @brief 用于实现 @ref xwssc_tx() 的通知发送结果的回调函数
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] txh: 发送句柄
 * @param[in] rc: 发送结果
 * @param[in] arg: 回调函数的参数（此处用于传递struct xwssc_txcb_arg的指针）
 */
static __xwmd_code
void xwssc_txcb_notify(struct xwssc * xwssc, xwssc_txh_t txh, xwer_t rc, void * arg)
{
        struct xwssc_carrier * car;
        struct xwssc_txcb_arg * cbarg;

        XWOS_UNUSED(xwssc);
        car = txh;
        xwssclogf(DEBUG, "[API] txh:%p, rc:%d\n", txh, rc);
        if (XWSSC_CRS_FINISH == car->state) {
                cbarg = arg;
                xwos_splk_lock(&cbarg->splk);
                cbarg->rc = rc;
                xwos_splk_unlock(&cbarg->splk);
                xwos_cond_unicast(&cbarg->cond);
        }
}

/**
 * @brief XWSSC API: 在限定的时间内，将一条用户数据加入到XWSSC的发送队列中，
 *                   并等待发送结果
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] data: 数据缓冲区的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回实际发送的数据长度
 * @param[in] pri: 用户数据的优先级
 * @param[in] port: 端口
 * @param[in] qos: 服务质量，取值范围： @ref xwssc_msg_qos_em
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWSSC未启动
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwmd_api
xwer_t xwssc_tx(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwtm_t to)
{
        struct xwssc_txcb_arg cbarg;
        union xwos_ulock ulk;
        xwssc_txh_t txh;
        xwsq_t lkst;
        xwer_t rc;

        XWSSC_VALIDATE((xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((data), "nullptr", -EFAULT);
        XWSSC_VALIDATE((size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((*size <= XWSSC_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWSSC_VALIDATE((pri < XWSSC_PRI_NUM), "pri-invalid", -E2BIG);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);
        XWSSC_VALIDATE((qos < XWSSC_MSG_QOS_NUM), "qos-invalid", -EINVAL);

        rc = xwssc_grab(xwssc);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        xwos_splk_init(&cbarg.splk);
        xwos_cond_init(&cbarg.cond);
        cbarg.rc = -EINPROGRESS;

        rc = xwssc_eq_msg(xwssc, data, *size, pri, port, qos,
                          xwssc_txcb_notify, &cbarg, &txh);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwssc_eq;
        }

        ulk.osal.splk = &cbarg.splk;
        xwos_splk_lock(&cbarg.splk);
        if (-EINPROGRESS == cbarg.rc) {
                rc = xwos_cond_wait_to(&cbarg.cond, ulk, XWOS_LK_SPLK, NULL, to, &lkst);
                if (XWOK == rc) {
                        rc = cbarg.rc;
                        xwos_splk_unlock(&cbarg.splk);
                } else {
                        if (XWOS_LKST_LOCKED == lkst) {
                                xwos_splk_unlock(&cbarg.splk);
                        }/* else {} */
                        xwos_splk_lock(&xwssc->txq.notiflk);
                        xwos_splk_lock(&cbarg.splk);
                        if (-EINPROGRESS == cbarg.rc) {
                                txh->slot->tx.ntfcb = NULL;
                                txh->slot->tx.cbarg = NULL;
                        } else {
                                rc = cbarg.rc;
                        }
                        xwos_splk_unlock(&cbarg.splk);
                        xwos_splk_unlock(&xwssc->txq.notiflk);
                }
        } else {
                rc = cbarg.rc;
                xwos_splk_unlock(&cbarg.splk);
        }
        xwos_cond_fini(&cbarg.cond);
        xwssc_put(xwssc);
        return rc;

err_xwssc_eq:
        xwos_cond_fini(&cbarg.cond);
        xwssc_put(xwssc);
err_ifnotrdy:
        return rc;
}

/**
 * @brief XWSSC API: 将一条用户数据加入到XWSSC的发送队列中
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] data: 数据缓冲区的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据长度
 * + (O) 作为输出时，返回实际入队的数据长度
 * @param[in] pri: 优先级
 * @param[in] port: 端口
 * @param[in] qos: 服务质量
 * @param[in] ntfcb: 通知发送结果的回调函数
 * @param[in] cbarg: 调用回调函数时的用户数据
 * @param[out] txhbuf: 指向缓冲区的指针，通过此缓冲区返回发送句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENODEV: 端口号超出范围
 * @retval -EINVAL: qos错误
 * @retval -ENOBUFS: 帧槽被使用完
 * @retval -EPERM: XWSSC未启动
 * @note
 * - 此函数将用户数据放如发送队列就返回。当用户数据被XWSSC的发送线程成功发送
 *   （接收到远程端应答），注册的回调函数会被调用；
 * - 回调函数在XWSSC的发送线程的线程上下文中执行，如果在此函数中使用了会
 *   长时间阻塞线程的函数，会导致XWSSC停止发送。
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwssc_eq(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwssc_ntf_f ntfcb, void * cbarg,
                xwssc_txh_t * txhbuf)
{
        xwer_t rc;

        XWSSC_VALIDATE((xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((data), "nullptr", -EFAULT);
        XWSSC_VALIDATE((size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((*size <= XWSSC_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWSSC_VALIDATE((pri < XWSSC_PRI_NUM), "pri-invalid", -E2BIG);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);
        XWSSC_VALIDATE((qos < XWSSC_MSG_QOS_NUM), "qos-invalid", -EINVAL);

        rc = xwssc_grab(xwssc);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }
        rc = xwssc_eq_msg(xwssc, data, *size, pri, port, qos, ntfcb, cbarg, txhbuf);
        xwssc_put(xwssc);

err_ifnotrdy:
        return rc;
}

/**
 * @brief XWSSC API: 中断发送
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] txh: 发送句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EACCES: 消息帧已经正在发送
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @details
 * 如果消息已经被 XWSSC TX 线程选中，发送不可被中断；
 * 仅当消息还在就绪队列中，未被选中发送时才可中断；
 */
__xwmd_api
xwer_t xwssc_abort(struct xwssc * xwssc, xwssc_txh_t txh)
{
        xwer_t rc;

        XWSSC_VALIDATE((xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((txh), "nullptr", -EFAULT);

        XWOS_UNUSED(xwssc);

        rc = xwaop_teq_then_write(xwu32_t, &txh->state,
                                  XWSSC_CRS_READY,
                                  XWSSC_CRS_ABORT,
                                  NULL);
        return rc;
}

/**
 * @brief XWSSC API: 获取发送状态
 * @param[in] txh: 发送句柄
 * @return 发送状态，取值： @ref xwssc_carrier_state_em
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwsq_t xwssc_get_txstate(xwssc_txh_t txh)
{
        return txh->state;
}

/**
 * @brief XWSSC API: 接收消息，若接收队列为空，就限时等待
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] port: 接收消息的端口
 * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的消息大小
 * @param[out] qos: 指向缓冲区的指针，此缓冲区用于返回消息的QoS，可为NULL表示不关心QoS
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODEV: 端口号超出范围
 * @retval -EPERM: XWSSC未启动
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwmd_api
xwer_t xwssc_rx(struct xwssc * xwssc, xwu8_t port,
                xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos,
                xwtm_t to)
{
        union xwssc_slot * slot;
        xwu8_t * sdupos;
        xwsz_t bufsize;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWSSC_VALIDATE((xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((rxbuf), "nullptr", -EFAULT);
        XWSSC_VALIDATE((size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);

        rc = xwssc_grab(xwssc);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        bufsize = *size;
        xwssclogf(DEBUG, "[API] port:%d, buffer size:0x%X\n", port, bufsize);
        rc = xwos_sem_wait_to(&xwssc->rxq.sem[port], to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_wait_to;
        }

        slot = xwssc_rxq_choose(xwssc, port);
        sdupos = XWSSC_SDUPOS(&slot->rx.frm.head);
        sdusize = XWSSC_RXSDUSIZE(&slot->rx);

        if (bufsize < sdusize) {
                realsize = bufsize;
                xwssclogf(WARNING,
                          "[API] Buffer is too small(0x%X)! Expected size is 0x%X\n",
                          bufsize, sdusize);
        } else {
                realsize = sdusize;
        }
        memcpy(rxbuf, sdupos, realsize);
        if (qos) {
                *qos = slot->rx.frm.head.qos;
        }
        *size = realsize;

        xwmm_bma_free(xwssc->mempool, slot);
        xwssc_put(xwssc);
        return XWOK;

err_sem_wait_to:
        xwssc_put(xwssc);
err_ifnotrdy:
        return rc;
}

/**
 * @brief XWSSC API: 尝试接收消息，若接收队列为空，立即返回错误码
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] port: 接收消息的端口
 * @param[out] rxbuf: 指向缓冲区的指针，此缓冲区用于接收消息
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的消息大小
 * @param[out] qos: 指向缓冲区的指针，此缓冲区用于返回消息的QoS，可为NULL表示不关心QoS
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODEV: 端口号超出范围
 * @retval -ENODATA: 接收队列为空
 * @retval -EPERM: XWSSC未启动
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwssc_try_rx(struct xwssc * xwssc, xwu8_t port,
                    xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos)
{
        union xwssc_slot * slot;
        xwu8_t * sdupos;
        xwsz_t bufsize;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWSSC_VALIDATE((xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((rxbuf), "nullptr", -EFAULT);
        XWSSC_VALIDATE((size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);

        rc = xwssc_grab(xwssc);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        bufsize = *size;
        xwssclogf(DEBUG, "[API] port:%d, buffer size:0x%X\n", port, bufsize);
        rc = xwos_sem_trywait(&xwssc->rxq.sem[port]);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_trywait;
        }

        slot = xwssc_rxq_choose(xwssc, port);
        sdupos = XWSSC_SDUPOS(&slot->rx.frm.head);
        sdusize = XWSSC_RXSDUSIZE(&slot->rx);

        if (bufsize < sdusize) {
                realsize = bufsize;
                xwssclogf(WARNING,
                          "[API] Buffer is too small(0x%X)! Expected size is 0x%X\n",
                          bufsize, sdusize);
        } else {
                realsize = sdusize;
        }
        memcpy(rxbuf, sdupos, realsize);
        if (qos) {
                *qos = slot->rx.frm.head.qos;
        }
        *size = realsize;

        xwmm_bma_free(xwssc->mempool, slot);
        xwssc_put(xwssc);
        return XWOK;

err_sem_trywait:
        xwssc_put(xwssc);
err_ifnotrdy:
        return rc;
}
