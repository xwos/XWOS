/**
 * @file
 * @brief W25Qxx编程器：UART硬件接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#include <xwos/standard.h>
#include <xwcd/ds/uart/dma.h>
#include <xwam/application/w25qrpt/w25qrpt.h>
#include <xwam/application/w25qrpt/hwifal.h>
#include <xwam/application/w25qrpt/hwif/uart.h>

static
xwer_t w25qrptif_uart_open(struct w25qrpt * w25qrpt);

static
xwer_t w25qrptif_uart_close(struct w25qrpt * w25qrpt);

static
xwer_t w25qrptif_uart_tx(struct w25qrpt * w25qrpt, const xwu8_t * data,
                         xwsz_t * size, xwtm_t * xwtm);

static
xwer_t w25qrptif_uart_rx(struct w25qrpt * w25qrpt, xwu8_t * buf,
                         xwsz_t * size, xwtm_t * xwtm);

const struct w25qrpt_hwifal_operations w25qrptif_uart_ops = {
        .open = w25qrptif_uart_open,
        .close = w25qrptif_uart_close,
        .tx = w25qrptif_uart_tx,
        .rx = w25qrptif_uart_rx,
};

static
xwer_t w25qrptif_uart_open(struct w25qrpt * w25qrpt)
{
        XWOS_UNUSED(w25qrpt);
        return XWOK;
}

static
xwer_t w25qrptif_uart_close(struct w25qrpt * w25qrpt)
{
        XWOS_UNUSED(w25qrpt);
        return XWOK;
}

static
xwer_t w25qrptif_uart_tx(struct w25qrpt * w25qrpt, const xwu8_t * data,
                         xwsz_t *size, xwtm_t * xwtm)
{
        return xwds_dmauartc_tx(w25qrpt->hwifcb, data, size, xwtm);
}

static
xwer_t w25qrptif_uart_rx(struct w25qrpt * w25qrpt, xwu8_t * buf,
                         xwsz_t * size, xwtm_t * xwtm)
{
	return xwds_dmauartc_rx(w25qrpt->hwifcb, buf, size, xwtm);
}
