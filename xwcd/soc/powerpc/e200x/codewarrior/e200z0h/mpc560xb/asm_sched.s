/**
 * @file
 * @brief switch context
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <cfg/XuanWuOS.h>
#include <e200x_core.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
        .equ            INTC_SSCIR0_3, 0xFFF48020

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
        .extern         soc_context

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
        .extern         xwos_scheduler_finish_swcx
        .extern         soc_scheduler_chkpstk
        .extern         _SDA_BASE_
        .extern         _SDA2_BASE_

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief switch context
 * @note
 * Stack frame:
 *  ____volatile registers________
 *  0x4C     *  GPR12       *    ^
 *  0x48     *  GPR11       *    |
 *  0x44     *  GPR10       *    |
 *  0x40     *  GPR9        *    |
 *  0x3C     *  GPR8        *    |
 *  0x38     *  GPR7        *  GPRs (32 bit)
 *  0x34     *  GPR6        *    |
 *  0x30     *  GPR5        *    |
 *  0x2C     *  GPR4        *    |
 *  0x28     *  GPR3        *    |
 *  0x24     *  GPR0        * ___v__________
 *  0x20     *  CR          * __CR__________
 *  0x1C     *  XER         *    ^
 *  0x18     *  CTR         *    |
 *  0x14     *  LR          * locals & padding for 16 B alignment
 *  0x10     *  SRR1        *    |
 *  0x0C     *  SRR0        *    |
 *  0x08     *  IRQn        * ___v__________
 *  0x04     * LR of caller * calling function will save old lr here
 *  0x00     *  SP          * Backchain
 *           ****************
 *
 * non-volatile registers stack frame
 *  __non-volatile registers___
 *  0x4C     *  GPR31    *    ^
 *  0x48     *  GPR30    *    |
 *  0x44     *  GPR29    *    |
 *  0x40     *  GPR28    *    |
 *  0x3C     *  GPR27    *    |
 *  0x38     *  GPR26    *    |
 *  0x34     *  GPR25    *    |
 *  0x30     *  GPR24    *    |
 *  0x2C     *  GPR23    *    |
 *  0x28     *  GPR22    *    |
 *  0x24     *  GPR21    *    |
 *  0x20     *  GPR20    *    |
 *  0x1C     *  GPR19    *    |
 *  0x18     *  GPR18    *    |
 *  0x14     *  GPR17    *    |
 *  0x10     *  GPR16    *    |
 *  0x0C     *  GPR15    *    |
 *  0x08     *  GPR14    *    |
 *  0x04     *  GPR13    *    |
 *  0x00     *  GPR2     *    |
 */
        .section        .xwos.isr.text,text_vle
        .global         soc_scheduler_isr_swcx
        .align          4
        .function       "soc_scheduler_isr_swcx", soc_scheduler_isr_swcx, \
                        soc_scheduler_isr_swcx_end - soc_scheduler_isr_swcx
soc_scheduler_isr_swcx:
        /* clear IRQ flag */
        e_lis           r3, INTC_SSCIR0_3@h
        e_or2i          r3, INTC_SSCIR0_3@l
        se_lwz          r4, 0 (r3)
        se_bseti        r4, 7
        se_stw          r4, 0 (r3)

        /* reserved stack frame for compiler */
        e_stwu          r1, -32 (r1)
        se_mflr         r0
        se_stw          r0, 36 (r1) /* save the lr to caller stack frame */
        /* check thread stack & get xwos scheduler */
        e_bl            soc_scheduler_chkpstk /* r3 = soc_scheduler_chkpstk(); */

        /* get thread SP */
        e_lis           r4, soc_context@h /* r4 = &soc_context; */
        e_or2i          r4, soc_context@l
        se_lwz          r5, 0x4 (r4) /* r5 = soc_context.thrd_sp; */
        /* save previous context */
        se_lwz          r4, 4 (r3) /* r4 = xwsd->pstk; */
        /* save non-volatile registers */
        e_subi          r5, r5, 0x50 /* non-volatile registers stack frame */
        se_stw          r2, 0 (r5) /* save r2 */
        e_stmw          r13, 4 (r5) /* save r13 ~ r31 */
        se_stw          r5, 0 (r4) /* xwsd->pstk->sp = r5; */

        /* restore current context */
        se_lwz          r4, 0 (r3) /* r4 = xwsd->cstk */
        se_lwz          r5, 0 (r4) /* r5 = r4->sp; */
        /* restore non-volatile registers */
        e_lmw           r13, 4 (r5) /* restore r13 ~ r31 */
        se_lwz          r2, 0 (r5) /* restore r2 */
        e_addi          r5, r5, 0x50 /* Del non-volatile registers stack frame */

        /* set thread SP */
        e_lis           r4, soc_context@h /* r4 = &soc_context; */
        e_or2i          r4, soc_context@l
        se_stw          r5, 0x4 (r4) /* soc_context.thrd_sp = r5; */

        /* finish the progress */
        e_bl            xwos_scheduler_finish_swcx
        se_lwz          r0, 36 (r1)
        se_mtlr         r0
        se_addi         r1, 32  /* delete reserved frame (used by compiler) */
        se_blr
