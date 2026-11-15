/**
 * @file
 * @brief SOC描述层：初始化
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <xwcd/soc/arm/v7m/arch_init.h>

extern xwu8_t armv7m_isr_stack_top[];

void soc_lowlevel_init(void);
void soc_init_dtcm(void);
void soc_init_itcm(void);
void soc_init_ram(void);

/**
 * @brief SOC Boot Entry
 */
__xwbsp_isr __xwcc_naked
void soc_isr_reset(void)
{
        /* ++++++++ Init core regs ++++++++ */
        __asm__ volatile(
        "       cpsid   i\n"
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
        "       mov     r12, #0\n"
        "       ldr     r0, =armv7m_isr_stack_top\n"
        "       msr     msp, r0\n");
        /* ++++++++ Add some delay to wait for SYSCFG ++++++++ */
        __asm__ volatile(
        "       ldr     r1, =0x3000\n"
        "       movs    r0, #0\n"
        "wait_for_syscfg:\n"
        "       adds    r0, r0, #1\n"
        "       nop\n"
        "       cmp     r0, r1\n"
        "       bne     wait_for_syscfg\n");
        /* ++++++++ Enable SYSCFG clock ++++++++ */
        __asm__ volatile(
                "       ldr     r0, =0x49001018\n" // RCU_APB2EN
        "       ldr     r1, [r0]\n"
        "       orr     r1, r1, #1\n"
        "       str     r1, [r0]\n");
        /* ++++++++ Detect CPU ID ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =0x40010224\n" // SYSCFG_CPNUM
        "       ldr     r0, [r0]\n"
        "       ubfx    r0, r0, #0, #2\n");
        __asm__ volatile(
        "       tbh     [pc, r0, lsl #1]\n"
        "switch_table_cpuid:\n"
        "       .2byte  (cpu0_boot - switch_table_cpuid) / 2\n"
        "       .2byte  (cpu1_boot - switch_table_cpuid) / 2\n"
        "       .2byte  (unified_boot - switch_table_cpuid) / 2\n"
        "       .align  2\n");
        /* ++++++++ ++++++++ ++++++++ CPU0 ++++++++ ++++++++ ++++++++ */
        __asm__ volatile(
        "cpu0_boot:\n");
        __asm__ volatile(
        "       bl      soc_lowlevel_init");
        __asm__ volatile(
        "       bl      soc_init_dtcm");
        __asm__ volatile(
        "       bl      soc_init_itcm");
        __asm__ volatile(
        "       bl      soc_init_ram");
        __asm__ volatile(
        "       b       unified_boot");
        /* ++++++++ ++++++++ ++++++++ CPU1 ++++++++ ++++++++ ++++++++ */
        __asm__ volatile(
        "cpu1_boot:\n");
        __asm__ volatile(
        "       bl      soc_lowlevel_init");
        __asm__ volatile(
        "       bl      soc_init_dtcm");
        __asm__ volatile(
        "       bl      soc_init_itcm");
        __asm__ volatile(
        "       b       unified_boot");
        /* ++++++++ ++++++++ ++++++++ Unified Boot Flow ++++++++ ++++++++ ++++++++ */
        __asm__ volatile(
        "unified_boot:\n");
        __asm__ volatile(
        "       bl      xwos_preinit");
        __asm__ volatile(
        "       bl      xwos_init");
        __asm__ volatile(
        "       bl      xwos_postinit");
        __asm__ volatile(
        "       bl      xwos_main");
}

void soc_lowlevel_init(void)
{
        arch_init();
        armv7m_scs.scb.itcmcr.bit.en = 1U;
        armv7m_scs.scb.dtcmcr.bit.en = 1U;
}

extern xwu8_t sram_mr_origin[];
extern xwu8_t sram_mr_size[];
extern xwu8_t lpm_mr_origin[];
extern xwu8_t lpm_mr_size[];

