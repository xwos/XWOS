/**
 * @file
 * @brief 点对点通讯协议：协议层
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
#include <xwos/lib/xwbmpaop.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/crc32.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/mif.h>

/**
 * @brief 控制帧模板：同步帧
 */
__xwmd_rodata const xwu8_t xwpcp_cmd_connect[] = {
        (xwu8_t)XWPCP_SOF,
        (xwu8_t)XWPCP_SOF,
        (xwu8_t)0x66, /* 帧头长度 | mirror */
        (xwu8_t)0x8E, /* 帧头校验和 */
        (xwu8_t)XWPCP_PORT_CMD, /* 端口:0 */
        (xwu8_t)XWPCP_PORT0_CMDID_CONNECT, /* 同步帧的命令ID */
        (xwu8_t)XWPCP_MSG_QOS_3, /* Qos: 可靠消息，有CRC校验码 */
        (xwu8_t)0x8, /* SDU长度 */
        (xwu8_t)'X', /* sdu[0]: protocol 0 */
        (xwu8_t)'W', /* sdu[1]: protocol 1 */
        (xwu8_t)'P', /* sdu[2]: protocol 2 */
        (xwu8_t)'C', /* sdu[3]: protocol 3 */
        (xwu8_t)'P', /* sdu[4]: protocol 4 */
        (xwu8_t)XWPCP_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWPCP_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWPCP_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)0xDF, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0x05, /* CRC32 第二字节 */
        (xwu8_t)0x7B, /* CRC32 第三字节 */
        (xwu8_t)0xB6, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
};

/**
 * @brief 控制帧模板：同步应答帧
 */
__xwmd_rodata const xwu8_t xwpcp_cmd_connect_ack[] = {
        (xwu8_t)XWPCP_SOF,
        (xwu8_t)XWPCP_SOF,
        (xwu8_t)0x66, /* 帧头长度 | mirror */
        (xwu8_t)0x0E, /* 帧头校验和 */
        (xwu8_t)XWPCP_PORT_CMD, /* 端口:0 */
        (xwu8_t)(XWPCP_PORT0_CMDID_CONNECT | XWPCP_ID_ACK), /* 同步帧的命令ID | 应答 */
        (xwu8_t)XWPCP_MSG_QOS_3, /* Qos: 可靠消息，有CRC校验码 */
        (xwu8_t)0x8, /* SDU长度 */
        (xwu8_t)'X', /* sdu[0]: protocol 0 */
        (xwu8_t)'W', /* sdu[1]: protocol 1 */
        (xwu8_t)'P', /* sdu[2]: protocol 2 */
        (xwu8_t)'C', /* sdu[3]: protocol 3 */
        (xwu8_t)'P', /* sdu[4]: protocol 4 */
        (xwu8_t)XWPCP_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWPCP_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWPCP_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)0xDF, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0x05, /* CRC32 第二字节 */
        (xwu8_t)0x7B, /* CRC32 第三字节 */
        (xwu8_t)0xB6, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
};

/**
 * @brief 数据应答帧模板
 */
__xwmd_rodata const xwu8_t xwpcp_sdu_ack[] = {
        (xwu8_t)XWPCP_SOF,
        (xwu8_t)XWPCP_SOF,
        (xwu8_t)0x66, /* 帧头长度 | mirror */
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)0x0, /* 帧头校验和 */
        (xwu8_t)0x0, /* 端口 */
        (xwu8_t)XWPCP_ID_ACK, /* id | XWPCP_ID_ACK */
        (xwu8_t)XWPCP_MSG_QOS_3, /* Qos: 可靠消息，有CRC校验码 */
        (xwu8_t)0x1, /* SDU长度 */
        (xwu8_t)0, /* sdu[0]: 应答 */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWPCP_EOF,
        (xwu8_t)XWPCP_EOF,
};

/**
 * @brief 应答错误对照表
 */
__xwmd_rodata const xwer_t xwpcp_callback_rc[XWPCP_ACK_NUM] = {
        [XWPCP_ACK_OK] = XWOK,
        [XWPCP_ACK_ECONNRESET] = -ECONNRESET,
        [XWPCP_ACK_EALREADY] = -EALREADY,
        [XWPCP_ACK_NOMEM] = -ENOMEM,
};

/**
 * @brief 数据长度编解码移位表
 */
__xwmd_rodata const xwu8_t xwpcp_ecsdusz_shift_table[] = {
        0, 7, 14, 21, 28, 35, 42, 49,
        56, 63, 70, 77, 84, 91, 98, 105,
        112, 119, 126, 133,
};

static __xwmd_code
xwer_t xwpcp_chk_frm(union xwpcp_slot * slot);

static __xwmd_code
void xwpcp_rxq_pub(struct xwpcp * xwpcp,
                   union xwpcp_slot * slot,
                   xwu8_t port);

static __xwmd_code
xwer_t xwpcp_rx_cmd_connect(struct xwpcp * xwpcp, union xwpcp_slot * slot);

static __xwmd_code
xwer_t xwpcp_rx_cmd_connect_ack(struct xwpcp * xwpcp, union xwpcp_slot * slot);

static __xwmd_code
xwer_t xwpcp_rx_sdu(struct xwpcp * xwpcp, union xwpcp_slot * slot);

static __xwmd_code
xwer_t xwpcp_rx_sdu_ack(struct xwpcp * xwpcp, union xwpcp_slot * slot);

static __xwmd_code
xwer_t xwpcp_rx_frm(struct xwpcp * xwpcp, union xwpcp_slot * slot);

static __xwmd_code
xwer_t xwpcp_rxfsm(struct xwpcp * xwpcp);

static __xwmd_code
void xwpcp_txq_add_head(struct xwpcp * xwpcp, struct xwpcp_carrier * car);

static __xwmd_code
void xwpcp_txq_add_tail(struct xwpcp * xwpcp, struct xwpcp_carrier * car);

