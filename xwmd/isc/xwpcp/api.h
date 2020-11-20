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

#ifndef __xwmd_isc_xwpcp_api_h__
#define __xwmd_isc_xwpcp_api_h__

#include <xwos/standard.h>

#define XWPCP_QOS(q)            ((xwu8_t)((q) << 7))
#define XWPCP_QOS_MSK           XWPCP_QOS(1U)

struct xwpcp;
struct xwpcp_hwifal_operations;
struct xwpcp_frmslot;

/**
 * @brief 帧句柄
 */
typedef struct xwpcp_frmslot * xwpcp_fhdl_t;

enum xwpcp_msg_qos_em {
        XWPCP_MSG_QOS_0 = XWPCP_QOS(0), /**< 只传输一次，不管对方是否收到 */
        XWPCP_MSG_QOS_1 = XWPCP_QOS(1), /**< 传输完成后需要等待对方确认收到，
                                             未收到确认应答会重新传输 */
};

/**
 * @brief 异步通知回调函数
 */
typedef void (*xwpcp_ntf_f)(struct xwpcp * /* xwpcp */,
                            xwpcp_fhdl_t /* hdl */,
                            xwer_t /* rc */,
                            void * /* arg */);

/**
 * @brief XWPCP用户报文
 */
struct xwpcp_msg {
        xwu8_t port;
        xwu8_t qos;
        xwsz_t size;
        xwu8_t * text;
};

xwer_t xwpcp_start(struct xwpcp * xwpcp, const char * name,
                   const struct xwpcp_hwifal_operations * hwifops,
                   void * hwifcb);
xwer_t xwpcp_stop(struct xwpcp * xwpcp);
xwer_t xwpcp_tx(struct xwpcp * xwpcp,
                const struct xwpcp_msg * msg, xwu8_t prio,
                xwtm_t * xwtm);
xwer_t xwpcp_eq(struct xwpcp * xwpcp,
                const struct xwpcp_msg * msg, xwu8_t prio,
                xwpcp_ntf_f cbfunc, void * cbarg,
                xwpcp_fhdl_t * fhdlbuf);
void xwpcp_lock_notif(struct xwpcp * xwpcp);
void xwpcp_unlock_notif(struct xwpcp * xwpcp);
xwer_t xwpcp_rx(struct xwpcp * xwpcp, struct xwpcp_msg * msgbuf, xwtm_t * xwtm);
xwer_t xwpcp_try_rx(struct xwpcp * xwpcp, struct xwpcp_msg * msgbuf);

#endif /* xwmd/isc/xwpcp/api.h */
