/**
 * @file
 * @brief XWOS移植层：中断
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_irq_h__
#define __xwos_ospl_irq_h__

#include <xwos/standard.h>

#if defined(XWCFG_CORE__mp)
#  include <xwos/mp/irq.h>
#elif defined(XWCFG_CORE__up)
#  include <xwos/up/irq.h>
#else
#  error "Can't find the configuration XWCFG_CORE!"
#endif
#include <xwosimpl_irq.h>

static __xwbsp_inline void xwospl_cpuirq_enable_lc(void);
static __xwbsp_inline void xwospl_cpuirq_disable_lc(void);
static __xwbsp_inline void xwospl_cpuirq_restore_lc(xwreg_t cpuirq);
static __xwbsp_inline void xwospl_cpuirq_save_lc(xwreg_t * cpuirq);
static __xwbsp_inline bool xwospl_cpuirq_test_lc(void);

xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf);
xwer_t xwospl_irq_enable(xwirq_t irqn);
xwer_t xwospl_irq_disable(xwirq_t irqn);
xwer_t xwospl_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t xwospl_irq_restore(xwirq_t irqn, xwreg_t flag);

#endif /* xwos/ospl/irq.h */
