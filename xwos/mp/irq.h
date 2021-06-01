/**
 * @file
 * @brief 玄武OS MP内核：中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - EXC：异常，编号为负数，CPU私有的中断；
 * - IRQ：中断，编号为正数或0，被所有CPU共享，默认被CPU0响应，可指定其他CPU响应。
 */

#ifndef __xwos_mp_irq_h__
#define __xwos_mp_irq_h__

#include <xwos/standard.h>
#include <xwos/ospl/soc/irq.h>

xwer_t xwmp_irq_request(xwirq_t irqn, xwisr_f isr, void * data,
                        const struct soc_irq_cfg * cfg);
xwer_t xwmp_irq_release(xwirq_t irqn);
xwer_t xwmp_irq_enable(xwirq_t irqn);
xwer_t xwmp_irq_disable(xwirq_t irqn);
xwer_t xwmp_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t xwmp_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t xwmp_irq_pend(xwirq_t irqn);
xwer_t xwmp_irq_clear(xwirq_t irqn);
xwer_t xwmp_irq_tst(xwirq_t irqn, bool * pending);
xwer_t xwmp_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t xwmp_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);
xwer_t xwmp_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf);
xwer_t xwmp_irq_get_id(xwirq_t * irqnbuf);
void xwmp_cpuirq_enable_lc(void);
void xwmp_cpuirq_disable_lc(void);
void xwmp_cpuirq_restore_lc(xwreg_t cpuirq);
void xwmp_cpuirq_save_lc(xwreg_t * cpuirq);

#endif /* xwos/mp/irq.h */
