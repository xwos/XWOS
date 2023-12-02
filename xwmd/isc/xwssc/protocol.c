/**
 * @file
 * @brief SOC间点对点通讯协议：协议层
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/crc32.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/hwifal.h>
#include <xwmd/isc/xwssc/mif.h>

/**
 * @ingroup xwmd_isc_xwssc_protocol
 * @{
 */

/**
 * @brief 控制帧模板：连接帧
 */
static __xwmd_rodata
const xwu8_t xwssc_frm_connection[] = {
        (xwu8_t)XWSSC_SOF,
        (xwu8_t)XWSSC_SOF,
        (xwu8_t)0x66, /* 帧头长度 | 长度的镜像 */
        (xwu8_t)0x7E, /* 帧头校验和 */
        (xwu8_t)0x0, /* 端口：不使用，默认为0 */
        (xwu8_t)0x0, /* ID：不使用，默认为0 */
        ((xwu8_t)XWSSC_MSG_QOS_3 |
         (xwu8_t)XWSSC_FLAG_CONNECT), /* Qos: 可靠消息 + CRC校验码 + CONNECT */
        (xwu8_t)0x8, /* SDU长度 */
        (xwu8_t)'X', /* sdu[0]: protocol 0 */
        (xwu8_t)'W', /* sdu[1]: protocol 1 */
        (xwu8_t)'S', /* sdu[2]: protocol 2 */
        (xwu8_t)'S', /* sdu[3]: protocol 3 */
        (xwu8_t)'C', /* sdu[4]: protocol 4 */
        (xwu8_t)XWSSC_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWSSC_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWSSC_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)0x78, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0x59, /* CRC32 第二字节 */
        (xwu8_t)0xB2, /* CRC32 第三字节 */
        (xwu8_t)0x85, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
};

/**
 * @brief 控制帧模板：连接应答帧
 */
static __xwmd_rodata
const xwu8_t xwssc_ackfrm_connection[] = {
        (xwu8_t)XWSSC_SOF,
        (xwu8_t)XWSSC_SOF,
        (xwu8_t)0x66, /* 帧头长度 | 长度的镜像 */
        (xwu8_t)0xFD, /* 帧头校验和 */
        (xwu8_t)0x0, /* 端口：不使用，默认为0 */
        (xwu8_t)0x0, /* ID：不使用，默认为0 */
        ((xwu8_t)XWSSC_MSG_QOS_3 |
         (xwu8_t)XWSSC_FLAG_CONNECT |
         (xwu8_t)XWSSC_FLAG_ACK), /* (可靠消息 + CRC校验码) + CONNECT + ACK */
        (xwu8_t)0x8, /* SDU长度 */
        (xwu8_t)'X', /* sdu[0]: protocol 0 */
        (xwu8_t)'W', /* sdu[1]: protocol 1 */
        (xwu8_t)'S', /* sdu[2]: protocol 2 */
        (xwu8_t)'S', /* sdu[3]: protocol 3 */
        (xwu8_t)'C', /* sdu[4]: protocol 4 */
        (xwu8_t)XWSSC_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWSSC_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWSSC_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)0x78, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0x59, /* CRC32 第二字节 */
        (xwu8_t)0xB2, /* CRC32 第三字节 */
        (xwu8_t)0x85, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
};

/**
 * @brief 数据应答帧模板
 */
static __xwmd_rodata
const xwu8_t xwssc_ackfrm_sdu[] = {
        (xwu8_t)XWSSC_SOF,
        (xwu8_t)XWSSC_SOF,
        (xwu8_t)0x66, /* 帧头长度 | mirror */
        (xwu8_t)0x0, /* 帧头校验和，依据实际情况填充 */
        (xwu8_t)0x0, /* 端口，依据实际情况填充 */
        (xwu8_t)0x0, /* ID，依据实际情况填充 */
        ((xwu8_t)XWSSC_MSG_QOS_3 |
         (xwu8_t)XWSSC_FLAG_ACK), /* (可靠消息 + CRC校验码) + ACK */
        (xwu8_t)0x1, /* SDU长度 */
        (xwu8_t)0, /* sdu[0]: 应答，依据实际情况填充 */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节），依据实际情况填充 */
        (xwu8_t)0, /* CRC32 第二字节，依据实际情况填充 */
        (xwu8_t)0, /* CRC32 第三字节，依据实际情况填充 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节），依据实际情况填充 */
        (xwu8_t)XWSSC_EOF,
        (xwu8_t)XWSSC_EOF,
};

/**
 * @brief 应答错误对照表
 */
static __xwmd_rodata
const xwer_t xwssc_callback_rc[XWSSC_ACK_NUM] = {
        [XWSSC_ACK_OK] = XWOK,
        [XWSSC_ACK_ECONNRESET] = -ECONNRESET,
        [XWSSC_ACK_EALREADY] = -EALREADY,
        [XWSSC_ACK_NOMEM] = -ENOMEM,
};

/**
 * @brief 数据长度编解码移位表
 */
static __xwmd_rodata
const xwu8_t xwssc_ecsdusz_shift_table[] = {
        0, 7, 14, 21, 28, 35, 42, 49,
        56, 63, 70, 77, 84, 91, 98, 105,
        112, 119, 126, 133,
};

static __xwmd_code
xwer_t xwssc_chk_frm(struct xwssc_frm * frm, xwsz_t sdusize);

static __xwmd_code
void xwssc_rxq_pub(struct xwssc * xwssc,
                   union xwssc_slot * slot,
                   xwu8_t port);

static __xwmd_code
xwer_t xwssc_rx_connection(struct xwssc * xwssc, struct xwssc_frm * frm);

static __xwmd_code
xwer_t xwssc_rx_ack_connection(struct xwssc * xwssc, struct xwssc_frm * frm);

static __xwmd_code
xwer_t xwssc_rx_sdu(struct xwssc * xwssc, union xwssc_slot * slot);

static __xwmd_code
xwer_t xwssc_rx_ack_sdu(struct xwssc * xwssc, struct xwssc_frm * frm);

static __xwmd_code
xwer_t xwssc_rxfsm(struct xwssc * xwssc);

static __xwmd_code
void xwssc_txq_add_head(struct xwssc * xwssc, struct xwssc_carrier * car);

static __xwmd_code
void xwssc_txq_add_tail(struct xwssc * xwssc, struct xwssc_carrier * car);

static __xwmd_code
xwer_t xwssc_tx_connection(struct xwssc * xwssc);

static __xwmd_code
xwer_t xwssc_tx_ack_connection(struct xwssc * xwssc);

