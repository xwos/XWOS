/**
 * @file
 * @brief 架构描述层：调度器
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
#include <armv7m_core.h>
#include <arch_skd.h>

#define SOC_EXC_SWCX_PRIO (SOC_IRQ_PRIO_LOWEST | SOC_IRQ_SUBPRIO_LOWEST)

#define XPSR_INITVAL                                    0x01000000U
#define FPSCR_INITVAL                                   0X00000000U
#define EXC_RETURN_HANDLE_MODE_BASIC                    0xFFFFFFF1U
#define EXC_RETURN_THREAD_MODE_PSP_BASIC                0xFFFFFFFDU
#define EXC_RETURN_THREAD_MODE_MSP_BASIC                0xFFFFFFF9U
#define EXC_RETURN_HANDLE_MODE_FPUEXTENDED              0xFFFFFFE1U
#define EXC_RETURN_THREAD_MODE_PSP_FPUEXTENDED          0xFFFFFFEDU
#define EXC_RETURN_THREAD_MODE_MSP_FPUEXTENDED          0xFFFFFFE9U

#define ARCH_NVGR_SIZE                                  0x28U
#define ARCH_NVFR_SIZE                                  0x40U

extern xwstk_t xwos_stk_top[];

__xwbsp_rodata const struct soc_irq_cfg cortex_m_swcx_irq_cfg = {
        .irqcfg = {
                .priority = SOC_EXC_SWCX_PRIO,
        },
};

__xwbsp_rodata const struct xwos_irq_resource cortex_m_swcx_irqrsc = {
        .irqn = SOC_EXC_PENDSV,
        .isr = xwospl_skd_isr_swcx,
        .cfg = &cortex_m_swcx_irq_cfg,
        .description = "irq.pendsv.armv7m",
};

static __xwbsp_code
void arch_skd_report_stk_overflow(struct xwospl_skd_stack_info * stk);

/**
 * @brief 初始化切换上下文中断
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 * @note
 * - 使用PendSV作为线程上下文切换的中断
 */
__xwbsp_code
xwer_t arch_skd_init_pendsv(struct xwospl_skd * xwskd)
{
        xwer_t rc;
        const struct xwos_irq_resource * irqrsc;

        XWOS_UNUSED(xwskd);

        irqrsc = &cortex_m_swcx_irqrsc;
        rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr,
                              XWOS_UNUSED_ARGUMENT, irqrsc->cfg);
        XWOS_BUG_ON(rc < 0);

        rc = xwos_irq_enable(irqrsc->irqn);
        XWOS_BUG_ON(rc < 0);

        return rc;
}

/**
 * @brief 初始化调度对象的栈
 * @param[in] stk: 栈信息结构体指针
 * @param[in] attr: 调度对象属性
 */
__xwbsp_code
void arch_skd_init_stack(struct xwospl_skd_stack_info * stk,
                         void (* exit)(xwer_t),
                         xwsq_t attr)
{
        bool privileged;
        xwstk_t * stkbtn;
        xwsq_t i, stknum;

        privileged = !!(attr & XWOSPL_SKDATTR_PRIVILEGED);

        stkbtn = (xwstk_t *)stk->base;
        stknum = stk->size / sizeof(xwstk_t);
        for (i = 0; i < stknum; i++) {
                stkbtn[i] = 0xFFFFFFFFU;
        }

#if (defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU))
        /* volatile FPU registers */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0U; /* reserved */
        stk->sp--;
        *(stk->sp) = (xwstk_t)FPSCR_INITVAL; /* fpscr */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s15 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s14 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s13 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s12 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s11 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s10 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s9 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s8 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s7 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s6 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s5 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s4 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s3 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s2 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s1 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s0 */
#endif

        /* volatile registers */
        stk->sp--;
        *(stk->sp) = (xwstk_t)XPSR_INITVAL; /* xpsr */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->main; /* pc */
        stk->sp--;
        *(stk->sp) = (xwstk_t)exit; /* lr value to exit */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r12 (ip) */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r3 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r2 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r1 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->arg; /* r0 */

#if (defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU))
        /* non-volatile FPU registers */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s31 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s30 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s29 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s28 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s27 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s26 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s25 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s24 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s23 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s22 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s21 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s20 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s19 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s18 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s17 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* s16 */
#endif

        /* return code */
#if (defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU))
        stk->sp--;
        *(stk->sp) = EXC_RETURN_THREAD_MODE_PSP_FPUEXTENDED; /* lr */
#else
        stk->sp--;
        *(stk->sp) = EXC_RETURN_THREAD_MODE_PSP_BASIC; /* lr */
#endif

        /* non-volatile registers */
        stk->sp--;
        *(stk->sp) = (xwstk_t)(((xwptr_t)stk->base) + stk->size); /* fp */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->base; /* sl */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r9 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r8 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r7 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r6 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r5 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r4 */

        /* CONTROL */
        if (privileged) {
#if (defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU))
                stk->sp--;
                *(stk->sp) = (xwstk_t)0x6; /* CONTROL: privileged access, psp, FPCA */
#else
                stk->sp--;
                *(stk->sp) = (xwstk_t)0x2; /* CONTROL reg: privileged access, psp */
#endif
        } else {
#if (defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU))
                stk->sp--;
                *(stk->sp) = (xwstk_t)0x7; /* CONTROL: unprivileged access, psp, FPCA */
#else
                stk->sp--;
                *(stk->sp) = (xwstk_t)0x3; /* CONTROL: unprivileged access, psp */
#endif
        }
}

