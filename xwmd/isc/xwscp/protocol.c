/**
 * @file
 * @brief 精简的点对点通讯协议：协议层
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/crc32.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/mm/bma.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/mif.h>

/**
 * @brief 连接命令的模板
 */
__xwmd_rodata const xwu8_t xwscp_cmd_connect[] = {
        (xwu8_t)XWSCP_SOF,
        (xwu8_t)XWSCP_SOF,
        (xwu8_t)0x66, /* 帧头长度 | mirror */
        (xwu8_t)0x8E, /* 帧头校验和 */
        (xwu8_t)XWSCP_PORT_CMD, /* 端口:0 */
        (xwu8_t)XWSCP_PORT0_CMD_CONNECT, /* 命令ID */
        (xwu8_t)XWSCP_MSG_QOS_3, /* Qos: 可靠消息，有CRC校验码 */
        (xwu8_t)0x8, /* SDU长度 */
        (xwu8_t)'X', /* sdu[0]: protocol 0 */
        (xwu8_t)'W', /* sdu[1]: protocol 1 */
        (xwu8_t)'S', /* sdu[2]: protocol 2 */
        (xwu8_t)'C', /* sdu[3]: protocol 3 */
        (xwu8_t)'P', /* sdu[4]: protocol 4 */
        (xwu8_t)XWSCP_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWSCP_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWSCP_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)0x59, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0x91, /* CRC32 第二字节 */
        (xwu8_t)0x09, /* CRC32 第三字节 */
        (xwu8_t)0x18, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWSCP_EOF,
        (xwu8_t)XWSCP_EOF,
};

/**
 * @brief 连接命令应答帧
 */
__xwmd_rodata const xwu8_t xwscp_cmd_connect_ack[] = {
        (xwu8_t)XWSCP_SOF,
        (xwu8_t)XWSCP_SOF,
        (xwu8_t)0x66, /* 帧头长度 | mirror */
        (xwu8_t)0x0E, /* 帧头校验和 */
        (xwu8_t)XWSCP_PORT_CMD, /* 端口:0 */
        (xwu8_t)(XWSCP_PORT0_CMD_CONNECT | XWSCP_ID_ACK), /* 命令ID | 应答 */
        (xwu8_t)XWSCP_MSG_QOS_3, /* Qos: 可靠消息，有CRC校验码 */
        (xwu8_t)0x8, /* SDU长度 */
        (xwu8_t)'X', /* sdu[0]: protocol 0 */
        (xwu8_t)'W', /* sdu[1]: protocol 1 */
        (xwu8_t)'S', /* sdu[2]: protocol 2 */
        (xwu8_t)'C', /* sdu[3]: protocol 3 */
        (xwu8_t)'P', /* sdu[4]: protocol 4 */
        (xwu8_t)XWSCP_VERSION_MAJOR, /* sdu[5]: major version */
        (xwu8_t)XWSCP_VERSION_MINOR, /* sdu[6]: minor version */
        (xwu8_t)XWSCP_VERSION_REVISION, /* sdu[7]: revision */
        (xwu8_t)0x59, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0x91, /* CRC32 第二字节 */
        (xwu8_t)0x09, /* CRC32 第三字节 */
        (xwu8_t)0x18, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWSCP_EOF,
        (xwu8_t)XWSCP_EOF,
};

/**
 * @brief 数据应答帧模板
 */
