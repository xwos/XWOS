/**
 * @file
 * @brief 架构描述层：System Call & Trap
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

#ifndef __arch_sc_trap_h__
#define __arch_sc_trap_h__

#ifndef __ASM__
#include <xwos/standard.h>
#endif /* #ifndef __ASM__ */

#define ARCH_SC_PRIVILEGE_START                 1
#define ARCH_SC_PRIVILEGE_END                   2

#ifndef __ASM__
xws64_t arch_systemcall(xwid_t sc_no, xwreg_t arg1, xwreg_t arg2);
#endif /* #ifndef __ASM__ */

#endif /* arch_sc_trap.h */