/**
 * @brief ADL ISR：启动调度器的SVC(svc 0)的处理函数
 * @param[in] xwskd: 调度器的指针
 */
__xwbsp_isr __xwcc_naked
void arch_skd_svcsr_start(__xwcc_unused struct xwospl_skd * xwskd)
{
        /* r3 = r0->cstk; */
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      ldr     r3, [r0, #0]");
        /* get value of stack pointer, r2 = cstk->sp */
        __asm__ volatile("      ldr     r2, [r3, #0]");
        /* restore CONTROL */
        __asm__ volatile("      ldr     r1, [r2], #4");
        __asm__ volatile("      msr     control, r1");
        /* restore non-volatile register: r4-r11, lr */
        __asm__ volatile("      ldmfd   r2!, {r4-r11, lr}");
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        /* restore s16-s31 */
        __asm__ volatile("      vldm    r2, {s16-s31}");
        __asm__ volatile("      add     r2, %[__nvfr]"
                         :
                         :[__nvfr] "I" (ARCH_NVFR_SIZE)
                         :);
#endif
        /* set PSP */
        __asm__ volatile("      msr     psp, r2");
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
        __asm__ volatile("      clrex");
        __asm__ volatile("      msr     msp, %[__xwos_stk_top]"
                         :
                         : [__xwos_stk_top] "Ir" (xwos_stk_top)
                         :);
        __asm__ volatile("      bx      lr");
}

/**
 * @brief 切换上下文的中断(PendSV)的处理函数
 */
__xwbsp_isr __xwcc_naked
void xwospl_skd_isr_swcx(void)
{
        /* get scheduler */
        __asm__ volatile(".syntax       unified");
        __asm__ volatile("      push    {lr}");
        __asm__ volatile("      sub     sp, #4");
        __asm__ volatile("      bl      arch_skd_chk_swcx");
        __asm__ volatile("      bl      xwosplcb_skd_pre_swcx_lic");
        __asm__ volatile("      add     sp, #4");
        __asm__ volatile("      pop     {lr}");

        /* get PSP */
        __asm__ volatile("      mrs     r2, psp");
        /* save previous context */
        /* r3 = r0->pstk; */
        __asm__ volatile("      ldr     r3, [r0, #4]");
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        /* save s16-s31 */
        __asm__ volatile("      sub     r2, %[__nvfr]"
                         :
                         :[__nvfr] "I" (ARCH_NVFR_SIZE)
                         :);
        __asm__ volatile("      vstmia  r2, {s16-s31}");
#endif
        /* save non-volatile registers: lr, r11-r4 */
        __asm__ volatile("      stmfd   r2!, {r4-r11, lr}");
        /* save control reg */
        __asm__ volatile("      mrs     r1, control");
        __asm__ volatile("      str     r1, [r2, #-4]!");
        /* save PSP, pstk->sp = r2; */
        __asm__ volatile("      str     r2, [r3, #0]");

        /* restore current context */
        /* r3 = r0->cstk; */
        __asm__ volatile("      ldr     r3, [r0, #0]");
        /* get value of stack pointer, r2 = cstk->sp */
        __asm__ volatile("      ldr     r2, [r3, #0]");
        /* restore CONTROL */
        __asm__ volatile("      ldr     r1, [r2], #4");
        __asm__ volatile("      msr     control, r1");
        /* restore non-volatile register: r4-r11, lr */
        __asm__ volatile("      ldmfd   r2!, {r4-r11, lr}");
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        /* restore s16-s31 */
        __asm__ volatile("      vldmia  r2, {s16-s31}");
        __asm__ volatile("      add     r2, %[__nvfr]"
                         :
                         :[__nvfr] "I" (ARCH_NVFR_SIZE)
                         :);
#endif
        /* set PSP */
        __asm__ volatile("      msr     psp, r2");
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
        __asm__ volatile("      clrex");

        /* finish the progress */
        __asm__ volatile("      push    {lr}");
        __asm__ volatile("      sub     sp, #4");
        __asm__ volatile("      bl      xwosplcb_skd_post_swcx_lic");
        __asm__ volatile("      add     sp, #4");
        __asm__ volatile("      pop     {pc}");
}

/**
 * @brief 切换线程上下文时检查线程的栈溢出
 * @return 调度器的指针
 */
__xwbsp_code
struct xwospl_skd * arch_skd_chk_swcx(void)
{
        struct xwospl_skd * xwskd;
#if defined(XWMMCFG_STACK_CHK_SWCX) && (1 == XWMMCFG_STACK_CHK_SWCX)
        struct xwospl_skd_stack_info * pstk;
        xwstk_t * stkbtn;
        xwptr_t psp;

        xwskd = xwosplcb_skd_get_lc();
        pstk = xwskd->pstk;
        stkbtn = (xwstk_t *)pstk->base;
        cm_get_psp(&psp);
#  if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        if ((psp - (ARCH_NVFR_SIZE + ARCH_NVGR_SIZE)) <
            (((xwptr_t)stkbtn) + ((XWOSPL_STACK_WATERMARK) * sizeof(xwstk_t)))) {
                arch_skd_report_stk_overflow(pstk);
        }
#  else
        if ((psp - ARCH_NVGR_SIZE) <
            (((xwptr_t)stkbtn) + ((XWOSPL_STACK_WATERMARK) * sizeof(xwstk_t)))) {
                arch_skd_report_stk_overflow(pstk);
        }
#  endif
#else
        xwskd = xwosplcb_skd_get_lc();
#endif
        return xwskd;
}

