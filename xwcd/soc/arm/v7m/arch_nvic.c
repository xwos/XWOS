/**
 * @file
 * @brief 架构描述层：NVIC中断控制器
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
#include <string.h>
#include <xwos/ospl/irq.h>
#include <armv7m_isa.h>
#include <armv7m_nvic.h>
#include <arch_irq.h>
#include <arch_nvic.h>

__xwbsp_code
void arch_nvic_init(void)
{
        cm_set_basepri(0);
        cm_nvic_set_prioritygrouping(SOCCFG_NVIC_SUBPRIO_BITIDX);
}

__xwbsp_code
xwer_t arch_nvic_irq_get_id(xwirq_t * irqnbuf)
{
        xwirq_t curr;
        xwer_t rc;

        curr = cm_scs.scb.icsr.bit.vect_active;
        if (0 == curr) {
                rc = -ENOTISRCTX;
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
