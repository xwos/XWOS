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

#ifndef __xwmd_isc_xwpcp_mif_h__
#define __xwmd_isc_xwpcp_mif_h__

#include <xwos/standard.h>

struct xwpcp;
struct xwpcp_hwifal_operation;
union xwpcp_slot;

/**
 * @brief 帧句柄
 */
typedef union xwpcp_slot * xwpcp_fh_t;

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
                             xwpcp_fh_t /* fh */,
                             xwer_t /* rc */,
                             void * /* arg */);

xwer_t xwpcp_start(struct xwpcp * xwpcp, const char * name,
                   const struct xwpcp_hwifal_operation * hwifops,
                   void * hwifcb);
xwer_t xwpcp_stop(struct xwpcp * xwpcp);
xwer_t xwpcp_tx(struct xwpcp * xwpcp,
                xwu8_t data[], xwsz_t size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwtm_t * xwtm);
xwer_t xwpcp_eq(struct xwpcp * xwpcp,
                xwu8_t data[], xwsz_t size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwpcp_ntf_f ntfcb, void * cbarg,
                xwpcp_fh_t * fhbuf);
void xwpcp_lock_notif(struct xwpcp * xwpcp);
void xwpcp_unlock_notif(struct xwpcp * xwpcp);
xwer_t xwpcp_rx(struct xwpcp * xwpcp, xwu8_t port,
                xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos,
                xwtm_t * xwtm);
xwer_t xwpcp_try_rx(struct xwpcp * xwpcp, xwu8_t port,
                    xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos);

#endif /* xwmd/isc/xwpcp/mif.h */
