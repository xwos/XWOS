/**
 * @file
 * @brief XuanWuOS内核：中断
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - ARCH IRQ：编号为负数，从-1开始编号
 * - CPU IRQ：编号为正数或0，从0开始编号
 */

#ifndef __xwos_irq_h__
#define __xwos_irq_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <soc_irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(SOCCFG_RO_ISRTABLE) && (1 == SOCCFG_RO_ISRTABLE)
  #define __soc_isr_table_qualifier const
#else /* SOCCFG_RO_ISRTABLE */
  #define __soc_isr_table_qualifier
#endif /* !SOCCFG_RO_ISRTABLE */

#define XWOS_IRQ_NUM            (ARCHCFG_IRQ_NUM + SOCCFG_IRQ_NUM)

/**
 * @brief XWOS IRQ Number
 */
#define XWOS_IRQ(x)     ((xwirq_t)x)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       type        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct soc_irq_cfg;

/**
 * @brief XWOS中断资源
 */
struct xwos_irq_resource {
        xwirq_t irqn; /**< 中断号 */
        xwisr_f isr; /**< 中断处理函数 */
        const struct soc_irq_cfg * cfg; /**< 配置 */
        const char * description; /**< 描述 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwos_irq_request(xwirq_t irqn, xwisr_f isr, xwsq_t flag, void * data);

__xwos_api
xwer_t xwos_irq_release(xwirq_t irqn);

__xwos_api
xwer_t xwos_irq_enable(xwirq_t irqn);

__xwos_api
xwer_t xwos_irq_disable(xwirq_t irqn);

__xwos_api
xwer_t xwos_irq_save(xwirq_t irqn, xwreg_t * flag);

__xwos_api
xwer_t xwos_irq_restore(xwirq_t irqn, xwreg_t flag);

__xwos_api
xwer_t xwos_irq_pend(xwirq_t irqn);

__xwos_api
xwer_t xwos_irq_clear(xwirq_t irqn);

__xwos_api
xwer_t xwos_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);

__xwos_api
xwer_t xwos_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);

__xwos_api
xwer_t xwos_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf);

__xwos_api
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：开启本地CPU的中断
 */
static __xwos_inline_api
void xwos_cpuirq_enable_lc(void)
{
        soc_cpuirq_enable_lc();
}

/**
 * @brief XWOS API：关闭本地CPU的中断
 */
static __xwos_inline_api
void xwos_cpuirq_disable_lc(void)
{
        soc_cpuirq_disable_lc();
}

/**
 * @brief XWOS API：恢复本地CPU的中断
 * @param flag: (I) 本地CPU的中断开关标志
 */
static __xwos_inline_api
void xwos_cpuirq_restore_lc(xwreg_t cpuirq)
{
        soc_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWOS API：保存然后关闭本地CPU的中断
 * @param flag: (O) 指向缓冲区的指针，此缓冲区用于返回本地CPU的中断开关标志
 */
static __xwos_inline_api
void xwos_cpuirq_save_lc(xwreg_t * cpuirq)
{
        soc_cpuirq_save_lc(cpuirq);
}

#endif /* xwos/irq.h */
