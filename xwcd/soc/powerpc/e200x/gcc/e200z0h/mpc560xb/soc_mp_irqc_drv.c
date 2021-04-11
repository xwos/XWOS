/**
 * @file
 * @brief SOC描述层：MP IRQ controller driver
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
#include <xwos/lib/xwbop.h>
#include <xwos/mp/irq.h>
#include <soc.h>
#include <soc_irq.h>
#include <soc_irqc.h>
#include <soc_sched.h>
#include <soc_mp_irqc_drv.h>

static __xwbsp_code
xwer_t soc_irqc_drv_probe(struct xwos_irqc * irqc);

static __xwbsp_code
xwer_t soc_irqc_drv_remove(struct xwos_irqc * irqc);

static __xwbsp_code
xwer_t soc_irqc_drv_request(struct xwos_irqc * irqc, xwirq_t irqn, xwisr_f isrfunc);

static __xwbsp_code
xwer_t soc_irqc_drv_release(struct xwos_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t soc_irqc_drv_enable(struct xwos_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t soc_irqc_drv_disable(struct xwos_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t soc_irqc_drv_save(struct xwos_irqc * irqc, xwirq_t irqn, xwreg_t * flag);

static __xwbsp_code
xwer_t soc_irqc_drv_restore(struct xwos_irqc * irqc, xwirq_t irqn, xwreg_t flag);

static __xwbsp_code
xwer_t soc_irqc_drv_pend(struct xwos_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t soc_irqc_drv_clear(struct xwos_irqc * irqc, xwirq_t irqn);

static __xwbsp_code
xwer_t soc_irqc_drv_cfg(struct xwos_irqc * irqc, xwirq_t irqn,
                        const struct soc_irq_cfg * cfg);

static __xwbsp_code
xwer_t soc_irqc_drv_get_cfg(struct xwos_irqc * irqc, xwirq_t irqn,
                            struct soc_irq_cfg * cfgbuf);

static __xwbsp_code
xwer_t soc_irqc_drv_get_data(struct xwos_irqc * irqc, xwirq_t irqn,
                             struct soc_irq_data * cfgdata);

__xwbsp_rodata const struct xwos_irqc_driver soc_irqc_drv = {
        .probe = soc_irqc_drv_probe,
        .remove = soc_irqc_drv_remove,
        .request = soc_irqc_drv_request,
        .release = soc_irqc_drv_release,
        .enable = soc_irqc_drv_enable,
        .disable = soc_irqc_drv_disable,
        .save = soc_irqc_drv_save,
        .restore = soc_irqc_drv_restore,
        .pend = soc_irqc_drv_pend,
        .clear = soc_irqc_drv_clear,
        .cfg = soc_irqc_drv_cfg,
        .get_cfg = soc_irqc_drv_get_cfg,
        .get_data = soc_irqc_drv_get_data,
};

static __xwbsp_code
xwer_t soc_irqc_drv_probe(struct xwos_irqc * irqc)
{
        __soc_isr_table_qualifier struct soc_isr_table * isr_table;
        xwu32_t i;

        irqccfg = irqc->soc_cfg;
        /* Module Control Register - MCR */
        /* Bit 26 - The vector table size for e200z0h Core is: 4 bytes */
        /* Bit 31 - The module for e200z0 Core is in Software Vector Mode */
        INTC.MCR.R = 0x00000000;

        /* INTC Module Configuration Register (IACKR) */
        /* Interrupt Acknowledge Register for e200z0h Core - IACKR */
        /* Bits 0-21 - The vector base address */
        INTC.IACKR.R = (volatile xwu32_t)isr_table;

        /* Priority Set Register */
        for (i = 0; i < xw_array_size(INTC.PSR); i++) {
                INTC.PSR[i].R = 0x00;
        }

        INTC.CPR.R = SOC_IRQC_OS_PRIO - 1;
        return XWOK;
}

static __xwbsp_code
xwer_t soc_irqc_drv_remove(struct xwos_irqc * irqc)
{
        INTC.CPR.R = SOC_IRQC_MAX_PRIO;
        memset(irqc->data, 0, SOCCFG_IRQ_NUM);

        return XWOK;
}