static __xwmd_code
xwer_t xwpcp_tx_cmd_connect(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcp_tx_cmd_connect_ack(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcp_connect(struct xwpcp * xwpcp);

static __xwmd_code
void xwpcp_finish_tx(struct xwpcp * xwpcp, struct xwpcp_carrier * car);

static __xwmd_code
xwer_t xwpcp_tx_frm(struct xwpcp * xwpcp, struct xwpcp_carrier * car);

static __xwmd_code
xwer_t xwpcp_txfsm(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcp_doze(xwu32_t cnt);

/**
 * @brief 增加XWPCP的引用计数，防止其被停止
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWPCP已经停止运行
 */
__xwmd_code
xwer_t xwpcp_grab(struct xwpcp * xwpcp)
{
        return xwos_object_grab(&xwpcp->xwobj);
}

/**
 * @brief 减少XWPCP的引用计数
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWPCP没有在运行
 */
__xwmd_code
xwer_t xwpcp_put(struct xwpcp * xwpcp)
{
        return xwos_object_put(&xwpcp->xwobj);
}

/**
 * @brief 编码数据长度
 * @param[in] sdusize: 数据长度
 * @param[out] ecsdusz: 指向缓冲区的指针，此缓冲区用于输出编码后的数据数组
 * @param[out] ecsize: 指向缓冲区的指针，此缓冲区用于输出编码后的数据数组的长度
 */
__xwmd_code
void xwpcp_encode_sdusize(xwsz_t sdusize, xwu8_t * ecsdusz, xwu8_t * ecsize)
{
        xwu8_t i;

        i = 0;
        while (true) {
                ecsdusz[i] = sdusize & 0x7F;
                sdusize >>= 7U;
                if (sdusize) {
                        ecsdusz[i] |= 0x80;
                        i++;
                } else {
                        i++;
                        break;
                }
        }
        *ecsize = i;
}

/**
 * @brief 解码数据长度
 * @param[in] ecsdusz: 编码的数据长度数据
 * @param[out] sdusize: 指向缓冲区的指针，解码后通过此缓冲区返回数据长度
 */
__xwmd_code
void xwpcp_decode_sdusize(xwu8_t * ecsdusz, xwsz_t * sdusize)
{
        xwsz_t dcsdusz;
        xwsz_t sft;
        xwsq_t i;

        i = 0;
        dcsdusz = 0;
        while (true) {
                sft = xwpcp_ecsdusz_shift_table[i];
                dcsdusz |= ((xwsz_t)(ecsdusz[i] & 0x7F) << sft);
                if (ecsdusz[i] & 0x80) {
                        i++;
                } else {
                        break;
                }
        }
        *sdusize = dcsdusz;
}

/**
 * @brief 计算数据流的校验和
 * @param[in] data: 数据流
 * @param[in] size: 数据流长度
 * @return 校验和
 */
__xwmd_code
xwu8_t xwpcp_cal_head_chksum(xwu8_t data[], xwsz_t size)
{
        xwu16_t chksum;
        xwu8_t i;

        chksum = 0;
        for (i = 0; i < size; i++) {
                chksum += (xwu16_t)data[i];
        }
        while (chksum & 0xFF00U) {
                chksum = (xwu16_t)(chksum >> 16U) + (xwu16_t)(chksum & 0xFFU);
        }
        chksum ^= 0xFF;
        return (xwu8_t)chksum;
}

/**
 * @brief 校验数据流
 * @param[in] data: 数据流
 * @param[in] size: 数据流长度
 * @return 布尔值
 * @retval true: 校验通过
 * @retval false: 校验失败
 */
__xwmd_code
bool xwpcp_chk_head(xwu8_t data[], xwsz_t size)
{
        xwu16_t chksum;
        xwu8_t i;

        chksum = 0;
        for (i = 0; i < size; i++) {
                chksum += (xwu16_t)data[i];
        }
        while (chksum & 0xFF00U) {
                chksum = (xwu16_t)(chksum >> 16U) + (xwu16_t)(chksum & 0xFFU);
        }
        return (chksum == 0xFF);
}

/**
 * @brief 校验帧是否合法
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] slot: 待校验的帧槽的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EBADMSG: 校验错误
 */
static __xwmd_code
xwer_t xwpcp_chk_frm(union xwpcp_slot * slot)
{
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t sdusize;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        sdupos = XWPCP_SDUPOS(&slot->rx.frm.head);
        sdusize = XWPCP_RXSDUSIZE(&slot->rx);
        crc32pos = &sdupos[sdusize];
        calsz = sdusize;
        crc32 = xwlib_crc32_calms(sdupos, &calsz);
        if ((((crc32 >> 24U) & 0xFFU) != crc32pos[0]) ||
            (((crc32 >> 16U) & 0xFFU) != crc32pos[1]) ||
            (((crc32 >> 8U) & 0xFFU) != crc32pos[2]) ||
            (((crc32 >> 0U) & 0xFFU) != crc32pos[3])) {
                rc = -EBADMSG;
        } else {
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 发布数据帧到接收队列
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] slot: 待发布的帧槽指针
 * @param[in] port: 端口值
 */
static __xwmd_code
void xwpcp_rxq_pub(struct xwpcp * xwpcp,
                   union xwpcp_slot * slot,
                   xwu8_t port)
{
        xwpcplogf(DEBUG,
                  "[rxthd] Publish a frame(%p): port:0x%X, frmsize:0x%u\n",
                  slot, port, slot->rx.frmsize);
        xwos_splk_lock(&xwpcp->rxq.lock[port]);
        xwlib_bclst_add_tail(&xwpcp->rxq.q[port], &slot->rx.node);
        xwos_splk_unlock(&xwpcp->rxq.lock[port]);
        xwos_sem_post(&xwpcp->rxq.sem[port]);
}

/**
 * @brief 从接收队列中选择一条数据帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] port: 端口
 * @return 帧槽的指针
 */
__xwmd_code
union xwpcp_slot * xwpcp_rxq_choose(struct xwpcp * xwpcp, xwu8_t port)
{
        union xwpcp_slot * slot;
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&xwpcp->rxq.lock[port], &cpuirq);
        slot = xwlib_bclst_first_entry(&xwpcp->rxq.q[port],
                                       union xwpcp_slot,
                                       rx.node);
        xwlib_bclst_del_init(&slot->rx.node);
        xwos_splk_unlock_cpuirqrs(&xwpcp->rxq.lock[port], cpuirq);
        return slot;
}

/**
 * @brief 接收同步帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] slot: 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_cmd_connect(struct xwpcp * xwpcp, union xwpcp_slot * slot)
{
        xwu8_t * sdupos;
        xwer_t rc;
        char proto[6];

        sdupos = XWPCP_SDUPOS(&slot->rx.frm.head);
        proto[0] = sdupos[0];
        proto[1] = sdupos[1];
        proto[2] = sdupos[2];
        proto[3] = sdupos[3];
        proto[4] = sdupos[4];
        proto[5] = '\0';
        xwpcplogf(DEBUG, "[rxthd] proto:%s-%d.%d.%d\n",
                  proto, sdupos[5], sdupos[6], sdupos[7]);
        if ((0 == strcmp(proto, "XWPCP")) &&
            (XWPCP_VERSION_MAJOR == sdupos[5]) &&
            (XWPCP_VERSION_MINOR == sdupos[6]) &&
            (XWPCP_VERSION_REVISION == sdupos[7])) {
                rc = xwpcp_tx_cmd_connect_ack(xwpcp);
                if (XWOK == rc) {
                        xwaop_write(xwu32_t, &xwpcp->rxq.cnt, 0, NULL);
                }/* else {} */
        } else {
                rc = -EPERM;
        }
        xwmm_bma_free(xwpcp->mempool, slot);
        return rc;
}

/**
 * @brief 接收同步应答帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] slot: 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_cmd_connect_ack(struct xwpcp * xwpcp, union xwpcp_slot * slot)
{
        xwu8_t * sdupos;
        xwsq_t hwifst;
        char proto[6];

        sdupos = XWPCP_SDUPOS(&slot->rx.frm.head);
        proto[0] = sdupos[0];
        proto[1] = sdupos[1];
        proto[2] = sdupos[2];
        proto[3] = sdupos[3];
        proto[4] = sdupos[4];
        proto[5] = '\0';

        xwpcplogf(DEBUG,
                  "[rxthd] proto:%s-%d.%d.%d, remote:%d, local:%d\n",
                  proto, sdupos[5], sdupos[6], sdupos[7]);
        xwaop_read(xwsq_t, &xwpcp->hwifst, &hwifst);
        if (!(XWPCP_HWIFST_CONNECT & hwifst)) {
                if ((0 == strcmp(proto, "XWPCP")) &&
                    (XWPCP_VERSION_MAJOR == sdupos[5]) &&
                    (XWPCP_VERSION_MINOR == sdupos[6]) &&
                    (XWPCP_VERSION_REVISION == sdupos[7])) {
                        xwaop_write(xwu32_t, &xwpcp->txq.cnt, 0, NULL);
                        xwaop_s1m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_CONNECT,
                                  NULL, NULL);
                        xwpcp_hwifal_notify(xwpcp, XWPCP_HWIFNTF_CONNECT);
                }/* else {} */
        }
        xwmm_bma_free(xwpcp->mempool, slot);
        return XWOK;
}

