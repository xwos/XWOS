/**
 * @file
 * @brief 玄武OS移植层：SOC位图原子操作
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_xwbmpaop_h__
#define __xwos_ospl_soc_xwbmpaop_h__

#include <xwos/standard.h>

bool xwbmpaop_t1i(xwbmp_a * bmp, xwsq_t idx);
void xwbmpaop_s1i(xwbmp_a * bmp, xwsq_t idx);
void xwbmpaop_c0i(xwbmp_a * bmp, xwsq_t idx);
void xwbmpaop_x1i(xwbmp_a * bmp, xwsq_t idx);

xwer_t xwbmpaop_t0i_then_s1i(xwbmp_a * bmp, xwsq_t idx);
xwer_t xwbmpaop_t1i_then_c0i(xwbmp_a * bmp, xwsq_t idx);

xwssq_t xwbmpaop_fls_then_c0i(xwbmp_a * bmp, xwsz_t num);
xwssq_t xwbmpaop_flz_then_s1i(xwbmp_a * bmp, xwsz_t num);
xwssq_t xwbmpaop_ffs_then_c0i(xwbmp_a * bmp, xwsz_t num);
xwssq_t xwbmpaop_ffz_then_s1i(xwbmp_a * bmp, xwsz_t num);

#include <xwosimpl_soc_xwbmpaop.h>

#endif /* xwos/ospl/soc/xwbmpaop.h */
