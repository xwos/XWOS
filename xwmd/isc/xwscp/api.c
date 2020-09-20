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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwaop.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/lock/mutex.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/api.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSCP_THRD_PRIORITY XWMDCFG_isc_xwscp_THRD_PRIORITY

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwmd_rodata const xwer_t xwscp_callback_rc[XWSCP_ACK_NUM];

/**
 * @brief 接收线程的描述
 */
static __xwmd_rodata
const struct xwosal_thrd_desc xwscp_thrd_td = {
        .name = "xwmd.isc.xwscp.thrd",
        .prio = XWSCP_THRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwscp_thrd,
        .arg = NULL, /* TBD */
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
/**
 * @brief 接收线程的ID
 */
xwid_t xwscp_thrd_tid;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
xwer_t xwscp_connect_once(struct xwscp * xwscp, xwtm_t * xwtm, xwsq_t * txcnt);

static __xwmd_code
xwer_t xwscp_tx_once(struct xwscp * xwscp, const xwu8_t msg[], xwsz_t * size,
                     xwtm_t * xwtm, xwsq_t * txcnt);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
void xwscp_init(struct xwscp * xwscp)
{
        xwscp->name = NULL;
        xwscp->hwifst = XWSCP_HWIFST_CLOSED;
        xwscp->hwifops = NULL;
        xwscp->hwifcb = NULL;
}

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
        rc = xwosal_mtx_init(&xwscp->txmtx, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init txmtx ... [rc:%d]\n", rc);
                goto err_txmtx_init;
        }
        rc = xwosal_mtx_init(&xwscp->csmtx, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init csmtx ... [rc:%d]\n", rc);
                goto err_csmtx_init;
        }
        rc = xwosal_cdt_init(&xwscp->cscdt);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init ACK condition ... [rc:%d]\n", rc);
                goto err_cscdt_init;
        }
        xwscp->txi.frm = NULL;

        /* 初始化接收状态机 */
        xwscp->rxq.cnt = XWSCP_ID_SYNC;
        xwlib_bclst_init_head(&xwscp->rxq.head);
        xwosal_splk_init(&xwscp->rxq.lock);
        rc = xwosal_smr_init(&xwscp->slot.smr, XWSCP_FRMSLOT_NUM, XWSCP_FRMSLOT_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init slotsmr ... [rc:%d]\n", rc);
                goto err_slotsmr_init;
        }

        /* 初始化帧槽 */
        rc = xwosal_smr_init(&xwscp->rxq.smr, 0, XWSCP_FRMSLOT_NUM);
        if (__xwcc_unlikely(rc < 0)) {
                xwscplogf(ERR, "Init rxqsmr ... [rc:%d]\n", rc);
                goto err_rxqsmr_init;
        }
        memset(xwscp->slot.frm, 0, sizeof(xwscp->slot.frm));
        xwlib_bclst_init_head(&xwscp->slot.q);
        for (i = 0; i < XWSCP_FRMSLOT_NUM; i ++) {
                xwlib_bclst_init_node(&xwscp->slot.frm[i].node);
                xwlib_bclst_add_tail(&xwscp->slot.q, &xwscp->slot.frm[i].node);
        }
        xwosal_splk_init(&xwscp->slot.lock);

        xwscp->name = name;
        xwscp->hwifops = hwifops;

        /* 打开端口 */
        rc = xwscp_hwifal_open(xwscp, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open;
        }

        /* 创建线程 */
        rc = xwosal_thrd_create(&xwscp_thrd_tid,
                                xwscp_thrd_td.name,
                                xwscp_thrd_td.func,
                                xwscp,
                                xwscp_thrd_td.stack_size,
                                xwscp_thrd_td.prio,
                                xwscp_thrd_td.attr);
        if (rc < 0) {
                goto err_thrd_create;
        }

        return XWOK;

err_thrd_create:
        xwscp_hwifal_close(xwscp);
err_hwifal_open:
err_rxqsmr_init:
        xwosal_smr_destroy(&xwscp->slot.smr);
err_slotsmr_init:
        xwosal_cdt_destroy(&xwscp->cscdt);
err_cscdt_init:
        xwosal_mtx_destroy(&xwscp->csmtx);
err_csmtx_init:
        xwosal_mtx_destroy(&xwscp->txmtx);
err_txmtx_init:
        return rc;
}

__xwmd_api
xwer_t xwscp_stop(struct xwscp * xwscp)
{
        xwer_t rc, childrc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);

        rc = xwosal_thrd_terminate(xwscp_thrd_tid, &childrc);
        if (XWOK == rc) {
                rc = xwosal_thrd_delete(xwscp_thrd_tid);
                if (XWOK == rc) {
                        xwscp_thrd_tid = 0;
                        xwscplogf(INFO, "Terminate XWSCP thread... [OK]\n");
                }
        }

        xwscp_hwifal_close(xwscp);
        xwosal_smr_destroy(&xwscp->rxq.smr);
        xwosal_smr_destroy(&xwscp->slot.smr);
        xwosal_cdt_destroy(&xwscp->cscdt);
        xwosal_mtx_destroy(&xwscp->csmtx);
        xwosal_mtx_destroy(&xwscp->txmtx);
        return XWOK;
}