/**
 * @brief 接收数据帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] slot: 接收到的帧槽指针
 * @return 错误码
 * @note
 * + 数据帧：
 *   - (xwu8_t)XWPCP_SOF,
 *   - (xwu8_t)XWPCP_SOF,
 *   - (xwu8_t)帧头长度 | 镜像反转
 *   - (xwu8_t)帧头校验和，从下一字节开始计算
 *   - (xwu8_t)端口
 *   - (xwu8_t)ID
 *   - (xwu8_t)QoS
 *   - (xwu8_t)ecsdusz[0:n] 编码的数据长度
 *   - (xwu8_t)sdu[0:msg->size - 1] 数据
 *   - (xwu8_t)sdu[msg->size + 0]/CRC32 最高有效字节
 *   - (xwu8_t)sdu[msg->size + 1]/CRC32 第二字节
 *   - (xwu8_t)sdu[msg->size + 2]/CRC32 第三字节
 *   - (xwu8_t)sdu[msg->size + 3]/CRC32 最低有效字节
 *   - (xwu8_t)XWPCP_EOF,
 *   - (xwu8_t)XWPCP_EOF,
 */
static __xwmd_code
xwer_t xwpcp_rx_sdu(struct xwpcp * xwpcp, union xwpcp_slot * slot)
{
        xwu32_t rxcnt;
        xwu8_t rmtid, lclid;
        xwu8_t port;
        xwer_t rc;

        xwaop_read(xwu32_t, &xwpcp->rxq.cnt, &rxcnt);
        lclid = XWPCP_ID(rxcnt);
        rmtid = slot->rx.frm.head.id;
        port = slot->rx.frm.head.port;
        xwpcplogf(DEBUG,
                  "[rxthd] port:0x%X, qos:0x%X, frmsize:0x%X, "
                  "Remote ID:0x%X, Local ID:0x%X\n",
                  port, slot->rx.frm.head.qos, slot->rx.frmsize, rmtid, lclid);
        if (slot->rx.frm.head.qos & XWPCP_MSG_QOS_ACK_MSK) {
                if (__xwcc_likely(rmtid == lclid)) {
                        /* 收到数据 */
                        rc = xwpcp_tx_sdu_ack(xwpcp, port, rmtid, XWPCP_ACK_OK);
                        if (XWOK == rc) {
                                xwaop_add(xwu32_t, &xwpcp->rxq.cnt, 1, NULL, NULL);
                                xwpcp_rxq_pub(xwpcp, slot, port);
                        } else {
                                xwmm_bma_free(xwpcp->mempool, slot);
                        }
                } else if (XWPCP_ID(rmtid + 1) == lclid) {
                        /* 收到重复的数据 */
                        rc = xwpcp_tx_sdu_ack(xwpcp, port, rmtid, XWPCP_ACK_EALREADY);
                        xwmm_bma_free(xwpcp->mempool, slot);
                } else {
                        /* 连接被复位 */
                        rc = xwpcp_tx_sdu_ack(xwpcp, port, rmtid, XWPCP_ACK_ECONNRESET);
                        xwmm_bma_free(xwpcp->mempool, slot);
                }
        } else {
                xwpcp_rxq_pub(xwpcp, slot, port);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 接收数据应答帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] slot: 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_sdu_ack(struct xwpcp * xwpcp, union xwpcp_slot * slot)
{
        xwu8_t * sdupos;
        xwu8_t rmtid;
        xwu8_t ack;
        xwer_t rc;
        xwsq_t hwifst;

        sdupos = XWPCP_SDUPOS(&slot->rx.frm.head);
        rmtid = XWPCP_ID(slot->rx.frm.head.id);
        ack = sdupos[0];
        xwpcplogf(DEBUG, "[rxthd] ID:0x%X, ACK:0x%X\n", rmtid, ack);
        rc = xwos_mtx_lock(&xwpcp->txq.csmtx);
        if (rc < 0) {
                goto err_mtx_lock;
        }
        xwaop_read(xwsq_t, &xwpcp->hwifst, &hwifst);
        if (XWPCP_HWIFST_TX & hwifst) {
                xwpcp->txq.remote.ack = ack;
                xwpcp->txq.remote.id = rmtid;
                xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX, NULL, NULL);
                xwos_mtx_unlock(&xwpcp->txq.csmtx);
                xwos_cond_unicast(&xwpcp->txq.cscond);
        } else {
                xwos_mtx_unlock(&xwpcp->txq.csmtx);
                xwpcplogf(ERR, "[rxthd] no sending frame!\n");
        }

err_mtx_lock:
        xwmm_bma_free(xwpcp->mempool, slot);
        return rc;
}

