/**
 * @file
 * @brief SOC描述层：UART
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

#ifndef __xwcd_soc_arm_v8a_a76a55_a7870_soc_uart_h__
#define __xwcd_soc_arm_v8a_a76a55_a7870_soc_uart_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/a76a55/a7870/soc.h>

struct soc_ap_uart_regs {
        xwu32_t txd; /**< 0x00 */
        xwu32_t rxd; /**< 0x04 */
        union {
                struct {
                        xwu32_t rxf_full_raw_sts : 1;
                        xwu32_t txf_empty_raw_sts : 1;
                        xwu32_t parity_err_raw_sts : 1;
                        xwu32_t frame_err_raw_sts : 1;
                        xwu32_t rxf_overrun_raw_sts : 1;
                        xwu32_t dsr_chg_raw_sts : 1;
                        xwu32_t cts_chg_raw_sts : 1;
                        xwu32_t brk_dtct_raw_sts : 1;
                        xwu32_t dsr : 1;
                        xwu32_t cts : 1;
                        xwu32_t rts : 1;
                        xwu32_t rxd : 1;
                        xwu32_t rxf_realempty : 1;
                        xwu32_t time_out_raw_sts : 1;
                        xwu32_t rxf_realfull : 1;
                        xwu32_t trans_over : 1;
                        xwu32_t abd_done_raw_sts : 1;
                        xwu32_t reserved : 15;
                } bit;
                xwu32_t u32;
        } sts0; /**< 0x08 */
        union {
                struct {
                        xwu32_t rxf_cnt : 8;
                        xwu32_t txf_cnt : 9;
                        xwu32_t tx_fifo_depth : 4;
                        xwu32_t rx_fifo_depth : 3;
                        xwu32_t reserved : 8;
                } bit;
                xwu32_t u32;
        } sts1; /**< 0x0C */
        union {
                struct {
                        xwu32_t rxf_full_int_en : 1;
                        xwu32_t txf_empty_int_en : 1;
                        xwu32_t parity_err_int_en : 1;
                        xwu32_t frame_err_int_en : 1;
                        xwu32_t rxf_overrun_int_en : 1;
                        xwu32_t dsr_chg_int_en : 1;
                        xwu32_t cts_chg_int_en : 1;
                        xwu32_t brk_dtct_int_en : 1;
                        xwu32_t reserved0 : 5;
                        xwu32_t time_out_int_en : 1;
                        xwu32_t reserved1 : 2;
                        xwu32_t abd_done_int_en : 1;
                        xwu32_t reserved : 15;
                } bit;
                xwu32_t u32;
        } ien; /**< 0x10 */
        union {
                struct {
                        xwu32_t reserved0 : 2;
                        xwu32_t parity_err_int_clr : 1;
                        xwu32_t frame_err_int_clr : 1;
                        xwu32_t rxf_overrun_int_clr : 1;
                        xwu32_t dsr_chg_int_clr : 1;
                        xwu32_t cts_chg_int_clr : 1;
                        xwu32_t brk_dtct_int_clr : 1;
                        xwu32_t reserved1 : 5;
                        xwu32_t time_out_int_clr : 1;
                        xwu32_t reserved2 : 2;
                        xwu32_t abd_done_int_clr : 1;
                        xwu32_t reserved3 : 15;
                } bit;
                xwu32_t u32;
        } iclr; /**< 0x14 */
        union {
                struct {
                        xwu32_t odd_parity : 1;
                        xwu32_t parity_en : 1;
                        xwu32_t byte_len : 2;
                        xwu32_t stop_bit_num : 2;
                        xwu32_t rts_reg : 1;
                        xwu32_t send_brk_en : 1;
                        xwu32_t dtr_reg : 1;
                        xwu32_t ir_tx_iv : 1;
                        xwu32_t ir_rx_iv : 1;
                        xwu32_t ir_tx_en : 1;
                        xwu32_t ir_dplx : 1;
                        xwu32_t ir_wctl : 1;
                        xwu32_t tx_mode : 1;
                        xwu32_t mode_sel : 1;
                        xwu32_t rx_mode : 1;
                        xwu32_t tx_data_swt_word : 1;
                        xwu32_t rx_data_swt_word : 1;
                        xwu32_t reserved0 : 13;
                } bit;
                xwu32_t u32;
        } ctrl0; /**< 0x18 */
        union {
                struct {
                        xwu32_t rcv_hw_flow_thld : 7;
                        xwu32_t rcv_hw_flow_en : 1;
                        xwu32_t tx_hw_flow_en : 1;
                        xwu32_t rx_tout_thld : 5;
                        xwu32_t loop_back : 1;
                        xwu32_t dma_en : 1;
                        xwu32_t tx_stop_bit_num : 2;
                        xwu32_t tx_stop_bit_en : 1;
                        xwu32_t abd_en : 1;
                        xwu32_t reserved0 : 12;
                } bit;
                xwu32_t u32;
        } ctrl1; /**< 0x1C */
        union {
                struct {
                        xwu32_t rxf_full_thld : 7;
                        xwu32_t reserved0 : 1;
                        xwu32_t txf_empty_thld : 8;
                        xwu32_t reserved1 : 16;
                } bit;
                xwu32_t u32;
        } ctrl2; /**< 0x20 */
        union {
                struct {
                        xwu32_t uart_ckd0 : 16;
                        xwu32_t reserved0 : 16;
                } bit;
                xwu32_t u32;
        } ckd0;           /**< 0x24 */
        xwu32_t reserved; /**< 0x28 */
        union {
                struct {
                        xwu32_t rxf_full_mask_sts : 1;
                        xwu32_t txf_empty_mask_sts : 1;
                        xwu32_t parity_err_mask_sts : 1;
                        xwu32_t frame_err_mask_sts : 1;
                        xwu32_t rxf_overrun_mask_sts : 1;
                        xwu32_t dsr_chg_mask_sts : 1;
                        xwu32_t cts_chg_mask_sts : 1;
                        xwu32_t brk_dtct_mask_sts : 1;
                        xwu32_t reserved0 : 5;
                        xwu32_t time_out_mask_sts : 1;
                        xwu32_t reserved1 : 2;
                        xwu32_t abd_done_mask_sts : 1;
                        xwu32_t reserved2 : 15;
                } bit;
                xwu32_t u32;
        } sts2;          /**< 0x02C */
        xwu32_t dspwait; /**< 0x30 */
        xwu32_t sfctl;   /**< 0x34 */
        xwu32_t xcmd;    /**< 0x38 */
        xwu32_t escape;  /**< 0x3C */
        xwu32_t abd0;    /**< 0x40 */
        xwu32_t abd1;    /**< 0x44 */
        xwu32_t abd2;    /**< 0x48 */
        xwu32_t version; /**< 0x4C */
};