static __xwmd_code
xwer_t xwssc_connect(struct xwssc * xwssc);

static __xwmd_code
void xwssc_finish_tx(struct xwssc * xwssc, struct xwssc_carrier * car);

static __xwmd_code
xwer_t xwssc_tx_frm(struct xwssc * xwssc, struct xwssc_carrier * car);

static __xwmd_code
xwer_t xwssc_txfsm(struct xwssc * xwssc);

static __xwmd_code
xwer_t xwssc_doze(xws32_t cnt);

/**
 * @brief 增加XWSSC的引用计数，防止其被停止
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWSSC已经停止运行
 */
__xwmd_code
xwer_t xwssc_grab(struct xwssc * xwssc)
{
        return xwos_object_grab(&xwssc->xwobj);
}

/**
 * @brief 减少XWSSC的引用计数
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWSSC没有在运行
 */
__xwmd_code
xwer_t xwssc_put(struct xwssc * xwssc)
{
        return xwos_object_put(&xwssc->xwobj);
}

/**
 * @brief 编码数据长度
 * @param[in] sdusize: 数据长度
 * @param[out] ecsdusz: 指向缓冲区的指针，此缓冲区用于输出编码后的数据数组
 * @param[out] ecsize: 指向缓冲区的指针，此缓冲区用于输出编码后的数据数组的长度
 */
