/**
 * @file
 * @brief 汇编库：位操作
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

#ifndef __asmlib_xwbop_h__
#define __asmlib_xwbop_h__

#include <xwos/standard.h>

xwssq_t arch_xwbop_ffs8(xwu8_t x);
xwssq_t arch_xwbop_fls8(xwu8_t x);
xwu16_t arch_xwbop_re16(xwu16_t x);
xwssq_t arch_xwbop_ffs16(xwu16_t x);
xwssq_t arch_xwbop_fls16(xwu16_t x);
xwu32_t arch_xwbop_re32(xwu32_t x);
xwssq_t arch_xwbop_ffs32(xwu32_t x);
xwssq_t arch_xwbop_fls32(xwu32_t x);
xwu64_t arch_xwbop_re64(xwu64_t x);
xwssq_t arch_xwbop_ffs64(xwu64_t x);
xwssq_t arch_xwbop_fls64(xwu64_t x);
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t len);
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t len);
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t len);
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t len);

#endif /* asmlib/xwbop.h */
