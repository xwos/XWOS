/**
 * @file
 * @brief 精简的点对点通讯协议：UART硬件接口
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/uart/dma.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/hwif/uart.h>

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
xwer_t xwscpif_uart_open(struct xwscp * xwscp);

static __xwmd_code
xwer_t xwscpif_uart_close(struct xwscp * xwscp);

static __xwmd_code
xwer_t xwscpif_uart_tx(struct xwscp * xwscp, const xwu8_t * data,
                       xwsz_t size, xwtm_t * xwtm);

static __xwmd_code
xwer_t xwscpif_uart_rx(struct xwscp * xwscp, xwu8_t * buf, xwsz_t * size);

static __xwmd_code
void xwscpif_uart_notify(struct xwscp * xwscp, xwsq_t ntf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_rodata
const struct xwscp_hwifal_operations xwscpif_uart_ops = {
        .open = xwscpif_uart_open,
        .close = xwscpif_uart_close,
        .tx = xwscpif_uart_tx,
        .rx = xwscpif_uart_rx,
        .notify = xwscpif_uart_notify,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
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
xwer_t xwscpif_uart_tx(struct xwscp * xwscp, const xwu8_t * data,
                       xwsz_t size, xwtm_t * xwtm)
{
        return xwds_dmauartc_tx(xwscp->hwifcb, data, &size, xwtm);
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
