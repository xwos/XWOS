/**
 * @file
 * @brief XuanWuOS内核：中断控制器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_irq_h__
#define __xwos_up_irq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       type        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_irqc;
struct soc_irq_cfg;
struct soc_isr_table;
struct soc_isr_data_table;

/**
 * @brief XWOS中断控制器
 */
struct xwos_irqc {
        const char * name; /**< 名字 */
        xwsz_t irqs_num; /**< 中断数量 */
        __soc_isr_table_qualifier struct soc_isr_table * isr_table; /**< 中断向量表 */
        __soc_isr_table_qualifier struct soc_irq_data_table * irq_data_table;
                                                                    /**< 中断数据表 */
        const void * soc_cfg; /**< SOC的私有配置 */
        void * data; /**< SOC平台的私有数据 */
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       Libraries for BSP driver      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_init_code
xwer_t xwos_irqc_init(const char * name, xwsz_t irqs_num,
                      __soc_isr_table_qualifier struct soc_isr_table * isr_table,
                      __soc_isr_table_qualifier struct soc_irq_data_table * data_table,
                      const void * soc_cfg);

__xwos_init_code
void xwos_irqc_set_data(void * data);

__xwos_code
void * xwos_irqc_get_data(void);

__xwos_code
xwer_t xwos_irqc_get_irqrsc(const struct xwos_irq_resource base[], xwsz_t num,
                            const char * description,
                            const struct xwos_irq_resource ** ptrbuf);

#endif /* xwos/up/irq.h */