/**
 * @brief 接收帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] slot: 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rx_frm(struct xwpcp * xwpcp, union xwpcp_slot * slot)
{
        xwer_t rc;
        xwu8_t port;
        xwu8_t id;
        xwu8_t qos;

        port = slot->rx.frm.head.port;
        id = slot->rx.frm.head.id;
        qos = slot->rx.frm.head.qos;
        xwpcplogf(DEBUG, "[rxthd] RX Frame(port:0x%X, ID:0x%X, qos:0x%X)\n",
                  port, id, qos);
        if (XWPCP_MSG_QOS_CHKSUM_MSK & qos) {
                rc = xwpcp_chk_frm(slot);
                if (rc < 0) {
                        xwmm_bma_free(xwpcp->mempool, slot);
                        goto err_badmsg;
                }
        }
        if (XWPCP_PORT_CMD == port) {
                if (XWPCP_PORT0_CMDID_CONNECT == id) {
                        rc = xwpcp_rx_cmd_connect(xwpcp, slot);
                } else {
                        rc = xwpcp_rx_cmd_connect_ack(xwpcp, slot);
                }
        } else {
                if (XWPCP_ID_ACK & id) {
                        rc = xwpcp_rx_sdu_ack(xwpcp, slot);
                } else {
                        rc = xwpcp_rx_sdu(xwpcp, slot);
                }
        }

err_badmsg:
        return rc;
}

/**
 * @brief XWPCP的接收线程状态机
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_rxfsm(struct xwpcp * xwpcp)
{
        union xwpcp_slot * slot;
        xwer_t rc;

        do {
                rc = xwpcp_hwifal_rx(xwpcp, &slot);
        } while (-EAGAIN == rc);
        if (XWOK == rc) {
                rc = xwpcp_rx_frm(xwpcp, slot);
        }
        return rc;
}

/**
 * @brief XWPCP的接收线程
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwpcp_rxthd(struct xwpcp * xwpcp)
{
        xwer_t rc;

        rc = XWOK;
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwpcplogf(DEBUG, "[rxthd] Start freezing ...\n");
                        rc = xwos_cthd_freeze();
                        if (rc < 0) {
                                xwpcplogf(DEBUG,
                                          "[rxthd] Failed to freeze ... rc: %d\n",
                                          rc);
                                xwpcp_doze(1);
                        }/* else {} */
                        xwpcplogf(DEBUG, "[rxthd] Resuming ...\n");
                } else {
                        rc = xwpcp_rxfsm(xwpcp);
                        if (XWOK == rc) {
                        } else if (-ETIMEDOUT == rc) {
                        } else if (-ENOMEM == rc) {
                                xwpcplogf(DEBUG,
                                          "[rxthd] no memory\n", rc);
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                xwpcplogf(DEBUG,
                                          "[rxthd] Interrupted ... [rc:%d]\n",
                                          rc);
                        } else if (-EBADMSG == rc) {
                                xwpcplogf(WARNING,
                                          "[rxthd] Bad Frame! \n");
                        } else {
                                xwpcplogf(ERR,
                                          "[rxthd] xwpcp_rxfsm() returns %d.\n",
                                          rc);
                                break;
                        }
                }
        }
        return rc;
}

/**
 * @brief 申请发送装置
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 发送装置的指针
 */
__xwmd_code
struct xwpcp_carrier * xwpcp_txq_carrier_alloc(struct xwpcp * xwpcp)
{
        struct xwpcp_carrier * car;
        xwssq_t caridx;

        caridx = xwbmpaop_ffz_then_s1i(xwpcp->txq.carbmp, XWPCP_MEMBLK_NUM);
        if (caridx < 0) {
                car = NULL;
        } else {
                car = &xwpcp->txq.car[caridx];
                xwlib_bclst_init_node(&car->node);
                XWPCP_BUG_ON(car->state != XWPCP_CRS_IDLE);
        }
        return car;
}

/**
 * @brief 释放发送装置
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] car: 发送装置的指针
 */
__xwmd_code
void xwpcp_txq_carrier_free(struct xwpcp * xwpcp, struct xwpcp_carrier * car)
{
        xwaop_write(xwu32_t, &car->state, XWPCP_CRS_IDLE, NULL);
        xwbmpaop_t1i_then_c0i(xwpcp->txq.carbmp, car->idx);
}

