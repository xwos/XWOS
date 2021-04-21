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

#define SYSHWT_CH               SOCCFG_SYSHWT_CHANNEL
#define SYSHWT_SRCCLK           SOCCFG_SYSHWT_SRCCLK

#if defined(XuanWuOS_CFG_CORE__mp)
  #define SYSHWT_HZ             (XWTM_S / XWMPCFG_SYSHWT_PERIOD)
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

__xwbsp_rodata const struct soc_irq_cfg soc_syshwt_irq_cfg = {
        .priority = SOC_SYSHWT_PRIO,
};

__xwbsp_rodata const struct xwos_irq_resource soc_syshwt_irqrsc = {
        .irqn = SYSHWT_IRQN,
        .isr = soc_syshwt_isr,
        .cfg = &soc_syshwt_irq_cfg,
        .description = "soc.syshwt.irq",
};

struct xwospl_skd * soc_skd_get_lc(void);

__xwbsp_code
xwer_t xwospl_syshwt_init(struct xwospl_syshwt * hwt)
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
        return XWOK;
}

/**
 * @brief 硬件定时器中断服务
 */
__xwbsp_isr
void soc_syshwt_isr(void)
{
        struct xwospl_skd * xwskd;

        /* clear IRQ flag */
        PIT.CH[SYSHWT_CH].TFLG.B.TIF = 1;
        xwskd = soc_skd_get_lc();

#if defined(XuanWuOS_CFG_CORE__mp)
        xwmp_syshwt_task(&xwskd->tt.hwt);
#elif defined(XuanWuOS_CFG_CORE__up)
        xwup_syshwt_task(&xwskd->tt.hwt);
#endif
}

__xwbsp_code
xwer_t xwospl_syshwt_start(struct xwospl_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        PIT.CH[SYSHWT_CH].TCTRL.B.TEN = 1;
        return XWOK;
}

__xwbsp_code
xwer_t xwospl_syshwt_stop(struct xwospl_syshwt * hwt)
{
        XWOS_UNUSED(hwt);
        PIT.CH[SYSHWT_CH].TCTRL.B.TEN = 0;
        return XWOK;
}

__xwbsp_code
xwtm_t xwospl_syshwt_get_timeconfetti(struct xwospl_syshwt * hwt)
{
        xwu32_t cnt;
        xwu32_t confetti;

        XWOS_UNUSED(hwt);
        cnt = PIT.CH[SYSHWT_CH].LDVAL.R - PIT.CH[SYSHWT_CH].CVAL.R + 1;
        confetti = cnt / (SYSHWT_SRCCLK / XWTM_MS) * XWTM_US;
        return (xwtm_t)confetti;
}
