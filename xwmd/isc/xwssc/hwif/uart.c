/**
 * @file
 * @brief SOC间点对点通讯协议：UART硬件接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwcd/ds/uart/dma.h>
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/hwifal.h>
#include <xwmd/isc/xwssc/hwif/uart.h>

static __xwmd_code
xwer_t xwsscif_uart_open(struct xwssc * xwssc);

static __xwmd_code
xwer_t xwsscif_uart_close(struct xwssc * xwssc);

static __xwmd_code
xwer_t xwsscif_uart_tx(struct xwssc * xwssc, const xwu8_t * data, xwsz_t size);

static __xwmd_code
xwer_t xwsscif_uart_rx(struct xwssc * xwssc, xwu8_t * buf, xwsz_t * size);

static __xwmd_code
void xwsscif_uart_notify(struct xwssc * xwssc, xwsq_t ntf);

__xwmd_rodata
const struct xwssc_hwifal_operation xwsscif_uart_ops = {
        .open = xwsscif_uart_open,
        .close = xwsscif_uart_close,
        .tx = xwsscif_uart_tx,
        .rx = xwsscif_uart_rx,
        .notify = xwsscif_uart_notify,
};

static __xwmd_code
xwer_t xwsscif_uart_open(struct xwssc * xwssc)
{
        XWOS_UNUSED(xwssc);
        return XWOK;
}

static __xwmd_code
xwer_t xwsscif_uart_close(struct xwssc * xwssc)
{
        XWOS_UNUSED(xwssc);
        return XWOK;
}

static __xwmd_code
xwer_t xwsscif_uart_tx(struct xwssc * xwssc, const xwu8_t * data, xwsz_t size)
{
        return xwds_dmauartc_tx(xwssc->hwifcb, data, &size, XWTM_MAX);
}

static __xwmd_code
xwer_t xwsscif_uart_rx(struct xwssc * xwssc, xwu8_t * buf, xwsz_t * size)
{
	return xwds_dmauartc_rx(xwssc->hwifcb, buf, size, XWTM_MAX);
}

static __xwmd_code
void xwsscif_uart_notify(struct xwssc * xwssc, xwsq_t ntf)
{
        XWOS_UNUSED(xwssc);
        if (XWSSC_HWIFNTF_NETUNREACH == ntf) {
        }
}
