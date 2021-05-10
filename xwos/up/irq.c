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

#include <string.h>
#include <xwos/standard.h>
#include <xwos/osal/irq.h>
#include <xwos/up/skd.h>
#include <xwos/up/irq.h>

/**
 * @brief 玄武OS UP内核中断控制器
 */
__xwup_data struct xwup_irqc xwup_irqc;

/**
 * @brief XWUP API：初始化中断控制器
 * @param name: (I) 名字字符串
 * @param irqs_num: (I) 中断的数量
 * @param soc_cfg: (I) SOC私有配置
 * @param ivt: (I) 中断向量表
 * @param data_table: (I) 中断数据表
 * @return 错误码
 */
__xwup_init_code
xwer_t xwup_irqc_init(const char * name, xwsz_t irqs_num,
                      __xwos_ivt_qualifier struct soc_isr_table * ivt,
                      __xwos_ivt_qualifier struct soc_isr_data_table * idvt,
                      const void * soc_cfg)
{
        xwer_t rc;

        xwup_irqc.name = name;
        xwup_irqc.irqs_num = irqs_num;
        xwup_irqc.ivt = ivt;
        xwup_irqc.idvt = idvt;
        xwup_irqc.soc_cfg = soc_cfg;
        rc = xwospl_irqc_init();
        return rc;
}

/**
 * @brief XWUP API：设置中断控制器的数据
 * @param data: (I) 数据
 */
__xwup_init_code
void xwup_irqc_set_data(void * data)
{
        xwup_irqc.data = data;
}

/**
 * @brief XWUP API：返回中断控制器的数据
 * @return 数据的首地址指针
 */
__xwup_code
void * xwup_irqc_get_data(void)
{
        return xwup_irqc.data;
}

/******** ******** IRQ APIs ******** ********/
__xwup_api
xwer_t xwup_irq_request(xwirq_t irqn, xwisr_f isr, void * data,
                        const struct soc_irq_cfg * cfg)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_request_irq(irqn, isr, data, cfg);
}

__xwup_api
xwer_t xwup_irq_release(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_release_irq(irqn);
}

__xwup_api
xwer_t xwup_irq_enable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_enable_irq(irqn);
}

__xwup_api
xwer_t xwup_irq_disable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_disable_irq(irqn);;
}

__xwup_api
xwer_t xwup_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_save_irq(irqn, flag);
}

__xwup_api
xwer_t xwup_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_restore_irq(irqn, flag);
}

__xwup_api
xwer_t xwup_irq_pend(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_pend_irq(irqn);
}

__xwup_api
xwer_t xwup_irq_clear(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_clear_irq(irqn);
}

__xwup_api
xwer_t xwup_irq_tst(xwirq_t irqn, bool * pending)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((pending), "nullptr", -EFAULT);

        return xwospl_irqc_tst_irq(irqn, pending);
}

__xwup_api
xwer_t xwup_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_cfg_irq(irqn, cfg);
}

__xwup_api
xwer_t xwup_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_get_irq_cfg(irqn, cfgbuf);
}

__xwup_api
xwer_t xwup_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return xwospl_irqc_get_irq_data(irqn, databuf);
}

__xwup_api
xwer_t xwup_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = xwospl_irq_get_id(irqnbuf);
        if (rc < 0) {
#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
                if (xwup_skd_tst_in_bh_lc()) {
                        rc = -EINBH;
                } else {
                        rc = -EINTHD;
                }
#else
                rc = -EINTHD;
#endif /* XWUPCFG_SKD_BH */
        }
        return rc;
}

/**
 * @brief XWUP API：开启本地CPU的中断
 */
__xwup_api
void xwup_cpuirq_enable_lc(void)
{
        xwospl_cpuirq_enable_lc();
}

/**
 * @brief XWUP API：关闭本地CPU的中断
 */
__xwup_api
void xwup_cpuirq_disable_lc(void)
{
        xwospl_cpuirq_disable_lc();
}

/**
 * @brief XWUP API：恢复本地CPU的中断
 * @param flag: (I) 本地CPU的中断开关标志
 */
__xwup_api
void xwup_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWUP API：保存然后关闭本地CPU的中断
 * @param flag: (O) 指向缓冲区的指针，此缓冲区用于返回本地CPU的中断开关标志
 */
__xwup_api
void xwup_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
}