static __xwmd_code
xwer_t xwscp_connect_once(struct xwscp * xwscp, xwtm_t * xwtm, xwsq_t * cnt)
{
        union xwlk_ulock ulk;
        xwsq_t lockstate;
        xwtm_t time;
        xwer_t rc;

        ulk.osal.id = xwosal_mtx_get_id(&xwscp->csmtx);
        rc = xwosal_mtx_lock(ulk.osal.id);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        xwscp->txi.frm = NULL;
        rc = xwscp_tx_cfrm_sync(xwscp, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_tx;
        }
        time = *xwtm > XWSCP_PERIOD ? XWSCP_PERIOD : *xwtm;
        rc = xwosal_cdt_timedwait(xwosal_cdt_get_id(&xwscp->cscdt),
                                  ulk, XWLK_TYPE_MTX, NULL,
                                  &time, &lockstate);
        if (__xwcc_likely(XWOK == rc)) {
                xwosal_mtx_unlock(ulk.osal.id);
                xwaop_add(xwu32_t, &xwscp->txi.cnt, 1, NULL, NULL);
        } else {
                if (XWLK_STATE_LOCKED == lockstate) {
                        xwosal_mtx_unlock(ulk.osal.id);
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
        xwosal_mtx_unlock(ulk.osal.id);
err_mtx_lock:
        return rc;
}

__xwmd_api
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t * xwtm)
{
        xwid_t txmtxid;
        xwsq_t cnt;
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((xwtm), "nullptr", -EFAULT);

        txmtxid = xwosal_mtx_get_id(&xwscp->txmtx);
        rc = xwosal_mtx_timedlock(txmtxid, xwtm);
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
        xwosal_mtx_unlock(txmtxid);
        return rc;

err_txmtx_timedlock:
        return rc;
}

static __xwmd_code
xwer_t xwscp_tx_once(struct xwscp * xwscp, const xwu8_t msg[], xwsz_t * size,
                     xwtm_t * xwtm, xwsq_t * cnt)
{
        union xwlk_ulock ulk;
        struct xwscp_frame frm;
        xwsq_t lockstate;
        xwtm_t time;
        xwer_t rc;
        xwer_t ack;

        rc = xwscp_fmt_msg(xwscp, &frm, msg, (xwu8_t)*size);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_fmt_msg;
        }
        ulk.osal.id = xwosal_mtx_get_id(&xwscp->csmtx);
        rc = xwosal_mtx_lock(ulk.osal.id);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        xwscp->txi.frm = &frm;
        rc = xwscp_hwifal_tx(xwscp, &frm, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_iftx;
        }
        time = *xwtm > XWSCP_PERIOD ? XWSCP_PERIOD : *xwtm;
        rc = xwosal_cdt_timedwait(xwosal_cdt_get_id(&xwscp->cscdt),
                                  ulk, XWLK_TYPE_MTX, NULL,
                                  &time, &lockstate);
        if (__xwcc_likely(XWOK == rc)) {
                ack = xwscp->txi.ack;
                xwscp->txi.frm = NULL;
                xwosal_mtx_unlock(ulk.osal.id);
                switch (ack) {
                case XWSCP_ACK_OK:
                        rc = xwscp_callback_rc[ack];
                        xwaop_add(xwu32_t, &xwscp->txi.cnt, 1, NULL, NULL);
                        break;
                case XWSCP_ACK_ESIZE:
                        rc = xwscp_callback_rc[ack];
                        xwaop_c0m(xwu32_t, &xwscp->txi.cnt, XWSCP_ID_MSK,
                                  NULL, NULL);
                        break;
                case XWSCP_ACK_EALREADY:
                        rc = xwscp_callback_rc[XWSCP_ACK_OK];
                        xwaop_add(xwu32_t, &xwscp->txi.cnt, 1, NULL, NULL);
                        break;
                case XWSCP_ACK_ECONNRESET:
                        xwaop_c0m(xwu32_t, &xwscp->txi.cnt, XWSCP_ID_MSK,
                                  NULL, NULL);
                        break;
                }
        } else {
                xwscp->txi.frm = NULL;
                if (XWLK_STATE_LOCKED == lockstate) {
                        xwosal_mtx_unlock(ulk.osal.id);
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
        xwosal_mtx_unlock(ulk.osal.id);
err_mtx_lock:
err_fmt_msg:
        return rc;
}

__xwmd_api
xwer_t xwscp_tx(struct xwscp * xwscp, const xwu8_t msg[], xwsz_t * size,
                xwtm_t * xwtm)
{
        xwid_t txmtxid;
        xwsq_t cnt;
        xwer_t rc;

        XWSCP_VALIDATE((xwscp), "nullptr", -EFAULT);
        XWSCP_VALIDATE((size), "nullptr", -EFAULT);
        XWSCP_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWSCP_VALIDATE((*size <= XWSCP_SDU_MAX_SIZE), "msg2long", -E2BIG);

        txmtxid = xwosal_mtx_get_id(&xwscp->txmtx);
        rc = xwosal_mtx_timedlock(txmtxid, xwtm);
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
                xwaop_c0m(xwu32_t, &xwscp->txi.cnt, XWSCP_ID_MSK, NULL, NULL);
                rc = -ENOTCONN;
        }/* else {} */
        xwosal_mtx_unlock(txmtxid);
        return rc;

err_txmtx_timedlock:
        return rc;
}

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
