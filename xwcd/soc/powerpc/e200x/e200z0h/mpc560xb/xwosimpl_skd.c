/**
 * @file
 * @brief XWOS移植实现层：调度
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
#include <e200x_isa.h>
#include <arch_sc_trap.h>
#include <soc.h>
#include <soc_irq.h>

#define SOC_STKFRAME_SIZE               0x50

xwer_t soc_skd_swi(xwer_t(* swifunc)(void *, void *), void * arg0, void * arg1);

static __xwbsp_code
void soc_skd_report_stk_overflow(struct xwospl_skdobj_stack * stk);

extern xwstk_t e200z0h_isr_stack_top[];
extern xwstk_t _SDA_BASE_[];
extern xwstk_t _SDA2_BASE_[];

__xwbsp_rodata const struct soc_irq_cfg soc_skd_irq_cfg_swcx = {
        .priority = SOC_SWCX_PRIO,
};

__xwbsp_rodata const struct soc_irq_cfg soc_skd_irq_cfg_swi = {
        .priority = SOC_SKD_SWI_PRIO,
};

__xwbsp_data struct e200x_context soc_context = {
        .isr_sp = e200z0h_isr_stack_top,
        .thd_sp = NULL,
        .irq_nesting_cnt = 0,
        .irqn = SOC_IRQN_NIL,
};

/**
 * @brief 初始化调度调度器
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 */
__xwbsp_init_code
xwer_t xwospl_skd_init(struct xwospl_skd * xwskd)
{
        xwer_t rc;

        XWOS_UNUSED(xwskd);

        rc = soc_irq_request(SOC_SWCX_IRQN, xwospl_skd_isr_swcx, XWOS_UNUSED_ARGUMENT);
        SDL_BUG_ON(rc < 0);
        rc = soc_irq_cfg(SOC_SWCX_IRQN, &soc_skd_irq_cfg_swcx);
        SDL_BUG_ON(rc < 0);
        rc = xwos_irq_enable(SOC_SWCX_IRQN);
        SDL_BUG_ON(rc < 0);

        rc = soc_irq_request(SOC_SKD_SWI_IRQN, soc_skd_isr_swi, XWOS_UNUSED_ARGUMENT);
        SDL_BUG_ON(rc < 0);
        rc = soc_irq_cfg(SOC_SKD_SWI_IRQN, &soc_skd_irq_cfg_swi);
        SDL_BUG_ON(rc < 0);
        rc = xwos_irq_enable(SOC_SKD_SWI_IRQN);
        SDL_BUG_ON(rc < 0);

        return XWOK;
}

/**
 * @brief 获取当前CPU的ID
 */
__xwbsp_code
xwid_t xwospl_skd_id_lc(void)
{
        return 0;
}

/**
 * @brief 初始化调度对象的栈
 * @param[in] stk: 栈信息结构体指针
 */
