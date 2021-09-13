/**
 * @file
 * @brief SOC描述层：SOC汇编代码
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * Copyright (c) 2019 Nuclei Limited. All rights reserved.
 * @copyright
 * + (c) 2021 隐星魂 (Roy.Sun) <https://xwos.tech>
 * + (c) 2019 Nuclei Limited. All rights reserved.
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

        .equ            SOC_EXCode_InsUnalign, 0
        .equ            SOC_EXCode_InsAccFault, 1
        .equ            SOC_EXCode_IlleIns, 2
        .equ            SOC_EXCode_Break, 3
        .equ            SOC_EXCode_LdAddrUnalign, 4
        .equ            SOC_EXCode_LdFault, 5
        .equ            SOC_EXCode_StAddrUnalign, 6
        .equ            SOC_EXCode_StAccessFault, 7
        .equ            SOC_EXCode_UmodeEcall, 8
        .equ            SOC_EXCode_MmodeEcall, 11
        .equ            SOC_EXCode_NMI, 0xFFF
#ifndef __riscv_32e
        .equ SOC_CALLER_CONTEXT_SIZE, (20 * REGBYTES)
        .equ SOC_CALLEE_CONTEXT_SIZE, (13 * REGBYTES)
#else
        .equ SOC_CALLER_CONTEXT_SIZE, (14 * REGBYTES)
        .equ SOC_CALLEE_CONTEXT_SIZE, (3 * REGBYTES)
#endif
        .equ SOC_CONTEXT_SIZE, (SOC_CALLER_CONTEXT_SIZE + SOC_CALLEE_CONTEXT_SIZE)

        .extern         __global_pointer$
        .extern         arch_lowlevel_init
        .extern         cpu_lowlevel_init
        .extern         soc_lowlevel_init
        .extern         board_lowlevel_init
        .extern         arch_relocate
        .extern         arch_init
        .extern         cpu_init
        .extern         soc_init
        .extern         board_init
        .extern         xwos_init
        .extern         xwos_main
        .extern         soc_esr_c_entry
        .extern         soc_esr_nmi
        .extern         soc_esr_ebreak
        .extern         soc_esr_xwsc

        .section        .soc.boot,"ax"
        .global         soc_boot
        .func           soc_boot
soc_boot:
        /* Boot Stage 1 */
        csrc            CSR_MSTATUS, MSTATUS_MIE /* 关闭全局中断 */
        /* 启动分为几种情况：
         * +   启动配置(boot0==1,boot1==1)，PC从0x20000000开始执行代码；
         * +   启动配置(boot0==0)，flash(0x08000000)被映射到0x0，PC从0x0开始执行代码；
         */
        la              a0, soc_boot /* 相对于PC加载soc_boot地址 */
        /* 判断soc_boot的值是否在RAM的范围内，若为真，直接相对跳转到soc_boot_vma */
        li              a1, 1
        slli            a1, a1, 29 /* a1 = 0x20000000 */
        bleu            a1, a0, soc_boot_vma
        /* 判断soc_boot的值是否在FLASH的范围内，若为真，直接相对跳转到soc_boot_vma */
        srli            a1, a1, 2 /* a1 = 0x08000000 */
        bleu            a1, a0, soc_boot_vma
        /* 若两个比较都为假，就加载soc_boot_vma在FLASH中的绝对地址，并跳转 */
        la              a0, soc_boot_vma
        add             a0, a0, a1
        jr              a0
soc_boot_vma:
        /* 初始化GP */
        .option         push
        .option         norelax
        la              gp, __global_pointer$
        .option         pop

        /* 初始化SP */
        la              sp, xwos_stk_top
        csrw            CSR_MSCRATCH, sp

        /* 设置NMI使用mtvec寄存器中的地址作为中断服务程序入口 */
        li              t0, MMISC_CTL_NMI_CAUSE_FFF
        csrs            CSR_MMISC_CTL, t0

        /* 设置异常服务程序入口 */
        la              t0, soc_esr_entry
        csrw            CSR_MTVEC, t0

        /* 设置ECLIC模式 */
        li              t0, 0x3f
        csrc            CSR_MTVEC, t0
        csrs            CSR_MTVEC, 0x3

        /* Boot Stage 2 */
#ifdef __riscv_flen
        /* Enable FPU */
        li              t0, MSTATUS_FS
        csrs            mstatus, t0
        csrw            fcsr, x0
