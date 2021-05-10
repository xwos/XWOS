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
  #include <xwos/mp/irq.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/irq.h>
#else
  #error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif

static __xwbsp_inline
void xwospl_cpuirq_enable_lc(void);

static __xwbsp_inline
void xwospl_cpuirq_disable_lc(void);

static __xwbsp_inline
void xwospl_cpuirq_restore_lc(xwreg_t cpuirq);

static __xwbsp_inline
void xwospl_cpuirq_save_lc(xwreg_t * cpuirq);

xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf);

#if defined(XuanWuOS_CFG_CORE__up)
xwer_t xwospl_irqc_init(void);
xwer_t xwospl_irqc_request_irq(xwirq_t irqn, xwisr_f isrfunc, void * data,
                               const struct soc_irq_cfg * cfg);
xwer_t xwospl_irqc_release_irq(xwirq_t irqn);
xwer_t xwospl_irqc_enable_irq(xwirq_t irqn);
xwer_t xwospl_irqc_disable_irq(xwirq_t irqn);
xwer_t xwospl_irqc_save_irq(xwirq_t irqn, xwreg_t * flag);
xwer_t xwospl_irqc_restore_irq(xwirq_t irqn, xwreg_t flag);
xwer_t xwospl_irqc_pend_irq(xwirq_t irqn);
xwer_t xwospl_irqc_clear_irq(xwirq_t irqn);
xwer_t xwospl_irqc_cfg_irq(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t xwospl_irqc_get_cfg_irq(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);
xwer_t xwospl_irqc_get_data_irq(xwirq_t irqn, struct soc_irq_data * databuf);
#endif /* XuanWuOS_CFG_CORE__up */

#include <xwosimpl_irq.h>

#endif /* xwos/ospl/irq.h */
