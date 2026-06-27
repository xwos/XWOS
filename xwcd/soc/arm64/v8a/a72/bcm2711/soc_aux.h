/**
 * @file
 * @brief SOC描述层：Auxiliaries
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_uart_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_uart_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc.h>

#define SOC_AUX_CLK (500000000U)

struct soc_aux_regs {
        union {
                struct {
                        xwu32_t mini_uart_irq:1;
                        xwu32_t spi1_irq:1;
                        xwu32_t spi2_irq:1;
                        xwu32_t reserved:29;
                } b;
                xwu32_t u32;
        } irq; /**< 0x00, Interrupt Status */

        union {
                struct {
                        xwu32_t mini_uart_en:1;
                        xwu32_t spi1_en:1;
                        xwu32_t spi2_en:1;
                        xwu32_t reserved:29;
                } b;
                xwu32_t u32;
        } enables; /**< 0x04, Enables */

        xwu32_t reserved0[14]; /**< 0x08 ~ 0x3C */

        struct {
                union {
                        struct {
                                xwu8_t data;
                                xwu8_t reserved0[3];
                        } b;
                        xwu32_t u32;
                } io; /**< 0x040, Mini Uart I/O Data */
                union {
                        struct {
                                xwu32_t rx:1;
                                xwu32_t tx:1;
                                xwu32_t reserved1:30;
                        } b;
                        xwu32_t u32;
                } ier; /**< 0x044, Mini Uart Interrupt Enable */
                union {
                        struct {
                                xwu32_t interrupt_pending:1;
                                xwu32_t status:2;
                                xwu32_t reserved0:3;
                                xwu32_t fifo_en:2;
                                xwu32_t reserved1:24;
                        } b;
                        xwu32_t u32;
                } iir; /**< 0x048, Mini Uart Interrupt Identify */
                union {
                        struct {
                                xwu32_t data_size:2;
                                xwu32_t reserved0:4;
                                xwu32_t break_frame:1;
                                xwu32_t dlab:1;
                                xwu32_t reserved1:24;
                        } b;
                        xwu32_t u32;
                } lcr; /**< 0x04C, Mini Uart Line Control */
                union {
                        struct {
                                xwu32_t reserved0:1;
                                xwu32_t rts:1;
                                xwu32_t reserved1:30;
                        } b;
                        xwu32_t u32;
                } mcr; /**< 0x050, Mini Uart Modem Control */
                union {
                        struct {
                                xwu32_t data_ready:1;
                                xwu32_t rx_overrun:1;
                                xwu32_t reserved0:3;
                                xwu32_t tx_empty:1;
                                xwu32_t tx_idle:1;
                                xwu32_t reserved1:25;
                        } b;
                        xwu32_t u32;
                } lsr; /**< 0x054, Mini Uart Line Status */
                union {
                        struct {
                                xwu32_t reserved0:4;
                                xwu32_t cts_status:1;
                                xwu32_t reserved1:27;
                        } b;
                        xwu32_t u32;
                } msr; /**< 0x058, Mini Uart Modem Status */
                union {
                        xwu32_t u32;
                } scratch; /**< 0x05C, Mini Uart Scratch */
                union {
                        struct {
                                xwu32_t rx:1;
                                xwu32_t tx:1;
                                xwu32_t rts_en:1;
                                xwu32_t cts_en:1;
                                xwu32_t rts_level:2;
                                xwu32_t rts_assert_level:1;
                                xwu32_t cts_assert_level:1;
                                xwu32_t reserved0:24;
                        } b;
                        xwu32_t u32;
                } cntl; /**< 0x060, Mini Uart Extra Control */
                union {
                        struct {
                                xwu32_t rx_available:1;
                                xwu32_t tx_available:1;
                                xwu32_t rx_idle:1;
                                xwu32_t tx_idle:1;
                                xwu32_t rx_overrun:1;
                                xwu32_t tx_fifo_full:1;
                                xwu32_t rts:1;
                                xwu32_t cts:1;
                                xwu32_t tx_fifo_empty:1;
                                xwu32_t tx_done:1;
                                xwu32_t reserved0:6;
                                xwu32_t rx_fifo_fill_level:4;
                                xwu32_t reserved1:4;
                                xwu32_t tx_fifo_fill_level:4;
                                xwu32_t reserved2:4;
                        } b;
                        xwu32_t u32;
                } stat; /**< 0x064, Mini Uart Extra Status */
                union {
                        struct {
                                xwu16_t baudrate;
                                xwu16_t reserved0;
                        } b;
                        xwu32_t u32;
                } baud; /**< 0x068, Mini Uart Baudrate */
        } miniuart;
};

#define soc_aux (*((volatile struct soc_aux_regs *)SOC_AUX_REGBASE))

void soc_miniuart_init(void);

static __xwbsp_inline
void soc_miniuart_enable_tx_irq(void)
{
        soc_aux.miniuart.ier.b.tx = 1U;
}

static __xwbsp_inline
void soc_miniuart_disable_tx_irq(void)
{
        soc_aux.miniuart.ier.b.tx = 0U;
}

static __xwbsp_inline
void soc_miniuart_enable_rx_irq(void)
{
        soc_aux.miniuart.ier.b.rx = 1U;
}

static __xwbsp_inline
void soc_miniuart_disable_rx_irq(void)
{
        soc_aux.miniuart.ier.b.rx = 0U;
}

void soc_miniuart_putc(char c);
void soc_miniuart_puts(const char * s);
void soc_miniuart_write(const xwu8_t * d, xwsz_t size);
void soc_miniuart_flush_rx_fifo(void);

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc_uart.h */
