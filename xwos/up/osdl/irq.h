/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_irq_h__
#define __xwos_up_osdl_irq_h__

#include <xwos/ospl/irq.h>

#define xwosdl_irq_resource xwup_irq_resource

static __xwcc_inline
xwer_t xwosdl_irq_request(xwirq_t irqn, xwisr_f isr, void * data,
                          const struct soc_irq_cfg * cfg)
{
        return xwup_irq_request(irqn, isr, data, cfg);
}

static __xwcc_inline
xwer_t xwosdl_irq_release(xwirq_t irqn)
{
        return xwup_irq_release(irqn);
}

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
xwer_t xwosdl_irq_pend(xwirq_t irqn)
{
        return xwup_irq_pend(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_clear(xwirq_t irqn)
{
        return xwup_irq_clear(irqn);
}

static __xwcc_inline
xwer_t xwosdl_irq_tst(xwirq_t irqn, bool * pending)
{
        return xwup_irq_tst(irqn, pending);
}

static __xwcc_inline
xwer_t xwosdl_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        return xwup_irq_cfg(irqn, cfg);
}

static __xwcc_inline
xwer_t xwosdl_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        return xwup_irq_get_cfg(irqn, cfgbuf);
}

static __xwcc_inline
xwer_t xwosdl_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        return xwup_irq_get_data(irqn, databuf);
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
void xwosdl_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwup_cpuirq_restore_lc(cpuirq);
}

static __xwcc_inline
void xwosdl_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwup_cpuirq_save_lc(cpuirq);
}

#endif /* xwos/up/osdl/irq.h */