__xwmd_code
void xwssc_encode_sdusize(xwsz_t sdusize, xwu8_t * ecsdusz, xwu8_t * ecsize)
{
        xwu8_t i;

        i = 0;
        while (true) {
                ecsdusz[i] = sdusize & (xwu8_t)0x7F;
                sdusize >>= 7U; // cppcheck-suppress [misra-c2012-17.8]
                if ((xwsz_t)0 != sdusize) {
                        ecsdusz[i] |= (xwu8_t)0x80;
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
void xwssc_decode_sdusize(xwu8_t * ecsdusz, xwsz_t * sdusize)
{
        xwsz_t dcsdusz;
        xwsz_t sft;
        xwsq_t i;

        i = 0;
        dcsdusz = 0;
        while (true) {
                sft = xwssc_ecsdusz_shift_table[i];
                dcsdusz |= ((xwsz_t)(ecsdusz[i] & (xwu8_t)0x7F) << sft);
                if ((xwu8_t)0 != (ecsdusz[i] & (xwu8_t)0x80)) {
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
xwu8_t xwssc_cal_head_chksum(xwu8_t data[], xwsz_t size)
{
        xwu16_t chksum;
        xwu8_t i;

        chksum = 0;
        for (i = 0; i < size; i++) {
                chksum += (xwu16_t)data[i];
        }
        while ((xwu16_t)0 != (chksum & (xwu16_t)0xFF00)) {
                chksum = (chksum >> (xwu16_t)8) + (chksum & (xwu16_t)0xFF);
        }
        chksum ^= (xwu16_t)0xFF;
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
bool xwssc_chk_head(xwu8_t data[], xwsz_t size)
{
        xwu16_t chksum;
        xwu8_t i;

        chksum = 0;
        for (i = 0; i < size; i++) {
                chksum += (xwu16_t)data[i];
        }
        while ((xwu16_t)0 != (chksum & (xwu16_t)0xFF00)) {
                chksum = (chksum >> (xwu16_t)8) + (chksum & (xwu16_t)0xFF);
        }
        return (chksum == (xwu16_t)0xFF);
}

/**
 * @brief 校验帧是否合法
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] frm: 待校验的消息帧的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EBADMSG: 校验错误
 */
static __xwmd_code
xwer_t xwssc_chk_frm(struct xwssc_frm * frm, xwsz_t sdusize)
{
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        sdupos = XWSSC_SDUPOS(&frm->head);
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
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] slot: 待发布的帧槽指针
 * @param[in] port: 端口值
 */
static __xwmd_code
void xwssc_rxq_pub(struct xwssc * xwssc,
                   union xwssc_slot * slot,
                   xwu8_t port)
{
        xwssclogf(xwssc, DEBUG,
                  "[R][RX][SDU] Publish slot::0x%lX, port:0x%X, frmsize:0x%u\n",
                  (xwptr_t)slot, port, slot->rx.frmsize);
        xwos_splk_lock(&xwssc->rxq.lock[port]);
        xwlib_bclst_add_tail(&xwssc->rxq.q[port], &slot->rx.node);
        xwos_splk_unlock(&xwssc->rxq.lock[port]);
        xwos_sem_post(&xwssc->rxq.sem[port]);
}

/**
 * @brief 从接收队列中选择一条数据帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] port: 端口
 * @return 帧槽的指针
 */
__xwmd_code
union xwssc_slot * xwssc_rxq_choose(struct xwssc * xwssc, xwu8_t port)
{
        union xwssc_slot * slot;
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&xwssc->rxq.lock[port], &cpuirq);
        slot = xwlib_bclst_first_entry(&xwssc->rxq.q[port],
                                       union xwssc_slot,
                                       rx.node);
        xwlib_bclst_del_init(&slot->rx.node);
        xwos_splk_unlock_cpuirqrs(&xwssc->rxq.lock[port], cpuirq);
        return slot;
}

/**
 * @brief 接收同步帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] frm: 消息帧
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_rx_connection(struct xwssc * xwssc, struct xwssc_frm * frm)
{
        xwu8_t * sdupos;
        xwer_t rc;
        char proto[6];

        sdupos = XWSSC_SDUPOS(&frm->head);
        proto[0] = sdupos[0];
        proto[1] = sdupos[1];
        proto[2] = sdupos[2];
        proto[3] = sdupos[3];
        proto[4] = sdupos[4];
        proto[5] = '\0';
        xwssclogf(xwssc, DEBUG, "[R][RX][CONN] proto:%s-%d.%d.%d\n",
                  proto, sdupos[5], sdupos[6], sdupos[7]);
        if ((0 == strcmp(proto, "XWSSC")) &&
            (XWSSC_VERSION_MAJOR == sdupos[5]) &&
            (XWSSC_VERSION_MINOR == sdupos[6])) {
                rc = xwssc_tx_ack_connection(xwssc);
                if (XWOK == rc) {
                        xwaop_write(xwu32_t, &xwssc->rxq.cnt, 0, NULL);
                }
        } else {
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief 接收同步应答帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] frm: 消息帧
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_rx_ack_connection(struct xwssc * xwssc, struct xwssc_frm * frm)
{
        xwu8_t * sdupos;
        xwsq_t hwifst;
        char proto[6];

        sdupos = XWSSC_SDUPOS(&frm->head);
        proto[0] = sdupos[0];
        proto[1] = sdupos[1];
        proto[2] = sdupos[2];
        proto[3] = sdupos[3];
        proto[4] = sdupos[4];
        proto[5] = '\0';

        xwssclogf(xwssc, DEBUG, "[R][RX][CONNACK] proto:%s-%d.%d.%d\n",
                  proto, sdupos[5], sdupos[6], sdupos[7]);
        xwaop_read(xwsq_t, &xwssc->hwifst, &hwifst);
        if (!((xwsq_t)XWSSC_HWIFST_CONNECT & hwifst)) {
                if ((0 == strcmp(proto, "XWSSC")) &&
                    (XWSSC_VERSION_MAJOR == sdupos[5]) &&
                    (XWSSC_VERSION_MINOR == sdupos[6])) {
                        xwaop_write(xwu32_t, &xwssc->txq.cnt, 0, NULL);
                        xwaop_s1m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_CONNECT,
                                  NULL, NULL);
                        xwssc_hwifal_notify(xwssc, XWSSC_HWIFNTF_CONNECT);
                }
        }
        return XWOK;
}

/**
 * @brief 接收数据帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] slot: 接收到的帧槽指针
 * @return 错误码
 * @note
 * + 数据帧：
 *   + (xwu8_t)XWSSC_SOF,
 *   + (xwu8_t)XWSSC_SOF,
 *   + (xwu8_t)帧头长度 | 镜像反转
 *   + (xwu8_t)帧头校验和
 *   + (xwu8_t)端口
 *   + (xwu8_t)ID
 *   + (xwu8_t)QoS
 *   + (xwu8_t)ecsdusz[0:n] 编码的数据长度
 *   + (xwu8_t)sdu[0:msg->size - 1] 数据
 *   + (xwu8_t)sdu[msg->size + 0]/CRC32 最高有效字节
 *   + (xwu8_t)sdu[msg->size + 1]/CRC32 第二字节
 *   + (xwu8_t)sdu[msg->size + 2]/CRC32 第三字节
 *   + (xwu8_t)sdu[msg->size + 3]/CRC32 最低有效字节
 *   + (xwu8_t)XWSSC_EOF,
 *   + (xwu8_t)XWSSC_EOF,
 */
static __xwmd_code
xwer_t xwssc_rx_sdu(struct xwssc * xwssc, union xwssc_slot * slot)
{
        xwu32_t rxcnt;
        xwu8_t rmtid;
        xwu8_t lclid;
        xwu8_t port;
        xwu8_t qos;
        xwer_t rc;

        xwaop_read(xwu32_t, &xwssc->rxq.cnt, &rxcnt);
        lclid = XWSSC_ID(rxcnt);
        rmtid = slot->rx.frm.head.id;
        port = slot->rx.frm.head.port;
        qos = slot->rx.frm.head.qos;
        xwssclogf(xwssc, DEBUG,
                  "[R][RX][SDU] slot:0x%lX, port:0x%X, qos:0x%X, frmsize:0x%X, "
                  "Remote ID:0x%X, Local ID:0x%X, Rx counter:0x%X\n",
                  (xwptr_t)slot,
                  port, qos, slot->rx.frmsize, rmtid, lclid, rxcnt);
        if ((xwu8_t)0 != (qos & (xwu8_t)XWSSC_MSG_QOS_RELIABLE_MSK)) {
                if (rmtid == lclid) {
                        /* 收到数据 */
                        rc = xwssc_tx_ack_sdu(xwssc, port, rmtid, XWSSC_ACK_OK);
                        if (XWOK == rc) {
                                xwaop_add(xwu32_t, &xwssc->rxq.cnt, 1, NULL, NULL);
                                xwssc_rxq_pub(xwssc, slot, port);
                        }
                } else if (XWSSC_ID(rmtid + (xwu8_t)1) == lclid) {
                        /* 收到重复的数据 */
                        xwssc_tx_ack_sdu(xwssc, port, rmtid, XWSSC_ACK_EALREADY);
                        rc = -EALREADY;
                } else {
                        /* 连接被复位 */
                        xwssc_tx_ack_sdu(xwssc, port, rmtid, XWSSC_ACK_ECONNRESET);
                        rc = -ECONNRESET;
                }
        } else {
                xwssc_rxq_pub(xwssc, slot, port);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 接收数据应答帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] frm: 消息帧
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_rx_ack_sdu(struct xwssc * xwssc, struct xwssc_frm * frm)
{
        xwu8_t * sdupos;
        xwu8_t rmtid;
        xwu8_t ack;
        xwer_t rc;
        xwsq_t hwifst;

        sdupos = XWSSC_SDUPOS(&frm->head);
        rmtid = XWSSC_ID(frm->head.id);
        ack = sdupos[0];
        xwssclogf(xwssc, DEBUG, "[R][SDUACK] ID:0x%X, ACK:0x%X\n", rmtid, ack);
        rc = xwos_mtx_lock(&xwssc->txq.csmtx);
        if (rc < 0) {
                goto err_mtx_lock;
        }
        xwaop_read(xwsq_t, &xwssc->hwifst, &hwifst);
        if ((xwsq_t)0 != ((xwsq_t)XWSSC_HWIFST_TX & hwifst)) {
                xwssc->txq.remote.ack = ack;
                xwssc->txq.remote.id = rmtid;
                xwaop_c0m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_TX, NULL, NULL);
                xwos_mtx_unlock(&xwssc->txq.csmtx);
                xwos_cond_unicast(&xwssc->txq.cscond);
        } else {
                xwos_mtx_unlock(&xwssc->txq.csmtx);
                rc = -ENOMSG;
        }

err_mtx_lock:
        return rc;
}

/**
 * @brief XWSSC的接收线程状态机
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_rxfsm(struct xwssc * xwssc)
{
        union {
                xwu8_t data[XWSSC_FRM_ONSTACK_MAXSIZE];
                struct xwssc_frm frm;
        } stream;
        union {
                union xwssc_slot * slot;
                void * raw;
        } mem;
        xwsz_t sdusize;
        xwu8_t qos;
        xwer_t rc;

        mem.raw = NULL;
        /* 接收消息头 */
        rc = xwssc_hwifal_rx_head(xwssc, &stream.frm);
        if (rc < 0) {
                goto err_rx_head;
        }
        qos = stream.frm.head.qos;
        xwssc_decode_sdusize(stream.frm.head.ecsdusz, &sdusize);
        xwssclogf(xwssc, DEBUG, "[R][HEAD] port:0x%X, ID:0x%X, qos:0x%X\n",
                  stream.frm.head.port, stream.frm.head.id, qos);

        /* 接收消息体 */
        if (XWSSC_FLAG_SDU == ((xwu8_t)XWSSC_FLAG_MSK & qos)) {
                xwsz_t ecsize;
                xwsz_t need;
                xwsz_t neednum;
                xwssq_t odr;

                /* 计算所需的内存大小 */
                ecsize = XWSSC_ECSIZE(&stream.frm.head);
                need = (sizeof(union xwssc_slot) + ecsize + sdusize +
                        XWSSC_CRC32_SIZE + XWSSC_EOF_SIZE);
                neednum = XWBOP_DIV_ROUND_UP(need, XWSSC_MEMBLK_SIZE);
                odr = xwbop_fls(xwsz_t, neednum);
                if ((odr < 0) || ((XWSSC_MEMBLK_SIZE << (xwsz_t)odr) < need)) {
                        odr++;
                }
                /* 申请用于接收数据的帧槽 */
                rc = xwmm_bma_alloc(xwssc->mempool, (xwsq_t)odr, &mem.raw);
                if (rc < 0) {
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwssc_tx_ack_sdu(xwssc,
                                         stream.frm.head.port, stream.frm.head.id,
                                         XWSSC_ACK_NOMEM);
                        goto err_bma_alloc;
                }
                /* 初始化帧槽 */
                xwlib_bclst_init_node(&mem.slot->rx.node);
                mem.slot->rx.frmsize = (sizeof(struct xwssc_frm) + ecsize + sdusize +
                                        XWSSC_CRC32_SIZE + XWSSC_EOF_SIZE);
                // cppcheck-suppress [misra-c2012-17.7]
                /* memset(mem.slot->rx.frm.sof, XWSSC_SOF, XWSSC_SOF_SIZE); */
                // cppcheck-suppress [misra-c2012-17.7]
                memcpy((xwu8_t *)&mem.slot->rx.frm.head, (xwu8_t *)&stream.frm.head,
                       sizeof(struct xwssc_frmhead) + ecsize);
                /* 接收消息体 */
                rc = xwssc_hwifal_rx_body(xwssc, &mem.slot->rx.frm, sdusize);
                if (rc < 0) {
                        goto err_rx_body;
                }
                /* 校验消息 */
                if ((xwu8_t)0 != ((xwu8_t)XWSSC_MSG_QOS_CHKSUM_MSK & qos)) {
                        rc = xwssc_chk_frm(&mem.slot->rx.frm, sdusize);
                        if (rc < 0) {
                                goto err_badmsg;
                        }
                }
                /* 处理消息 */
                rc = xwssc_rx_sdu(xwssc, mem.slot);
                if (rc < 0) {
                        goto err_process;
                }
        } else {
                /* 接收消息体 */
                rc = xwssc_hwifal_rx_body(xwssc, &stream.frm, sdusize);
                if (rc < 0) {
                        goto err_rx_body;
                }
                /* 校验消息 */
                if ((xwu8_t)0 != ((xwu8_t)XWSSC_MSG_QOS_CHKSUM_MSK & qos)) {
                        rc = xwssc_chk_frm(&stream.frm, sdusize);
                        if (rc < 0) {
                                goto err_badmsg;
                        }
                }
                /* 处理消息 */
                if ((xwu8_t)0 != ((xwu8_t)XWSSC_FLAG_CONNECT & qos)) {
                        if ((xwu8_t)0 != ((xwu8_t)XWSSC_FLAG_ACK & qos)) {
                                rc = xwssc_rx_ack_connection(xwssc, &stream.frm);
                        } else {
                                rc = xwssc_rx_connection(xwssc, &stream.frm);
                        }
                } else {
                        rc = xwssc_rx_ack_sdu(xwssc, &stream.frm);
                }
                if (rc < 0) {
                        goto err_process;
                }
        }
        return XWOK;

err_process:
err_badmsg:
err_rx_body:
        if (NULL != mem.raw) {
                // cppcheck-suppress [misra-c2012-17.7]
                xwmm_bma_free(xwssc->mempool, mem.raw);
                mem.raw = NULL;
        }
err_bma_alloc:
err_rx_head:
        return rc;
}

/**
 * @brief XWSSC的接收线程
 * @param[in] xwssc: XWSSC对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwssc_rxthd(struct xwssc * xwssc)
{
        xwer_t rc;

        rc = XWOK;
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwssclogf(xwssc, DEBUG, "[R] Start freezing ...\n");
                        rc = xwos_cthd_freeze();
                        if (rc < 0) {
                                xwssclogf(xwssc, DEBUG,
                                          "[R] Failed to freeze ... <rc:%d>\n",
                                          rc);
                                xwssc_doze(1); // cppcheck-suppress [misra-c2012-17.7]
                        }
                        xwssclogf(xwssc, DEBUG, "[R] Resuming ...\n");
                }
                rc = xwssc_rxfsm(xwssc);
                switch (rc) {
                case XWOK:
                        break;
                case -EAGAIN:
                        xwssclogf(xwssc, WARNING,
                                  "[R] Data stream error! Wait for SOF.\n");
                        break;
                case -ETIMEDOUT:
                        xwssclogf(xwssc, DEBUG, "[R] Timeout!\n");
                        break;
                case -EINTR:
                case -ERESTARTSYS:
                        xwssclogf(xwssc, DEBUG, "[R] Interrupted!\n");
                        break;
                case -ENOMEM:
                        xwssclogf(xwssc, DEBUG, "[R] No Memory\n");
                        break;
                case -EBADMSG:
                        xwssclogf(xwssc, WARNING, "[R] Bad Message Frame!\n");
                        break;
                case -EPERM:
                        xwssclogf(xwssc, ERR, "[R] Protocol not match!\n");
                        break;
                case -EALREADY:
                        xwssclogf(xwssc, WARNING, "[R] Repeated Message Frame!\n");
                        break;
                case -ECONNRESET:
                        xwssclogf(xwssc, WARNING, "[R] Connection Reset!\n");
                        break;
                case -ENOMSG:
                        xwssclogf(xwssc, WARNING, "[R] No sending message!\n");
                        break;
                default:
                        xwssclogf(xwssc, CRIT, "[R] BUG!!! xwssc_rxfsm() returns %d!\n", rc);
                        XWSSC_BUG();
                        break;
                }
        }
        return rc;
}

/**
 * @brief 申请发送装置
 * @param[in] xwssc: XWSSC对象的指针
 * @return 发送装置的指针
 */
__xwmd_code
struct xwssc_carrier * xwssc_txq_carrier_alloc(struct xwssc * xwssc)
{
        struct xwssc_carrier * car;
        xwssq_t caridx;

        caridx = xwbmpaop_ffz_then_s1i(xwssc->txq.carbmp, XWSSC_MEMBLK_NUM);
        if (caridx < 0) {
                car = NULL;
        } else {
                car = &xwssc->txq.car[caridx];
                xwlib_bclst_init_node(&car->node);
                XWSSC_BUG_ON(car->state != (xwu32_t)XWSSC_CRS_IDLE);
        }
        return car;
}

/**
 * @brief 释放发送装置
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] car: 发送装置的指针
 */
__xwmd_code
void xwssc_txq_carrier_free(struct xwssc * xwssc, struct xwssc_carrier * car)
{
        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_IDLE, NULL);
        // cppcheck-suppress [misra-c2012-17.7]
        xwbmpaop_t1i_then_c0i(xwssc->txq.carbmp, car->idx);
}

/**
 * @brief 将待发送的帧放到发送队列的头部
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] car: 发送装置的指针
 */
static __xwmd_code
void xwssc_txq_add_head(struct xwssc * xwssc, struct xwssc_carrier * car)
{
        xwu8_t pri;

        pri = car->pri;
        xwos_splk_lock(&xwssc->txq.qlock);
        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_READY, NULL);
        xwlib_bclst_add_head(&xwssc->txq.q[pri], &car->node);
        if (!xwbmpop_t1i(xwssc->txq.qnebmp, (xwsq_t)pri)) {
                xwbmpop_s1i(xwssc->txq.qnebmp, (xwsq_t)pri);
        }
        xwos_splk_unlock(&xwssc->txq.qlock);
}

/**
 * @brief 将待发送的帧放到发送队列的尾部
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] car: 发送装置的指针
 */
static __xwmd_code
void xwssc_txq_add_tail(struct xwssc * xwssc, struct xwssc_carrier * car)
{
        xwu8_t pri;

        pri = car->pri;
        xwos_splk_lock(&xwssc->txq.qlock);
        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_READY, NULL);
        xwlib_bclst_add_tail(&xwssc->txq.q[pri], &car->node);
        if (!xwbmpop_t1i(xwssc->txq.qnebmp, (xwsq_t)pri)) {
                xwbmpop_s1i(xwssc->txq.qnebmp, (xwsq_t)pri);
        }
        xwos_splk_unlock(&xwssc->txq.qlock);
}

/**
 * @brief 从最高优先级的发送队列中选择一个待发送的帧槽
 * @param[in] xwssc: XWSSC对象的指针
 * @return 发送装置的指针
 */
__xwmd_code
struct xwssc_carrier * xwssc_txq_choose(struct xwssc * xwssc)
{
        struct xwssc_carrier * car;
        xwu8_t pri;

        xwos_splk_lock(&xwssc->txq.qlock);
        pri = (xwu8_t)xwbmpop_fls(xwssc->txq.qnebmp, XWSSC_PRI_NUM);
        car = xwlib_bclst_first_entry(&xwssc->txq.q[pri],
                                      struct xwssc_carrier,
                                      node);
        xwlib_bclst_del_init(&car->node);
        if (xwlib_bclst_tst_empty(&xwssc->txq.q[pri])) {
                xwbmpop_c0i(xwssc->txq.qnebmp, (xwsq_t)pri);
        }
        xwos_splk_unlock(&xwssc->txq.qlock);
        return car;
}

/**
 * @brief 发送同步帧
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_tx_connection(struct xwssc * xwssc)
{
        xwer_t rc;

        xwssclogf(xwssc, DEBUG, "[T][TX][CONN] Connecting ...\n");
        rc = xwos_mtx_lock(&xwssc->txq.csmtx);
        if (XWOK == rc) {
                rc = xwssc_hwifal_tx(xwssc,
                                     xwssc_frm_connection,
                                     sizeof(xwssc_frm_connection));
                xwos_mtx_unlock(&xwssc->txq.csmtx);
        }
        return rc;
}

/**
 * @brief 发送同步应答帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] rxcnt: 应答接收计数器
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_tx_ack_connection(struct xwssc * xwssc)
{
        xwer_t rc;

        xwssclogf(xwssc, DEBUG, "[R][TX][CONNACK] Connecting ...\n");
        rc = xwos_mtx_lock(&xwssc->txq.csmtx);
        if (XWOK == rc) {
                rc = xwssc_hwifal_tx(xwssc,
                                     xwssc_ackfrm_connection,
                                     sizeof(xwssc_ackfrm_connection));
                xwos_mtx_unlock(&xwssc->txq.csmtx);
        }
        return rc;
}

/**
 * @brief 发送数据应答帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[out] frm: 指向缓冲区的指针，此缓冲区被用于格式化一帧
 * @param[in] port: 端口
 * @param[in] id: 应答远端消息的id
 * @param[in] ack: 应答
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_tx_ack_sdu(struct xwssc * xwssc, xwu8_t port, xwu8_t id, xwu8_t ack)
{
        xwu8_t stream[sizeof(xwssc_ackfrm_sdu)];
        struct xwssc_frm * frm;
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t sdusize;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        xwssclogf(xwssc, DEBUG, "[R][TX][SDUACK] ACK:0x%X, ID:0x%X\n", ack, id);
        frm = (struct xwssc_frm *)stream;
        // cppcheck-suppress [misra-c2012-17.7]
        memcpy(stream, xwssc_ackfrm_sdu, sizeof(xwssc_ackfrm_sdu));
        frm->head.port = port;
        frm->head.id = id;
        frm->head.chksum = 0; /* 计算前先填0 */
        calsz = XWSSC_FRMHEAD_SIZE(frm->head.headsize);
        frm->head.chksum = xwssc_cal_head_chksum((xwu8_t *)&frm->head, calsz);
        sdupos = XWSSC_SDUPOS(&frm->head);
        sdupos[0] = ack;

        sdusize = 1;
        crc32pos = &sdupos[sdusize];
        calsz = sdusize;
        crc32 = xwlib_crc32_calms(sdupos, &calsz);
        crc32pos[0] = (xwu8_t)((crc32 >> (xwu32_t)24) & (xwu32_t)0xFF);
        crc32pos[1] = (xwu8_t)((crc32 >> (xwu32_t)16) & (xwu32_t)0xFF);
        crc32pos[2] = (xwu8_t)((crc32 >> (xwu32_t)8) & (xwu32_t)0xFF);
        crc32pos[3] = (xwu8_t)((crc32 >> (xwu32_t)0) & (xwu32_t)0xFF);
        rc = xwos_mtx_lock(&xwssc->txq.csmtx);
        if (XWOK == rc) {
                rc = xwssc_hwifal_tx(xwssc, stream, sizeof(xwssc_ackfrm_sdu));
                xwos_mtx_unlock(&xwssc->txq.csmtx);
        }
        return rc;
}

/**
 * @brief 格式化数据帧并加入到发送队列
 * @param[in] xwssc: XWSSC对象的指针
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
 *   + (xwu8_t)XWSSC_SOF,
 *   + (xwu8_t)XWSSC_SOF,
 *   + (xwu8_t)帧头长度 | 镜像反转
 *   + (xwu8_t)帧头校验和
 *   + (xwu8_t)端口
 *   + (xwu8_t)ID
 *   + (xwu8_t)QoS
 *   + (xwu8_t)ecsdusz[0:n] 编码的数据长度
 *   + (xwu8_t)sdu[0:sdusize - 1]: 数据
 *   + (xwu8_t)sdu[sdusize + 0]/CRC32 最高有效字节
 *   + (xwu8_t)sdu[sdusize + 1]/CRC32 第二字节
 *   + (xwu8_t)sdu[sdusize + 2]/CRC32 第三字节
 *   + (xwu8_t)sdu[sdusize + 3]/CRC32 最低有效字节
 *   + (xwu8_t)XWSSC_EOF,
 *   + (xwu8_t)XWSSC_EOF,
 */
__xwmd_code
xwer_t xwssc_eq_msg(struct xwssc * xwssc,
                    const xwu8_t sdu[], xwsz_t sdusize,
                    xwu8_t pri, xwu8_t port, xwu8_t qos,
                    xwssc_ntf_f ntfcb, void * cbarg,
                    xwssc_txh_t * txhbuf)
{
        xwu8_t ecsdusz[sizeof(xwsz_t) << 1];
        xwu8_t ecsize;
        xwu8_t frmheadsz;
        xwu8_t frmheadszmir;
        struct xwssc_carrier * car;
        union xwssc_slot * slot;
        xwsz_t need;
        xwsz_t neednum;
        xwssq_t odr;
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        /* 申请发送装置 */
        car = xwssc_txq_carrier_alloc(xwssc);
        if (NULL == car) {
                rc = -ENOMEM;
                goto err_car_alloc;
        }
        car->pri = pri;

        /* 申请帧槽 */
        xwssc_encode_sdusize(sdusize, ecsdusz, &ecsize);
        need = sizeof(union xwssc_slot) + ecsize + sdusize +
               XWSSC_CRC32_SIZE + XWSSC_EOF_SIZE;
        neednum = XWBOP_DIV_ROUND_UP(need, XWSSC_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWSSC_MEMBLK_SIZE << (xwsz_t)odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwssc->mempool, (xwsq_t)odr, (void **)&slot);
        if (rc < 0) {
                goto err_bma_alloc;
        }
        car->slot = slot;

        /* 设置发送信息 */
        slot->tx.ntfcb = ntfcb;
        slot->tx.cbarg = cbarg;
        slot->tx.frmsize = sizeof(struct xwssc_frm) + ecsize + sdusize +
                           XWSSC_CRC32_SIZE + XWSSC_EOF_SIZE;

        xwssclogf(xwssc, DEBUG,
                  "[A][EQ] car(0x%lX), slot(0x%lX), "
                  "pri:0x%X, port:0x%X, sdusize:0x%X\n",
                  (xwptr_t)car, (xwptr_t)slot, pri, port, sdusize);
        /* SOF */
        // cppcheck-suppress [misra-c2012-17.7]
        memset(slot->tx.frm.sof, XWSSC_SOF, XWSSC_SOF_SIZE);
        /* Head */
        frmheadsz = sizeof(struct xwssc_frmhead) + ecsize;
        frmheadszmir = xwbop_rbit(xwu8_t, frmheadsz);
        slot->tx.frm.head.headsize = frmheadsz | frmheadszmir;
        slot->tx.frm.head.port = port;
        slot->tx.frm.head.qos = qos & (xwu8_t)XWSSC_MSG_QOS_MSK;
        // cppcheck-suppress [misra-c2012-17.7]
        memcpy(slot->tx.frm.head.ecsdusz, ecsdusz, ecsize);
        /* slot->tx.frm.head.id 与 slot->tx.frm.head.chksum 待发送时才能确定 */

        /* SDU */
        sdupos = XWSSC_SDUPOS(&slot->tx.frm.head);
        memcpy(sdupos, sdu, sdusize); // cppcheck-suppress [misra-c2012-17.7]

        /* CRC32 */
        crc32pos = &sdupos[sdusize];
        if ((xwu8_t)0 != (qos & (xwu8_t)XWSSC_MSG_QOS_CHKSUM_MSK)) {
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
        // cppcheck-suppress [misra-c2012-17.7]
        memset(&crc32pos[XWSSC_CRC32_SIZE], XWSSC_EOF, XWSSC_EOF_SIZE);

        /* 加入到发送队列 */
        xwssc_txq_add_tail(xwssc, car);
        xwos_sem_post(&xwssc->txq.qsem);

        if (NULL != txhbuf) {
                *txhbuf = (xwssc_txh_t)car;
        }
        return XWOK;

err_bma_alloc:
        xwssc_txq_carrier_free(xwssc, car);
err_car_alloc:
        return rc;
}

/**
 * @brief 连接远端
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_connect(struct xwssc * xwssc)
{
        xwsq_t hwifst;
        xwsz_t cnt;
        xwer_t rc;

        cnt = 0;
        rc = XWOK;
        do { // cppcheck-suppress [misra-c2012-15.4]
                xwaop_read(xwsq_t, &xwssc->hwifst, &hwifst);
                if ((xwsq_t)0 != ((xwsq_t)XWSSC_HWIFST_CONNECT & hwifst)) {
                        break;
                }
                rc = xwssc_tx_connection(xwssc);
                if (XWOK == rc) {
                        rc = xwssc_doze(1); // cppcheck-suppress [misra-c2012-17.7]
                        if (rc < 0) {
                                break;
                        }
                        cnt++;
                } else {
                        break;
                }
        } while (cnt < XWSSC_RETRY_NUM);
        if ((XWOK == rc) && (XWSSC_RETRY_NUM == cnt)) {
                xwssc_hwifal_notify(xwssc, XWSSC_HWIFNTF_NETUNREACH);
                xwssc_doze(3); // cppcheck-suppress [misra-c2012-17.7]
        }
        return rc;
}

/**
 * @brief 指示数据帧的发送状态
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] car: 发送装置的指针
 */
static __xwmd_code
void xwssc_finish_tx(struct xwssc * xwssc, struct xwssc_carrier * car)
{
        xwu8_t ack;
        xwu8_t rmtid;

        ack = xwssc->txq.remote.ack;
        rmtid = xwssc->txq.remote.id;
        xwssclogf(xwssc, DEBUG, "[T][>] Remote ACK:0x%X, Remote ID:0x%X\n", ack, rmtid);
        if (rmtid != car->slot->tx.frm.head.id) {
                xwaop_c0m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_CONNECT, NULL, NULL);
                xwaop_write(xwu32_t, &car->state, XWSSC_CRS_READY, NULL);
                xwssc->txq.tmp = car;
                xwssclogf(xwssc, DEBUG, "[T][>] Remote ACK ID error!\n");
        } else {
                switch (ack) {
                case XWSSC_ACK_OK:
                        xwaop_add(xwu32_t, &xwssc->txq.cnt, 1, NULL, NULL);
                        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_FINISH, NULL);
                        xwos_splk_lock(&xwssc->txq.notiflk);
                        if (NULL != car->slot->tx.ntfcb) {
                                car->slot->tx.ntfcb(xwssc, (xwssc_txh_t)car,
                                                    xwssc_callback_rc[XWSSC_ACK_OK],
                                                    car->slot->tx.cbarg);
                        }
                        xwos_splk_unlock(&xwssc->txq.notiflk);
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmm_bma_free(xwssc->mempool, car->slot);
                        car->slot = NULL;
                        xwssc_txq_carrier_free(xwssc, car);
                        break;
                case XWSSC_ACK_EALREADY:
                        xwssclogf(xwssc, DEBUG, "[T][>] Msg is already transmitted!\n");
                        xwaop_add(xwu32_t, &xwssc->txq.cnt, 1, NULL, NULL);
                        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_FINISH, NULL);
                        xwos_splk_lock(&xwssc->txq.notiflk);
                        if (NULL != car->slot->tx.ntfcb) {
                                car->slot->tx.ntfcb(xwssc, (xwssc_txh_t)car,
                                                    xwssc_callback_rc[XWSSC_ACK_OK],
                                                    car->slot->tx.cbarg);
                        }
                        xwos_splk_unlock(&xwssc->txq.notiflk);
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmm_bma_free(xwssc->mempool, car->slot);
                        car->slot = NULL;
                        xwssc_txq_carrier_free(xwssc, car);
                        break;
                case XWSSC_ACK_ECONNRESET:
                        xwssclogf(xwssc, WARNING, "[T][>] Link has been severed!\n");
                        xwaop_c0m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_CONNECT,
                                  NULL, NULL);
                        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_READY, NULL);
                        xwssc->txq.tmp = car;
                        break;
                case XWSSC_ACK_NOMEM:
                        xwssclogf(xwssc, WARNING, "[T][>] Remote has no memory!\n");
                        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_READY, NULL);
                        xwssc->txq.tmp = car;
                        xwssc_doze(1); // cppcheck-suppress [misra-c2012-17.7]
                        break;
                default:
                        XWSSC_BUG();
                        xwaop_write(xwu32_t, &car->state, XWSSC_CRS_READY, NULL);
                        xwssc->txq.tmp = car;
                        break;
                }
        }
}

