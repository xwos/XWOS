/**
 * @file
 * @brief 玄武OS移植实现层：系统硬件定时器
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
#include <xwos/osal/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/ospl/syshwt.h>
#include <soc.h>

#define SYSHWT_SRCCLK           BRDCFG_SYSHWT_SRCCLK

__xwbsp_rodata const struct soc_irq_cfg soc_syshwt_irq_cfg = {
        .level = 0,
        .priority = 0,
        .trig = ECLIC_LEVEL_TRIGGER,
        .shv = ECLIC_NON_VECTOR_INTERRUPT,
};

__xwbsp_rodata const struct xwos_irq_resource soc_syshwt_irqrsc = {
        .irqn = SysTimer_IRQn,
        .isr = xwospl_syshwt_isr,
        .cfg = &soc_syshwt_irq_cfg,
        .description = "soc.syshwt.irq",
};

__xwbsp_code
xwer_t xwospl_syshwt_init(struct xwospl_syshwt * hwt)
{
        xwer_t rc;

        XWOS_UNUSED(hwt);

        SysTimer_Stop();
        SysTimer_SetLoadValue(0);
        SysTick_Reload(SYSHWT_SRCCLK / XWOSPL_SYSHWT_HZ);

        hwt->irqrsc = &soc_syshwt_irqrsc;
        hwt->irqs_num = 1;
        rc = xwos_irq_request(hwt->irqrsc->irqn, hwt->irqrsc->isr,
                              XWOS_UNUSED_ARGUMENT, hwt->irqrsc->cfg);
        if (XWOK == rc) {
                rc = xwos_irq_enable(hwt->irqrsc->irqn);
        }
        return rc;
}

__xwbsp_isr
void xwospl_syshwt_isr(void)
{
        struct xwospl_skd * xwskd;

        SysTick_Reload(SYSHWT_SRCCLK / XWOSPL_SYSHWT_HZ);
        xwskd = xwosplcb_skd_get_lc();
        xwosplcb_syshwt_task(&xwskd->tt.hwt);
}

__xwbsp_code
xwer_t xwospl_syshwt_start(struct xwospl_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        SysTimer_Start();
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_syshwt_stop(struct xwospl_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        SysTimer_Stop();
        return XWOK;
}

__xwbsp_code
xwtm_t xwospl_syshwt_get_timeconfetti(struct xwospl_syshwt * hwt)
{
        xwu64_t delta;
        xwu64_t confetti;

        XWOS_UNUSED(hwt);
        delta = SysTimer_GetCompareValue() - SysTimer_GetLoadValue();
        confetti = delta / (SYSHWT_SRCCLK / XWTM_MS) * XWTM_US;
        return (xwtm_t)confetti;
}
