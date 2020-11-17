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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#if defined(XuanWuOS_CFG_CORE__mp)
  #include <mp_nvic.h>
  #include <xwos/mp/irq.h>
  #include <xwos/mp/skd.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/irq.h>
  #include <xwos/up/skd.h>
  #include <up_nvic.h>
#else
  #error "Can't find the configuration of XuanWuOS_CFG_CORE!"
#endif
#include <armv7m_core.h>
#include <soc.h>
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
}

__xwbsp_init_code
void soc_init(void)
{
        xwer_t rc;

#if (!defined(SOCCFG_RO_ISRTABLE)) || (1 != SOCCFG_RO_ISRTABLE)
        soc_relocate_isrtable();
#endif /* !SOCCFG_RO_ISRTABLE */

#if defined(XuanWuOS_CFG_CORE__mp)
        xwid_t id = xwmp_skd_get_id();

        /* Interrupt controller of CPU */
        xwmp_irqc_construct(&cortexm_nvic[id]);
        rc = xwmp_irqc_register(&cortexm_nvic[id], id, NULL);
        XWOS_BUG_ON(rc < 0);

        /* Init scheduler of local CPU */
        rc = xwmp_skd_init_lc();
        XWOS_BUG_ON(rc);
#elif defined(XuanWuOS_CFG_CORE__up)
        /* Init interrupt controller */
        rc = xwup_irqc_init("cortex-m.nvic",
                            (SOCCFG_IRQ_NUM + ARCHCFG_IRQ_NUM),
                            &soc_isr_table,
                            &soc_isr_data_table,
                            &armv7_nvic_cfg);
        XWOS_BUG_ON(rc < 0);

        /* Init scheduler */
        rc = xwup_skd_init_lc();
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
