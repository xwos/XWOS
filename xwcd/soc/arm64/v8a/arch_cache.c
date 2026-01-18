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

#include <xwcd/soc/arm64/v8a/arch_cache.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>

__xwbsp_code
void armv8a_invalidate_icache_all(void)
{
        __asm__ volatile(
        "       ic      ialluis\n"
        "       isb\n"
        : : :);
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
}

__xwbsp_code
void armv8a_clean_dcache(xwptr_t origin, xwsz_t size)
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
}

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
}
