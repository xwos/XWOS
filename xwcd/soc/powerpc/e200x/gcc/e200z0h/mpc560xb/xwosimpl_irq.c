/**
 * @file
 * @brief 玄武OS内核实现层：中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <soc.h>

__xwbsp_code
xwer_t xwospl_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;
        xwirq_t irq;

        if (SOC_IRQN_NIL == soc_context.irqn) {
                rc = -ENOTINISR;
                irq = 0;
        } else {
                rc = XWOK;
                irq = soc_context.irqn;
        }
        if (!is_err_or_null(irqnbuf)) {
                *irqnbuf = irq;
        }
        return rc;
}

__xwos_isr
void soc_isr_noop(void)
{
}

/**
 * @brief 设置中断优先级
 * @param irq: (I) 中断号
 * @param priority: (I) 优先级
 */
__xwbsp_code
void soc_irq_set_priority(xwirq_t irq, xwu32_t priority)
{
        INTC.PSR[irq].R = (xwu8_t)priority;
}

/**
 * @brief 获取中断优先级
 * @param irq: (I) 中断号
 * @param priority: (I) 优先级
 */
__xwbsp_code
xwu32_t soc_irq_get_priority(xwirq_t irq)
{
        return (xwu32_t)INTC.PSR[irq].R;
}

#if defined(XuanWuOS_CFG_CORE__up)
xwer_t soc_irqc_drv_init(void);
xwer_t soc_irqc_drv_request(xwirq_t irqn, xwisr_f isrfunc, xwsq_t flag, void * data);
xwer_t soc_irqc_drv_release(xwirq_t irqn);
xwer_t soc_irqc_drv_enable(xwirq_t irqn);
xwer_t soc_irqc_drv_disable(xwirq_t irqn);
xwer_t soc_irqc_drv_save(xwirq_t irqn, xwreg_t * flag);
xwer_t soc_irqc_drv_restore(xwirq_t irqn, xwreg_t flag);
xwer_t soc_irqc_drv_pend(xwirq_t irqn);
xwer_t soc_irqc_drv_clear(xwirq_t irqn);
xwer_t soc_irqc_drv_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg);
xwer_t soc_irqc_drv_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf);
xwer_t soc_irqc_drv_get_data(xwirq_t irqn, struct soc_irq_data * databuf);

__xwbsp_code
xwer_t xwospl_irqc_init(void)
{
        return soc_irqc_drv_init();
}

__xwbsp_code
xwer_t xwospl_irqc_request_irq(xwirq_t irqn, xwisr_f isrfunc, xwsq_t flag, void * data)
{
        return soc_irqc_drv_request(irqn, isrfunc, flag, data);
}

__xwbsp_code
xwer_t xwospl_irqc_release_irq(xwirq_t irqn)
{
        return soc_irqc_drv_release(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_enable_irq(xwirq_t irqn)
{
        return soc_irqc_drv_enable(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_disable_irq(xwirq_t irqn)
{
        return soc_irqc_drv_disable(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_save_irq(xwirq_t irqn, xwreg_t * flag)
{
        return soc_irqc_drv_save(irqn, flag);
}

__xwbsp_code
xwer_t xwospl_irqc_restore_irq(xwirq_t irqn, xwreg_t flag)
{
        return soc_irqc_drv_restore(irqn, flag);
}

__xwbsp_code
xwer_t xwospl_irqc_pend_irq(xwirq_t irqn)
{
        return soc_irqc_drv_pend(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_clear_irq(xwirq_t irqn)
{
        return soc_irqc_drv_clear(irqn);
}

__xwbsp_code
xwer_t xwospl_irqc_cfg_irq(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        return soc_irqc_drv_cfg(irqn, cfg);
}

__xwbsp_code
xwer_t xwospl_irqc_get_cfg_irq(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        return soc_irqc_drv_get_cfg(irqn, cfgbuf);
}

__xwbsp_code
xwer_t xwospl_irqc_get_data_irq(xwirq_t irqn, struct soc_irq_data * databuf)
{
        return soc_irqc_drv_get_data(irqn, databuf);
}
#endif /* XuanWuOS_CFG_CORE__up */
