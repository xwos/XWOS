/**
 * @file
 * @brief 点对点通讯协议：协议层
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
#include <xwos/lib/string.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/crc32.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mutex.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/api.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 控制帧模板：同步帧
 */
__xwmd_rodata const xwu8_t xwpcp_cfrm_sync[] = {
        (xwu8_t)XWPCP_HWIFAL_SOF,
        (xwu8_t)0x15, /* 帧长度 */
        (xwu8_t)0xA8, /* 帧长度的镜像反转 */
        (xwu8_t)(XWPCP_QOS(1) | XWPCP_PORT_CMD), /* 端口:0，QoS:1 */
        (xwu8_t)XWPCP_ID_SYNC, /* 同步帧的id */
        (xwu8_t)'X', /* sdu[0]: protocol head 0 */
        (xwu8_t)'W', /* sdu[1]: protocol head 1 */
        (xwu8_t)'P', /* sdu[2]: protocol head 2 */
        (xwu8_t)'C', /* sdu[3]: protocol head 3 */
        (xwu8_t)'P', /* sdu[4]: protocol head 4 */
        (xwu8_t)XWPCP_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWPCP_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWPCP_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)XWPCP_SYNC_KEY, /* sdu[8]: 同步秘钥 */
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)0, /* sdu[9]: 发送计数器：第一字节（最高有效字节） */
        (xwu8_t)0, /* sdu[10]: 发送计数器：第二字节 */
        (xwu8_t)0, /* sdu[11]: 发送计数器：第三字节 */
        (xwu8_t)0, /* sdu[12]: 发送计数器：第四字节（第低有效字节） */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWPCP_HWIFAL_EOF,
};

/**
 * @brief 控制帧模板：同步应答帧
 */
__xwmd_rodata const xwu8_t xwpcp_cfrm_sync_ack[] = {
        (xwu8_t)XWPCP_HWIFAL_SOF,
        (xwu8_t)0x15, /* 帧长度 */
        (xwu8_t)0xA8, /* 帧长度的镜像反转 */
        (xwu8_t)(XWPCP_QOS(1) | XWPCP_PORT_CMD), /* 端口:0，QoS:1 */
        (xwu8_t)(XWPCP_ID_SYNC | XWPCP_ID_ACK), /* 同步应答帧的id */
        (xwu8_t)'X', /* sdu[0]: protocol head 0 */
        (xwu8_t)'W', /* sdu[1]: protocol head 1 */
        (xwu8_t)'P', /* sdu[2]: protocol head 2 */
        (xwu8_t)'C', /* sdu[3]: protocol head 3 */
        (xwu8_t)'P', /* sdu[4]: protocol head 4 */
        (xwu8_t)XWPCP_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWPCP_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWPCP_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)XWPCP_SYNC_KEY, /* sdu[8]: 同步应答帧的秘钥 */
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)0, /* sdu[9]: 接收计数器：第一字节（最高有效字节） */
        (xwu8_t)0, /* sdu[10]: 接收计数器：第二字节 */
        (xwu8_t)0, /* sdu[11]: 接收计数器：第三字节 */
        (xwu8_t)0, /* sdu[12]: 接收计数器：第四字节（第低有效字节） */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWPCP_HWIFAL_EOF,
};

/**
 * @brief 数据应答帧模板
 */
__xwmd_rodata const xwu8_t xwpcp_frm_sdu_ack[] = {
        (xwu8_t)XWPCP_HWIFAL_SOF,
        (xwu8_t)0x9, /* 帧长度 */
        (xwu8_t)0x90, /* 帧长度的镜像反转 */
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)XWPCP_QOS(1), /* 端口、Qos */
        (xwu8_t)XWPCP_ID_ACK, /* id | XWPCP_ID_ACK */
        (xwu8_t)0, /* sdu[0]: 应答 */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWPCP_HWIFAL_EOF,
};

/**
 * @brief 应答错误对照表
 */
