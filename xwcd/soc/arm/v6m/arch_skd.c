/**
 * @file
 * @brief 架构描述层：调度器
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

#include <xwos/standard.h>
#include <xwos/osal/irq.h>
#include <xwos/ospl/skd.h>
#include <armv6m_isa.h>
#include <arch_skd.h>

#define SOC_EXC_SWCX_PRIO                               (SOC_IRQ_PRIO_LOWEST)

#define XPSR_INITVAL                                    0x01000000U
#define FPSCR_INITVAL                                   0X00000000U
#define EXC_RETURN_HANDLE_MODE_BASIC                    0xFFFFFFF1U
#define EXC_RETURN_THREAD_MODE_PSP_BASIC                0xFFFFFFFDU
#define EXC_RETURN_THREAD_MODE_MSP_BASIC                0xFFFFFFF9U

#define ARCH_NVGR_SIZE                                  36U

extern xwstk_t armv6m_isr_stack_top[];

static __xwbsp_code
void arch_skd_report_stk_overflow(struct xwospl_skdobj_stack * stk);

/**
 * @brief 初始化切换上下文中断
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 * @note
 * - 使用PendSV作为线程上下文切换的中断
 */
__xwbsp_code
void arch_skd_init_pendsv(void)
{
        cm_nvic_set_sysirq_priority(SOC_EXC_PENDSV, SOC_EXC_SWCX_PRIO);
}

/**
 * @brief 初始化调度对象的栈
 * @param[in] stk: 栈信息结构体指针
 * @param[in] exit: 退出函数
 */
__xwbsp_code
void arch_skd_init_stack(struct xwospl_skdobj_stack * stk, void (* exit)(xwer_t))
{
        bool privileged;
        xwstk_t * stkbtn;
        xwsq_t i, stknum;

        privileged = !!(stk->flag & XWOSPL_SKDOBJ_FLAG_PRIVILEGED);

        stkbtn = (xwstk_t *)stk->base;
        stknum = stk->size / sizeof(xwstk_t);
        for (i = 0; i < stknum; i++) {
                stkbtn[i] = 0xFFFFFFFFU;
        }

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
                stk->sp--;
                *(stk->sp) = (xwstk_t)0x2; /* CONTROL reg: privileged access, psp */
        } else {
                stk->sp--;
                *(stk->sp) = (xwstk_t)0x3; /* CONTROL: unprivileged access, psp */
        }
}

/**
 * @brief 启动调度器的SVC(svc 0)的处理函数
 * @param[in] xwskd: 调度器的指针
 */
__xwbsp_isr __xwcc_naked
void arch_svc_skd_start(__xwcc_unused struct xwospl_skd * xwskd)
{
        /* r3 = r0->cstk; */
        __asm__ volatile(".syntax       unified");
        /* get xwskd->cstk */
        __asm__ volatile("      ldr     r3, [r0, %[__cstk]]"
                         :
                         :[__cstk] "I" (xwcc_offsetof(struct xwospl_skd, cstk))
                         : "r0", "r3");
        /* get value of stack pointer, r2 = cstk->sp */
        __asm__ volatile("      ldr     r2, [r3, %[__sp]]"
                         :
                         :[__sp] "I" (xwcc_offsetof(struct xwospl_skdobj_stack, sp))
                         : "r2", "r3");
        /* restore CONTROL */
        __asm__ volatile("      ldr     r1, [r2, #0]");
        __asm__ volatile("      msr     control, r1");
        /* restore non-volatile register: r8-fp */
        __asm__ volatile("      adds    r2, #20");
        __asm__ volatile("      ldmia   r2!, {r4-r7}");
        __asm__ volatile("      mov     fp, r7");
        __asm__ volatile("      mov     sl, r6");
        __asm__ volatile("      mov     r9, r5");
        __asm__ volatile("      mov     r8, r4");
        /* set PSP */
        __asm__ volatile("      msr     psp, r2");
        /* restore non-volatile register: r4-r7 */
        __asm__ volatile("      subs    r2, #32");
        __asm__ volatile("      ldmfd   r2!, {r4-r7}");
        __asm__ volatile("      msr     msp, %[__armv6m_isr_stack_top]"
                         :
                         : [__armv6m_isr_stack_top] "Ir" (armv6m_isr_stack_top)
                         :);
        __asm__ volatile("      mov     r0, %[__exc]\n"
                         "      mov     lr, r0\n"
                         :
                         : [__exc] "rI" (EXC_RETURN_THREAD_MODE_PSP_BASIC)
                         : "r0", "lr");

        __asm__ volatile("      push    {lr}");
        __asm__ volatile("      sub     sp, #4");
        __asm__ volatile("      bl      xwosplcb_skd_post_start_lic");
        __asm__ volatile("      add     sp, #4");
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
        __asm__ volatile("      pop     {pc}");
}

