/**
 * @file
 * @brief soc initialization
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc.h>
#include <soc_wdg.h>
#include <soc_osc.h>
#include <soc_ics.h>
#include <soc_sim.h>
#if defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/irq.h>
  #include <up_nvic.h>
  #include <xwos/up/scheduler.h>
#else
  #error "Can't find the configuration of XuanWuOS_CFG_CORE!"
#endif
#include <soc_init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (!defined(SOCCFG_RO_ISRTABLE)) || (1 != SOCCFG_RO_ISRTABLE)
static __xwos_init_code
void soc_relocate_isrtable(void);
#endif /* !SOCCFG_RO_ISRTABLE */

static __xwbsp_init_code
void soc_wdg_init(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__flscfg struct soc_flash_cfgs soc_flash_cfgs = {
        .backdoor_key = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        },
        .fprot = 0xFF,
        .fsec = 0xFE,
        .fopt = 0xFF,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
void soc_lowlevel_init(void)
{
        soc_wdg_init();
}

__xwbsp_init_code
void soc_init(void)
{
#if (!defined(SOCCFG_RO_ISRTABLE)) || (1 != SOCCFG_RO_ISRTABLE)
        soc_relocate_isrtable();
#endif /* !SOCCFG_RO_ISRTABLE */

#if defined(XuanWuOS_CFG_CORE__up)
        /* Init interrupt controller */
        xwer_t rc;
        rc = xwos_irqc_init("cortex-m.nvic",
                            (SOCCFG_IRQ_NUM + ARCHCFG_IRQ_NUM),
                            &soc_isr_table,
                            &soc_irq_data_table,
                            &armv6_nvic_cfg);
        XWOS_BUG_ON(rc < 0);

        /* Init scheduler */
        rc = xwos_scheduler_init();
        XWOS_BUG_ON(rc);
#endif
}

#if (!defined(SOCCFG_RO_ISRTABLE)) || (1 != SOCCFG_RO_ISRTABLE)
static __xwos_init_code
void soc_relocate_isrtable(void)
{
        xwsz_t cnt, i;
        xwu8_t * src;
        xwu8_t * dst;

        src = xwos_vctbl_lma_base;
        dst = xwos_vctbl_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)xwos_vctbl_vma_end - (xwsz_t)xwos_vctbl_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
}
#endif /* !SOCCFG_RO_ISRTABLE */

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