__xwmd_rodata const xwer_t xwpcp_callback_rc[XWPCP_ACK_NUM] = {
        [XWPCP_ACK_OK] = XWOK,
        [XWPCP_ACK_ECONNRESET] = -ECONNRESET,
        [XWPCP_ACK_EALREADY] = -EALREADY,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
void xwpcp_txq_add_head(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
void xwpcp_txq_add_tail(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
xwer_t xwpcp_tx_cfrm_sync(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcp_tx_cfrm_sync_ack(struct xwpcp * xwpcp, xwu32_t rxcnt);

static __xwmd_code
xwer_t xwpcp_tx_frm_sdu_ack(struct xwpcp * xwpcp, xwu8_t port, xwu8_t id, xwu8_t ack);

static __xwmd_code
xwer_t xwpcp_chk_frm(struct xwpcp_frame * frm);

static __xwmd_code
void xwpcp_finish_tx(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
void xwpcp_rxq_pub(struct xwpcp * xwpcp,
                   struct xwpcp_frmslot * frmslot,
                   xwu8_t port);

static __xwmd_code
xwer_t xwpcp_rx_cfrm_sync(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
xwer_t xwpcp_rx_cfrm_sync_ack(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
xwer_t xwpcp_rx_frm_sdu(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
xwer_t xwpcp_rx_frm_sdu_ack(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
xwer_t xwpcp_rx_frm(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
xwer_t xwpcp_rxfsm(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcp_connect(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcp_tx_frm(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot);

static __xwmd_code
xwer_t xwpcp_txfsm(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcp_thrd_pause(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 抓住XWPCP，防止其被停止
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWPCP已经停止运行
 */
__xwmd_code
xwer_t xwpcp_grab(struct xwpcp * xwpcp)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq_t, &xwpcp->refcnt,
                                XWPCP_REFCNT_STARTED, 1,
                                NULL, NULL);
        return rc;
}

/**
 * @brief 减少XWPCP的引用计数
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWPCP没有在运行
 */
__xwmd_code
xwer_t xwpcp_put(struct xwpcp * xwpcp)
{
        xwer_t rc;

        rc = xwaop_tge_then_sub(xwsq_t, &xwpcp->refcnt,
                                XWPCP_REFCNT_INUSED, 1,
                                NULL, NULL);
        return rc;
}

/**
 * @brief 将待发送的帧槽加入到发送队列的头部
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 帧槽
 */
static __xwmd_code
void xwpcp_txq_add_head(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwu8_t prio;

        prio = frmslot->priority;
        xwosal_splk_lock(&xwpcp->txq.lock);
        xwlib_bclst_add_head(&xwpcp->txq.q[prio], &frmslot->node);
        if (!xwbmpop_t1i(xwpcp->txq.nebmp, (xwsq_t)prio)) {
                xwbmpop_s1i(xwpcp->txq.nebmp, (xwsq_t)prio);
        }/* else {} */
        xwosal_splk_unlock(&xwpcp->txq.lock);
}

/**
 * @brief 将待发送的帧槽加入到发送队列的尾部
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 帧槽
 */
static __xwmd_code
void xwpcp_txq_add_tail(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwu8_t prio;

        prio = frmslot->priority;
        xwosal_splk_lock(&xwpcp->txq.lock);
        xwlib_bclst_add_tail(&xwpcp->txq.q[prio], &frmslot->node);
        if (!xwbmpop_t1i(xwpcp->txq.nebmp, (xwsq_t)prio)) {
                xwbmpop_s1i(xwpcp->txq.nebmp, (xwsq_t)prio);
        }/* else {} */
        xwosal_splk_unlock(&xwpcp->txq.lock);
}

/**
 * @brief 从最高优先级的发送队列中选择一个待发送的帧槽
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 帧槽的指针
 */
__xwmd_code
struct xwpcp_frmslot * xwpcp_txq_choose(struct xwpcp * xwpcp)
{
        struct xwpcp_frmslot * frmslot;
        xwu8_t prio;

        xwosal_splk_lock(&xwpcp->txq.lock);
        prio = (xwu8_t)xwbmpop_fls(xwpcp->txq.nebmp, XWPCP_PRIORITY_NUM);
        frmslot = xwlib_bclst_first_entry(&xwpcp->txq.q[prio],
                                          struct xwpcp_frmslot,
                                          node);
        xwlib_bclst_del_init(&frmslot->node);
        if (xwlib_bclst_tst_empty(&xwpcp->txq.q[prio])) {
                xwbmpop_c0i(xwpcp->txq.nebmp, (xwsq_t)prio);
        }/* else {} */
        xwosal_splk_unlock(&xwpcp->txq.lock);
        return frmslot;
}

/**
 * @brief 发送同步帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_tx_cfrm_sync(struct xwpcp * xwpcp)
{
        xwu8_t stream[sizeof(xwpcp_cfrm_sync)];
        struct xwpcp_frame * frm;
        xwid_t csmtxid;
        xwu32_t txcnt;
        xwsz_t infolen;
        xwu32_t crc32;
        xwer_t rc;

        frm = (struct xwpcp_frame *)stream;
        xwaop_read(xwu32_t, &xwpcp->txq.cnt, &txcnt);
        if (XWPCP_ID_SYNC == XWPCP_ID(txcnt)) {
                xwpcplogf(DEBUG, "TX SYNC frame.\n");
                memcpy(frm, xwpcp_cfrm_sync, sizeof(xwpcp_cfrm_sync));
                frm->sdu[9] = (xwu8_t)((txcnt >> 24U) & 0xFFU);
                frm->sdu[10] = (xwu8_t)((txcnt >> 16U) & 0xFFU);
                frm->sdu[11] = (xwu8_t)((txcnt >> 8U) & 0xFFU);
                frm->sdu[12] = (xwu8_t)((txcnt >> 0U) & 0xFFU);
                infolen = frm->head.frmlen - XWPCP_CHKSUM_SIZE;
                crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
                frm->sdu[13] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
                frm->sdu[14] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
                frm->sdu[15] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
                frm->sdu[16] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
                csmtxid = xwosal_mtx_get_id(&xwpcp->txq.csmtx);
                rc = xwosal_mtx_lock(csmtxid);
                if (XWOK == rc) {
                        rc = xwpcp_hwifal_tx(xwpcp, frm);
                        xwosal_mtx_unlock(csmtxid);
                }/* else {} */
        } else {
                rc = -EALREADY;
        }
        return rc;
}

/**
 * @brief 发送同步应答帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param rxcnt: (I) 应答接收计数器
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_tx_cfrm_sync_ack(struct xwpcp * xwpcp, xwu32_t rxcnt)
{
        xwu8_t stream[sizeof(xwpcp_cfrm_sync_ack)];
        struct xwpcp_frame * frm;
        xwid_t csmtxid;
        xwsz_t infolen;
        xwu32_t crc32;
        xwer_t rc;

        xwpcplogf(DEBUG, "TX SYNC-ACK frame.\n");
        frm = (struct xwpcp_frame *)stream;
        memcpy(frm, xwpcp_cfrm_sync_ack, sizeof(xwpcp_cfrm_sync_ack));
        frm->sdu[9] = (xwu8_t)((rxcnt >> 24U) & 0xFFU);
        frm->sdu[10] = (xwu8_t)((rxcnt >> 16U) & 0xFFU);
        frm->sdu[11] = (xwu8_t)((rxcnt >> 8U) & 0xFFU);
        frm->sdu[12] = (xwu8_t)((rxcnt >> 0U) & 0xFFU);
        infolen = frm->head.frmlen - XWPCP_CHKSUM_SIZE;
        crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
        frm->sdu[13] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
        frm->sdu[14] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
        frm->sdu[15] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
        frm->sdu[16] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
        csmtxid = xwosal_mtx_get_id(&xwpcp->txq.csmtx);
        rc = xwosal_mtx_lock(csmtxid);
        if (XWOK == rc) {
                rc = xwpcp_hwifal_tx(xwpcp, frm);
                xwosal_mtx_unlock(csmtxid);
        }/* else {} */
        return rc;
}

/**
 * @brief 发送数据应答帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frm: (O) 指向缓冲区的指针，此缓冲区被用于格式化一帧
 * @param port: (I) 端口
 * @param id: (I) 应答远端消息的id
 * @param ack: (I) 应答
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_tx_frm_sdu_ack(struct xwpcp * xwpcp, xwu8_t port, xwu8_t id, xwu8_t ack)
{
        xwu8_t stream[sizeof(xwpcp_frm_sdu_ack)];
        struct xwpcp_frame * frm;
        xwu32_t crc32;
        xwsz_t infolen;
        xwid_t csmtxid;
        xwer_t rc;

        xwpcplogf(DEBUG, "ACK:0x%X, id:0x%X\n", ack, id);
        frm = (struct xwpcp_frame *)stream;
        memcpy(frm, xwpcp_frm_sdu_ack, sizeof(struct xwpcp_frmhead));
        frm->head.port = port | XWPCP_QOS(1);
        frm->head.id = id | XWPCP_ID_ACK;
        frm->sdu[0] = ack;
        infolen = frm->head.frmlen - XWPCP_CHKSUM_SIZE;
        crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
        frm->sdu[1] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
        frm->sdu[2] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
        frm->sdu[3] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
        frm->sdu[4] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
        csmtxid = xwosal_mtx_get_id(&xwpcp->txq.csmtx);
        rc = xwosal_mtx_lock(csmtxid);
        if (XWOK == rc) {
                rc = xwpcp_hwifal_tx(xwpcp, frm);
                xwosal_mtx_unlock(csmtxid);
        }/* else {} */
        return rc;
}

/**
 * @brief 在限定的时间内将报文格式化为数据帧并加入到发送队列，超时将返回错误码
 * @param xwpcp: (I) XWPCP对象的指针
 * @param msg: (I) 报文
 * @param prio: (I) 优先级
 * @param cbfunc: (I) 异步通知的回调函数
 * @param cbarg: (I) 调用异步通知回调函数时用户自定义的参数
 * @param fhdlbuf: (O) 指向缓冲区的指针，通过此缓冲区返回帧句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 帧槽被使用完了
 * @note
 * + 数据帧：
 *   - (xwu8_t)帧长度
 *   - (xwu8_t)帧长度的镜像反转
 *   - (xwu8_t)(端口 | QoS)
 *   - (xwu8_t)id
 *   - (xwu8_t)sdu[0 ~ msg->size - 1]: 报文数据
 *   - (xwu8_t)sdu[msg->size + 0]/CRC32 最高有效字节
 *   - (xwu8_t)sdu[msg->size + 1]/CRC32 第二字节
 *   - (xwu8_t)sdu[msg->size + 2]/CRC32 第三字节
 *   - (xwu8_t)sdu[msg->size + 3]/CRC32 最低有效字节
 */
__xwmd_code
xwer_t xwpcp_eq_msg(struct xwpcp * xwpcp,
                    const struct xwpcp_msg * msg,
                    xwu8_t prio,
                    xwpcp_ntf_f cbfunc, void * cbarg,
                    xwpcp_fhdl_t * fhdlbuf)
{
        struct xwpcp_frmslot * frmslot;
        xwsz_t need, neednum;
        xwssq_t odr;
        void * mem;
        xwer_t rc;

        /* 申请帧槽 */
        need = sizeof(struct xwpcp_frmslot) + msg->size +
               XWPCP_CHKSUM_SIZE + XWPCP_HWIFAL_EOF_SIZE;
        neednum = DIV_ROUND_UP(need, XWPCP_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWPCP_MEMBLK_SIZE << odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwpcp->slot.pool, (xwsq_t)odr, &mem);
        if (__unlikely(rc < 0)) {
                goto err_bma_alloc;
        }
        frmslot = mem;

        /* 设置发送信息 */
        frmslot->cbfunc = cbfunc;
        frmslot->cbarg = cbarg;
        xwlib_bclst_init_node(&frmslot->node);
        frmslot->priority = prio;

        /* 格式化帧 */
        xwpcplogf(DEBUG,
                  "frmslot(%p), prio:0x%X, msg->port:0x%X, msg->size:0x%X\n",
                  &frmslot, prio, msg->port, msg->size);
        frmslot->frm.sof = XWPCP_HWIFAL_SOF;
        frmslot->frm.head.frmlen = (xwu8_t)(msg->size + XWPCP_CHKSUM_SIZE +
                                            sizeof(struct xwpcp_frmhead));
        frmslot->frm.head.mirror = xwbop_rbit(xwu8_t, frmslot->frm.head.frmlen);
        frmslot->frm.head.port = msg->port | msg->qos;
        frmslot->frm.head.id = XWPCP_ID(0); /* 待发送时才能确定 */
        memcpy(&frmslot->frm.sdu[0], msg->text, msg->size);
        frmslot->frm.sdu[msg->size + XWPCP_CHKSUM_SIZE] = XWPCP_HWIFAL_EOF;

        /* 加入到发送队列 */
        xwpcp_txq_add_tail(xwpcp, frmslot);
        xwosal_smr_post(xwosal_smr_get_id(&xwpcp->txq.smr));

        if (fhdlbuf) {
                *fhdlbuf = (xwpcp_fhdl_t)frmslot;
        }/* else {} */
        return XWOK;

err_bma_alloc:
        return rc;
}

/**
 * @brief 校验帧是否合法
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frm: (I) 被校验的帧的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EBADMSG: 校验错误
 */
static __xwmd_code
xwer_t xwpcp_chk_frm(struct xwpcp_frame * frm)
{
        xwu8_t frmlen;
        xwu8_t mirror;
        xwu32_t crc32;
        xwu8_t * crc32_pos;
        xwsz_t infolen;
        xwer_t rc;

        frmlen = frm->head.frmlen;
        mirror = xwbop_rbit(xwu8_t, frmlen);

        if (__unlikely(frm->head.frmlen < XWPCP_FRM_MINSIZE)) {
                rc = -EBADMSG;
        } else if (mirror != frm->head.mirror) {
                rc = -EBADMSG;
        } else {
                infolen = (xwsz_t)frmlen - XWPCP_CHKSUM_SIZE;
                crc32_pos = &frm->sdu[infolen - sizeof(struct xwpcp_frmhead)];
                crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
                if ((((crc32 >> 24U) & 0xFFU) != crc32_pos[0]) ||
                    (((crc32 >> 16U) & 0xFFU) != crc32_pos[1]) ||
                    (((crc32 >> 8U) & 0xFFU) != crc32_pos[2]) ||
                    (((crc32 >> 0U) & 0xFFU) != crc32_pos[3])) {
                        rc = -EBADMSG;
                } else {
                        rc = XWOK;
                }
        }
        return rc;
}

/**
 * @brief 指示数据帧的发送状态
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 数据帧帧槽的指针
 */
static __xwmd_code
void xwpcp_finish_tx(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwu8_t rmtack;
        __maybe_unused xwu8_t rmtid;

        rmtack = xwpcp->txq.txi.remote.ack;
        rmtid = xwpcp->txq.txi.remote.id;
        xwpcplogf(DEBUG, "ACK:0x%X, id:0x%X\n", rmtack, rmtid);

        switch (rmtack) {
        case XWPCP_ACK_OK:
                xwaop_add(xwu32_t, &xwpcp->txq.cnt, 1, NULL, NULL);
                xwosal_splk_lock(&xwpcp->txq.ntflock);
                if (frmslot->cbfunc) {
                        frmslot->cbfunc(xwpcp, (xwpcp_fhdl_t)frmslot,
                                        xwpcp_callback_rc[XWPCP_ACK_OK],
                                        frmslot->cbarg);
                }/* else {} */
                xwosal_splk_unlock(&xwpcp->txq.ntflock);
                xwmm_bma_free(xwpcp->slot.pool, frmslot);
                break;
        case XWPCP_ACK_EALREADY:
                xwpcplogf(DEBUG, "Msg is already TX-ed!\n");
                xwaop_add(xwu32_t, &xwpcp->txq.cnt, 1, NULL, NULL);
                xwosal_splk_lock(&xwpcp->txq.ntflock);
                if (frmslot->cbfunc) {
                        frmslot->cbfunc(xwpcp, (xwpcp_fhdl_t)frmslot,
                                        xwpcp_callback_rc[XWPCP_ACK_OK],
                                        frmslot->cbarg);
                }/* else {} */
                xwosal_splk_unlock(&xwpcp->txq.ntflock);
                xwmm_bma_free(xwpcp->slot.pool, frmslot);
                break;
        case XWPCP_ACK_ECONNRESET:
                xwpcplogf(WARNING, "Link has been severed!\n");
                xwaop_c0m(xwu32_t, &xwpcp->txq.cnt, XWPCP_ID_MSK, NULL, NULL);
                xwpcp->txq.tmp = frmslot;
                break;
        default:
                xwpcplogf(ALERT, "ACK:0x%X, frmslot:%p\n", rmtack, frmslot);
                XWPCP_BUG();
                xwpcp->txq.tmp = frmslot;
                break;
        }
}

/**
 * @brief 发布数据帧到接收队列
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 待发布的帧槽指针
 * @param port: (I) 端口值
 */
static __xwmd_code
void xwpcp_rxq_pub(struct xwpcp * xwpcp,
                   struct xwpcp_frmslot * frmslot,
                   xwu8_t port)
{
        xwpcplogf(DEBUG,
                  "Publish a frame(%p): port:0x%X, frmlen:0x%u\n",
                  frmslot, port, frmslot->frm.head.frmlen);
        xwosal_splk_lock(&xwpcp->rxq.lock[port]);
        xwlib_bclst_add_tail(&xwpcp->rxq.q[port], &frmslot->node);
        xwosal_splk_unlock(&xwpcp->rxq.lock[port]);
        xwosal_smr_post(xwosal_smr_get_id(&xwpcp->rxq.smr[port]));
}

/**
 * @brief 从接收队列中选择一条数据帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param port: (I) 端口
 * @return 帧槽的指针
 */
__xwmd_code
struct xwpcp_frmslot * xwpcp_rxq_choose(struct xwpcp * xwpcp, xwu8_t port)
{
        struct xwpcp_frmslot * frmslot;

        xwosal_splk_lock(&xwpcp->rxq.lock[port]);
        frmslot = xwlib_bclst_first_entry(&xwpcp->rxq.q[port],
                                          struct xwpcp_frmslot,
                                          node);
        xwlib_bclst_del_init(&frmslot->node);
        xwosal_splk_unlock(&xwpcp->rxq.lock[port]);
        return frmslot;
}

/**
 * @brief 接收同步帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_cfrm_sync(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwer_t rc;
        xwu32_t rmttxcnt;
        char proto[6];

        proto[0] = frmslot->frm.sdu[0];
        proto[1] = frmslot->frm.sdu[1];
        proto[2] = frmslot->frm.sdu[2];
        proto[3] = frmslot->frm.sdu[3];
        proto[4] = frmslot->frm.sdu[4];
        proto[5] = '\0';
        xwpcplogf(DEBUG, "proto:%s-%d.%d.%d, key:0x%X\n",
                  proto,
                  frmslot->frm.sdu[5], frmslot->frm.sdu[6], frmslot->frm.sdu[7],
                  frmslot->frm.sdu[8]);
        if (((XWPCP_PORT_CMD | XWPCP_QOS(1)) == frmslot->frm.head.port) &&
            (0 == strcmp(proto, "XWPCP")) &&
            (XWPCP_VERSION_MAJOR == frmslot->frm.sdu[5]) &&
            (XWPCP_VERSION_MINOR == frmslot->frm.sdu[6]) &&
            (XWPCP_VERSION_REVISION == frmslot->frm.sdu[7]) &&
            (XWPCP_SYNC_KEY == frmslot->frm.sdu[8])) {
                rmttxcnt = ((xwu32_t)frmslot->frm.sdu[9]) << 24U;
                rmttxcnt |= ((xwu32_t)frmslot->frm.sdu[10]) << 16U;
                rmttxcnt |= ((xwu32_t)frmslot->frm.sdu[11]) << 8U;
                rmttxcnt |= ((xwu32_t)frmslot->frm.sdu[12]) << 0U;
                rc = xwpcp_tx_cfrm_sync_ack(xwpcp, rmttxcnt);
                if (XWOK == rc) {
                        xwaop_write(xwu32_t, &xwpcp->rxq.cnt, rmttxcnt + 1, NULL);
                }/* else {} */
        } else {
                rc = -EPERM;
        }
        xwmm_bma_free(xwpcp->slot.pool, frmslot);
        return rc;
}

/**
 * @brief 接收同步应答帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_cfrm_sync_ack(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwu32_t txcnt;
        xwu32_t rmtrxcnt;
        char proto[6];

        proto[0] = frmslot->frm.sdu[0];
        proto[1] = frmslot->frm.sdu[1];
        proto[2] = frmslot->frm.sdu[2];
        proto[3] = frmslot->frm.sdu[3];
        proto[4] = frmslot->frm.sdu[4];
        proto[5] = '\0';

        rmtrxcnt = ((xwu32_t)frmslot->frm.sdu[9]) << 24U;
        rmtrxcnt |= ((xwu32_t)frmslot->frm.sdu[10]) << 16U;
        rmtrxcnt |= ((xwu32_t)frmslot->frm.sdu[11]) << 8U;
        rmtrxcnt |= ((xwu32_t)frmslot->frm.sdu[12]) << 0U;
        xwaop_read(xwu32_t, &xwpcp->txq.cnt, &txcnt);

        xwpcplogf(DEBUG, "proto:%s-%d.%d.%d, key:0x%X, remote:%d, local:%d\n",
                  proto,
                  frmslot->frm.sdu[5], frmslot->frm.sdu[6], frmslot->frm.sdu[7],
                  frmslot->frm.sdu[8],
                  rmtrxcnt, txcnt);

        if (((XWPCP_PORT_CMD | XWPCP_QOS(1)) == frmslot->frm.head.port) &&
            (0 == strcmp(proto, "XWPCP")) &&
            (XWPCP_VERSION_MAJOR == frmslot->frm.sdu[5]) &&
            (XWPCP_VERSION_MINOR == frmslot->frm.sdu[6]) &&
            (XWPCP_VERSION_REVISION == frmslot->frm.sdu[7]) &&
            (XWPCP_SYNC_KEY == frmslot->frm.sdu[8]) &&
            (rmtrxcnt == txcnt)) {
                xwaop_add(xwu32_t, &xwpcp->txq.cnt, 1, NULL, NULL);
                xwpcp_hwifal_notify(xwpcp, XWPCP_HWIFNTF_CONNECT);
        }/* else {} */
        xwmm_bma_free(xwpcp->slot.pool, frmslot);
        return XWOK;
}

/**
 * @brief 接收数据帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 * @note
 * + 数据帧：
 *   - (xwu8_t)帧长度
 *   - (xwu8_t)帧长度的镜像反转
 *   - (xwu8_t)目的端口 | QOS
 *   - (xwu8_t)id
 *   - (xwu8_t)sdu[0 ~ msg->size - 1]: 报文数据
 *   - (xwu8_t)sdu[msg->size + 0]/CRC32 最高有效字节
 *   - (xwu8_t)sdu[msg->size + 1]/CRC32 第二字节
 *   - (xwu8_t)sdu[msg->size + 2]/CRC32 第三字节
 *   - (xwu8_t)sdu[msg->size + 3]/CRC32 最低有效字节
 */
static __xwmd_code
xwer_t xwpcp_rx_frm_sdu(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwu32_t rxcnt;
        xwu8_t rmtid, lclid;
        xwu8_t port, qos;
        xwer_t rc;

        rc = XWOK;
        xwaop_read(xwu32_t, &xwpcp->rxq.cnt, &rxcnt);
        lclid = XWPCP_ID(rxcnt);
        rmtid = frmslot->frm.head.id;
        port = XWPCP_PORT(frmslot->frm.head.port);
        qos = XWPCP_PORT_QOS(frmslot->frm.head.port);
        xwpcplogf(DEBUG,
                  "port:0x%X, qos:0x%X, frmlen:0x%X, remote id:0x%X, local id:0x%X\n",
                  port, qos, frmslot->frm.head.frmlen, rmtid, lclid);
        if (XWPCP_QOS(1) == qos) {
                if (__likely(rmtid == lclid)) {
                        /* 收到报文 */
                        rc = xwpcp_tx_frm_sdu_ack(xwpcp, port, rmtid, XWPCP_ACK_OK);
                        if (XWOK == rc) {
                                xwaop_add(xwu32_t, &xwpcp->rxq.cnt, 1, NULL, NULL);
                                xwpcp_rxq_pub(xwpcp, frmslot, port);
                        } else {
                                xwmm_bma_free(xwpcp->slot.pool, frmslot);
                        }
                } else if (XWPCP_ID(rmtid + 1) == lclid) {
                        /* 收到重复的报文 */
                        rc = xwpcp_tx_frm_sdu_ack(xwpcp, port, rmtid,
                                                  XWPCP_ACK_EALREADY);
                        xwmm_bma_free(xwpcp->slot.pool, frmslot);
                } else {
                        /* 链接被复位 */
                        rc = xwpcp_tx_frm_sdu_ack(xwpcp, port, rmtid,
                                                  XWPCP_ACK_ECONNRESET);
                        xwmm_bma_free(xwpcp->slot.pool, frmslot);
                }
        } else {
                xwpcp_rxq_pub(xwpcp, frmslot, port);
        }
        return rc;
}

/**
 * @brief 接收数据应答帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_frm_sdu_ack(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwid_t csmtxid;
        xwid_t cscdtid;
        xwu8_t rmtid;
        xwu8_t ack;
        xwer_t rc;
        xwsq_t hwifst;

        csmtxid = xwosal_mtx_get_id(&xwpcp->txq.csmtx);
        cscdtid = xwosal_cdt_get_id(&xwpcp->txq.cscdt);
        rmtid = XWPCP_ID(frmslot->frm.head.id);
        ack = frmslot->frm.sdu[0];
        xwpcplogf(DEBUG, "id:0x%X, ACK:0x%X\n", rmtid, ack);
        rc = xwosal_mtx_lock(csmtxid);
        if (__unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        xwaop_read(xwsq_t, &xwpcp->hwifst, &hwifst);
        if (XWPCP_HWIFST_TX & hwifst) {
                /* xwpcp->txq.txi.remote在锁外的函数xwpcp_finish_tx()中被使用，
                   此函数在TX线程中运行，而TX线程可能此时正运行在另一个CPU上。
                   因此程序必须保证xwpcp->txq.txi.remote对其他CPU的生效是在清除状态位
                   XWPCP_HWIFST_TX之前。由于原子操作带有内存屏障的语义，
                   这里代码的顺序不能随意调整。*/
                xwpcp->txq.txi.remote.ack = ack;
                xwpcp->txq.txi.remote.id = rmtid;
                xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX, NULL, NULL);
                xwosal_mtx_unlock(csmtxid);
                xwosal_cdt_unicast(cscdtid);
        } else {
                xwosal_mtx_unlock(csmtxid);
                xwpcplogf(ERR, "There is no sending frame!\n");
        }
err_mtx_lock:
        xwmm_bma_free(xwpcp->slot.pool, frmslot);
        return rc;
}

/**
 * @brief 接收帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_frm(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwer_t rc;
        xwu8_t id;

        id = frmslot->frm.head.id;
        xwpcplogf(DEBUG, "frame id:0x%X\n", id);
        switch (id) {
        case XWPCP_ID_SYNC:
                rc = xwpcp_rx_cfrm_sync(xwpcp, frmslot);
                break;
        case (XWPCP_ID_SYNC | XWPCP_ID_ACK):
                rc = xwpcp_rx_cfrm_sync_ack(xwpcp, frmslot);
                break;
        default:
                if (XWPCP_ID_ACK & id) {
                        rc = xwpcp_rx_frm_sdu_ack(xwpcp, frmslot);
                } else {
                        rc = xwpcp_rx_frm_sdu(xwpcp, frmslot);
                }
                break;
        }
        return rc;
}

/**
 * @brief XWPCP的接收线程状态机
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rxfsm(struct xwpcp * xwpcp)
{
        struct xwpcp_frmslot * frmslot;
        xwer_t rc;

        do {
                rc = xwpcp_hwifal_rx(xwpcp, &frmslot);
                if (XWOK == rc) {
                        rc = xwpcp_chk_frm(&frmslot->frm);
                }
        } while ((-EAGAIN == rc) || (-EBADMSG == rc));
        if (XWOK == rc) {
                rc = xwpcp_rx_frm(xwpcp, frmslot);
        }
        return rc;
}

/**
 * @brief XWPCP的接收线程
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwpcp_rxthrd(struct xwpcp * xwpcp)
{
        xwer_t rc;

        rc = XWOK;
        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        xwpcplogf(DEBUG, "Start freezing ...\n");
                        rc = xwosal_cthrd_freeze();
                        if (__unlikely(rc < 0)) {
                                xwpcplogf(DEBUG, "Failed to freeze ... rc: %d\n", rc);
                                xwpcp_thrd_pause();
                        }/* else {} */
                        xwpcplogf(DEBUG, "Resuming ...\n");
                } else {
                        rc = xwpcp_rxfsm(xwpcp);
                        if (XWOK == rc) {
                        } else if (-ETIMEDOUT == rc) {
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                xwpcplogf(DEBUG, "Interrupted ... [rc:%d]\n", rc);
                        } else if (-EBADMSG == rc) {
                                xwpcplogf(WARNING, "Bad frame! \n");
                        } else {
                                xwpcplogf(ALERT, "xwpcp_rxfsm() returns %d.\n", rc);
                                xwosal_cthrd_wait_exit();
                                break;
                        }
                }
        }
        return rc;
}