/******** ******** irq operations ******** ********/
static __xwbsp_code
xwer_t soc_irqc_drv_request(struct xwos_irqc * irqc, xwirq_t irqn, xwisr_f isrfunc)
{
        XWOS_UNUSED(irqc);
        XWOS_UNUSED(irqn);
        XWOS_UNUSED(isrfunc);

#if !defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE)
        if (irqn >= 0) {
                __soc_isr_table_qualifier struct soc_isr_table * isr_table;
                __soc_isr_table_qualifier struct soc_isr_data_table * isr_data_table;

                isr_table = irqc->isr_table;
                isr_table->soc[irqn] = isrfunc;

                isr_data_table = irqc->isr_data_table;
                if ((NULL != isr_data_table) && (NULL != data)) {
                        isr_data_table->soc[irqn] = data;
                }
        }
#endif /* !SOCCFG_RO_ISRTABLE */

        return XWOK;
}

static __xwbsp_code
xwer_t soc_irqc_drv_release(struct xwos_irqc * irqc, xwirq_t irqn)
{
        XWOS_UNUSED(irqc);
        XWOS_UNUSED(irqn);

#if !defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE)
        if (irqn >= 0) {
                __soc_isr_table_qualifier struct soc_isr_table * isr_table;
                __soc_isr_table_qualifier struct soc_isr_data_table * isr_data_table;

                isr_table = irqc->isr_table;
                isr_table->soc[irqn] = soc_isr_nop;
        }
#endif /* !SOCCFG_RO_ISRTABLE */

        return XWOK;
}

static __xwbsp_code
xwer_t soc_irqc_drv_enable(struct xwos_irqc * irqc, xwirq_t irqn)
{
        XWOS_UNUSED(irqc);
        if (irqn >= 0) {
                INTC.PSR[irqn].R |= XWBOP_BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

static __xwbsp_code
xwer_t soc_irqc_drv_disable(struct xwos_irqc * irqc, xwirq_t irqn)
{
        XWOS_UNUSED(irqc);
        if (irqn >= 0) {
                INTC.PSR[irqn].R &= ~XWBOP_BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

static __xwbsp_code
xwer_t soc_irqc_drv_save(struct xwos_irqc * irqc, xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;

        XWOS_UNUSED(irqc);
        if (irqn >= 0) {
                *flag = (xwreg_t)INTC.PSR[irqn].R;
                INTC.PSR[irqn].R &= ~XWBOP_BIT(SOC_IRQC_ENBIT);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

static __xwbsp_code
xwer_t soc_irqc_drv_restore(struct xwos_irqc * irqc, xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;

        XWOS_UNUSED(irqc);
        if (irqn >= 0) {
                INTC.PSR[irqn].R = (xwu8_t)flag & 0xFF;
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

static __xwbsp_code
xwer_t soc_irqc_drv_pend(struct xwos_irqc * irqc, xwirq_t irqn)
{
        XWOS_UNUSED(irqc);
        XWOS_UNUSED(irqn);
        return -ENOSYS;
}

static __xwbsp_code
xwer_t soc_irqc_drv_clear(struct xwos_irqc * irqc, xwirq_t irqn)
{
        XWOS_UNUSED(irqc);
        XWOS_UNUSED(irqn);
        return -ENOSYS;
}

static __xwbsp_code
xwer_t soc_irqc_drv_cfg(struct xwos_irqc * irqc, xwirq_t irqn,
                        const struct soc_irq_cfg * cfg)
{
        xwer_t rc;
        xwu8_t prio;

        XWOS_UNUSED(irqc);
        if (irqn < 0) {
                rc = -ENOSYS;
        } else {
                prio = INTC.PSR[irqn].R & XWBOP_BIT(SOC_IRQC_ENBIT);
                prio |= (cfg->priority & (XWBOP_BIT(SOC_IRQC_ENBIT) - 1));
                INTC.PSR[irqn].R = prio;
                rc = XWOK;
        }
        return rc;
}

static __xwbsp_code
xwer_t soc_irqc_drv_get_cfg(struct xwos_irqc * irqc, xwirq_t irqn,
                            struct soc_irq_cfg * cfgbuf)
{
        xwer_t rc;

        XWOS_UNUSED(irqc);
        if (irqn >= 0) {
                cfgbuf->priority = INTC.PSR[irqn].R & (XWBOP_BIT(SOC_IRQC_ENBIT) - 1);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

static __xwbsp_code
xwer_t soc_irqc_drv_get_data(struct xwos_irqc * irqc, xwirq_t irqn,
                             struct soc_irq_data * databuf)
{
        __soc_isr_table_qualifier struct soc_isr_data_table * isr_data_table;
        xwer_t rc;

        if (irqn >= 0) {
                isr_data_table = irqc->isr_data_table;
                databuf->data = isr_data_table->soc[irqn];
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}
