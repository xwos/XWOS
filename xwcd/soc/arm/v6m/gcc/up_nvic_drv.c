/**
 * @file
 * @brief 架构描述层：单核系统的中断控制器驱动
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
#include <armv6m_core.h>
#include <armv6m_nvic.h>
#include <up_nvic.h>
#include <up_nvic_drv.h>

extern struct xwup_irqc xwup_irqc;

__xwbsp_code
xwer_t cortexm_nvic_drv_init(void)
{
        struct xwup_irqc * irqc;
        const struct cortexm_nvic_cfg * irqccfg;

        irqc = &xwup_irqc;
        irqccfg = (const struct cortexm_nvic_cfg *)irqc->soc_cfg;

        cm_scs.scb.vtor.u32 = (xwu32_t)irqc->ivt;
        cm_set_basepri(irqccfg->basepri);

        return XWOK;
}

/******** ******** IRQ Drivers ******** ********/
__xwbsp_code
xwer_t cortexm_nvic_drv_request(xwirq_t irqn,
                                __xwcc_unused xwisr_f isrfunc,
                                __xwcc_unused void * data,
                                const struct soc_irq_cfg * cfg)
{
#if (!defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE))
        __xwos_ivt_qualifier struct soc_isr_table * ivt;
        __xwos_ivt_qualifier struct soc_isr_data_table * idvt;

        ivt = xwup_irqc.ivt;
        ivt->soc[irqn] = isrfunc;

        idvt = xwup_irqc.idvt;
        if ((NULL != idvt) && (NULL != data)) {
                idvt->soc[irqn] = data;
        }
#endif /* !SOCCFG_RO_ISRTABLE */
        if (cfg) {
                cortexm_nvic_drv_cfg(irqn, cfg);
        }
        return XWOK;
}

__xwbsp_code
xwer_t cortexm_nvic_drv_release(__xwcc_unused xwirq_t irqn)
{
#if (!defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE))
        __xwos_ivt_qualifier struct soc_isr_table * ivt;
        __xwos_ivt_qualifier struct soc_isr_data_table * idvt;

        ivt = xwup_irqc.ivt;
        ivt->soc[irqn] = arch_isr_noop;
#endif /* !SOCCFG_RO_ISRTABLE */
        return XWOK;
}

__xwbsp_code
xwer_t cortexm_nvic_drv_enable(__xwcc_unused xwirq_t irqn)
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

__xwbsp_code
xwer_t cortexm_nvic_drv_disable(__xwcc_unused xwirq_t irqn)
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

__xwbsp_code
xwer_t cortexm_nvic_drv_save(xwirq_t irqn, xwreg_t *flag)
{
        xwer_t rc = XWOK;

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

__xwbsp_code
xwer_t cortexm_nvic_drv_restore(xwirq_t irqn, xwreg_t flag)
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

__xwbsp_code
xwer_t cortexm_nvic_drv_pend(xwirq_t irqn)
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

__xwbsp_code
xwer_t cortexm_nvic_drv_clear(xwirq_t irqn)
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

__xwbsp_code
xwer_t cortexm_nvic_drv_tst(xwirq_t irqn, bool * pending)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                *pending = cm_nvic_get_irq_pending(irqn);
        } else if (ARCH_IRQ_PENDSV == irqn) {
                *pending = cm_nvic_get_pendsv_pending();
        } else if (ARCH_IRQ_SYSTICK == irqn) {
                *pending = cm_nvic_get_systick_pending();
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t cortexm_nvic_drv_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        if (irqn >= 0) {
                cm_nvic_set_irq_priority(irqn, cfg->irqcfg.priority);
        } else {
                cm_nvic_set_sysirq_priority(irqn, cfg->irqcfg.priority);
        }
        return XWOK;
}

__xwbsp_code
xwer_t cortexm_nvic_drv_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        if (irqn >= 0) {
                cfgbuf->irqcfg.priority = cm_nvic_get_irq_priority(irqn);
        } else {
                cfgbuf->irqcfg.priority = cm_nvic_get_sysirq_priority(irqn);
        }
        return XWOK;
}

__xwbsp_code
xwer_t cortexm_nvic_drv_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        __xwos_ivt_qualifier struct soc_isr_data_table * idvt;

        idvt = xwup_irqc.idvt;
        databuf->data = idvt->soc[irqn];
        return XWOK;
}
