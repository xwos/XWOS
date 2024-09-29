/**
 * @file
 * @brief XWOS UP内核：中断控制器
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
#include <xwos/up/skd.h>
#include <xwos/up/irq.h>

__xwup_api
xwer_t xwup_irq_enable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_enable(irqn);
}

__xwup_api
xwer_t xwup_irq_disable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_disable(irqn);;
}

__xwup_api
xwer_t xwup_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_save(irqn, flag);
}

__xwup_api
xwer_t xwup_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (-(xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_restore(irqn, flag);
}

__xwup_api
xwer_t xwup_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = xwospl_irq_get_id(irqnbuf);
        if (rc < 0) {
                rc = -ENOTISRCTX;
        }
        return rc;
}

__xwup_api
void xwup_cpuirq_enable_lc(void)
{
        struct xwup_skd * xwskd = xwup_skd_get_lc();
        if (xwskd->dis_irq_cnt > (xwsq_t)1) {
                xwskd->dis_irq_cnt--;
        } else if ((xwsq_t)1 == xwskd->dis_irq_cnt) {
                xwskd->dis_irq_cnt--;
                xwospl_cpuirq_enable_lc();
        } else {}
}

__xwup_api
void xwup_cpuirq_disable_lc(void)
{
        struct xwup_skd * xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_disable_lc();
        xwskd->dis_irq_cnt++;
}

__xwup_api
void xwup_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwup_api
void xwup_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
}
