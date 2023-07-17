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
#include <soc.h>
#include <soc_wdg.h>
#include <soc_osc.h>
#include <soc_ics.h>
#include <soc_sim.h>
#include <soc_init.h>

static __xwbsp_init_code
void soc_wdg_init(void);

static __xwos_init_code
void soc_relocate_isrtable(void);

__flscfg struct soc_flash_cfgs soc_flash_cfgs = {
        .backdoor_key = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        },
        .fprot = 0xFF,
        .fsec = 0xFE,
        .fopt = 0xFF,
};

__xwbsp_init_code
void soc_lowlevel_init(void)
{
        soc_wdg_init();
}

__xwbsp_init_code
void soc_init(void)
{
        xwer_t rc;

        soc_relocate_isrtable();
        rc = xwosplcb_skd_init_lc();
        XWOS_BUG_ON(rc);
}

extern xwu8_t armv6m_ivt_lma_base[];
extern xwu8_t armv6m_ivt_vma_base[];
extern xwu8_t armv6m_ivt_vma_end[];

static __xwos_init_code
void soc_relocate_isrtable(void)
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
        cm_scs.scb.vtor.u32 = (xwu32_t)armv6m_ivt_vma_base;
}

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
