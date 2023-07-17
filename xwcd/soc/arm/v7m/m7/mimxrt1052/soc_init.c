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
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <armv7m_isa.h>
#include <arch_nvic.h>
#include <soc.h>
#include <soc_init.h>

static __xwos_init_code
void soc_relocate_isrtable(void);

static __xwbsp_init_code
void soc_relocate(void);

extern xwu8_t armv7m_ivt_lma_base[];
extern xwu8_t armv7m_ivt_vma_base[];
extern xwu8_t armv7m_ivt_vma_end[];

extern xwu8_t itcm_lma_base[];
extern xwu8_t itcm_vma_base[];
extern xwu8_t itcm_vma_end[];
extern xwu8_t itcm_bss_vma_base[];
extern xwu8_t itcm_bss_vma_end[];

extern xwu8_t ocram_lma_base[];
extern xwu8_t ocram_vma_base[];
extern xwu8_t ocram_vma_end[];
extern xwu8_t ocram_bss_vma_base[];
extern xwu8_t ocram_bss_vma_end[];

extern xwu8_t ncache_lma_base[];
extern xwu8_t ncache_vma_base[];
extern xwu8_t ncache_vma_end[];
extern xwu8_t ncache_bss_vma_base[];
extern xwu8_t ncache_bss_vma_end[];

__xwbsp_init_code
void soc_lowlevel_init(void)
{
        /* cm_scs.scb.cacr.bit.siwt = 1; */
        /* cm_scs.scb.cacr.bit.forcewt = 1; */
}

__xwbsp_init_code
void soc_init(void)
{
        xwer_t rc;

        soc_relocate_isrtable();
        soc_relocate();
        rc = xwosplcb_skd_init_lc();
        XWOS_BUG_ON(rc);
}

static __xwos_init_code
void soc_relocate_isrtable(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = armv7m_ivt_lma_base;
        dst = armv7m_ivt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)armv7m_ivt_vma_end - (xwsz_t)armv7m_ivt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
}

static __xwbsp_init_code
void soc_relocate(void)
{
        xwsz_t count, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = itcm_lma_base;
        dst = itcm_vma_base;
        if (dst != src) {
                count = (xwsz_t)itcm_vma_end - (xwsz_t)itcm_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = itcm_bss_vma_base;
        count = (xwsz_t)itcm_bss_vma_end - (xwsz_t)itcm_bss_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }

        src = ocram_lma_base;
        dst = ocram_vma_base;
        if (dst != src) {
                count = (xwsz_t)ocram_vma_end - (xwsz_t)ocram_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = ocram_bss_vma_base;
        count = (xwsz_t)ocram_bss_vma_end - (xwsz_t)ocram_bss_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }

        src = ncache_lma_base;
        dst = ncache_vma_base;
        if (dst != src) {
                count = (xwsz_t)ncache_vma_end - (xwsz_t)ncache_vma_base;
                for (i = 0; i < count; i++) {
                        *dst = *src;
                        dst++;
                        src++;
                }
        }

        dst = ncache_bss_vma_base;
        count = (xwsz_t)ncache_bss_vma_end - (xwsz_t)ncache_bss_vma_base;
        for (i = 0; i < count; i++) {
                *dst = 0;
                dst++;
        }
}