/**
 * @brief 发送帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] car: 发送装置的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_tx_frm(struct xwssc * xwssc, struct xwssc_carrier * car)
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
        xwaop_read(xwu32_t, &xwssc->txq.cnt, &txcnt);
        id = XWSSC_ID(txcnt);
        car->slot->tx.frm.head.id = id;
        car->slot->tx.frm.head.chksum = 0; /* 计算前先填0 */
        calsz = XWSSC_FRMHEAD_SIZE(car->slot->tx.frm.head.headsize);
        car->slot->tx.frm.head.chksum =
                        xwssc_cal_head_chksum((xwu8_t *)&car->slot->tx.frm.head,
                                              calsz);

        /* 发送 */
        cnt = 0;
        ulk.osal.mtx = &xwssc->txq.csmtx;
        rc = xwos_mtx_lock(&xwssc->txq.csmtx);
        if (rc < 0) {
                goto err_mtx_lock;
        }
        if ((xwu8_t)0 !=
            (car->slot->tx.frm.head.qos & (xwu8_t)XWSSC_MSG_QOS_RELIABLE_MSK)) {
                xwaop_s1m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_TX, NULL, NULL);
                do { // cppcheck-suppress [misra-c2012-15.4]
                        xwssclogf(xwssc, DEBUG,
                                  "[T][.] carrier(0x%lX), ID:0x%X, cnt:0x%X\n",
                                  (xwptr_t)car, id, cnt);
                        rc = xwssc_hwifal_tx(xwssc,
                                             (const xwu8_t *)&car->slot->tx.frm,
                                             car->slot->tx.frmsize);
                        if (rc < 0) {
                                xwaop_c0m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_TX,
                                          NULL, NULL);
                                xwos_mtx_unlock(&xwssc->txq.csmtx);
                                goto err_if_tx;
                        }
                        rc = xwos_cond_wait_to(&xwssc->txq.cscond,
                                               ulk, XWOS_LK_MTX, NULL,
                                               xwtm_ft(XWSSC_RETRY_PERIOD), &lkst);
                        if (XWOK == rc) {
                                xwos_mtx_unlock(&xwssc->txq.csmtx);
                                xwssc_finish_tx(xwssc, car);
                                break;
                        } else if (-ETIMEDOUT == rc) {
                                rc = xwos_mtx_lock(&xwssc->txq.csmtx);
                                if (rc < 0) {
                                        xwaop_c0m(xwsq_t, &xwssc->hwifst,
                                                  XWSSC_HWIFST_TX, NULL, NULL);
                                        goto err_mtx_lock;
                                }
                                xwaop_read(xwsq_t, &xwssc->hwifst, &hwifst);
                                if ((xwsq_t)0 != ((xwsq_t)XWSSC_HWIFST_TX & hwifst)) {
                                        cnt++;
                                        rc = -ETIMEDOUT;
                                } else {
                                        xwos_mtx_unlock(&xwssc->txq.csmtx);
                                        xwssc_finish_tx(xwssc, car);
                                        rc = XWOK;
                                        break;
                                }
                        } else {
                                xwaop_c0m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_TX,
                                          NULL, NULL);
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwos_mtx_unlock(&xwssc->txq.csmtx);
                                }
                                break;
                        }
                } while (cnt < XWSSC_RETRY_NUM);
                if ((XWSSC_RETRY_NUM == cnt) && (-ETIMEDOUT == rc)) {
                        xwaop_c0m(xwsq_t, &xwssc->hwifst,
                                  (XWSSC_HWIFST_TX | XWSSC_HWIFST_CONNECT),
                                  NULL, NULL);
                        xwos_mtx_unlock(&xwssc->txq.csmtx);
                        xwssc_doze(3); // cppcheck-suppress [misra-c2012-17.7]
                }
        } else {
                rc = xwssc_hwifal_tx(xwssc,
                                     (const xwu8_t *)&car->slot->tx.frm,
                                     car->slot->tx.frmsize);
                if (rc < 0) {
                        xwos_mtx_unlock(&xwssc->txq.csmtx);
                        goto err_if_tx;
                }
                xwos_mtx_unlock(&xwssc->txq.csmtx);
                xwos_splk_lock(&xwssc->txq.notiflk);
                if (NULL != car->slot->tx.ntfcb) {
                        car->slot->tx.ntfcb(xwssc, (xwssc_txh_t)car,
                                            xwssc_callback_rc[XWSSC_ACK_OK],
                                            car->slot->tx.cbarg);
                }
                xwos_splk_unlock(&xwssc->txq.notiflk);
                // cppcheck-suppress [misra-c2012-17.7]
                xwmm_bma_free(xwssc->mempool, car->slot);
                car->slot = NULL;
                xwssc_txq_carrier_free(xwssc, car);
        }

