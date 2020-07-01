/**
 * @file
 * @brief 架构描述层：Scheduler Adaptation LIB
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
  #include <xwos/smp/thread.h>
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/scheduler.h>
  #include <xwos/up/thread.h>
#endif
#include <armv6m_core.h>
#include <arch_sched.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCH_IRQ_SWCX_PRIO                              (ARCH_IRQ_PRIO_LOWEST)

#define XPSR_INITVAL                                    0x01000000U
#define FPSCR_INITVAL                                   0X00000000U
#define EXC_RETURN_HANDLE_MODE_BASIC                    0xFFFFFFF1U
#define EXC_RETURN_THREAD_MODE_PSP_BASIC                0xFFFFFFFDU
#define EXC_RETURN_THREAD_MODE_MSP_BASIC                0xFFFFFFF9U

#define ARCH_NVGR_SIZE                                  36U

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern xwstk_t xwos_stk_top[];

__xwbsp_rodata const struct soc_irq_cfg cortex_m_swcx_irq_cfg = {
        .irqcfg = {
                .priority = ARCH_IRQ_SWCX_PRIO,
        },
};

__xwbsp_rodata const struct xwos_irq_resource cortex_m_swcx_irqrsc = {
        .irqn = ARCH_IRQ_PENDSV,
        .isr = arch_scheduler_isr_swcx,
        .cfg = &cortex_m_swcx_irq_cfg,
        .description = "rsc.irq.swcx.armv6m",
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_code
void arch_scheduler_report_stk_overflow(struct xwos_sdobj_stack_info * stk);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief ADL BSP LIB：初始化切换上下文中断
 * @param xwsd: (I) 调度器的指针
 * @return 错误码
 * @note
 * - 使用PendSV作为线程上下文切换的中断
 */
__xwbsp_code
xwer_t arch_scheduler_lib_init_pendsv(struct xwos_scheduler * xwsd)
{
        xwer_t rc;
        const struct xwos_irq_resource * irqrsc;

        XWOS_UNUSED(xwsd);

        irqrsc = &cortex_m_swcx_irqrsc;
        rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr, XWOS_UNUSED_ARGUMENT, NULL);
        XWOS_BUG_ON(rc < 0);

        rc = xwos_irq_cfg(irqrsc->irqn, irqrsc->cfg);
        XWOS_BUG_ON(rc < 0);

        rc = xwos_irq_enable(irqrsc->irqn);
        XWOS_BUG_ON(rc < 0);

        return rc;
}

/**
 * @brief ADL BSP LIB：初始化调度对象的栈
 * @param stk: (I) 栈信息结构体指针
 * @param attr: (I) 调度对象属性
 */
