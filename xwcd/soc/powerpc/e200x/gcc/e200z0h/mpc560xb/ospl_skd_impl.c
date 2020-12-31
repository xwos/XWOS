/**
 * @file
 * @brief 玄武OS内核适配代码：调度器
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
#include <e200x_core.h>
#include <arch_sc_trap.h>
#include <soc.h>

static __xwbsp_code
xwer_t soc_skd_swi(xwer_t(* swifunc)(void *, void *), void * arg0, void * arg1);

static __xwbsp_code
void soc_skd_report_stk_overflow(struct xwospl_skd_stack_info * stk);

extern xwstk_t xwos_stk_top[];
extern xwstk_t _SDA_BASE_[];
extern xwstk_t _SDA2_BASE_[];

__xwbsp_rodata const struct soc_irq_cfg soc_skd_irq_cfg[] = {
        [0] = {
                .priority = SOC_SWCX_PRIO,
        },
        [1] = {
                .priority = SOC_SKD_SWI_PRIO,
        },
};

__xwbsp_rodata const struct xwos_irq_resource soc_skd_irqrsc[] = {
        [0] = {
                .irqn = SOC_SWCX_IRQN,
                .isr = soc_skd_isr_swcx,
                .cfg = &soc_skd_irq_cfg[0],
                .description = "arch_switch_context_irq",
        },
        [1] = {
                .irqn = SOC_SKD_SWI_IRQN,
                .isr = soc_skd_isr_swi,
                .cfg = &soc_skd_irq_cfg[1],
                .description = "soc_skd_swi_irq",
        },
};

__xwbsp_data struct e200x_context soc_context = {
        .isr_sp = xwos_stk_top,
        .thd_sp = NULL,
        .irq_nesting_cnt = 0,
        .irqn = SOC_IRQN_NIL,
};

/**
 * @brief 获取调度器结构体指针
 * @return 调度器的指针
 */
__xwbsp_code
struct xwospl_skd * soc_skd_get_lc(void)
{
#if defined(XuanWuOS_CFG_CORE__mp)
        return xwmp_skd_get_lc();
#elif defined(XuanWuOS_CFG_CORE__up)
        return xwup_skd_get_lc();
#else
  #error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif
}

/**
 * @brief 触发调度软中断
 * @param swifunc: (I) 软中断中要执行的函数
 * @param arg0: (I) 参数1
 * @param arg1: (I) 参数2
 * @return 被执行的函数的返回值
 */
static __xwbsp_code
xwer_t soc_skd_swi(xwer_t(* swifunc)(void *, void *), void * arg0, void * arg1)
{
        xwer_t rc;
        xwreg_t value;
        xwptr_t addr;

        rc = XWOK;
        value = 0x2U;
        addr = (xwptr_t)&INTC.SSCIR[1];
        asm volatile(
        "       se_mr   3, %[__arg0]\n"
        "       se_mr   4, %[__arg1]\n"
        "       se_mr   5, %[__swifunc]\n"
        "       se_stb  %[__value], 0 (%[__addr])\n"
        "       se_mr   %[__rc], 3\n"
        : [__rc] "=&r" (rc)
        : [__arg0] "r" (arg0),
          [__arg1] "r" (arg1),
          [__swifunc] "r" (swifunc),
          [__value] "r" (value),
          [__addr] "r" (addr)
        : "r3", "r4", "r5", "memory"
        );
        return rc;
}

/**
 * @brief SOC Adaptation Function：初始化调度调度器
 * @param xwskd: (I) 调度器的指针
 * @return 错误码
 */
__xwbsp_init_code
xwer_t xwospl_skd_init(struct xwospl_skd * xwskd)
{
        const struct xwos_irq_resource * irqrsc;
        xwsq_t i;
        xwer_t rc;

        XWOS_UNUSED(xwskd);
        for (i = 0; i < xw_array_size(soc_skd_irqrsc); i++) {
                irqrsc = &soc_skd_irqrsc[i];
                rc = xwos_irq_request(irqrsc->irqn, irqrsc->isr,
                                      XWOS_UNUSED_ARGUMENT, NULL);
                SDL_BUG_ON(rc < 0);

                rc = xwos_irq_cfg(irqrsc->irqn, irqrsc->cfg);
                SDL_BUG_ON(rc < 0);

                rc = xwos_irq_enable(irqrsc->irqn);
                SDL_BUG_ON(rc < 0);
        }
        return rc;
}