err_if_tx:
err_mtx_lock:
        return rc;
}

/**
 * @brief XWSSC的发送线程状态机
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_txfsm(struct xwssc * xwssc)
{
        struct xwssc_carrier * car;
        xwu32_t state;
        xwsq_t hwifst;
        xwer_t rc;

        xwaop_read(xwsq_t, &xwssc->hwifst, &hwifst);
        if ((xwsq_t)0 != ((xwsq_t)XWSSC_HWIFST_CONNECT & hwifst)) {
                /* 选择一个待发送的帧 */
                if (NULL != xwssc->txq.tmp) {
                        car = xwssc->txq.tmp;
                        xwssc->txq.tmp = NULL;
                } else {
                        rc = xwos_sem_wait(&xwssc->txq.qsem);
                        if (rc < 0) {
                                goto err_txqsem_wait;
                        }
                        car = xwssc_txq_choose(xwssc);
                        XWSSC_BUG_ON(is_err(car));
                }
                xwssclogf(xwssc, DEBUG,
                          "[T][<] Choose carrier(0x%lX), port:0x%X, state:%d\n",
                          (xwptr_t)car,
                          car->slot->tx.frm.head.port,
                          car->state);
                rc = xwaop_teq_then_write(xwu32_t, &car->state,
                                          XWSSC_CRS_READY,
                                          XWSSC_CRS_INPROGRESS,
                                          &state);
                if (XWOK == rc) {
                        rc = xwssc_tx_frm(xwssc, car);
                        if (rc < 0) {
                                goto err_tx_frm;
                        }
                } else {
                        if ((xwu32_t)XWSSC_CRS_ABORT == state) {
                                xwos_splk_lock(&xwssc->txq.notiflk);
                                if (NULL != car->slot->tx.ntfcb) {
                                        car->slot->tx.ntfcb(xwssc,
                                                            (xwssc_txh_t)car,
                                                            -ECONNABORTED,
                                                            car->slot->tx.cbarg);
                                }
                                xwos_splk_unlock(&xwssc->txq.notiflk);
                        } else {
                                xwos_splk_lock(&xwssc->txq.notiflk);
                                if (NULL != car->slot->tx.ntfcb) {
                                        car->slot->tx.ntfcb(xwssc,
                                                            (xwssc_txh_t)car,
                                                            -EPROTO,
                                                            car->slot->tx.cbarg);
                                }
                                xwos_splk_unlock(&xwssc->txq.notiflk);
                        }
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmm_bma_free(xwssc->mempool, car->slot);
                        car->slot = NULL;
                        xwssc_txq_carrier_free(xwssc, car);
                }
        } else {
                /* 连接 */
                rc = xwssc_connect(xwssc);
                if (rc < 0) {
                        goto err_connect;
                }
        }
        return XWOK;

