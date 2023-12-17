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

#include <xwcd/soc/arm64/v8a/arch_skd.h>
#include <xwos/osal/irq.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_regs.h>
#include <xwcd/soc/arm64/v8a/arch_exception.h>

/* #define SOC_DBGF */
#include <soc_debug.h>

/**
 * @brief 初始化调度对象的栈
 * @param[in] stk: 栈信息结构体指针
 * @param[in] exit: 退出函数
 */
__xwbsp_code
void armv8a_skd_init_stack(struct xwospl_skdobj_stack * stk,
                           void (* exit)(xwer_t))
{
        xwstk_t * stkbtn;
        xwsq_t i;
        xwsq_t stknum;
        struct armv8a_reg_frame * regs;

        stkbtn = (xwstk_t *)stk->base;
        stknum = stk->size / sizeof(xwstk_t);
        for (i = 0; i < stknum; i++) {
                stkbtn[i] = (xwu64_t)0xFFFFFFFFFFFFFFFF;
        }
        regs = (struct armv8a_reg_frame *)stk->sp;
        regs--; /* Reserve stack to init register frame */
        for (i = 1; i < xw_array_size(regs->r) - 1; i++) {
                regs->r[i] = 0;
        }
        regs->r[29] = (xwu64_t)0; /* set fp to 0 to end the backtrace */
        regs->r[0] = (xwu64_t)stk->arg;
        regs->lr = (xwu64_t)exit;
        regs->sp = (xwu64_t)stk->sp;
        regs->elr = (xwu64_t)stk->main;
        /* AARCH64的XWOS内核运行在EL2H，线程运行在EL2T，参数 `privileged` 无效。*/
        regs->spsr = ARMV8A_SPSR_EL2T;
        stk->sp = (xwstk_t *)regs;
}

/**
 * @brief 启动调度器
 * @param[in] xwskd: 调度器的指针
 */
__xwbsp_code
void armv8a_skd_start(__xwcc_unused struct xwospl_skd * xwskd,
                      __xwcc_unused xwstk_t * sp_el2)
{
        __asm__ volatile(
        "       mov     sp, x1\n"
        : : :
        );
        /* get xwskd->cstk, x1 = x0->cstk */
        __asm__ volatile(
        "       ldr     x1, [x0, #%[__cstk]]\n"
        :
        :[__cstk] "I" (xwcc_offsetof(struct xwospl_skd, cstk))
        : "x1", "x0"
        );
        /* get sp, r2 = cstk->sp */
        __asm__ volatile(
        "       ldr     x0, [x1, #%[__sp]]\n"
        :
        :[__sp] "I" (xwcc_offsetof(struct xwospl_skdobj_stack, sp))
        : "x1", "x0"
        );
        /* restore callee-saved (non-volatile) register frame */
        __asm__ volatile(
        "       ldp     x28, x29, [x0, #0xE0]\n"
        "       ldp     x26, x27, [x0, #0xD0]\n"
        "       ldp     x24, x25, [x0, #0xC0]\n"
        "       ldp     x22, x23, [x0, #0xB0]\n"
        "       ldp     x20, x21, [x0, #0xA0]\n"
        "       ldp     x18, x19, [x0, #0x90]\n"
        : : :
        );
        /* restore caller-saved (volatile) register frame */
        __asm__ volatile(
        "       ldp     x2, x3, [x0, #0x100]\n"
        "       msr     spsr_el2, x3\n"
        "       msr     elr_el2, x2\n"
        "       ldp     lr, x1, [x0, #0xF0]\n"
        "       msr     sp_el0, x1\n"
        "       ldp     x16, x17, [x0, #0x80]\n"
        "       ldp     x14, x15, [x0, #0x70]\n"
        "       ldp     x12, x13, [x0, #0x60]\n"
        "       ldp     x10, x11, [x0, #0x50]\n"
        "       ldp     x8, x9, [x0, #0x40]\n"
        "       ldp     x6, x7, [x0, #0x30]\n"
        "       ldp     x4, x5, [x0, #0x20]\n"
        "       ldp     x2, x3, [x0, #0x10]\n"
        "       ldp     x0, x1, [x0, #0x00]\n"
        : : :
        );
        /* return to thread */
        __asm__ volatile(
        "       dsb     sy\n"
        "       isb\n"
        "       eret\n"
        : : :
        );
}

