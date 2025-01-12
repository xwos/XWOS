/**
 * @file
 * @brief SOC描述层：初始化
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

#include <xwcd/soc/arm/v7m/m7/fc7300/soc_init.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm/v7m/armv7m_isa.h>

extern xwu8_t itcm_mr_origin[];
extern xwu8_t itcm_mr_size[];
extern xwu8_t dtcm_mr_origin[];
extern xwu8_t dtcm_mr_size[];

/**
 * @brief SOC Boot Entry
 */
__xwbsp_isr __xwcc_naked
void soc_isr_reset(void)
{
        /* ++++++++ Init core regs ++++++++ */
        __asm__ volatile(
        "       cpsid   i\n"
        "       mov     r0, #0\n"
        "       mov     r1, #0\n"
        "       mov     r2, #0\n"
        "       mov     r3, #0\n"
        "       mov     r4, #0\n"
        "       mov     r5, #0\n"
        "       mov     r6, #0\n"
        "       mov     r7, #0\n"
        "       mov     r8, #0\n"
        "       mov     r9, #0\n"
        "       mov     r10, #0\n"
        "       mov     r11, #0\n"
        "       mov     r12, #0\n");
        /* ++++++++ Detect CPU ID ++++++++ */
        __asm__ volatile(
        "       ldr     r1, =0xE0080000\n"
        "       ldr     r0, [r1, #4]\n"
        "       ldr     r1, [r1, #32]\n"
        "       ubfx    r0, r0, #17, #3\n");
        __asm__ volatile(
        "       tbh     [pc, r0, lsl #1]\n"
        "switch_table_cpuid:\n"
        "       .2byte  (cpu0_startup - switch_table_cpuid) / 2\n"
        "       .2byte  (cpu1_startup - switch_table_cpuid) / 2\n"
        "       .2byte  (cpu2_startup - switch_table_cpuid) / 2\n"
        "       .2byte  (unified_startup - switch_table_cpuid) / 2\n"
        "       .align  2\n");
        /* ++++++++ ++++++++ ++++++++ CPU0 ++++++++ ++++++++ ++++++++ */
        __asm__ volatile(
        "cpu0_startup:\n");
        /* ++++++++ Errata MCU debug Issue 1: lockstep lost ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =0xE000EDFC\n"
        "       ldr     r1, =0x01000000\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0xE0001004\n"
        "       ldr     r1, =0x0\n"
        "       str     r1, [r0, #0]\n");
        /* ++++++++ Disable wdog 0 ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =0x40022004\n"
        "       ldr     r1, =0x08181982\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0x40022000\n"
        "       ldr     r1, =0x2920\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0x40022008\n"
        "       ldr     r1, =0xF000\n"
        "       str     r1, [r0, #0]\n");
        /* ++++++++ Errata MCU debug Issue 2: ITCM error hardfault ++++++++ */
        /* Shoult set ITCM initial auto enable in NVR */
        /* Disable AXBS/CPU0/1/2/ ECC */
        __asm__ volatile(
        "       ldr     r0, =0x40072020\n"
        "       ldr     r1, =0xAAAAAAAA\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072024\n"
        "       ldr     r1, =0x000003AA\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072028\n"
        "       ldr     r1, =0x00000AAA\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072030\n"
        "       ldr     r1, =0x00000AAA\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072038\n"
        "       ldr     r1, =0x00000AAA\n"
        "       str     r1, [r0, #0]\n");
        /* Enable PCC.STCU */
        __asm__ volatile(
        "       ldr     r0, =0x400241FC\n"
        "       ldr     r1, =0x00800000\n"
        "       str     r1, [r0, #0]\n");
        /* Clear all itcm dtcm sram */
        __asm__ volatile(
        "       ldr     r0, =0x4007f050\n"
        "       ldr     r1, =0x1FF0\n"
        "       str     r1, [r0,#0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x4007f048\n"
        "       ldr     r1, =0x1\n"
        "       str     r1,[r0,#0]\n");
        __asm__ volatile(
        "       mov     r2, #1\n"
        "loop_busy:\n"
        "       ldr     r0, =0x4007f04C\n"
        "       ldr     r1, [r0, #0]\n"
        "       cmp     r1, r2\n"
        "       bne     loop_busy\n");
        /* Enable AXBS/CPU0/1/2/ ECC */
        __asm__ volatile(
        "       ldr     r0 ,=0x40072020\n"
        "       ldr     r1, =0x0FFFFFFF\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072024\n"
        "       ldr     r1, =0x000003FF\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072028\n"
        "       ldr     r1, =0x00000FFF\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072030\n"
        "       ldr     r1, =0x00000FFF\n"
        "       str     r1, [r0, #0]\n");
        __asm__ volatile(
        "       ldr     r0, =0x40072038\n"
        "       ldr     r1, =0x00000FFF\n"
        "       str     r1, [r0, #0]\n");
        /* ++++++++ Jump to unified flow ++++++++ */
        __asm__ volatile(
        "       b       unified_startup\n");
        /* -------- -------- -------- CPU0 -------- -------- -------- */
        /* ++++++++ ++++++++ ++++++++ CPU1 ++++++++ ++++++++ ++++++++ */
        __asm__ volatile(
        "cpu1_startup:\n");
        /* ++++++++ Errata MCU debug Issue 1: lockstep lost ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =0xE000EDFC\n"
        "       ldr     r1, =0x01000000\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0xE0001004\n"
        "       ldr     r1, =0x0\n"
        "       str     r1, [r0, #0]\n");
        /* ++++++++ Disable wdog 1 ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =0x40433004\n"
        "       ldr     r1, =0x08181982\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0x40433000\n"
        "       ldr     r1, =0x2920\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0x40433008\n"
        "       ldr     r1, =0xF000\n"
        "       str     r1, [r0, #0]\n");
        /* ++++++++ Jump to unified flow ++++++++ */
        __asm__ volatile(
        "       b       unified_startup\n");
        /* -------- -------- -------- CPU1 -------- -------- -------- */
        /* ++++++++ ++++++++ ++++++++ CPU2 ++++++++ ++++++++ ++++++++ */
        __asm__ volatile(
        "cpu2_startup:\n");
        /* ++++++++ Disable wdog 2 ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =0x40434004\n"
        "       ldr     r1, =0x08181982\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0x40434000\n"
        "       ldr     r1, =0x2920\n"
        "       str     r1, [r0, #0]\n"
        "       ldr     r0, =0x40434008\n"
        "       ldr     r1, =0xF000\n"
        "       str     r1, [r0, #0]\n");
        /* ++++++++ Jump to unified flow ++++++++ */
        __asm__ volatile(
        "       b       unified_startup\n");
        /* -------- -------- -------- CPU2 -------- -------- -------- */
        __asm__ volatile(
        "unified_startup:\n");
        /* ++++++++ Fill TCM with 0 ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =dtcm_mr_origin\n"
        "       ldr     r1, =dtcm_mr_size\n"
        "       movs    r2, 0\n"
        "       movs    r3, 0\n"
        "loop_dtcm_init:\n"
        "       stm     r0!, {r2,r3}\n"
        "       subs    r1, #8\n"
        "       bgt     loop_dtcm_init\n");
        __asm__ volatile(
        "       ldr     r0, =itcm_mr_origin\n"
        "       ldr     r1, =itcm_mr_size\n"
        "       movs    r2, 0\n"
        "       movs    r3, 0\n"
        "loop_itcm_init:\n"
        "       stm     r0!, {r2,r3}\n"
        "       subs    r1, #8\n"
        "       bgt     loop_itcm_init\n");
        /* -------- Fill TCM with 0 -------- */
        __asm__ volatile(
        "       ldr     r0, =armv7m_isr_stack_top\n"
        "       msr     msp, r0\n");
        __asm__ volatile(
        "       bl      xwos_preinit");
        __asm__ volatile(
        "       bl      xwos_init");
        __asm__ volatile(
        "       bl      xwos_postinit");
        __asm__ volatile(
        "       bl      xwos_main");
}

