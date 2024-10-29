/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：中断
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_irq_h__
#define __xwos_up_osdl_irq_h__

#include <xwos/ospl/irq.h>

static __xwcc_inline
xwer_t xwosdl_irq_enable(xwirq_t irqn)
{
        return xwup_irq_enable(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_disable(xwirq_t irqn)
{
        return xwup_irq_disable(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        return xwup_irq_save(irqn, flag);
}

static __xwcc_inline
xwer_t xwosdl_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        return xwup_irq_restore(irqn, flag);
}

static __xwcc_inline
xwer_t xwosdl_irq_get_id(xwirq_t * irqnbuf)
{
        return xwup_irq_get_id(irqnbuf);
}

static __xwcc_inline
void xwosdl_cpuirq_enable_lc(void)
{
        xwup_cpuirq_enable_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_disable_lc(void)
{
        xwup_cpuirq_disable_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_resume_lc(void)
{
        xwup_cpuirq_resume_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_suspend_lc(void)
{
        xwup_cpuirq_suspend_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwup_cpuirq_restore_lc(cpuirq);
}

static __xwcc_inline
void xwosdl_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwup_cpuirq_save_lc(cpuirq);
}

static __xwcc_inline
bool xwosdl_cpuirq_test_lc(void)
{
        return xwup_cpuirq_test_lc();
}

#endif /* xwos/up/osdl/irq.h */
