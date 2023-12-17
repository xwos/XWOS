/**
 * @file
 * @brief 架构描述层：ARMv8A MMU
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

#include <xwcd/soc/arm64/v8a/arch_mmu.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>

__xwbsp_code
void armv8a_invalidate_tlb_all(void)
{
        xwu64_t el;

        armv8a_sysreg_read(&el, CurrentEL);
        el >>= 2UL;
        switch (el) {
        case 3:
                __asm__ volatile(
                "       tlbi    alle3\n"
                "       dsb     sy\n"
                "       isb\n"
                : : :);
                break;
        case 2:
                __asm__ volatile(
                "       tlbi    alle2\n"
                "       dsb     sy\n"
                "       isb\n"
                : : :);
                break;
        case 1:
                __asm__ volatile(
                "       tlbi    vmalle1\n"
                "       dsb     sy\n"
                "       isb\n"
                : : :);
        default:
                break;
        }
}
