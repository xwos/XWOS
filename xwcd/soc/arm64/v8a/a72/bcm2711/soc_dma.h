/**
 * @file
 * @brief SOC描述层：DMA
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_dma_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_dma_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc.h>

/**
 * @brief 普通DMA通道(0~6,15)的控制块
 */
struct soc_dma_cb {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t inten:1;
                        xwu32_t tdmode:1;
                        xwu32_t reserved0:1;
                        xwu32_t wait_resp:1;
                        xwu32_t dest_inc:1;
                        xwu32_t dest_width:1;
                        xwu32_t dest_dreq:1;
                        xwu32_t reserved1:1;
                        xwu32_t src_inc:1;
                        xwu32_t src_width:1;
                        xwu32_t src_dreq:1;
                        xwu32_t src_ignore:1;
                        xwu32_t burst_length:4;
                        xwu32_t permap:5;
                        xwu32_t waits:5;
                        xwu32_t no_wide_bursts:1;
                        xwu32_t reserved2:5;
                } b;
        } ti; /**< 0x00 DMA Transfer Information */
        xwu32_t src; /**< 0x04 DMA Source Address */
        xwu32_t dst; /**< 0x08 DMA Destination Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t xlength:16;
                        xwu32_t ylength:14;
                        xwu32_t reserved0:2;
                } b;
        } size; /**< 0x0C DMA Transfer Length */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t s_stride:16;
                        xwu32_t d_stride:16;
                } b;
        } stride; /**< 0x10 DMA 2D Stride */
        xwu32_t next; /**< 0x14 DMA Next Control Block Address */
        xwu32_t reserved0[2]; /**< 0x18 ~ 0x1C */
};

/**
 * @brief 普通DMA通道(0~6,15)的寄存器
 */
struct soc_dma_regs {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t active:1;
                        xwu32_t end:1;
                        xwu32_t intr:1;
                        xwu32_t dreq:1;
                        xwu32_t reserved0:4;
                        xwu32_t error:1;
                        xwu32_t reserved1:7;
                        xwu32_t priority:4;
                        xwu32_t panic_priority:4;
                        xwu32_t reserved2:4;
                        xwu32_t wait_for_outstanding_writes:1;
                        xwu32_t disdebug:1;
                        xwu32_t abort:1;
                        xwu32_t reset:1;
                } b;
        } cs; /**< 0x00 DMA Channel Control and Status */
        union {
                xwu32_t u32;
        } cb; /**< 0x04 DMA Channel Control Block Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t inten:1;
                        xwu32_t tdmode:1;
                        xwu32_t reserved0:1;
                        xwu32_t wait_resp:1;
                        xwu32_t dest_inc:1;
                        xwu32_t dest_width:1;
                        xwu32_t dest_dreq:1;
                        xwu32_t reserved1:1;
                        xwu32_t src_inc:1;
                        xwu32_t src_width:1;
                        xwu32_t src_dreq:1;
                        xwu32_t src_ignore:1;
                        xwu32_t burst_length:4;
                        xwu32_t permap:5;
                        xwu32_t waits:5;
                        xwu32_t no_wide_bursts:1;
                        xwu32_t reserved2:5;
                } b;
        } ti; /**< 0x08 DMA Transfer Information */
        xwu32_t src; /**< 0x0C DMA Source Address */
        xwu32_t dst; /**< 0x10 DMA Destination Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t xlength:16;
                        xwu32_t ylength:14;
                        xwu32_t reserved0:2;
                } b;
        } size; /**< 0x14 DMA Transfer Length */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t s_stride:16;
                        xwu32_t d_stride:16;
                } b;
        } stride; /**< 0x18 DMA 2D Stride */
        xwu32_t next; /**< 0x1C DMA Next Control Block Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t read_last_not_set_error:1;
                        xwu32_t fifo_error:1;
                        xwu32_t read_error:1;
                        xwu32_t reserved0:1;
                        xwu32_t outstanding_writes:4;
                        xwu32_t dma_id:8;
                        xwu32_t dma_state:9;
                        xwu32_t version:3;
                        xwu32_t lite:1;
                        xwu32_t reserved1:3;
                } b;
        } debug; /**< 0x20 DMA Debug */
};

