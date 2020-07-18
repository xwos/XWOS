/**
 * @file
 * @brief SOC IRQ controller driver
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
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/up/irq.h>
#include <soc.h>
#include <soc_irq.h>
#include <soc_sched.h>
#include <soc_up_irqc_drv.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwos_data struct xwos_irqc xwos_irqc;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ******** static function prototypes ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code
xwer_t soc_irqc_drv_init(void)
{
        __soc_isr_table_qualifier struct soc_isr_table * isr_table;
        xwu32_t i;

        isr_table = xwos_irqc.isr_table;
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

/******** ******** irq operations ******** ********/
__xwbsp_code
xwer_t soc_irqc_drv_request(xwirq_t irqn, xwisr_f isrfunc, xwsq_t flag, void * data)
{
        XWOS_UNUSED(irqn);
        XWOS_UNUSED(isrfunc);
        XWOS_UNUSED(flag);
        XWOS_UNUSED(data);

#if !defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE)
        if (irqn >= 0) {
                __soc_isr_table_qualifier struct soc_isr_table * isr_table;
                __soc_isr_table_qualifier struct soc_isr_data_table * isr_data_table;

                isr_table = xwos_irqc.isr_table;
                isr_table->soc[irqn] = isrfunc;

                isr_data_table = xwos_irqc.isr_data_table;
                if ((NULL != isr_data_table) && (NULL != data)) {
                        isr_data_table->soc[irqn] = data;
                }
        }
#endif /* !SOCCFG_RO_ISRTABLE */

        return XWOK;
}

__xwbsp_code
xwer_t soc_irqc_drv_release(xwirq_t irqn)
{
        XWOS_UNUSED(irqn);

#if !defined(SOCCFG_RO_ISRTABLE) || (1 != SOCCFG_RO_ISRTABLE)

        if (irqn >= 0) {
                __soc_isr_table_qualifier struct soc_isr_table * isr_table;
                __soc_isr_table_qualifier struct soc_isr_data_table * isr_data_table;

                isr_table = xwos_irqc.isr_table;
                isr_table->soc[irqn] = soc_isr_nop;
        }
#endif /* !SOCCFG_RO_ISRTABLE */

        return XWOK;
}

__xwbsp_code
xwer_t soc_irqc_drv_enable(xwirq_t irqn)
{
        if (irqn >= 0) {
                INTC.PSR[irqn].R |= BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irqc_drv_disable(xwirq_t irqn)
{
        if (irqn >= 0) {
                INTC.PSR[irqn].R &= ~BIT(SOC_IRQC_ENBIT);
        }
        return XWOK;
}

__xwbsp_code
xwer_t soc_irqc_drv_save(xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;

        if (irqn >= 0) {
                *flag = (xwreg_t)INTC.PSR[irqn].R;
                INTC.PSR[irqn].R &= ~BIT(SOC_IRQC_ENBIT);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irqc_drv_restore(xwirq_t irqn, xwreg_t flag)
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
xwer_t soc_irqc_drv_pend(xwirq_t irqn)
{
        XWOS_UNUSED(irqn);
        return -ENOSYS;
}

__xwbsp_code
xwer_t soc_irqc_drv_clear(xwirq_t irqn)
{
        XWOS_UNUSED(irqn);
        return -ENOSYS;
}

__xwbsp_code
xwer_t soc_irqc_drv_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        xwer_t rc;
        xwu8_t prio;

        if (irqn >= 0) {
                prio = INTC.PSR[irqn].R & BIT(SOC_IRQC_ENBIT);
                prio |= (cfg->priority & (BIT(SOC_IRQC_ENBIT) - 1));
                INTC.PSR[irqn].R = prio;
                rc = XWOK;
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irqc_drv_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        xwer_t rc;

        if (irqn >= 0) {
                cfgbuf->priority = INTC.PSR[irqn].R & (BIT(SOC_IRQC_ENBIT) - 1);
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}

__xwbsp_code
xwer_t soc_irqc_drv_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        __soc_isr_table_qualifier struct soc_irq_data_table * irq_data_table;
        xwer_t rc;

        if (irqn >= 0) {
                irq_data_table = xwos_irqc.irq_data_table;
                databuf->data = irq_data_table->soc[irqn];
                rc = XWOK;
        } else {
                rc = -EPERM;
        }
        return rc;
}