#endif
        /* Enable mcycle and minstret counter */
        csrci           CSR_MCOUNTINHIBIT, MCOUNTINHIBIT_IR | MCOUNTINHIBIT_CY

        /* Boot Stage 3 */
        call            arch_lowlevel_init
        call            cpu_lowlevel_init
        call            soc_lowlevel_init
        call            board_lowlevel_init
        call            xwos_init
        call            arch_relocate
        call            arch_init
        call            cpu_init
        call            soc_init
        call            board_init
        call            xwos_main
        .endfunc
soc_boot_end:
        .size           soc_boot, soc_boot_end - soc_boot

/**
 * @brief Macro to save caller x-registers
 */
        .macro SOC_SAVE_CALLER_X_CONTEXT
        /* Allocate stack space for context saving */
        addi            sp, sp, -(SOC_CALLER_CONTEXT_SIZE)
        STORE           ra, 0 * REGBYTES (sp)
        STORE           t0, 1 * REGBYTES (sp)
        STORE           t1, 2 * REGBYTES (sp)
        STORE           t2, 3 * REGBYTES (sp)
        STORE           a0, 4 * REGBYTES (sp)
        STORE           a1, 5 * REGBYTES (sp)
        STORE           a2, 6 * REGBYTES (sp)
        STORE           a3, 7 * REGBYTES (sp)
        STORE           a4, 8 * REGBYTES (sp)
        STORE           a5, 9 * REGBYTES (sp)
#ifndef __riscv_32e
        STORE           a6, 14 * REGBYTES (sp)
        STORE           a7, 15 * REGBYTES (sp)
        STORE           t3, 16 * REGBYTES (sp)
        STORE           t4, 17 * REGBYTES (sp)
        STORE           t5, 18 * REGBYTES (sp)
        STORE           t6, 19 * REGBYTES (sp)
#endif
        .endm

/**
 * @brief Macro to restore caller x-registers
 */
        .macro SOC_RESTORE_CALLER_X_CONTEXT
#ifndef __riscv_32e
        LOAD            t6, 19 * REGBYTES (sp)
        LOAD            t5, 18 * REGBYTES (sp)
        LOAD            t4, 17 * REGBYTES (sp)
        LOAD            t3, 16 * REGBYTES (sp)
        LOAD            a7, 15 * REGBYTES (sp)
        LOAD            a6, 14 * REGBYTES (sp)
#endif
        LOAD            a5, 9 * REGBYTES (sp)
        LOAD            a4, 8 * REGBYTES (sp)
        LOAD            a3, 7 * REGBYTES (sp)
        LOAD            a2, 6 * REGBYTES (sp)
        LOAD            a1, 5 * REGBYTES (sp)
        LOAD            a0, 4 * REGBYTES (sp)
        LOAD            t2, 3 * REGBYTES (sp)
        LOAD            t1, 2 * REGBYTES (sp)
        LOAD            t0, 1 * REGBYTES (sp)
        LOAD            ra, 0 * REGBYTES (sp)
        addi            sp, sp, SOC_CALLER_CONTEXT_SIZE
        .endm

/**
 * @brief Macro to restore caller registers when EXC is ecall
 */
        .macro SOC_RESTORE_CALLER_X_CONTEXT_ECALL
#ifndef __riscv_32e
        LOAD            t6, 19 * REGBYTES (sp)
        LOAD            t5, 18 * REGBYTES (sp)
        LOAD            t4, 17 * REGBYTES (sp)
        LOAD            t3, 16 * REGBYTES (sp)
        LOAD            a7, 15 * REGBYTES (sp)
        LOAD            a6, 14 * REGBYTES (sp)
#endif
        LOAD            a5, 9 * REGBYTES (sp)
        LOAD            a4, 8 * REGBYTES (sp)
        LOAD            a3, 7 * REGBYTES (sp)
        LOAD            a2, 6 * REGBYTES (sp)
        /* Don't restore a0, a1, because they are return value. */
        LOAD            t2, 3 * REGBYTES (sp)
        LOAD            t1, 2 * REGBYTES (sp)
        LOAD            t0, 1 * REGBYTES (sp)
        LOAD            ra, 0 * REGBYTES (sp)
        addi            sp, sp, SOC_CALLER_CONTEXT_SIZE
        .endm

/**
 * @brief Macro to save caller CSRs to stack
 */
        .macro SOC_SAVE_CALLER_CSR_CONTEXT
        csrr            t0, CSR_MSCRATCH
        STORE           t0, 10 * REGBYTES (sp)
        csrrwi          x0, CSR_PUSHMCAUSE, 11
        csrrwi          x0, CSR_PUSHMEPC, 12
        csrrwi          x0, CSR_PUSHMSUBM, 13
        .endm

