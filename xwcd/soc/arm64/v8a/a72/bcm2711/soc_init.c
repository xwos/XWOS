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

#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_init.h>
#include <xwos/ospl/skd.h>
#include <xwcd/soc/arm64/v8a/arch_regs.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_init.h>
#include <xwcd/soc/arm64/v8a/arch_exception.h>
#include <xwcd/soc/arm64/v8a/arch_gic2.h>
#include <xwcd/soc/arm64/v8a/arch_timer.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_gpio.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_uart.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_console.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_cache.h>

/* #define SOC_DBGF */
#include "soc_debug.h"

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

/**
 * @brief 重定向数据区到内存
 */
__xwbsp_init_code
void soc_clear_bss(void)
{
        xwsz_t cnt, i;
        xwu8_t * dst;

        dst = bss_vma_base;
        cnt = (xwsz_t)bss_vma_end - (xwsz_t)bss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }

        dst = tbss_vma_base;
        cnt = (xwsz_t)tbss_vma_end - (xwsz_t)tbss_vma_base;
        for (i = 0; i < cnt; i++) {
                *dst = 0;
                dst++;
        }
}

__xwbsp_code
void soc_init_sysreg(void)
{
        xwu64_t cpuectlr;
        xwu64_t actlr;
        xwu64_t el;
        xwu64_t l2ctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        switch (el) {
        case 3:
                actlr = 0x73; /* (BIT(0) | BIT(1) | BIT(4) | BIT(5) | BIT(6)) */
                armv8a_sysreg_write(actlr_el3, actlr);
                soc_show_sysreg(actlr_el3);

                [[fallthrough]];
        case 2:
                actlr = 0x73; /* (BIT(0) | BIT(1) | BIT(4) | BIT(5) | BIT(6)) */
                armv8a_sysreg_write(actlr_el2, actlr);
                soc_show_sysreg(actlr_el2);

                /* LOCAL_PRESCALER; divide-by (0x80000000 / register_val) == 1 */
                soc_armlocal.local_prescaler = 0x80000000;

                [[fallthrough]];
        case 1:
        default:
                cpuectlr = (1U << 6U); /* Set SMPEN */
                armv8a_sysreg_write(S3_1_C15_C2_1, cpuectlr);

                armv8a_sysreg_read(&l2ctlr, S3_1_C11_C0_2);
                l2ctlr |= 0x22U;
                armv8a_sysreg_write(S3_1_C11_C0_2, l2ctlr);
        }


}

/**
 * @brief SOC的初始化
 */
__xwbsp_init_code
void soc_init(void)
{
        xwid_t cpuid;
        xwu64_t el;

        cpuid = xwospl_skd_get_cpuid_lc();
        if (0 == cpuid) {
                soc_console_init();
        }
        armv8a_init_vector();
        armv8a_init();
        soc_init_sysreg();
        armv8a_timer_init();

        if (CPUCFG_MAIN_CPU == cpuid) {
                soc_clear_bss();
        }

        /* soc_icache_enable(); */
        soc_dcache_enable();

        if (CPUCFG_MAIN_CPU == cpuid) {
                armv8a_gic2_init_runtime();
                armv8a_gic2_init_distributor();
        }
        armv8a_gic2_init_cpuif();

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        soc_infof("BCM2711",
                  "******** XWOS Start on CPU%dEL%d ********\n\r",
                  cpuid, el);
        if (3 == el) {
                armv8a_switch_el3_to_el2_aarch64();
                soc_infof("BCM2711", "Drop CPU%d to EL%d\n\r", cpuid, el - 1);
        }
}

__xwcc_section(".armv8a.percpu.stack") __xwcc_aligned(0x800)
xwu8_t soc_percpu_stack[CPUCFG_CPU_NUM][CPUCFG_CPU_STACK_SIZE];

/**
 * @brief BCM2711的CPU启动入口
 * @param[in] sp: 栈指针
 * @details
 * 此函数由bootloader调用，bootloader需要传递栈指针给此函数。
 */
__xwcc_section(".armv8a.boot")
void soc_percpu_boot(void)
{
        __asm__ volatile(
        "       msr     daifclr, #0xC\n"
        "       msr     daifset, #0x3\n"
        : : :
        );
        /* get stack */
        __asm__ volatile(
        "       msr     spsel, #1\n"
        "       mrs     x0, mpidr_el1\n"
        "       ubfx    x0, x0, #8, #8\n"
        "       mov     x1, #%[__stack_size]\n"
        "       mul     x1, x1, x0\n"
        "       ldr     x2, =soc_percpu_stack\n"
        "       add     sp, x2, x1\n"
        :
        : [__stack_size] "L" (CPUCFG_CPU_STACK_SIZE)
        : "x0", "x1", "x2"
        );
        __asm__ volatile(
        "       mov     fp, xzr" /* set fp to 0 to end the backtrace */
        : : :
        );
        __asm__ volatile(
        "       bl      xwos_preinit"
        : : :
        );
        __asm__ volatile(
        "       bl      xwos_init"
        : : :
        );
        __asm__ volatile(
        "       bl      xwos_postinit"
        : : :
        );
        __asm__ volatile(
        "       bl      xwos_main"
        : : :
        );
}
