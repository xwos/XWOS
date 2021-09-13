/**
 * @file
 * @brief 玄武OS移植实现层：调度
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
#include <xwos/ospl/soc/xwsc.h>
#include <xwos/ospl/skd.h>
#include <soc.h>

#ifndef __riscv_32e
#  define SOC_CALLER_CONTEXT_SIZE (20 * REGBYTES)
#  define SOC_CALLEE_CONTEXT_SIZE (13 * REGBYTES)
#else
#  define SOC_CALLER_CONTEXT_SIZE (14 * REGBYTES)
#  define SOC_CALLEE_CONTEXT_SIZE (3 * REGBYTES)
#endif
#define SOC_CONTEXT_SIZE (SOC_CALLER_CONTEXT_SIZE + SOC_CALLEE_CONTEXT_SIZE)

extern xwu8_t xwos_stk_top[];

__xwbsp_rodata const struct soc_irq_cfg soc_swcx_irqcfg = {
        .level = 0,
        .priority = 0,
        .trig = ECLIC_LEVEL_TRIGGER,
        .shv = ECLIC_NON_VECTOR_INTERRUPT,
};

__xwbsp_rodata const struct xwos_irq_resource soc_swcx_irqrsc = {
        .irqn = SysTimerSW_IRQn,
        .isr = xwospl_skd_isr_swcx,
        .cfg = &soc_swcx_irqcfg,
        .description = "soc.irq.swcx",
};

__xwbsp_code
xwer_t xwospl_skd_init(struct xwospl_skd * xwskd)
{
        xwer_t rc;
        const struct xwos_irq_resource * irqrsc;

        XWOS_UNUSED(xwskd);
        irqrsc = &soc_swcx_irqrsc;
        rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr,
                              XWOS_UNUSED_ARGUMENT, irqrsc->cfg);
        XWOS_BUG_ON(rc < 0);

        rc = xwos_irq_enable(irqrsc->irqn);
        XWOS_BUG_ON(rc < 0);
        return rc;
}

/**
 * @brief Init thread stack frame
 * @note
 * Stack frame:
 *  __caller-saved (volatile) context__
 *  19 * 4      t6 (x31) ----------
 *  18 * 4      t5 (x30)          |
 *  17 * 4      t4 (x29)          |
 *  16 * 4      t3 (x28)          |
 *  15 * 4      a7 (x17)          |
 *  14 * 4      a6 (x16)          |
 *  13 * 4      msubm ---------   |
 *  12 * 4      mepc          |   |
 *  11 * 4      mcause        |   |
 *  10 * 4      mscratch      |   |
 *  9 * 4       a5 (x15)      | RV32I
 *  8 * 4       a4 (x14)      |   |
 *  7 * 4       a3 (x13)      |   |
 *  6 * 4       a2 (x12)    RV32E |
 *  5 * 4       a1 (x11)      |   |
 *  4 * 4       a0 (x10)      |   |
 *  3 * 4       t2 (x7)       |   |
 *  2 * 4       t1 (x6)       |   |
 *  1 * 4       t0 (x5)       |   |
 *  0 * 4       ra (x1)       |   |
 *  __caller-saved (volatile) context__
 *
 *  __callee-saved (non-volatile) context__
 *  12 * 4      s11 (x27)         |
 *  11 * 4      s10 (x26)         |
 *  10 * 4      s9 (x25)          |
 *  9 * 4       s8 (x24)          |
 *  8 * 4       s7 (x23)          |
 *  7 * 4       s6 (x22)          |
 *  6 * 4       s5 (x21)        RV32I
 *  5 * 4       s4 (x20)          |
 *  4 * 4       s3 (x19)          |
 *  3 * 4       s2 (x18)          |
 *  2 * 4       mstatus -------   |
 *  1 * 4       s1 (x9)       |   |
 *  0 * 4       s0 (x8)     RV32E |
 *  __callee-saved (non-volatile) context__
 */
__xwbsp_code
void xwospl_skd_init_stack(struct xwospl_skd_stack_info * stk,
                           void (* exit)(xwer_t),
                           xwsq_t attr)
{
        bool privileged;
        xwstk_t * stkbtn;
        xwsq_t i, stknum;
        xwreg_t csr;

        privileged = !!(attr & XWOSPL_SKDATTR_PRIVILEGED);

        /* Fill all stack memory bits to 1 */
        stkbtn = (xwstk_t *)stk->base;
        stknum = stk->size / sizeof(xwstk_t);
        for (i = 0; i < stknum; i++) {
                stkbtn[i] = 0xFFFFFFFFU;
        }

        /* caller-saved (volatile) context */
#ifndef __riscv_32e
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x31 (t6) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x30 (t5) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x29 (t4) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x28 (t3) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x17 (a7) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x16 (a6) */
#endif
        /* MSUBM */
        stk->sp--;
        *(stk->sp) = (xwstk_t)(1 << 6); /* ptyp:0, typ:1 */
        /* MEPC */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->main;
        /* MCAUSE */
        if (privileged) {
                /* INTERRUPT:1,MINHV:0,MPP,MPIE:1,MPIL:0,EXCCODE:SysTimerSW_IRQn */
                csr = (xwstk_t)((1 << 31) | (3 << 28) | (1 << 27) | SysTimerSW_IRQn);
        } else {
                csr = (xwstk_t)((1 << 31) | (1 << 27) | SysTimerSW_IRQn);
        }
        stk->sp--;
        *(stk->sp) = csr;
        /* MSCRATCH */
        stk->sp--;
        *(stk->sp) = (xwstk_t)&xwos_stk_top;
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x15 (a5) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x14 (a4) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x13 (a3) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x12 (a2) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x11 (a1) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->arg; /* x10 (a0) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x7 (t2) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x6 (t1) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x5 (t0) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)exit; /* x1 (ra) */

        /* callee-saved (non-volatile) context */
#ifndef __riscv_32e
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x27 (s11) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x26 (s10) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x25 (s9) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x24 (s8) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x23 (s7) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x22 (s6) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x21 (s5) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x20 (s4) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x19 (s3) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x18 (s2) */
#endif
        /* MSTATUS */
        if (privileged) {
                csr = (xwstk_t)MSTATUS_MPP | MSTATUS_MPIE;
        } else {
                csr = (xwstk_t)MSTATUS_MPIE;
        }
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        csr |= MSTATUS_FS;
#endif
        stk->sp--;
        *(stk->sp) = csr;
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x9 (s1) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* x8 (fp) */
}