/**
 * @brief 切换上下文的中断(PendSV)的处理函数
 * @note
 * - 此中断为系统中最低优先级的中断。
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

        /* get PSP */
        __asm__ volatile("      mrs     r2, psp");
        /* save previous context */
        /* r3 = r0->pstk; */
        __asm__ volatile("      ldr     r3, [r0, %[__pstk]]"
                         :
                         :[__pstk] "I" (xwcc_offsetof(struct xwospl_skd, pstk))
                         : "r0", "r3");
        /* setup stack frame */
        __asm__ volatile("      subs    r2, %[__nvgr]"
                         :
                         : [__nvgr] "I" (ARCH_NVGR_SIZE)
                         : "r0");
        /* save PSP, pstk->sp = r2; */
        __asm__ volatile("      str     r2, [r3, %[__sp]]"
                         :
                         :[__sp] "I" (xwcc_offsetof(struct xwospl_skdobj_stack, sp))
                         : "r2", "r3");
        /* save control reg */
        __asm__ volatile("      mrs     r1, control");
        __asm__ volatile("      str     r1, [r2, #0]");
        __asm__ volatile("      adds    r2, #4");
        /* save non-volatile registers: r4-r7 */
        __asm__ volatile("      stmia   r2!, {r4-r7}");
        /* save non-volatile registers: r8-fp */
        __asm__ volatile("      mov     r4, r8");
        __asm__ volatile("      mov     r5, r9");
        __asm__ volatile("      mov     r6, sl");
        __asm__ volatile("      mov     r7, fp");
        __asm__ volatile("      stmia   r2!, {r4-r7}");

        /* restore current context */
        /* r3 = r0->cstk; */
        __asm__ volatile("      ldr     r3, [r0, %[__cstk]]"
                         :
                         :[__cstk] "I" (xwcc_offsetof(struct xwospl_skd, cstk))
                         : "r0", "r3");
        /* get value of stack pointer, r2 = cstk->sp */
        __asm__ volatile("      ldr     r2, [r3, %[__sp]]"
                         :
                         :[__sp] "I" (xwcc_offsetof(struct xwospl_skdobj_stack, sp))
                         : "r2", "r3");
        /* restore CONTROL */
        __asm__ volatile("      ldr     r1, [r2, #0]");
        __asm__ volatile("      msr     control, r1");
        /* restore non-volatile register: r8-fp */
        __asm__ volatile("      adds    r2, #20");
        __asm__ volatile("      ldmia   r2!, {r4-r7}");
        __asm__ volatile("      mov     fp, r7");
        __asm__ volatile("      mov     sl, r6");
        __asm__ volatile("      mov     r9, r5");
        __asm__ volatile("      mov     r8, r4");
        /* set PSP */
        __asm__ volatile("      msr     psp, r2");
        /* restore non-volatile register: r4-r7 */
        __asm__ volatile("      subs    r2, #32");
        __asm__ volatile("      ldmia   r2!, {r4-r7}");

        /* finish the progress */
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
        __asm__ volatile("      bl      xwosplcb_skd_post_swcx_lic");
        __asm__ volatile("      add     sp, #4");
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
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
        struct xwospl_skdobj_stack * pstk;
        xwstk_t * stkbtn;
        xwsz_t guard;
        xwptr_t psp;

        xwskd = xwosplcb_skd_get_lc();
        pstk = xwskd->pstk;
        stkbtn = pstk->guard_base;
        guard = pstk->guard;
        cm_get_psp(&psp);
        if ((psp - ARCH_NVGR_SIZE) < ((xwptr_t)stkbtn + guard)) {
                arch_skd_report_stk_overflow(pstk);
        }
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
        struct xwospl_skdobj_stack * cstk;
        union {
                xwstk_t * ptr;
                xwptr_t value;
        } stk;
        xwstk_t * stkbtn;
        xwsz_t guard;
        xwsz_t i;

        xwskd = xwosplcb_skd_get_lc();
        cstk = xwskd->cstk;
        stkbtn = cstk->guard_base;
        guard = cstk->guard;
        cm_get_psp(&stk.value);
        if ((stk.value - ARCH_NVGR_SIZE) < ((xwptr_t)stkbtn + guard)) {
                arch_skd_report_stk_overflow(cstk);
        }
        for (i = 0; i < (guard / sizeof(xwstk_t)); i++) {
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
void arch_skd_report_stk_overflow(struct xwospl_skdobj_stack * stk)
{
        XWOS_UNUSED(stk);
        XWOS_BUG();
}
