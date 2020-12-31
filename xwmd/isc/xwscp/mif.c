/**
 * @file
 * @brief 精简的点对点通讯协议：应用接口层
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
#include <xwos/lib/xwaop.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/mif.h>

/**
 * @brief 接收线程的优先级
 */
#define XWSCP_THD_PRIORITY XWMDCFG_isc_xwscp_THD_PRIORITY

extern __xwmd_rodata const xwer_t xwscp_callback_rc[XWSCP_ACK_NUM];

/**
 * @brief 接收线程的描述
 */
static __xwmd_rodata
const struct xwos_thd_desc xwscp_thd_desc = {
        .name = "xwmd.isc.xwscp.thd",
        .prio = XWSCP_THD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thd_f)xwscp_thd,
        .arg = NULL, /* TBD */
        .attr = XWOS_SKDATTR_PRIVILEGED,
};

static __xwmd_code
xwer_t xwscp_connect_once(struct xwscp * xwscp, xwtm_t * xwtm, xwsq_t * txcnt);

static __xwmd_code
xwer_t xwscp_tx_once(struct xwscp * xwscp, const xwu8_t msg[], xwsz_t * size,
                     xwtm_t * xwtm, xwsq_t * txcnt);

static __xwmd_code
void xwscp_init(struct xwscp * xwscp)
{
        xwscp->name = NULL;
        xwscp->hwifst = XWSCP_HWIFST_CLOSED;
        xwscp->hwifops = NULL;
        xwscp->hwifcb = NULL;
}

/**
 * @brief XWSCP API: 启动XWSCP
 * @param xwscp: (I) XWSCP对象的指针
 * @param name: (I) XWSCP实例的名字
 * @param hwifops: (I) 数据链路层操作函数集合
 * @param hwifcb: (I) 硬件接口控制块指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwmd_api
xwer_t xwscp_start(struct xwscp * xwscp, const char * name,
                   const struct xwscp_hwifal_operations * hwifops,
                   void * hwifcb)
{
        xwsq_t i;
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((hwifops), "nullptr", -EFAULT);
        XWSCP_VALIDATE((hwifops->tx), "nullptr", -EFAULT);
        XWSCP_VALIDATE((hwifops->rx), "nullptr", -EFAULT);

        xwscplogf(DEBUG, "Starting XWSCP-%s ...\n", XWSCP_VERSION);

        xwscp_init(xwscp);

        /* 初始化发送状态机 */
        xwscp->txi.cnt = XWSCP_ID_SYNC;
        rc = xwos_mtx_init(&xwscp->txmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init txmtx ... [rc:%d]\n", rc);
                goto err_txmtx_init;
        }
        rc = xwos_mtx_init(&xwscp->csmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init csmtx ... [rc:%d]\n", rc);
                goto err_csmtx_init;
        }
        rc = xwos_cond_init(&xwscp->cscond);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init ACK condition ... [rc:%d]\n", rc);
                goto err_cscond_init;
        }
        xwscp->txi.frm = NULL;

        /* 初始化接收状态机 */
        xwscp->rxq.cnt = XWSCP_ID_SYNC;
        xwlib_bclst_init_head(&xwscp->rxq.head);
        xwos_splk_init(&xwscp->rxq.lock);
        rc = xwos_sem_init(&xwscp->slot.sem, XWSCP_FRMSLOT_NUM, XWSCP_FRMSLOT_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init slotsem ... [rc:%d]\n", rc);
                goto err_slotsem_init;
        }

        /* 初始化帧槽 */
        rc = xwos_sem_init(&xwscp->rxq.sem, 0, XWSCP_FRMSLOT_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init rxqsem ... [rc:%d]\n", rc);
                goto err_rxqsem_init;
        }
        memset(xwscp->slot.frm, 0, sizeof(xwscp->slot.frm));
        xwlib_bclst_init_head(&xwscp->slot.q);
        for (i = 0; i < XWSCP_FRMSLOT_NUM; i ++) {
                xwlib_bclst_init_node(&xwscp->slot.frm[i].node);
                xwlib_bclst_add_tail(&xwscp->slot.q, &xwscp->slot.frm[i].node);
        }
        xwos_splk_init(&xwscp->slot.lock);

        xwscp->name = name;
        xwscp->hwifops = hwifops;

        /* 打开端口 */
        rc = xwscp_hwifal_open(xwscp, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open;
        }

        /* 创建线程 */
        rc = xwos_thd_create(&xwscp->thd,
                             xwscp_thd_desc.name,
                             xwscp_thd_desc.func,
                             xwscp,
                             xwscp_thd_desc.stack_size,
                             xwscp_thd_desc.prio,
                             xwscp_thd_desc.attr);
        if (rc < 0) {
                goto err_thd_create;
        }

        return XWOK;

err_thd_create:
        xwscp_hwifal_close(xwscp);
err_hwifal_open:
err_rxqsem_init:
        xwos_sem_destroy(&xwscp->slot.sem);
