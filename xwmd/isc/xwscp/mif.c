/**
 * @file
 * @brief 精简的点对点通讯协议：应用接口层
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
#include <xwos/lib/xwaop.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/mm/bma.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/mif.h>

/**
 * @brief 接收线程的优先级
 */
#define XWSCP_THD_PRIORITY XWMDCFG_isc_xwscp_THD_PRIORITY

extern __xwmd_rodata const xwer_t xwscp_callback_rc[XWSCP_ACK_NUM];

static __xwmd_code
xwer_t xwscp_gc(void * obj);

static __xwmd_code
xwer_t xwscp_connect_once(struct xwscp * xwscp, xwtm_t to);

static __xwmd_code
xwer_t xwscp_tx_once(struct xwscp * xwscp,
                     const xwu8_t data[], xwsz_t * size, xwu8_t qos,
                     xwtm_t to);

/**
 * @brief XWSCP API: 启动XWSCP
 * @param[in] xwscp: XWSCP对象的指针
 * @param[in] name: XWSCP实例的名字
 * @param[in] hwifops: 硬件接口抽象层操作函数集合
 * @param[in] hwifcb: 硬件接口控制块指针
 * @param[in] mem: 连续的内存块
 * @param[in] memsize: 连续的内存块大小，值必须为 @ref XWSCP_MEMPOOL_SIZE
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval --ENOMEM: 内存池台小
 * @retval -EPERM: XWSCP未初始化
 * @note
 * + 参数```mem```作为xwscp发送和接收缓冲区，用户可以
 *   使用```xwu8_t xwscp_mem[XWSCP_MEMPOOL_SIZE];```定义，也可增加修饰符
 *   将这段内存定义在DMA区域，并对齐到L1CacheLine，以便提高整体效率；
 * + 参数```memsize```作用是提醒用户```mem```的大小必须为 @ref XWSCP_MEMPOOL_SIZE ，
 *   API内部会做检查。
 * @note
 * + 同步/异步：同步
 * + 上下文：线程
 * + 重入性：不可重入
 */
