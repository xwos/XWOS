/**
 * @file
 * @brief 架构描述层：初始化
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

#include <xwcd/soc/arm64/v8a/arch_init.h>
#include <xwcd/soc/arm64/v8a/arch_regs.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_exception.h>

/* #define SOC_DBGF */
#include <soc_debug.h>

/**
 * @brief ARMv8A的初始化
 */
__xwbsp_init_code
void armv8a_init(void)
{
        xwu64_t hcr;
        xwu64_t el;
        xwu64_t scr;
        xwu64_t sctlr;
        xwu64_t cptr;
        xwu64_t cpacr;
        xwu64_t spsr;
        xwu64_t cntvoff;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        switch (el) {
        case 3:
                armv8a_sysreg_read(&scr, scr_el3);
                scr &= ~(ARMV8A_SCR_TWE /* Disable TWE */
                         | ARMV8A_SCR_TWI /* Disable TWI */
                         | ARMV8A_SCR_SMD /* Enable SMC */
                         | ARMV8A_SCR_EA /* Disable SError */
                         | ARMV8A_SCR_FIQ /* Disable FIQ */
                         | ARMV8A_SCR_IRQ /* Disable IRQ */);
                scr |= (ARMV8A_SCR_NS /* Non-secure world */
                        | ARMV8A_SCR_RES1_4 /* Reserved bit, must set to 1 */
                        | ARMV8A_SCR_RES1_5 /* Reserved bit, must set to 1 */
                        | ARMV8A_SCR_HCE /* enable HVC instruction */
                        | ARMV8A_SCR_SIF /* Secure instruction fetches from Non-Secure memory are not permitted. */
                        | ARMV8A_SCR_RW /* Keep AArch64 when droping EL */);
                armv8a_sysreg_write(scr_el3, scr);
                soc_show_sysreg(scr_el3);

                armv8a_sysreg_read(&sctlr, sctlr_el3);
                sctlr &= ~(ARMV8A_SCTLR_EE); /* EE: little-endian */
                sctlr |= (ARMV8A_SCTLR_SA /* Stack Alignment Check */
                          /* | ARMV8A_SCTLR_A */ /* Alignment Check. May cause exception */);
                armv8a_sysreg_write(sctlr_el3, sctlr);
                soc_show_sysreg(sctlr_el3);

                cptr = 0; /* Enable FP/SIMD in Low EL */
                armv8a_sysreg_write(cptr_el3, cptr);
                soc_show_sysreg(cptr_el3);

                spsr = (ARMV8A_SPSR_D
                        | ARMV8A_SPSR_A
                        | ARMV8A_SPSR_I
                        | ARMV8A_SPSR_F
                        | ARMV8A_SPSR_EL2H);
                armv8a_sysreg_write(spsr_el3, spsr);
                soc_show_sysreg(spsr_el3);

                [[fallthrough]];
        case 2:
                armv8a_sysreg_read(&hcr, hcr_el2);
                hcr |= (ARMV8A_HCR_RW /* Keep AArch64 when droping EL */
                        | ARMV8A_HCR_FMO /* Enable IRQ in EL2 */
                        | ARMV8A_HCR_IMO /* Enable IRQ in EL2 */);
                armv8a_sysreg_write(hcr_el2, hcr);
                soc_show_sysreg(hcr_el2);

                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr &= ~(ARMV8A_SCTLR_EE); /* EE: little-endian */
                sctlr |= (ARMV8A_SCTLR_SA /* Stack Alignment Check */
                          /* | ARMV8A_SCTLR_A */ /* Alignment Check. May cause exception */);
                armv8a_sysreg_write(sctlr_el2, sctlr);
                soc_show_sysreg(sctlr_el2);

                cptr = 0; /* Enable FP/SIMD in Low EL */
                armv8a_sysreg_write(cptr_el2, cptr);
                soc_show_sysreg(cptr_el2);

                spsr = (ARMV8A_SPSR_D
                        | ARMV8A_SPSR_A
                        | ARMV8A_SPSR_I
                        | ARMV8A_SPSR_F
                        | ARMV8A_SPSR_EL1H);
                armv8a_sysreg_write(spsr_el2, spsr);
                soc_show_sysreg(spsr_el2);

                cntvoff = 0;
                armv8a_sysreg_write(cntvoff_el2, cntvoff);

                [[fallthrough]];
        case 1:
        default:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr &= ~ARMV8A_SCTLR_EE; /* EE: little-endian */
                sctlr |= (ARMV8A_SCTLR_SA /* Stack Alignment Check */
                          /* | ARMV8A_SCTLR_A */ /* Alignment Check. May cause exception */);
                armv8a_sysreg_write(sctlr_el1, sctlr);
                soc_show_sysreg(sctlr_el1);

                cpacr = (ARMV8A_CPACR_EL1_FPEN /* Enable FPU/SIMD */
#if defined(CPUCFG_SVE) && (1 == CPUCFG_SVE)
                          | ARMV8A_CPACR_EL1_ZEN /* Enable SVE */
#endif
#if defined(CPUCFG_SME) && (1 == CPUCFG_SME)
                          | ARMV8A_CPACR_EL1_SMEN /* Enable SME */
#endif
                         );
                armv8a_sysreg_write(cpacr_el1, cpacr);
                soc_show_sysreg(cpacr_el1);
        }
}
