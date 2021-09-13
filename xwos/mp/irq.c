/**
 * @file
 * @brief 玄武OS MP内核：中断控制器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/irq.h>

__xwmp_api
xwer_t xwmp_irq_request(xwirq_t irqn, xwisr_f isr, void * data,
                        const struct soc_irq_cfg * cfg)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_request(irqn, isr, data, cfg);
}

__xwmp_api
xwer_t xwmp_irq_release(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_release(irqn);
}

__xwmp_api
xwer_t xwmp_irq_enable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_enable(irqn);
}

__xwmp_api
xwer_t xwmp_irq_disable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_disable(irqn);;
}

__xwmp_api
xwer_t xwmp_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_save(irqn, flag);
}

__xwmp_api
xwer_t xwmp_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_restore(irqn, flag);
}

__xwmp_api
xwer_t xwmp_irq_pend(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_pend(irqn);
}

__xwmp_api
xwer_t xwmp_irq_clear(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_clear(irqn);
}

__xwmp_api
xwer_t xwmp_irq_tst(xwirq_t irqn, bool * pending)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((pending), "nullptr", -EFAULT);

        return xwospl_irq_tst(irqn, pending);
}

__xwmp_api
xwer_t xwmp_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_cfg(irqn, cfg);
}

__xwmp_api
xwer_t xwmp_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_get_cfg(irqn, cfgbuf);
}

__xwmp_api
xwer_t xwmp_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM),
                      "out-of-range", -ERANGE);
        XWOS_VALIDATE((irqn >= (0 - (xwirq_t)SOCCFG_EXC_NUM)),
                      "out-of-range", -ERANGE);

        return xwospl_irq_get_data(irqn, databuf);
}

__xwmp_api
xwer_t xwmp_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = xwospl_irq_get_id(irqnbuf);
        if (rc < 0) {
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
                if (xwmp_skd_tst_in_bh_lc()) {
                        rc = -EINBH;
                } else {
                        rc = -EINTHD;
                }
#else
                rc = -EINTHD;
#endif
        }
        return rc;
}

__xwmp_api
void xwmp_cpuirq_enable_lc(void)
{
        xwospl_cpuirq_enable_lc();
}

__xwmp_api
void xwmp_cpuirq_disable_lc(void)
{
        xwospl_cpuirq_disable_lc();
}

__xwmp_api
void xwmp_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwmp_api
void xwmp_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
}