/**
 * @brief 异常时检查当前线程的栈溢出
 * @return 调度器的指针
 */
__xwbsp_code
struct xwospl_skd * arch_skd_chk_stk(void)
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
        cm_get_psp(&stk.value);
#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
        if ((stk.value - (ARCH_NVFR_SIZE + ARCH_NVGR_SIZE)) <
            (((xwptr_t)stkbtn) + ((XWOSPL_STACK_WATERMARK) * sizeof(xwstk_t)))) {
                arch_skd_report_stk_overflow(cstk);
        }
#else
        if ((stk.value - ARCH_NVGR_SIZE) <
            (((xwptr_t)stkbtn) + ((XWOSPL_STACK_WATERMARK) * sizeof(xwstk_t)))) {
                arch_skd_report_stk_overflow(cstk);
        }
#endif
        for (i = 0; i < XWOSPL_STACK_WATERMARK; i++) {
                if (0xFFFFFFFFU != stkbtn[i]) {
                        arch_skd_report_stk_overflow(cstk);
                }
        }
        return xwskd;
}

/**
 * @brief 报告栈溢出
 * @param[in] stk: 溢出的栈
 */
static __xwbsp_code
void arch_skd_report_stk_overflow(struct xwospl_skd_stack_info * stk)
{
        XWOS_UNUSED(stk);
        XWOS_BUG();
}