extern xwu8_t itcm_code_lma_base[];
extern xwu8_t itcm_code_vma_base[];
extern xwu8_t itcm_code_vma_end[];

extern xwu8_t dtcm_data_lma_base[];
extern xwu8_t dtcm_data_vma_base[];
extern xwu8_t dtcm_data_vma_end[];

extern xwu8_t dtcm_bss_vma_base[];
extern xwu8_t dtcm_bss_vma_end[];

extern xwu8_t standby_data_lma_base[];
extern xwu8_t standby_data_vma_base[];
extern xwu8_t standby_data_vma_end[];

extern xwu8_t xwos_data_lma_base[];
extern xwu8_t xwos_data_vma_base[];
extern xwu8_t xwos_data_vma_end[];

extern xwu8_t data_lma_base[];
extern xwu8_t data_vma_base[];
extern xwu8_t data_vma_end[];

extern xwu8_t bss_vma_base[];
extern xwu8_t bss_vma_end[];

extern xwu8_t tdata_lma_base[];
extern xwu8_t tdata_vma_base[];
extern xwu8_t tdata_vma_end[];

extern xwu8_t tbss_vma_base[];
extern xwu8_t tbss_vma_end[];

extern xwu8_t noncacheable_data_lma_base[];
extern xwu8_t noncacheable_data_vma_base[];
extern xwu8_t noncacheable_data_vma_end[];

extern xwu8_t noncacheable_bss_vma_base[];
extern xwu8_t noncacheable_bss_vma_end[];

extern xwu8_t shareable_data_lma_base[];
extern xwu8_t shareable_data_vma_base[];
extern xwu8_t shareable_data_vma_end[];