err_slotsem_init:
        xwos_cond_destroy(&xwscp->cscond);
err_cscond_init:
        xwos_mtx_destroy(&xwscp->csmtx);
err_csmtx_init:
        xwos_mtx_destroy(&xwscp->txmtx);
err_txmtx_init:
        return rc;
}

/**
 * @brief XWSCP API: 停止XWSCP
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
__xwmd_api
xwer_t xwscp_stop(struct xwscp * xwscp)
{
        xwer_t rc, childrc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);

        rc = xwos_thd_stop(xwscp->thd, &childrc);
        if (XWOK == rc) {
                xwscplogf(INFO, "Stop XWSCP thread... [OK]\n");
        }

        xwscp_hwifal_close(xwscp);
        xwos_sem_destroy(&xwscp->rxq.sem);
        xwos_sem_destroy(&xwscp->slot.sem);
        xwos_cond_destroy(&xwscp->cscond);
        xwos_mtx_destroy(&xwscp->csmtx);
        xwos_mtx_destroy(&xwscp->txmtx);
        return XWOK;
}

static __xwmd_code
xwer_t xwscp_connect_once(struct xwscp * xwscp, xwtm_t * xwtm, xwsq_t * cnt)
{
        union xwos_ulock ulk;
        xwsq_t lockstate;
        xwtm_t time;
        xwer_t rc;

        ulk.osal.mtx = &xwscp->csmtx;
        rc = xwos_mtx_lock(&xwscp->csmtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        xwscp->txi.frm = NULL;
        rc = xwscp_tx_cfrm_sync(xwscp, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tx;
        }
        time = *xwtm > XWSCP_PERIOD ? XWSCP_PERIOD : *xwtm;
        rc = xwos_cond_timedwait(&xwscp->cscond,
                                 ulk, XWOS_LK_MTX, NULL,
                                 &time, &lockstate);
        if (__xwcc_likely(XWOK == rc)) {
                xwos_mtx_unlock(&xwscp->csmtx);
                xwaop_add(xwu32, &xwscp->txi.cnt, 1, NULL, NULL);
        } else {
                if (XWOS_LKST_LOCKED == lockstate) {
                        xwos_mtx_unlock(&xwscp->csmtx);
                }/* else {} */
                if (-ETIMEDOUT == rc) {
                        *xwtm -= XWSCP_PERIOD;
                        if (__xwcc_likely(*xwtm > 0)) {
                                *cnt = *cnt + 1;
                                rc = -EAGAIN;
                        }/* else {} */
                }/* else {} */
        }
        return rc;

err_tx:
        xwos_mtx_unlock(&xwscp->csmtx);
err_mtx_lock:
        return rc;
}

/**
 * @brief XWSCP API: 连接远程端
 * @param xwscp: (I) XWSCP对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 远程端无回应
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmd_api
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t * xwtm)
{
        xwsq_t cnt;
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((xwtm), "nullptr", -EFAULT);

        rc = xwos_mtx_timedlock(&xwscp->txmtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_txmtx_timedlock;
        }
        cnt = 0;
        do {
                if ((XWMDCFG_isc_xwscp_RETRY_NUM / 2) == cnt) {
                        xwscp_hwifal_notify(xwscp, XWSCP_HWIFNTF_NETUNREACH, xwtm);
                }/* else {} */
                rc = xwscp_connect_once(xwscp, xwtm, &cnt);
        } while ((-EAGAIN == rc) && (cnt < XWSCP_RETRY_NUM));
        if (((-EAGAIN == rc)) && (XWSCP_RETRY_NUM == cnt)) {
                rc = -ENOTCONN;
        }/* else {} */
        xwos_mtx_unlock(&xwscp->txmtx);
        return rc;

err_txmtx_timedlock:
        return rc;
}

