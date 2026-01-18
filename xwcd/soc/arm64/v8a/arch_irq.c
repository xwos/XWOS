/**
 * @file
 * @brief 架构描述层：陷阱
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

#include <xwos/standard.h>
#include <xwcd/soc/arm64/v8a/arch_exception.h>
#if defined(ARCHCFG_GICV2) && (1 == ARCHCFG_GICV2)
#  include <xwcd/soc/arm64/v8a/arch_gic2.h>
#endif
#if defined(ARCHCFG_GICV3) && (1 == ARCHCFG_GICV3)
#  include <xwcd/soc/arm64/v8a/arch_gic3.h>
#endif
#include <xwos/ospl/skd.h>

/* #define SOC_DBGF */
#include <soc_debug.h>

__xwcc_section(".armv8a.exception.text.el1")
void armv8a_irq_el1(__xwcc_unused struct armv8a_reg_frame * regs,
                    __xwcc_unused xwu64_t routine)
{
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        soc_dbgf("IRQ", "IRQ exception 0x%x detected on CPU%d@EL1\r\n",
                 routine, cpuid);
        armv8a_gic_isr(cpuid);
}

__xwcc_section(".armv8a.exception.text.el1")
void armv8a_fiq_el1(__xwcc_unused struct armv8a_reg_frame * regs,
                    __xwcc_unused xwu64_t routine)
{
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        soc_dbgf("FIQ", "FIQ exception 0x%x detected on CPU%d@EL1\r\n",
                 routine, cpuid);
        armv8a_gic_isr(cpuid);
}

__xwcc_section(".armv8a.exception.text.el2")
void armv8a_irq_el2(__xwcc_unused struct armv8a_reg_frame * regs,
                    __xwcc_unused xwu64_t routine)
{
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        soc_dbgf("IRQ", "IRQ exception 0x%x detected on CPU%d@EL2\r\n",
                 routine, cpuid);
        armv8a_gic_isr(cpuid);
}

__xwcc_section(".armv8a.exception.text.el2")
void armv8a_fiq_el2(__xwcc_unused struct armv8a_reg_frame * regs,
                    __xwcc_unused xwu64_t routine)
{
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        soc_dbgf("FIQ", "FIQ exception 0x%x detected on CPU%d@EL2\r\n",
                 routine, cpuid);
        armv8a_gic_isr(cpuid);
}

__xwcc_section(".armv8a.exception.text.el3")
void armv8a_irq_el3(__xwcc_unused struct armv8a_reg_frame * regs,
                    __xwcc_unused xwu64_t routine)
{
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        soc_dbgf("IRQ", "IRQ exception 0x%x detected on CPU%d@EL3\r\n",
                 routine, cpuid);
        armv8a_gic_isr(cpuid);
}

__xwcc_section(".armv8a.exception.text.el3")
void armv8a_fiq_el3(__xwcc_unused struct armv8a_reg_frame * regs,
                    __xwcc_unused xwu64_t routine)
{
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        soc_dbgf("FIQ", "FIQ exception 0x%x detected on CPU%d@EL3\r\n",
                 routine, cpuid);
        armv8a_gic_isr(cpuid);
}

__xwbsp_init_code
void armv8a_init_vector(void)
{
        xwu64_t el;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        switch (el) {
        case 3:
                armv8a_sysreg_write(vbar_el3, (xwu64_t)armv8a_exception_vector_el3);
                soc_show_sysreg(vbar_el3);
                [[fallthrough]];
        case 2:
                armv8a_sysreg_write(vbar_el2, (xwu64_t)armv8a_exception_vector_el2);
                soc_show_sysreg(vbar_el2);
                [[fallthrough]];
        case 1:
        default:
                armv8a_sysreg_write(vbar_el1, (xwu64_t)armv8a_exception_vector_el1);
                soc_show_sysreg(vbar_el1);
        }
}
