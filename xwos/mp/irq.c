/**
 * @file
 * @brief XWOS MP内核：中断控制器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/irq.h>

__xwmp_api
xwer_t xwmp_irq_enable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_enable(irqn);
}

__xwmp_api
xwer_t xwmp_irq_disable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_disable(irqn);;
}

__xwmp_api
xwer_t xwmp_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_save(irqn, flag);
}

__xwmp_api
xwer_t xwmp_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_restore(irqn, flag);
}

__xwmp_api
xwer_t xwmp_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = xwospl_irq_get_id(irqnbuf);
        if (rc < 0) {
                rc = -ENOTISRCTX;
        }
        return rc;
}

__xwmp_api
void xwmp_cpuirq_enable_lc(void)
{
        xwospl_cpuirq_enable_lc();
}

__xwmp_api
void xwmp_cpuirq_disable_lc(void)
{
        xwospl_cpuirq_disable_lc();
}

__xwmp_api
void xwmp_cpuirq_resume_lc(void)
{
        xwmp_skd_enth_lc(); // cppcheck-suppress [misra-c2012-17.7]
}

__xwmp_api
void xwmp_cpuirq_suspend_lc(void)
{
        xwmp_skd_dsth_lc(); // cppcheck-suppress [misra-c2012-17.7]
}

__xwmp_api
void xwmp_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwmp_api
void xwmp_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
}

__xwmp_api
bool xwmp_cpuirq_test_lc(void)
{
        return xwospl_cpuirq_test_lc();
}