/**
 * @brief DMALite通道(7~10)的控制块
 */
struct soc_dma_lite_cb {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t inten:1;
                        xwu32_t reserved0:1;
                        xwu32_t reserved1:1;
                        xwu32_t wait_resp:1;
                        xwu32_t dest_inc:1;
                        xwu32_t dest_width:1;
                        xwu32_t dest_dreq:1;
                        xwu32_t reserved2:1;
                        xwu32_t src_inc:1;
                        xwu32_t src_width:1;
                        xwu32_t src_dreq:1;
                        xwu32_t reserved3:1;
                        xwu32_t burst_length:4;
                        xwu32_t permap:5;
                        xwu32_t waits:5;
                        xwu32_t reserved4:6;
                } b;
        } ti; /**< 0x00 DMA Lite Transfer Information */
        xwu32_t src; /**< 0x04 DMA Source Address */
        xwu32_t dst; /**< 0x08 DMA Destination Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t xlength:16;
                        xwu32_t reserved0:16;
                } b;
        } size; /**< 0x0C DMA Lite Transfer Length */
        xwu32_t reserved0; /**< 0x10 Reserved */
        xwu32_t next; /**< 0x14 DMA Next Control Block Address */
        xwu32_t reserved1[2]; /**< 0x18 ~ 0x1C */
};

/**
 * @brief DMALite通道(7~10)的寄存器
 */
struct soc_dma_lite_regs {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t active:1;
                        xwu32_t end:1;
                        xwu32_t intr:1;
                        xwu32_t dreq:1;
                        xwu32_t reserved0:4;
                        xwu32_t error:1;
                        xwu32_t reserved1:7;
                        xwu32_t priority:4;
                        xwu32_t panic_priority:4;
                        xwu32_t reserved2:4;
                        xwu32_t wait_for_outstanding_writes:1;
                        xwu32_t disdebug:1;
                        xwu32_t abort:1;
                        xwu32_t reset:1;
                } b;
        } cs; /**< 0x00 DMA Channel Control and Status */
        union {
                xwu32_t u32;
        } cb; /**< 0x04 DMA Channel Control Block Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t inten:1;
                        xwu32_t reserved0:1;
                        xwu32_t reserved1:1;
                        xwu32_t wait_resp:1;
                        xwu32_t dest_inc:1;
                        xwu32_t dest_width:1;
                        xwu32_t dest_dreq:1;
                        xwu32_t reserved2:1;
                        xwu32_t src_inc:1;
                        xwu32_t src_width:1;
                        xwu32_t src_dreq:1;
                        xwu32_t reserved3:1;
                        xwu32_t burst_length:4;
                        xwu32_t permap:5;
                        xwu32_t waits:5;
                        xwu32_t reserved4:6;
                } b;
        } ti; /**< 0x08 DMA Lite Transfer Information */
        xwu32_t src; /**< 0x0C DMA Source Address */
        xwu32_t dst; /**< 0x10 DMA Destination Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t xlength:16;
                        xwu32_t reserved0:16;
                } b;
        } size; /**< 0x14 DMA Lite Transfer Length */
        xwu32_t reserved0; /**< 0x18 Reserved (no STRIDE) */
        xwu32_t next; /**< 0x1C DMA Next Control Block Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t read_last_not_set_error:1;
                        xwu32_t fifo_error:1;
                        xwu32_t read_error:1;
                        xwu32_t reserved0:1;
                        xwu32_t outstanding_writes:4;
                        xwu32_t dma_id:8;
                        xwu32_t dma_state:9;
                        xwu32_t version:3;
                        xwu32_t lite:1;
                        xwu32_t reserved1:3;
                } b;
        } debug; /**< 0x20 DMA Lite Debug */
};

/**
 * @brief DMA4 通道(11~14)的控制块
 */
