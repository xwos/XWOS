/**
 * @file
 * @brief XWOS通用库：8位原子操作函数库
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_xwaop8_h__
#define __xwos_lib_xwaop8_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** ******** unsigned ******** ******** ******** ********/
__xwlib_code
xwu8_t xwaop__xwu8_t__load(__xwcc_atomic xwu8_t * a,
                           const enum xwmb_memory_order_em mo);

__xwlib_code
xwu8_t xwaop__xwu8_t__store(__xwcc_atomic xwu8_t * a,
                            const enum xwmb_memory_order_em mo,
                            xwu8_t v);

__xwlib_code
void xwaop__xwu8_t__read(__xwcc_atomic xwu8_t * a,
                         xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__write(__xwcc_atomic xwu8_t * a,
                          xwu8_t v,
                          xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__teq_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tne_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgt_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tge_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tlt_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tle_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtlt_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgelt_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtle_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgele_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__add(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__teq_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tne_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgt_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tge_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tlt_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tle_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtlt_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgelt_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtle_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgele_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__sub(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__teq_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tne_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgt_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tge_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tlt_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tle_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtlt_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgelt_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtle_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgele_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__rsb(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__teq_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tne_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgt_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tge_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tlt_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tle_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtlt_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgelt_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgtle_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tgele_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__or(__xwcc_atomic xwu8_t * a,
                       xwu8_t v,
                       xwu8_t * nv, xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__and(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__xor(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

__xwlib_code
void xwaop__xwu8_t__c0m(__xwcc_atomic xwu8_t * a,
                        xwu8_t m,
                        xwu8_t * nv, xwu8_t * ov);


__xwlib_code
xwer_t xwaop__xwu8_t__t1ma_then_c0m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__t1mo_then_c0m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__t0ma_then_s1m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__t0mo_then_s1m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

__xwlib_code
xwer_t xwaop__xwu8_t__tst_then_op(__xwcc_atomic xwu8_t * a,
                                  xwaop_tst_f tst, void * tst_args,
                                  xwaop_op_f op, void * op_args,
                                  xwu8_t * nv, xwu8_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
__xwlib_code
xws8_t xwaop__xws8_t__load(__xwcc_atomic xws8_t * a,
                           const enum xwmb_memory_order_em mo);

__xwlib_code
xws8_t xwaop__xws8_t__store(__xwcc_atomic xws8_t * a,
                            const enum xwmb_memory_order_em mo,
                            xws8_t v);

__xwlib_code
void xwaop__xws8_t__read(__xwcc_atomic xws8_t * a,
                         xws8_t * ov);

__xwlib_code
void xwaop__xws8_t__write(__xwcc_atomic xws8_t * a,
                          xws8_t v,
                          xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_write(__xwcc_atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_write(__xwcc_atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_write(__xwcc_atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_write(__xwcc_atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_write(__xwcc_atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_write(__xwcc_atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_write(__xwcc_atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_write(__xwcc_atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_write(__xwcc_atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_write(__xwcc_atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

__xwlib_code
void xwaop__xws8_t__add(__xwcc_atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_add(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_add(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_add(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_add(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_add(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_add(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_add(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_add(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_add(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_add(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
void xwaop__xws8_t__sub(__xwcc_atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_sub(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_sub(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_sub(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_sub(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_sub(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_sub(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_sub(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_sub(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_sub(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_sub(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
void xwaop__xws8_t__rsb(__xwcc_atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_rsb(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_rsb(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_rsb(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_rsb(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_rsb(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_rsb(__xwcc_atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_rsb(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_rsb(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_rsb(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_rsb(__xwcc_atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

__xwlib_code
void xwaop__xws8_t__or(__xwcc_atomic xws8_t * a,
                       xws8_t v,
                       xws8_t * nv, xws8_t * ov);

__xwlib_code
void xwaop__xws8_t__and(__xwcc_atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

__xwlib_code
void xwaop__xws8_t__xor(__xwcc_atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

static __xwlib_inline
void xwaop__xws8_t__c0m(__xwcc_atomic xws8_t * a,
                        xws8_t m,
                        xws8_t * nv, xws8_t * ov)
{
        xwaop__xwu8_t__c0m((__xwcc_atomic xwu8_t *)a,
                           (xwu8_t)m,
                           (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t1ma_then_c0m(__xwcc_atomic xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t1ma_then_c0m((__xwcc_atomic xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t1mo_then_c0m(__xwcc_atomic xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t1mo_then_c0m((__xwcc_atomic xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t0ma_then_s1m(__xwcc_atomic xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t0ma_then_s1m((__xwcc_atomic xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t0mo_then_s1m(__xwcc_atomic xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t0mo_then_s1m((__xwcc_atomic xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

__xwlib_code
xwer_t xwaop__xws8_t__tst_then_op(__xwcc_atomic xws8_t * a,
                                  xwaop_tst_f tst, void * tst_args,
                                  xwaop_op_f op, void * op_args,
                                  xws8_t * nv, xws8_t * ov);

#endif /* xwos/lib/xwaop8.h */