extern xwu8_t shareable_bss_vma_base[];
extern xwu8_t shareable_bss_vma_end[];


/**
 * @brief 重定向STANDBY数据到内存
 */
__xwbsp_init_code
void soc_relocate_standby_data(void)
{
        xwid_t cpuid;
        xwsz_t cnt;
        xwsz_t i;
        xwu8_t * src;
        xwu8_t * dst;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (0U == cpuid) {
                src = standby_data_lma_base;
                dst = standby_data_vma_base;
                if (dst != src) {
                        cnt = ((xwsz_t)standby_data_vma_end -
                               (xwsz_t)standby_data_vma_base);
                        for (i = 0; i < cnt; i++) {
                                dst[i] = src[i];
                        }
                }
        }
}

/**
 * @brief 重定向数据区到内存
 */
__xwbsp_init_code
void soc_relocate_data(void)
{
        xwid_t cpuid;
        xwsz_t cnt;
        xwsz_t i;
        xwu8_t * src;
        xwu8_t * dst;

        src = itcm_code_lma_base;
        dst = itcm_code_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)itcm_code_vma_end - (xwsz_t)itcm_code_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }

        src = dtcm_data_lma_base;
        dst = dtcm_data_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)dtcm_data_vma_end - (xwsz_t)dtcm_data_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }

        dst = dtcm_bss_vma_base;
        cnt = (xwsz_t)dtcm_bss_vma_end - (xwsz_t)dtcm_bss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }

        cpuid = xwospl_skd_get_cpuid_lc();
        if (0U == cpuid) {
                src = xwos_data_lma_base;
                dst = xwos_data_vma_base;
                if (dst != src) {
                        cnt = (xwsz_t)xwos_data_vma_end - (xwsz_t)xwos_data_vma_base;
                        for (i = 0; i < cnt; i++) {
                                dst[i] = src[i];
                        }
                }

                src = data_lma_base;
                dst = data_vma_base;
                if (dst != src) {
                        cnt = (xwsz_t)data_vma_end - (xwsz_t)data_vma_base;
                        for (i = 0; i < cnt; i++) {
                                *dst = *src;
                                dst++;
                                src++;
                        }
                }

                dst = bss_vma_base;
                cnt = (xwsz_t)bss_vma_end - (xwsz_t)bss_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = 0;
                        dst++;
                }

                src = tdata_lma_base;
                dst = tdata_vma_base;
                if (dst != src) {
                        cnt = (xwsz_t)tdata_vma_end - (xwsz_t)tdata_vma_base;
                        for (i = 0; i < cnt; i++) {
                                *dst = *src;
                                dst++;
                                src++;
                        }
                }

                dst = tbss_vma_base;
                cnt = (xwsz_t)tbss_vma_end - (xwsz_t)tbss_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = 0;
                        dst++;
                }

                src = noncacheable_data_lma_base;
                dst = noncacheable_data_vma_base;
                if (dst != src) {
                        cnt = ((xwsz_t)noncacheable_data_vma_end -
                               (xwsz_t)noncacheable_data_vma_base);
                        for (i = 0; i < cnt; i++) {
                                dst[i] = src[i];
                        }
                }

                dst = noncacheable_bss_vma_base;
                cnt = ((xwsz_t)noncacheable_bss_vma_end -
                       (xwsz_t)noncacheable_bss_vma_base);
                for (i = 0; i < cnt; i++) {
                        *dst = 0;
                        dst++;
                }

                src = shareable_data_lma_base;
                dst = shareable_data_vma_base;
                if (dst != src) {
                        cnt = ((xwsz_t)shareable_data_vma_end -
                               (xwsz_t)shareable_data_vma_base);
                        for (i = 0; i < cnt; i++) {
                                dst[i] = src[i];
                        }
                }

                dst = shareable_bss_vma_base;
                cnt = (xwsz_t)shareable_bss_vma_end - (xwsz_t)shareable_bss_vma_base;
                for (i = 0; i < cnt; i++) {
                        *dst = 0;
                        dst++;
                }
        }
}

extern xwu8_t armv7m_ivt_lma_base[];
extern xwu8_t armv7m_ivt_vma_base[];
extern xwu8_t armv7m_ivt_vma_end[];

/**
 * @brief 重定向中断向量表到内存
 */
__xwos_init_code
void soc_relocate_ivt(void)
{
        xwsz_t cnt;
        xwsz_t i;
        xwu8_t * src;
        xwu8_t * dst;

        src = armv7m_ivt_lma_base;
        dst = armv7m_ivt_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)armv7m_ivt_vma_end - (xwsz_t)armv7m_ivt_vma_base;
                for (i = 0; i < cnt; i++) {
                        dst[i] = src[i];
                }
        }
        armv7m_scs.scb.vtor.u32 = (xwu32_t)armv7m_ivt_vma_base;
}