__xwmd_rodata const xwu8_t xwscp_sdu_ack[] = {
        (xwu8_t)XWSCP_SOF,
        (xwu8_t)XWSCP_SOF,
        (xwu8_t)0x66, /* 帧头长度 | mirror */
        /* 以下数据需依据实际情况填充 */
        (xwu8_t)0x0, /* 帧头校验和 */
        (xwu8_t)XWSCP_PORT_DATA, /* 数据端口 */
        (xwu8_t)XWSCP_ID_ACK, /* ID | XWSCP_ID_ACK */
        (xwu8_t)XWSCP_MSG_QOS_3, /* Qos: 可靠消息，有CRC校验码 */
        (xwu8_t)0x1, /* SDU长度 */
        (xwu8_t)0, /* sdu[0]: 应答 */
        (xwu8_t)0, /* CRC32 第一字节（最高有效字节） */
        (xwu8_t)0, /* CRC32 第二字节 */
        (xwu8_t)0, /* CRC32 第三字节 */
        (xwu8_t)0, /* CRC32 第四字节（第低有效字节） */
        (xwu8_t)XWSCP_EOF,
        (xwu8_t)XWSCP_EOF,
};

__xwmd_rodata const xwer_t xwscp_callback_rc[XWSCP_ACK_NUM] = {
        [XWSCP_ACK_OK] = XWOK,
        [XWSCP_ACK_EALREADY] = -EALREADY,
        [XWSCP_ACK_ECONNRESET] = -ECONNRESET,
        [XWSCP_ACK_NOMEM] = -ENOMEM,
};

/**
 * @brief 数据长度编解码移位表
 */
__xwmd_rodata const xwu8_t xwscp_ecsdusz_shift_table[] = {
        0, 7, 14, 21, 28, 35, 42, 49,
        56, 63, 70, 77, 84, 91, 98, 105,
        112, 119, 126, 133,
};

static __xwmd_code
xwer_t xwscp_chk_frm(union xwscp_slot * slot);

static __xwmd_code
xwer_t xwscp_rx_cmd_connect(struct xwscp * xwscp, union xwscp_slot * slot);

static __xwmd_code
xwer_t xwscp_rx_cmd_connect_ack(struct xwscp * xwscp, union xwscp_slot * slot);

static __xwmd_code
xwer_t xwscp_rx_sdu(struct xwscp * xwscp, union xwscp_slot * slot);

static __xwmd_code
xwer_t xwscp_rx_sdu_ack(struct xwscp * xwscp, union xwscp_slot * slot);

static __xwmd_code
xwer_t xwscp_rx_frm(struct xwscp * xwscp, union xwscp_slot * slot);

static __xwmd_code
xwer_t xwscp_fsm(struct xwscp * xwscp);

static __xwmd_code
xwer_t xwscp_tx_cmd_connect_ack(struct xwscp * xwscp);

/**
 * @brief 增加XWSCP的引用计数，防止其被停止
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWSCP已经停止运行
 */
__xwmd_code
xwer_t xwscp_grab(struct xwscp * xwscp)
{
        return xwos_object_grab(&xwscp->xwobj);
}

/**
 * @brief 减少XWSCP的引用计数
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: XWSCP没有在运行
 */
__xwmd_code
xwer_t xwscp_put(struct xwscp * xwscp)
{
        return xwos_object_put(&xwscp->xwobj);
}

/**
 * @brief 编码数据长度
 * @param sdusize: (I) 数据长度
 * @param ecsdusz: (O) 指向缓冲区的指针，此缓冲区用于输出编码后的数据数组
 * @param ecsize: (O) 指向缓冲区的指针，此缓冲区用于输出编码后的数据数组的长度
 */
__xwmd_code
void xwscp_encode_sdusize(xwsz_t sdusize, xwu8_t * ecsdusz, xwu8_t * ecsize)
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
 * @param ecsdusz: (I) 编码的数据长度数据
 * @param sdusize: (O) 指向缓冲区的指针，解码后通过此缓冲区返回数据长度
 */
