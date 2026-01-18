/**
 * @file
 * @brief XWOS移植实现层：SOC系统调用
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

__xwbsp_code
xws64_t xwsc(xwsc_f func, xwptr_t argnum, ...)
{
        union {
                xws64_t v64;
                struct {
                        xwu32_t l;
                        xws32_t h;
                } v32;
        } rc;
        va_list args;

        va_start(args, argnum);
        __asm__ volatile(
        "       mv      a0, %[__func]\n" /* sc function */
        "       mv      a1, %[__argnum]\n" /* number of args */
        "       mv      a2, %[__args]\n" /* args address */
        "       ecall\n"
        "       mv      %[__rch], a1\n"
        "       mv      %[__rcl], a0\n"
        : [__rch] "=r" (rc.v32.h),
          [__rcl] "=r" (rc.v32.l)
        : [__func] "r" (func),
          [__argnum] "r" (argnum),
          [__args] "r" (args)
        : "memory", "a0", "a1", "a2"
        );
        va_end(args);
        return rc.v64;
}