static __xwmd_code
xwer_t xwscp_tx_once(struct xwscp * xwscp, const xwu8_t msg[], xwsz_t * size,
                     xwtm_t * xwtm, xwsq_t * cnt)
{
        union xwos_ulock ulk;
        struct xwscp_frame frm;
        xwsq_t lockstate;
        xwtm_t time;
        xwer_t rc;
        xwer_t ack;

        rc = xwscp_fmt_msg(xwscp, &frm, msg, (xwu8_t)*size);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_fmt_msg;
        }
        ulk.osal.mtx = &xwscp->csmtx;
        rc = xwos_mtx_lock(&xwscp->csmtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        xwscp->txi.frm = &frm;
        rc = xwscp_hwifal_tx(xwscp, &frm, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_iftx;
        }
        time = *xwtm > XWSCP_PERIOD ? XWSCP_PERIOD : *xwtm;
        rc = xwos_cond_timedwait(&xwscp->cscond,
                                 ulk, XWOS_LK_MTX, NULL,
                                 &time, &lockstate);
        if (__xwcc_likely(XWOK == rc)) {
                ack = xwscp->txi.ack;
                xwscp->txi.frm = NULL;
                xwos_mtx_unlock(&xwscp->csmtx);
                switch (ack) {
                case XWSCP_ACK_OK:
                        rc = xwscp_callback_rc[ack];
                        xwaop_add(xwu32, &xwscp->txi.cnt, 1, NULL, NULL);
                        break;
                case XWSCP_ACK_ESIZE:
                        rc = xwscp_callback_rc[ack];
                        xwaop_c0m(xwu32, &xwscp->txi.cnt, XWSCP_ID_MSK,
                                  NULL, NULL);
                        break;
                case XWSCP_ACK_EALREADY:
                        rc = xwscp_callback_rc[XWSCP_ACK_OK];
                        xwaop_add(xwu32, &xwscp->txi.cnt, 1, NULL, NULL);
                        break;
                case XWSCP_ACK_ECONNRESET:
                        xwaop_c0m(xwu32, &xwscp->txi.cnt, XWSCP_ID_MSK,
                                  NULL, NULL);
                        break;
                }
        } else {
                xwscp->txi.frm = NULL;
                if (XWOS_LKST_LOCKED == lockstate) {
                        xwos_mtx_unlock(&xwscp->csmtx);
                }/* else {} */
                if (-ETIMEDOUT == rc) {
                        *xwtm -= XWSCP_PERIOD;
                        if (__xwcc_likely(*xwtm > 0)) {
                                *cnt = *cnt + 1;
                                rc = -EAGAIN;
                        }/* else {} */
                }/* else {} */
        }
        return rc;

err_iftx:
        xwscp->txi.frm = NULL;
        xwos_mtx_unlock(&xwscp->csmtx);
err_mtx_lock:
err_fmt_msg:
        return rc;
}

/**
 * @brief XWSCP API: 发送一条报文，并在限定的时间等待回应
 * @param xwscp: (I) XWSCP对象的指针
 * @param msg: (I) 报文
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示报文的字节数
 *              (O) 作为输出时，返回实际发送的字节数
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -E2BIG: 数据太长
 * @retval -ENOTCONN: 远程端无回应
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmd_api
xwer_t xwscp_tx(struct xwscp * xwscp, const xwu8_t msg[], xwsz_t * size,
                xwtm_t * xwtm)
{
        xwsq_t cnt;
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((size), "nullptr", -EFAULT);
        XWSCP_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWSCP_VALIDATE((*size <= XWSCP_SDU_MAX_SIZE), "msg2long", -E2BIG);

        rc = xwos_mtx_timedlock(&xwscp->txmtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_txmtx_timedlock;
        }
        cnt = 0;
        do {
                if ((XWMDCFG_isc_xwscp_RETRY_NUM / 2) == cnt) {
                        xwscp_hwifal_notify(xwscp, XWSCP_HWIFNTF_NETUNREACH, xwtm);
                }/* else {} */
                rc = xwscp_tx_once(xwscp, msg, size, xwtm, &cnt);
                if (-ENOLINK == rc) {
                        rc = xwscp_connect_once(xwscp, xwtm, &cnt);
                        if (XWOK == rc) {
                                rc = -EAGAIN;
                        }/* else {} */
                }/* else {} */
        } while ((-EAGAIN == rc) && (cnt < XWSCP_RETRY_NUM));
        if (((-EAGAIN == rc)) && (XWSCP_RETRY_NUM == cnt)) {
                xwaop_c0m(xwu32, &xwscp->txi.cnt, XWSCP_ID_MSK, NULL, NULL);
                rc = -ENOTCONN;
        }/* else {} */
        xwos_mtx_unlock(&xwscp->txmtx);
        return rc;

err_txmtx_timedlock:
        return rc;
}

/**
 * @brief XWSCP API: 接收一条报文，若接收队列为空，就限时等待
 * @param xwscp: (I) XWSCP对象的指针
 * @param buf: (I) 接收报文数据的缓冲区指针
 * @param size: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示报文的字节数
 *              (O) 作为输出时，返回实际接收的字节数
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwmd_api
xwer_t xwscp_rx(struct xwscp * xwscp, xwu8_t buf[], xwsz_t * size, xwtm_t * xwtm)
{
        xwer_t rc;
        struct xwscp_frmslot * pubfrm;
        xwsz_t realsize;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((size), "nullptr", -EFAULT);
        XWSCP_VALIDATE((xwtm), "nullptr", -EFAULT);

        rc = xwscp_rxq_choose(xwscp, &pubfrm, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_rxq_choose;
        }
        realsize = pubfrm->frm.head.frmlen - sizeof(struct xwscp_frmhead) -
                   XWSCP_CHKSUM_SIZE;
        realsize = realsize > *size ? *size : realsize;
        memcpy(buf, pubfrm->frm.sdu, realsize);
        xwscp_free_frmslot(xwscp, pubfrm);
        *size = realsize;
        return XWOK;

err_rxq_choose:
        return rc;
}