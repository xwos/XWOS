/**
 * @file
 * @brief 架构描述层：e00z0x core startup
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

#include <cfg/project.h>

        .extern         e200z0h_isr_stack_mr_size
        .extern         e200z0h_isr_stack_top
        .extern         e200z0h_isr_stack_end
        .extern         _SDA_BASE_
        .extern         _SDA2_BASE_
        .extern         xwos_preinit
        .extern         xwos_init
        .extern         xwos_postinit
        .extern         xwos_main

/**
 * @brief Reset handler
 */
        .section        .xwos.init.text,"ax"
        .global         arch_isr_reset
        .func
arch_isr_reset:
        /* step 1: Disable IRQ */
        wrteei          0
        /* step 2: Disable watchdog */
        e_lis           r6, 0xFFF3
        e_or2i          r6, 0x8010
        e_li            r7, 0xC520
        se_stw          r7, 0x0 (r6)
        e_li            r7, 0xD928
        se_stw          r7, 0x0 (r6)  /* # Disengage soft-lock */

        e_lis           r6, 0xFFF3
        e_or2i          r6, 0x8000
        e_lis           r7, 0xFF00
        e_or2i          r7, 0x010A
        se_stw          r7, 0x0 (r6)  /* # WEN = 0 */

        /* step 3: lowlevel-init */
        e_lis           r1, e200z0h_isr_stack_top@h /* sp = e200z0h_isr_stack_top; */
        e_or2i          r1, e200z0h_isr_stack_top@l
        e_lis           r13, _SDA_BASE_@h
        e_or2i          r13, _SDA_BASE_@l
        e_lis           r2, _SDA2_BASE_@h
        e_or2i          r2, _SDA2_BASE_@l
        e_stwu          r1, -64 (r1) /* Reserve stack frame to backchain; */
        e_bl            xwos_preinit

        /* step 4: init xwos-stack & ecc flags */
        e_li            r4, 0
        e_lis           r1, e200z0h_isr_stack_top@h /* r1 = e200z0h_isr_stack_top; */
        e_or2i          r1, e200z0h_isr_stack_top@l
        e_lis           r3, e200z0h_isr_stack_mr_size@h /* r3 = e200z0h_isr_stack_mr_size */
        e_or2i          r3, e200z0h_isr_stack_mr_size@l
        e_srwi          r3, r3, 2 /* r3 = r3 / 4; */
        mtctr           r3 /* ctr = r3; */
loop:
        e_stwu          r4, -4 (r1) /* *(r1-4) = 0; r1 -= 4; */
        e_bdnz          loop /* Loop for stack */
        /* Now only SRAM in e200z0h_isr_stack_mr can be used. */
        /* ECC RAM non-correction error report disabled
           ECC FLASH non-correction error report disabled
           Reporting of single-bit RAM corrections disabled
           Reporting of single-bit flash corrections disabled */
        e_lis           r11, 0xFFF4
        e_or2i          r11, 0x0043
        e_li            r12, 0x0000
        e_stb           r12, 0 (r11)

        /* step 5: setup env of the C language */
        e_lis           r1, e200z0h_isr_stack_top@h
        e_or2i          r1, e200z0h_isr_stack_top@l
        e_lis           r13, _SDA_BASE_@h
        e_or2i          r13, _SDA_BASE_@l
        e_lis           r2, _SDA2_BASE_@h
        e_or2i          r2, _SDA2_BASE_@l
        e_li            r0, 0
        e_stwu          r1, -64 (r1)

        /* step 6: init */
        e_bl            xwos_init

        /* step 7: postinit */
        e_bl            xwos_postinit

        /* step 8: goto user main function */
        e_bl            xwos_main
arch_isr_reset_end:
        .endfunc
        .size           arch_isr_reset, arch_isr_reset_end - arch_isr_reset

/**
 * @brief Interrupt Vector Prefix
 */
        .section        .bam_bootarea,"a"
        .long           0x005A0000      /* BOOTID: Boot Identifier is: 005A0000*/
        .long           arch_isr_reset   /* entry address */

/**
 * @brief lowpower wakeup mode entry
 */
        .global         stdby_wkup_entry
        .section        .stdby_wkup,"ax"
        .func
        .align          4
stdby_wkup_entry:
        /* Re-enter in DRUN mode to update the configuration */
        /* ME.DRUN.R = 0x001F0010; */
        e_lis           r3, 0x001F
        se_addi         r3, 0x0010
        e_lis           r8, 0xC3FE
        e_stw           r3, -16340 (r8)

        /* ME.MCTL.R = 0x30005AF0; */
        e_lis           r9, 0x3000
        e_add16i        r9, r9, 23280
        e_lis           r8, 0xC3FE
        e_stw           r9, -16380 (r8)

        /* ME.MCTL.R = 0x3000A50F; */
        e_lis           r9, 0x3001
        e_add16i        r9, r9, -23281
        e_lis           r8, 0xC3FE
        e_stw           r9, -16380 (r8)

        /* while (0x1 == ME.GS.B.S_MTRANS) {} */
1:
        e_lis           r8, 0xC3FE
        e_lwz           r8, -16384 (r8)
        e_extrwi        r0, r8, 1, 4 /* e_rlwinm  r0,r8,5,31,31 */
        se_cmpli        r0, 0x0001
        se_beq          1b

        e_lis           r3, arch_isr_reset@h
        e_or2i          r3, arch_isr_reset@l
        se_mtlr         r3
        se_blr
        .endfunc
stdby_wkup_entry_end:
        .size           stdby_wkup_entry, . - stdby_wkup_entry
