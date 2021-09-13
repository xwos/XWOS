/**
 * @file
 * @brief 玄武OS移植层：中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_irq_h__
#define __xwos_ospl_irq_h__

#include <xwos/standard.h>

#if defined(XuanWuOS_CFG_CORE__mp)
#  include <xwos/mp/irq.h>
#elif defined(XuanWuOS_CFG_CORE__up)
#  include <xwos/up/irq.h>
#else
#  error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif

extern __xwos_ivt_qualifier struct soc_ivt xwospl_ivt;
extern __xwos_ivt_qualifier struct soc_idvt xwospl_idvt;

static __xwbsp_inline
void xwospl_cpuirq_enable_lc(void);
static __xwbsp_inline
void xwospl_cpuirq_disable_lc(void);
static __xwbsp_inline
void xwospl_cpuirq_restore_lc(xwreg_t cpuirq);
static __xwbsp_inline
void xwospl_cpuirq_save_lc(xwreg_t * cpuirq);

xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf);
xwer_t xwospl_irq_request(xwirq_t irqn, xwisr_f isrfunc, void * data,
                          const struct soc_irq_cfg * cfg);
xwer_t xwospl_irq_release(xwirq_t irqn);
xwer_t xwospl_irq_enable(xwirq_t irqn);
xwer_t xwospl_irq_disable(xwirq_t irqn);
xwer_t xwospl_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t xwospl_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t xwospl_irq_pend(xwirq_t irqn);
xwer_t xwospl_irq_clear(xwirq_t irqn);
xwer_t xwospl_irq_tst(xwirq_t irqn, bool * pending);
xwer_t xwospl_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t xwospl_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);
xwer_t xwospl_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf);

#include <xwosimpl_irq.h>

#endif /* xwos/ospl/irq.h */