__xwbsp_code
void arch_scheduler_lib_init_sdobj_stack(struct xwos_sdobj_stack_info * stk,
                                         xwsq_t attr)
{
        bool privileged;

        privileged = !!(attr & XWSDOBJ_ATTR_PRIVILEGED);

        /* volatile registers */
        stk->sp--;
        *(stk->sp) = (xwstk_t)XPSR_INITVAL; /* xpsr */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->main; /* pc */
        stk->sp--;
        *(stk->sp) = (xwstk_t)xwos_cthrd_exit; /* lr value to exit */
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
 * @brief ADL ISR：启动调度器的SVC(svc 0)的处理函数
 * @param xwsd: (I) XWOS调度器的指针
 */
__xwbsp_isr __naked
void arch_scheduler_svcsr_start(__maybe_unused struct xwos_scheduler * xwsd)
{
        /* r3 = r0->cstk; */
        __asm__ volatile("      ldr     r3, [r0, #0]");
        /* get value of stack pointer, r2 = cstk->sp */
        __asm__ volatile("      ldr     r2, [r3, #0]");
        /* restore CONTROL */
        __asm__ volatile("      ldr     r1, [r2, #0]");
        __asm__ volatile("      msr     control, r1");
        /* restore non-volatile register: r8-fp */
        __asm__ volatile("      add     r2, #20");
        __asm__ volatile("      ldmia   r2!, {r4-r7}");
        __asm__ volatile("      mov     fp, r7");
        __asm__ volatile("      mov     sl, r6");
        __asm__ volatile("      mov     r9, r5");
        __asm__ volatile("      mov     r8, r4");
        /* set PSP */
        __asm__ volatile("      msr     psp, r2");
        /* restore non-volatile register: r4-r7 */
        __asm__ volatile("      sub     r2, #32");
        __asm__ volatile("      ldmfd   r2!, {r4-r7}");
        __asm__ volatile("      msr     msp, %[__xwos_stk_top]"
                         :
                         : [__xwos_stk_top] "Ir" (xwos_stk_top)
                         :);
        __asm__ volatile("      mov     r0, %[__exc]\n"
                         "      mov     lr, r0\n"
                         :
                         : [__exc] "rI" (EXC_RETURN_THREAD_MODE_PSP_BASIC)
                         : "r0", "lr");
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
        __asm__ volatile("      bx      lr");
}

/**
 * @brief ADL ISR：切换上下文的中断(PendSV)的处理函数
 * @note
 * - 此中断为系统中最低优先级的中断。
 */
__xwbsp_isr __naked
void arch_scheduler_isr_swcx(void)
{
        /* get xwos_scheduler, */
        __asm__ volatile("      push    {lr}");
        __asm__ volatile("      sub     sp, #4");
        __asm__ volatile("      bl      arch_scheduler_lib_chkpstk");
        /* get PSP */
        __asm__ volatile("      mrs     r2, psp");
        /* save previous context */
        /* r3 = r0->pstk; */
        __asm__ volatile("      ldr     r3, [r0, #4]");
        /* setup stack frame */
        __asm__ volatile("      sub     r2, %[__nvgr]"
                         :
                         : [__nvgr] "I" (ARCH_NVGR_SIZE)
                         :);
        /* save PSP */
        __asm__ volatile("      str     r2, [r3, #0]");
        /* save control reg */
        __asm__ volatile("      mrs     r1, control");
        __asm__ volatile("      str     r1, [r2, #0]");
        __asm__ volatile("      add     r2, #4");
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
        __asm__ volatile("      ldr     r3, [r0, #0]");
        /* get value of stack pointer, r2 = cstk->sp */
        __asm__ volatile("      ldr     r2, [r3, #0]");
        /* restore CONTROL */
        __asm__ volatile("      ldr     r1, [r2, #0]");
        __asm__ volatile("      msr     control, r1");
        /* restore non-volatile register: r8-fp */
        __asm__ volatile("      add     r2, #20");
        __asm__ volatile("      ldmia   r2!, {r4-r7}");
        __asm__ volatile("      mov     fp, r7");
        __asm__ volatile("      mov     sl, r6");
        __asm__ volatile("      mov     r9, r5");
        __asm__ volatile("      mov     r8, r4");
        /* set PSP */
        __asm__ volatile("      msr     psp, r2");
        /* restore non-volatile register: r4-r7 */
        __asm__ volatile("      sub     r2, #32");
        __asm__ volatile("      ldmia   r2!, {r4-r7}");

        /* finish the progress */
        __asm__ volatile("      dsb");
        __asm__ volatile("      isb");
        /* xwos_scheduler_finish_swcx(r0) */
        __asm__ volatile("      bl      xwos_scheduler_finish_swcx");
        __asm__ volatile("      add     sp, #4");
        __asm__ volatile("      pop     {pc}");
}

/**
 * @brief ADL BSP LIB：异常时检查当前线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwos_scheduler * arch_scheduler_lib_chkcstk(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_sdobj_stack_info * cstk;
        union {
                xwstk_t * ptr;
                xwptr_t value;
        } stk;

        xwsd = xwos_scheduler_get_lc();
        cstk = xwsd->cstk;
        cm_get_psp(&stk.value);
        if ((stk.value - ARCH_NVGR_SIZE) < (xwptr_t)cstk->base) {
                arch_scheduler_report_stk_overflow(cstk);
        }/* else {} */
        return xwsd;
}

/**
 * @brief ADL BSP LIB：切换线程上下文时检查前一个线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwos_scheduler * arch_scheduler_lib_chkpstk(void)
{
        struct xwos_scheduler * xwsd;
        struct xwos_sdobj_stack_info * pstk;
        union {
                xwstk_t *ptr;
                xwptr_t value;
        } stk;

        xwsd = xwos_scheduler_get_lc();
        pstk = xwsd->pstk;
        cm_get_psp(&stk.value);
        if ((stk.value - ARCH_NVGR_SIZE) < (xwptr_t)pstk->base) {
                arch_scheduler_report_stk_overflow(pstk);
        }/* else {} */
        return xwsd;
}

/**
 * @brief 报告栈溢出
 * @param stk: (I) 溢出的栈
 */
static __xwbsp_code
void arch_scheduler_report_stk_overflow(struct xwos_sdobj_stack_info * stk)
{
        XWOS_UNUSED(stk);
        XWOS_BUG();
}
