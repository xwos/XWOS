/**
 * @file
 * @brief SOC描述层：Firmware信息
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

#ifndef __xwcd_soc_arm_v7m_m7_gd32a7_soc_firmware_h__
#define __xwcd_soc_arm_v7m_m7_gd32a7_soc_firmware_h__

#include <xwos/standard.h>
#include <xwcd/soc/arm/v7m/arch_firmware.h>

#define GD32A724_IVT_TAG                (0x5AA55AA5U)
#define GD32A724_IVT_BCW_CM7_0_EN       (1U << 0U) /* 启用Core0 */
#define GD32A724_IVT_BCW_CM7_1_EN       (1U << 1U) /* 启用Core1 */
#define GD32A724_IVT_LC_PROD            (0xDADADADAU)
#define GD32A724_IVT_LC_INFIELD         (0xBABABABAU)
#define GD32A724_IVT_LC_FA              (0x5A5A5A5AU)

/**
 * @brief GD32A724的IVT（固件头）
 * @note
 * + 《User Manual》 p77表1-4：IVT共256字节，sBAF会按照以下顺序（优先级）搜索IVT：
 *   + Pflash0 0x08000000
 *   + Pflash1 0x08200000
 *   + Dflash0 0x08800000
 */
struct soc_firmware_header {
        xwu32_t tag;            /**< 0x00 标签：0x5AA55AA5 */
        xwu32_t bcw;            /**< 0x04 引导配置字 */
        xwu32_t reserved0;      /**< 0x08 保留 */
        xwu32_t core0_bootaddr; /**< 0x0C core0的引导地址 */
        xwu32_t reserved1;      /**< 0x10 保留 */
        xwu32_t core1_bootaddr; /**< 0x14 core1的引导地址 */
        xwu32_t reserved2;      /**< 0x18 保留 */
        xwu32_t reserved3;      /**< 0x1C 保留 */
        xwu32_t reserved4;      /**< 0x20 保留 */
        xwu32_t lc;             /**< 0x24 生命周期配置字 */
        xwu32_t reserved5;      /**< 0x28 保留 */
        xwu32_t reserved6;      /**< 0x2C 保留 */
        xwu32_t reserved7[244]; /**< 0x30~0x3FF 保留 */
};

#endif /* xwcd/soc/arm/v7m/m7/gd32a7/soc_firmware.h */