/**
 * @brief 玄武OS内核调度器适配函数：获取当前CPU的ID
 */
__xwbsp_code
xwid_t xwospl_skd_id_lc(void)
{
        return 0;
}

/**
 * @brief SOC Adaptation Function：初始化调度对象的栈
 * @param stk: (I) 栈信息结构体指针
 * @param attr: (I) 调度对象属性
 */
__xwbsp_code
void xwospl_skd_init_stack(struct xwospl_skd_stack_info * stk,
                           void (* exit)(xwer_t),
                           xwsq_t attr)
{
        bool privileged;
        union msr_reg msr;
        xwu32_t * stkbtn;
        xwsq_t i;

#if defined(XuanWuOS_CFG_CORE__mp)
        privileged = !!(attr & XWMP_SKDATTR_PRIVILEGED);
#elif defined(XuanWuOS_CFG_CORE__up)
        privileged = !!(attr & XWUP_SKDATTR_PRIVILEGED);
#endif
        asm volatile(
        "       mfmsr           %[__msr]"
        : [__msr] "=&r" (msr.w)
        :
        : "memory"
        );
        msr.b.ee = 1; /* enable external interrupt */
        if (privileged) {
                msr.b.pr = 0; /* privileged */
        } else {
                msr.b.pr = 1; /* unprivileged */
        }

        /* reserved frame (used by compiler) 8*4 = 32 bytes */
        stk->sp -= 8;
        /* volatile registers stack frame */
        stk->sp--;
        *(stk->sp) = (xwstk_t)12; /* r12 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)11; /* r11 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)10; /* r10 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)9; /* r9 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)8; /* r8 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)7; /* r7 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)6; /* r6 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)5; /* r5 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)4; /* r4 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->arg; /* r3 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* r0 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* cr */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* xer */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* ctr */
        stk->sp--;
        *(stk->sp) = (xwstk_t)exit; /* lr */
        stk->sp--;
        *(stk->sp) = (xwstk_t)msr.w; /* srr1 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)stk->main; /* srr0 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)SOC_IRQN_NIL; /* IRQn */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* reserved */
        stk->sp--;
        *(stk->sp) = (xwstk_t)0; /* reserved */

        /* non-volatile registers stack frame */
        stk->sp--;
        *(stk->sp) = (xwstk_t)31; /* r31 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)30; /* r30 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)29; /* r29 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)28; /* r28 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)27; /* r27 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)26; /* r26 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)25; /* r25 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)24; /* r24 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)23; /* r23 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)22; /* r22 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)21; /* r21 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)20; /* r20 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)19; /* r19 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)18; /* r18 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)17; /* r17 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)16; /* r16 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)15; /* r15 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)14; /* r14 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)_SDA_BASE_; /* r13 */
        stk->sp--;
        *(stk->sp) = (xwstk_t)_SDA2_BASE_; /* r2 */

        stkbtn = (xwu32_t *)stk->base;
        for (i = 0; i < 16; i++) {
                stkbtn[i] = 0xFFFFFFFFU;
        }
}

/**
 * @brief SOC Adaptation Function：触发切换上下文的软中断
 * @param xwskd: (I) XWOS调度器的指针
 */
__xwbsp_code
void soc_skd_req_swcx(struct xwospl_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        INTC.SSCIR[SOC_SWCX_CH].R = 0x2;
}

/**
 * @brief 异常时检查当前线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwospl_skd * soc_skd_chkcstk(void)
{
        struct xwospl_skd * xwskd;
        struct xwospl_skd_stack_info * cstk;
        union {
                xwstk_t * ptr;
                xwptr_t value;
        } stk;

        xwskd = soc_skd_get_lc();
        cstk = xwskd->cstk;
        stk.ptr = soc_context.thd_sp;
        if ((stk.value - 0x50) < (xwptr_t)cstk->base) {
                soc_skd_report_stk_overflow(cstk);
        }
        return xwskd;
}

/**
 * @brief 切换线程上下文时检查前一个线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwospl_skd * soc_skd_chkpstk(void)
{
        struct xwospl_skd * xwskd;
        struct xwospl_skd_stack_info * pstk;
        union {
                xwstk_t * ptr;
                xwptr_t value;
        } stk;

        xwskd = soc_skd_get_lc();
        pstk = xwskd->pstk;
        stk.ptr = soc_context.thd_sp;
        if ((stk.value - 0x50) < (xwptr_t)pstk->base) {
                soc_skd_report_stk_overflow(pstk);
        }
        return xwskd;
}

/**
 * @brief 报告栈溢出
 * @param stk: (I) 溢出的栈
 */
