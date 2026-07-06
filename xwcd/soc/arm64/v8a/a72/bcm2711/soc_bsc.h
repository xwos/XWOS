/**
 * @file
 * @brief SOC描述层：BSC (I2C)
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

#ifndef __xwcd_soc_arm_v8a_a72_bcm2711_soc_bsc_h__
#define __xwcd_soc_arm_v8a_a72_bcm2711_soc_bsc_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc.h>

#define SOC_BSC_DIV_CDIV_100KHZ    0x05dcU
#define SOC_BSC_DEL_REDL_DEFAULT   0x0030U
#define SOC_BSC_DEL_FEDL_DEFAULT   0x0030U
#define SOC_BSC_CLKT_TOUT_DEFAULT  0x0040U

struct soc_bsc_regs {
        union {
                xwu32_t u32;
                struct {
                        xwu32_t read:1;
                        xwu32_t reserved0:3;
                        xwu32_t clear:2;
                        xwu32_t reserved1:1;
                        xwu32_t st:1;
                        xwu32_t intd:1;
                        xwu32_t intt:1;
                        xwu32_t intr:1;
                        xwu32_t reserved2:4;
                        xwu32_t i2cen:1;
                        xwu32_t reserved3:16;
                } b;
        } c; /**< 0x00 Control */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t ta:1;
                        xwu32_t done:1;
                        xwu32_t txw:1;
                        xwu32_t rxr:1;
                        xwu32_t txd:1;
                        xwu32_t rxd:1;
                        xwu32_t txe:1;
                        xwu32_t rxf:1;
                        xwu32_t err:1;
                        xwu32_t clkt:1;
                        xwu32_t reserved:22;
                } b;
        } s; /**< 0x04 Status */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t dlen:16;
                        xwu32_t reserved:16;
                } b;
        } dlen; /**< 0x08 Data Length */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t addr:7;
                        xwu32_t reserved:25;
                } b;
        } a; /**< 0x0C Slave Address */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t data:8;
                        xwu32_t reserved:24;
                } b;
        } fifo; /**< 0x10 Data FIFO */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t cdiv:16;
                        xwu32_t reserved:16;
                } b;
        } div; /**< 0x14 Clock Divider */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t redl:16;
                        xwu32_t fedl:16;
                } b;
        } del; /**< 0x18 Data Delay */

        union {
                xwu32_t u32;
                struct {
                        xwu32_t tout:16;
                        xwu32_t reserved:16;
                } b;
        } clkt; /**< 0x1C Clock Stretch Timeout */
};

#define soc_bsc0 (*((volatile struct soc_bsc_regs *)SOC_BSC0_REGBASE))
#define soc_bsc1 (*((volatile struct soc_bsc_regs *)SOC_BSC1_REGBASE))
#define soc_bsc3 (*((volatile struct soc_bsc_regs *)SOC_BSC3_REGBASE))
#define soc_bsc4 (*((volatile struct soc_bsc_regs *)SOC_BSC4_REGBASE))
#define soc_bsc5 (*((volatile struct soc_bsc_regs *)SOC_BSC5_REGBASE))
#define soc_bsc6 (*((volatile struct soc_bsc_regs *)SOC_BSC6_REGBASE))

#endif /* xwcd/soc/arm64/v8a/a72/bcm2711/soc_bsc.h */
