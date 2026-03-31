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
#include <xwcd/soc/arm64/v8a/a76a55/a7870/soc.h>
#include <xwcd/soc/arm64/v8a/a76a55/a7870/soc_uart.h>

#define soc_console soc_ap_uart1

static void soc_console_putc(char c)
{
        volatile xwsz_t fifo;

        do {
                fifo = ((1UL << soc_console.sts1.bit.tx_fifo_depth) -
                        soc_console.sts1.bit.txf_cnt);
        } while (0 == fifo);
        soc_console.txd = (xwu32_t)c;
}

static char soc_console_getc(void)
{
        while (0 == soc_console.sts1.bit.rxf_cnt) {
        }
        return (char)(soc_console.rxd & (xwu32_t)0xFF);
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
