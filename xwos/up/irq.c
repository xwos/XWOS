/**
 * @file
 * @brief XuanWuOS内核：中断控制器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/string.h>
#include <xwos/irq.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/irq.h>
#include <soc_up_irqc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_data struct xwos_irqc xwos_irqc;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS INIT LIB：初始化中断控制器
 * @param name: (I) 名字字符串
 * @param irqs_num: (I) 中断的数量
 * @param soc_cfg: (I) SOC私有配置
 * @param isr_table: (I) 中断向量表
 * @param data_table: (I) 中断数据表
 * @return 错误码
 */
__xwos_init_code
xwer_t xwos_irqc_init(const char * name, xwsz_t irqs_num,
                      __soc_isr_table_qualifier struct soc_isr_table * isr_table,
                      __soc_isr_table_qualifier struct soc_irq_data_table * data_table,
                      const void * soc_cfg)
{
        xwer_t rc;

        xwos_irqc.name = name;
        xwos_irqc.irqs_num = irqs_num;
        xwos_irqc.isr_table = isr_table;
        xwos_irqc.irq_data_table = data_table;
        xwos_irqc.soc_cfg = soc_cfg;
        rc = soc_irqc_init();
        return rc;
}

/**
 * @brief XWOS INIT LIB：设置中断控制器的数据
 * @param data: (I) 数据
 */
__xwos_init_code
void xwos_irqc_set_data(void * data)
{
        xwos_irqc.data = data;
}

/**
 * @brief XWOS IRQC BSP LIB：返回中断控制器的数据
 * @return 数据的首地址指针
 */
__xwos_code
void * xwos_irqc_get_data(void)
{
        return xwos_irqc.data;
}

/**
 * @brief XWOS IRQC BSP LIB：通过描述得到中断资源
 * @param base: (I) 中断资源数组的首地址
 * @param num: (I) 中断资源数组中元素个数
 * @param description: (I) 描述
 * @param ptrbuf: (O) 指向用于返回结果的缓冲区的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval <0: 没有找到资源
 */
__xwos_code
xwer_t xwos_irqc_get_irqrsc(const struct xwos_irq_resource base[], xwsz_t num,
                            const char * description,
                            const struct xwos_irq_resource ** ptrbuf)
{
        xwsz_t i;
        for (i = 0; i < num; i++) {
                if (!strcmp(base[i].description, description)) {
                        *ptrbuf = &base[i];
                        return XWOK;
                }
        }
        *ptrbuf = NULL;
        return -ENODEV;
}

/******** ******** IRQ APIs ******** ********/
__xwos_api
xwer_t xwos_irq_request(xwirq_t irqn, xwisr_f isr, xwsq_t flag, void * data)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_request_irq(irqn, isr, flag, data);
}

__xwos_api
xwer_t xwos_irq_release(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_release_irq(irqn);
}

__xwos_api
xwer_t xwos_irq_enable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_enable_irq(irqn);
}

__xwos_api
xwer_t xwos_irq_disable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_disable_irq(irqn);;
}

__xwos_api
xwer_t xwos_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_save_irq(irqn, flag);
}

__xwos_api
xwer_t xwos_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_restore_irq(irqn, flag);
}

__xwos_api
xwer_t xwos_irq_pend(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_pend_irq(irqn);
}

__xwos_api
xwer_t xwos_irq_clear(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_clear_irq(irqn);
}

__xwos_api
xwer_t xwos_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_cfg_irq(irqn, cfg);
}

__xwos_api
xwer_t xwos_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_get_cfg_irq(irqn, cfgbuf);
}

__xwos_api
xwer_t xwos_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_get_data_irq(irqn, databuf);
}

__xwos_api
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = soc_irq_get_id(irqnbuf);
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
        if (rc < 0) {
                if (xwos_scheduler_tst_in_bh_lc()) {
                        rc = -EINBH;
                } else {
                        rc = -EINTHRD;
                }
        }
#endif /* XWUPCFG_SD_BH */
        return rc;
}
