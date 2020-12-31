/**
 * @file
 * @brief 架构描述层：对称多核系统的中断控制器配驱动
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
#include <string.h>
#include <xwos/ospl/irq.h>
#include <arch_irq.h>
#include <armv7m_core.h>
#include <armv7m_nvic.h>
#include <mp_nvic.h>
#include <mp_nvic_drv.h>

static __xwbsp_code
xwer_t cortexm_nvic_drv_probe(struct xwmp_irqc * irqc);

static __xwbsp_code
xwer_t cortexm_nvic_drv_remove(struct xwmp_irqc * irqc);

static __xwbsp_code
xwer_t cortexm_nvic_drv_request(__xwcc_unused struct xwmp_irqc * irqc,
                                __xwcc_unused xwirq_t irqn,
                                __xwcc_unused xwisr_f isrfunc,
                                __xwcc_unused xwsq_t flag,
                                __xwcc_unused void * data);

static __xwbsp_code
xwer_t cortexm_nvic_drv_release(struct xwmp_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t cortexm_nvic_drv_enable(struct xwmp_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t cortexm_nvic_drv_disable(struct xwmp_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t cortexm_nvic_drv_save(struct xwmp_irqc * irqc, xwirq_t irqn, xwreg_t * flag);

static __xwbsp_code
xwer_t cortexm_nvic_drv_restore(struct xwmp_irqc * irqc, xwirq_t irqn, xwreg_t flag);

static __xwbsp_code
xwer_t cortexm_nvic_drv_pend(struct xwmp_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t cortexm_nvic_drv_clear(struct xwmp_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t cortexm_nvic_drv_cfg(struct xwmp_irqc * irqc, xwirq_t irqn,
                            const struct soc_irq_cfg * cfg);

static __xwbsp_code
xwer_t cortexm_nvic_drv_get_cfg(struct xwmp_irqc * irqc, xwirq_t irqn,
                                struct soc_irq_cfg * cfgbuf);

static __xwbsp_code
xwer_t cortexm_nvic_drv_get_data(__xwcc_unused struct xwmp_irqc * irqc,
                                 xwirq_t irqn, struct soc_irq_data * cfgbuf);

__xwbsp_rodata const struct xwmp_irqc_driver cortexm_nvic_drv = {
        .probe = cortexm_nvic_drv_probe,
        .remove = cortexm_nvic_drv_remove,
        .request = cortexm_nvic_drv_request,
        .release = cortexm_nvic_drv_release,
        .enable = cortexm_nvic_drv_enable,
        .disable = cortexm_nvic_drv_disable,
        .save = cortexm_nvic_drv_save,
        .restore = cortexm_nvic_drv_restore,
        .pend = cortexm_nvic_drv_pend,
        .clear = cortexm_nvic_drv_clear,
        .cfg = cortexm_nvic_drv_cfg,
        .get_cfg = cortexm_nvic_drv_get_cfg,
        .get_data = cortexm_nvic_drv_get_data,
};

static __xwbsp_code
xwer_t cortexm_nvic_drv_probe(struct xwmp_irqc * irqc)
{
        const struct cortexm_nvic_cfg * irqccfg;
        irqccfg = (const struct cortexm_nvic_cfg *)irqc->soc_cfg;

        cm_scs.scb.vtor.u32 = (xwu32_t)irqc->isr_table;
        cm_set_basepri(irqccfg->basepri);
        cm_nvic_set_prioritygrouping(irqccfg->subprio);

        return XWOK;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_remove(struct xwmp_irqc * irqc)
{
        (void)irqc;
        return XWOK;
}

/******** ******** irq operations ******** ********/
static __xwbsp_code
xwer_t cortexm_nvic_drv_request(__xwcc_unused struct xwmp_irqc * irqc,
                                __xwcc_unused xwirq_t irqn,
                                __xwcc_unused xwisr_f isrfunc,
                                __xwcc_unused xwsq_t flag,
                                __xwcc_unused void * data)
{
#if (!defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE))
        struct soc_isr_table * isr_table;
        struct soc_isr_data_table * irq_data_table;

        isr_table = irqc->isr_table;
        isr_table->soc[irqn] = isrfunc;

        irq_data_table = irqc->irq_data_table;
        if ((NULL != irq_data_table) && (NULL != data)) {
                irq_data_table->soc[irqn] = data;
        }
#endif /* !SOCCFG_RO_ISRTABLE */
        return XWOK;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_release(__xwcc_unused struct xwmp_irqc * irqc,
                                __xwcc_unused xwirq_t irqn)
{
#if (!defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE))
        struct soc_isr_table * isr_table;

        isr_table = irqc->isr_table;
        isr_table->soc[irqn] = arch_isr_noop;
#endif /* #if (!defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE)) */
        return XWOK;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_enable(__xwcc_unused struct xwmp_irqc * irqc,
                               __xwcc_unused xwirq_t irqn)
{
        if (irqn >= 0) {
                cm_nvic_enable_irq(irqn);
        } else if (ARCH_IRQ_MMFAULT == irqn) {
                cm_nvic_enable_memfault();
        } else if (ARCH_IRQ_BUSFAULT == irqn) {
                cm_nvic_enable_busfault();
        } else if (ARCH_IRQ_USGFAULT == irqn) {
                cm_nvic_enable_usgfault();
        }/* else {} */
        return XWOK;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_disable(__xwcc_unused struct xwmp_irqc * irqc,
                                __xwcc_unused xwirq_t irqn)
{
        if (irqn >= 0) {
                cm_nvic_disable_irq(irqn);
        } else if (ARCH_IRQ_MMFAULT == irqn) {
                cm_nvic_disable_memfault();
        } else if (ARCH_IRQ_BUSFAULT == irqn) {
                cm_nvic_disable_busfault();
        } else if (ARCH_IRQ_USGFAULT == irqn) {
                cm_nvic_disable_usgfault();
        }/* else {} */
        return XWOK;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_save(__xwcc_unused struct xwmp_irqc * irqc,
                             xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                cm_nvic_save_irq(irqn, flag);
                cm_nvic_disable_irq(irqn);
        } else if (ARCH_IRQ_MMFAULT == irqn) {
                cm_nvic_save_memfault(flag);
                cm_nvic_disable_memfault();
        } else if (ARCH_IRQ_BUSFAULT == irqn) {
                cm_nvic_save_busfault(flag);
                cm_nvic_disable_busfault();
        } else if (ARCH_IRQ_USGFAULT == irqn) {
                cm_nvic_save_usgfault(flag);
                cm_nvic_disable_usgfault();
        } else {
                rc = -EPERM;
        }
        return rc;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_restore(__xwcc_unused struct xwmp_irqc * irqc,
                                xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                cm_nvic_restore_irq(irqn, flag);
        } else if (ARCH_IRQ_MMFAULT == irqn) {
                cm_nvic_restore_memfault(flag);
        } else if (ARCH_IRQ_BUSFAULT == irqn) {
                cm_nvic_restore_busfault(flag);
        } else if (ARCH_IRQ_USGFAULT == irqn) {
                cm_nvic_restore_usgfault(flag);
        } else {
                rc = -EPERM;
        }
        return rc;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_pend(__xwcc_unused struct xwmp_irqc * irqc, xwirq_t irqn)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                cm_nvic_software_trigger_irq(irqn);
        } else if (ARCH_IRQ_NMI == irqn) {
                cm_nvic_pend_nmi();
        } else if (ARCH_IRQ_PENDSV == irqn) {
                cm_nvic_pend_pendsv();
        } else if (ARCH_IRQ_SYSTICK == irqn) {
                cm_nvic_pend_systick();
        } else {
                rc = -EPERM;
        }
        return rc;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_clear(__xwcc_unused struct xwmp_irqc * irqc, xwirq_t irqn)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                cm_nvic_clear_irq_pending(irqn);
        } else if (ARCH_IRQ_PENDSV == irqn) {
                cm_nvic_clear_systick_pending();
        } else if (ARCH_IRQ_SYSTICK == irqn) {
                cm_nvic_clear_systick_pending();
        } else {
                rc = -EPERM;
        }
        return rc;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_cfg(__xwcc_unused struct xwmp_irqc * irqc,
                            xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        if (irqn >= 0) {
                cm_nvic_set_irq_priority(irqn, cfg->irqcfg.priority);
        } else {
                cm_nvic_set_sysirq_priority(irqn, cfg->irqcfg.priority);
        }
        return XWOK;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_get_cfg(__xwcc_unused struct xwmp_irqc * irqc,
                                xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        if (irqn >= 0) {
                cfgbuf->irqcfg.priority = cm_nvic_get_irq_priority(irqn);
        } else {
                cfgbuf->irqcfg.priority = cm_nvic_get_sysirq_priority(irqn);
        }
        return XWOK;
}

static __xwbsp_code
xwer_t cortexm_nvic_drv_get_data(__xwcc_unused struct xwmp_irqc * irqc,
                                 xwirq_t irqn, struct soc_irq_data * databuf)
{
        __soc_isr_table_qualifier struct soc_isr_data_table * irq_data_table;

        irq_data_table = irqc->irq_data_table;
        databuf->data = irq_data_table->soc[irqn];
        return XWOK;
}