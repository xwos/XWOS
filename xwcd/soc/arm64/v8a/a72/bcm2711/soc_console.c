/**
 * @file
 * @brief SOC描述层：紧急终端
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

#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_console.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_uart.h>

#define soc_console (*((volatile struct soc_uart_regs *)SOC_UART0_REGBASE))

void soc_console_init(void)
{
        /* Init GPIO */
        /* select ALT0 */
        soc_gpio.gpfsel1.bit.fsel14 = SOC_GPIO_ALT0;
        soc_gpio.gpfsel1.bit.fsel15 = SOC_GPIO_ALT0;

        /* Pullup */
        soc_gpio.gpio_pup_pdn_cntrl_reg0.bit.pin14 = SOC_GPIO_PUPDC_PU;
        soc_gpio.gpio_pup_pdn_cntrl_reg0.bit.pin15 = SOC_GPIO_PUPDC_PU;

        /* Init UART */
        soc_console.cr.u32 = 0U;
        soc_console.imsc.u32 = 0U;
	/*
         * baud divisor = UARTCLK / (16 * baud_rate)
         * baud divisor = 48M / (16 * 115200) = 26.0416666667
         * integer part = 26
         * fractional part = (int) ((0.0416666667 * 64) + 0.5) = 3
         * generated baud rate divisor = 26 + (3 / 64) = 26.046875
         * generated baud rate = 48M / (16 * 26.046875) = 115177
         * error = |(115177 - 115200) / 115200 * 100| = 0.02%
         */
        soc_console.ibrd.u32 = 26U;
        soc_console.fbrd.u32 = 3U;

        soc_console.lcrh.bit.fen = 1;
        soc_console.lcrh.bit.wlen = SOC_UART_LCRH_WLEN_8BIT;

        soc_console.cr.bit.rxe = 1;
        soc_console.cr.bit.txe = 1;
        soc_console.cr.bit.uarten = 1;
}

static void soc_console_putc(char c)
{
        while (soc_console.fr.bit.txff) {
        }
        soc_console.dr = (xwu32_t)c;
}

static char soc_console_getc(void)
{
        while (soc_console.fr.bit.rxfe) {
        }
        return (char)(soc_console.dr & (xwu32_t)0xFF);
}

void soc_console_puts(const char * s)
{
        while ('\0' != *s) {
                soc_console_putc(*s);
                s++;
        }
}

xwsz_t soc_console_gets(char * buf, xwsz_t bufsz)
{
        xwsz_t i = 0;

        while (i < bufsz) {
                buf[i] = soc_console_getc();
                i++;
                if ('\0' == buf[i]) {
                        break;
                }
        }
        return i;
}

void soc_console_tx(const xwu8_t * d, xwsz_t size)
{
        xwsz_t i = 0;

        while (size > i) {
                soc_console_putc(d[i]);
                i++;
        }
}

xwsz_t soc_console_rx(char * buf, xwsz_t bufsz)
{
        xwsz_t i = 0;

        while (i < bufsz) {
                buf[i] = soc_console_getc();
                i++;
        }
        return i;
}
