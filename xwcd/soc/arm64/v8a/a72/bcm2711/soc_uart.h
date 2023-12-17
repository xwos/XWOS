/**
 * @file
 * @brief SOC描述层：UART
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_uart_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_uart_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc.h>

#define SOC_UART_CLK (48000000U)

struct soc_uart_regs {
        xwu32_t dr; /**< 0x00 Data register */

        union {
                struct {
                        xwu32_t fe:1;
                        xwu32_t pe:1;
                        xwu32_t be:1;
                        xwu32_t oe:1;
                        xwu32_t reserved:28;
                } bit;
                xwu32_t u32;
        } rsrecr; /**< 0x04 */

        xwu32_t reserved0[4]; /**< 0x08 ~ 0x14 */

        union {
                struct {
                        xwu32_t cts:1;
                        xwu32_t dsr:1;
                        xwu32_t dcd:1;
                        xwu32_t busy:1;
                        xwu32_t rxfe:1;
                        xwu32_t txff:1;
                        xwu32_t rxff:1;
                        xwu32_t txfe:1;
                        xwu32_t ri:1;
                        xwu32_t reserved:23;
                } bit;
                xwu32_t u32;
        } fr; /**< 0x18 flag register */

        xwu32_t reserved1; /**< 0x1C */
        xwu32_t ilpr; /**< 0x20 */

        union {
                struct {
                        xwu32_t ibrd:16;
                        xwu32_t reserved:16;
                } bit;
                xwu32_t u32;
        } ibrd; /**< 0x24 Integer Baud rate divisor */

        union {
                struct {
                        xwu32_t fbrd:6;
                        xwu32_t reserved:26;
                } bit;
                xwu32_t u32;
        } fbrd; /**< 0x28 Fractional Baud rate divisor */

#define SOC_UART_LCRH_WLEN_8BIT 3U
#define SOC_UART_LCRH_WLEN_7BIT 2U
#define SOC_UART_LCRH_WLEN_6BIT 1U
#define SOC_UART_LCRH_WLEN_5BIT 0U
        union {
                struct {
                        xwu32_t brk:1;
                        xwu32_t pen:1;
                        xwu32_t eps:1;
                        xwu32_t stp2:1;
                        xwu32_t fen:1;
                        xwu32_t wlen:2;
                        xwu32_t sps:1;
                        xwu32_t reserved:24;
                } bit;
                xwu32_t u32;
        } lcrh; /**< 0x2C Line Control register */

        union {
                struct {
                        xwu32_t uarten:1;
                        xwu32_t siren:1;
                        xwu32_t sirlp:1;
                        xwu32_t reserved0:4;
                        xwu32_t lbe:1;
                        xwu32_t txe:1;
                        xwu32_t rxe:1;
                        xwu32_t dtr:1;
                        xwu32_t rts:1;
                        xwu32_t out1:1;
                        xwu32_t out2:1;
                        xwu32_t rtsen:1;
                        xwu32_t ctsen:1;
                        xwu32_t reserved1:16;
                } bit;
                xwu32_t u32;
        } cr; /**< 0x30 Control register */

#define SOC_UART_IFLS_FIFO_1_8 0U
#define SOC_UART_IFLS_FIFO_1_4 1U
#define SOC_UART_IFLS_FIFO_1_2 2U
#define SOC_UART_IFLS_FIFO_3_4 3U
#define SOC_UART_IFLS_FIFO_7_8 4U
        union {
                struct {
                        xwu32_t txiflsel:3;
                        xwu32_t rxiflsel:3;
                        xwu32_t txifpsel:3;
                        xwu32_t rxifpsel:3;
                        xwu32_t reserved:20;
                } bit;
                xwu32_t u32;
        } ifls; /**< 0x34 Interrupt FIFO Level Select Register */

        union {
                struct {
                        xwu32_t rimim:1;
                        xwu32_t ctsmim:1;
                        xwu32_t dcdmim:1;
                        xwu32_t dsrmim:1;
                        xwu32_t rxim:1;
                        xwu32_t txim:1;
                        xwu32_t rtim:1;
                        xwu32_t feim:1;
                        xwu32_t peim:1;
                        xwu32_t beim:1;
                        xwu32_t oeim:1;
                        xwu32_t reserved0:21;
                } bit;
                xwu32_t u32;
        } imsc; /**< 0x38 Interrupt Mask Set Clear Register */

        union {
                struct {
                        xwu32_t rimris:1;
                        xwu32_t ctsmris:1;
                        xwu32_t dcdmris:1;
                        xwu32_t dsrmris:1;
                        xwu32_t rxris:1;
                        xwu32_t txris:1;
                        xwu32_t rtris:1;
                        xwu32_t feris:1;
                        xwu32_t peris:1;
                        xwu32_t beris:1;
                        xwu32_t oeris:1;
                        xwu32_t reserved0:21;
                } bit;
                xwu32_t u32;
        } ris; /**< 0x3C Raw Interrupt Status Register */

        union {
                struct {
                        xwu32_t rimmis:1;
                        xwu32_t ctsmmis:1;
                        xwu32_t dcdmmis:1;
                        xwu32_t dsrmmis:1;
                        xwu32_t rxmis:1;
                        xwu32_t txmis:1;
                        xwu32_t rtmis:1;
                        xwu32_t femis:1;
                        xwu32_t pemis:1;
                        xwu32_t bemis:1;
                        xwu32_t oemis:1;
                        xwu32_t reserved0:21;
                } bit;
                xwu32_t u32;
        } mis; /**< 0x40 Masked Interrupt Status Register */

        union {
                struct {
                        xwu32_t rimic:1;
                        xwu32_t ctsmic:1;
                        xwu32_t dcdmic:1;
                        xwu32_t dsrmic:1;
                        xwu32_t rxic:1;
                        xwu32_t txic:1;
                        xwu32_t rtic:1;
                        xwu32_t feic:1;
                        xwu32_t peic:1;
                        xwu32_t beic:1;
                        xwu32_t oeic:1;
                        xwu32_t reserved0:21;
                } bit;
                xwu32_t u32;
        } icr; /**< 0x44 Interrupt Clear Register */

        union {
                struct {
                        xwu32_t rxdmae:1;
                        xwu32_t txdmae:1;
                        xwu32_t dmaonerr:1;
                        xwu32_t reserved0:29;
                } bit;
                xwu32_t u32;
        } dmacr; /**< 0x48 DMA Control Register */

        xwu32_t reserved2[13]; /**< 0x4C ~ 0x7C */
        xwu32_t itcr; /**< 0x80 Test Control register */
        xwu32_t itip; /**< 0x84 Integration test input reg */
        xwu32_t itop; /**< 0x88 Integration test output reg */
        xwu32_t tdr; /**< 0x8C Test Data reg */
};

#define soc_uart0 (*((volatile struct soc_uart_regs *)SOC_UART0_REGBASE))
#define soc_uart2 (*((volatile struct soc_uart_regs *)SOC_UART2_REGBASE))
#define soc_uart3 (*((volatile struct soc_uart_regs *)SOC_UART3_REGBASE))
#define soc_uart4 (*((volatile struct soc_uart_regs *)SOC_UART4_REGBASE))
#define soc_uart5 (*((volatile struct soc_uart_regs *)SOC_UART5_REGBASE))

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc_uart.h */
