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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/api.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********        type       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwpcp_tx_cbarg {
        struct xwosal_splk splk;
        struct xwosal_cdt cdt;
        volatile xwer_t rc;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWPCP_TXTHRD_PRIORITY XWMDCFG_isc_xwpcp_TXTHRD_PRIORITY
#define XWPCP_RXTHRD_PRIORITY XWMDCFG_isc_xwpcp_RXTHRD_PRIORITY

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 发送线程的描述
 */
static __xwmd_rodata
const struct xwosal_thrd_desc xwpcp_rxthrd_td = {
        .name = "xwmd.isc.xwpcp.rxthrd",
        .prio = XWPCP_RXTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwpcp_rxthrd,
        .arg = NULL, /* TBD */
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
/**
 * @brief 接收线程的ID
 */
xwid_t xwpcp_rxthrd_tid;

/**
 * @brief 发送线程的描述
 */
static __xwmd_rodata
const struct xwosal_thrd_desc xwpcp_txthrd_td = {
        .name = "xwmd.isc.xwpcp.txthrd",
        .prio = XWPCP_TXTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwpcp_txthrd,
        .arg = NULL, /* TBD */
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
/**
 * @brief 发送线程的ID
 */
xwid_t xwpcp_txthrd_tid;

/**
 * @brief 内存池
 */
xwu8_t __xwcc_aligned_l1cacheline xwpcp_mempool[XWPCP_MEMPOOL_SIZE];

/**
 * @brief 内存池名字
 */
static __xwmd_rodata
const char xwpcp_frmslot_mempool_name[] = "mempool.frameslot.xwpcp";

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
void xwpcp_init(struct xwpcp * xwpcp);

static __xwmd_code
void xwpcp_tx_notify(struct xwpcp * xwpcp, xwpcp_fhdl_t fhdl, xwer_t rc, void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
void xwpcp_init(struct xwpcp * xwpcp)
{
        xwpcp->name = NULL;
        xwpcp->refcnt = XWPCP_REFCNT_STOPPED;
        xwpcp->hwifst = XWPCP_HWIFST_CLOSED;
        xwpcp->hwifops = NULL;
        xwpcp->hwifcb = NULL;
}

__xwmd_api
xwer_t xwpcp_start(struct xwpcp * xwpcp, const char * name,
                   const struct xwpcp_hwifal_operations * hwifops,
                   void * hwifcb)
{
        xwer_t rc, childrc;
        struct xwmm_bma * slotpool;
        xwssq_t i, j;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((hwifops), "nullptr", -EFAULT);
        XWPCP_VALIDATE((hwifops->tx), "nullptr", -EFAULT);
        XWPCP_VALIDATE((hwifops->rx), "nullptr", -EFAULT);

        xwpcplogf(DEBUG, "Starting XWPCP-%s ...\n", XWPCP_VERSION);

        xwpcp_init(xwpcp);
        rc = xwaop_teq_then_add(xwsq_t, &xwpcp->refcnt,
                                XWPCP_REFCNT_STOPPED, 1,
                                NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "XWPCP is not constructed! ... [rc:%d]\n", rc);
                rc = -EPERM;
                goto err_grab_xwpcp;
        }

        /* 创建内存池 */
        rc = xwmm_bma_create(&slotpool, xwpcp_frmslot_mempool_name,
                             (xwptr_t)xwpcp_mempool, (xwsz_t)sizeof(xwpcp_mempool),
                             XWPCP_MEMBLK_SIZE);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Create bma ... [rc:%d]\n", rc);
                goto err_bma_create;
        }
        xwpcp->slot.pool = slotpool;
        xwpcplogf(DEBUG, "Create BMA ... [OK], slotpool:%p\n", slotpool);

        /* 初始化发送状态机 */
        xwpcp->txq.cnt = XWPCP_ID_SYNC;
        for (i = 0; i < (xwssq_t)XWPCP_PRIORITY_NUM; i++) {
                xwlib_bclst_init_head(&xwpcp->txq.q[i]);
        }
        memset(&xwpcp->txq.nebmp, 0, sizeof(xwpcp->txq.nebmp));
        xwosal_splk_init(&xwpcp->txq.lock);
        rc = xwosal_smr_init(&xwpcp->txq.smr, 0, XWPCP_MEMBLK_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Init TXQ semaphore ... [rc:%d]\n", rc);
                goto err_txqsmr_init;
        }
        rc = xwosal_mtx_init(&xwpcp->txq.csmtx, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Init xwpcp->csmtx ... [rc:%d]\n", rc);
                goto err_csmtx_init;
        }
        rc = xwosal_cdt_init(&xwpcp->txq.cscdt);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "Init xwpcp->cscdt ... [rc:%d]\n", rc);
                goto err_cscdt_init;
        }
        xwpcp->txq.txi.sender = NULL;
        xwpcp->txq.tmp = NULL;
        xwosal_splk_init(&xwpcp->txq.ntflock);

        /* 初始化接收状态机 */
        xwpcp->rxq.cnt = XWPCP_ID_SYNC;
        for (i = 0; i < (xwssq_t)XWPCP_PORT_NUM; i++) {
                xwlib_bclst_init_head(&xwpcp->rxq.q[i]);
                xwosal_splk_init(&xwpcp->rxq.lock[i]);
                rc = xwosal_smr_init(&xwpcp->rxq.smr[i], 0, XWPCP_MEMBLK_NUM);
                if (__xwcc_unlikely(rc < 0)) {
                        xwpcplogf(ERR, "Init RXQ semaphore[%d] ... [rc:%d]\n",
                                  i, rc);
                        goto err_rxqsmr_init;
                }
        }

        xwpcp->name = name;
        xwpcp->hwifops = hwifops;
        xwpcp->hwifst = XWPCP_HWIFST_CLOSED;

        rc = xwpcp_hwifal_open(xwpcp, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open;
        }

        /* 创建线程 */
        rc = xwosal_thrd_create(&xwpcp_rxthrd_tid,
                                xwpcp_rxthrd_td.name,
                                xwpcp_rxthrd_td.func,
                                xwpcp,
                                xwpcp_rxthrd_td.stack_size,
                                xwpcp_rxthrd_td.prio,
                                xwpcp_rxthrd_td.attr);
        if (rc < 0) {
                goto err_rxthrd_create;
        }

        rc = xwosal_thrd_create(&xwpcp_txthrd_tid,
                                xwpcp_txthrd_td.name,
                                xwpcp_txthrd_td.func,
                                xwpcp,
                                xwpcp_txthrd_td.stack_size,
                                xwpcp_txthrd_td.prio,
                                xwpcp_txthrd_td.attr);
        if (rc < 0) {
                goto err_txthrd_create;
        }

        return XWOK;

err_txthrd_create:
        xwosal_thrd_terminate(xwpcp_rxthrd_tid, &childrc);
        xwosal_thrd_delete(xwpcp_rxthrd_tid);
        xwpcp_rxthrd_tid = 0;
err_rxthrd_create:
        xwpcp_hwifal_close(xwpcp);
err_hwifal_open:
err_rxqsmr_init:
        for (j = i - 1; j >= 0; j--) {
                xwosal_smr_destroy(&xwpcp->rxq.smr[j]);
        }
        xwosal_cdt_destroy(&xwpcp->txq.cscdt);
err_cscdt_init:
        xwosal_mtx_destroy(&xwpcp->txq.csmtx);
err_csmtx_init:
        xwosal_smr_destroy(&xwpcp->txq.smr);
err_txqsmr_init:
        xwmm_bma_delete(xwpcp->slot.pool);
        xwpcp->slot.pool = NULL;
err_bma_create:
        xwaop_sub(xwsq_t, &xwpcp->refcnt, 1, NULL, NULL);
err_grab_xwpcp:
        return rc;
}

