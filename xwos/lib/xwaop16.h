/**
 * @file
 * @brief XWOS通用库：16位原子操作函数库
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_lib_xwaop16_h__
#define __xwos_lib_xwaop16_h__

#include <xwos/standard.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
__xwlib_code
xwu16_t xwaop__xwu16_t__load(__xwcc_atomic xwu16_t * a,
                             const enum xwmb_memory_order_em mo);

__xwlib_code
xwu16_t xwaop__xwu16_t__store(__xwcc_atomic xwu16_t * a,
                              const enum xwmb_memory_order_em mo,
                              xwu16_t v);

__xwlib_code
void xwaop__xwu16_t__read(__xwcc_atomic xwu16_t * a,
                          xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__write(__xwcc_atomic xwu16_t * a,
                           xwu16_t v,
                           xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__teq_then_write(__xwcc_atomic xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tne_then_write(__xwcc_atomic xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgt_then_write(__xwcc_atomic xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tge_then_write(__xwcc_atomic xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tlt_then_write(__xwcc_atomic xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tle_then_write(__xwcc_atomic xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtlt_then_write(__xwcc_atomic xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgelt_then_write(__xwcc_atomic xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtle_then_write(__xwcc_atomic xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgele_then_write(__xwcc_atomic xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__add(__xwcc_atomic xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__teq_then_add(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tne_then_add(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgt_then_add(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tge_then_add(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tlt_then_add(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tle_then_add(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtlt_then_add(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgelt_then_add(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtle_then_add(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgele_then_add(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__sub(__xwcc_atomic xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__teq_then_sub(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t, xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tne_then_sub(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgt_then_sub(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tge_then_sub(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tlt_then_sub(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tle_then_sub(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtlt_then_sub(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgelt_then_sub(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtle_then_sub(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgele_then_sub(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__rsb(__xwcc_atomic xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__teq_then_rsb(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tne_then_rsb(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgt_then_rsb(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tge_then_rsb(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tlt_then_rsb(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tle_then_rsb(__xwcc_atomic xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtlt_then_rsb(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgelt_then_rsb(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgtle_then_rsb(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tgele_then_rsb(__xwcc_atomic xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__or(__xwcc_atomic xwu16_t * a,
                        xwu16_t v,
                        xwu16_t * nv, xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__and(__xwcc_atomic xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__xor(__xwcc_atomic xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

__xwlib_code
void xwaop__xwu16_t__c0m(__xwcc_atomic xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__t1ma_then_c0m(__xwcc_atomic xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__t1mo_then_c0m(__xwcc_atomic xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__t0ma_then_s1m(__xwcc_atomic xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__t0mo_then_s1m(__xwcc_atomic xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

__xwlib_code
xwer_t xwaop__xwu16_t__tst_then_op(__xwcc_atomic xwu16_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu16_t * nv, xwu16_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
__xwlib_code
xws16_t xwaop__xws16_t__load(__xwcc_atomic xws16_t * a,
                             const enum xwmb_memory_order_em mo);

__xwlib_code
xws16_t xwaop__xws16_t__store(__xwcc_atomic xws16_t * a,
                              const enum xwmb_memory_order_em mo,
                              xws16_t v);

__xwlib_code
void xwaop__xws16_t__read(__xwcc_atomic xws16_t * a,
                          xws16_t * ov);

__xwlib_code
void xwaop__xws16_t__write(__xwcc_atomic xws16_t * a,
                           xws16_t v,
                           xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__teq_then_write(__xwcc_atomic xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tne_then_write(__xwcc_atomic xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgt_then_write(__xwcc_atomic xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tge_then_write(__xwcc_atomic xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tlt_then_write(__xwcc_atomic xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tle_then_write(__xwcc_atomic xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtlt_then_write(__xwcc_atomic xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgelt_then_write(__xwcc_atomic xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtle_then_write(__xwcc_atomic xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgele_then_write(__xwcc_atomic xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

__xwlib_code
void xwaop__xws16_t__add(__xwcc_atomic xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__teq_then_add(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tne_then_add(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgt_then_add(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tge_then_add(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tlt_then_add(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tle_then_add(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtlt_then_add(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgelt_then_add(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtle_then_add(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgele_then_add(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
void xwaop__xws16_t__sub(__xwcc_atomic xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__teq_then_sub(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tne_then_sub(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgt_then_sub(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tge_then_sub(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tlt_then_sub(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tle_then_sub(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtlt_then_sub(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgelt_then_sub(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtle_then_sub(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgele_then_sub(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
void xwaop__xws16_t__rsb(__xwcc_atomic xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__teq_then_rsb(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tne_then_rsb(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgt_then_rsb(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tge_then_rsb(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tlt_then_rsb(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tle_then_rsb(__xwcc_atomic xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtlt_then_rsb(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgelt_then_rsb(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgtle_then_rsb(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
xwer_t xwaop__xws16_t__tgele_then_rsb(__xwcc_atomic xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

__xwlib_code
void xwaop__xws16_t__or(__xwcc_atomic xws16_t * a,
                        xws16_t v,
                        xws16_t * nv, xws16_t * ov);

__xwlib_code
void xwaop__xws16_t__and(__xwcc_atomic xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

__xwlib_code
void xwaop__xws16_t__xor(__xwcc_atomic xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

static __xwlib_inline
void xwaop__xws16_t__c0m(__xwcc_atomic xws16_t * a,
                         xws16_t m,
                         xws16_t * nv, xws16_t * ov)
{
        xwaop__xwu16_t__c0m((__xwcc_atomic xwu16_t *)a,
                            (xwu16_t)m,
                            (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t1ma_then_c0m(__xwcc_atomic xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t1ma_then_c0m((__xwcc_atomic xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t1mo_then_c0m(__xwcc_atomic xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t1mo_then_c0m((__xwcc_atomic xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t0ma_then_s1m(__xwcc_atomic xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t0ma_then_s1m((__xwcc_atomic xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t0mo_then_s1m(__xwcc_atomic xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t0mo_then_s1m((__xwcc_atomic xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

__xwlib_code
xwer_t xwaop__xws16_t__tst_then_op(__xwcc_atomic xws16_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xws16_t * nv, xws16_t * ov);

#endif /* xwos/lib/xwaop16.h */
