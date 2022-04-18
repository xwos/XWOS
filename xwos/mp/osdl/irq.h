/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：中断
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_osdl_irq_h__
#define __xwos_mp_osdl_irq_h__

#include <xwos/ospl/irq.h>

static __xwcc_inline
xwer_t xwosdl_irq_request(xwirq_t irqn, xwisr_f isr, void * data,
                          const struct soc_irq_cfg * cfg)
{
        return xwmp_irq_request(irqn, isr, data, cfg);
}

static __xwcc_inline
xwer_t xwosdl_irq_release(xwirq_t irqn)
{
        return xwmp_irq_release(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_enable(xwirq_t irqn)
{
        return xwmp_irq_enable(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_disable(xwirq_t irqn)
{
        return xwmp_irq_disable(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        return xwmp_irq_save(irqn, flag);
}

static __xwcc_inline
xwer_t xwosdl_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        return xwmp_irq_restore(irqn, flag);
}

static __xwcc_inline
xwer_t xwosdl_irq_pend(xwirq_t irqn)
{
        return xwmp_irq_pend(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_clear(xwirq_t irqn)
{
        return xwmp_irq_clear(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_tst(xwirq_t irqn, bool * pending)
{
        return xwmp_irq_tst(irqn, pending);
}

static __xwcc_inline
xwer_t xwosdl_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        return xwmp_irq_cfg(irqn, cfg);
}

static __xwcc_inline
xwer_t xwosdl_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        return xwmp_irq_get_cfg(irqn, cfgbuf);
}

static __xwcc_inline
xwer_t xwosdl_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        return xwmp_irq_get_data(irqn, databuf);
}

static __xwcc_inline
xwer_t xwosdl_irq_get_id(xwirq_t * irqnbuf)
{
        return xwmp_irq_get_id(irqnbuf);
}

static __xwcc_inline
void xwosdl_cpuirq_enable_lc(void)
{
        xwmp_cpuirq_enable_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_disable_lc(void)
{
        xwmp_cpuirq_disable_lc();
}

static __xwcc_inline
void xwosdl_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwmp_cpuirq_restore_lc(cpuirq);
}

static __xwcc_inline
void xwosdl_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwmp_cpuirq_save_lc(cpuirq);
}

#endif /* xwos/mp/osdl/irq.h */
