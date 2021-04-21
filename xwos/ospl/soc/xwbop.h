/**
 * @file
 * @brief 玄武OS移植层：SOC位操作
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_xwbop_h__
#define __xwos_ospl_soc_xwbop_h__

#include <xwos/standard.h>

xwssq_t xwbop_ffs8(xwu8_t x);
xwssq_t xwbop_fls8(xwu8_t x);
xwu8_t xwbop_rbit8(xwu8_t x);
xwsz_t xwbop_weight8(xwu8_t x);

xwssq_t xwbop_ffs16(xwu16_t x);
xwssq_t xwbop_fls16(xwu16_t x);
xwu16_t xwbop_rbit16(xwu16_t x);
xwu16_t xwbop_re16(xwu16_t a);
xws32_t xwbop_re16s32(xwu16_t x);
xwsz_t xwbop_weight16(xwu16_t x);

xwssq_t xwbop_ffs32(xwu32_t x);
xwssq_t xwbop_fls32(xwu32_t x);
xwu32_t xwbop_rbit32(xwu32_t x);
xwu32_t xwbop_re32(xwu32_t x);
xws64_t xwbop_re32s64(xwu32_t x);
xwsz_t xwbop_weight32(xwu32_t x);

xwssq_t xwbop_ffs64(xwu64_t x);
xwssq_t xwbop_fls64(xwu64_t x);
xwu64_t xwbop_re64(xwu64_t x);
xwu64_t xwbop_rbit64(xwu64_t x);
xwsz_t xwbop_weight64(xwu64_t x);

void xwbmpop_s1i(xwbmp_t * bmp, xwsq_t n);
void xwbmpop_c0i(xwbmp_t * bmp, xwsq_t n);
void xwbmpop_x1i(xwbmp_t * bmp, xwsq_t n);
bool xwbmpop_t1i(xwbmp_t * bmp, xwsq_t n);
xwssq_t xwbmpop_ffs(xwbmp_t * bmp, xwsz_t num);
xwssq_t xwbmpop_ffz(xwbmp_t * bmp, xwsz_t num);
xwssq_t xwbmpop_fls(xwbmp_t * bmp, xwsz_t num);
xwssq_t xwbmpop_flz(xwbmp_t * bmp, xwsz_t num);

#include <xwosimpl_soc_xwbop.h>

#endif /* xwos/ospl/soc/xwbop.h */
