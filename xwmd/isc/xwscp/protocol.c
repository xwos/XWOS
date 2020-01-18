/**
 * @file
 * @brief 精简的点对点通讯协议：协议层
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/crc32.h>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 同步帧的模板
 */
__xwmd_rodata const xwu8_t xwscp_cfrm_sync[] = {
        (xwu8_t)SOSCP_HWIFAL_SOF,
        (xwu8_t)0xD, /* 帧长度 */
        (xwu8_t)0xB0, /* 帧长度的镜像反转 */
        (xwu8_t)SOSCP_PORT_CMD | SOSCP_QOS(1), /* 端口:0，QoS:1 */
        (xwu8_t)SOSCP_ID_SYNC, /* 同步帧的id */
        (xwu8_t)SOSCP_SYNC_KEY, /* sdu[0]: 同步帧的秘钥*/
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)0, /* sdu[1]: 发送计数器：第一字节（最高有效字节） */
        (xwu8_t)0, /* sdu[2]: 发送计数器：第二字节 */
        (xwu8_t)0, /* sdu[3]: 发送计数器：第三字节 */
        (xwu8_t)0, /* sdu[4]: 发送计数器：第四字节（第低有效字节） */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)SOSCP_HWIFAL_EOF,
};

/**
 * @brief 控制帧模板：同步应答帧
 */
__xwmd_rodata const xwu8_t xwscp_cfrm_syncack[] = {
        (xwu8_t)SOSCP_HWIFAL_SOF,
        (xwu8_t)0xD, /* 帧长度 */
        (xwu8_t)0xB0, /* 帧长度的镜像反转 */
        (xwu8_t)SOSCP_PORT_CMD | SOSCP_QOS(1), /* 端口:0，QoS:1 */
        (xwu8_t)(SOSCP_ID_SYNC | SOSCP_ID_ACK), /* 同步应答帧的id */
        (xwu8_t)SOSCP_SYNC_KEY, /* sdu[0]: 同步应答帧的秘钥 */
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)0, /* sdu[1]: 接收计数器：第一字节（最高有效字节） */
        (xwu8_t)0, /* sdu[2]: 接收计数器：第二字节 */
        (xwu8_t)0, /* sdu[3]: 接收计数器：第三字节 */
        (xwu8_t)0, /* sdu[4]: 接收计数器：第四字节（第低有效字节） */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)SOSCP_HWIFAL_EOF,
};

/**
 * @brief 数据应答帧模板
 */
__xwmd_rodata const xwu8_t xwscp_frm_sdu_ack[] = {
        (xwu8_t)SOSCP_HWIFAL_SOF,
        (xwu8_t)0x9, /* 帧长度 */
        (xwu8_t)0x90, /* 帧长度的镜像反转 */
        (xwu8_t)SOSCP_PORT_DATA | SOSCP_QOS(1), /* DATA端口与Qos(1) */
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)SOSCP_ID_ACK, /* id | SOSCP_ID_ACK */
        (xwu8_t)0, /* sdu[0]: 应答 */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)SOSCP_HWIFAL_EOF,
};

