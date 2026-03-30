/**
 * @file
 * @brief 架构描述层：ARMv8A CACHE
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

#include <xwcd/soc/arm64/v8a/arch_regs.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>
#include <xwcd/soc/arm64/v8a/arch_cache.h>

__xwbsp_code
void armv8a_icache_enable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2UL;
        armv8a_invalidate_icache_all();
        switch (el) {
        case 2UL:
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr |= ARMV8A_SCTLR_I;
                armv8a_sysreg_write(sctlr_el2, sctlr);
                break;
        case 1UL:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr |= ARMV8A_SCTLR_I;
                armv8a_sysreg_write(sctlr_el1, sctlr);
                break;
        default:
                break;
        }
}

__xwbsp_code
void armv8a_icache_disable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2UL;
        switch (el) {
        case 2UL:
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr &= ~(ARMV8A_SCTLR_I);
                armv8a_sysreg_write(sctlr_el2, sctlr);
                break;
        case 1UL:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr &= ~(ARMV8A_SCTLR_I);
                armv8a_sysreg_write(sctlr_el1, sctlr);
                break;
        default:
                break;
        }
}

__xwbsp_code
void armv8a_invalidate_icache_all(void)
{
        __asm__ volatile(
        "       ic      ialluis\n"
        : : :);
        armv8a_isb();
}

void armv8a_dcache_enable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2UL;
        switch (el) {
        case 2UL:
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                sctlr |= ARMV8A_SCTLR_C;
                armv8a_sysreg_write(sctlr_el2, sctlr);
                break;
        case 1UL:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                sctlr |= ARMV8A_SCTLR_C;
                armv8a_sysreg_write(sctlr_el1, sctlr);
                break;
        default:
                break;
        }
}

void armv8a_dcache_disable(void)
{
        xwu64_t el;
        xwu64_t sctlr;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2UL;
        switch (el) {
        case 2UL:
                armv8a_sysreg_read(&sctlr, sctlr_el2);
                if (0 != (sctlr & ARMV8A_SCTLR_C)) {
                        sctlr &= ~(ARMV8A_SCTLR_C | ARMV8A_SCTLR_M);
                        armv8a_sysreg_write(sctlr_el2, sctlr);
                }
                break;
        case 1UL:
                armv8a_sysreg_read(&sctlr, sctlr_el1);
                if (0 != (sctlr & ARMV8A_SCTLR_C)) {
                        sctlr &= ~(ARMV8A_SCTLR_C | ARMV8A_SCTLR_M);
                        armv8a_sysreg_write(sctlr_el1, sctlr);
                }
                break;
        default:
                break;
        }
}

__xwbsp_code
void armv8a_flush_dcache_all(void)
{
        xwu64_t clidr;
        xwu64_t loc;
        xwu64_t cache;
        xwu64_t ctype;
        xwu64_t ccsidr;
        xwu64_t line_size_shift;
        xwu64_t way;
        xwu64_t way_shift;
        xwu64_t set;
        xwu64_t dcop;
        xwu64_t way_idx;
        xwu64_t set_idx;

        armv8a_sysreg_read(&clidr, clidr_el1);
        loc = (clidr >> 24UL) & 7UL;
        for (cache = 0; cache < loc; cache++) {
                ctype = (clidr >> cache) & 7UL;
                if (2 == ctype) { /* 2 means data cache */
                        armv8a_sysreg_write(csselr_el1, ((xwu64_t)cache << 1UL));
                        armv8a_isb();
                        armv8a_sysreg_read(&ccsidr, ccsidr_el1);
                        line_size_shift = (ccsidr & 7UL) + 4UL;
                        way = (ccsidr >> 3UL) & 0x3FFUL;
                        set = (ccsidr >> 13UL) & 0x7FFFUL;
                        __asm__ volatile(
                        "       clz      %w[__way_shift], %w[__way]\n"
                        : [__way_shift] "=&r" (way_shift)
                        : [__way] "r" (way)
                        :);
                        for (set_idx = 0; set_idx <= set; set_idx++) {
                                for (way_idx = 0; way_idx <= way; way_idx++) {
                                        dcop = cache << 1UL;
                                        dcop |= (way_idx << way_shift);
                                        dcop |= (set_idx << line_size_shift);
                                        __asm__ volatile(
                                        "       dc      cisw, %[__dcop]\n"
                                        :
                                        : [__dcop] "r" (dcop)
                                        :);

                                }
                        }
                }
        }
        armv8a_dsb(sy);
        armv8a_isb();
}

