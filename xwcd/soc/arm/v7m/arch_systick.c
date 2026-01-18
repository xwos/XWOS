/**
 * @file
 * @brief 架构描述层：Cortex-M Systick Timer
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#include <xwcd/soc/arm/v7m/arch_systick.h>
#include <xwos/osal/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/ospl/syshwt.h>
#include <xwcd/soc/arm/v7m/armv7m_isa.h>
#include <xwcd/soc/arm/v7m/arch_irq.h>
#include <xwcd/soc/arm/v7m/arch_skd.h>

#ifdef BRDCFG_SYSHWT_SRCCLK
#  define ARCH_SYSHWT_SRCCLK BRDCFG_SYSHWT_SRCCLK
#else
#  define ARCH_SYSHWT_SRCCLK BRDCFG_SYSCLK
#endif

__xwbsp_rodata const xwirq_t armv7m_systick_irqrsc[1] = {
        SOC_EXC_SYSTICK,
};

__xwbsp_code
void arch_systick_init(struct xwospl_syshwt * hwt)
{
        hwt->irqrsc = armv7m_systick_irqrsc;
        hwt->irqs_num = 1;
        armv7m_nvic_set_sysirq_priority(SOC_EXC_SYSTICK, SOC_EXC_TICK_PRIO);

        armv7m_scs.systick.rvr.u32 = (ARCH_SYSHWT_SRCCLK / XWOSPL_SYSHWT_HZ) - 1;
        armv7m_scs.systick.cvr.u32 = 0x0; /* clear value */
#ifdef BRDCFG_SYSHWT_SRCCLK
        armv7m_scs.systick.csr.u32 = 0x2;/* external clk, enable interrupt */
#else
        armv7m_scs.systick.csr.u32 = 0x6;/* processor clk, enable interrupt */
#endif
}

__xwbsp_code
xwer_t arch_systick_start(__xwcc_unused struct xwospl_syshwt * hwt)
{
        armv7m_scs.systick.csr.bit.en = 1;
        return XWOK;
}

__xwbsp_code
xwer_t arch_systick_stop(__xwcc_unused struct xwospl_syshwt * hwt)
{
        armv7m_scs.systick.csr.bit.en = 0;
        return XWOK;
}

__xwbsp_code
xwtm_t arch_systick_get_timeconfetti(__xwcc_unused struct xwospl_syshwt * hwt)
{
        xwu32_t delta;
        xwu32_t confetti;

        delta = armv7m_scs.systick.rvr.u32 - armv7m_scs.systick.cvr.u32 + (xwu32_t)1;
        confetti = (delta / ((xwu32_t)ARCH_SYSHWT_SRCCLK / (xwu32_t)xwtm_ms(1)) *
                    (xwu32_t)xwtm_us(1));
        return (xwtm_t)confetti;
}

__xwbsp_isr
void xwospl_syshwt_isr(void)
{
        struct xwospl_skd * xwskd;
        __xwcc_unused xwu32_t csr;

        /* read to clear COUNTFLAG */
        xwmb_read(xwu32_t, csr, &armv7m_scs.systick.csr.u32);
        xwskd = xwosplcb_skd_get_lc();
        xwosplcb_syshwt_task(&xwskd->tt.hwt);
}
