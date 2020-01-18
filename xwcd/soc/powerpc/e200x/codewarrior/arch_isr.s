/**
 * @file
 * @brief e200z0 core exception handlers
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
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
        .equ            INTC_IACKR, 0xFFF48010  /* Interrupt Acknowledge Reg. addr. */
        .equ            INTC_EOIR, 0xFFF48018   /* End Of Interrupt Reg. addr. */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
        .extern         arch_privilege_start_entry
        .extern         arch_privilege_end_entry
        .extern         soc_reset_hard
        .extern         soc_eii_isr

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
        .section        .xwos.rodata
        .align          4
sc_table:
        .long           sc_noop /* sc 0 */
        .long           arch_privilege_start_entry /* sc 1 */
        .long           arch_privilege_end_entry /* sc 2 */
        .long           sc_noop /* sc 3 */
        .long           sc_noop /* sc 4 */
        .long           sc_noop /* sc 5 */
        .long           sc_noop /* sc 6 */
        .long           sc_noop /* sc 7 */
        .long           sc_noop /* sc 8 */
        .size           sc_table, . - sc_table

/**
 * @brief exeception handlers
 */
        .section        .exec_vect_vle,text_vle
        .global         arch_isrtable
        .align          16 /* Use .align 16 for Metrowerks.
                              Use .align 4 for Diab and GreenHills. */
arch_isrtable:
        .org 0x0000
        e_b             arch_isr_cii    /* critical input interrupt */
        .org 0x0010
        e_b             arch_isr_mci    /* machine check interrupt */
        .org 0x0020
        e_b             arch_isr_dsi    /* data Storage */
        .org 0x0030
        e_b             arch_isr_isi    /* instruction storage interrupt */
        .org 0x0040
        e_b             soc_eii_isr     /* external input interrupt */
        .org 0x0050
        e_b             arch_isr_ai     /* alignment interrupt */
        .org 0x0060
        e_b             arch_isr_pi     /* program interrupt */
        .org 0x0080
        e_b             arch_isr_sc     /* System call interrupt */
        .org 0x00f0
        e_b             arch_isr_dbg    /* Debug interrupt */
        .size           arch_isrtable, . - arch_isrtable

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief init IVPR
 */
        .section        .xwos.init.text,text_vle
        .global         arch_ivpr_init
        .align          4
        .function       "arch_ivpr_init", arch_ivpr_init, \
                        arch_ivpr_init_end - arch_ivpr_init
arch_ivpr_init:
        e_lis           r3, arch_isrtable@h
        e_or2i          r3, arch_isrtable@l
        mtivpr          r3
        se_blr
arch_ivpr_init_end:
        .size           arch_ivpr_init, . - arch_ivpr_init