/**
 * @brief 链接远端
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_connect(struct xwpcp * xwpcp)
{
        xwtm_t xwtm;
        xwsz_t cnt;
        xwer_t rc;

        cnt = 0;
        rc = XWOK;
        do {
                rc = xwpcp_tx_cfrm_sync(xwpcp);
                if (XWOK == rc) {
                        xwtm = XWPCP_RETRY_PERIOD;
                        rc = xwosal_cthrd_sleep(&xwtm);
                        if (__unlikely(rc < 0)) {
                                break;
                        }
                        cnt++;
                } else {
                        break;
                }
        } while (cnt < XWPCP_RETRY_NUM);
        if ((XWOK == rc) && (XWPCP_RETRY_NUM == cnt)) {
                xwpcp_hwifal_notify(xwpcp, XWPCP_HWIFNTF_NETUNREACH);
                xwpcp_thrd_pause();
                xwpcp_thrd_pause();
                xwpcp_thrd_pause();
        }/* else {} */
        return rc;
}

/**
 * @brief 发送帧
 * @param xwpcp: (I) XWPCP对象的指针
 * @param frmslot: (I) 待发送的帧的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_tx_frm(struct xwpcp * xwpcp, struct xwpcp_frmslot * frmslot)
{
        xwu8_t id;
        xwu8_t qos;
        xwu32_t crc32;
        xwsz_t infolen;
        xwu8_t * crc32pos;
        xwtm_t xwtm;
        xwsz_t cnt;
        xwsq_t lockstate;
        xwid_t cscdtid;
        union xwlk_ulock ulk;
        xwu32_t txcnt;
        xwsq_t hwifst;
        xwer_t rc;

        cscdtid = xwosal_cdt_get_id(&xwpcp->txq.cscdt);
        ulk.osal.id = xwosal_mtx_get_id(&xwpcp->txq.csmtx);
        xwaop_read(xwu32_t, &xwpcp->txq.cnt, &txcnt);
        id = XWPCP_ID(txcnt);
        frmslot->frm.head.id = id;
        qos = XWPCP_PORT_QOS(frmslot->frm.head.port);
        infolen = frmslot->frm.head.frmlen - XWPCP_CHKSUM_SIZE;
        crc32pos = ((xwu8_t *)&frmslot->frm.head) + infolen;
        crc32 = xwlib_crc32_calms((xwu8_t *)&frmslot->frm.head, infolen);
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
        cnt = 0;
        rc = xwosal_mtx_lock(ulk.osal.id);
        if (__unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        if (XWPCP_QOS(1) == qos) {
                xwpcp->txq.txi.sender = frmslot;
                xwaop_s1m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX, NULL, NULL);
                do {
                        xwpcplogf(DEBUG,
                                  "frmslot(%p), local id:0x%X, cnt:0x%X\n",
                                  frmslot, id, cnt);
                        rc = xwpcp_hwifal_tx(xwpcp, &frmslot->frm);
                        if (__unlikely(rc < 0)) {
                                xwpcp->txq.txi.sender = NULL;
                                xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX,
                                          NULL, NULL);
                                xwosal_mtx_unlock(ulk.osal.id);
                                goto err_if_tx;
                        }
                        xwtm = XWPCP_RETRY_PERIOD;
                        rc = xwosal_cdt_timedwait(cscdtid,
                                                  ulk, XWLK_TYPE_MTX, NULL,
                                                  &xwtm, &lockstate);
                        if (XWOK == rc) {
                                xwosal_mtx_unlock(ulk.osal.id);
                                /* 互斥锁有内存屏障的语义，
                                   无锁访问xwpcp->txq.txi.remote */
                                xwpcp_finish_tx(xwpcp, frmslot);
                                break;
                        } else if (-ETIMEDOUT == rc) {
                                rc = xwosal_mtx_lock(ulk.osal.id);
                                if (__unlikely(rc < 0)) {
                                        /* 原子操作带有内存屏障的语义，
                                           下面语句顺序不能调换 */
                                        xwpcp->txq.txi.sender = NULL;
                                        xwaop_c0m(xwsq_t, &xwpcp->hwifst,
                                                  XWPCP_HWIFST_TX, NULL, NULL);
                                        goto err_mtx_lock;
                                }
                                xwaop_read(xwsq_t, &xwpcp->hwifst, &hwifst);
                                if (XWPCP_HWIFST_TX & hwifst) {
                                        cnt++;
                                        rc = -ETIMEDOUT;
                                } else {
                                        xwosal_mtx_unlock(ulk.osal.id);
                                        /* 在判断状态位XWPCP_HWIFST_TX之后
                                           无锁访问xwpcp->txq.txi.remote */
                                        xwpcp_finish_tx(xwpcp, frmslot);
                                        rc = XWOK;
                                        break;
                                }
                        } else {
                                xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX,
                                          NULL, NULL);
                                if (XWLK_STATE_LOCKED == lockstate) {
                                        xwosal_mtx_unlock(ulk.osal.id);
                                }/* else {} */
                                break;
                        }
                } while (cnt < XWPCP_RETRY_NUM);
                if ((XWPCP_RETRY_NUM == cnt) && (-ETIMEDOUT == rc)) {
                        xwaop_c0m(xwu32_t, &xwpcp->txq.cnt, XWPCP_ID_MSK,
                                  NULL, NULL);
                        xwpcp->txq.txi.sender = NULL;
                        xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX,
                                  NULL, NULL);
                        xwosal_mtx_unlock(ulk.osal.id);
                        xwpcp_thrd_pause();
                        xwpcp_thrd_pause();
                        xwpcp_thrd_pause();
                }
        } else {
                rc = xwpcp_hwifal_tx(xwpcp, &frmslot->frm);
                if (__unlikely(rc < 0)) {
                        xwosal_mtx_unlock(ulk.osal.id);
                        goto err_if_tx;
                }
        }

