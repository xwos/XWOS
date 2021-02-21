/**
 * @file
 * @brief 玄武OS MP内核：MP中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - ARCH IRQ：编号为负数
 * - CPU IRQ：编号为正数或0
 */

#ifndef __xwos_mp_irq_h__
#define __xwos_mp_irq_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>

struct xwmp_irqc;
struct soc_irq_cfg;
struct soc_irq_data;
struct soc_isr_table;
struct soc_isr_data_table;

/**
 * @brief XWOS MP中断资源
 */
struct xwmp_irq_resource {
        xwirq_t irqn; /**< 中断号 */
        xwisr_f isr; /**< 中断处理函数 */
        const struct soc_irq_cfg * cfg; /**< 配置 */
        const char * description; /**< 描述 */
};

/**
 * @brief XWOS MP中断控制器的驱动函数集
 */
struct xwmp_irqc_driver {
        xwer_t (* probe)(struct xwmp_irqc *); /**< 探测  */
        xwer_t (* remove)(struct xwmp_irqc *); /**< 删除 */
        xwer_t (* request)(struct xwmp_irqc *, xwirq_t, xwisr_f,
                           xwsq_t, void *); /**< 申请中断 */
        xwer_t (* release)(struct xwmp_irqc *, xwirq_t); /**< 释放中断 */
        xwer_t (* enable)(struct xwmp_irqc *, xwirq_t); /**< 开启中断 */
        xwer_t (* disable)(struct xwmp_irqc *, xwirq_t); /**< 关闭中断 */
        xwer_t (* save)(struct xwmp_irqc *, xwirq_t,
                        xwreg_t *); /**< 保存中断开关标志并关闭中断 */
        xwer_t (* restore)(struct xwmp_irqc *, xwirq_t,
                           xwreg_t); /**< 恢复中断开关标志 */
        xwer_t (* pend)(struct xwmp_irqc *, xwirq_t); /**< 挂起中断标志 */
        xwer_t (* clear)(struct xwmp_irqc *, xwirq_t); /**< 清除中断标志 */
        xwer_t (* cfg)(struct xwmp_irqc *, xwirq_t,
                       const struct soc_irq_cfg *); /**< 配置中断 */
        xwer_t (* get_cfg)(struct xwmp_irqc *, xwirq_t,
                           struct soc_irq_cfg *); /**< 获取中断配置 */
        xwer_t (* get_data)(struct xwmp_irqc *, xwirq_t,
                            struct soc_irq_data *); /**< 获取中断数据 */
};

/**
 * @brief XWOS MP中断控制器
 */
struct xwmp_irqc {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        /* attribute */
        const char * name; /**< 名字 */
        const struct xwmp_irqc_driver * drv; /**< 中断控制器驱动函数集 */
        xwsz_t irqs_num; /**< 中断数量 */
        __soc_isr_table_qualifier
        struct soc_isr_table * isr_table; /**< 中断向量表 */
        __soc_isr_table_qualifier
        struct soc_isr_data_table * isr_data_table; /**< 中断数据表 */
        const void * soc_cfg; /**< SOC的私有配置 */
        void * data; /**< SOC平台的私有数据 */

        /* private */
        xwid_t cpuid;
};

/**
 * @brief XWOS MP中断
 */
struct xwmp_irq {
        xwid_a cpuid; /**< IRQ绑定的CPU ID */
};

void xwmp_irqc_subsys_init(void);
xwer_t xwmp_irqc_grab(struct xwmp_irqc * irqc);
xwer_t xwmp_irqc_put(struct xwmp_irqc * irqc);

void xwmp_irqc_construct(struct xwmp_irqc * irqc);
void xwmp_irqc_destruct(struct xwmp_irqc * irqc);
xwer_t xwmp_irqc_register(struct xwmp_irqc * irqc, xwid_t cpuid, xwobj_gc_f gcfunc);
xwer_t xwmp_irqc_deregister(struct xwmp_irqc * irqc);
struct xwmp_irqc * xwmp_irqc_get_lc(void);
void xwmp_irqc_set_data(struct xwmp_irqc * irqc, void * data);
void * xwmp_irqc_get_data(struct xwmp_irqc * irqc);
struct xwmp_irqc * xwmp_irq_get_irqc(xwirq_t irqn);
xwer_t xwmp_irq_request(xwirq_t irqn, xwisr_f isr, xwsq_t flag, void * data);
xwer_t xwmp_irq_release(xwirq_t irqn);
xwer_t xwmp_irq_enable(xwirq_t irqn);
xwer_t xwmp_irq_disable(xwirq_t irqn);
xwer_t xwmp_irq_save(xwirq_t irqn, xwreg_t * flag);
xwer_t xwmp_irq_restore(xwirq_t irqn, xwreg_t flag);
xwer_t xwmp_irq_pend(xwirq_t irqn);
xwer_t xwmp_irq_clear(xwirq_t irqn);
xwer_t xwmp_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t xwmp_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);
xwer_t xwmp_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf);
xwer_t xwmp_irq_get_id(xwirq_t * irqnbuf);
void xwmp_cpuirq_enable_lc(void);
void xwmp_cpuirq_disable_lc(void);
void xwmp_cpuirq_restore_lc(xwreg_t cpuirq);
void xwmp_cpuirq_save_lc(xwreg_t * cpuirq);

#endif /* xwos/mp/irq.h */
