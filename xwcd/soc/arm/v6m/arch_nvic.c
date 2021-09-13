/**
 * @file
 * @brief 架构描述层：NVIC中断控制器
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
#include <arch_nvic.h>

__xwbsp_code
xwer_t arch_nvic_init(void)
{
        cm_scs.scb.vtor.u32 = (xwu32_t)&xwospl_ivt;
        return XWOK;
}

/******** ******** IRQ Drivers ******** ********/
__xwbsp_code
xwer_t arch_nvic_irq_get_id(xwirq_t * irqnbuf)
{
        xwirq_t curr;
        xwer_t rc;

        curr = cm_scs.scb.icsr.bit.vect_active;
        if (0 == curr) {
                rc = -ENOTINISR;
        } else {
                curr -= 16;
                rc = XWOK;
        }
        if (!is_err_or_null(irqnbuf)) {
                *irqnbuf = curr;
        }
        return rc;
}

__xwbsp_code
xwer_t arch_nvic_irq_request(xwirq_t irqn,
                             __xwcc_unused xwisr_f isrfunc,
                             __xwcc_unused void * data,
                             const struct soc_irq_cfg * cfg)
{
#if (!defined(SOCCFG_RO_IVT) || (1 != SOCCFG_RO_IVT))
        __xwos_ivt_qualifier struct soc_ivt * ivt;
        __xwos_ivt_qualifier struct soc_idvt * idvt;

        ivt = &xwospl_ivt;
        ivt->irq[irqn] = isrfunc;

        idvt = &xwospl_idvt;
        if ((NULL != idvt) && (NULL != data)) {
                idvt->irq[irqn] = data;
        }
#endif /* !SOCCFG_RO_IVT */
        if (cfg) {
                arch_nvic_irq_cfg(irqn, cfg);
        }
        return XWOK;
}

__xwbsp_code
xwer_t arch_nvic_irq_release(__xwcc_unused xwirq_t irqn)
{
#if (!defined(SOCCFG_RO_IVT) || (1 != SOCCFG_RO_IVT))
        __xwos_ivt_qualifier struct soc_ivt * ivt;
        __xwos_ivt_qualifier struct soc_idvt * idvt;

        ivt = &xwospl_ivt;
        ivt->irq[irqn] = arch_isr_noop;

        idvt = &xwospl_idvt;
        if (NULL != idvt) {
                idvt->irq[irqn] = NULL;
        }
#endif /* !SOCCFG_RO_IVT */
        return XWOK;
}

__xwbsp_code
xwer_t arch_nvic_irq_enable(__xwcc_unused xwirq_t irqn)
{
        if (irqn >= 0) {
                cm_nvic_enable_irq(irqn);
        } else if (SOC_EXC_MMFAULT == irqn) {
                cm_nvic_enable_memfault();
        } else if (SOC_EXC_BUSFAULT == irqn) {
                cm_nvic_enable_busfault();
        } else if (SOC_EXC_USGFAULT == irqn) {
                cm_nvic_enable_usgfault();
        }/* else {} */
        return XWOK;
}

__xwbsp_code
xwer_t arch_nvic_irq_disable(__xwcc_unused xwirq_t irqn)
{
        if (irqn >= 0) {
                cm_nvic_disable_irq(irqn);
        } else if (SOC_EXC_MMFAULT == irqn) {
                cm_nvic_disable_memfault();
        } else if (SOC_EXC_BUSFAULT == irqn) {
                cm_nvic_disable_busfault();
        } else if (SOC_EXC_USGFAULT == irqn) {
                cm_nvic_disable_usgfault();
        }/* else {} */
        return XWOK;
}

__xwbsp_code
xwer_t arch_nvic_irq_save(xwirq_t irqn, xwreg_t *flag)
{
        xwer_t rc = XWOK;

        if (irqn >= 0) {
                cm_nvic_save_irq(irqn, flag);
                cm_nvic_disable_irq(irqn);
        } else if (SOC_EXC_MMFAULT == irqn) {
                cm_nvic_save_memfault(flag);
                cm_nvic_disable_memfault();
        } else if (SOC_EXC_BUSFAULT == irqn) {
                cm_nvic_save_busfault(flag);
                cm_nvic_disable_busfault();
        } else if (SOC_EXC_USGFAULT == irqn) {
                cm_nvic_save_usgfault(flag);
                cm_nvic_disable_usgfault();
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t arch_nvic_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                cm_nvic_restore_irq(irqn, flag);
        } else if (SOC_EXC_MMFAULT == irqn) {
                cm_nvic_restore_memfault(flag);
        } else if (SOC_EXC_BUSFAULT == irqn) {
                cm_nvic_restore_busfault(flag);
        } else if (SOC_EXC_USGFAULT == irqn) {
                cm_nvic_restore_usgfault(flag);
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t arch_nvic_irq_pend(xwirq_t irqn)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                cm_nvic_software_trigger_irq(irqn);
        } else if (SOC_EXC_NMI == irqn) {
                cm_nvic_pend_nmi();
        } else if (SOC_EXC_PENDSV == irqn) {
                cm_nvic_pend_pendsv();
        } else if (SOC_EXC_SYSTICK == irqn) {
                cm_nvic_pend_systick();
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t arch_nvic_irq_clear(xwirq_t irqn)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                cm_nvic_clear_irq(irqn);
        } else if (SOC_EXC_PENDSV == irqn) {
                cm_nvic_clear_systick();
        } else if (SOC_EXC_SYSTICK == irqn) {
                cm_nvic_clear_systick();
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t arch_nvic_irq_tst(xwirq_t irqn, bool * pending)
{
        xwer_t rc;

        rc = XWOK;
        if (irqn >= 0) {
                *pending = cm_nvic_tst_irq(irqn);
        } else if (SOC_EXC_PENDSV == irqn) {
                *pending = cm_nvic_tst_pendsv();
        } else if (SOC_EXC_SYSTICK == irqn) {
                *pending = cm_nvic_tst_systick();
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t arch_nvic_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        if (irqn >= 0) {
                cm_nvic_set_irq_priority(irqn, cfg->irqcfg.priority);
        } else {
                cm_nvic_set_sysirq_priority(irqn, cfg->irqcfg.priority);
        }
        return XWOK;
}

__xwbsp_code
xwer_t arch_nvic_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        if (irqn >= 0) {
                cfgbuf->irqcfg.priority = cm_nvic_get_irq_priority(irqn);
        } else {
                cfgbuf->irqcfg.priority = cm_nvic_get_sysirq_priority(irqn);
        }
        return XWOK;
}

__xwbsp_code
xwer_t arch_nvic_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        __xwos_ivt_qualifier struct soc_idvt * idvt;

        idvt = &xwospl_idvt;
        databuf->data = idvt->irq[irqn];
        return XWOK;
}
