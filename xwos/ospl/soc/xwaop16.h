/**
 * @file
 * @brief XWOS移植层：SOC 16位原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_soc_xwaop16_h__
#define __xwos_ospl_soc_xwaop16_h__

#include <xwos/standard.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
xwu16_t xwaop__xwu16_t__load(atomic_xwu16_t * a,
                             const enum xwaop_memory_order_em mo);

xwu16_t xwaop__xwu16_t__store(atomic_xwu16_t * a,
                              const enum xwaop_memory_order_em mo,
                              xwu16_t v);

void xwaop__xwu16_t__read(atomic_xwu16_t * a,
                          xwu16_t * ov);

void xwaop__xwu16_t__write(atomic_xwu16_t * a,
                           xwu16_t v,
                           xwu16_t * ov);

xwer_t xwaop__xwu16_t__teq_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16_t__tne_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgt_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16_t__tge_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16_t__tlt_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16_t__tle_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtlt_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgelt_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtle_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgele_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov);

void xwaop__xwu16_t__add(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__teq_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tne_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgt_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tge_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tlt_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tle_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtlt_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgelt_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtle_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgele_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16_t__sub(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__teq_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t, xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tne_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgt_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tge_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tlt_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tle_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtlt_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgelt_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtle_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgele_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16_t__rsb(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__teq_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tne_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgt_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tge_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tlt_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tle_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtlt_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgelt_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgtle_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tgele_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16_t__or(atomic_xwu16_t * a,
                        xwu16_t v,
                        xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16_t__and(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16_t__xor(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16_t__c0m(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__t1ma_then_c0m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__t1mo_then_c0m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__t0ma_then_s1m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__t0mo_then_s1m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16_t__tst_then_op(atomic_xwu16_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu16_t * nv, xwu16_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws16_t xwaop__xws16_t__load(atomic_xws16_t * a,
                             const enum xwaop_memory_order_em mo);

xws16_t xwaop__xws16_t__store(atomic_xws16_t * a,
                              const enum xwaop_memory_order_em mo,
                              xws16_t v);

void xwaop__xws16_t__read(atomic_xws16_t * a,
                          xws16_t * ov);

void xwaop__xws16_t__write(atomic_xws16_t * a,
                           xws16_t v,
                           xws16_t * ov);

xwer_t xwaop__xws16_t__teq_then_write(atomic_xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16_t__tne_then_write(atomic_xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16_t__tgt_then_write(atomic_xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16_t__tge_then_write(atomic_xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16_t__tlt_then_write(atomic_xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16_t__tle_then_write(atomic_xws16_t * a,
                                      xws16_t t,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16_t__tgtlt_then_write(atomic_xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

xwer_t xwaop__xws16_t__tgelt_then_write(atomic_xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

xwer_t xwaop__xws16_t__tgtle_then_write(atomic_xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

xwer_t xwaop__xws16_t__tgele_then_write(atomic_xws16_t * a,
                                        xws16_t l, xws16_t r,
                                        xws16_t v,
                                        xws16_t * ov);

void xwaop__xws16_t__add(atomic_xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__teq_then_add(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tne_then_add(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgt_then_add(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tge_then_add(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tlt_then_add(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tle_then_add(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgtlt_then_add(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgelt_then_add(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgtle_then_add(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgele_then_add(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

void xwaop__xws16_t__sub(atomic_xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__teq_then_sub(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tne_then_sub(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgt_then_sub(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tge_then_sub(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tlt_then_sub(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tle_then_sub(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgtlt_then_sub(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgelt_then_sub(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgtle_then_sub(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgele_then_sub(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

void xwaop__xws16_t__rsb(atomic_xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__teq_then_rsb(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tne_then_rsb(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgt_then_rsb(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tge_then_rsb(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tlt_then_rsb(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tle_then_rsb(atomic_xws16_t * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgtlt_then_rsb(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgelt_then_rsb(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgtle_then_rsb(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16_t__tgele_then_rsb(atomic_xws16_t * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * nv, xws16_t * ov);

void xwaop__xws16_t__or(atomic_xws16_t * a,
                        xws16_t v,
                        xws16_t * nv, xws16_t * ov);

void xwaop__xws16_t__and(atomic_xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

void xwaop__xws16_t__xor(atomic_xws16_t * a,
                         xws16_t v,
                         xws16_t * nv, xws16_t * ov);

static __xwlib_inline
void xwaop__xws16_t__c0m(atomic_xws16_t * a,
                         xws16_t m,
                         xws16_t * nv, xws16_t * ov)
{
        xwaop__xwu16_t__c0m((atomic_xwu16_t *)a,
                            (xwu16_t)m,
                            (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t1ma_then_c0m(atomic_xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t1ma_then_c0m((atomic_xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t1mo_then_c0m(atomic_xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t1mo_then_c0m((atomic_xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t0ma_then_s1m(atomic_xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t0ma_then_s1m((atomic_xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16_t__t0mo_then_s1m(atomic_xws16_t * a,
                                     xws16_t m,
                                     xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16_t__t0mo_then_s1m((atomic_xwu16_t *)a,
                                             (xwu16_t)m,
                                             (xwu16_t *)nv, (xwu16_t *)ov);
}

xwer_t xwaop__xws16_t__tst_then_op(atomic_xws16_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xws16_t * nv, xws16_t * ov);

#include <xwosimpl_soc_xwaop16.h>

#endif /* xwos/ospl/soc/xwaop16.h */
