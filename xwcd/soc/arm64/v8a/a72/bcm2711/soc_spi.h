/**
 * @file
 * @brief SOC描述层：SPI
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_spi_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_spi_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc.h>

#define SOC_SPI_CS_CLEAR_NO     0U
#define SOC_SPI_CS_CLEAR_TX     1U
#define SOC_SPI_CS_CLEAR_RX     2U

#define SOC_SPI_CS_CHIP_SEL_0   0U
#define SOC_SPI_CS_CHIP_SEL_1   1U
#define SOC_SPI_CS_CHIP_SEL_2   2U

struct soc_spi_regs {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t chip_sel:2;
                        xwu32_t cpha:1;
                        xwu32_t cpol:1;
                        xwu32_t clear:2;
                        xwu32_t cspol:1;
                        xwu32_t ta:1;
                        xwu32_t dmaen:1;
                        xwu32_t intd:1;
                        xwu32_t intr:1;
                        xwu32_t adcs:1;
                        xwu32_t ren:1;
                        xwu32_t len:1;
                        xwu32_t lmono:1;
                        xwu32_t te_en:1;
                        xwu32_t done:1;
                        xwu32_t rxd:1;
                        xwu32_t txd:1;
                        xwu32_t rxr:1;
                        xwu32_t rxf:1;
                        xwu32_t cspol0:1;
                        xwu32_t cspol1:1;
                        xwu32_t cspol2:1;
                        xwu32_t dma_len:1;
                        xwu32_t len_long:1;
                        xwu32_t reserved:6;
                } b;
        } cs; /**< 0x00 Control and Status */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t data:32;
                } b;
        } fifo; /**< 0x04 TX and RX FIFOs */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t cdiv:16;
                        xwu32_t reserved:16;
                } b;
        } clk; /**< 0x08 Clock Divider */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t len:16;
                        xwu32_t reserved:16;
                } b;
        } dlen; /**< 0x0C Data Length */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t toh:4;
                        xwu32_t reserved:28;
                } b;
        } ltoh; /**< 0x10 LoSSI mode TOH */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t tdreq:8;
                        xwu32_t tpanic:8;
                        xwu32_t rdreq:8;
                        xwu32_t rpanic:8;
                } b;
        } dc; /**< 0x14 DMA DREQ Controls */
};

#define soc_spi0 (*((volatile struct soc_spi_regs *)SOC_SPI0_REGBASE))
#define soc_spi3 (*((volatile struct soc_spi_regs *)SOC_SPI3_REGBASE))
#define soc_spi4 (*((volatile struct soc_spi_regs *)SOC_SPI4_REGBASE))
#define soc_spi5 (*((volatile struct soc_spi_regs *)SOC_SPI5_REGBASE))
#define soc_spi6 (*((volatile struct soc_spi_regs *)SOC_SPI6_REGBASE))

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc_spi.h */