/**
 * @brief Macro to restore caller CSRs from stack
 */
        .macro SOC_RESTORE_CALLER_CSR_CONTEXT mepcfix
        LOAD            t0, 13 * REGBYTES (sp)
        csrw            CSR_MSUBM, t0
        LOAD            t0, 12 * REGBYTES (sp)
        addi            t0, t0, \mepcfix
        csrw            CSR_MEPC, t0
        LOAD            t0, 11 * REGBYTES (sp)
        csrw            CSR_MCAUSE, t0
        LOAD            t0, 10 * REGBYTES (sp)
        csrw            CSR_MSCRATCH, t0
        .endm


/**
 * @brief Exception/NMI Service Routine Entry
 * @details
 * - This function provide common entry functions for exception/nmi.
 * @remarks
 * - This function provide a default exception/nmi entry.
 *   ABI defined caller save register and some CSR registers
 *   to be saved before enter interrupt handler and be restored before return.
 */
        .section        .soc.esr.entry,"ax"
        /* In CLIC mode, the exeception entry must be 64bytes aligned */
        .align          6 /* 1 << 6 */
        .global         soc_esr_entry
        .func           soc_esr_entry
soc_esr_entry:
        SOC_SAVE_CALLER_X_CONTEXT
        SOC_SAVE_CALLER_CSR_CONTEXT
        /* switch (mcase & 0xFFF) */
        csrr            t0, mcause
        li              t1, 0xFFF
        and             t1, t0, t1
        li              t2, SOC_EXCode_NMI
        beq             t1, t2, 2f
        li              t2, SOC_EXCode_Break
        beq             t1, t2, 3f
        li              t2, SOC_EXCode_UmodeEcall
        beq             t1, t2, 4f
        li              t2, SOC_EXCode_MmodeEcall
        beq             t1, t2, 4f
1:      /* Other exception */
        mv              a0, t0
        mv              a1, sp
        /* soc_esr_c_entry(mcause, sp) */
        call            soc_esr_c_entry
        /* Restore registers */
        SOC_RESTORE_CALLER_CSR_CONTEXT 0
        SOC_RESTORE_CALLER_X_CONTEXT
        j               5f
2:      /* NMI */
        call            soc_esr_nmi
        SOC_RESTORE_CALLER_CSR_CONTEXT 0
        SOC_RESTORE_CALLER_X_CONTEXT_ECALL
3:      /* ebreak */
        call            soc_esr_ebreak /* soc_esr_ebreak(); */
        SOC_RESTORE_CALLER_CSR_CONTEXT 4
        SOC_RESTORE_CALLER_X_CONTEXT
        j               5f
4:      /* ecall */
        call            soc_esr_xwsc /* a0, a1 = soc_esr_xwsc(a0, a1, a2); */
        SOC_RESTORE_CALLER_CSR_CONTEXT 4
        SOC_RESTORE_CALLER_X_CONTEXT_ECALL
5:      /* Return to regular code */
        mret
        .endfunc
soc_esr_entry_end:
        .size           soc_esr_entry, soc_esr_entry_end - soc_esr_entry


/**
 * @brief Non-Vector Interrupt Service Routine Entry
 * @details
 * This function provide common entry functions for handling
 * non-vector interrupts
 * @remarks
 * This function provide a default non-vector interrupt entry.
 * ABI defined caller save register and some CSR registers need
 * to be saved before enter interrupt handler and be restored before return.
 */
        .section        .soc.isr.entry,"ax"
        .align          2 /* 1 << 2 */
        .global         soc_isr_entry
        .func           soc_isr_entry
/* void soc_isr_entry(void); */
soc_isr_entry:
        /* Save context */
        SOC_SAVE_CALLER_X_CONTEXT
        SOC_SAVE_CALLER_CSR_CONTEXT

        /* Change stack */
        csrrw           sp, CSR_MSCRATCHCSWL, sp /* exchange MSCRATCH, sp */

        /* This special CSR read/write operation, which is actually
         * claim the CLIC to find its pending highest ID, if the ID
         * is not 0, then automatically enable the mstatus.MIE, and
         * jump to its vector-entry-label, and update the link register
         */
        csrrw           ra, CSR_JALMNXTI, ra

        /* Critical section with interrupts disabled */
        csrc            CSR_MSTATUS, MSTATUS_MIE

        /* Restore stack */
        csrrw           sp, CSR_MSCRATCHCSWL, sp /* exchange sp, MSCRATCH */

        /* Restore context */
        SOC_RESTORE_CALLER_CSR_CONTEXT 0
        SOC_RESTORE_CALLER_X_CONTEXT

        /* Return to regular code */
        mret
        .endfunc
soc_isr_entry_end:
        .size           soc_isr_entry, soc_isr_entry_end - soc_isr_entry
