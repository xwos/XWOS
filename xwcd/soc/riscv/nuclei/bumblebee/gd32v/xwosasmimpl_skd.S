/**
 * @file
 * @brief XWOS移植实现层：调度器汇编代码
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + (c) 2021 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#include "NMSIS/Core/Include/riscv_encoding.h"

#ifndef __riscv_32e
        .equ SOC_CALLER_CONTEXT_SIZE, (20 * REGBYTES)
        .equ SOC_CALLEE_CONTEXT_SIZE, (13 * REGBYTES)
#else
        .equ SOC_CALLER_CONTEXT_SIZE, (14 * REGBYTES)
        .equ SOC_CALLEE_CONTEXT_SIZE, (3 * REGBYTES)
#endif
        .equ SOC_CONTEXT_SIZE, (SOC_CALLER_CONTEXT_SIZE + SOC_CALLEE_CONTEXT_SIZE)

        .extern         soc_skd_chk_swcx
        .extern         xwosplcb_skd_post_start_lic
        .extern         xwosplcb_skd_pre_swcx_lic
        .extern         xwosplcb_skd_post_swcx_lic

/**
 * @brief Macro to save callee context
 */
        .macro SOC_SAVE_CALLEE_CONTEXT sp, tmp
        addi            \sp, \sp, -(SOC_CALLEE_CONTEXT_SIZE)
        STORE           s0, 0 * REGBYTES (\sp)
        STORE           s1, 1 * REGBYTES (\sp)
        csrr            \tmp, mstatus
        STORE           \tmp, 2 * REGBYTES (\sp)
#ifndef __riscv_32e
        STORE           s2, 3 * REGBYTES (\sp)
        STORE           s3, 4 * REGBYTES (\sp)
        STORE           s4, 5 * REGBYTES (\sp)
        STORE           s5, 6 * REGBYTES (\sp)
        STORE           s6, 7 * REGBYTES (\sp)
        STORE           s7, 8 * REGBYTES (\sp)
        STORE           s8, 9 * REGBYTES (\sp)
        STORE           s9, 10 * REGBYTES (\sp)
        STORE           s10, 11 * REGBYTES (\sp)
        STORE           s11, 12 * REGBYTES (\sp)
#endif
        .endm

/**
 * @brief Macro to restore callee context
 */
        .macro SOC_RESTORE_CALLEE_CONTEXT sp, tmp
#ifndef __riscv_32e
        LOAD            s11, 12 * REGBYTES (\sp)
        LOAD            s10, 11 * REGBYTES (\sp)
        LOAD            s9, 10 * REGBYTES (\sp)
        LOAD            s8, 9 * REGBYTES (\sp)
        LOAD            s7, 8 * REGBYTES (\sp)
        LOAD            s6, 7 * REGBYTES (\sp)
        LOAD            s5, 6 * REGBYTES (\sp)
        LOAD            s4, 5 * REGBYTES (\sp)
        LOAD            s3, 4 * REGBYTES (\sp)
        LOAD            s2, 3 * REGBYTES (\sp)
#endif
        LOAD            \tmp, 2 * REGBYTES (\sp)
        csrw            mstatus, \tmp
        LOAD            s1, 1 * REGBYTES (\sp)
        LOAD            s0, 0 * REGBYTES (\sp)
        addi            \sp, \sp, SOC_CALLEE_CONTEXT_SIZE
        .endm

/**
 * @brief Macro to restore caller x-registers
 */
        .macro SOC_RESTORE_CALLER_CONTEXT sp, tmp
#ifndef __riscv_32e
        LOAD            t6, 19 * REGBYTES (\sp)
        LOAD            t5, 18 * REGBYTES (\sp)
        LOAD            t4, 17 * REGBYTES (\sp)
        LOAD            t3, 16 * REGBYTES (\sp)
        LOAD            a7, 15 * REGBYTES (\sp)
        LOAD            a6, 14 * REGBYTES (\sp)