static __xwbsp_code
void soc_skd_report_stk_overflow(struct xwospl_skd_stack_info * stk)
{
        XWOS_UNUSED(stk);
        SDL_BUG();
}

/**
 * @brief 玄武OS内核调度器适配函数：暂停调度器
 * @param xwskd: (I) 调度器的指针
 * @return 错误码
 */
__xwbsp_code
xwer_t xwospl_skd_suspend(struct xwospl_skd * xwskd)
{
        xwer_t rc;

#if defined(XuanWuOS_CFG_CORE__mp)
        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwmp_skd_suspend_lic,
                         (void *)xwskd, (void *)XWOS_UNUSED_ARGUMENT);
#elif defined(XuanWuOS_CFG_CORE__up)
        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwup_skd_suspend_lic,
                         (void *)xwskd, (void *)XWOS_UNUSED_ARGUMENT);
#endif
        return rc;
}

/**
 * @brief 玄武OS内核调度器适配函数：继续调度器
 * @param xwskd: (I) 调度器的指针
 * @return 错误码
 */
__xwbsp_code
xwer_t xwospl_skd_resume(struct xwospl_skd * xwskd)
{
        xwer_t rc;

#if defined(XuanWuOS_CFG_CORE__mp)
        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwmp_skd_resume_lic,
                         (void *)xwskd, (void *)XWOS_UNUSED_ARGUMENT);
#elif defined(XuanWuOS_CFG_CORE__up)
        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwup_skd_resume_lic,
                         (void *)xwskd, (void *)XWOS_UNUSED_ARGUMENT);
#endif
        return rc;
}

/**
 * @brief 玄武OS内核调度器适配函数：本地CPU上的线程退出
 * @param thd: (I) 线程控制块对象的指针
 * @param rc: (I) 线程退出抛出的返回值
 */
__xwbsp_code
void xwospl_thd_exit_lc(struct xwospl_thd * thd, xwer_t rc)
{
#if defined(XuanWuOS_CFG_CORE__mp)
        soc_skd_swi((xwer_t(*)(void *, void *))xwmp_thd_exit_lic,
                    (void *)thd, (void *)rc);
#elif defined(XuanWuOS_CFG_CORE__up)
        soc_skd_swi((xwer_t(*)(void *, void *))xwup_thd_exit_lic,
                    (void *)thd, (void *)rc);
#endif
        while (true) {
        }
}

/**
 * @brief 玄武OS内核调度器适配函数：冻结本地CPU中正在运行的线程
 * @param thd: (I) 线程控制块对象的指针
 */
__xwbsp_code
xwer_t xwospl_thd_freeze_lc(struct xwospl_thd * thd)
{
        xwer_t rc;

#if defined(XuanWuOS_CFG_CORE__mp)
        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwmp_thd_freeze_lic,
                         (void *)thd, (void *)XWOS_UNUSED_ARGUMENT);
#elif defined(XuanWuOS_CFG_CORE__up)
        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwup_thd_freeze_lic,
                         (void *)thd, (void *)XWOS_UNUSED_ARGUMENT);
#endif
        return rc;
}

#if defined(XuanWuOS_CFG_CORE__mp)
/**
 * @brief 玄武OS内核调度器适配函数：将线程迁出其他CPU，并准备迁入其他CPU
 * @param thd: (I) 线程控制块对象的指针
 * @param cpuid: (I) 目的地CPU的ID
 * @return 错误码
 */
__xwbsp_code
xwer_t xwospl_thd_outmigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        xwer_t rc;

        XWOS_UNUSED(thd);
        XWOS_UNUSED(cpuid);

        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwmp_thd_outmigrate_lic,
                         (void *)thd, (void *)cpuid);
        return rc;
}

/**
 * @brief 玄武OS内核调度器适配函数：迁移线程至目标CPU
 * @param thd: (I) 线程控制块对象的指针
 * @param cpuid: (I) 目的地CPU的ID
 */
__xwbsp_code
void xwospl_thd_immigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        XWOS_UNUSED(cpuid);
        xwmp_thd_immigrate_lic(thd);
}
#endif /* XuanWuOS_CFG_CORE__mp */