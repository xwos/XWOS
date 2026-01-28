/**
 * @file
 * @brief SOC间点对点通讯协议：硬件接口层：UART
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwssc_hwif_uart_h__
#define __xwmd_isc_xwssc_hwif_uart_h__

#include <xwos/standard.h>
#include <xwmd/isc/xwssc/hwifal.h>

/**
 * @defgroup xwmd_isc_xwssc_hwif_uart SOC间点对点通讯协议：硬件接口层：UART
 * @ingroup xwmd_isc_xwssc_hwif
 * @{
 */

extern struct xwssc_hwifal_operation xwsscif_uart_ops; /**< UART接口操作函数集 */

/**
 * @brief XWSSCIF API: 设置状态通知回调函数
 * @param[in] notification: 回调函数
 * @details
 * + 当XWSSC与远程端连接成功，会通过 `notification(xwssc, XWSSC_HWIFNTF_CONNECT)`
 *   通知用户
 * + 当XWSSC与远程端连接断开，会通过 `notification(xwssc, XWSSC_HWIFNTF_NETUNREACH)`
 *   通知用户
 */
void xwsscif_uart_set_notification(void (* notification)(struct xwssc * , xwsq_t));

/**
 * @} xwmd_isc_xwssc_hwif_uart
 */

#endif /* xwmd/isc/xwssc/hwif/uart.h */
