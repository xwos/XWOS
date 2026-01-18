/**
 * @file
 * @brief 架构描述层：CPU ID
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

#ifndef __xwcd_soc_powerpc_e200x_arch_cpuid_h__
#define __xwcd_soc_powerpc_e200x_arch_cpuid_h__

#include <xwos/standard.h>

static __xwbsp_inline
xwid_t arch_cpu_get_id(void)
{
        return 0;
}

#endif /* xwcd/soc/powerpc/e200x/arch_cpuid.h */
