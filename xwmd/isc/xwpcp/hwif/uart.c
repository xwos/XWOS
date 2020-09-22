/**
 * @file
 * @brief 点对点通讯协议：UART硬件接口
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/uart/dma.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/hwif/uart.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
xwer_t xwpcpif_uart_open(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcpif_uart_close(struct xwpcp * xwpcp);

static __xwmd_code
xwer_t xwpcpif_uart_tx(struct xwpcp * xwpcp, const xwu8_t * data, xwsz_t size);

static __xwmd_code
xwer_t xwpcpif_uart_rx(struct xwpcp * xwpcp, xwu8_t * buf, xwsz_t * size);

static __xwmd_code
void xwpcpif_uart_notify(struct xwpcp * xwpcp, xwsq_t ntf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_rodata
const struct xwpcp_hwifal_operations xwpcpif_uart_ops = {
        .open = xwpcpif_uart_open,
        .close = xwpcpif_uart_close,
        .tx = xwpcpif_uart_tx,
        .rx = xwpcpif_uart_rx,
        .notify = xwpcpif_uart_notify,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwmd_code
xwer_t xwpcpif_uart_open(struct xwpcp * xwpcp)
{
        XWOS_UNUSED(xwpcp);
        return XWOK;
}

static __xwmd_code
xwer_t xwpcpif_uart_close(struct xwpcp * xwpcp)
{
        XWOS_UNUSED(xwpcp);
        return XWOK;
}

static __xwmd_code
xwer_t xwpcpif_uart_tx(struct xwpcp * xwpcp, const xwu8_t * data, xwsz_t size)
{
        xwtm_t desired;

        desired = XWTM_MAX;
        return xwds_dmauartc_tx(xwpcp->hwifcb, data, &size, &desired);
}

static __xwmd_code
xwer_t xwpcpif_uart_rx(struct xwpcp * xwpcp, xwu8_t * buf, xwsz_t * size)
{
        xwtm_t desired;

        desired = XWTM_MAX;
	return xwds_dmauartc_rx(xwpcp->hwifcb, buf, size, &desired);
}

static __xwmd_code
void xwpcpif_uart_notify(struct xwpcp * xwpcp, xwsq_t ntf)
{
        XWOS_UNUSED(xwpcp);
        if (XWPCP_HWIFNTF_NETUNREACH == ntf) {
        }
}