__xwmd_api
xwer_t xwpcp_stop(struct xwpcp * xwpcp)
{
        struct xwpcp_frmslot * frmslot;
        xwer_t rc, childrc;
        xwssq_t j;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);

        rc = xwosal_thrd_terminate(xwpcp_txthrd_tid, &childrc);
        if (XWOK == rc) {
                rc = xwosal_thrd_delete(xwpcp_txthrd_tid);
                if (XWOK == rc) {
                        xwpcp_txthrd_tid = 0;
                        xwpcplogf(INFO, "Terminate XWPCP TX thread... [OK]\n");
                }
        }

        rc = xwosal_thrd_terminate(xwpcp_rxthrd_tid, &childrc);
        if (XWOK == rc) {
                rc = xwosal_thrd_delete(xwpcp_rxthrd_tid);
                if (XWOK == rc) {
                        xwpcp_rxthrd_tid = 0;
                        xwpcplogf(INFO, "Terminate XWPCP RX thread... [OK]\n");
                }
        }

        rc = xwaop_teq_then_sub(xwsq_t, &xwpcp->refcnt,
                                XWPCP_REFCNT_STARTED, 1,
                                NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                xwpcplogf(ERR, "XWPCP is in used.\n");
                rc = -EPERM;
                goto err_ifbusy;
        }

        rc = xwpcp_hwifal_close(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_hwifal_close;
        }

        /* 释放TXQ中的剩余帧 */
        do {
                rc = xwosal_smr_trywait(xwosal_smr_get_id(&xwpcp->txq.smr));
                if (rc < 0) {
                        break;
                }
                frmslot = xwpcp_txq_choose(xwpcp);
                xwmm_bma_free(xwpcp->slot.pool, frmslot);
        } while (true);

        /* 释放RXQ中的剩余帧 */
        for (j = XWPCP_PORT_NUM - 1; j >= 0; j--) {
                do {
                        rc = xwosal_smr_trywait(xwosal_smr_get_id(&xwpcp->rxq.smr[j]));
                        if (rc < 0) {
                                break;
                        }
                        frmslot = xwpcp_rxq_choose(xwpcp, (xwu8_t)j);
                        xwmm_bma_free(xwpcp->slot.pool, frmslot);
                } while (true);
        }

        for (j = XWPCP_PORT_NUM - 1; j >= 0; j--) {
                xwosal_smr_destroy(&xwpcp->rxq.smr[j]);
        }
        xwosal_cdt_destroy(&xwpcp->txq.cscdt);
        xwosal_mtx_destroy(&xwpcp->txq.csmtx);
        xwosal_smr_destroy(&xwpcp->txq.smr);
        xwmm_bma_delete(xwpcp->slot.pool);
        xwpcp->slot.pool = NULL;
        return XWOK;

