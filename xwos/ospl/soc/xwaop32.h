/**
 * @file
 * @brief XWOS移植层：SOC 32位原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_soc_xwaop32_h__
#define __xwos_ospl_soc_xwaop32_h__

#include <xwos/standard.h>
#include <xwosimpl_soc_xwaop32.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
xwu32_t xwaop__xwu32_t__load(atomic_xwu32_t * a,
                             const enum xwaop_memory_order_em mo);

xwu32_t xwaop__xwu32_t__store(atomic_xwu32_t * a,
                              const enum xwaop_memory_order_em mo,
                              xwu32_t v);

void xwaop__xwu32_t__read(atomic_xwu32_t * a,
                          xwu32_t * ov);

void xwaop__xwu32_t__write(atomic_xwu32_t * a,
                           xwu32_t v,
                           xwu32_t * ov);

xwer_t xwaop__xwu32_t__teq_then_write(atomic_xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32_t__tne_then_write(atomic_xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgt_then_write(atomic_xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32_t__tge_then_write(atomic_xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32_t__tlt_then_write(atomic_xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32_t__tle_then_write(atomic_xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtlt_then_write(atomic_xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgelt_then_write(atomic_xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtle_then_write(atomic_xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgele_then_write(atomic_xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov);

void xwaop__xwu32_t__add(atomic_xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__teq_then_add(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tne_then_add(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgt_then_add(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tge_then_add(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tlt_then_add(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tle_then_add(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtlt_then_add(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgelt_then_add(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtle_then_add(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgele_then_add(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32_t__sub(atomic_xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__teq_then_sub(atomic_xwu32_t * a,
                                    xwu32_t t, xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tne_then_sub(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgt_then_sub(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tge_then_sub(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tlt_then_sub(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tle_then_sub(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtlt_then_sub(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgelt_then_sub(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtle_then_sub(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgele_then_sub(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32_t__rsb(atomic_xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__teq_then_rsb(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tne_then_rsb(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgt_then_rsb(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tge_then_rsb(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tlt_then_rsb(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tle_then_rsb(atomic_xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtlt_then_rsb(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgelt_then_rsb(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgtle_then_rsb(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tgele_then_rsb(atomic_xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32_t__or(atomic_xwu32_t * a,
                        xwu32_t v,
                        xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32_t__and(atomic_xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32_t__xor(atomic_xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32_t__c0m(atomic_xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__t1ma_then_c0m(atomic_xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__t1mo_then_c0m(atomic_xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__t0ma_then_s1m(atomic_xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__t0mo_then_s1m(atomic_xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32_t__tst_then_op(atomic_xwu32_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu32_t * nv, xwu32_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws32_t xwaop__xws32_t__load(atomic_xws32_t * a,
                             const enum xwaop_memory_order_em mo);

xws32_t xwaop__xws32_t__store(atomic_xws32_t * a,
                              const enum xwaop_memory_order_em mo,
                              xws32_t v);

void xwaop__xws32_t__read(atomic_xws32_t * a,
                          xws32_t * ov);

void xwaop__xws32_t__write(atomic_xws32_t * a,
                           xws32_t v,
                           xws32_t * ov);

xwer_t xwaop__xws32_t__teq_then_write(atomic_xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32_t__tne_then_write(atomic_xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32_t__tgt_then_write(atomic_xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32_t__tge_then_write(atomic_xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32_t__tlt_then_write(atomic_xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32_t__tle_then_write(atomic_xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32_t__tgtlt_then_write(atomic_xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

xwer_t xwaop__xws32_t__tgelt_then_write(atomic_xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

xwer_t xwaop__xws32_t__tgtle_then_write(atomic_xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

xwer_t xwaop__xws32_t__tgele_then_write(atomic_xws32_t * a,
                                        xws32_t l, xws32_t r,
                                        xws32_t v,
                                        xws32_t * ov);

void xwaop__xws32_t__add(atomic_xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__teq_then_add(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tne_then_add(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgt_then_add(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tge_then_add(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tlt_then_add(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tle_then_add(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgtlt_then_add(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgelt_then_add(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgtle_then_add(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgele_then_add(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

void xwaop__xws32_t__sub(atomic_xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__teq_then_sub(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tne_then_sub(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgt_then_sub(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tge_then_sub(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tlt_then_sub(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tle_then_sub(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgtlt_then_sub(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgelt_then_sub(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgtle_then_sub(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgele_then_sub(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

void xwaop__xws32_t__rsb(atomic_xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__teq_then_rsb(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tne_then_rsb(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgt_then_rsb(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tge_then_rsb(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tlt_then_rsb(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tle_then_rsb(atomic_xws32_t * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgtlt_then_rsb(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgelt_then_rsb(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgtle_then_rsb(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32_t__tgele_then_rsb(atomic_xws32_t * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * nv, xws32_t * ov);

void xwaop__xws32_t__or(atomic_xws32_t * a,
                        xws32_t v,
                        xws32_t * nv, xws32_t * ov);

void xwaop__xws32_t__and(atomic_xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

void xwaop__xws32_t__xor(atomic_xws32_t * a,
                         xws32_t v,
                         xws32_t * nv, xws32_t * ov);

static __xwlib_inline
void xwaop__xws32_t__c0m(atomic_xws32_t * a,
                         xws32_t m,
                         xws32_t * nv, xws32_t * ov)
{
        xwaop__xwu32_t__c0m((atomic_xwu32_t *)a,
                            (xwu32_t)m,
                            (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t1ma_then_c0m(atomic_xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t1ma_then_c0m((atomic_xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t1mo_then_c0m(atomic_xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t1mo_then_c0m((atomic_xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t0ma_then_s1m(atomic_xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t0ma_then_s1m((atomic_xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32_t__t0mo_then_s1m(atomic_xws32_t * a,
                                     xws32_t m,
                                     xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32_t__t0mo_then_s1m((atomic_xwu32_t *)a,
                                             (xwu32_t)m,
                                             (xwu32_t *)nv, (xwu32_t *)ov);
}

xwer_t xwaop__xws32_t__tst_then_op(atomic_xws32_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xws32_t * nv, xws32_t * ov);

#endif /* xwos/ospl/soc/xwaop32.h */