void soc_init_ram(void)
{
        xwu64_t * origin;
        xwsz_t cnt;
        xwsz_t i;

        origin = (xwu64_t *)((xwu32_t)lpm_mr_origin + (xwu32_t)lpm_mr_size);
        cnt = ((xwsz_t)sram_mr_size - (xwsz_t)lpm_mr_size) / sizeof(xwu64_t);
        for (i = 0U; i < cnt; i++) {
                origin[i] = (xwu64_t)0;
        }

        /* TODO: 增加低功耗唤醒的判断，决定是否初始化低功耗保留数据区 */
}

extern xwu8_t dtcm_mr_origin[];
extern xwu8_t dtcm_mr_size[];

void soc_init_dtcm(void)
{
        xwu64_t * origin;
        xwsz_t cnt;
        xwsz_t i;

        origin = (xwu64_t *)dtcm_mr_origin;
        cnt = (xwsz_t)dtcm_mr_size / sizeof(xwu64_t);
        for (i = 0U; i < cnt; i++) {
                origin[i] = (xwu64_t)0;
        }
}

extern xwu8_t itcm_mr_origin[];
extern xwu8_t itcm_mr_size[];

void soc_init_itcm(void)
{
        xwu64_t * origin;
        xwsz_t cnt;
        xwsz_t i;

        origin = (xwu64_t *)itcm_mr_origin;
        cnt = (xwsz_t)itcm_mr_size / sizeof(xwu64_t);
        for (i = 0U; i < cnt; i++) {
                origin[i] = (xwu64_t)0;
        }
}

extern xwu8_t itcm_code_lma_base[];
extern xwu8_t itcm_code_vma_base[];
extern xwu8_t itcm_code_vma_end[];

extern xwu8_t cpu0_data_lma_base[];
extern xwu8_t cpu0_data_vma_base[];
extern xwu8_t cpu0_data_vma_end[];

extern xwu8_t cpu1_data_lma_base[];
extern xwu8_t cpu1_data_vma_base[];
extern xwu8_t cpu1_data_vma_end[];

extern xwu8_t cpu2_data_lma_base[];
extern xwu8_t cpu2_data_vma_base[];
extern xwu8_t cpu2_data_vma_end[];

extern xwu8_t cpu3_data_lma_base[];
extern xwu8_t cpu3_data_vma_base[];
extern xwu8_t cpu3_data_vma_end[];

extern xwu8_t dtcm_data_lma_base[];
extern xwu8_t dtcm_data_vma_base[];
extern xwu8_t dtcm_data_vma_end[];

extern xwu8_t dtcm_bss_vma_base[];
extern xwu8_t dtcm_bss_vma_end[];

extern xwu8_t lpm_data_lma_base[];
extern xwu8_t lpm_data_vma_base[];
extern xwu8_t lpm_data_vma_end[];

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
 * @brief 重定向低功耗保持数据到内存
 */
__xwbsp_init_code
void soc_relocate_lpm_data(void)
{
        xwid_t cpuid;
        xwsz_t cnt;
        xwsz_t i;
        xwu8_t * src;
        xwu8_t * dst;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (0U == cpuid) {
                src = lpm_data_lma_base;
                dst = lpm_data_vma_base;
                if (dst != src) {
                        cnt = ((xwsz_t)lpm_data_vma_end -
                               (xwsz_t)lpm_data_vma_base);
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
        switch (cpuid) {
        case 0U:
                src = cpu0_data_lma_base;
                dst = cpu0_data_vma_base;
                if (dst != src) {
                        cnt = (xwsz_t)cpu0_data_vma_end - (xwsz_t)cpu0_data_vma_base;
                        for (i = 0; i < cnt; i++) {
                                dst[i] = src[i];
                        }
                }
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
                break;
#if CPUCFG_CPU_NUM >= 2
        case 1U:
                src = cpu1_data_lma_base;
                dst = cpu1_data_vma_base;
                if (dst != src) {
                        cnt = (xwsz_t)cpu1_data_vma_end - (xwsz_t)cpu1_data_vma_base;
                        for (i = 0; i < cnt; i++) {
                                dst[i] = src[i];
                        }
                }
                break;
#endif
        default:
                break;
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