/**
 * @brief critical input interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_cii
        .align          4
        .function       "arch_isr_cii", arch_isr_cii, arch_isr_cii_end - arch_isr_cii
arch_isr_cii:
        e_stwu          r1, -0x50 (r1)
        e_b             *
        e_add16i        r1, r1, 0x50
        se_rfi
arch_isr_cii_end:
        .size           arch_isr_cii, . - arch_isr_cii

/**
 * @brief machine check interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_mci
        .align          4
        .function       "arch_isr_mci", arch_isr_mci, arch_isr_mci_end - arch_isr_mci
arch_isr_mci:
        e_stwu          r1, -0x50 (r1)
        e_bl            soc_reset_hard
        e_b             *
        e_add16i        r1, r1, 0x50
        se_rfi
arch_isr_mci_end:
        .size           arch_isr_mci, . - arch_isr_mci

/**
 * @brief data Storage interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_dsi
        .align          4
        .function       "arch_isr_dsi", arch_isr_dsi, arch_isr_dsi_end - arch_isr_dsi
arch_isr_dsi:
        e_stwu          r1, -0x50 (r1)
        e_bl            soc_reset_hard
        e_b             *
        e_add16i        r1, r1, 0x50
        se_rfi
arch_isr_dsi_end:
        .size           arch_isr_dsi, . - arch_isr_dsi

/**
 * @brief instruction storage interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_isi
        .align          4
        .function       "arch_isr_isi", arch_isr_isi, arch_isr_isi_end - arch_isr_isi
arch_isr_isi:
        e_stwu          r1, -0x50 (r1)
        e_bl            soc_reset_hard
        e_b             *
        e_add16i        r1, r1, 0x50
        se_rfi
arch_isr_isi_end:
        .size           arch_isr_isi, . - arch_isr_isi

/**
 * @brief alignment interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_ai
        .align          4
        .function       "arch_isr_ai", arch_isr_ai, arch_isr_ai_end - arch_isr_ai
arch_isr_ai:
        e_stwu          r1, -0x50 (r1)
        e_bl            soc_reset_hard
        e_b             *
        e_add16i        r1, r1, 0x50
        se_rfi
arch_isr_ai_end:
        .size           arch_isr_ai, arch_isr_ai_end - arch_isr_ai

/**
 * @brief program interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_pi
        .align          4
        .function       "arch_isr_pi", arch_isr_pi, arch_isr_pi_end - arch_isr_pi
arch_isr_pi:
        e_stwu          r1, -0x50 (r1)
        e_bl            soc_reset_hard
        e_b             *
        e_add16i        r1, r1, 0x50
        se_rfi
arch_isr_pi_end:
        .size           arch_isr_pi, arch_isr_pi_end - arch_isr_pi

/**
 * @brief System call interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_sc
        .align          4
        .function       "arch_isr_sc", arch_isr_sc, arch_isr_sc_end - arch_isr_sc
arch_isr_sc:
        e_stwu          r1, -32 (r1)            /* reserved stack frame for compiler */
        se_stw          r4, 16 (r1)             /* save arg1 */
        mfsrr1          r4                      /* save srr1; */
        se_stw          r4, 28 (r1)
        mfsrr0          r4                      /* save srr0; */
        e_stw           r4, 24 (r1)
        wrteei          1                       /* enable external interrupt */
        se_mflr         r0
        se_stw          r0, 36 (r1)
        se_stw          r5, 20 (r1)             /* save arg2 */

        /* get system call handler */
        e_lis           r4, sc_table@h          /* r4 = sc_table */
        e_or2i          r4, sc_table@l
        e_slwi          r3, r3, 2               /* r3 *= 4; */
        lwzx            r5, r3, r4              /* r5 = sc_table[sc_no]; */
        se_mtlr         r5                      /* lr = r5; */
        se_lwz          r3, 16 (r1)             /* r3 = arg1; (old r4) */
        se_lwz          r4, 20 (r1)             /* r4 = arg2; (old r5) */
        e_addi          r5, r1, 24              /* r5 = &(srr0) */
        e_addi          r6, r1, 28              /* r6 = &(srr1) */

        /* branch to sc function */
        se_blrl                                 /* (r3, r4) = (*lr)(r3, r4, r5, r6); */

        se_lwz          r0, 36 (r1)
        se_mtlr         r0
        wrteei          0
        /* r5 is the 3rd augrment of system call, so don't need to restore it. */
        e_lwz           r5, 24 (r1)           /* restore srr0; */
        mtsrr0          r5
        se_lwz          r5, 28 (r1)           /* restore srr1; */
        mtsrr1          r5
        se_addi         r1, 32  /* delete reserved frame (used by compiler) */
        se_rfi /* r3 and r4 is result when system call. */
arch_isr_sc_end:
        .size           arch_isr_sc, arch_isr_sc_end - arch_isr_sc

        .align          4
        .function       "sc_noop", sc_noop, sc_noop_end - sc_noop
sc_noop:
        se_blr
sc_noop_end:
        .size           sc_noop, sc_noop_end - sc_noop

/**
 * @brief Debug interrupt handler
 */
        .section        .xwos.isr.text,text_vle
        .global         arch_isr_dbg
        .align          4
        .function       "arch_isr_dbg", arch_isr_dbg, arch_isr_dbg_end - arch_isr_dbg
arch_isr_dbg:
        e_stwu          r1, -0x50 (r1)
        e_b             *
        e_add16i        r1, r1, 0x50
        se_rfi
arch_isr_dbg_end:
        .size           arch_isr_dbg, . - arch_isr_dbg
