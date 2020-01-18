/**
 * @file
 * @brief SOC Adaptation Code：硬件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#endif
#include <arch_cpuid.h>
#include <soc.h>
#include <soc_irq.h>
#include <soc_irqc.h>
#include <soc_syshwt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SYSHWT_CH               SOCCFG_SYSHWT_CHANNEL
#define SYSHWT_SRCCLK           SOCCFG_SYSHWT_SRCCLK

#if defined(XuanWuOS_CFG_CORE__smp)
  #define SYSHWT_HZ             (XWTM_S / XWSMPCFG_SYSHWT_PERIOD)
#elif defined(XuanWuOS_CFG_CORE__up)
  #define SYSHWT_HZ             (XWTM_S / XWUPCFG_SYSHWT_PERIOD)
#endif

#if (0 == SYSHWT_CH)
  #define SYSHWT_IRQN           IRQ_PIT0
#elif (1 == SYSHWT_CH)
  #define SYSHWT_IRQN           IRQ_PIT1
#elif (2 == SYSHWT_CH)
  #define SYSHWT_IRQN           IRQ_PIT2
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_rodata const struct soc_irq_cfg soc_syshwt_irq_cfg = {
        .priority = SOC_SYSHWT_PRIO,
};

__xwbsp_rodata const struct xwos_irq_resource soc_syshwt_irqrsc = {
        .irqn = SYSHWT_IRQN,
        .isr = soc_syshwt_isr,
        .cfg = &soc_syshwt_irq_cfg,
        .description = "soc_syshwt_irq",
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SOC Adaptation Function：Setup HW timer
 */
__xwbsp_code
xwer_t soc_syshwt_init(struct xwos_syshwt * hwt)
{
        xwer_t rc;

        XWOS_UNUSED(hwt);
        hwt->irqrsc = &soc_syshwt_irqrsc;
        hwt->irqs_num = 1;
        rc = xwos_irq_request(hwt->irqrsc->irqn, hwt->irqrsc->isr,
                              XWOS_UNUSED_ARGUMENT, NULL);
        SDL_BUG_ON(rc < 0);

        rc = xwos_irq_cfg(hwt->irqrsc->irqn, hwt->irqrsc->cfg);
        SDL_BUG_ON(rc < 0);

        rc = xwos_irq_enable(hwt->irqrsc->irqn);
        SDL_BUG_ON(rc < 0);

        /* stop clock of PIT to init */
        PIT.PITMCR.B.MDIS = 1;
        /* stopped in DEBUG mode */
        PIT.PITMCR.B.FRZ = 1;
        PIT.CH[SYSHWT_CH].LDVAL.R = (SYSHWT_SRCCLK / SYSHWT_HZ) - 1;
        PIT.CH[SYSHWT_CH].TCTRL.B.TIE = 1;
        PIT.CH[SYSHWT_CH].TCTRL.B.TEN = 0;
        /* Enable clock of PIT */
        PIT.PITMCR.B.MDIS = 0;
        return OK;
}

/**
 * @brief HW Timer Interrupt Service Routine
 */
__xwbsp_isr
void soc_syshwt_isr(void)
{
        struct xwos_scheduler * xwsd;

        /* clear IRQ flag */
        PIT.CH[SYSHWT_CH].TFLG.B.TIF = 1;
        xwsd = xwos_scheduler_get_lc();
        xwos_syshwt_task(&xwsd->tt.hwt);
}

/**
 * @brief SOC Adaptation Function：Start HW timer
 * @param hwt: (I) hardware timer
 */
__xwbsp_code
xwer_t soc_syshwt_start(struct xwos_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        PIT.CH[SYSHWT_CH].TCTRL.B.TEN = 1;
        return OK;
}

/**
 * @brief SOC Adaptation Function：Stop HW timer
 * @param hwt: (I) hardware timer
 */
__xwbsp_code
xwer_t soc_syshwt_stop(struct xwos_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        PIT.CH[SYSHWT_CH].TCTRL.B.TEN = 0;
        return OK;
}

/**
 * @brief SOC Adaptation Function：Get time confetti in the current tick
 * @param hwt: (I) hardware timer
 * @return time confetti
 */
__xwbsp_code
xwtm_t soc_syshwt_get_timeconfetti(__maybe_unused struct xwos_syshwt * hwt)
{
        xwu32_t cnt;
        xwu32_t confetti;

        cnt = PIT.CH[SYSHWT_CH].LDVAL.R - PIT.CH[SYSHWT_CH].CVAL.R + 1;
        confetti = cnt / (SYSHWT_SRCCLK / XWTM_MS) * XWTM_US;
        return (xwtm_t)confetti;
}
