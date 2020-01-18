/**
 * @file
 * @brief 位操作汇编库：arch_xwbop_ffs64
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <asmlib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_code __naked
xwssq_t arch_xwbop_ffs64(__maybe_unused xwu64_t x)
{
        __asm__ volatile("      rbit    r0, r0");
        __asm__ volatile("      rbit    r1, r1");
        __asm__ volatile("      cmp     r0, #0");
        __asm__ volatile("      itt     ne");
        __asm__ volatile("      clzne   r0, r0");
        __asm__ volatile("      bxne    lr");
        __asm__ volatile("      clz     r0, r1");
        __asm__ volatile("      cmp     r0, #32");
        __asm__ volatile("      ite     eq");
        __asm__ volatile("      mvneq   r0, #0");
        __asm__ volatile("      addne   r0, #32");
        __asm__ volatile("      bx      lr");
}
