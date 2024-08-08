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

#include <xwcd/soc/arm/v6m/m0p/kea/soc_init.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm/v6m/m0p/kea/soc.h>
#include <xwcd/soc/arm/v6m/m0p/kea/soc_wdg.h>
#include <xwcd/soc/arm/v6m/m0p/kea/soc_osc.h>
#include <xwcd/soc/arm/v6m/m0p/kea/soc_ics.h>
#include <xwcd/soc/arm/v6m/m0p/kea/soc_sim.h>

struct soc_flash_cfgs {
        xwu8_t backdoor_key[8]; /**< Backdoor Comparison Key 0 ~ 7, offset: 0x0 */
        xwu8_t reserved0[4];
        xwu8_t reserved1[1];
        xwu8_t fprot; /**< Non-volatile P-Flash Protection Register, offset: 0xD */
        xwu8_t fsec; /**< Non-volatile Flash Security Register, offset: 0xE */
        xwu8_t fopt; /**< Non-volatile Flash Option Register, offset: 0xF */
};

__flscfg struct soc_flash_cfgs soc_flash_cfgs = {
        .backdoor_key = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        },
        .fprot = 0xFF,
        .fsec = 0xFE,
        .fopt = 0xFF,
};

extern xwu8_t armv6m_ivt_lma_base[];
extern xwu8_t armv6m_ivt_vma_base[];
extern xwu8_t armv6m_ivt_vma_end[];

/**
 * @brief 重定向数据区到内存
 */
__xwos_init_code
void soc_relocate_ivt(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = armv6m_ivt_lma_base;
        dst = armv6m_ivt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)armv6m_ivt_vma_end - (xwsz_t)armv6m_ivt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
        armv6m_scs.scb.vtor.u32 = (xwu32_t)armv6m_ivt_vma_base;
}

extern xwu8_t data_lma_base[];
extern xwu8_t data_vma_base[];
extern xwu8_t data_vma_end[];

extern xwu8_t bss_vma_base[];
extern xwu8_t bss_vma_end[];

/**
 * @brief 重定向数据区到内存
 */
__xwbsp_init_code
void soc_relocate_data(void)
{
        xwsz_t count, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = data_lma_base;
        dst = data_vma_base;
        if (dst != src) {
                count = (xwsz_t)data_vma_end - (xwsz_t)data_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = bss_vma_base;
        count = (xwsz_t)bss_vma_end - (xwsz_t)bss_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }
}

/**
 * @brief 初始化SOC的看门狗
 */
static __xwbsp_init_code
void soc_wdg_init(void)
{
        xwu8_t cs1;
        xwu8_t cs2;
        xwu16_t toval;
        xwu16_t win;

        cs1 = soc_wdg.cs1.byte;
        cs1 &= ~SOC_WDG_CS1_EN_MASK;
        cs1 |= SOC_WDG_CS1_UPDATE_MASK;
        cs2 = soc_wdg.cs2.byte;
        toval = soc_wdg.toval.dbyte;
        win = soc_wdg.win.dbyte;
        SOC_WDG_UNLOCK();
        soc_wdg.cs2.byte = cs2;
        soc_wdg.toval.dbyte = toval;
        soc_wdg.win.dbyte = win;
        soc_wdg.cs1.byte = cs1;
}

/**
 * @brief SOC的初始化
 */
__xwbsp_init_code
void soc_init(void)
{
        soc_wdg_init();
}
