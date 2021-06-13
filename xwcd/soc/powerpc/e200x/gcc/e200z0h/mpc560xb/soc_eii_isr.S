/**
 * @file
 * @brief SOC描述层：External Input Interrupt Handler
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

#include <cfg/XuanWuOS.h>

        .equ            INTC_IACKR, 0xFFF48010  /* Interrupt Acknowledge Reg. addr. */
        .equ            INTC_EOIR, 0xFFF48018   /* End Of Interrupt Reg. addr. */
        .extern         soc_context

/**
 * @brief external input interrupt handler
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
 */
        .section        .xwos.isr.text,"ax"
        .global         soc_eii_isr
        .align          4
soc_eii_isr:
        /* prolog */
        /* volatile registers stack frame */
        e_stwu          r1, -0x50 (r1)          /* sp-=0x50; *sp = sp; */

        /* save work registers */
        se_stw          r5, 0x30 (r1)           /* save r5; */
        se_stw          r4, 0x2C (r1)           /* save r4; */
        se_stw          r3, 0x28 (r1)           /* save r3; */

        mfcr            r3                      /* save cr; */
        se_stw          r3, 0x20 (r1)
        mfsrr1          r3                      /* save srr1; */
        se_stw          r3, 0x10 (r1)
        mfsrr0          r3                      /* save srr0; */
        e_stw           r3, 0x0C (r1)
        se_mflr         r3                      /* save lr; */
        se_stw          r3, 0x14 (r1)

        /* get interrupt handler */
        e_lis           r3, INTC_IACKR@h        /* r3 = &INTC_IACKR; */
        e_or2i          r3, INTC_IACKR@l
        e_lwz           r5, 0x0 (r3)            /* r5 = *(&INTC_IACKR); */
        e_lwz           r3, 0x0 (r5)            /* r3 = *(*(&INTC_IACKR)); */
        se_mtlr         r3                      /* lr = interrupt handler */

        /* cal IRQ number */
        e_rlwinm        r5, r5, 30, 23, 31      /* r5 = (r5 >> 2) & 0x1FF; */

        /* change mode */
        e_lis           r3, soc_context@h /* r3 = &soc_context; */
        e_or2i          r3, soc_context@l
        se_lwz          r4, 0xC (r3)            /* r4 = soc_context.irqn; */
        se_stw          r4, 0x8 (r1)            /* save old irqn; */
        se_stw          r5, 0xC (r3)            /* soc_context.irqn = r5; */
        se_lwz          r4, 0x8 (r3)            /* r4 = soc_context.irq_nesting_cnt; */
        se_addi         r4, 1                   /* soc_context.irq_nesting_cnt++; */
        se_stw          r4, 0x8 (r3)
        se_cmpi         r4, 1                   /* if (soc_context.irq_nesting_cnt != 1) {*/
        se_bne          chmod_2                 /*      goto chmod_2; */
chmod_1:                                        /* } */
        se_stw          r1, 0x4 (r3)            /* soc_context.thd_sp = sp; */
        se_lwz          r1, 0x0 (r3)            /* sp = soc_context.sys_stack; */
        e_stwu          r1, -0x50 (r1)          /* Create reserved ISR stack frame. */
        msync
        se_isync
chmod_2:
        wrteei          1                       /* enable external interrupt */

        /* save volatile registers */
        mfxer           r3                      /* save xer; */
        se_stw          r3, 0x1C (r1)
        se_mfctr        r3                      /* save ctr; */
        se_stw          r3, 0x18 (r1)
        e_stw           r12, 0x4C (r1)          /* save r12; */
        e_stw           r11, 0x48 (r1)          /* save r11; */
        e_stw           r10, 0x44 (r1)          /* save r10; */
        e_stw           r9, 0x40 (r1)           /* save r9; */
        e_stw           r8, 0x3C (r1)           /* save r8; */
        se_stw          r7, 0x38 (r1)           /* save r7; */
        se_stw          r6, 0x34 (r1)           /* save r6; */
        se_stw          r0, 0x24 (r1)           /* save r0; */

        /* Branch to ISR, and return here */
        se_blrl

        /* epilog */
        /* restore volatile registers */
        se_lwz          r0, 0x24 (r1)           /* restore r0; */
        se_lwz          r6, 0x34 (r1)           /* restore r6; */
        se_lwz          r7, 0x38 (r1)           /* restore r7; */
        e_lwz           r8, 0x3C (r1)           /* restore r8; */
        e_lwz           r9, 0x40 (r1)           /* restore r9; */
        e_lwz           r10, 0x44 (r1)          /* restore r10; */
        e_lwz           r11, 0x48 (r1)          /* restore r11; */
        e_lwz           r12, 0x4C (r1)          /* restore r12; */
        se_lwz          r3, 0x18 (r1)           /* restore ctr; */
        se_mtctr        r3
        se_lwz          r3, 0x1C (r1)           /* restore xer; */
        mtxer           r3

        /* End the IRQ */
        mbar            0                       /* Ensure store to clear interrupt's flag bit completed */
        e_lis           r3, INTC_EOIR@h         /* Load EOIR address */
        e_or2i          r3, INTC_EOIR@l
        e_li            r4, 0
        wrteei          0
        e_stw           r4, 0x0 (r3)            /* *(INTC_EOIR) = 0; */

        /* change mode */
        e_lis           r3, soc_context@h /* r3 = &soc_context; */
        e_or2i          r3, soc_context@l
        se_lwz          r4, 0x8 (r3)            /* r4 = soc_context.irq_nesting_cnt; */
        se_subi         r4, 1                   /* soc_context.irq_nesting_cnt--; */
        se_stw          r4, 0x8 (r3)
        se_cmpi         r4, 0                   /* if (soc_context.irq_nesting_cnt != 0) {*/
        se_bne          rstmod_2                /*         goto @rstmod_2; */
rstmod_1:                                       /* } */
        e_addi          r1, r1, 0x50            /* Del reserved ISR stack frame */
        se_stw          r1, 0x0 (r3)            /* soc_context.sys_stack = sp; */
        se_lwz          r1, 0x4 (r3)            /* sp = soc_context.thd_sp; */
        msync
        se_isync
rstmod_2:
        se_lwz          r5, 0x8 (r1)            /* r5 = old irqn; */
        se_stw          r5, 0xC (r3)            /* soc_context.irqn = r5; */
        /* delete stack frame when the CPUIRQ is disabled.
           Otherwise the stack may be overflow. */
        se_lwz          r3, 0x14 (r1)           /* restore lr; */
        se_mtlr         r3
        e_lwz           r3, 0x0C (r1)           /* restore srr0; */
        mtsrr0          r3
        se_lwz          r3, 0x10 (r1)           /* restore srr1; */
        mtsrr1          r3
        se_lwz          r3, 0x20 (r1)           /* restore cr; */
        mtcrf           0xff, r3
        se_lwz          r5, 0x30 (r1)           /* restore r5; */
        se_lwz          r4, 0x2C (r1)           /* restore r4; */
        se_lwz          r3, 0x28 (r1)           /* restore r3; */
        e_addi          r1, r1, 0x50            /* Del volatile reg stack frame */
        se_rfi
soc_eii_isr_end:
        .size           soc_eii_isr, soc_eii_isr_end - soc_eii_isr
