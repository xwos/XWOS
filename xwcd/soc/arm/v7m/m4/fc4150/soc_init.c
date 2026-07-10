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

extern xwu8_t lpm_data_lma_base[];
extern xwu8_t lpm_data_vma_base[];
extern xwu8_t lpm_data_vma_end[];

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

#define WDOG0_BASE_ADDR                 (0x40052000U)
#define WDOG1_BASE_ADDR                 (0x40053000U)

#define WDOG_CS_OFFSET                  (0x0000U) /* Watchdog Control and Status Register */
#define WDOG_COUNTER_OFFSET             (0x0004U) /* Watchdog Counter Register */
#define WDOG_TIMEOUT_OFFSET             (0x0008U) /* Watchdog Timeout Value Register */
#define WDOG_WINDOW_OFFSET              (0x000CU) /* Watchdog Window Register */

#define WDOG_CS_UPDATE                  (1U << 5U)  /* Bit 5:  Allow updates */
#define WDOG_CS_CLK_SEL_SHIFT           (8U) /* Bits 8-9: Watchdog Clock */
#define WDOG_CS_CLK_SEL_MASK            (3U << WDOG_CS_CLK_SEL_SHIFT)
#define WDOG_CS_CLK_SEL_BUS_CLK         (0U << WDOG_CS_CLK_SEL_SHIFT) /* Bus clock */
#define WDOG_CS_CLK_SEL_AON_CLK         (1U << WDOG_CS_CLK_SEL_SHIFT) /* AON clock */
#define WDOG_CS_CLK_SEL_SOSC_CLK        (2U << WDOG_CS_CLK_SEL_SHIFT) /* SOSC clock */
#define WDOG_CS_CLK_SEL_SIRC_CLK        (3U << WDOG_CS_CLK_SEL_SHIFT) /* SIRC clock */
#define WDOG_CS_RECFG_STAT              (1U << 10U) /* Bit 10: Reconfiguration Success */
#define WDOG_CS_ULK_STAT                (1U << 11U) /* Bit 11: Unlock status */
#define WDOG_CS_PRESCALER_SHIFT         (12U) /* Bit 12: Watchdog 256 prescale enable/disable */
#define WDOG_CS_PRESCALER_MASK          (1U << WDOG_CS_PRESCALER_SHIFT)
#define WDOG_CS_PRESCALER_ENABLE        (1U << WDOG_CS_PRESCALER_SHIFT) /* Bit 12: Watchdog prescalr */

#define WDOG_COUNTER_UNLOCK             (0x08181982U)  /* Value to unlock the watchdog registers */
#define WDOG_CS_DISABLE_WDOG            (WDOG_CS_UPDATE | WDOG_CS_CLK_SEL_AON_CLK | \
                                         WDOG_CS_PRESCALER_ENABLE | WDOG_CS_ULK_STAT)

void soc_disable_wdog0(void)
{
        register xwu32_t val;
        register xwu32_t try = 128u;

        /* If it is not the first time to configure wdog, unlock status will only
           persist for 128 bus clocks. */
        while (0UL != try) {
                val = xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET);
                if ((val & WDOG_CS_ULK_STAT) == 0U) {
                        break;
                }
                try--;
        }
        /* If ULK_STAT turns into 0 in 128 try counts, it means this is not the
           first time to configure the wdog. */
        if (0UL != try) {
                /* When ULK_STAT = 0, the wdog can only be unlocked when RECFG_STAT
                   becomes 1. */
                do {
                        val = xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET);
                } while ((val & WDOG_CS_RECFG_STAT) == 0U);

                /* Unlock the wdog.
                   Note: The unlock status only persist for 128 bus clocks.
                   SHALL NOT use single-step or break points in the following lines. */
                xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_COUNTER_OFFSET) =
                        WDOG_COUNTER_UNLOCK;

                do {
                        /* Wait until the unlock take effect. */
                        val = xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET);
                } while ((val & WDOG_CS_ULK_STAT) == 0U);
        }
        /* Disable Watchdog */
        xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET) = WDOG_CS_DISABLE_WDOG;
        xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_TIMEOUT_OFFSET) = 0xFFFFU;
        /* Wait the RECFG_STAT to become 1. */
        do {
                val = xwmb_access(xwu32_t, WDOG0_BASE_ADDR + WDOG_CS_OFFSET);
        } while ((val & WDOG_CS_RECFG_STAT) == 0U);
}

void soc_disable_wdog1(void)
{
        register xwu32_t val;
        register xwu32_t try = 128u;

        /* If it is not the first time to configure wdog, unlock status will only
           persist for 128 bus clocks. */
        while (0UL != try) {
                val = xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET);
                if ((val & WDOG_CS_ULK_STAT) == 0U) {
                        break;
                }
                try--;
        }
        /* If ULK_STAT turns into 0 in 128 try counts, it means this is not the
           first time to configure the wdog. */
        if (0UL != try) {
                /* When ULK_STAT = 0, the wdog can only be unlocked when RECFG_STAT
                   becomes 1. */
                do {
                        val = xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET);
                } while ((val & WDOG_CS_RECFG_STAT) == 0U);

                /* Unlock the wdog.
                   Note: The unlock status only persist for 128 bus clocks.
                   SHALL NOT use single-step or break points in the following lines. */
                xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_COUNTER_OFFSET) =
                        WDOG_COUNTER_UNLOCK;

                do {
                        /* Wait until the unlock take effect. */
                        val = xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET);
                } while ((val & WDOG_CS_ULK_STAT) == 0U);
        }
        /* Disable Watchdog */
        xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET) = WDOG_CS_DISABLE_WDOG;
        xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_TIMEOUT_OFFSET) = 0xFFFFU;
        /* Wait the RECFG_STAT to become 1. */
        do {
                val = xwmb_access(xwu32_t, WDOG1_BASE_ADDR + WDOG_CS_OFFSET);
        } while ((val & WDOG_CS_RECFG_STAT) == 0U);
}

void soc_disable_wdg(void)
{
        soc_disable_wdog0();
        soc_disable_wdog1();
}