/**
 * @brief 将待发送的帧放到发送队列的头部
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] car: 发送装置的指针
 */
static __xwmd_code
void xwpcp_txq_add_head(struct xwpcp * xwpcp, struct xwpcp_carrier * car)
{
        xwu8_t pri;

        pri = car->pri;
        xwos_splk_lock(&xwpcp->txq.qlock);
        xwaop_write(xwu32_t, &car->state, XWPCP_CRS_READY, NULL);
        xwlib_bclst_add_head(&xwpcp->txq.q[pri], &car->node);
        if (!xwbmpop_t1i(xwpcp->txq.qnebmp, (xwsq_t)pri)) {
                xwbmpop_s1i(xwpcp->txq.qnebmp, (xwsq_t)pri);
        }/* else {} */
        xwos_splk_unlock(&xwpcp->txq.qlock);
}

/**
 * @brief 将待发送的帧放到发送队列的尾部
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] car: 发送装置的指针
 */
static __xwmd_code
void xwpcp_txq_add_tail(struct xwpcp * xwpcp, struct xwpcp_carrier * car)
{
        xwu8_t pri;

        pri = car->pri;
        xwos_splk_lock(&xwpcp->txq.qlock);
        xwaop_write(xwu32_t, &car->state, XWPCP_CRS_READY, NULL);
        xwlib_bclst_add_tail(&xwpcp->txq.q[pri], &car->node);
        if (!xwbmpop_t1i(xwpcp->txq.qnebmp, (xwsq_t)pri)) {
                xwbmpop_s1i(xwpcp->txq.qnebmp, (xwsq_t)pri);
        }/* else {} */
        xwos_splk_unlock(&xwpcp->txq.qlock);
}

/**
 * @brief 从最高优先级的发送队列中选择一个待发送的帧槽
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 发送装置的指针
 */
__xwmd_code
struct xwpcp_carrier * xwpcp_txq_choose(struct xwpcp * xwpcp)
{
        struct xwpcp_carrier * car;
        xwu8_t pri;

        xwos_splk_lock(&xwpcp->txq.qlock);
        pri = (xwu8_t)xwbmpop_fls(xwpcp->txq.qnebmp, XWPCP_PRI_NUM);
        car = xwlib_bclst_first_entry(&xwpcp->txq.q[pri],
                                      struct xwpcp_carrier,
                                      node);
        xwlib_bclst_del_init(&car->node);
        if (xwlib_bclst_tst_empty(&xwpcp->txq.q[pri])) {
                xwbmpop_c0i(xwpcp->txq.qnebmp, (xwsq_t)pri);
        }/* else {} */
        xwos_splk_unlock(&xwpcp->txq.qlock);
        return car;
}

/**
 * @brief 发送同步帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_tx_cmd_connect(struct xwpcp * xwpcp)
{
        xwer_t rc;

        xwpcplogf(DEBUG, "[txthd] TX SYNC Frame.\n");
        rc = xwos_mtx_lock(&xwpcp->txq.csmtx);
        if (XWOK == rc) {
                rc = xwpcp_hwifal_tx(xwpcp,
                                     (xwu8_t *)xwpcp_cmd_connect,
                                     sizeof(xwpcp_cmd_connect));
                xwos_mtx_unlock(&xwpcp->txq.csmtx);
        }/* else {} */
        return rc;
}

/**
 * @brief 发送同步应答帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] rxcnt: 应答接收计数器
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_tx_cmd_connect_ack(struct xwpcp * xwpcp)
{
        xwer_t rc;

        xwpcplogf(DEBUG, "[txthd] TX SYNC-ACK Frame.\n");
        rc = xwos_mtx_lock(&xwpcp->txq.csmtx);
        if (XWOK == rc) {
                rc = xwpcp_hwifal_tx(xwpcp,
                                     (xwu8_t *)xwpcp_cmd_connect_ack,
                                     sizeof(xwpcp_cmd_connect_ack));
                xwos_mtx_unlock(&xwpcp->txq.csmtx);
        }/* else {} */
        return rc;
}

/**
 * @brief 发送数据应答帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[out] frm: 指向缓冲区的指针，此缓冲区被用于格式化一帧
 * @param[in] port: 端口
 * @param[in] id: 应答远端消息的id
 * @param[in] ack: 应答
 * @return 错误码
 */
__xwmd_code
xwer_t xwpcp_tx_sdu_ack(struct xwpcp * xwpcp, xwu8_t port, xwu8_t id, xwu8_t ack)
{
        xwu8_t stream[sizeof(xwpcp_sdu_ack)];
        struct xwpcp_frm * frm;
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t sdusize;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        xwpcplogf(DEBUG, "[txthd] ACK:0x%X, id:0x%X\n", ack, id);
        frm = (struct xwpcp_frm *)stream;
        memcpy(stream, xwpcp_sdu_ack, sizeof(xwpcp_sdu_ack));
        frm->head.port = port;
        frm->head.id = id | XWPCP_ID_ACK;
        sdupos = XWPCP_SDUPOS(&frm->head);
        sdupos[0] = ack;

        sdusize = 1;
        crc32pos = &sdupos[sdusize];
        calsz = sdusize;
        crc32 = xwlib_crc32_calms(sdupos, &calsz);
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
        rc = xwos_mtx_lock(&xwpcp->txq.csmtx);
        if (XWOK == rc) {
                rc = xwpcp_hwifal_tx(xwpcp, stream, sizeof(xwpcp_sdu_ack));
                xwos_mtx_unlock(&xwpcp->txq.csmtx);
        }/* else {} */
        return rc;
}

