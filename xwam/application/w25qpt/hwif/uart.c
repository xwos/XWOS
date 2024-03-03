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
#include <xwcd/ds/uart/controller.h>
#include <xwam/application/w25qpt/w25qpt.h>
#include <xwam/application/w25qpt/hwifal.h>
#include <xwam/application/w25qpt/hwif/uart.h>

static
xwer_t w25qptif_uart_open(struct w25qpt * w25qpt);

static
xwer_t w25qptif_uart_close(struct w25qpt * w25qpt);

static
xwer_t w25qptif_uart_tx(struct w25qpt * w25qpt, const xwu8_t * data,
                        xwsz_t * size, xwtm_t to);

static
xwer_t w25qptif_uart_rx(struct w25qpt * w25qpt, xwu8_t * buf,
                        xwsz_t * size, xwtm_t to);

const struct w25qpt_hwifal_operations w25qptif_uart_ops = {
        .open = w25qptif_uart_open,
        .close = w25qptif_uart_close,
        .tx = w25qptif_uart_tx,
        .rx = w25qptif_uart_rx,
};

static
xwer_t w25qptif_uart_open(struct w25qpt * w25qpt)
{
        XWOS_UNUSED(w25qpt);
        return XWOK;
}

static
xwer_t w25qptif_uart_close(struct w25qpt * w25qpt)
{
        XWOS_UNUSED(w25qpt);
        return XWOK;
}

static
xwer_t w25qptif_uart_tx(struct w25qpt * w25qpt, const xwu8_t * data,
                        xwsz_t *size, xwtm_t to)
{
        return xwds_uartc_tx(w25qpt->hwifcb, data, size, to);
}

static
xwer_t w25qptif_uart_rx(struct w25qpt * w25qpt, xwu8_t * buf,
                        xwsz_t * size, xwtm_t to)
{
	return xwds_uartc_rx(w25qpt->hwifcb, buf, size, to);
}