err_hwifal_close:
        xwaop_add(xwsq_t, &xwpcp->refcnt, 1, NULL, NULL);
err_ifbusy:
        return rc;
}

/**
 * @brief 实现@ref xwpcp_tx()的回调函数
 * @param xwpcp: (I) XWPCP对象的指针
 * @param fhdl: (I) 帧槽句柄
 * @param rc: (I) 发送结果
 * @param arg: (I) 自定义参数
 */
static __xwmd_code
void xwpcp_tx_notify(struct xwpcp * xwpcp, xwpcp_fhdl_t fhdl, xwer_t rc, void * arg)
{
        struct xwpcp_tx_cbarg * cbarg;

        XWOS_UNUSED(xwpcp);
        XWOS_UNUSED(fhdl);

        xwpcplogf(DEBUG, "fhdl:%p, rc:%d\n", fhdl, rc);
        cbarg = arg;
        xwosal_splk_lock(&cbarg->splk);
        cbarg->rc = rc;
        xwosal_splk_unlock(&cbarg->splk);
        xwosal_cdt_unicast(xwosal_cdt_get_id(&cbarg->cdt));
}

__xwmd_api
xwer_t xwpcp_tx(struct xwpcp * xwpcp,
                const struct xwpcp_msg * msg, xwu8_t prio,
                xwtm_t * xwtm)
{
        struct xwpcp_tx_cbarg cbarg;
        union xwlk_ulock ulk;
        xwpcp_fhdl_t fhdl;
        xwid_t cdtid;
        xwsq_t lkst;
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((msg), "nullptr", -EFAULT);
        XWPCP_VALIDATE((msg->port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((msg->port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);
        XWPCP_VALIDATE(((XWPCP_MSG_QOS_1 == msg->qos) ||
                        (XWPCP_MSG_QOS_0 == msg->qos)), "qos-invalid", -EINVAL);
        XWPCP_VALIDATE((msg->size <= XWPCP_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWPCP_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWPCP_VALIDATE((prio < XWPCP_PRIORITY_NUM), "prio-invalid", -E2BIG);

        xwpcplogf(DEBUG, "msg->port:%d, msg->size:0x%X\n", msg->port, msg->size);
        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        xwosal_splk_init(&cbarg.splk);
        rc = xwosal_cdt_init(&cbarg.cdt);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_cdt_init;
        }
        cbarg.rc = -EINPROGRESS;

        cdtid = xwosal_cdt_get_id(&cbarg.cdt);
        rc = xwpcp_eq(xwpcp, msg, prio, xwpcp_tx_notify, &cbarg, &fhdl);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwpcp_eq;
        }

        ulk.osal.splk = &cbarg.splk;
        xwosal_splk_lock(&cbarg.splk);
        if (-EINPROGRESS == cbarg.rc) {
                rc = xwosal_cdt_timedwait(cdtid, ulk, XWLK_TYPE_SPLK, NULL,
                                          xwtm, &lkst);
                if (XWOK == rc) {
                        rc = cbarg.rc;
                        xwosal_splk_unlock(&cbarg.splk);
                } else {
                        if (XWLK_STATE_LOCKED == lkst) {
                                xwosal_splk_unlock(&cbarg.splk);
                        }/* else {} */
                        xwpcp_lock_ntflock(xwpcp);
                        xwosal_splk_lock(&cbarg.splk);
                        if (-EINPROGRESS == cbarg.rc) {
                                fhdl->cbfunc = NULL;
                                fhdl->cbarg = NULL;
                        } else {
                                rc = cbarg.rc;
                        }
                        xwosal_splk_unlock(&cbarg.splk);
                        xwpcp_unlock_ntflock(xwpcp);
                }
        } else {
                rc = cbarg.rc;
                xwosal_splk_unlock(&cbarg.splk);
        }
        xwosal_cdt_destroy(&cbarg.cdt);
        xwpcp_put(xwpcp);
        return rc;

err_xwpcp_eq:
        xwosal_cdt_destroy(&cbarg.cdt);
err_cdt_init:
        xwpcp_put(xwpcp);
err_ifnotrdy:
        return rc;
}

__xwmd_api
xwer_t xwpcp_eq(struct xwpcp * xwpcp,
                const struct xwpcp_msg * msg, xwu8_t prio,
                xwpcp_ntf_f cbfunc, void * cbarg,
                xwpcp_fhdl_t * fhdlbuf)
{
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE(msg, "nullptr", -EFAULT);
        XWPCP_VALIDATE((msg->port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((msg->port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);
        XWPCP_VALIDATE(((XWPCP_MSG_QOS_1 == msg->qos) ||
                        (XWPCP_MSG_QOS_0 == msg->qos)), "qos-invalid", -EINVAL);
        XWPCP_VALIDATE((msg->size <= XWPCP_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWPCP_VALIDATE((prio < XWPCP_PRIORITY_NUM), "prio-invalid", -E2BIG);

        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }
        if (prio >= XWPCP_MAX_PRIORITY) {
                prio = XWPCP_MAX_PRIORITY - 1;
        }/* else {} */
        rc = xwpcp_eq_msg(xwpcp, msg, prio, cbfunc, cbarg, fhdlbuf);
        xwpcp_put(xwpcp);

err_ifnotrdy:
        return rc;
}

__xwmd_api
void xwpcp_lock_ntflock(struct xwpcp * xwpcp)
{
        xwosal_splk_lock(&xwpcp->txq.ntflock);
}

__xwmd_api
void xwpcp_unlock_ntflock(struct xwpcp * xwpcp)
{
        xwosal_splk_unlock(&xwpcp->txq.ntflock);
}

__xwmd_api
xwer_t xwpcp_rx(struct xwpcp * xwpcp, struct xwpcp_msg * msgbuf, xwtm_t * xwtm)
{
        struct xwpcp_frmslot * frmslot;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((msgbuf), "nullptr", -EFAULT);
        XWPCP_VALIDATE((msgbuf->port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((msgbuf->port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);
        XWPCP_VALIDATE((xwtm), "nullptr", -EFAULT);

        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        xwpcplogf(DEBUG, "port:%d, buffer size:0x%X\n", msgbuf->port, msgbuf->size);
        rc = xwosal_smr_timedwait(xwosal_smr_get_id(&xwpcp->rxq.smr[msgbuf->port]),
                                  xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_timedwait;
        }

        frmslot = xwpcp_rxq_choose(xwpcp, msgbuf->port);
        sdusize = frmslot->frm.head.frmlen -
                  sizeof(struct xwpcp_frmhead) -
                  XWPCP_CHKSUM_SIZE;

        if (msgbuf->size < sdusize) {
                realsize = msgbuf->size;
                xwpcplogf(WARNING,
                        "buffer size is too small(0x%X)!, expected size is 0x%X\n",
                        msgbuf->size, sdusize);
        } else {
                realsize = sdusize;
        }
        memcpy(msgbuf->text, frmslot->frm.sdu, realsize);
        msgbuf->qos = XWPCP_PORT_QOS(frmslot->frm.head.port);
        msgbuf->size = realsize;

        xwmm_bma_free(xwpcp->slot.pool, frmslot);
        xwpcp_put(xwpcp);
        return XWOK;

err_smr_timedwait:
        xwpcp_put(xwpcp);
err_ifnotrdy:
        return rc;
}

__xwmd_api
xwer_t xwpcp_try_rx(struct xwpcp * xwpcp, struct xwpcp_msg * msgbuf)
{
        struct xwpcp_frmslot * frmslot;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWPCP_VALIDATE((xwpcp), "nullptr", -EFAULT);
        XWPCP_VALIDATE((msgbuf), "nullptr", -EFAULT);
        XWPCP_VALIDATE((msgbuf->port > XWPCP_PORT_CMD), "port0-not-permitted", -ENXIO);
        XWPCP_VALIDATE((msgbuf->port < XWPCP_PORT_NUM), "no-such-port", -ENODEV);

        rc = xwpcp_grab(xwpcp);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        xwpcplogf(DEBUG, "port:%d, buffer size:0x%X\n", msgbuf->port, msgbuf->size);
        rc = xwosal_smr_trywait(xwosal_smr_get_id(&xwpcp->rxq.smr[msgbuf->port]));
        if (__xwcc_unlikely(rc < 0)) {
                goto err_smr_trywait;
        }

        frmslot = xwpcp_rxq_choose(xwpcp, msgbuf->port);
        sdusize = frmslot->frm.head.frmlen - sizeof(struct xwpcp_frmhead) -
                  XWPCP_CHKSUM_SIZE;

        if (msgbuf->size < sdusize) {
                realsize = msgbuf->size;
                xwpcplogf(WARNING,
                        "buffer size is too small(0x%X)!, expected size is 0x%X\n",
                        msgbuf->size, sdusize);
        } else {
                realsize = sdusize;
        }
        memcpy(msgbuf->text, frmslot->frm.sdu, realsize);
        msgbuf->size = realsize;
        msgbuf->qos = XWPCP_PORT_QOS(frmslot->frm.head.port);

        xwmm_bma_free(xwpcp->slot.pool, frmslot);
        xwpcp_put(xwpcp);
        return XWOK;

err_smr_trywait:
        xwpcp_put(xwpcp);
err_ifnotrdy:
        return rc;
}
