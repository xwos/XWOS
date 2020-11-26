/**
 * @file
 * @brief 点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwpcp_hwifal_h__
#define __xwmd_isc_xwpcp_hwifal_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

struct xwpcp;
union xwpcp_slot;
struct xwpcp_frm;

/**
 * @breif 硬件接口抽象层操作函数集
 */
struct xwpcp_hwifal_operation {
        xwer_t (* open)(struct xwpcp *); /**< 打开硬件接口 */
        xwer_t (* close)(struct xwpcp *); /**< 关闭硬件接口 */
        xwer_t (* tx)(struct xwpcp *, const xwu8_t *, xwsz_t); /**< 发送数据 */
        xwer_t (* rx)(struct xwpcp *, xwu8_t *, xwsz_t *); /**< 接收数据 */
        void (* notify)(struct xwpcp *, xwsq_t); /**< 通知事件 */
};

/**
 * @breif 硬件接口抽象层状态枚举
 */
enum xwpcp_hwifal_state_em {
        XWPCP_HWIFST_CLOSED, /**< 硬件接口已经关闭 */
        XWPCP_HWIFST_CONNECT = BIT(0), /**< 硬件接口已经链接 */
        XWPCP_HWIFST_RX = BIT(1), /**< 硬件接口正在接收数据 */
        XWPCP_HWIFST_TX = BIT(2), /**< 硬件接口正在发送一帧 */
};

/**
 * @breif 硬件接口层事件枚举
 */
enum xwpcp_hwifal_notification_em {
        XWPCP_HWIFNTF_CONNECT = 0, /**< 与远程端链接 */
        XWPCP_HWIFNTF_NETUNREACH = 1, /**< 远程端无响应 */
};

xwer_t xwpcp_hwifal_open(struct xwpcp * xwpcp, void * hwifcb);
xwer_t xwpcp_hwifal_close(struct xwpcp * xwpcp);
xwer_t xwpcp_hwifal_tx_raw(struct xwpcp * xwpcp, xwu8_t * stream, xwsz_t size);
xwer_t xwpcp_hwifal_tx(struct xwpcp * xwpcp, union xwpcp_slot * slot);
xwer_t xwpcp_hwifal_rx(struct xwpcp * xwpcp, union xwpcp_slot ** slotbuf);
void xwpcp_hwifal_notify(struct xwpcp * xwpcp, xwsq_t evt);

#endif /* xwmd/isc/xwpcp/hwifal.h */
