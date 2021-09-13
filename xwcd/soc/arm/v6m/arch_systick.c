/**
 * @file
 * @brief 架构描述层：Cortex-M Systick Timer
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
#include <armv6m_core.h>
#include <arch_irq.h>
#include <arch_skd.h>
#include <arch_systick.h>

#ifdef BRDCFG_SYSHWT_SRCCLK
  #define ARCH_SYSHWT_SRCCLK    BRDCFG_SYSHWT_SRCCLK
#else
  #define ARCH_SYSHWT_SRCCLK    BRDCFG_SYSCLK
#endif

__xwbsp_rodata const struct soc_irq_cfg cortex_m_systick_irqcfg = {
        .irqcfg = {
                .priority = SOC_EXC_TICK_PRIO,
        },
};

__xwbsp_rodata const struct xwos_irq_resource cortex_m_systick_irqrsc = {
        .irqn = SOC_EXC_SYSTICK,
        .isr = xwospl_syshwt_isr,
        .cfg = &cortex_m_systick_irqcfg,
        .description = "irq.armv6m.systick",
};

__xwbsp_code
xwer_t arch_systick_init(struct xwospl_syshwt * hwt)
{
        xwer_t rc;

        hwt->irqrsc = &cortex_m_systick_irqrsc;
        hwt->irqs_num = 1;
        rc = xwos_irq_request(hwt->irqrsc->irqn, hwt->irqrsc->isr,
                              XWOS_UNUSED_ARGUMENT, hwt->irqrsc->cfg);
        XWOS_BUG_ON(rc < 0);

        rc = xwos_irq_enable(hwt->irqrsc->irqn);
        XWOS_BUG_ON(rc < 0);

        cm_scs.systick.rvr.u32 = (ARCH_SYSHWT_SRCCLK / XWOSPL_SYSHWT_HZ) - 1;
        cm_scs.systick.cvr.u32 = 0x0; /* clear value */
#ifdef BRDCFG_SYSHWT_SRCCLK
        cm_scs.systick.csr.u32 = 0x2;/* external clk, enable interrupt */
#else
        cm_scs.systick.csr.u32 = 0x6;/* processor clk, enable interrupt */
#endif
        return rc;
}

__xwbsp_code
xwer_t arch_systick_start(__xwcc_unused struct xwospl_syshwt * hwt)
{
        cm_scs.systick.csr.bit.en = 1;
        return XWOK;
}

__xwbsp_code
xwer_t arch_systick_stop(__xwcc_unused struct xwospl_syshwt * hwt)
{
        cm_scs.systick.csr.bit.en = 0;
        return XWOK;
}

__xwbsp_code
xwtm_t arch_systick_get_timeconfetti(__xwcc_unused struct xwospl_syshwt * hwt)
{
        xwu32_t delta;
        xwu32_t confetti;

        delta = cm_scs.systick.rvr.u32 - cm_scs.systick.cvr.u32 + 1;
        confetti = delta / (ARCH_SYSHWT_SRCCLK / XWTM_MS) * XWTM_US;
        return (xwtm_t)confetti;
}

__xwbsp_isr
void xwospl_syshwt_isr(void)
{
        struct xwospl_skd * xwskd;
        __xwcc_unused xwu32_t csr;

        xwmb_read(xwu32_t, csr, &cm_scs.systick.csr.u32); /* read to clear COUNTFLAG */
        xwskd = xwosplcb_skd_get_lc();
        xwosplcb_syshwt_task(&xwskd->tt.hwt);
}
