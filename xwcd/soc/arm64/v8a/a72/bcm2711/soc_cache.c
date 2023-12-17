/**
 * @file
 * @brief SOC描述层：Cache
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

#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_cache.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_regs.h>
#include <xwcd/soc/arm64/v8a/arch_mmu.h>
#include <xwcd/soc/arm64/v8a/arch_cache.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_mmu.h>

void soc_dcache_enable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        switch (el) {
        case 3:
                armv8a_flush_dcache_all();
                soc_mmu_init();
                armv8a_sysreg_read(&sctlr, sctlr_el3);
                sctlr |= ARMV8A_SCTLR_C;
                armv8a_sysreg_write(sctlr_el3, sctlr);
                break;
        case 2:
                armv8a_flush_dcache_all();
                soc_mmu_init();
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr |= ARMV8A_SCTLR_C;
                armv8a_sysreg_write(sctlr_el2, sctlr);
                break;
        case 1:
        default:
                armv8a_flush_dcache_all();
                soc_mmu_init();
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr |= ARMV8A_SCTLR_C;
                armv8a_sysreg_write(sctlr_el1, sctlr);
                break;
        }
}

void soc_dcache_disable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        switch (el) {
        case 3:
                armv8a_sysreg_read(&sctlr, sctlr_el3);
                if (0 != (sctlr & ARMV8A_SCTLR_C)) {
                        sctlr &= ~(ARMV8A_SCTLR_C | ARMV8A_SCTLR_M);
                        armv8a_sysreg_write(sctlr_el3, sctlr);
                        armv8a_flush_dcache_all();
                        armv8a_invalidate_tlb_all();
                }
                break;
        case 2:
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                if (0 != (sctlr & ARMV8A_SCTLR_C)) {
                        sctlr &= ~(ARMV8A_SCTLR_C | ARMV8A_SCTLR_M);
                        armv8a_sysreg_write(sctlr_el3, sctlr);
                        armv8a_flush_dcache_all();
                        armv8a_invalidate_tlb_all();
                }
                break;
        case 1:
        default:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                if (0 != (sctlr & ARMV8A_SCTLR_C)) {
                        sctlr &= ~(ARMV8A_SCTLR_C | ARMV8A_SCTLR_M);
                        armv8a_sysreg_write(sctlr_el3, sctlr);
                        armv8a_flush_dcache_all();
                        armv8a_invalidate_tlb_all();
                }
                break;
        }
}

void soc_icache_enable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        armv8a_invalidate_icache_all();
        switch (el) {
        case 3:
                armv8a_sysreg_read(&sctlr, sctlr_el3);
                sctlr |= ARMV8A_SCTLR_I;
                armv8a_sysreg_write(sctlr_el3, sctlr);
                break;
        case 2:
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr |= ARMV8A_SCTLR_I;
                armv8a_sysreg_write(sctlr_el2, sctlr);
                break;
        case 1:
        default:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr |= ARMV8A_SCTLR_I;
                armv8a_sysreg_write(sctlr_el1, sctlr);
                break;
        }
}

void soc_icache_disable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2;
        switch (el) {
        case 3:
                armv8a_sysreg_read(&sctlr, sctlr_el3);
                sctlr &= ~(ARMV8A_SCTLR_I);
                armv8a_sysreg_write(sctlr_el3, sctlr);
                break;
        case 2:
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr &= ~(ARMV8A_SCTLR_I);
                armv8a_sysreg_write(sctlr_el2, sctlr);
                break;
        case 1:
        default:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr &= ~(ARMV8A_SCTLR_I);
                armv8a_sysreg_write(sctlr_el1, sctlr);
                break;
        }
}
