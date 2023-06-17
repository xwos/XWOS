/**
 * @file
 * @brief XWOS移植实现层：中断
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
#include <xwos/lib/xwbop.h>
#include <xwos/ospl/skd.h>
#include <soc.h>
#include <soc_irq.h>

extern xwu8_t e200z0h_ivt_lma_base[];
extern xwu8_t e200z0h_ivt_vma_base[];

__xwbsp_code
xwer_t soc_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;
        xwirq_t irq;

        if (SOC_IRQN_NIL == soc_context.irqn) {
                rc = -ENOTISRCTX;
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
        struct soc_ivt * ivt;
        xwu32_t i;

        ivt = &soc_ivt;
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
xwer_t soc_irq_request(xwirq_t irqn, xwisr_f isrfunc, void * data)
{
        if ((irqn >= 0) && (e200z0h_ivt_lma_base != e200z0h_ivt_vma_base)) {
                struct soc_ivt * ivt;
                struct soc_idvt * idvt;

                ivt = &soc_ivt;
                ivt->irq[irqn] = isrfunc;

                idvt = &soc_idvt;
                if ((NULL != idvt) && (NULL != data)) {
                        idvt->irq[irqn] = data;
                }
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_release(xwirq_t irqn)
{
        if ((irqn >= 0) && (e200z0h_ivt_lma_base != e200z0h_ivt_vma_base)) {
                struct soc_ivt * ivt;

                ivt = &soc_ivt;
                ivt->irq[irqn] = soc_isr_noop;
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_enable(xwirq_t irqn)
{
        if (irqn >= 0) {
                INTC.PSR[irqn].R |= XWBOP_BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_disable(xwirq_t irqn)
{
        if (irqn >= 0) {
                INTC.PSR[irqn].R &= ~XWBOP_BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irq_save(xwirq_t irqn, xwreg_t * flag)
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
xwer_t soc_irq_restore(xwirq_t irqn, xwreg_t flag)
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
xwer_t soc_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
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
xwer_t soc_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
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
xwer_t soc_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        struct soc_idvt * idvt;
        xwer_t rc;

        if (irqn >= 0) {
                idvt = &soc_idvt;
                databuf->data = idvt->irq[irqn];
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}