struct soc_dma4_cb {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t inten:1;
                        xwu32_t tdmode:1;
                        xwu32_t reserved0:5;
                        xwu32_t wait_resp:1;
                        xwu32_t permap:5;
                        xwu32_t reserved1:1;
                        xwu32_t s_dreq:1;
                        xwu32_t d_dreq:1;
                        xwu32_t s_waits:8;
                        xwu32_t d_waits:8;
                } b;
        } ti; /**< 0x00 DMA4 Transfer Information */
        xwu32_t src; /**< 0x04 DMA4 Source Address [31:0] */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t addr_hi:8;
                        xwu32_t burst_length:4;
                        xwu32_t inc:1;
                        xwu32_t size:2;
                        xwu32_t ignore:1;
                        xwu32_t stride:16;
                } b;
        } srci; /**< 0x08 DMA4 Source Address [40:32] and Info */
        xwu32_t dst; /**< 0x0C DMA4 Destination Address [31:0] */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t addr_hi:8;
                        xwu32_t burst_length:4;
                        xwu32_t inc:1;
                        xwu32_t size:2;
                        xwu32_t ignore:1;
                        xwu32_t stride:16;
                } b;
        } dsti; /**< 0x10 DMA4 Destination Address [40:32] and Info */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t xlength:16;
                        xwu32_t ylength:14;
                        xwu32_t reserved0:2;
                } b;
        } size; /**< 0x14 DMA4 Transfer Length */
        xwu32_t next; /**< 0x18 DMA4 Next Control Block Address */
        xwu32_t reserved; /**< 0x1C */
};

/**
 * @brief DMA4 通道(11~14)的寄存器
 */
struct soc_dma4_regs {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t active:1;
                        xwu32_t end:1;
                        xwu32_t intr:1;
                        xwu32_t dreq:1;
                        xwu32_t reserved0:4;
                        xwu32_t error:1;
                        xwu32_t reserved1:7;
                        xwu32_t priority:4;
                        xwu32_t panic_priority:4;
                        xwu32_t reserved2:4;
                        xwu32_t wait_for_outstanding_writes:1;
                        xwu32_t disdebug:1;
                        xwu32_t abort:1;
                        xwu32_t halt:1;
                } b;
        } cs; /**< 0x00 DMA4 Control and Status */
        union {
                xwu32_t u32;
        } cb; /**< 0x04 DMA4 Control Block Address */
        xwu32_t reserved0; /**< 0x08 Reserved */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t reserved0:2;
                        xwu32_t read_last_not_set_error:1;
                        xwu32_t fifo_error:1;
                        xwu32_t read_error:1;
                        xwu32_t reserved1:4;
                        xwu32_t halt_on_error:1;
                        xwu32_t abort_on_error:1;
                        xwu32_t disable_clk_gate:1;
                        xwu32_t reserved2:2;
                        xwu32_t r_state:4;
                        xwu32_t w_state:4;
                        xwu32_t reserved3:1;
                        xwu32_t reset:1;
                        xwu32_t id:4;
                        xwu32_t version:4;
                } b;
        } debug; /**< 0x0C DMA4 Debug */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t inten:1;
                        xwu32_t tdmode:1;
                        xwu32_t reserved0:5;
                        xwu32_t wait_resp:1;
                        xwu32_t permap:5;
                        xwu32_t reserved1:1;
                        xwu32_t s_dreq:1;
                        xwu32_t d_dreq:1;
                        xwu32_t s_waits:8;
                        xwu32_t d_waits:8;
                } b;
        } ti; /**< 0x10 DMA4 Transfer Information */
        xwu32_t src; /**< 0x14 DMA4 Source Address [31:0] */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t addr_hi:8;
                        xwu32_t burst_length:4;
                        xwu32_t inc:1;
                        xwu32_t size:2;
                        xwu32_t ignore:1;
                        xwu32_t stride:16;
                } b;
        } srci; /**< 0x18 DMA4 Source Address [40:32] and Info */
        xwu32_t dst; /**< 0x1C DMA4 Destination Address [31:0] */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t addr_hi:8;
                        xwu32_t burst_length:4;
                        xwu32_t inc:1;
                        xwu32_t size:2;
                        xwu32_t ignore:1;
                        xwu32_t stride:16;
                } b;
        } dsti; /**< 0x20 DMA4 Destination Address [40:32] and Info */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t xlength:16;
                        xwu32_t ylength:14;
                        xwu32_t reserved0:2;
                } b;
        } size; /**< 0x24 DMA4 Transfer Length */
        xwu32_t next; /**< 0x28 DMA4 Next Control Block Address */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t outstanding_writes:9;
                        xwu32_t reserved0:7;
                        xwu32_t outstanding_reads:9;
                        xwu32_t reserved1:7;
                } b;
        } debug2; /**< 0x2C DMA4 More Debug */
};

