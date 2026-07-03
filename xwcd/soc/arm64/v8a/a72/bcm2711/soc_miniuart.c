/**
 * @file
 * @brief SOC描述层：紧急终端
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_aux.h>

void soc_miniuart_init(void)
{
        /* Init GPIO */
        soc_gpio.gpfsel1.b.fsel14 = SOC_GPIO_ALT5;
        soc_gpio.gpfsel1.b.fsel15 = SOC_GPIO_ALT5;
        soc_gpio.gpio_pup_pdn_cntrl_reg0.b.pin14 = SOC_GPIO_PUPDC_PU;
        soc_gpio.gpio_pup_pdn_cntrl_reg0.b.pin15 = SOC_GPIO_PUPDC_PU;

        /* Init UART */
        soc_aux.enables.b.mini_uart_en = 1U;
        soc_aux.mu.baud.u32 = 30U; /* 2M = 500M / (8 * (Baud + 1)) */
        soc_aux.mu.lcr.b.data_size = 3U; /* 8bit */
        soc_aux.mu.cntl.b.rx = 1U;
        soc_aux.mu.cntl.b.tx = 1U;
}

void soc_miniuart_putc(char c)
{
        while (soc_aux.mu.stat.b.tx_fifo_full) {
        }
        soc_aux.mu.io.u32 = (xwu32_t)c;
}

void soc_miniuart_puts(const char * s)
{
        xwsq_t idx;

        idx = 0;
        while ('\0' != s[idx]) {
                if (soc_aux.mu.lsr.b.tx_empty) {
                        soc_aux.mu.io.u32 = (xwu32_t)s[idx];
                        idx++;
                }
        }
}

void soc_miniuart_write(const xwu8_t * data, xwsz_t size)
{
        xwsq_t idx;

        idx = 0;
        while (idx < size) {
                if (soc_aux.mu.lsr.b.tx_empty) {
                        soc_aux.mu.io.u32 = (xwu32_t)data[idx];
                        idx++;
                }
        }
}

void soc_miniuart_flush_rx_fifo(void)
{
        __xwcc_unused xwu32_t data;
        while (soc_aux.mu.lsr.b.data_ready) {
                data = soc_aux.mu.io.u32;
        }
}