err_if_tx:
err_mtx_lock:
        return rc;
}

/**
 * @brief XWPCP的发送线程状态机
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_txfsm(struct xwpcp * xwpcp)
{
        struct xwpcp_frmslot * sender;
        xwu32_t txcnt;
        xwer_t rc;

        xwaop_read(xwu32_t, &xwpcp->txq.cnt, &txcnt);
        if (XWPCP_ID_SYNC == XWPCP_ID(txcnt)) {
                /* 链接远端 */
                rc = xwpcp_connect(xwpcp);
        } else {
                /* 选择一个待发送的帧 */
                if (xwpcp->txq.tmp) {
                        sender = xwpcp->txq.tmp;
                        xwpcp->txq.tmp = NULL;
                } else {
                        rc = xwosal_smr_wait(xwosal_smr_get_id(&xwpcp->txq.smr));
                        if (__unlikely(rc < 0)) {
                                goto err_txqsmr_wait;
                        }
                        sender = xwpcp_txq_choose(xwpcp);
                        XWPCP_BUG_ON(is_err(sender));
                }
                xwpcplogf(DEBUG,
                          "Choose frmslot(%p), frmlen:0x%X, port:0x%X\n",
                          sender,
                          sender->frm.head.frmlen,
                          sender->frm.head.port);

                rc = xwpcp_tx_frm(xwpcp, sender);
                if (__unlikely(rc < 0)) {
                        goto err_tx_frm;
                }
        }
        return XWOK;

