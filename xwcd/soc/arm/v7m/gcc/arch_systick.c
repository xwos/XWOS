/**
 * @file
 * @brief Cortex-M Systick Timer
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/irq.h>
#if defined(XuanWuOS_CFG_CORE__smp)
  #include <xwos/smp/scheduler.h>
  #include <xwos/smp/tt.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/scheduler.h>
  #include <xwos/up/tt.h>
#else
  #error "Can't find the configuration of XuanWuOS_CFG_CORE!"
#endif
#include <armv7m_core.h>
#include <arch_irq.h>
#include <arch_systick.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifdef BRDCFG_SYSHWT_SRCCLK
  #define ARCH_SYSHWT_SRCCLK    BRDCFG_SYSHWT_SRCCLK
#else
  #define ARCH_SYSHWT_SRCCLK    BRDCFG_SYSCLK
#endif

#if defined(XuanWuOS_CFG_CORE__smp)
  #define ARCH_SYSHWT_HZ        (XWTM_S / XWSMPCFG_SYSHWT_PERIOD)
#elif defined(XuanWuOS_CFG_CORE__up)
  #define ARCH_SYSHWT_HZ        (XWTM_S / XWUPCFG_SYSHWT_PERIOD)
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct soc_irq_cfg cortex_m_systick_irqcfg = {
        .irqcfg = {
                .priority = ARCH_IRQ_TICK_PRIO,
        },
};

__xwbsp_rodata const struct xwos_irq_resource cortex_m_systick_irqrsc = {
        .irqn = ARCH_IRQ_SYSTICK,
        .isr = arch_systick_isr,
        .cfg = &cortex_m_systick_irqcfg,
        .description = "irq.armv7m.systick",
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Init systick timer
 */
__xwbsp_code
xwer_t arch_systick_init(struct xwos_syshwt * hwt)
{
        xwer_t rc;

        hwt->irqrsc = &cortex_m_systick_irqrsc;
        hwt->irqs_num = 1;
        rc = xwos_irq_request(hwt->irqrsc->irqn, hwt->irqrsc->isr,
                              XWOS_UNUSED_ARGUMENT, NULL);
        XWOS_BUG_ON(rc < 0);

        rc = xwos_irq_cfg(hwt->irqrsc->irqn, hwt->irqrsc->cfg);
        XWOS_BUG_ON(rc < 0);

        rc = xwos_irq_enable(hwt->irqrsc->irqn);
        XWOS_BUG_ON(rc < 0);

        cm_scs.systick.rvr.u32 = (ARCH_SYSHWT_SRCCLK / ARCH_SYSHWT_HZ) - 1;
        cm_scs.systick.cvr.u32 = 0x0; /* clear value */
#ifdef BRDCFG_SYSHWT_SRCCLK
        cm_scs.systick.csr.u32 = 0x2;/* external clk, enable interrupt */
#else
        cm_scs.systick.csr.u32 = 0x6;/* processor clk, enable interrupt */
#endif
        return rc;
}

/**
 * @brief Systick timer interrupt handler
 */
__xwbsp_isr
void arch_systick_isr(void)
{
        struct xwos_scheduler * xwsd;
        __maybe_unused xwu32_t csr;

        xwmb_read(xwu32_t, csr, &cm_scs.systick.csr.u32); /* read to clear COUNTFLAG */
        xwsd = xwos_scheduler_get_lc();
        xwos_syshwt_task(&xwsd->tt.hwt);
}

/**
 * @brief Start Systick Timer
 */
__xwbsp_code
xwer_t arch_systick_start(__maybe_unused struct xwos_syshwt * hwt)
{
        cm_scs.systick.csr.bit.en = 1;
        return OK;
}

/**
 * @brief Stop Systick Timer
 */
__xwbsp_code
xwer_t arch_systick_stop(__maybe_unused struct xwos_syshwt * hwt)
{
        cm_scs.systick.csr.bit.en = 0;
        return OK;
}

/**
 * @brief Get time confetti in the current tick
 * @param hwt: (I) hardware timer
 * @retval time confetti
 */
__xwbsp_code
xwtm_t arch_systick_get_timeconfetti(__maybe_unused struct xwos_syshwt * hwt)
{
        xwu32_t cnt;
        xwu32_t confetti;

        cnt = cm_scs.systick.rvr.u32 - cm_scs.systick.cvr.u32 + 1;
        confetti = cnt / (ARCH_SYSHWT_SRCCLK / XWTM_MS) * XWTM_US;
        return (xwtm_t)confetti;
}