#endif
        LOAD            \tmp, 13 * REGBYTES (\sp)
        csrw            CSR_MSUBM, \tmp
        LOAD            \tmp, 12 * REGBYTES (\sp)
        csrw            CSR_MEPC, \tmp
        LOAD            \tmp, 11 * REGBYTES (\sp)
        csrw            CSR_MCAUSE, \tmp
        LOAD            \tmp, 10 * REGBYTES (\sp)
        csrw            CSR_MSCRATCH, \tmp
        LOAD            a5, 9 * REGBYTES (\sp)
        LOAD            a4, 8 * REGBYTES (\sp)
        LOAD            a3, 7 * REGBYTES (\sp)
        LOAD            a2, 6 * REGBYTES (\sp)
        LOAD            a1, 5 * REGBYTES (\sp)
        LOAD            a0, 4 * REGBYTES (\sp)
        LOAD            t2, 3 * REGBYTES (\sp)
        LOAD            t1, 2 * REGBYTES (\sp)
        LOAD            t0, 1 * REGBYTES (\sp)
        LOAD            ra, 0 * REGBYTES (\sp)
        addi            \sp, \sp, SOC_CALLER_CONTEXT_SIZE
        .endm

/**
 * @brief Local CPU starts scheduler
 * @param[in] xwskd(a0): XWOS scheduler
 */
        .section        .xwos.isr.text,"ax"
        .global         xwospl_skd_start
        .align          2 /* 1 < 2 */
        .func           xwospl_skd_start
/* xwer_t xwospl_skd_start(__maybe_unused struct xwos_skd * xwskd) */
xwospl_skd_start:
        /* a0 = xwosplcb_skd_post_start_lic(xwskd)  */
        addi            sp, sp, -16
        STORE           ra, 3 * REGBYTES (sp)
        call            xwosplcb_skd_post_start_lic
        LOAD            ra, 3 * REGBYTES (sp)
        addi            sp, sp, 16
        /* Load SP */
        LOAD            a0, 0 * REGBYTES (a0) /* a0 = xwskd->cstk */
        LOAD            sp, 0 * REGBYTES (a0) /* sp = cstk->sp */
        LOAD            tp, 1 * REGBYTES (a0) /* tp = cstk->tls */
        SOC_RESTORE_CALLEE_CONTEXT sp, t0
        SOC_RESTORE_CALLER_CONTEXT sp, t0
        /* Return to thread main function */
        mret
        .endfunc
xwospl_skd_start_end:
        .size           xwospl_skd_start, xwospl_skd_start_end - xwospl_skd_start


/**
 * @brief Switch Context ISR
 */
        .section        .xwos.isr.text,"ax"
        .global         xwospl_skd_isr_swcx
        .align          2 /* 1 < 2 */
        .func           xwospl_skd_isr_swcx
        /* void xwospl_skd_isr_swcx(void) */
xwospl_skd_isr_swcx:
        /* prolog */
        addi            sp, sp, -16
        STORE           ra, 3 * REGBYTES (sp)

        /* check previous stack */
        call            soc_skd_chk_swcx
        call            xwosplcb_skd_pre_swcx_lic /* a0 = xwskd; */

        /* Save previous context */
        csrr            t0, CSR_MSCRATCH /* t0 = previous stack sp */
        SOC_SAVE_CALLEE_CONTEXT t0, t1
        LOAD            t1, 1 * REGBYTES (a0) /* t1 = a0->pstk */
        STORE           t0, 0 * REGBYTES (t1) /* pstk->sp = t0 */

        /* Restore current context */
        LOAD            t1, 0 * REGBYTES (a0) /* t1 = skd->cstk */
        LOAD            tp, 1 * REGBYTES (t1) /* tp = cstk->tls */
        LOAD            t0, 0 * REGBYTES (t1) /* t0 = cstk->sp */
        SOC_RESTORE_CALLEE_CONTEXT t0, t1
        csrw            CSR_MSCRATCH, t0 /* MSCRATCH = current stack sp */

        /* Finish */
        call            xwosplcb_skd_post_swcx_lic

        /* epilog */
        LOAD            ra, 3 * REGBYTES (sp)
        addi            sp, sp, 16
        fence           iorw, iorw
        fence.i
        ret
        .endfunc
xwospl_skd_isr_swcx_end:
        .size           xwospl_skd_isr_swcx, xwospl_skd_isr_swcx_end - xwospl_skd_isr_swcx