typedef union {
        uint32_t u32;
        struct {
                unsigned int rsv0 : 1;        // Bit 0: rsv
                unsigned int iis0_eb : 1;     // Bit 1: 启用 IIS0
                unsigned int iis1_eb : 1;     // Bit 2: 启用 IIS1
                unsigned int iis2_eb : 1;     // Bit 3: 启用 IIS2
                unsigned int apb_reg_eb : 1;  // Bit 4: not used
                unsigned int spi0_eb : 1;     // Bit 5: 启用 SPI0
                unsigned int spi1_eb : 1;     // Bit 6: 启用 SPI1
                unsigned int spi2_eb : 1;     // Bit 7: 启用 SPI2
                unsigned int uart3_eb : 1;    // Bit 8: 启用 uart3
                unsigned int rsv1 : 5;        // Bit 9-13: rsv
                unsigned int uart0_eb : 1;    // Bit 14: 启用 uart0
                unsigned int uart1_eb : 1;    // Bit 15: 启用 uart1
                unsigned int uart2_eb : 1;    // Bit 16: 启用 uart2
                unsigned int rsv2 : 1;        // Bit 17: rsv
                unsigned int spi0_lcd_eb : 1; // Bit 18: 启用spi0_lcd
                unsigned int spi1_lcd_eb : 1; // Bit 19: 启用spi1_lcd
                unsigned int spi2_lcd_eb : 1; // Bit 20: 启用spi2_lcd
                unsigned int spi3_lcd_eb : 1; // Bit 21: 启用spi3_lcd
                unsigned int rsv3 : 8;        // Bit 22-29: rsv
                unsigned int ce_sec_eb : 1;   // Bit 30: 启用 ce_sec
                unsigned int ce_pub_eb : 1;   // Bit 31: 启用 ce_pub
        } bits;                               // 位域结构体，每位代表一个功能
} eb_apb_reg_t;

#define SOC_AP_UART_CLK 26000000UL

#define soc_ap_uart0 (*((volatile struct soc_ap_uart_regs *)SOC_AP_UART0_PHYS))
#define soc_ap_uart1 (*((volatile struct soc_ap_uart_regs *)SOC_AP_UART1_PHYS))
#define soc_ap_uart2 (*((volatile struct soc_ap_uart_regs *)SOC_AP_UART2_PHYS))
#define soc_ap_uart3 (*((volatile struct soc_ap_uart_regs *)SOC_AP_UART3_PHYS))

#endif /* xwcd/soc/arm64/v8a/a76a55/a7870/soc_uart.h */
