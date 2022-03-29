/**
 * @file
 * @brief 玄武OS移植层：SOC 64位原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_xwaop64_h__
#define __xwos_ospl_soc_xwaop64_h__

#include <xwos/standard.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
xwu64_t xwaop__xwu64_t__load(atomic_xwu64_t * a,
                             const enum xwmb_memory_order_em mo);

xwu64_t xwaop__xwu64_t__store(atomic_xwu64_t * a,
                              const enum xwmb_memory_order_em mo,
                              xwu64_t v);

void xwaop__xwu64_t__read(atomic_xwu64_t * a,
                          xwu64_t * ov);

void xwaop__xwu64_t__write(atomic_xwu64_t * a,
                           xwu64_t v,
                           xwu64_t * ov);

xwer_t xwaop__xwu64_t__teq_then_write(atomic_xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64_t__tne_then_write(atomic_xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgt_then_write(atomic_xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64_t__tge_then_write(atomic_xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64_t__tlt_then_write(atomic_xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64_t__tle_then_write(atomic_xwu64_t * a,
                                      xwu64_t t,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtlt_then_write(atomic_xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgelt_then_write(atomic_xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtle_then_write(atomic_xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgele_then_write(atomic_xwu64_t * a,
                                        xwu64_t l, xwu64_t r,
                                        xwu64_t v,
                                        xwu64_t * ov);

void xwaop__xwu64_t__add(atomic_xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__teq_then_add(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tne_then_add(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgt_then_add(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tge_then_add(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tlt_then_add(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tle_then_add(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtlt_then_add(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgelt_then_add(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtle_then_add(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgele_then_add(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64_t__sub(atomic_xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__teq_then_sub(atomic_xwu64_t * a,
                                    xwu64_t t, xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tne_then_sub(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgt_then_sub(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tge_then_sub(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tlt_then_sub(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tle_then_sub(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtlt_then_sub(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgelt_then_sub(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtle_then_sub(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgele_then_sub(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64_t__rsb(atomic_xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__teq_then_rsb(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tne_then_rsb(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgt_then_rsb(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tge_then_rsb(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tlt_then_rsb(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tle_then_rsb(atomic_xwu64_t * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtlt_then_rsb(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgelt_then_rsb(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgtle_then_rsb(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tgele_then_rsb(atomic_xwu64_t * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64_t__or(atomic_xwu64_t * a,
                        xwu64_t v,
                        xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64_t__and(atomic_xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64_t__xor(atomic_xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64_t__c0m(atomic_xwu64_t * a,
                         xwu64_t v,
                         xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__t1ma_then_c0m(atomic_xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__t1mo_then_c0m(atomic_xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__t0ma_then_s1m(atomic_xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__t0mo_then_s1m(atomic_xwu64_t * a,
                                     xwu64_t m,
                                     xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64_t__tst_then_op(atomic_xwu64_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu64_t * nv, xwu64_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws64_t xwaop__xws64_t__load(atomic_xws64_t * a,
                             const enum xwmb_memory_order_em mo);

xws64_t xwaop__xws64_t__store(atomic_xws64_t * a,
                              const enum xwmb_memory_order_em mo,
                              xws64_t v);

void xwaop__xws64_t__read(atomic_xws64_t * a,
                          xws64_t * ov);

void xwaop__xws64_t__write(atomic_xws64_t * a,
                           xws64_t v,
                           xws64_t * ov);

xwer_t xwaop__xws64_t__teq_then_write(atomic_xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64_t__tne_then_write(atomic_xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64_t__tgt_then_write(atomic_xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64_t__tge_then_write(atomic_xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64_t__tlt_then_write(atomic_xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64_t__tle_then_write(atomic_xws64_t * a,
                                      xws64_t t,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64_t__tgtlt_then_write(atomic_xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

xwer_t xwaop__xws64_t__tgelt_then_write(atomic_xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

xwer_t xwaop__xws64_t__tgtle_then_write(atomic_xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

xwer_t xwaop__xws64_t__tgele_then_write(atomic_xws64_t * a,
                                        xws64_t l, xws64_t r,
                                        xws64_t v,
                                        xws64_t * ov);

void xwaop__xws64_t__add(atomic_xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__teq_then_add(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tne_then_add(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgt_then_add(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tge_then_add(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tlt_then_add(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tle_then_add(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgtlt_then_add(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgelt_then_add(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgtle_then_add(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgele_then_add(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

void xwaop__xws64_t__sub(atomic_xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__teq_then_sub(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tne_then_sub(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgt_then_sub(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tge_then_sub(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tlt_then_sub(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tle_then_sub(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgtlt_then_sub(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgelt_then_sub(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgtle_then_sub(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgele_then_sub(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

void xwaop__xws64_t__rsb(atomic_xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__teq_then_rsb(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tne_then_rsb(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgt_then_rsb(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tge_then_rsb(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tlt_then_rsb(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tle_then_rsb(atomic_xws64_t * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgtlt_then_rsb(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgelt_then_rsb(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgtle_then_rsb(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64_t__tgele_then_rsb(atomic_xws64_t * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * nv, xws64_t * ov);

void xwaop__xws64_t__or(atomic_xws64_t * a,
                        xws64_t v,
                        xws64_t * nv, xws64_t * ov);

void xwaop__xws64_t__and(atomic_xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

void xwaop__xws64_t__xor(atomic_xws64_t * a,
                         xws64_t v,
                         xws64_t * nv, xws64_t * ov);

static __xwlib_inline
void xwaop__xws64_t__c0m(atomic_xws64_t * a,
                         xws64_t m,
                         xws64_t * nv, xws64_t * ov)
{
        xwaop__xwu64_t__c0m((atomic_xwu64_t *)a,
                            (xwu64_t)m,
                            (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t1ma_then_c0m(atomic_xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t1ma_then_c0m((atomic_xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t1mo_then_c0m(atomic_xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t1mo_then_c0m((atomic_xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t0ma_then_s1m(atomic_xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t0ma_then_s1m((atomic_xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64_t__t0mo_then_s1m(atomic_xws64_t * a,
                                     xws64_t m,
                                     xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64_t__t0mo_then_s1m((atomic_xwu64_t *)a,
                                             (xwu64_t)m,
                                             (xwu64_t *)nv, (xwu64_t *)ov);
}

xwer_t xwaop__xws64_t__tst_then_op(atomic_xws64_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xws64_t * nv, xws64_t * ov);

#include <xwosimpl_soc_xwaop64.h>

#endif /* xwos/ospl/soc/xwaop64.h */
