/**
 * @file
 * @brief XuanWuOS内核：SMP中断
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

#ifndef __xwos_smp_irq_h__
#define __xwos_smp_irq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       type        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_irqc;
struct soc_irq_cfg;
struct soc_irq_data;
struct soc_isr_table;
struct soc_isr_data_table;

/**
 * @brief XWOS中断控制器的驱动函数集
 */
struct xwos_irqc_driver {
        xwer_t (* probe)(struct xwos_irqc *); /**< 探测  */
        xwer_t (* remove)(struct xwos_irqc *); /**< 删除 */
        xwer_t (* request)(struct xwos_irqc *, xwirq_t, xwisr_f,
                           xwsq_t, void *); /**< 申请中断 */
        xwer_t (* release)(struct xwos_irqc *, xwirq_t); /**< 释放中断 */
        xwer_t (* enable)(struct xwos_irqc *, xwirq_t); /**< 开启中断 */
        xwer_t (* disable)(struct xwos_irqc *, xwirq_t); /**< 关闭中断 */
        xwer_t (* save)(struct xwos_irqc *, xwirq_t,
                        xwreg_t *); /**< 保存中断开关标志并关闭中断 */
        xwer_t (* restore)(struct xwos_irqc *, xwirq_t,
                           xwreg_t); /**< 恢复中断开关标志 */
        xwer_t (* pend)(struct xwos_irqc *, xwirq_t); /**< 挂起中断标志 */
        xwer_t (* clear)(struct xwos_irqc *, xwirq_t); /**< 清除中断标志 */
        xwer_t (* cfg)(struct xwos_irqc *, xwirq_t,
                       const struct soc_irq_cfg *); /**< 配置中断 */
        xwer_t (* get_cfg)(struct xwos_irqc *, xwirq_t,
                           struct soc_irq_cfg *); /**< 得到中断配置 */
        xwer_t (* get_data)(struct xwos_irqc *, xwirq_t,
                            struct soc_irq_data *); /**< 得到中断数据 */
};

/**
 * @brief XWOS中断控制器
 */
struct xwos_irqc {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        /* attribute */
        const char * name; /**< 名字 */
        const struct xwos_irqc_driver * drv; /**< 中断控制器驱动函数集 */
        xwsz_t irqs_num; /**< 中断数量 */
        __soc_isr_table_qualifier struct soc_isr_table * isr_table; /**< 中断向量表 */
        __soc_isr_table_qualifier struct soc_irq_data_table * irq_data_table;
                                                                    /**< 中断数据表 */
        const void * soc_cfg; /**< SOC的私有配置 */
        void * data; /**< SOC平台的私有数据 */

        /* private */
        xwid_t cpuid;
};

struct xwos_irq {
        __xwcc_atomic xwid_t cpuid;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param irqc: (I) 中断控制器对象的指针
 * @return 错误码
 */
static __xwcc_inline
xwer_t xwos_irqc_grab(struct xwos_irqc * irqc)
{
        return xwos_object_grab(&irqc->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param irqc: (I) 中断控制器对象的指针
 * @return 错误码
 */
static __xwcc_inline
xwer_t xwos_irqc_put(struct xwos_irqc * irqc)
{
        return xwos_object_put(&irqc->xwobj);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       Libraries for BSP driver      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_init_code
void xwos_irqc_subsys_init(void);

__xwos_init_code
void xwos_irqc_construct(struct xwos_irqc * irqc);

__xwos_exit_code
void xwos_irqc_destruct(struct xwos_irqc * irqc);

__xwos_init_code
xwer_t xwos_irqc_register(struct xwos_irqc * irqc, xwid_t cpuid, xwobj_gc_f gcfunc);

__xwos_exit_code
xwer_t xwos_irqc_deregister(struct xwos_irqc * irqc);

__xwos_init_code
void xwos_irqc_set_data(struct xwos_irqc * irqc, void * data);

__xwos_code
void * xwos_irqc_get_data(struct xwos_irqc * irqc);

__xwos_code
struct xwos_irqc * xwos_irq_get_irqc(xwirq_t irqn);

__xwos_code
xwer_t xwos_irqc_get_irqrsc(const struct xwos_irq_resource base[], xwsz_t num,
                            const char * description,
                            const struct xwos_irq_resource ** ptrbuf);

#endif /* xwos/smp/irq.h */