soc_scheduler_isr_swcx_end:
        .size           soc_scheduler_isr_swcx, \
                        soc_scheduler_isr_swcx - soc_scheduler_isr_swcx

/**
 * @brief Local CPU starts scheduler
 * @param xwsd(r3): xwos scheduler
 */
        .section        .xwos.isr.text,text_vle
        .global         soc_scheduler_start_lc
        .align          4
        .function       "soc_scheduler_start_lc", soc_scheduler_start_lc, \
                        soc_scheduler_start_lc_end - soc_scheduler_start_lc
/* xwer_t soc_scheduler_start_lc(__maybe_unused struct xwos_scheduler * xwsd) */
soc_scheduler_start_lc:
        se_lwz          r4, 0x0 (r3) /* r4 = xwsd->cstk; */
        se_lwz          r1, 0x0 (r4) /* r1 = r4->sp; */

        /* restore non-volatile registers */
        se_lwz          r2, 0x0 (r1) /* restore r2 */
        e_lmw           r13, 0x4 (r1) /* restore r13 ~ r31 */
        e_addi          r1, r1, 0x50 /* delete stack frame */

        /* restore volatile registers */
        e_lwz           r3, 0x0C (r1) /* restore srr0; */
        mtsrr0          r3
        se_lwz          r3, 0x10 (r1) /* restore srr1; */
        mtsrr1          r3
        se_lwz          r3, 0x14 (r1) /* restore lr; */
        se_mtlr         r3
        se_lwz          r3, 0x18 (r1) /* restore ctr; */
        se_mtctr        r3
        se_lwz          r3, 0x1C (r1) /* restore xer */
        mtxer           r3
        se_lwz          r3, 0x20 (r1) /* restore cr */
        mtcrf           0xff, r3
        se_lwz          r0, 0x24 (r1) /* restore r0; */
        se_lwz          r3, 0x28 (r1) /* restore r3; */
        se_lwz          r4, 0x2C (r1) /* restore r4; */
        se_lwz          r5, 0x30 (r1) /* restore r5; */
        se_lwz          r6, 0x34 (r1) /* restore r6; */
        se_lwz          r7, 0x38 (r1) /* restore r7; */
        e_lwz           r8, 0x3C (r1) /* restore r8; */
        e_lwz           r9, 0x40 (r1) /* restore r9; */
        e_lwz           r10, 0x44 (r1) /* restore r10; */
        e_lwz           r11, 0x48 (r1) /* restore r11; */
        e_lwz           r12, 0x4C (r1) /* restore r12; */
        /* Del volatile reg stack frame */
        e_addi          r1, r1, 0x50 /* delete stack frame */
        se_isync
        se_rfi
soc_scheduler_start_lc_end:
        .size           soc_scheduler_start_lc, \
                        soc_scheduler_start_lc_end - soc_scheduler_start_lc

/**
 * @brief scheduler software interrupt
 */
        .section        .xwos.isr.text,text_vle
        .global         soc_scheduler_isr_swi
        .align          4
        .function       "soc_scheduler_isr_swi", soc_scheduler_isr_swi, \
                        soc_scheduler_isr_swi_end - soc_scheduler_isr_swi
/* void soc_scheduler_isr_swi(void) */
soc_scheduler_isr_swi:
        /* clear IRQ flag */
        se_mr           r6, r4                  /* @ref eii_sysisr, r4 is used as old SP */
        e_lis           r3, INTC_SSCIR0_3@h
        e_or2i          r3, INTC_SSCIR0_3@l
        se_lwz          r4, 0 (r3)
        se_bseti        r4, 15
        se_stw          r4, 0 (r3)

        se_lwz          r3, 0x28 (r6)           /* restore r3; arg0 */
        se_lwz          r4, 0x2C (r6)           /* restore r4; arg1 */
        se_lwz          r5, 0x30 (r6)           /* restore r5; swi_entry */
        e_stwu          r1, -32 (r1)            /* reserved stack frame for compiler */
        se_mflr         r0
        se_stw          r0, 36 (r1)             /* save old lr to caller stack frame */
        se_stw          r6, 16 (r1)             /* push r6 */
        se_mtlr         r5
        se_blrl                                 /* r3 = lr(r3, r4); */
        se_lwz          r6, 16 (r1)             /* pop r6 */
        se_lwz          r0, 36 (r1)
        se_mtlr         r0
        se_addi         r1, 32
        se_stw          r3, 0x28 (r6)           /* save the result */
        se_blr
soc_scheduler_isr_swi_end:
        .size           soc_scheduler_isr_swi, \
                        soc_scheduler_isr_swi_end - soc_scheduler_isr_swi