__xwbsp_code
void xwospl_skd_init_stack(struct xwospl_skdobj_stack * stk,
                           void (* exit)(xwer_t))
{
        bool privileged;
        union msr_reg msr;
        xwstk_t * stkbtn;
        xwsq_t i, stknum;

        privileged = !!(stk->flag & XWOSPL_SKDOBJ_FLAG_PRIVILEGED);

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

        stkbtn = (xwstk_t *)stk->base;
        stknum = stk->size / sizeof(xwstk_t);
        for (i = 0; i < stknum; i++) {
                stkbtn[i] = 0xFFFFFFFFU;
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
}

/**
 * @brief 触发切换上下文的软中断
 * @param[in] xwskd: XWOS调度器的指针
 */
__xwbsp_code
void soc_skd_req_swcx(struct xwospl_skd * xwskd)
{
        XWOS_UNUSED(xwskd);
        INTC.SSCIR[SOC_SWCX_CH].R = 0x2;
        eppc_dmb();
        eppc_isb();
}

/**
 * @brief 切换线程上下文时检查线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwospl_skd * soc_skd_chk_swcx(void)
{
#if defined(XWMMCFG_STACK_CHK_SWCX) && (1 == XWMMCFG_STACK_CHK_SWCX)
        struct xwospl_skd * xwskd;
        struct xwospl_skdobj_stack * pstk;
        xwstk_t * stkbtn;
        xwsz_t guard;
        xwptr_t sp;

        xwskd = xwosplcb_skd_get_lc();
        pstk = xwskd->pstk;
        stkbtn = (xwstk_t *)pstk->base;
        guard = pstk->guard;
        sp = (xwptr_t)soc_context.thd_sp;
        if ((sp - SOC_STKFRAME_SIZE) < ((xwptr_t)stkbtn + guard)) {
                soc_skd_report_stk_overflow(pstk);
        }
        return xwskd;
#else
        return xwosplcb_skd_get_lc();
#endif
}

/**
 * @brief 异常时检查当前线程的栈溢出
 * @return XWOS调度器的指针
 */
__xwbsp_code
struct xwospl_skd * soc_skd_chk_stk(void)
{
        struct xwospl_skd * xwskd;
        struct xwospl_skdobj_stack * cstk;
        xwstk_t * stkbtn;
        xwsz_t guard;
        xwptr_t sp;
        xwsz_t i;

        xwskd = xwosplcb_skd_get_lc();
        cstk = xwskd->cstk;
        stkbtn = (xwstk_t *)cstk->base;
        guard = cstk->guard;
        sp = (xwptr_t)soc_context.thd_sp;
        if ((sp - SOC_STKFRAME_SIZE) < ((xwptr_t)stkbtn + guard)) {
                soc_skd_report_stk_overflow(cstk);
        }
        for (i = 0; i < (guard / sizeof(xwstk_t)); i++) {
                if (0xFFFFFFFFU != stkbtn[i]) {
                        soc_skd_report_stk_overflow(cstk);
                }
        }
        return xwskd;
}

/**
 * @brief 报告栈溢出
 * @param[in] stk: 溢出的栈
 */
static __xwbsp_code
void soc_skd_report_stk_overflow(struct xwospl_skdobj_stack * stk)
{
        XWOS_UNUSED(stk);
        SDL_BUG();
}

/**
 * @brief 暂停调度器
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 */
__xwbsp_code
xwer_t xwospl_skd_suspend(struct xwospl_skd * xwskd)
{
        xwer_t rc;

        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwosplcb_skd_suspend_lic,
                         (void *)xwskd, (void *)XWOS_UNUSED_ARGUMENT);
        return rc;
}

/**
 * @brief 继续调度器
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 */
__xwbsp_code
xwer_t xwospl_skd_resume(struct xwospl_skd * xwskd)
{
        xwer_t rc;

        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwosplcb_skd_resume_lic,
                         (void *)xwskd, (void *)XWOS_UNUSED_ARGUMENT);
        return rc;
}

/**
 * @brief 本地CPU上的线程退出
 * @param[in] thd: 线程控制块对象的指针
 * @param[in] rc: 线程退出抛出的返回值
 */
__xwbsp_code
void xwospl_thd_exit_lc(struct xwospl_thd * thd, xwer_t rc)
{
        soc_skd_swi((xwer_t(*)(void *, void *))xwosplcb_thd_exit_lic,
                    (void *)thd, (void *)rc);
        while (true) {
        }
}

/**
 * @brief 冻结本地CPU中正在运行的线程
 * @param[in] thd: 线程控制块对象的指针
 */
__xwbsp_code
xwer_t xwospl_thd_freeze_lc(struct xwospl_thd * thd)
{
        xwer_t rc;

        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwosplcb_thd_freeze_lic,
                         (void *)thd, (void *)XWOS_UNUSED_ARGUMENT);
        return rc;
}

#if (1 == XWOSRULE_THD_MIGRATION)
/**
 * @brief 将线程迁出其他CPU，并准备迁入其他CPU
 * @param[in] thd: 线程控制块对象的指针
 * @param[in] cpuid: 目的地CPU的ID
 * @return 错误码
 */
__xwbsp_code
xwer_t xwospl_thd_outmigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        xwer_t rc;

        rc = soc_skd_swi((xwer_t(*)(void *, void *))xwosplcb_thd_outmigrate_lic,
                         (void *)thd, (void *)cpuid);
        return rc;
}

/**
 * @brief 迁移线程至目标CPU
 * @param[in] thd: 线程控制块对象的指针
 * @param[in] cpuid: 目的地CPU的ID
 */
__xwbsp_code
void xwospl_thd_immigrate(struct xwospl_thd * thd, xwid_t cpuid)
{
        XWOS_UNUSED(cpuid);
        xwosplcb_thd_immigrate_lic(thd);
}
#endif