__xwmd_code
void xwscp_decode_sdusize(xwu8_t * ecsdusz, xwsz_t * sdusize)
{
        xwsz_t dcsdusz;
        xwsz_t sft;
        xwsq_t i;

        i = 0;
        dcsdusz = 0;
        while (true) {
                sft = xwscp_ecsdusz_shift_table[i];
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
 * @brief 计算Head的校验和
 * @param head: (I) 数据流
 * @param size: (I) 数据流长度
 * @return 校验和
 */
__xwmd_code
xwu8_t xwscp_cal_head_chksum(xwu8_t head[], xwsz_t size)
{
        xwu16_t chksum;
        xwu8_t i;

        chksum = 0;
        for (i = 0; i < size; i++) {
                chksum += (xwu16_t)head[i];
        }
        while (chksum & 0xFF00U) {
                chksum = (xwu16_t)(chksum >> 16U) + (xwu16_t)(chksum & 0xFFU);
        }
        chksum ^= 0xFF;
        return (xwu8_t)chksum;
}

/**
 * @brief 校验Head
 * @param head: (I) 数据流
 * @param size: (I) 数据流长度
 * @return 布尔值
 * @retval true: 校验通过
 * @retval false: 校验失败
 */
__xwmd_code
bool xwscp_chk_head(xwu8_t head[], xwsz_t size)
{
        xwu16_t chksum;
        xwu8_t i;

        chksum = 0;
        for (i = 0; i < size; i++) {
                chksum += (xwu16_t)head[i];
        }
        while (chksum & 0xFF00U) {
                chksum = (xwu16_t)(chksum >> 16U) + (xwu16_t)(chksum & 0xFFU);
        }
        return (chksum == 0xFF);
}

/**
 * @brief 校验帧是否合法
 * @param xwscp: (I) XWSCP对象的指针
 * @param slot: (I) 待校验的帧槽的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EBADMSG: 校验错误
 */
static __xwmd_code
xwer_t xwscp_chk_frm(union xwscp_slot * slot)
{
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t sdusize;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        sdupos = XWSCP_SDUPOS(&slot->rx.frm.head);
        sdusize = XWSCP_RXSDUSIZE(&slot->rx);
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
 * @param xwscp: (I) XWSCP对象的指针
 * @param slot: (I) 待发布的数据帧
 */
__xwmd_code
void xwscp_rxq_pub(struct xwscp * xwscp, union xwscp_slot * slot)
{
        xwos_splk_lock(&xwscp->rx.lock);
        xwlib_bclst_add_tail(&xwscp->rx.head, &slot->rx.node);
        xwos_splk_unlock(&xwscp->rx.lock);
        xwos_sem_post(&xwscp->rx.sem);
}

/**
 * @brief 从接收队列中选择一条数据帧
 * @param xwscp: (I) XWSCP对象的指针
 * @return 帧槽的指针
 */
__xwmd_code
union xwscp_slot * xwscp_rxq_choose(struct xwscp * xwscp)
{
        union xwscp_slot * slot;
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&xwscp->rx.lock, &cpuirq);
        slot = xwlib_bclst_first_entry(&xwscp->rx.head,
                                       union xwscp_slot,
                                       rx.node);
        xwlib_bclst_del_init(&slot->rx.node);
        xwos_splk_unlock_cpuirqrs(&xwscp->rx.lock, cpuirq);
        return slot;
}

/**
 * @brief 接收连接命令
 * @param xwscp: (I) XWSCP对象的指针
 * @param slot: (I) 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_rx_cmd_connect(struct xwscp * xwscp, union xwscp_slot * slot)
{
        xwu8_t * sdupos;
        xwer_t rc;
        char proto[6];

        sdupos = XWSCP_SDUPOS(&slot->rx.frm.head);
        proto[0] = sdupos[0];
        proto[1] = sdupos[1];
        proto[2] = sdupos[2];
        proto[3] = sdupos[3];
        proto[4] = sdupos[4];
        proto[5] = '\0';
        xwscplogf(DEBUG,
                  "[rxthd] proto:%s-%d.%d.%d\n",
                  proto, sdupos[5], sdupos[6], sdupos[7]);
        if ((0 == strcmp(proto, "XWSCP")) &&
            (XWSCP_VERSION_MAJOR == sdupos[5]) &&
            (XWSCP_VERSION_MINOR == sdupos[6]) &&
            (XWSCP_VERSION_REVISION == sdupos[7])) {
                rc = xwscp_tx_cmd_connect_ack(xwscp);
                if (XWOK == rc) {
                        xwaop_write(xwu32, &xwscp->rx.cnt, 0, NULL);
                }/* else {} */
        } else {
                rc = -EPERM;
        }
        xwmm_bma_free(xwscp->mempool, slot);
        return rc;
}

