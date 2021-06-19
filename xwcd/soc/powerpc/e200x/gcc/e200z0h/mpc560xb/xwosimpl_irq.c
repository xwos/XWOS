/**
 * @file
 * @brief 玄武OS移植实现层：中断
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
#include <xwos/lib/xwbop.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <soc.h>

__xwbsp_code
xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;
        xwirq_t irq;

        if (SOC_IRQN_NIL == soc_context.irqn) {
                rc = -ENOTINISR;
                irq = 0;
        } else {
                rc = XWOK;
                irq = soc_context.irqn;
        }
        if (!is_err_or_null(irqnbuf)) {
                *irqnbuf = irq;
        }
        return rc;
}

__xwos_isr
void soc_isr_noop(void)
{
}

__xwbsp_code
void soc_irqc_init(void)
{
        __xwos_ivt_qualifier struct soc_ivt * ivt;
        xwu32_t i;

        ivt = &xwospl_ivt;
        /* Module Control Register - MCR */
        /* Bit 26 - The vector table size for e200z0h Core is: 4 bytes */
        /* Bit 31 - The module for e200z0 Core is in Software Vector Mode */
        INTC.MCR.R = 0x00000000;

        /* INTC Module Configuration Register (IACKR) */
        /* Interrupt Acknowledge Register for e200z0h Core - IACKR */
        /* Bits 0-21 - The vector base address */
        INTC.IACKR.R = (volatile xwu32_t)ivt;

        /* Priority Set Register */
        for (i = 0; i < xw_array_size(INTC.PSR); i++) {
                INTC.PSR[i].R = 0x00;
        }

        INTC.CPR.R = SOC_IRQC_OS_PRIO - 1;
}

__xwbsp_code
xwer_t xwospl_irq_request(xwirq_t irqn, xwisr_f isrfunc, void * data,
                          const struct soc_irq_cfg * cfg)
{
        XWOS_UNUSED(isrfunc);
        XWOS_UNUSED(data);

#if !defined(SOCCFG_RO_IVT) || (1 != SOCCFG_RO_IVT)
        if (irqn >= 0) {
                __xwos_ivt_qualifier struct soc_ivt * ivt;
                __xwos_ivt_qualifier struct soc_idvt * idvt;

                ivt = &xwospl_ivt;
                ivt->irq[irqn] = isrfunc;

                idvt = &xwospl_idvt;
                if ((NULL != idvt) && (NULL != data)) {
                        idvt->irq[irqn] = data;
                }
        }
#endif /* !SOCCFG_RO_IVT */
        if (cfg) {
                xwospl_irq_cfg(irqn, cfg);
        }
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_release(xwirq_t irqn)
{
        XWOS_UNUSED(irqn);

#if !defined(SOCCFG_RO_IVT) || (1 != SOCCFG_RO_IVT)

        if (irqn >= 0) {
                __xwos_ivt_qualifier struct soc_ivt * ivt;

                ivt = &xwospl_ivt;
                ivt->irq[irqn] = soc_isr_noop;
        }
#endif /* !SOCCFG_RO_IVT */
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_enable(xwirq_t irqn)
{
        if (irqn >= 0) {
                INTC.PSR[irqn].R |= XWBOP_BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_disable(xwirq_t irqn)
{
        if (irqn >= 0) {
                INTC.PSR[irqn].R &= ~XWBOP_BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;

        if (irqn >= 0) {
                *flag = (xwreg_t)INTC.PSR[irqn].R;
                INTC.PSR[irqn].R &= ~XWBOP_BIT(SOC_IRQC_ENBIT);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t xwospl_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;

        if (irqn >= 0) {
                INTC.PSR[irqn].R = (xwu8_t)flag & 0xFF;
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t xwospl_irq_pend(xwirq_t irqn)
{
        XWOS_UNUSED(irqn);
        return -ENOSYS;
}

__xwbsp_code
xwer_t xwospl_irq_clear(xwirq_t irqn)
{
        XWOS_UNUSED(irqn);
        return -ENOSYS;
}

__xwbsp_code
xwer_t xwospl_irq_tst(xwirq_t irqn, bool * pending)
{
        XWOS_UNUSED(irqn);
        XWOS_UNUSED(pending);
        return -ENOSYS;
}

__xwbsp_code
xwer_t xwospl_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        xwer_t rc;
        xwu8_t prio;

        if (irqn >= 0) {
                prio = INTC.PSR[irqn].R & XWBOP_BIT(SOC_IRQC_ENBIT);
                prio |= (cfg->priority & (XWBOP_BIT(SOC_IRQC_ENBIT) - 1));
                INTC.PSR[irqn].R = prio;
                rc = XWOK;
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwbsp_code
xwer_t xwospl_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        xwer_t rc;

        if (irqn >= 0) {
                cfgbuf->priority = INTC.PSR[irqn].R & (XWBOP_BIT(SOC_IRQC_ENBIT) - 1);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t xwospl_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        __xwos_ivt_qualifier struct soc_idvt * idvt;
        xwer_t rc;

        if (irqn >= 0) {
                idvt = &xwospl_idvt;
                databuf->data = idvt->irq[irqn];
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}