__xwmd_api
xwer_t xwscp_start(struct xwscp * xwscp, const char * name,
                   const struct xwscp_hwifal_operations * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((hwifops), "nullptr", -EFAULT);
        XWSCP_VALIDATE((hwifops->tx), "nullptr", -EFAULT);
        XWSCP_VALIDATE((hwifops->rx), "nullptr", -EFAULT);

        XWSCP_VALIDATE_FORCE((mem), "nullptr", -EFAULT);
        XWSCP_VALIDATE_FORCE((XWSCP_MEMPOOL_SIZE == memsize),
                             "memsize-error",
                             -ESIZE);

        xwscplogf(DEBUG, "Starting XWSCP-%s ...\n", XWSCP_VERSION);

        xwos_object_construct(&xwscp->xwobj);
        xwscp->hwifcb = NULL;
        xwscp->hwifst = XWSCP_HWIFST_CLOSED;
        xwscp->name = name;
        xwscp->hwifops = hwifops;
        xwscp->rx.thd = XWOS_THD_NILD;
        xwscp->mempool = NULL;
        xwos_object_activate(&xwscp->xwobj, xwscp_gc);

        /* 创建内存池 */
        xwscp->mempool = (struct xwmm_bma *)&xwscp->mempool_bma_raw;
        rc = xwmm_bma_init(xwscp->mempool, "xwscp.mempool",
                           (xwptr_t)mem, memsize,
                           XWSCP_MEMBLK_SIZE, XWSCP_MEMBLK_ODR);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "init bma ... [rc:%d]\n", rc);
                goto err_bma_init;
        }

        /* 初始化发送状态机 */
        xwscp->tx.cnt = 0;
        rc = xwos_mtx_init(&xwscp->tx.mtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init tx.mtx ... [rc:%d]\n", rc);
                goto err_txmtx_init;
        }
        rc = xwos_mtx_init(&xwscp->tx.csmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init csmtx ... [rc:%d]\n", rc);
                goto err_csmtx_init;
        }
        rc = xwos_cond_init(&xwscp->tx.cscond);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init ACK condition ... [rc:%d]\n", rc);
                goto err_cscond_init;
        }
        xwscp->tx.remote.ack = 0;
        xwscp->tx.remote.id = 0;

        /* 初始化接收状态机 */
        xwscp->rx.cnt = 0;
        xwlib_bclst_init_head(&xwscp->rx.head);
        xwos_splk_init(&xwscp->rx.lock);
        rc = xwos_sem_init(&xwscp->rx.sem, 0, XWSCP_MEMBLK_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init rx.sem ... [rc:%d]\n", rc);
                goto err_rxsem_init;
        }

        /* 打开端口 */
        rc = xwscp_hwifal_open(xwscp, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open;
        }

        /* 创建线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "xwmd.isc.xwscp.thd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = XWSCP_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&xwscp->rx.thd, &attr, (xwos_thd_f)xwscp_thd, xwscp);
        if (rc < 0) {
                goto err_rxthd_create;
        }

        return XWOK;

err_rxthd_create:
        xwscp_hwifal_close(xwscp);
err_hwifal_open:
        xwos_sem_fini(&xwscp->rx.sem);
err_rxsem_init:
        xwos_cond_fini(&xwscp->tx.cscond);
err_cscond_init:
        xwos_mtx_fini(&xwscp->tx.csmtx);
err_csmtx_init:
        xwos_mtx_fini(&xwscp->tx.mtx);
err_txmtx_init:
err_bma_init:
        xwos_object_rawput(&xwscp->xwobj);
        return rc;
}

/**
 * @brief XWSCP API: 停止XWSCP
 * @param[in] xwscp: XWSCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwscp_stop(struct xwscp * xwscp)
{
        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);

        return xwscp_put(xwscp);
}

/**
 * @brief XWSCP的垃圾回收函数
 * @param[in] obj: XWSCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_gc(void * obj)
{
        struct xwscp * xwscp;
        xwer_t rc, childrc;

        xwscp = obj;
        if (xwscp->rx.thd.thd) {
                rc = xwos_thd_stop(xwscp->rx.thd, &childrc);
                if (XWOK == rc) {
                        xwscp->rx.thd = XWOS_THD_NILD;
                        xwscplogf(INFO, "Stop XWSCP RX thread... [OK]\n");
                }
        }

        xwscp_hwifal_close(xwscp);
        xwos_sem_fini(&xwscp->rx.sem);
        xwos_cond_fini(&xwscp->tx.cscond);
        xwos_mtx_fini(&xwscp->tx.csmtx);
        xwos_mtx_fini(&xwscp->tx.mtx);
        return XWOK;
}

static __xwmd_code
xwer_t xwscp_connect_once(struct xwscp * xwscp, xwtm_t to)
{
        union xwos_ulock ulk;
        xwsq_t lkst;
        xwtm_t next;
        xwer_t rc;

        ulk.osal.mtx = &xwscp->tx.csmtx;
        rc = xwos_mtx_lock_to(&xwscp->tx.csmtx, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock_to;
        }
        xwaop_c0m(xwsq_t, &xwscp->hwifst, XWSCP_HWIFST_CONNECT, NULL, NULL);
        rc = xwscp_tx_cmd_connect(xwscp);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tx;
        }
        next = xwtm_ft(XWSCP_RETRY_PERIOD);
        if (xwtm_cmp(to, next) > 0) {
                rc = xwos_cond_wait_to(&xwscp->tx.cscond,
                                       ulk, XWOS_LK_MTX, NULL,
                                       next, &lkst);
                if (-ETIMEDOUT == rc) {
                        rc = -EAGAIN;
                }
        } else {
                rc = xwos_cond_wait_to(&xwscp->tx.cscond,
                                       ulk, XWOS_LK_MTX, NULL,
                                       to, &lkst);
        }
        if (__xwcc_likely(XWOK == rc)) {
                xwos_mtx_unlock(&xwscp->tx.csmtx);
                xwaop_write(xwu32_t, &xwscp->tx.cnt, 0, NULL);
                xwaop_s1m(xwsq_t, &xwscp->hwifst, XWSCP_HWIFST_CONNECT, NULL, NULL);
                xwscp_hwifal_notify(xwscp, XWSCP_HWIFNTF_CONNECT);
        }
        return rc;

err_tx:
        xwos_mtx_unlock(&xwscp->tx.csmtx);
err_mtx_lock_to:
        return rc;
}

/**
 * @brief XWSCP API: 连接远程端
 * @param[in] xwscp: XWSCP对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENETUNREACH: 远程端无回应
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwmd_api
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t to)
{
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);

        rc = xwos_mtx_lock_to(&xwscp->tx.mtx, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_txmtx_lock_to;
        }
        do {
                rc = xwscp_connect_once(xwscp, to);
        } while (-EAGAIN == rc);
        if (-ETIMEDOUT == rc) {
                xwscp_hwifal_notify(xwscp, XWSCP_HWIFNTF_NETUNREACH);
                rc = -ENETUNREACH;
        }
        xwos_mtx_unlock(&xwscp->tx.mtx);
        return rc;

err_txmtx_lock_to:
        return rc;
}

static __xwmd_code
xwer_t xwscp_finish_tx(struct xwscp * xwscp, union xwscp_slot * slot)
{
        xwu8_t ack;
        xwu8_t rmtid;
        xwer_t rc;

        ack = xwscp->tx.remote.ack;
        rmtid = xwscp->tx.remote.id;

        if (rmtid != slot->tx.frm.head.id) {
                xwaop_c0m(xwsq_t, &xwscp->hwifst, XWSCP_HWIFST_CONNECT, NULL, NULL);
                xwscplogf(DEBUG, "[txthd] Remote ACK ID error!\n");
                rc = -ECONNRESET;
        } else {
                switch (ack) {
                case XWSCP_ACK_OK:
                        rc = xwscp_callback_rc[ack];
                        xwaop_add(xwu32_t, &xwscp->tx.cnt, 1, NULL, NULL);
                        break;
                case XWSCP_ACK_EALREADY:
                        rc = xwscp_callback_rc[XWSCP_ACK_OK];
                        xwaop_add(xwu32_t, &xwscp->tx.cnt, 1, NULL, NULL);
                        break;
                case XWSCP_ACK_ECONNRESET:
                        xwaop_c0m(xwsq_t, &xwscp->hwifst, XWSCP_HWIFST_CONNECT,
                                  NULL, NULL);
                        rc = -ECONNRESET;
                        break;
                case XWSCP_ACK_NOMEM:
                        xwscplogf(WARNING, "[txthd] Remote has no memory!\n");
                        rc = -ENOMEM;
                        break;
                default:
                        XWSCP_BUG();
                        rc = -EBUG;
                        break;
                }
        }
        return rc;
}

static __xwmd_code
xwer_t xwscp_tx_once(struct xwscp * xwscp,
                     const xwu8_t data[], xwsz_t * size, xwu8_t qos,
                     xwtm_t to)
{
        union xwos_ulock ulk;
        union xwscp_slot * slot;
        xwsq_t lkst;
        xwtm_t next;
        xwer_t rc;

        rc = xwscp_fmt_msg(xwscp, data, *size, qos, &slot);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_fmt_msg;
        }
        ulk.osal.mtx = &xwscp->tx.csmtx;
        rc = xwos_mtx_lock_to(&xwscp->tx.csmtx, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock_to;
        }
        xwaop_s1m(xwsq_t, &xwscp->hwifst, XWSCP_HWIFST_TX, NULL, NULL);
        rc = xwscp_hwifal_tx(xwscp, (xwu8_t *)&slot->tx.frm, slot->tx.frmsize);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_iftx;
        }
        next = xwtm_ft(XWSCP_RETRY_PERIOD);
        if (xwtm_cmp(to, next) > 0) {
                rc = xwos_cond_wait_to(&xwscp->tx.cscond,
                                       ulk, XWOS_LK_MTX, NULL,
                                       next, &lkst);
                if (-ETIMEDOUT == rc) {
                        rc = -EAGAIN;
                }
        } else {
                rc = xwos_cond_wait_to(&xwscp->tx.cscond,
                                       ulk, XWOS_LK_MTX, NULL,
                                       to, &lkst);
        }
        if (__xwcc_likely(XWOK == rc)) {
                xwos_mtx_unlock(&xwscp->tx.csmtx);
                rc = xwscp_finish_tx(xwscp, slot);
        } else {
                xwaop_c0m(xwsq_t, &xwscp->hwifst, XWSCP_HWIFST_TX, NULL, NULL);
        }
        return rc;

err_iftx:
        xwos_mtx_unlock(&xwscp->tx.csmtx);
err_mtx_lock_to:
err_fmt_msg:
        return rc;
}

/**
 * @brief XWSCP API: 发送一条报文，并在限定的时间等待回应
 * @param[in] xwscp: XWSCP对象的指针
 * @param[in] data: 数据缓冲区的指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示数据的字节数
 * + (O) 作为输出时，返回实际发送的字节数
 * @param[in] qos: 服务质量，取值范围： @ref xwscp_msg_qos_em
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENOTCONN: 无链接
 * @retval -ECONNRESET: 链接被复位
 * @retval -ENETUNREACH: 远程端无回应
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwmd_api
xwer_t xwscp_tx(struct xwscp * xwscp,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t qos, xwtm_t to)
{
        xwsq_t hwifst;
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((size), "nullptr", -EFAULT);
        XWSCP_VALIDATE((*size <= XWSCP_SDU_MAX_SIZE), "msg2long", -E2BIG);

        rc = xwos_mtx_lock_to(&xwscp->tx.mtx, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_txmtx_lock_to;
        }
        xwaop_read(xwsq_t, &xwscp->hwifst, &hwifst);
        if (XWSCP_HWIFST_CONNECT & hwifst) {
                do {
                        rc = xwscp_tx_once(xwscp, data, size, qos, to);
                } while (-EAGAIN == rc);
                if (-ETIMEDOUT == rc) {
                        xwscp_hwifal_notify(xwscp, XWSCP_HWIFNTF_NETUNREACH);
                        rc = -ENETUNREACH;
                }
        } else {
                rc = -ENOTCONN;
        }
        xwos_mtx_unlock(&xwscp->tx.mtx);
        return rc;

err_txmtx_lock_to:
        return rc;
}

/**
 * @brief XWSCP API: 接收一条报文，若接收队列为空，就限时等待
 * @param[in] xwscp: XWSCP对象的指针
 * @param[in] buf: 接收数据的缓冲区指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示报文的字节数
 * + (O) 作为输出时，返回实际接收的字节数
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
__xwmd_api
xwer_t xwscp_rx(struct xwscp * xwscp, xwu8_t buf[], xwsz_t * size, xwtm_t to)
{
        xwer_t rc;
        union xwscp_slot * slot;
        xwu8_t * sdupos;
        xwsz_t sdusize;
        xwsz_t realsize;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((size), "nullptr", -EFAULT);

        rc = xwos_sem_wait_to(&xwscp->rx.sem, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_wait_to;
        }
        slot = xwscp_rxq_choose(xwscp);
        sdupos = XWSCP_SDUPOS(&slot->rx.frm.head);
        sdusize = XWSCP_RXSDUSIZE(&slot->rx);
        realsize = sdusize > *size ? *size : sdusize;
        memcpy(buf, sdupos, realsize);
        xwmm_bma_free(xwscp->mempool, slot);
        *size = realsize;
        return XWOK;

err_sem_wait_to:
        return rc;
}

/**
 * @brief XWSCP API: 尝试接收消息，若接收队列为空，立即返回错误码
 * @param[in] xwscp: XWSCP对象的指针
 * @param[in] buf: 接收数据的缓冲区指针
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示报文的字节数
 * + (O) 作为输出时，返回实际接收的字节数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwscp_try_rx(struct xwscp * xwscp, xwu8_t buf[], xwsz_t * size)
{
        xwer_t rc;
        union xwscp_slot * slot;
        xwu8_t * sdupos;
        xwsz_t sdusize;
        xwsz_t realsize;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((size), "nullptr", -EFAULT);

        rc = xwos_sem_trywait(&xwscp->rx.sem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_trywait;
        }
        slot = xwscp_rxq_choose(xwscp);
        sdupos = XWSCP_SDUPOS(&slot->rx.frm.head);
        sdusize = XWSCP_RXSDUSIZE(&slot->rx);
        realsize = sdusize > *size ? *size : sdusize;
        memcpy(buf, sdupos, realsize);
        xwmm_bma_free(xwscp->mempool, slot);
        *size = realsize;
        return XWOK;

err_sem_trywait:
        return rc;
}