err_tx_frm:
        xwssc->txq.tmp = car;
err_txqsem_wait:
err_connect:
        return rc;
}

/**
 * @brief XWSSC的发送线程
 * @param[in] xwssc: XWSSC对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwssc_txthd(struct xwssc * xwssc)
{
        xwer_t rc;

        rc = XWOK;
        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwssclogf(xwssc, DEBUG, "[T] Start freezing ...\n");
                        rc = xwos_cthd_freeze();
                        if (rc < 0) {
                                xwssclogf(xwssc, DEBUG,
                                          "[T] Failed to freeze ... <rc:%d>\n",
                                          rc);
                        }
                        xwssclogf(xwssc, DEBUG, "[T] Resuming ...\n");
                }
                rc = xwssc_txfsm(xwssc);
                switch (rc) {
                case XWOK:
                        break;
                case -EINTR:
                case -ERESTARTSYS:
                        xwssclogf(xwssc, DEBUG, "[T] Interrupted!\n");
                        break;
                case -EOVERFLOW:
                        xwssclogf(xwssc, WARNING, "[T] Buffer of HWIF is overflow!\n");
                        xwssc_doze(1); // cppcheck-suppress [misra-c2012-17.7]
                        break;
                case -ETIMEDOUT:
                        xwssclogf(xwssc, WARNING, "[T] Timeout!\n");
                        break;
                default:
                        xwssclogf(xwssc, CRIT,
                                  "[T] BUG!!! xwssc_txfsm() returns %d!\n", rc);
                        XWSSC_BUG();
                        break;
                }
        }
        if (NULL != xwssc->txq.tmp) {
                xwssc_txq_add_head(xwssc, xwssc->txq.tmp);
                xwssc->txq.tmp = NULL;
                xwos_sem_post(&xwssc->txq.qsem);
        }
        return rc;
}

/**
 * @brief 短暂暂停一下XWSSC的线程
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_doze(xws32_t cnt)
{
        return xwos_cthd_sleep((xws32_t)XWSSC_RETRY_PERIOD * cnt);
}

/**
 * @} xwmd_isc_xwssc_protocol
 */