/**
 * @brief 接收连接命令应答
 * @param xwscp: (I) XWSCP对象的指针
 * @param slot: (I) 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_rx_cmd_connect_ack(struct xwscp * xwscp, union xwscp_slot * slot)
{
        xwu8_t * sdupos;
        xwsq_t hwifst;
        char proto[6];
        xwer_t rc;

        sdupos = XWSCP_SDUPOS(&slot->rx.frm.head);
        proto[0] = sdupos[0];
        proto[1] = sdupos[1];
        proto[2] = sdupos[2];
        proto[3] = sdupos[3];
        proto[4] = sdupos[4];
        proto[5] = '\0';

        xwscplogf(DEBUG,
                  "[rxthd] proto:%s-%d.%d.%d, remote:%d, local:%d\n",
                  proto, sdupos[5], sdupos[6], sdupos[7]);
        rc = xwos_mtx_lock(&xwscp->tx.csmtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        xwaop_read(xwsq, &xwscp->hwifst, &hwifst);
        if (!(XWSCP_HWIFST_CONNECT & hwifst)) {
                if ((0 == strcmp(proto, "XWSCP")) &&
                    (XWSCP_VERSION_MAJOR == sdupos[5]) &&
                    (XWSCP_VERSION_MINOR == sdupos[6]) &&
                    (XWSCP_VERSION_REVISION == sdupos[7])) {
                        xwscp->tx.remote.ack = XWSCP_ACK_OK;
                        xwscp->tx.remote.id = XWSCP_PORT0_CMD_CONNECT;
                        xwos_mtx_unlock(&xwscp->tx.csmtx);
                        xwos_cond_broadcast(&xwscp->tx.cscond);
                } else {
                        xwos_mtx_unlock(&xwscp->tx.csmtx);
                }
        } else {
                xwos_mtx_unlock(&xwscp->tx.csmtx);
        }

err_mtx_lock:
        xwmm_bma_free(xwscp->mempool, slot);
        return rc;
}

/**
 * @brief 接收数据帧
 * @param xwscp: (I) XWSCP对象的指针
 * @param slot: (I) 接收到的帧槽指针
 * @return 错误码
 * @note
 * + 数据帧：
 *   - (xwu8_t)XWSCP_SOF,
 *   - (xwu8_t)XWSCP_SOF,
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
 *   - (xwu8_t)XWSCP_EOF,
 *   - (xwu8_t)XWSCP_EOF,
 */