void armv8a_skd_swcx(void)
{
        struct xwospl_skd * xwskd;
        struct armv8a_reg_frame * csp;
        struct armv8a_reg_frame * psp;

        /* Check previous stack */
        armv8a_sysreg_read((xwu64_t *)&psp, sp_el0);
        xwskd = armv8a_skd_chk_swcx((xwreg_t)psp);
        soc_dbgf("SWCX",
                 "PSTK: {.name = %s, sp = 0x%lX}\r\n",
                 xwskd->pstk->name, (xwptr_t)psp);
        xwosplcb_skd_pre_swcx_lic(xwskd);
        /* TODO: Save FP regs frame */
        /* Save previous stack pointer */
        xwskd->pstk->sp = (xwstk_t *)psp;

        csp = (struct armv8a_reg_frame *)xwskd->cstk->sp;
        /* TODO: Restore FP regs frame */
        /* Restore current stack pointer */
        armv8a_sysreg_write(sp_el0, (xwu64_t)csp);
        soc_dbgf("SWCX",
                 "CSTK: {.name = %s, sp = 0x%lX}\r\n",
                 xwskd->cstk->name, (xwptr_t)csp);
        /* Finish the progress */
        xwosplcb_skd_post_swcx_lic(xwskd);
        armv8a_clear_exclusive();
        armv8a_dsb(sy);
        armv8a_isb();
}

/**
 * @brief 切换线程上下文时检查线程的栈溢出
 * @return 调度器的指针
 */
__xwbsp_code
struct xwospl_skd * armv8a_skd_chk_swcx(xwreg_t psp)
{
        struct xwospl_skd * xwskd = xwosplcb_skd_get_lc();
#if defined(XWMMCFG_STACK_CHK_SWCX) && (1 == XWMMCFG_STACK_CHK_SWCX)
        struct xwospl_skdobj_stack * cstk;
        struct xwospl_skdobj_stack * pstk;
        xwu64_t * stkbtn;
        xwsz_t guard;
        xwsz_t i;

        cstk = xwskd->cstk;
        pstk = xwskd->pstk;
        stkbtn = (xwu64_t *)pstk->guard_base;
        guard = pstk->guard;
        if ((psp) < ((xwptr_t)stkbtn + guard)) {
                soc_logf("cstk: {.name:%s, .sp: 0x%lX}\r\n", cstk->name, cstk->sp);
                armv8a_backtrace_frame((struct armv8a_reg_frame *)cstk->sp, cstk->name);
                armv8a_dump((struct armv8a_reg_frame *)cstk->sp);
                soc_logf("pstk: {.name:%s, .sp: 0x%lX}\r\n", pstk->name, pstk->sp);
                armv8a_backtrace_frame((struct armv8a_reg_frame *)pstk->sp, pstk->name);
                armv8a_dump((struct armv8a_reg_frame *)pstk->sp);
                soc_logf("sp_el0: 0x%lX\r\n", psp);
                armv8a_backtrace_frame((struct armv8a_reg_frame *)psp, "overflow");
                armv8a_dump((struct armv8a_reg_frame *)psp);
                soc_panic("Stack overflow! reporter:%s\r\n", __FUNCTION__);
        }
        for (i = 0; i < guard; i += 8) {
                if ((xwu64_t)0xFFFFFFFFFFFFFFFF != stkbtn[i / 8U]) {
                        soc_panic("Stack overflow! Guard break! reporter:%s\r\n",
                                  __FUNCTION__);
                }
        }

#endif
        return xwskd;
}

/**
 * @brief 异常时检查当前线程的栈溢出
 * @return 调度器的指针
 */
__xwbsp_code
struct xwospl_skd * armv8a_skd_chk_stk(xwreg_t csp)
{
        struct xwospl_skd * xwskd;
        struct xwospl_skdobj_stack * cstk;
        xwu64_t * stkbtn;
        xwsz_t guard;

        xwskd = xwosplcb_skd_get_lc();
        cstk = xwskd->cstk;
        stkbtn = (xwu64_t *)cstk->guard_base;
        guard = cstk->guard;
        if (csp < ((xwptr_t)stkbtn + guard)) {
                soc_logf("cstk: {.name:%s, .sp: 0x%lX}\r\n", cstk->name, cstk->sp);
                soc_logf("sp_el0: 0x%016lX\r\n", csp);
                armv8a_backtrace_frame((struct armv8a_reg_frame *)csp, "fault");
                armv8a_dump((struct armv8a_reg_frame *)csp);
                soc_panic("Stack overflow! reporter:%s\r\n", __FUNCTION__);
        }
        return xwskd;
}
