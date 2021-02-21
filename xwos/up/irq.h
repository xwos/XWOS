/**
 * @file
 * @brief 玄武OS UP内核：中断控制器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_irq_h__
#define __xwos_up_irq_h__

#include <xwos/standard.h>

struct xwup_irqc;
struct soc_irq_cfg;
struct soc_irq_data;
struct soc_isr_table;
struct soc_isr_data_table;

/**
 * @brief XWOS MP中断资源
 */
struct xwup_irq_resource {
        xwirq_t irqn; /**< 中断号 */
        xwisr_f isr; /**< 中断处理函数 */
        const struct soc_irq_cfg * cfg; /**< 配置 */
        const char * description; /**< 描述 */
};


/**
 * @brief XWOS UP中断控制器
 */
struct xwup_irqc {
        const char * name; /**< 名字 */
        xwsz_t irqs_num; /**< 中断数量 */
        __soc_isr_table_qualifier
        struct soc_isr_table * isr_table; /**< 中断向量表 */
        __soc_isr_table_qualifier
        struct soc_isr_data_table * isr_data_table; /**< 中断数据表 */
        const void * soc_cfg; /**< SOC的私有配置 */
        void * data; /**< SOC平台的私有数据 */
};

xwer_t xwup_irqc_init(const char * name, xwsz_t irqs_num,
                      __soc_isr_table_qualifier struct soc_isr_table * isr_table,
                      __soc_isr_table_qualifier struct soc_isr_data_table * data_table,
                      const void * soc_cfg);
void xwup_irqc_set_data(void * data);
void * xwup_irqc_get_data(void);

xwer_t xwup_irq_request(xwirq_t irqn, xwisr_f isr, xwsq_t flag, void * data);
xwer_t xwup_irq_release(xwirq_t irqn);
xwer_t xwup_irq_enable(xwirq_t irqn);
xwer_t xwup_irq_disable(xwirq_t irqn);
xwer_t xwup_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t xwup_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t xwup_irq_pend(xwirq_t irqn);
xwer_t xwup_irq_clear(xwirq_t irqn);
xwer_t xwup_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t xwup_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);
xwer_t xwup_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf);
xwer_t xwup_irq_get_id(xwirq_t * irqnbuf);
void xwup_cpuirq_enable_lc(void);
void xwup_cpuirq_disable_lc(void);
void xwup_cpuirq_restore_lc(xwreg_t cpuirq);
void xwup_cpuirq_save_lc(xwreg_t * cpuirq);

#endif /* xwos/up/irq.h */