static __xwmd_code
xwer_t xwscp_rx_sdu(struct xwscp * xwscp, union xwscp_slot * slot)
{
        xwu32_t rxcnt;
        xwu8_t rmtid, lclid;
        xwer_t rc;

        xwaop_read(xwu32, &xwscp->rx.cnt, &rxcnt);
        lclid = XWSCP_ID(rxcnt);
        rmtid = slot->rx.frm.head.id;
        xwscplogf(DEBUG,
                  "qos:0x%X, frmsize:0x%X, "
                  "Remote ID:0x%X, Local ID:0x%X\n",
                  slot->frm.head.qos, slot->frmsize, rmtid, lclid);

        if (slot->rx.frm.head.qos & XWSCP_MSG_QOS_ACK_MSK) {
                if (__xwcc_likely(rmtid == lclid)) {
                        /* 收到数据 */
                        rc = xwscp_tx_sdu_ack(xwscp, rmtid, XWSCP_ACK_OK);
                        if (XWOK == rc) {
                                xwaop_add(xwu32, &xwscp->rx.cnt, 1, NULL, NULL);
                                xwscp_rxq_pub(xwscp, slot);
                        } else {
                                xwmm_bma_free(xwscp->mempool, slot);
                        }
                } else if (XWSCP_ID(rmtid + 1) == lclid) {
                        /* 收到重复的数据 */
                        rc = xwscp_tx_sdu_ack(xwscp, rmtid, XWSCP_ACK_EALREADY);
                        xwmm_bma_free(xwscp->mempool, slot);
                } else {
                        /* 连接被复位 */
                        rc = xwscp_tx_sdu_ack(xwscp, rmtid, XWSCP_ACK_ECONNRESET);
                        xwmm_bma_free(xwscp->mempool, slot);
                }
        } else {
                xwscp_rxq_pub(xwscp, slot);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 接收数据应答帧
 * @param xwscp: (I) XWSCP对象的指针
 * @param slot: (I) 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_rx_sdu_ack(struct xwscp * xwscp, union xwscp_slot * slot)
{
        xwu8_t * sdupos;
        xwu8_t rmtid;
        xwu8_t ack;
        xwer_t rc;
        xwsq_t hwifst;

        sdupos = XWSCP_SDUPOS(&slot->rx.frm.head);
        rmtid = XWSCP_ID(slot->rx.frm.head.id);
        ack = sdupos[0];
        xwscplogf(DEBUG, "[rxthd] ID:0x%X, ACK:0x%X\n", rmtid, ack);
        rc = xwos_mtx_lock(&xwscp->tx.csmtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_lock;
        }
        xwaop_read(xwsq, &xwscp->hwifst, &hwifst);
        if (XWSCP_HWIFST_TX & hwifst) {
                xwscp->tx.remote.ack = ack;
                xwscp->tx.remote.id = rmtid;
                xwaop_c0m(xwsq, &xwscp->hwifst, XWSCP_HWIFST_TX, NULL, NULL);
                xwos_mtx_unlock(&xwscp->tx.csmtx);
                xwos_cond_broadcast(&xwscp->tx.cscond);
        } else {
                xwos_mtx_unlock(&xwscp->tx.csmtx);
                xwscplogf(NOTICE, "[rxthd] no sending frame!\n");
        }

err_mtx_lock:
        xwmm_bma_free(xwscp->mempool, slot);
        return rc;
}

/**
 * @brief 接收并处理一帧消息
 * @param xwscp: (I) XWSCP对象的指针
 * @param slot: (I) 接收到的帧槽指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_rx_frm(struct xwscp * xwscp, union xwscp_slot * slot)
{
        xwer_t rc;
        xwu8_t port;
        xwu8_t id;
        xwu8_t qos;

        port = slot->rx.frm.head.port;
        id = slot->rx.frm.head.id;
        qos = slot->rx.frm.head.qos;
        xwscplogf(DEBUG, "[rxthd] RX Frame(port:0x%X, ID:0x%X, qos:0x%X)\n",
                  port, id, qos);
        if (XWSCP_MSG_QOS_CHKSUM_MSK & qos) {
                rc = xwscp_chk_frm(slot);
                if (rc < 0) {
                        xwmm_bma_free(xwscp->mempool, slot);
                        goto err_badmsg;
                }
        }
        if (XWSCP_PORT_CMD == port) {
                if (XWSCP_PORT0_CMD_CONNECT == id) {
                        rc = xwscp_rx_cmd_connect(xwscp, slot);
                } else {
                        rc = xwscp_rx_cmd_connect_ack(xwscp, slot);
                }
        } else {
                if (XWSCP_ID_ACK & id) {
                        rc = xwscp_rx_sdu_ack(xwscp, slot);
                } else {
                        rc = xwscp_rx_sdu(xwscp, slot);
                }
        }

err_badmsg:
        return rc;
}

/**
 * @brief XWSCP的接收线程状态机
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_fsm(struct xwscp * xwscp)
{
        union xwscp_slot * slot;
        xwer_t rc;

        do {
                rc = xwscp_hwifal_rx(xwscp, &slot);
        } while (-EAGAIN == rc);
        if (XWOK == rc) {
                rc = xwscp_rx_frm(xwscp, slot);
        }
        return rc;
}

/**
 * @brief XWSCP的接收线程
 * @param xwscp: (I) XWSCP对象的指针
 * @return 线程返回值
 */
__xwmd_code
xwer_t xwscp_thd(struct xwscp * xwscp)
{
        xwer_t rc = XWOK;

        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwscplogf(DEBUG, "Start freezing ...\n");
                        rc = xwos_cthd_freeze();
                        if (__xwcc_unlikely(rc < 0)) {
                                xwscplogf(ERR, "Failed to freeze ... [rc:%d]\n", rc);
                                xwos_cthd_yield();
                        }/* else {} */
                        xwscplogf(DEBUG, "Resuming ...\n");
                } else if (XWSCP_HWIFST_RX == xwscp->hwifst) {
                        rc = xwscp_fsm(xwscp);
                        if (XWOK == rc) {
                        } else if (-ETIMEDOUT == rc) {
                        } else if (-ENOMEM == rc) {
                        } else if ((-EINTR == rc) || (-ERESTARTSYS == rc)) {
                                xwscplogf(DEBUG, "Interrupted ... [rc:%d]\n", rc);
                        } else if (-EBADMSG == rc) {
                                xwscplogf(WARNING, "Bad frame! \n");
                        } else {
                                xwscplogf(ERR, "xwscp_fsm() ... [rc:%d]\n", rc);
                                break;
                        }
                } else {
                        break;
                }
        }
        return rc;
}

