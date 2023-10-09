/**
 * @file
 * @brief SOC间点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwssc_hwifal_h__
#define __xwmd_isc_xwssc_hwifal_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/**
 * @ingroup xwmd_isc_xwssc_hwifal
 * @{
 */

struct xwssc;
union xwssc_slot;
struct xwssc_frm;

/**
 * @brief 硬件接口抽象层操作函数集
 */
struct xwssc_hwifal_operation {
        // cppcheck-suppress [misra-c2012-5.8]
        xwer_t (* open)(struct xwssc *); /**< 打开硬件接口 */
        // cppcheck-suppress [misra-c2012-5.8]
        xwer_t (* close)(struct xwssc *); /**< 关闭硬件接口 */
        xwer_t (* tx)(struct xwssc *, const xwu8_t *, xwsz_t); /**< 发送数据 */
        xwer_t (* rx)(struct xwssc *, xwu8_t *, xwsz_t *); /**< 接收数据 */
        void (* notify)(struct xwssc *, xwsq_t); /**< 通知事件 */
};

/**
 * @brief 硬件接口抽象层状态枚举
 */
enum xwssc_hwifal_state_em {
        XWSSC_HWIFST_CLOSED = 0U, /**< 硬件接口已经关闭 */
        XWSSC_HWIFST_CONNECT = XWBOP_BIT(0), /**< 硬件接口已经连接 */
        XWSSC_HWIFST_RX = XWBOP_BIT(1), /**< 硬件接口正在接收数据 */
        XWSSC_HWIFST_TX = XWBOP_BIT(2), /**< 硬件接口正在发送数据 */
};

/**
 * @brief 硬件接口层事件枚举
 */
enum xwssc_hwifal_notification_em {
        XWSSC_HWIFNTF_CONNECT = 0U, /**< 与远程端链接 */
        XWSSC_HWIFNTF_NETUNREACH = 1U, /**< 远程端无响应 */
};

xwer_t xwssc_hwifal_open(struct xwssc * xwssc, void * hwifcb);
xwer_t xwssc_hwifal_close(struct xwssc * xwssc);
xwer_t xwssc_hwifal_tx(struct xwssc * xwssc, const xwu8_t * stream, xwsz_t size);
xwer_t xwssc_hwifal_rx_head(struct xwssc * xwssc, struct xwssc_frm * frm);
xwer_t xwssc_hwifal_rx_body(struct xwssc * xwssc,
                            struct xwssc_frm * frm, xwsz_t sdusize);
void xwssc_hwifal_notify(struct xwssc * xwssc, xwsq_t evt);

/**
 * @} xwmd_isc_xwssc_hwifal
 */

#endif /* xwmd/isc/xwssc/hwifal.h */
