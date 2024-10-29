/**
 * @file
 * @brief XWOS UP内核：中断
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - EXC：异常，编号为负数，CPU私有的中断；
 * - IRQ：中断，编号为正数或0，被所有CPU共享，默认被CPU0响应，可指定其他CPU响应。
 */

#ifndef __xwos_up_irq_h__
#define __xwos_up_irq_h__

#include <xwos/standard.h>

xwer_t xwup_irq_enable(xwirq_t irqn);
xwer_t xwup_irq_disable(xwirq_t irqn);
xwer_t xwup_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t xwup_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t xwup_irq_get_id(xwirq_t * irqnbuf);
void xwup_cpuirq_enable_lc(void);
void xwup_cpuirq_disable_lc(void);
void xwup_cpuirq_resume_lc(void);
void xwup_cpuirq_suspend_lc(void);
void xwup_cpuirq_restore_lc(xwreg_t cpuirq);
void xwup_cpuirq_save_lc(xwreg_t * cpuirq);
bool xwup_cpuirq_test_lc(void);

#endif /* xwos/up/irq.h */