/**
 * @brief 格式化数据帧并加入到发送队列
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] sdu: 数据
 * @param[in] sdusize: 数据长度
 * @param[in] pri: 优先级
 * @param[in] port: 端口
 * @param[in] qos: 服务质量
 * @param[in] ntfcb: 通知发送结果的回调函数
 * @param[in] cbarg: 调用回调函数时的用户数据
 * @param[out] txhbuf: 指向缓冲区的指针，通过此缓冲区返回发送句柄
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 * @note
 * + 数据帧：
 *   - (xwu8_t)XWPCP_SOF,
 *   - (xwu8_t)XWPCP_SOF,
 *   - (xwu8_t)帧头长度 | 镜像反转
 *   - (xwu8_t)帧头校验和，从下一字节开始计算
 *   - (xwu8_t)端口
 *   - (xwu8_t)ID
 *   - (xwu8_t)QoS
 *   - (xwu8_t)ecsdusz[0:n] 编码的数据长度
 *   - (xwu8_t)sdu[0:sdusize - 1]: 数据
 *   - (xwu8_t)sdu[sdusize + 0]/CRC32 最高有效字节
 *   - (xwu8_t)sdu[sdusize + 1]/CRC32 第二字节
 *   - (xwu8_t)sdu[sdusize + 2]/CRC32 第三字节
 *   - (xwu8_t)sdu[sdusize + 3]/CRC32 最低有效字节
 *   - (xwu8_t)XWPCP_EOF,
 *   - (xwu8_t)XWPCP_EOF,
 */
__xwmd_code
xwer_t xwpcp_eq_msg(struct xwpcp * xwpcp,
                    const xwu8_t sdu[], xwsz_t sdusize,
                    xwu8_t pri, xwu8_t port, xwu8_t qos,
                    xwpcp_ntf_f ntfcb, void * cbarg,
                    xwpcp_txh_t * txhbuf)
{
        xwu8_t ecsdusz[sizeof(xwsz_t) << 1];
        xwu8_t ecsize;
        xwu8_t frmheadsz;
        xwu8_t frmheadszmir;
        struct xwpcp_carrier * car;
        union xwpcp_slot * slot;
        xwsz_t need, neednum;
        xwssq_t odr;
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        /* 申请发送装置 */
        car = xwpcp_txq_carrier_alloc(xwpcp);
        if (NULL == car) {
                rc = -ENOMEM;
                goto err_car_alloc;
        }
        car->pri = pri;

        /* 申请帧槽 */
        xwpcp_encode_sdusize(sdusize, ecsdusz, &ecsize);
        need = sizeof(union xwpcp_slot) + ecsize + sdusize +
               XWPCP_CRC32_SIZE + XWPCP_EOF_SIZE;
        neednum = XWBOP_DIV_ROUND_UP(need, XWPCP_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWPCP_MEMBLK_SIZE << odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwpcp->mempool, (xwsq_t)odr, (void **)&slot);
        if (rc < 0) {
                goto err_bma_alloc;
        }
        car->slot = slot;

        /* 设置发送信息 */
        slot->tx.ntfcb = ntfcb;
        slot->tx.cbarg = cbarg;
        slot->tx.frmsize = sizeof(struct xwpcp_frm) + ecsize + sdusize +
                           XWPCP_CRC32_SIZE + XWPCP_EOF_SIZE;

        xwpcplogf(DEBUG,
                  "[userthd] car(%p), slot(%p), pri:0x%X, port:0x%X, sdusize:0x%X\n",
                  car, slot, pri, port, sdusize);
        /* SOF */
        memset(slot->tx.frm.sof, XWPCP_SOF, XWPCP_SOF_SIZE);
        /* Head */
        frmheadsz = sizeof(struct xwpcp_frmhead) + ecsize;
        frmheadszmir = xwbop_rbit(xwu8_t, frmheadsz);
        slot->tx.frm.head.headsize = frmheadsz | frmheadszmir;
        slot->tx.frm.head.port = port;
        slot->tx.frm.head.qos = qos;
        memcpy(slot->tx.frm.head.ecsdusz, ecsdusz, ecsize);
        /* slot->tx.frm.head.id 与 slot->tx.frm.head.chksum 待发送时才能确定 */

        /* SDU */
        sdupos = XWPCP_SDUPOS(&slot->tx.frm.head);
        memcpy(sdupos, sdu, sdusize);

        /* CRC32 */
        crc32pos = &sdupos[sdusize];
        if (qos & XWPCP_MSG_QOS_CHKSUM_MSK) {
                calsz = sdusize;
                crc32 = xwlib_crc32_calms(sdu, &calsz);
                crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
                crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
                crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
                crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
        } else {
                crc32pos[0] = 0;
                crc32pos[1] = 0;
                crc32pos[2] = 0;
                crc32pos[3] = 0;
        }

        /* EOF */
        memset(&crc32pos[XWPCP_CRC32_SIZE], XWPCP_EOF, XWPCP_EOF_SIZE);

        /* 加入到发送队列 */
        xwpcp_txq_add_tail(xwpcp, car);
        xwos_sem_post(&xwpcp->txq.qsem);

        if (txhbuf) {
                *txhbuf = (xwpcp_txh_t)car;
        }
        return XWOK;

err_bma_alloc:
        xwpcp_txq_carrier_free(xwpcp, car);
err_car_alloc:
        return rc;
}

/**
 * @brief 连接远端
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_connect(struct xwpcp * xwpcp)
{
        xwsq_t hwifst;
        xwsz_t cnt;
        xwer_t rc;

        cnt = 0;
        rc = XWOK;
        do {
                xwaop_read(xwsq_t, &xwpcp->hwifst, &hwifst);
                if (XWPCP_HWIFST_CONNECT & hwifst) {
                        break;
                }
                rc = xwpcp_tx_cmd_connect(xwpcp);
                if (XWOK == rc) {
                        rc = xwpcp_doze(1);
                        if (rc < 0) {
                                break;
                        }
                        cnt++;
                } else {
                        break;
                }
        } while (cnt < XWPCP_RETRY_NUM);
        if ((XWOK == rc) && (XWPCP_RETRY_NUM == cnt)) {
                xwpcp_hwifal_notify(xwpcp, XWPCP_HWIFNTF_NETUNREACH);
                xwpcp_doze(3);
        }/* else {} */
        return rc;
}

/**
 * @brief 指示数据帧的发送状态
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] car: 发送装置的指针
 */