__xwmd_rodata const xwer_t xwscp_callback_rc[SOSCP_ACK_NUM] = {
        [SOSCP_ACK_OK] = OK,
        [SOSCP_ACK_ESIZE] = -EMSGSIZE,
        [SOSCP_ACK_EALREADY] = -EALREADY,
        [SOSCP_ACK_ECONNRESET] = -ECONNRESET,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
xwer_t xwscp_tx_cfrm_syncack(struct xwscp * xwscp, xwu32_t rxcnt);

static __xwmd_code
xwer_t xwscp_tx_sdu_ack(struct xwscp * xwscp, xwu8_t rxsize,
                        xwu8_t id, xwu8_t ack);

static __xwmd_code
xwer_t xwscp_chk_frm(struct xwscp_frame * frm);

static __xwmd_code
xwer_t xwscp_rx_cfrm_sync(struct xwscp * xwscp, struct xwscp_frmslot * frmslot);

static __xwmd_code
xwer_t xwscp_rx_cfrm_syncack(struct xwscp * xwscp, struct xwscp_frmslot * frmslot);

static __xwmd_code
xwer_t xwscp_rx_frm_sdu_ack(struct xwscp * xwscp, struct xwscp_frmslot * frmslot);

static __xwmd_code
xwer_t xwscp_rx_frm_sdu(struct xwscp * xwscp, struct xwscp_frmslot * frmslot);

static __xwmd_code
xwer_t xwscp_rx_frm(struct xwscp * xwscp, struct xwscp_frmslot * frmslot);

static __xwmd_code
xwer_t xwscp_fsm(struct xwscp * xwscp);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 申请一个帧槽
 * @param xwscp: (I) SOSCP对象的指针
 * @param slotbuf: (O) 指向缓冲区的指针，通过此缓冲区返回帧槽指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_alloc_frmslot(struct xwscp * xwscp, struct xwscp_frmslot ** slotbuf)
{
        xwer_t rc;
        struct xwscp_frmslot * frmslot;
        xwid_t smrid;

        smrid = xwosal_smr_get_id(&xwscp->slot.smr);
        rc = xwosal_smr_wait(smrid);
        if (__unlikely(rc < 0)) {
                goto err_slotsmr_wait;
        }
        xwosal_splk_lock(&xwscp->slot.lock);
        frmslot = xwlib_bclst_first_entry(&xwscp->slot.q, struct xwscp_frmslot, node);
        xwlib_bclst_del_init(&frmslot->node);
        xwosal_splk_unlock(&xwscp->slot.lock);
        *slotbuf = frmslot;

err_slotsmr_wait:
        return rc;
}

/**
 * @brief 释放一个帧槽
 * @param xwscp: (I) SOSCP对象的指针
 * @param frmslot: (I) 帧槽指针
 */
__xwmd_code
void xwscp_free_frmslot(struct xwscp * xwscp, struct xwscp_frmslot * frmslot)
{
        xwid_t smrid;

        xwosal_splk_lock(&xwscp->slot.lock);
        xwlib_bclst_add_tail(&xwscp->slot.q, &frmslot->node);
        xwosal_splk_unlock(&xwscp->slot.lock);
        smrid = xwosal_smr_get_id(&xwscp->slot.smr);
        xwosal_smr_post(smrid);
}

/**
 * @brief 发送同步帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @note
 * + 同步帧：
 *   - (xwu8_t)0xD, 帧长度
 *   - (xwu8_t)0xB0, 帧长度的镜像反转
 *   - (xwu8_t)(SOSCP_PORT_CMD | SOSCP_QOS(1)), 端口:SOSCP_PORT_CMD，QoS:1
 *   - (xwu8_t)SOSCP_ID_SYNC, 同步帧的id
 *   - (xwu8_t)SOSCP_SYNC_KEY, sdu[0], 同步帧的秘钥
 *   - (xwu8_t)sdu[1], 发送计数器第一字节（最高有效字节）
 *   - (xwu8_t)sdu[2], 发送计数器第二字节
 *   - (xwu8_t)sdu[3], 发送计数器第三字节
 *   - (xwu8_t)sdu[4], 发送计数器第四字节（第低有效字节）
 *   - (xwu8_t)CRC32 第一字节（最高有效字节）
 *   - (xwu8_t)CRC32 第二字节
 *   - (xwu8_t)CRC32 第三字节
 *   - (xwu8_t)CRC32 第四字节（第低有效字节）
 */
__xwmd_code
xwer_t xwscp_tx_cfrm_sync(struct xwscp * xwscp, xwtm_t * xwtm)
{
        xwu8_t stream[sizeof(xwscp_cfrm_sync)];
        struct xwscp_frame * frm;
        xwid_t csmtxid;
        xwu32_t txcnt;
        xwsz_t infolen;
        xwu32_t crc32;
        xwer_t rc;

        frm = (struct xwscp_frame *)stream;
        xwaop_read(xwu32_t, &xwscp->txi.cnt, &txcnt);
        if (SOSCP_ID_SYNC == SOSCP_ID(txcnt)) {
                xwscplogf(DEBUG, "TX SYNC frame.\n");
                memcpy(frm, xwscp_cfrm_sync, sizeof(xwscp_cfrm_sync));
                frm->sdu[1] = (xwu8_t)((txcnt >> 24U) & 0xFFU);
                frm->sdu[2] = (xwu8_t)((txcnt >> 16U) & 0xFFU);
                frm->sdu[3] = (xwu8_t)((txcnt >> 8U) & 0xFFU);
                frm->sdu[4] = (xwu8_t)((txcnt >> 0U) & 0xFFU);
                infolen = frm->head.frmlen - SOSCP_CHKSUM_SIZE;
                crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
                frm->sdu[5] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
                frm->sdu[6] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
                frm->sdu[7] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
                frm->sdu[8] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
                csmtxid = xwosal_mtx_get_id(&xwscp->csmtx);
                rc = xwosal_mtx_lock(csmtxid);
                if (OK == rc) {
                        rc = xwscp_hwifal_tx(xwscp, frm, xwtm);
                        xwosal_mtx_unlock(csmtxid);
                }/* else {} */
        } else {
                rc = -EALREADY;
        }
        return rc;
}

/**
 * @brief 发送同步应答帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param rxcnt: (I) 应答接收计数器
 * @return 错误码
 * @note
 * + 同步应答帧：
 *   - (xwu8_t)0xD, 帧长度
 *   - (xwu8_t)0xB0, 帧长度的镜像反转
 *   - (xwu8_t)(SOSCP_PORT_CMD | SOSCP_QOS(1)), 端口:SOSCP_PORT_CMD，QoS:1
 *   - (xwu8_t)(SOSCP_ID_SYNC | SOSCP_ID_ACK), 同步应答帧的id
 *   - (xwu8_t)SOSCP_SYNC_KEY, sdu[0], 同步应答帧的秘钥
 *   - (xwu8_t)sdu[1], 接收计数器：第一字节（最高有效字节）
 *   - (xwu8_t)sdu[2], 接收计数器：第二字节
 *   - (xwu8_t)sdu[3], 接收计数器：第三字节
 *   - (xwu8_t)sdu[4], 接收计数器：第四字节（第低有效字节）
 *   - (xwu8_t)CRC32 第一字节（最高有效字节）
 *   - (xwu8_t)CRC32 第二字节
 *   - (xwu8_t)CRC32 第三字节
 *   - (xwu8_t)CRC32 第四字节（第低有效字节）
 */
static __xwmd_code
xwer_t xwscp_tx_cfrm_syncack(struct xwscp * xwscp, xwu32_t rxcnt)
{
        xwu8_t stream[sizeof(xwscp_cfrm_syncack)];
        struct xwscp_frame * frm;
        xwtm_t xwtm;
        xwid_t csmtxid;
        xwsz_t infolen;
        xwu32_t crc32;
        xwer_t rc;

        xwscplogf(DEBUG, "TX SYNC-ACK frame.\n");
        frm = (struct xwscp_frame *)stream;
        memcpy(frm, xwscp_cfrm_syncack, sizeof(xwscp_cfrm_syncack));
        frm->sdu[1] = (xwu8_t)((rxcnt >> 24U) & 0xFFU);
        frm->sdu[2] = (xwu8_t)((rxcnt >> 16U) & 0xFFU);
        frm->sdu[3] = (xwu8_t)((rxcnt >> 8U) & 0xFFU);
        frm->sdu[4] = (xwu8_t)((rxcnt >> 0U) & 0xFFU);
        infolen = frm->head.frmlen - SOSCP_CHKSUM_SIZE;
        crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
        frm->sdu[5] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
        frm->sdu[6] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
        frm->sdu[7] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
        frm->sdu[8] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
        csmtxid = xwosal_mtx_get_id(&xwscp->csmtx);
        rc = xwosal_mtx_lock(csmtxid);
        if (OK == rc) {
                xwtm = XWTM_MAX;
                rc = xwscp_hwifal_tx(xwscp, frm, &xwtm);
                xwosal_mtx_unlock(csmtxid);
        }/* else {} */
        return rc;
}

/**
 * @brief 发送数据应答帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param rxsize: (I) 接收到的有效字节数（不包括帧头和校验和）
 * @param id: (I) 应答远程端的id
 * @param ack: (I) 应答码
 * @note
 * 数据应答帧
 * - (xwu8_t)0x9, 帧长度
 * - (xwu8_t)0x90, 帧长度的镜像反转
 * - (xwu8_t)应答的数据帧的id | SOSCP_ID_ACK
 * - (xwu8_t)sdu[0]: 收到的有效字节数（不包括帧头和校验和）
 * - (xwu8_t)sdu[1]: 应答代码
 * - (xwu8_t)CRC32 第一字节（最高有效字节）
 * - (xwu8_t)CRC32 第二字节
 * - (xwu8_t)CRC32 第三字节
 * - (xwu8_t)CRC32 第四字节（第低有效字节）
 */
static __xwmd_code
xwer_t xwscp_tx_sdu_ack(struct xwscp * xwscp, xwu8_t rxsize,
                        xwu8_t id, xwu8_t ack)
{
        xwu8_t stream[sizeof(xwscp_frm_sdu_ack)];
        struct xwscp_frame * frm;
        xwsz_t infolen;
        xwu32_t crc32;
        xwid_t csmtxid;
        xwer_t rc;
        xwtm_t xwtm;

        frm = (struct xwscp_frame *)stream;
        memcpy(frm, xwscp_frm_sdu_ack, sizeof(struct xwscp_frmhead));
        frm->head.id = id | SOSCP_ID_ACK;
        frm->sdu[0] = rxsize;
        frm->sdu[1] = ack;
        infolen = frm->head.frmlen - SOSCP_CHKSUM_SIZE;
        crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
        frm->sdu[2] = (xwu8_t)((crc32 >> 24) & 0xFF);
        frm->sdu[3] = (xwu8_t)((crc32 >> 16) & 0xFF);
        frm->sdu[4] = (xwu8_t)((crc32 >> 8) & 0xFF);
        frm->sdu[5] = (xwu8_t)((crc32 >> 0) & 0xFF);

        csmtxid = xwosal_mtx_get_id(&xwscp->csmtx);
        rc = xwosal_mtx_lock(csmtxid);
        if (OK == rc) {
                xwtm = XWTM_MAX;
                rc = xwscp_hwifal_tx(xwscp, frm, &xwtm);
                xwosal_mtx_unlock(csmtxid);
        }/* else {} */
        return rc;
}

/**
 * @brief 格式化报文为数据帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param frm: (O) 输出格式化后的报文的缓冲区
 * @param sdu: (I) 报文数据
 * @param size: (I) 报文数据的大小
 * @note
 * + 数据帧：
 *   - (xwu8_t)帧长度
 *   - (xwu8_t)帧长度的镜像反转
 *   - (xwu8_t)(SOSCP_PORT_DATA | QoS)
 *   - (xwu8_t)id
 *   - (xwu8_t)sdu[0 ~ msg->size - 1]: 报文数据
 *   - (xwu8_t)sdu[msg->size + 0]/CRC32 最高有效字节
 *   - (xwu8_t)sdu[msg->size + 1]/CRC32 第二字节
 *   - (xwu8_t)sdu[msg->size + 2]/CRC32 第三字节
 *   - (xwu8_t)sdu[msg->size + 3]/CRC32 最低有效字节
 */
__xwmd_code
xwer_t xwscp_fmt_msg(struct xwscp * xwscp, struct xwscp_frame * frm,
                     const xwu8_t sdu[], xwu8_t size)
{
        xwer_t rc;
        xwu32_t crc32;
        xwu32_t txcnt;
        xwsz_t infolen;
        xwu8_t id;

        xwaop_read(xwu32_t, &xwscp->txi.cnt, &txcnt);
        id = SOSCP_ID(txcnt);
        if (SOSCP_ID_SYNC == id) {
                rc = -ENOLINK;
                goto err_notsync;
        }
        infolen = size + sizeof(struct xwscp_frmhead);
        frm->sof = SOSCP_HWIFAL_SOF,
        frm->head.frmlen = (xwu8_t)(infolen + SOSCP_CHKSUM_SIZE);
        frm->head.mirror = xwbop_rbit(xwu8_t, frm->head.frmlen);
        frm->head.port = SOSCP_PORT_DATA | SOSCP_QOS(1);
        frm->head.id = id;
        memcpy(frm->sdu, sdu, size);
        crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
        frm->sdu[size + 0] = (xwu8_t)((crc32 >> 24) & 0xFF);
        frm->sdu[size + 1] = (xwu8_t)((crc32 >> 16) & 0xFF);
        frm->sdu[size + 2] = (xwu8_t)((crc32 >> 8) & 0xFF);
        frm->sdu[size + 3] = (xwu8_t)((crc32 >> 0) & 0xFF);
        frm->eof = SOSCP_HWIFAL_EOF;
        return OK;

err_notsync:
        return rc;
}

/**
 * @brief 校验帧是否合法
 * @param xwscp: (I) SOSCP对象的指针
 * @param frm: (I) 被校验的帧的指针
 * @retval OK: 合法
 * @retval -EBADMSG: 校验错误
 */
static __xwmd_code
xwer_t xwscp_chk_frm(struct xwscp_frame * frm)
{
        xwu8_t frmlen;
        xwu8_t mirror;
        xwu8_t * crc32_pos;
        xwu32_t crc32;
        xwsz_t infolen;
        xwer_t rc;

        frmlen = frm->head.frmlen;
        mirror = xwbop_rbit(xwu8_t, frmlen);
        if (mirror != frm->head.mirror) {
                rc = -EBADMSG;
        } else {
                infolen = (xwsz_t)frmlen - SOSCP_CHKSUM_SIZE;
                crc32_pos = &frm->sdu[infolen - sizeof(struct xwscp_frmhead)];
                crc32 = xwlib_crc32_calms((xwu8_t *)&frm->head, infolen);
                if ((((crc32 >> 24) & 0xFF) != crc32_pos[0]) ||
                    (((crc32 >> 16) & 0xFF) != crc32_pos[1]) ||
                    (((crc32 >> 8) & 0xFF) != crc32_pos[2]) ||
                    (((crc32 >> 0) & 0xFF) != crc32_pos[3])) {
                        rc = -EBADMSG;
                } else {
                        rc = OK;
                }
        }
        return rc;
}

/**
 * @brief 发布数据帧到接收队列
 * @param xwscp: (I) SOSCP对象的指针
 * @param pubfrm: (I) 待发布的数据帧
 */
__xwmd_code
void xwscp_rxq_pub(struct xwscp * xwscp, struct xwscp_frmslot * pubfrm)
{
        xwid_t smrid;

        xwosal_splk_lock(&xwscp->rxq.lock);
        xwlib_bclst_add_tail(&xwscp->rxq.head, &pubfrm->node);
        xwosal_splk_unlock(&xwscp->rxq.lock);
        smrid = xwosal_smr_get_id(&xwscp->rxq.smr);
        xwosal_smr_post(smrid);
}

/**
 * @brief 从接收队列中选择一条数据帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param slotbuf (O) 指向缓冲区的指针，通过此缓冲区返回被选中的帧槽指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_rxq_choose(struct xwscp * xwscp, struct xwscp_frmslot ** slotbuf,
                        xwtm_t * xwtm)
{
        struct xwscp_frmslot * pubfrm;
        xwid_t smrid;
        xwer_t rc;

        smrid = xwosal_smr_get_id(&xwscp->rxq.smr);
        rc = xwosal_smr_timedwait(smrid, xwtm);
        if (__unlikely(rc < 0)) {
                goto err_rxqsmr_timedwait;
        }
        xwosal_splk_lock(&xwscp->rxq.lock);
        pubfrm = xwlib_bclst_first_entry(&xwscp->rxq.head, struct xwscp_frmslot, node);
        xwlib_bclst_del_init(&pubfrm->node);
        xwosal_splk_unlock(&xwscp->rxq.lock);
        *slotbuf = pubfrm;

err_rxqsmr_timedwait:
        return rc;
}

/**
 * @brief 接收同步帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 * + 同步帧：
 *   - (xwu8_t)0xD, 帧长度
 *   - (xwu8_t)0xB0, 帧长度的镜像反转
 *   - (xwu8_t)(SOSCP_PORT_CMD | SOSCP_QOS(1)), 端口:SOSCP_PORT_CMD，QoS:1
 *   - (xwu8_t)SOSCP_ID_SYNC, 同步帧的id
 *   - (xwu8_t)SOSCP_SYNC_KEY, sdu[0], 同步帧的秘钥
 *   - (xwu8_t)sdu[1], 发送计数器第一字节（最高有效字节）
 *   - (xwu8_t)sdu[2], 发送计数器第二字节
 *   - (xwu8_t)sdu[3], 发送计数器第三字节
 *   - (xwu8_t)sdu[4], 发送计数器第四字节（第低有效字节）
 *   - (xwu8_t)CRC32 第一字节（最高有效字节）
 *   - (xwu8_t)CRC32 第二字节
 *   - (xwu8_t)CRC32 第三字节
 *   - (xwu8_t)CRC32 第四字节（第低有效字节）
 */
static __xwmd_code
xwer_t xwscp_rx_cfrm_sync(struct xwscp * xwscp, struct xwscp_frmslot * frmslot)
{
        xwer_t rc;
        xwu32_t rmttxcnt;

        xwscplogf(DEBUG, "id:0x%X, key:0x%X\n",
                  frmslot->frm.head.id, frmslot->frm.sdu[0]);
        if (((SOSCP_PORT_CMD | SOSCP_QOS(1)) == frmslot->frm.head.port) &&
            (SOSCP_ID_SYNC == frmslot->frm.head.id) &&
            (SOSCP_SYNC_KEY == frmslot->frm.sdu[0])) {
                rmttxcnt = ((xwu32_t)frmslot->frm.sdu[1]) << 24U;
                rmttxcnt |= ((xwu32_t)frmslot->frm.sdu[2]) << 16U;
                rmttxcnt |= ((xwu32_t)frmslot->frm.sdu[3]) << 8U;
                rmttxcnt |= ((xwu32_t)frmslot->frm.sdu[4]) << 0U;
                rc = xwscp_tx_cfrm_syncack(xwscp, rmttxcnt);
                if (OK == rc) {
                        xwaop_write(xwu32_t, &xwscp->rxq.cnt, rmttxcnt + 1, NULL);
                }/* else {} */
        } else {
                rc = -EPERM;
        }
        xwscp_free_frmslot(xwscp, frmslot);
        return rc;
}

/**
 * @brief 接收同步应答帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 * @note
 * + 同步应答帧：
 *   - (xwu8_t)0xD, 帧长度
 *   - (xwu8_t)0xB0, 帧长度的镜像反转
 *   - (xwu8_t)(SOSCP_PORT_CMD | SOSCP_QOS(1)), 端口:SOSCP_PORT_CMD，QoS:1
 *   - (xwu8_t)(SOSCP_ID_SYNC | SOSCP_ID_ACK), 同步应答帧的id
 *   - (xwu8_t)SOSCP_SYNC_KEY, sdu[0], 同步应答帧的秘钥
 *   - (xwu8_t)sdu[1], 接收计数器：第一字节（最高有效字节）
 *   - (xwu8_t)sdu[2], 接收计数器：第二字节
 *   - (xwu8_t)sdu[3], 接收计数器：第三字节
 *   - (xwu8_t)sdu[4], 接收计数器：第四字节（第低有效字节）
 *   - (xwu8_t)CRC32 第一字节（最高有效字节）
 *   - (xwu8_t)CRC32 第二字节
 *   - (xwu8_t)CRC32 第三字节
 *   - (xwu8_t)CRC32 第四字节（第低有效字节）
 */
static __xwmd_code
xwer_t xwscp_rx_cfrm_syncack(struct xwscp * xwscp, struct xwscp_frmslot * frmslot)
{
        xwid_t csmtxid;
        struct xwscp_frame * frm;
        xwer_t rc;
        xwtm_t xwtm;

        csmtxid = xwosal_mtx_get_id(&xwscp->csmtx);
        rc = xwosal_mtx_lock(csmtxid);
        if (OK == rc) {
                frm = xwscp->txi.frm;
                xwosal_mtx_unlock(csmtxid);
                if (NULL == frm) {
                        xwosal_cdt_unicast(xwosal_cdt_get_id(&xwscp->cscdt));
                        xwtm = XWTM_MAX;
                        xwscp_hwifal_notify(xwscp, SOSCP_HWIFNTF_CONNECT, &xwtm);
                }/* else {} */
        }/* else {} */
        xwscp_free_frmslot(xwscp, frmslot);
        return rc;
}

/**
 * @brief 接收数据应答帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 * @note
 * 数据应答帧
 * - (xwu8_t)0x9, 帧长度
 * - (xwu8_t)0x90, 帧长度的镜像反转
 * - (xwu8_t)应答的数据帧的id | SOSCP_ID_ACK
 * - (xwu8_t)sdu[0]: 收到的有效字节数（不包括帧头和校验和）
 * - (xwu8_t)sdu[1]: 应答代码
 * - (xwu8_t)CRC32 第一字节（最高有效字节）
 * - (xwu8_t)CRC32 第二字节
 * - (xwu8_t)CRC32 第三字节
` * - (xwu8_t)CRC32 第四字节（第低有效字节）
 */
static __xwmd_code
xwer_t xwscp_rx_frm_sdu_ack(struct xwscp * xwscp, struct xwscp_frmslot * frmslot)
{
        xwid_t csmtxid;
        struct xwscp_frame * frm;
        xwu8_t rmtrxsize, txsize;
        xwer_t rc;

        rc = OK;
        rmtrxsize = frmslot->frm.sdu[0];
        csmtxid = xwosal_mtx_get_id(&xwscp->csmtx);
        rc = xwosal_mtx_lock(csmtxid);
        if (__unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        frm = xwscp->txi.frm;
        if (!is_err_or_null(frm)) {
                txsize = frm->head.frmlen - sizeof(struct xwscp_frmhead) -
                         SOSCP_CHKSUM_SIZE;
                if (txsize == rmtrxsize) {
                        xwscp->txi.ack = frmslot->frm.sdu[1];
                } else {
                        xwscp->txi.ack = SOSCP_ACK_ESIZE;
                }
                xwosal_mtx_unlock(csmtxid);
                xwosal_cdt_unicast(xwosal_cdt_get_id(&xwscp->cscdt));
        } else {
                xwosal_mtx_unlock(csmtxid);
        }

err_mtx_lock:
        xwscp_free_frmslot(xwscp, frmslot);
        return rc;
}

/**
 * @brief 接收数据帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param frmslot: (I) 接收到的帧的帧槽指针
 * @return 错误码
 * @note
 * 数据帧:
 * - (xwu8_t)帧长度
 * - (xwu8_t)帧长度的镜像反转
 * - (xwu8_t)数据帧的id
 * - (xwu8_t)sdu[0 ... ] 数据
 * - (xwu8_t)CRC32 第一字节（最高有效字节）
 * - (xwu8_t)CRC32 第二字节
 * - (xwu8_t)CRC32 第三字节
 * - (xwu8_t)CRC32 第四字节（第低有效字节）
 */
static __xwmd_code
xwer_t xwscp_rx_frm_sdu(struct xwscp * xwscp, struct xwscp_frmslot * frmslot)
{
        xwer_t rc;
        xwu32_t rxcnt;
        xwu8_t rxid;
        xwu8_t rxsize;
        xwu8_t localid;

        rxid = SOSCP_ID(frmslot->frm.head.id);
        rxsize = frmslot->frm.head.frmlen - sizeof(struct xwscp_frmhead) -
                 SOSCP_CHKSUM_SIZE;
        xwaop_read(xwu32_t, &xwscp->rxq.cnt, &rxcnt);
        localid = SOSCP_ID(rxcnt);

        if (rxid == localid) {
                rc = xwscp_tx_sdu_ack(xwscp, rxsize, localid, SOSCP_ACK_OK);
                if (OK == rc) {
                        xwaop_add(xwu32_t, &xwscp->rxq.cnt, 1, NULL, NULL);
                        xwscp_rxq_pub(xwscp, frmslot);
                } else {
                        xwscp_free_frmslot(xwscp, frmslot);
                }
        } else if (SOSCP_ID(rxid + 1) == localid) {
                rc = xwscp_tx_sdu_ack(xwscp, rxsize, localid, SOSCP_ACK_EALREADY);
                xwscp_free_frmslot(xwscp, frmslot);
        } else {
                rc = xwscp_tx_sdu_ack(xwscp, rxsize, localid, SOSCP_ACK_ECONNRESET);
                xwscp_free_frmslot(xwscp, frmslot);
        }
        return rc;
}

/**
 * @brief 接收一帧
 * @param xwscp: (I) SOSCP对象的指针
 * @param frmslot: (I) 帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_rx_frm(struct xwscp * xwscp, struct xwscp_frmslot * frmslot)
{
        xwer_t rc;
        xwu8_t id;

        id = frmslot->frm.head.id;
        xwscplogf(DEBUG, "frame id: 0x%X\n", id);
        switch (id) {
        case SOSCP_ID_SYNC:
                rc = xwscp_rx_cfrm_sync(xwscp, frmslot);
                break;
        case SOSCP_ID_SYNC | SOSCP_ID_ACK:
                rc = xwscp_rx_cfrm_syncack(xwscp, frmslot);
                break;
        default:
                if (SOSCP_ID_ACK & id) {
                        rc = xwscp_rx_frm_sdu_ack(xwscp, frmslot);
                } else {
                        rc = xwscp_rx_frm_sdu(xwscp, frmslot);
                }
                break;
        }
        return rc;
}

/**
 * @brief SOSCP的接收线程状态机
 * @param xwscp: (I) SOSCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_fsm(struct xwscp * xwscp)
{
        struct xwscp_frmslot * frmslot;
        xwer_t rc;

        do {
                rc = xwscp_hwifal_rx(xwscp, &frmslot);
                if (OK == rc) {
                        rc = xwscp_chk_frm(&frmslot->frm);
                }
        } while ((-EAGAIN == rc) || (-EBADMSG == rc));
        if (OK == rc) {
                rc = xwscp_rx_frm(xwscp, frmslot);
        }
        return rc;
}

/**
 * @brief SOSCP的接收线程
 * @param xwscp: (I) SOSCP对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwscp_thrd(struct xwscp * xwscp)
{
        xwer_t rc = OK;

        while (!xwosal_cthrd_shld_stop()) {
                if (xwosal_cthrd_shld_frz()) {
                        xwscplogf(DEBUG, "Start freezing ...\n");
                        rc = xwosal_cthrd_freeze();
                        if (__unlikely(rc < 0)) {
                                xwscplogf(ERR, "Failed to freeze ... [rc:%d]\n", rc);
                                xwosal_cthrd_yield();
                        }/* else {} */
                        xwscplogf(DEBUG, "Resuming ...\n");
                } else if (SOSCP_HWIFST_OPENED == xwscp->hwifst) {
                        rc = xwscp_fsm(xwscp);
                        if (OK == rc) {
                        } else if (-ETIMEDOUT == rc) {
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                xwscplogf(DEBUG, "Interrupted ... [rc:%d]\n", rc);
                        } else if (-EBADMSG == rc) {
                                xwscplogf(WARNING, "Bad frame! \n");
                        } else {
                                xwscplogf(ALERT, "xwscp_fsm() ... [rc:%d]\n", rc);
                                xwosal_cthrd_wait_exit();
                                break;
                        }
                } else {
                        xwosal_cthrd_wait_exit();
                        break;
                }
        }
        return rc;
}
