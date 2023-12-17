/**
 * @file
 * @brief 架构描述层：ARMv8A Generic Timer
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include <xwcd/soc/arm64/v8a/arch_timer.h>
#include <xwos/osal/irq.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#if defined(ARCHCFG_GICV2) && (1 == ARCHCFG_GICV2)
#  include <xwcd/soc/arm64/v8a/arch_gic2.h>
#endif
#if defined(ARCHCFG_GICV3) && (1 == ARCHCFG_GICV3)
#  include <xwcd/soc/arm64/v8a/arch_gic3.h>
#endif

/* #define SOC_DBGF */
#include <soc_debug.h>

#define ARMV8A_CNTHCTL_EL1PCTEN         (1U << 0U)
#define ARMV8A_CNTHCTL_EL1PCEN          (1U << 1U)
#define ARMV8A_CNTHCTL_EVNTEN           (1U << 2U)
#define ARMV8A_CNTHCTL_EVNTDIR          (1U << 3U)

__xwbsp_code
void armv8a_timer_init(void)
{
        __xw_io xwu64_t cnthctl;

        cnthctl = (ARMV8A_CNTHCTL_EL1PCTEN /* allow accessing to CNTPCT_EL0 in EL1/0 */ |
                   ARMV8A_CNTHCTL_EL1PCEN /* allow accessing to CNTP_CVAL_EL0 in EL1/0 */);
        armv8a_sysreg_write(cnthctl_el2, cnthctl);
        soc_show_sysreg(cnthctl_el2);
}

struct armv8a_cnthp {
        xwu64_t cval; /**< backup cval to calculate next cval */
        void (* handler)(xwid_t /* cpuid */); /**< IRQ handler */
};

__xwcc_alignl1cache
struct armv8a_cnthp armv8a_cnthp[CPUCFG_CPU_NUM];

__xwbsp_code
void armv8a_cnthp_init(xwu64_t hz, void (* handler)(xwid_t))
{
        __xw_io xwid_t cpuid;
        __xw_io xwu64_t cntfrq;
        xwu64_t reloading;

        armv8a_sysreg_write(cnthp_ctl_el2, (xwu64_t)0);

        soc_show_sysreg(cntfrq_el0);
        soc_show_sysreg(cntpct_el0);

        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&cntfrq, cntfrq_el0);
        reloading = cntfrq / hz;
        armv8a_sysreg_write(cnthp_tval_el2, reloading);
        /* Backup value as next origin time */
        armv8a_sysreg_read(&armv8a_cnthp[cpuid].cval, cnthp_cval_el2);
        armv8a_cnthp[cpuid].handler = handler;
        /* Enable timer & clear IMASK */
        armv8a_sysreg_write(cnthp_ctl_el2, (xwu64_t)1);

        soc_show_sysreg(cnthp_tval_el2);
        soc_show_sysreg(cnthp_cval_el2);
        soc_show_sysreg(cnthp_tval_el2);
        soc_show_sysreg(cnthp_ctl_el2);

        soc_dbgf("CNTHP", "Init CNTHP, origin:%ld\r\n",
                 armv8a_cnthp[cpuid].cval);

        /* Init IRQ */
        armv8a_gic_irq_set_priority(ARMV8A_IRQn_CNTHP, armv8a_gic_get_min_priority());
        armv8a_gic_irq_set_isr(ARMV8A_IRQn_CNTHP, armv8a_cnthp_isr);
        armv8a_gic_irq_enable(ARMV8A_IRQn_CNTHP);
}

__xwbsp_code
void armv8a_cnthp_fini(void)
{
}

void armv8a_cnthp_reload(xwu64_t hz)
{
        __xw_io xwid_t cpuid;
        __xw_io xwu64_t cntfrq;
        xwu64_t reloading;
        xwu64_t cntpct;

        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&cntfrq, cntfrq_el0);
        reloading = cntfrq / hz;
        armv8a_cnthp[cpuid].cval += reloading;
        armv8a_sysreg_read(&cntpct, cntpct_el0);
        if (cntpct >= armv8a_cnthp[cpuid].cval) {
                armv8a_sysreg_write(cnthp_tval_el2, reloading);
                armv8a_sysreg_read(&armv8a_cnthp[cpuid].cval, cnthp_cval_el2);
        } else {
                armv8a_sysreg_write(cnthp_cval_el2, armv8a_cnthp[cpuid].cval);
        }
        /* Enable timer & clear IMASK */
        armv8a_sysreg_write(cnthp_ctl_el2, (xwu64_t)1);
}