/**
 * @brief 发送同步帧
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_tx_cmd_connect(struct xwscp * xwscp)
{
        xwer_t rc;

        xwscplogf(DEBUG, "TX connect cmd.\n");
        rc = xwscp_hwifal_tx(xwscp,
                             (xwu8_t *)xwscp_cmd_connect,
                             sizeof(xwscp_cmd_connect));
        return rc;
}

/**
 * @brief 发送同步应答帧
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwscp_tx_cmd_connect_ack(struct xwscp * xwscp)
{
        xwer_t rc;

        xwscplogf(DEBUG, "TX connect-ack cmd.\n");
        rc = xwos_mtx_lock(&xwscp->tx.csmtx);
        if (XWOK == rc) {
                rc = xwscp_hwifal_tx(xwscp,
                                     (xwu8_t *)xwscp_cmd_connect_ack,
                                     sizeof(xwscp_cmd_connect_ack));
                xwos_mtx_unlock(&xwscp->tx.csmtx);
        }/* else {} */
        return rc;
}

/**
 * @brief 格式化数据帧并加入到发送队列
 * @param xwscp: (I) XWSCP对象的指针
 * @param sdu: (I) 数据
 * @param sdusize: (I) 数据长度
 * @param qos: (I) 服务质量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOMEM: 内存不足
 * @note
 * + 数据帧：
 *   - (xwu8_t)XWSCP_SOF,
 *   - (xwu8_t)XWSCP_SOF,
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
 *   - (xwu8_t)XWSCP_EOF,
 *   - (xwu8_t)XWSCP_EOF,
 */
