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

#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
static __xwos_init_code
void soc_relocate_isrtable(void);
#endif

extern xwu8_t xwos_ivt_lma_base[];
extern xwu8_t xwos_ivt_vma_base[];
extern xwu8_t xwos_ivt_vma_end[];

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

#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
        soc_relocate_isrtable();
#endif

        /* Init scheduler of local CPU */
        rc = xwosplcb_skd_init_lc();
        XWOS_BUG_ON(rc);
}

#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
static __xwos_init_code
void soc_relocate_isrtable(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = xwos_ivt_lma_base;
        dst = xwos_ivt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)xwos_ivt_vma_end - (xwsz_t)xwos_ivt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
}
#endif
