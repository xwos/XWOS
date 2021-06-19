/**
 * @file
 * @brief SOC描述层：初始化
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <soc.h>
#include <soc_init.h>

#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
static __xwos_init_code
void soc_relocate_ivt(void);
#endif /* !SOCCFG_RO_IVT */

extern xwu8_t evt_lma_base[];
extern xwu8_t evt_vma_base[];
extern xwu8_t evt_vma_end[];

extern xwu8_t eclic_ivt_lma_base[];
extern xwu8_t eclic_ivt_vma_base[];
extern xwu8_t eclic_ivt_vma_end[];

__xwbsp_init_code
void soc_lowlevel_init(void)
{
        SysTimer_Stop();
}

__xwbsp_init_code
void soc_init(void)
{
#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
        soc_relocate_ivt();
#endif /* !SOCCFG_RO_IVT */

        soc_irqc_init();
        xwosplcb_skd_init_lc();
}

#if (!defined(SOCCFG_RO_IVT)) || (1 != SOCCFG_RO_IVT)
static __xwos_init_code
void soc_relocate_ivt(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = eclic_ivt_lma_base;
        dst = eclic_ivt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)eclic_ivt_vma_end - (xwsz_t)eclic_ivt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }

        src = evt_lma_base;
        dst = evt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)evt_vma_end - (xwsz_t)evt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
}
#endif /* !SOCCFG_RO_IVT */
