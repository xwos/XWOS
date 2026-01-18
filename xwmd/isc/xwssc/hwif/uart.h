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

extern
const struct xwssc_hwifal_operation xwsscif_uart_ops; /**< UART接口操作函数集 */

/**
 * @} xwmd_isc_xwssc_hwif_uart
 */

#endif /* xwmd/isc/xwssc/hwif/uart.h */
