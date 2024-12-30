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

#include <xwcd/soc/arm/v7m/m4/fc4150/soc_init.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm/v7m/armv7m_isa.h>

extern xwu8_t armv7m_isr_stack_mr_origin[];
extern xwu8_t armv7m_isr_stack_mr_size[];

/**
 * @brief SOC Boot Entry
 */
__xwbsp_isr __xwcc_naked
void soc_isr_reset(void)
{
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
        /* ++++++++ Init ISR Stack ++++++++ */
        __asm__ volatile(
        "       ldr     r0, =armv7m_isr_stack_mr_origin\n"
        "       ldr     r1, =armv7m_isr_stack_mr_size\n"
        "       movs    r2, 0\n"
        "       movs    r3, 0\n"
        "loop_armv7m_isr_stack_init:\n"
        "       stm     r0!, {r2,r3}\n"
        "       subs    r1, #8\n"
        "       bgt     loop_armv7m_isr_stack_init\n");
        /* -------- Init ISR Stack -------- */
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

extern xwu8_t ramcode_lma_base[];
extern xwu8_t ramcode_vma_base[];
extern xwu8_t ramcode_vma_end[];

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

/**
 * @brief 重定向数据区到内存
 */
__xwbsp_init_code
void soc_relocate_data(void)
{
        xwsz_t cnt;
        xwsz_t i;
        xwu8_t * src;
        xwu8_t * dst;

        src = ramcode_lma_base;
        dst = ramcode_vma_base;
        if (dst != src) {
                cnt = (xwsz_t)ramcode_vma_end - (xwsz_t)ramcode_vma_base;
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
        xwsz_t cnt, i;
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

void soc_disable_wdg(void)
{
        /* disable wdog 0 */
        *(volatile xwu32_t *)0x40052004 = 0x08181982;
        while (0U == (0x800u & *(volatile xwu32_t *)0x40052000));
        *(volatile xwu32_t *)0x40052000 = 0x2920;
        *(volatile xwu32_t *)0x40052008 = 0xF000;
        while (0U == (0x400u & *(volatile xwu32_t *)0x40052000));

        /* disable wdog 1 */
        *(volatile xwu32_t *)0x40053004 = 0x08181982;
        while (0U == (0x800u & *(volatile xwu32_t *)0x40053000));
        *(volatile xwu32_t *)0x40053000 = 0x2920;
        *(volatile xwu32_t *)0x40053008 = 0xF000;
        while (0U == (0x400u & *(volatile xwu32_t *)0x40053000));
}
