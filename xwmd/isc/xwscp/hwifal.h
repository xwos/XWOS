/**
 * @file
 * @brief 精简的点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwscp_hwifal_h__
#define __xwmd_isc_xwscp_hwifal_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

#define XWSCP_HWIFAL_SOF        ((xwu8_t)'S')
#define XWSCP_HWIFAL_EOF        ((xwu8_t)(XWSCP_HWIFAL_SOF ^ 0xFF))
#define XWSCP_HWIFAL_SOF_SIZE   (1U)
#define XWSCP_HWIFAL_EOF_SIZE   (1U)

struct xwscp;
union xwscp_slot;

/**
 * @breif 硬件接口抽象层操作函数集
 */
struct xwscp_hwifal_operations {
        xwer_t (* open)(struct xwscp *); /**< 打开硬件接口 */
        xwer_t (* close)(struct xwscp *); /**< 关闭硬件接口 */
        xwer_t (* tx)(struct xwscp *, const xwu8_t *, xwsz_t); /**< 发送数据 */
        xwer_t (* rx)(struct xwscp *, xwu8_t *, xwsz_t *); /**< 接收数据 */
        void (* notify)(struct xwscp *, xwsq_t); /**< 通知事件 */
};

enum xwscp_hwifal_state_em {
        XWSCP_HWIFST_CLOSED, /**< 硬件接口已经关闭 */
        XWSCP_HWIFST_CONNECT = XWBOP_BIT(0), /**< 硬件接口正在连接 */
        XWSCP_HWIFST_RX = XWBOP_BIT(1), /**< 硬件接口正在接收数据 */
        XWSCP_HWIFST_TX = XWBOP_BIT(2), /**< 硬件接口正在发送一帧 */
};

enum xwscp_hwifal_notification_em {
        XWSCP_HWIFNTF_CONNECT = 0, /**< 与远程端连接 */
        XWSCP_HWIFNTF_NETUNREACH = 1, /**< 远程端无响应 */
};

xwer_t xwscp_hwifal_open(struct xwscp * xwscp, void * hwifcb);
xwer_t xwscp_hwifal_close(struct xwscp * xwscp);
xwer_t xwscp_hwifal_tx(struct xwscp * xwscp, xwu8_t * stream, xwsz_t size);
xwer_t xwscp_hwifal_rx(struct xwscp * xwscp, union xwscp_slot ** slotbuf);
void xwscp_hwifal_notify(struct xwscp * xwscp, xwsq_t evt);

#endif /* xwmd/isc/xwscp/hwifal.h */