__xwbsp_code
xwid_t xwospl_skd_id_lc(void)
{
        xwid_t hartid;

        hartid = (xwid_t)__RV_CSR_READ(CSR_MHARTID);
        return hartid;
}

__xwbsp_code
void soc_skd_req_swcx(struct xwospl_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        SysTimer_SetSWIRQ();
        xwmb_mp_mb();
}

/**
 * @brief 报告栈溢出
 * @param[in] stk: 溢出的栈
 */
static __xwbsp_code
void soc_skd_report_stk_overflow(struct xwospl_skd_stack_info * stk)
{
        XWOS_UNUSED(stk);
        XWOS_BUG();
}

/**
 * @brief 切换线程上下文时检查线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwospl_skd * soc_skd_chk_swcx(void)
{
        struct xwospl_skd * xwskd;
#if defined(XWMMCFG_STACK_CHK_SWCX) && (1 == XWMMCFG_STACK_CHK_SWCX)
        struct xwospl_skd_stack_info * pstk;
        union {
                xwstk_t * ptr;
                xwptr_t value;
        } stk;
        xwstk_t * stkbtn;

        xwskd = xwosplcb_skd_get_lc();
        pstk = xwskd->pstk;
        stkbtn = (xwstk_t *)pstk->base;
        stk.ptr = pstk->sp;
        if ((stk.value - SOC_CALLEE_CONTEXT_SIZE) <
            (((xwptr_t)stkbtn) + ((XWOSPL_STACK_WATERMARK) * sizeof(xwstk_t)))) {
                soc_skd_report_stk_overflow(pstk);
        }
#else
        xwskd = xwosplcb_skd_get_lc();
#endif
        SysTimer_ClearSWIRQ();
        return xwskd;
}

/**
 * @brief 异常时检查当前线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwospl_skd * soc_skd_chk_stk(void)
{
        struct xwospl_skd * xwskd;
        struct xwospl_skd_stack_info * cstk;
        union {
                xwstk_t * ptr;
                xwptr_t value;
        } stk;
        xwstk_t * stkbtn;
        xwsz_t i;

        xwskd = xwosplcb_skd_get_lc();
        cstk = xwskd->cstk;
        stkbtn = (xwstk_t *)cstk->base;
        stk.ptr = cstk->sp;
        if ((stk.value - SOC_CALLEE_CONTEXT_SIZE) <
            (((xwptr_t)stkbtn) + ((XWOSPL_STACK_WATERMARK) * sizeof(xwstk_t)))) {
                soc_skd_report_stk_overflow(cstk);
        }
        for (i = 0; i < XWOSPL_STACK_WATERMARK; i++) {
                if (0xFFFFFFFFU != stkbtn[i]) {
                        soc_skd_report_stk_overflow(cstk);
                }
        }
        return xwskd;
}

__xwbsp_code
xwer_t xwospl_skd_suspend(struct xwospl_skd * xwskd)
{
        xwer_t rc;

        rc = (xwer_t)xwsc((xwsc_f)xwosplcb_skd_suspend_lic, 1, xwskd);
        return rc;
}

__xwbsp_code
xwer_t xwospl_skd_resume(struct xwospl_skd * xwskd)
{
        xwer_t rc;

        rc = (xwer_t)xwsc((xwsc_f)xwosplcb_skd_resume_lic, 1, xwskd);
        return rc;
}

__xwbsp_code
void xwospl_thd_exit_lc(struct xwospl_thd * thd, xwer_t rc)
{
        rc = (xwer_t)xwsc((xwsc_f)xwosplcb_thd_exit_lic, 2, thd, rc);
        while (true) {
        }
}

__xwbsp_code
xwer_t xwospl_thd_freeze_lc(struct xwospl_thd * thd)
{
        xwer_t rc;

        rc = xwsc((xwsc_f)xwosplcb_thd_freeze_lic, 1, thd);
        return rc;
}

#if defined(XuanWuOS_CFG_CORE__mp)
__xwbsp_code
xwer_t xwospl_thd_outmigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        xwer_t rc;

        rc = xwsc((xwsc_f)xwosplcb_thd_outmigrate_lic, 2, thd, cpuid);
        return rc;
}

__xwbsp_code
void xwospl_thd_immigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        XWOS_UNUSED(cpuid);
        xwosplcb_thd_immigrate_lic(thd);
}
#endif