__xwbsp_code
xwtm_t armv8a_cnthp_get_timeconfetti(xwu64_t hz)
{
        __xw_io xwu64_t cntfrq;
        xwu64_t reloading;
        xwu64_t delta;
        xwu64_t confetti;

        armv8a_sysreg_read(&cntfrq, cntfrq_el0);
        reloading = cntfrq / hz;
        armv8a_sysreg_read(&delta, cnthp_tval_el2);
        delta = reloading - delta;
        confetti = (delta / (cntfrq / (xwu64_t)xwtm_ms(1)) * (xwu64_t)xwtm_us(1));
        return (xwtm_t)confetti;
}

__xwbsp_isr
void armv8a_cnthp_isr(void)
{
        __xw_io xwid_t cpuid;

        armv8a_sysreg_write(cnthp_ctl_el2, (xwu64_t)0);
        cpuid = xwospl_skd_get_cpuid_lc();
        if (NULL != armv8a_cnthp[cpuid].handler) {
                armv8a_cnthp[cpuid].handler(cpuid);
        }
}

struct armv8a_cntp {
        xwu64_t cval; /**< backup cval to calculate next cval */
        void (* handler)(xwid_t /* cpuid */); /**< IRQ handler */
};

__xwcc_alignl1cache
struct armv8a_cntp armv8a_cntp[CPUCFG_CPU_NUM];

__xwbsp_code
void armv8a_cntp_init(xwu64_t hz, void (* handler)(xwid_t))
{
        __xw_io xwid_t cpuid;
        xwu64_t cntfrq;
        xwu64_t reloading;

        armv8a_sysreg_write(cntp_ctl_el0, (xwu64_t)0);

        soc_show_sysreg(cntfrq_el0);
        soc_show_sysreg(cntpct_el0);

        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&cntfrq, cntfrq_el0);
        reloading = cntfrq / hz;
        armv8a_sysreg_write(cntp_tval_el0, reloading);
        /* Backup value as next origin time */
        armv8a_sysreg_read(&armv8a_cntp[cpuid].cval, cntp_cval_el0);
        armv8a_cntp[cpuid].handler = handler;
        /* Enable timer & clear IMASK */
        armv8a_sysreg_write(cntp_ctl_el0, (xwu64_t)1);

        soc_show_sysreg(cntp_tval_el0);
        soc_show_sysreg(cntp_cval_el0);
        soc_show_sysreg(cntp_tval_el0);
        soc_show_sysreg(cntp_ctl_el0);

        soc_dbgf("CNTP", "Init CNTP origin:%ld\r\n",
                 armv8a_cntp[cpuid].cval);

        /* Init IRQ */
        armv8a_gic_irq_set_priority(ARMV8A_IRQn_CNTP, armv8a_gic_get_min_priority());
        armv8a_gic_irq_set_isr(ARMV8A_IRQn_CNTP, armv8a_cntp_isr);
        armv8a_gic_irq_enable(ARMV8A_IRQn_CNTP);
}

__xwbsp_code
void armv8a_cntp_fini(void)
{
}

void armv8a_cntp_reload(xwu64_t hz)
{
        __xw_io xwid_t cpuid;
        __xw_io xwu64_t cntfrq;
        xwu64_t reloading;
        xwu64_t cntpct;

        cpuid = xwospl_skd_get_cpuid_lc();
        armv8a_sysreg_read(&cntfrq, cntfrq_el0);
        reloading = cntfrq / hz;
        armv8a_cntp[cpuid].cval += reloading;
        armv8a_sysreg_read(&cntpct, cntpct_el0);
        if (cntpct >= armv8a_cntp[cpuid].cval) {
                armv8a_sysreg_write(cntp_tval_el0, reloading);
                armv8a_sysreg_read(&armv8a_cntp[cpuid].cval, cntp_cval_el0);
        } else {
                armv8a_sysreg_write(cntp_cval_el0, armv8a_cntp[cpuid].cval);
        }
        /* Enable timer & clear IMASK */
        armv8a_sysreg_write(cntp_ctl_el0, (xwu64_t)1);
}

__xwbsp_code
xwtm_t armv8a_cntp_get_timeconfetti(xwu64_t hz)
{
        __xw_io xwu64_t cntfrq;
        xwu64_t reloading;
        xwu64_t delta;
        xwu64_t confetti;

        armv8a_sysreg_read(&cntfrq, cntfrq_el0);
        reloading = cntfrq / hz;
        armv8a_sysreg_read(&delta, cntp_tval_el0);
        delta = reloading - delta;
        confetti = (delta / (cntfrq / (xwu64_t)xwtm_ms(1)) * (xwu64_t)xwtm_us(1));
        return (xwtm_t)confetti;
}

__xwbsp_isr
void armv8a_cntp_isr(void)
{
        __xw_io xwid_t cpuid;

        armv8a_sysreg_write(cntp_ctl_el0, (xwu64_t)0);
        cpuid = xwospl_skd_get_cpuid_lc();
        if (NULL != armv8a_cntp[cpuid].handler) {
                armv8a_cntp[cpuid].handler(cpuid);
        }
}