__xwbsp_code
void armv8a_invalidate_dcache_all(void)
{
        xwu64_t clidr;
        xwu64_t loc;
        xwu64_t cache;
        xwu64_t ctype;
        xwu64_t ccsidr;
        xwu64_t line_size_shift;
        xwu64_t way;
        xwu64_t way_shift;
        xwu64_t set;
        xwu64_t dcop;
        xwu64_t way_idx;
        xwu64_t set_idx;

        armv8a_sysreg_read(&clidr, clidr_el1);
        loc = (clidr >> 24UL) & 7UL;
        for (cache = 0; cache < loc; cache++) {
                ctype = (clidr >> cache) & 7UL;
                if (2 == ctype) { /* 2 means data cache */
                        armv8a_sysreg_write(csselr_el1, ((xwu64_t)cache << 1UL));
                        armv8a_isb();
                        armv8a_sysreg_read(&ccsidr, ccsidr_el1);
                        line_size_shift = (ccsidr & 7UL) + 4UL;
                        way = (ccsidr >> 3UL) & 0x3FFUL;
                        set = (ccsidr >> 13UL) & 0x7FFFUL;
                        __asm__ volatile(
                        "       clz      %w[__way_shift], %w[__way]\n"
                        : [__way_shift] "=&r" (way_shift)
                        : [__way] "r" (way)
                        :);
                        for (set_idx = 0; set_idx <= set; set_idx++) {
                                for (way_idx = 0; way_idx <= way; way_idx++) {
                                        dcop = cache << 1UL;
                                        dcop |= (way_idx << way_shift);
                                        dcop |= (set_idx << line_size_shift);
                                        __asm__ volatile(
                                        "       dc      isw, %[__dcop]\n"
                                        :
                                        : [__dcop] "r" (dcop)
                                        :);

                                }
                        }
                }
        }
        armv8a_dsb(sy);
        armv8a_isb();
}

__xwbsp_code
void armv8a_flush_dcache(xwptr_t origin, xwsz_t size)
{
        xwu64_t ctr;
        xwu64_t line_size;
        xwu64_t line_mask;
        xwu64_t addr;
        xwu64_t end = origin + size;

        armv8a_sysreg_read(&ctr, ctr_el0);
        line_size = 4UL << ((ctr >> 16UL) & 0xFUL);
        line_mask = line_size - 1UL;
        addr = origin & (~line_mask); /* aligned */
        while (addr < end) {
                __asm__ volatile(
                        "       dc      civac, %[__addr]\n"
                :
                : [__addr] "r" (addr)
                :
                );
                addr += line_size;
        }
        armv8a_dsb(sy);
}

__xwbsp_code
void armv8a_clean_dcache_poc(xwptr_t origin, xwsz_t size)
{
        xwu64_t ctr;
        xwu64_t line_size;
        xwu64_t line_mask;
        xwu64_t addr;
        xwu64_t end = origin + size;

        armv8a_sysreg_read(&ctr, ctr_el0);
        line_size = 4UL << ((ctr >> 16UL) & 0xFUL);
        line_mask = line_size - 1UL;
        addr = origin & (~line_mask); /* aligned */
        while (addr < end) {
                __asm__ volatile(
                        "       dc      cvac, %[__addr]\n"
                :
                : [__addr] "r" (addr)
                :
                );
                addr += line_size;
        }
        armv8a_dsb(sy);
}

__xwbsp_code
void armv8a_clean_dcache_pou(xwptr_t origin, xwsz_t size)
{
        xwu64_t ctr;
        xwu64_t line_size;
        xwu64_t line_mask;
        xwu64_t addr;
        xwu64_t end = origin + size;

        armv8a_sysreg_read(&ctr, ctr_el0);
        line_size = 4UL << ((ctr >> 16UL) & 0xFUL);
        line_mask = line_size - 1UL;
        addr = origin & (~line_mask); /* aligned */
        while (addr < end) {
                __asm__ volatile(
                        "       dc      cvau, %[__addr]\n"
                :
                : [__addr] "r" (addr)
                :
                );
                addr += line_size;
        }
        armv8a_dsb(ish);
}

#if defined(ARCHCFG_FEAT_DPB) && (1 == ARCHCFG_FEAT_DPB)
__xwbsp_code
void armv8a_clean_dcache_pop(xwptr_t origin, xwsz_t size)
{
        xwu64_t ctr;
        xwu64_t line_size;
        xwu64_t line_mask;
        xwu64_t addr;
        xwu64_t end = origin + size;

        armv8a_sysreg_read(&ctr, ctr_el0);
        line_size = 4UL << ((ctr >> 16UL) & 0xFUL);
        line_mask = line_size - 1UL;
        addr = origin & (~line_mask); /* aligned */
        while (addr < end) {
                __asm__ volatile(
                        "       dc      cvap, %[__addr]\n"
                :
                : [__addr] "r" (addr)
                :
                );
                addr += line_size;
        }
        armv8a_dsb(ish);
}
#endif

__xwbsp_code
void armv8a_invalidate_dcache(xwptr_t origin, xwsz_t size)
{
        xwu64_t ctr;
        xwu64_t line_size;
        xwu64_t line_mask;
        xwu64_t addr;
        xwu64_t end = origin + size;

        armv8a_sysreg_read(&ctr, ctr_el0);
        line_size = 4UL << ((ctr >> 16UL) & 0xFUL);
        line_mask = line_size - 1UL;
        addr = origin & (~line_mask); /* aligned */
        while (addr < end) {
                __asm__ volatile(
                        "       dc      ivac, %[__addr]\n"
                :
                : [__addr] "r" (addr)
                :
                );
                addr += line_size;
        }
        armv8a_dsb(sy);
}