__xwmd_code
xwer_t xwscp_fmt_msg(struct xwscp * xwscp,
                     const xwu8_t sdu[], xwsz_t sdusize, xwu8_t qos,
                     union xwscp_slot ** out)
{
        xwu8_t ecsdusz[sizeof(xwsz_t) << 1];
        xwu8_t ecsize;
        xwu8_t frmheadsz;
        xwu8_t frmheadszmir;
        xwu8_t id;
        xwu32_t txcnt;
        union xwscp_slot * slot;
        xwsz_t need, neednum;
        xwssq_t odr;
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        /* 申请帧槽 */
        xwscp_encode_sdusize(sdusize, ecsdusz, &ecsize);
        need = sizeof(union xwscp_slot) + ecsize;
        neednum = XWBOP_DIV_ROUND_UP(need, XWSCP_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWSCP_MEMBLK_SIZE << odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwscp->mempool, (xwsq_t)odr, (void **)&slot);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_bma_alloc;
        }

        /* 设置发送信息 */
        xwscplogf(DEBUG, "slot(%p), sdusize:0x%X\n", slot, sdusize);
        slot->tx.frmsize = sizeof(struct xwscp_frm) + ecsize + sdusize +
                           XWSCP_CRC32_SIZE + XWSCP_EOF_SIZE;
        /* SOF */
        memset(slot->tx.frm.sof, XWSCP_SOF, XWSCP_SOF_SIZE);
        /* Head */
        xwaop_read(xwu32, &xwscp->tx.cnt, &txcnt);
        id = XWSCP_ID(txcnt);
        frmheadsz = sizeof(struct xwscp_frmhead) + ecsize;
        frmheadszmir = xwbop_rbit(xwu8_t, frmheadsz);
        slot->tx.frm.head.headsize = frmheadsz | frmheadszmir;
        slot->tx.frm.head.port = XWSCP_PORT_DATA;
        slot->tx.frm.head.id = id;
        slot->tx.frm.head.chksum = 0;
        slot->tx.frm.head.qos = qos;
        memcpy(slot->tx.frm.head.ecsdusz, ecsdusz, ecsize);
        calsz = frmheadsz;
        slot->tx.frm.head.chksum = xwscp_cal_head_chksum((xwu8_t *)&slot->tx.frm.head,
                                                         calsz);

        /* SDU */
        sdupos = XWSCP_SDUPOS(&slot->tx.frm.head);
        memcpy(sdupos, sdu, sdusize);

        /* CRC32 */
        crc32pos = &sdupos[sdusize];
        if (qos & XWSCP_MSG_QOS_CHKSUM_MSK) {
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
        memset(&crc32pos[XWSCP_CRC32_SIZE], XWSCP_EOF, XWSCP_EOF_SIZE);

        *out = slot;
        return XWOK;

err_bma_alloc:
        return rc;
}

/**
 * @brief 发送数据应答帧
 * @param xwscp: (I) XWSCP对象的指针
 * @param id: (I) 应答远程端的id
 * @param ack: (I) 应答码
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_tx_sdu_ack(struct xwscp * xwscp, xwu8_t id, xwu8_t ack)
{
        xwu8_t stream[sizeof(xwscp_sdu_ack)];
        struct xwscp_frm * frm;
        xwu8_t * sdupos;
        xwu8_t * crc32pos;
        xwsz_t sdusize;
        xwsz_t calsz;
        xwu32_t crc32;
        xwer_t rc;

        xwscplogf(DEBUG, "ACK:0x%X, id:0x%X\n", ack, id);
        frm = (struct xwscp_frm *)stream;
        memcpy(stream, xwscp_sdu_ack, sizeof(xwscp_sdu_ack));
        frm->head.id = id | XWSCP_ID_ACK;
        sdupos = XWSCP_SDUPOS(&frm->head);
        sdupos[0] = ack;

        sdusize = 1;
        crc32pos = &sdupos[sdusize];
        calsz = sdusize;
        crc32 = xwlib_crc32_calms(sdupos, &calsz);
        crc32pos[0] = (xwu8_t)((crc32 >> 24U) & 0xFFU);
        crc32pos[1] = (xwu8_t)((crc32 >> 16U) & 0xFFU);
        crc32pos[2] = (xwu8_t)((crc32 >> 8U) & 0xFFU);
        crc32pos[3] = (xwu8_t)((crc32 >> 0U) & 0xFFU);
        rc = xwos_mtx_lock(&xwscp->tx.csmtx);
        if (XWOK == rc) {
                rc = xwscp_hwifal_tx(xwscp, stream, sizeof(xwscp_sdu_ack));
                xwos_mtx_unlock(&xwscp->tx.csmtx);
        }/* else {} */
        return rc;
}