static __xwmd_code
void xwpcp_finish_tx(struct xwpcp * xwpcp, struct xwpcp_carrier * car)
{
        xwu8_t ack;
        xwu8_t rmtid;

        ack = xwpcp->txq.remote.ack;
        rmtid = xwpcp->txq.remote.id;
        xwpcplogf(DEBUG, "[txthd] Remote ACK:0x%X, Remote ID:0x%X\n", ack, rmtid);
        if (rmtid != car->slot->tx.frm.head.id) {
                xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_CONNECT, NULL, NULL);
                xwpcp->txq.tmp = car;
                xwpcplogf(DEBUG, "[txthd] Remote ACK ID error!\n");
        } else {
                switch (ack) {
                case XWPCP_ACK_OK:
                        xwaop_add(xwu32_t, &xwpcp->txq.cnt, 1, NULL, NULL);
                        xwaop_write(xwu32_t, &car->state, XWPCP_CRS_FINISH, NULL);
                        xwos_splk_lock(&xwpcp->txq.notiflk);
                        if (car->slot->tx.ntfcb) {
                                car->slot->tx.ntfcb(xwpcp, (xwpcp_txh_t)car,
                                                    xwpcp_callback_rc[XWPCP_ACK_OK],
                                                    car->slot->tx.cbarg);
                        }/* else {} */
                        xwos_splk_unlock(&xwpcp->txq.notiflk);
                        xwmm_bma_free(xwpcp->mempool, car->slot);
                        car->slot = NULL;
                        xwpcp_txq_carrier_free(xwpcp, car);
                        break;
                case XWPCP_ACK_EALREADY:
                        xwpcplogf(DEBUG, "[txthd] Msg is already TX-ed!\n");
                        xwaop_add(xwu32_t, &xwpcp->txq.cnt, 1, NULL, NULL);
                        xwaop_write(xwu32_t, &car->state, XWPCP_CRS_FINISH, NULL);
                        xwos_splk_lock(&xwpcp->txq.notiflk);
                        if (car->slot->tx.ntfcb) {
                                car->slot->tx.ntfcb(xwpcp, (xwpcp_txh_t)car,
                                                    xwpcp_callback_rc[XWPCP_ACK_OK],
                                                    car->slot->tx.cbarg);
                        }/* else {} */
                        xwos_splk_unlock(&xwpcp->txq.notiflk);
                        xwmm_bma_free(xwpcp->mempool, car->slot);
                        car->slot = NULL;
                        xwpcp_txq_carrier_free(xwpcp, car);
                        break;
                case XWPCP_ACK_ECONNRESET:
                        xwpcplogf(WARNING, "[txthd] Link has been severed!\n");
                        xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_CONNECT,
                                  NULL, NULL);
                        xwpcp->txq.tmp = car;
                        break;
                case XWPCP_ACK_NOMEM:
                        xwpcplogf(WARNING, "[txthd] Remote has no memory!\n");
                        xwpcp->txq.tmp = car;
                        break;
                default:
                        XWPCP_BUG();
                        xwpcp->txq.tmp = car;
                        break;
                }
        }
}

/**
 * @brief 发送帧
 * @param[in] xwpcp: XWPCP对象的指针
 * @param[in] car: 发送装置的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_tx_frm(struct xwpcp * xwpcp, struct xwpcp_carrier * car)
{
        xwu32_t txcnt;
        xwu8_t id;
        xwsz_t calsz;
        xwsz_t cnt;
        xwsq_t lkst;
        union xwos_ulock ulk;
        xwsq_t hwifst;
        xwer_t rc;

        /* 填充Head剩余字节 */
        xwaop_read(xwu32_t, &xwpcp->txq.cnt, &txcnt);
        id = XWPCP_ID(txcnt);
        car->slot->tx.frm.head.id = id;
        car->slot->tx.frm.head.chksum = 0;
        calsz = XWPCP_FRMHEAD_SIZE(car->slot->tx.frm.head.headsize);
        car->slot->tx.frm.head.chksum =
                        xwpcp_cal_head_chksum((xwu8_t *)&car->slot->tx.frm.head,
                                              calsz);

        /* 发送 */
        cnt = 0;
        ulk.osal.mtx = &xwpcp->txq.csmtx;
        rc = xwos_mtx_lock(&xwpcp->txq.csmtx);
        if (rc < 0) {
                goto err_mtx_lock;
        }
        if (car->slot->tx.frm.head.qos & XWPCP_MSG_QOS_ACK_MSK) {
                xwaop_s1m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX, NULL, NULL);
                do {
                        xwpcplogf(DEBUG,
                                  "[txthd] carrier(%p), ID:0x%X, cnt:0x%X\n",
                                  car, id, cnt);
                        rc = xwpcp_hwifal_tx(xwpcp,
                                             (xwu8_t *)&car->slot->tx.frm,
                                             car->slot->tx.frmsize);
                        if (rc < 0) {
                                xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX,
                                          NULL, NULL);
                                xwos_mtx_unlock(&xwpcp->txq.csmtx);
                                goto err_if_tx;
                        }
                        rc = xwos_cond_wait_to(&xwpcp->txq.cscond,
                                               ulk, XWOS_LK_MTX, NULL,
                                               xwtm_ft(XWPCP_RETRY_PERIOD), &lkst);
                        if (XWOK == rc) {
                                xwos_mtx_unlock(&xwpcp->txq.csmtx);
                                xwpcp_finish_tx(xwpcp, car);
                                break;
                        } else if (-ETIMEDOUT == rc) {
                                rc = xwos_mtx_lock(&xwpcp->txq.csmtx);
                                if (rc < 0) {
                                        xwaop_c0m(xwsq_t, &xwpcp->hwifst,
                                                  XWPCP_HWIFST_TX, NULL, NULL);
                                        goto err_mtx_lock;
                                }
                                xwaop_read(xwsq_t, &xwpcp->hwifst, &hwifst);
                                if (XWPCP_HWIFST_TX & hwifst) {
                                        cnt++;
                                        rc = -ETIMEDOUT;
                                } else {
                                        xwos_mtx_unlock(&xwpcp->txq.csmtx);
                                        xwpcp_finish_tx(xwpcp, car);
                                        rc = XWOK;
                                        break;
                                }
                        } else {
                                xwaop_c0m(xwsq_t, &xwpcp->hwifst, XWPCP_HWIFST_TX,
                                          NULL, NULL);
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwos_mtx_unlock(&xwpcp->txq.csmtx);
                                }
                                break;
                        }
                } while (cnt < XWPCP_RETRY_NUM);
                if ((XWPCP_RETRY_NUM == cnt) && (-ETIMEDOUT == rc)) {
                        xwaop_c0m(xwsq_t, &xwpcp->hwifst,
                                  (XWPCP_HWIFST_TX | XWPCP_HWIFST_CONNECT),
                                  NULL, NULL);
                        xwos_mtx_unlock(&xwpcp->txq.csmtx);
                        xwpcp_doze(3);
                }
        } else {
                rc = xwpcp_hwifal_tx(xwpcp,
                                     (xwu8_t *)&car->slot->tx.frm,
                                     car->slot->tx.frmsize);
                if (rc < 0) {
                        xwos_mtx_unlock(&xwpcp->txq.csmtx);
                        goto err_if_tx;
                }
                xwos_mtx_unlock(&xwpcp->txq.csmtx);
                xwos_splk_lock(&xwpcp->txq.notiflk);
                if (car->slot->tx.ntfcb) {
                        car->slot->tx.ntfcb(xwpcp, (xwpcp_txh_t)car,
                                            xwpcp_callback_rc[XWPCP_ACK_OK],
                                            car->slot->tx.cbarg);
                }/* else {} */
                xwos_splk_unlock(&xwpcp->txq.notiflk);
                xwmm_bma_free(xwpcp->mempool, car->slot);
                car->slot = NULL;
                xwpcp_txq_carrier_free(xwpcp, car);
        }

