/**
 * @file
 * @brief SOC描述层：mulit-core
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
#include <xwos/lib/xwbop.h>
#include <xwcd/soc/arm64/v8a/arch_cache.h>
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_init.h>

typedef struct {
        xwu64_t cpu0;
        xwu64_t cpu1;
        xwu64_t cpu2;
        xwu64_t cpu3;
} soc_spin_table_s;

#define soc_spin_table (*(soc_spin_table_s *)0xD8)

void soc_boot_mp(xwsq_t cpumsk)
{
        if (cpumsk & XWBOP_BIT(1)) {
                soc_spin_table.cpu1 = (xwptr_t)soc_percpu_boot;
        }
        if (cpumsk & XWBOP_BIT(2)) {
                soc_spin_table.cpu2 = (xwptr_t)soc_percpu_boot;
        }
        if (cpumsk & XWBOP_BIT(3)) {
                soc_spin_table.cpu3 = (xwptr_t)soc_percpu_boot;
        }
        armv8a_dsb(st);
        __asm__ volatile("sev");
}
