/**
 * @file
 * @brief SOC描述层：初始化
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

#include <xwos/standard.h>
#include <xwos/ospl/skd.h>
#include <soc_init.h>

struct s32k3_image_vector_table {
        xwu32_t boot_marker;
        xwu32_t boot_cfg_word;
        xwu32_t reserved0;
        xwu32_t m7_0_start_address;
        xwu32_t reserved1;
        xwu32_t m7_1_start_address;
        xwu32_t reserved2;
        xwu32_t m7_2_start_address;
        xwu32_t reserved3;
        xwu32_t lifecycle_configuration_pointer;
        xwu32_t reserved4[216];
};

#define SBAF_BOOT_MARKER        (0x5AA55AA5)
#define CM7_0_ENABLE_SHIFT      (0)
#define CM7_1_ENABLE_SHIFT      (1)
#define CM7_0_ENABLE            (1)
#define CM7_1_ENABLE            (0)

extern xwu8_t armv7m_ivt_lma_base[];

__xwcc_section(".boot_header")
const struct s32k3_image_vector_table soc_image_vector_table = {
        .boot_marker = SBAF_BOOT_MARKER,
        .boot_cfg_word = (CM7_0_ENABLE << CM7_0_ENABLE_SHIFT) | (CM7_1_ENABLE << CM7_1_ENABLE_SHIFT),
        .m7_0_start_address = (xwu32_t)armv7m_ivt_lma_base,
        .m7_1_start_address = (xwu32_t)armv7m_ivt_lma_base,
        .m7_2_start_address = (xwu32_t)armv7m_ivt_lma_base,
        .lifecycle_configuration_pointer = 0,
};

#define SOC_SWT0_CR (*((__xw_io xwreg_t *)0x40270000))
#define SOC_SWT0_SR (*((__xw_io xwreg_t *)0x40270010))
#define SOC_SWT0_SR_UNLOCK_KEY0 0xC520
#define SOC_SWT0_SR_UNLOCK_KEY1 0xD928

__xwbsp_init_code
void soc_disable_swt0(void)
{
        SOC_SWT0_SR = SOC_SWT0_SR_UNLOCK_KEY0;
        SOC_SWT0_SR = SOC_SWT0_SR_UNLOCK_KEY1;
        SOC_SWT0_CR = 0xFF000040;
}

#define SOC_SWT1_CR (*((__xw_io xwreg_t *)0x4046C000))
#define SOC_SWT1_SR (*((__xw_io xwreg_t *)0x4046C010))
#define SOC_SWT1_SR_UNLOCK_KEY0 0xC520
#define SOC_SWT1_SR_UNLOCK_KEY1 0xD928

__xwbsp_init_code
void soc_disable_swt1(void)
{
        SOC_SWT1_SR = SOC_SWT1_SR_UNLOCK_KEY0;
        SOC_SWT1_SR = SOC_SWT1_SR_UNLOCK_KEY1;
        SOC_SWT1_CR = 0xFF000040;
}

#define SOC_MSCM_CPXNUM (*((__xw_io xwreg_t *)0x40260004U))

__xwbsp_init_code
void soc_init(void)
{
        xwid_t cpuid;

        cpuid = SOC_MSCM_CPXNUM & 0x3U;
        switch (cpuid) {
        case 0:
                soc_disable_swt0();
                break;
        case 1:
                soc_disable_swt1();
                break;
        default:
                XWOS_BUG();
        }
}
