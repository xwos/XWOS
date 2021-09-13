/**
 * @file
 * @brief 点对点通讯协议：模块接口
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwpcp_mif_h__
#define __xwmd_isc_xwpcp_mif_h__

#include <xwos/standard.h>

#if (XWMDCFG_isc_xwpcp_MEMBLK_SIZE & (XWMDCFG_isc_xwpcp_MEMBLK_SIZE - 1))
#  error "XWMDCFG_isc_xwpcp_MEMBLK_SIZE must be the power of 2!"
#endif
#if ((1 << XWMDCFG_isc_xwpcp_MEMBLK_ODR) > 32768U)
#  error "power(2, XWMDCFG_isc_xwpcp_MEMBLK_ODR) must be <= 32768!"
#endif
#if (XWMDCFG_isc_xwpcp_PORT_NUM > 256)
#  error "XWPCP Only supports 256 ports (0 ~ 255)!"
#endif
#if (XWMDCFG_isc_xwpcp_PRI_NUM > 64)
#  error "XWPCP Only supports 64 priorities!"
#endif

#define XWPCP_VERSION           ("3.0.0")
#define XWPCP_VERSION_MAJOR     3U
#define XWPCP_VERSION_MINOR     0U
#define XWPCP_VERSION_REVISION  0U

#define XWPCP_PRI_NUM           (XWMDCFG_isc_xwpcp_PRI_NUM)
#define XWPCP_INVALID_PRI       (0xFFU)
#define XWPCP_MAX_PRI           (XWPCP_PRI_NUM - 1)
#define XWPCP_PORT_NUM          (XWMDCFG_isc_xwpcp_PORT_NUM)
#define XWPCP_RETRY_PERIOD      (XWMDCFG_isc_xwpcp_RETRY_PERIOD)
#define XWPCP_RETRY_NUM         (XWMDCFG_isc_xwpcp_RETRY_NUM)
#define XWPCP_MEMBLK_SIZE       (XWMDCFG_isc_xwpcp_MEMBLK_SIZE)
#define XWPCP_MEMBLK_ODR        (XWMDCFG_isc_xwpcp_MEMBLK_ODR)
#define XWPCP_MEMBLK_NUM        (1 << XWPCP_MEMBLK_ODR)
#define XWPCP_MEMPOOL_SIZE      (XWPCP_MEMBLK_SIZE * XWPCP_MEMBLK_NUM)
#define XWPCP_DEF_MEMPOOL(name) __xwcc_alignl1cache xwu8_t name[XWPCP_MEMPOOL_SIZE]

struct xwpcp;
struct xwpcp_hwifal_operation;
struct xwpcp_carrier;

/**
 * @brief 发送句柄
 */
typedef struct xwpcp_carrier * xwpcp_txh_t;

/**
 * @brief 发送状态枚举
 */
enum xwpcp_carrier_state_em {
        XWPCP_CRS_IDLE = 0,
        XWPCP_CRS_READY,
        XWPCP_CRS_INPROGRESS,
        XWPCP_CRS_ABORT,
        XWPCP_CRS_FINISH,
};

/**
 * @brief 服务质量枚举
 */
enum xwpcp_msg_qos_em {
        XWPCP_MSG_QOS_0 = 0, /**< 不可靠消息，无CRC校验码 */
        XWPCP_MSG_QOS_1, /**< 不可靠消息，有CRC校验码 */
        XWPCP_MSG_QOS_2, /**< 可靠消息，无CRC校验码 */
        XWPCP_MSG_QOS_3, /**< 可靠消息，有CRC校验码 */
        XWPCP_MSG_QOS_NUM,

        XWPCP_MSG_QOS_CHKSUM_MSK = (1 << 0U),
        XWPCP_MSG_QOS_ACK_MSK = (1 << 1U),
};

/**
 * @brief 通知发送结果的回调函数
 */
typedef void (* xwpcp_ntf_f)(struct xwpcp * /* xwpcp */,
                             xwpcp_txh_t /* txh */,
                             xwer_t /* rc */,
                             void * /* arg */);

xwer_t xwpcp_start(struct xwpcp * xwpcp, const char * name,
                   const struct xwpcp_hwifal_operation * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize);
xwer_t xwpcp_stop(struct xwpcp * xwpcp);
xwer_t xwpcp_tx(struct xwpcp * xwpcp,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwtm_t * xwtm);
xwer_t xwpcp_eq(struct xwpcp * xwpcp,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwpcp_ntf_f ntfcb, void * cbarg,
                xwpcp_txh_t * txhbuf);
xwer_t xwpcp_abort(struct xwpcp * xwpcp, xwpcp_txh_t txh);
xwsq_t xwpcp_get_txstate(xwpcp_txh_t txh);
xwer_t xwpcp_rx(struct xwpcp * xwpcp, xwu8_t port,
                xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos,
                xwtm_t * xwtm);
xwer_t xwpcp_try_rx(struct xwpcp * xwpcp, xwu8_t port,
                    xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos);

#endif /* xwmd/isc/xwpcp/mif.h */