err_tx_frm:
        xwpcp->txq.tmp = sender;
err_txqsmr_wait:
        return rc;
}

/**
 * @brief XWPCP的发送线程
 * @param xwpcp: (I) XWPCP对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwpcp_txthrd(struct xwpcp * xwpcp)
{
        xwer_t rc;

        rc = XWOK;
        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        xwpcplogf(DEBUG, "Start freezing ...\n");
                        rc = xwosal_cthrd_freeze();
                        if (__unlikely(rc < 0)) {
                                xwpcplogf(DEBUG, "Failed to freeze ... rc: %d\n", rc);
                                xwpcp_thrd_pause();
                        }
                        xwpcplogf(DEBUG, "Resuming ...\n");
                } else {
                        rc = xwpcp_txfsm(xwpcp);
                        if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                xwpcplogf(DEBUG, "Interrupted ... rc: %d\n", rc);
                        } else if (-EOVERFLOW == rc) {
                                xwpcplogf(DEBUG, "Buffer of HWIF is overflow!\n");
                                xwpcp_thrd_pause();
                        } else if ((-ETIMEDOUT != rc) && (XWOK != rc)) {
                                xwpcplogf(ALERT, "xwpcp_txfsm() ... rc: %d.\n", rc);
                                xwosal_cthrd_wait_exit();
                                break;
                        }/* else {} */
                }
        }
        if (xwpcp->txq.tmp) {
                xwpcp_txq_add_head(xwpcp, xwpcp->txq.tmp);
                xwpcp->txq.tmp = NULL;
                xwosal_smr_post(xwosal_smr_get_id(&xwpcp->txq.smr));
        }/* else {} */
        return rc;
}

/**
 * @brief 暂停一下XWPCP的线程
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_thrd_pause(void)
{
        xwtm_t sleep;

        sleep = XWPCP_RETRY_PERIOD;
        return xwosal_cthrd_sleep(&sleep);
}