/**
 * @brief DMA 全局寄存器
 */
struct soc_dma_global_regs {
        xwu32_t reserved0[(0xFE0 - 0xF00) / 4]; /**< 0xF00 ~ 0xFDC Reserved */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t int0:1;
                        xwu32_t int1:1;
                        xwu32_t int2:1;
                        xwu32_t int3:1;
                        xwu32_t int4:1;
                        xwu32_t int5:1;
                        xwu32_t int6:1;
                        xwu32_t int7:1;
                        xwu32_t int8:1;
                        xwu32_t int9:1;
                        xwu32_t int10:1;
                        xwu32_t int11:1;
                        xwu32_t int12:1;
                        xwu32_t int13:1;
                        xwu32_t int14:1;
                        xwu32_t int15:1;
                        xwu32_t reserved0:16;
                } b;
        } int_status; /**< 0xFE0 DMA Interrupt Status */
        xwu32_t reserved1[(0xFF0 - 0xFE4) / 4]; /**< 0xFE4 ~ 0xFEC Reserved */
        union {
                xwu32_t u32;
                struct {
                        xwu32_t en0:1;
                        xwu32_t en1:1;
                        xwu32_t en2:1;
                        xwu32_t en3:1;
                        xwu32_t en4:1;
                        xwu32_t en5:1;
                        xwu32_t en6:1;
                        xwu32_t en7:1;
                        xwu32_t en8:1;
                        xwu32_t en9:1;
                        xwu32_t en10:1;
                        xwu32_t en11:1;
                        xwu32_t en12:1;
                        xwu32_t en13:1;
                        xwu32_t en14:1;
                        xwu32_t reserved0:9;
                        xwu32_t page:4;
                        xwu32_t pagelite:4;
                } b;
        } enable; /**< 0xFF0 DMA Global Enable */
};

#define soc_dma0 (*((volatile struct soc_dma_regs *)SOC_DMA0_REGBASE))
#define soc_dma1 (*((volatile struct soc_dma_regs *)SOC_DMA1_REGBASE))
#define soc_dma2 (*((volatile struct soc_dma_regs *)SOC_DMA2_REGBASE))
#define soc_dma3 (*((volatile struct soc_dma_regs *)SOC_DMA3_REGBASE))
#define soc_dma4 (*((volatile struct soc_dma_regs *)SOC_DMA4_REGBASE))
#define soc_dma5 (*((volatile struct soc_dma_regs *)SOC_DMA5_REGBASE))
#define soc_dma6 (*((volatile struct soc_dma_regs *)SOC_DMA6_REGBASE))
#define soc_dma7 (*((volatile struct soc_dma_lite_regs *)SOC_DMA7_REGBASE))
#define soc_dma8 (*((volatile struct soc_dma_lite_regs *)SOC_DMA8_REGBASE))
#define soc_dma9 (*((volatile struct soc_dma_lite_regs *)SOC_DMA9_REGBASE))
#define soc_dma10 (*((volatile struct soc_dma_lite_regs *)SOC_DMA10_REGBASE))
#define soc_dma11 (*((volatile struct soc_dma4_regs *)SOC_DMA11_REGBASE))
#define soc_dma12 (*((volatile struct soc_dma4_regs *)SOC_DMA12_REGBASE))
#define soc_dma13 (*((volatile struct soc_dma4_regs *)SOC_DMA13_REGBASE))
#define soc_dma14 (*((volatile struct soc_dma4_regs *)SOC_DMA14_REGBASE))
#define soc_dma15 (*((volatile struct soc_dma_regs *)SOC_DMA15_REGBASE))
#define soc_dma (*((volatile struct soc_dma_global_regs *)(SOC_DMA_GLOBAL_REGBASE)))

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc_dma.h */