err_if_tx:
err_mtx_lock:
        return rc;
}

/**
 * @brief XWPCP的发送线程状态机
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_txfsm(struct xwpcp * xwpcp)
{
        struct xwpcp_carrier * car;
        xwu32_t state;
        xwsq_t hwifst;
        xwer_t rc;

        xwaop_read(xwsq_t, &xwpcp->hwifst, &hwifst);
        if (XWPCP_HWIFST_CONNECT & hwifst) {
                /* 选择一个待发送的帧 */
                if (xwpcp->txq.tmp) {
                        car = xwpcp->txq.tmp;
                        xwpcp->txq.tmp = NULL;
                } else {
                        rc = xwos_sem_wait(&xwpcp->txq.qsem);
                        if (rc < 0) {
                                goto err_txqsem_wait;
                        }
                        car = xwpcp_txq_choose(xwpcp);
                        XWPCP_BUG_ON(is_err(car));
                }
                xwpcplogf(DEBUG,
                          "[txthd] Choose carrier(%p), ID:0x%X, port:0x%X\n",
                          car,
                          car->slot->tx.frm.head.id,
                          car->slot->tx.frm.head.port);
                rc = xwaop_teq_then_write(xwu32_t, &car->state,
                                          XWPCP_CRS_READY,
                                          XWPCP_CRS_INPROGRESS,
                                          &state);
                if (XWOK == rc) {
                        rc = xwpcp_tx_frm(xwpcp, car);
                        if (rc < 0) {
                                goto err_tx_frm;
                        }
                } else {
                        if (XWPCP_CRS_ABORT == state) {
                                xwos_splk_lock(&xwpcp->txq.notiflk);
                                if (car->slot->tx.ntfcb) {
                                        car->slot->tx.ntfcb(xwpcp,
                                                            (xwpcp_txh_t)car,
                                                            -ECONNABORTED,
                                                            car->slot->tx.cbarg);
                                }/* else {} */
                                xwos_splk_unlock(&xwpcp->txq.notiflk);
                                xwmm_bma_free(xwpcp->mempool, car->slot);
                                car->slot = NULL;
                                xwpcp_txq_carrier_free(xwpcp, car);
                        }
                }
        } else {
                /* 连接 */
                rc = xwpcp_connect(xwpcp);
                if (rc < 0) {
                        goto err_connect;
                }
        }
        return XWOK;

err_tx_frm:
        xwpcp->txq.tmp = car;
err_txqsem_wait:
err_connect:
        return rc;
}

/**
 * @brief XWPCP的发送线程
 * @param[in] xwpcp: XWPCP对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwpcp_txthd(struct xwpcp * xwpcp)
{
        xwer_t rc;

        rc = XWOK;
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwpcplogf(DEBUG, "[txthd] Start freezing ...\n");
                        rc = xwos_cthd_freeze();
                        if (rc < 0) {
                                xwpcplogf(DEBUG,
                                          "[txthd] Failed to freeze ... rc: %d\n",
                                          rc);
                        }
                        xwpcplogf(DEBUG, "[txthd] Resuming ...\n");
                } else {
                        rc = xwpcp_txfsm(xwpcp);
                        if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                xwpcplogf(DEBUG,
                                          "[txthd] Interrupted ... rc: %d\n",
                                          rc);
                        } else if (-EOVERFLOW == rc) {
                                xwpcplogf(DEBUG,
                                          "[txthd] Buffer of HWIF is overflow!\n");
                                xwpcp_doze(1);
                        } else if ((-ETIMEDOUT != rc) && (XWOK != rc)) {
                                xwpcplogf(ERR,
                                          "[txthd] xwpcp_txfsm() ... rc: %d.\n",
                                          rc);
                                break;
                        }/* else {} */
                }
        }
        if (xwpcp->txq.tmp) {
                xwpcp_txq_add_head(xwpcp, xwpcp->txq.tmp);
                xwpcp->txq.tmp = NULL;
                xwos_sem_post(&xwpcp->txq.qsem);
        }/* else {} */
        return rc;
}

/**
 * @brief 短暂暂停一下XWPCP的线程
 * @return 错误码
 */
static __xwmd_code
xwer_t xwpcp_doze(xwu32_t cnt)
{
        return xwos_cthd_sleep(XWPCP_RETRY_PERIOD * cnt);
}
