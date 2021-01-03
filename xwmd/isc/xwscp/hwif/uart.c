/**
 * @file
 * @brief 精简的点对点通讯协议：UART硬件接口
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwcd/ds/uart/dma.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/hwif/uart.h>

static __xwmd_code
xwer_t xwscpif_uart_open(struct xwscp * xwscp);

static __xwmd_code
xwer_t xwscpif_uart_close(struct xwscp * xwscp);

static __xwmd_code
xwer_t xwscpif_uart_tx(struct xwscp * xwscp, const xwu8_t * data, xwsz_t size);

static __xwmd_code
xwer_t xwscpif_uart_rx(struct xwscp * xwscp, xwu8_t * buf, xwsz_t * size);

static __xwmd_code
void xwscpif_uart_notify(struct xwscp * xwscp, xwsq_t ntf);

__xwmd_rodata
const struct xwscp_hwifal_operations xwscpif_uart_ops = {
        .open = xwscpif_uart_open,
        .close = xwscpif_uart_close,
        .tx = xwscpif_uart_tx,
        .rx = xwscpif_uart_rx,
        .notify = xwscpif_uart_notify,
};

static __xwmd_code
xwer_t xwscpif_uart_open(struct xwscp * xwscp)
{
        XWOS_UNUSED(xwscp);
        return XWOK;
}

static __xwmd_code
xwer_t xwscpif_uart_close(struct xwscp * xwscp)
{
        XWOS_UNUSED(xwscp);
        return XWOK;
}

static __xwmd_code
xwer_t xwscpif_uart_tx(struct xwscp * xwscp, const xwu8_t * data, xwsz_t size)
{
        xwtm_t desired;

        desired = XWTM_MAX;
        return xwds_dmauartc_tx(xwscp->hwifcb, data, &size, &desired);
}

static __xwmd_code
xwer_t xwscpif_uart_rx(struct xwscp * xwscp, xwu8_t * buf, xwsz_t * size)
{
        xwtm_t desired;

        desired = XWTM_MAX;
	return xwds_dmauartc_rx(xwscp->hwifcb, buf, size, &desired);
}

static __xwmd_code
void xwscpif_uart_notify(struct xwscp * xwscp, xwsq_t ntf)
{
        XWOS_UNUSED(xwscp);
        if (XWSCP_HWIFNTF_NETUNREACH == ntf) {
        }
}
