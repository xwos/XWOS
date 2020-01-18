/**
 * @file
 * @brief S32K14x initializations
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
#if defined(XuanWuOS_CFG_CORE__smp)
  #include <xwos/smp/irq.h>
  #include <xwos/smp/cpu.h>
  #include <xwos/smp/scheduler.h>
  #include <xwos/smp/pm.h>
  #include <soc_xwpmdm.h>
  #include <smp_nvic.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/irq.h>
  #include <up_nvic.h>
  #include <xwos/up/scheduler.h>
#else
  #error "Can't find the configuration of XuanWuOS_CFG_CORE!"
#endif
#include <soc_clk.h>
#include <soc_wdg.h>
#include <soc_init.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (!defined(SOCCFG_RO_ISRTABLE)) || (1 != SOCCFG_RO_ISRTABLE)
static __xwos_init_code
void soc_relocate_isrtable(void);
#endif /* !SOCCFG_RO_ISRTABLE */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwu8_t xwos_vctbl_lma_base[];
extern xwu8_t xwos_vctbl_vma_base[];
extern xwu8_t xwos_vctbl_vma_end[];

__flscfg struct soc_flash_cfgs soc_flash_cfgs = {
        .backdoor_key = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        },
        .fprot = {
                0xFF, 0xFF, 0xFF, 0xFF,
        },
        .fsec = 0xFE,
        .fopt = 0x7F,
        .feprot = 0xFF,
        .fdprot = 0xFF,
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
void soc_lowlevel_init(void)
{
        soc_clk_lpo_init();
        soc_wdg_disable();
        soc_clk_xwsc_init_8mhz();
        soc_clk_spll_init_160mhz();
        soc_clk_nrun_init_80mhz();
}

__xwbsp_init_code
void soc_init(void)
{
        xwer_t rc;

#if (!defined(SOCCFG_RO_ISRTABLE)) || (1 != SOCCFG_RO_ISRTABLE)
        soc_relocate_isrtable();
#endif /* !SOCCFG_RO_ISRTABLE */

#if defined(XuanWuOS_CFG_CORE__smp)
        /* PM domain */
        xwos_pmdm_init(&soc_xwpm_domain);

        /* Interrupt controller */
        xwos_irqc_construct(&cortexm_nvic[0]);
        rc = xwos_irqc_register(&cortexm_nvic[0], xwos_cpu_get_id(), NULL);
        XWOS_BUG_ON(rc < 0);

        /* Init scheduler of CPU */
        rc = xwos_scheduler_init_lc(&soc_xwpm_domain);
        XWOS_BUG_ON(rc);
#elif defined(XuanWuOS_CFG_CORE__up)
        /* Init interrupt controller */
        rc = xwos_irqc_init("cortex-m.nvic",
                            (SOCCFG_IRQ_NUM + ARCHCFG_IRQ_NUM),
                            &armv7_nvic_cfg);
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
