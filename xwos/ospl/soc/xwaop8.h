/**
 * @file
 * @brief 玄武OS移植层：SOC 8位原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_soc_xwaop8_h__
#define __xwos_ospl_soc_xwaop8_h__

#include <xwos/standard.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
xwu8_t xwaop__xwu8_t__load(atomic_xwu8_t * a,
                           const enum xwaop_memory_order_em mo);

xwu8_t xwaop__xwu8_t__store(atomic_xwu8_t * a,
                            const enum xwaop_memory_order_em mo,
                            xwu8_t v);

void xwaop__xwu8_t__read(atomic_xwu8_t * a,
                         xwu8_t * ov);

void xwaop__xwu8_t__write(atomic_xwu8_t * a,
                          xwu8_t v,
                          xwu8_t * ov);

xwer_t xwaop__xwu8_t__teq_then_write(atomic_xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8_t__tne_then_write(atomic_xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgt_then_write(atomic_xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8_t__tge_then_write(atomic_xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8_t__tlt_then_write(atomic_xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8_t__tle_then_write(atomic_xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtlt_then_write(atomic_xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgelt_then_write(atomic_xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtle_then_write(atomic_xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgele_then_write(atomic_xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov);

void xwaop__xwu8_t__add(atomic_xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__teq_then_add(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tne_then_add(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgt_then_add(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tge_then_add(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tlt_then_add(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tle_then_add(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtlt_then_add(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgelt_then_add(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtle_then_add(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgele_then_add(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8_t__sub(atomic_xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__teq_then_sub(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tne_then_sub(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgt_then_sub(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tge_then_sub(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tlt_then_sub(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tle_then_sub(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtlt_then_sub(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgelt_then_sub(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtle_then_sub(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgele_then_sub(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8_t__rsb(atomic_xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__teq_then_rsb(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tne_then_rsb(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgt_then_rsb(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tge_then_rsb(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tlt_then_rsb(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tle_then_rsb(atomic_xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtlt_then_rsb(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgelt_then_rsb(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgtle_then_rsb(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tgele_then_rsb(atomic_xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8_t__or(atomic_xwu8_t * a,
                       xwu8_t v,
                       xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8_t__and(atomic_xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8_t__xor(atomic_xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8_t__c0m(atomic_xwu8_t * a,
                        xwu8_t m,
                        xwu8_t * nv, xwu8_t * ov);


xwer_t xwaop__xwu8_t__t1ma_then_c0m(atomic_xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__t1mo_then_c0m(atomic_xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__t0ma_then_s1m(atomic_xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__t0mo_then_s1m(atomic_xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8_t__tst_then_op(atomic_xwu8_t * a,
                                  xwaop_tst_f tst, void * tst_args,
                                  xwaop_op_f op, void * op_args,
                                  xwu8_t * nv, xwu8_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws8_t xwaop__xws8_t__load(atomic_xws8_t * a,
                           const enum xwaop_memory_order_em mo);

xws8_t xwaop__xws8_t__store(atomic_xws8_t * a,
                            const enum xwaop_memory_order_em mo,
                            xws8_t v);

void xwaop__xws8_t__read(atomic_xws8_t * a,
                         xws8_t * ov);

void xwaop__xws8_t__write(atomic_xws8_t * a,
                          xws8_t v,
                          xws8_t * ov);

xwer_t xwaop__xws8_t__teq_then_write(atomic_xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8_t__tne_then_write(atomic_xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8_t__tgt_then_write(atomic_xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8_t__tge_then_write(atomic_xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8_t__tlt_then_write(atomic_xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8_t__tle_then_write(atomic_xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8_t__tgtlt_then_write(atomic_xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

xwer_t xwaop__xws8_t__tgelt_then_write(atomic_xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

xwer_t xwaop__xws8_t__tgtle_then_write(atomic_xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

xwer_t xwaop__xws8_t__tgele_then_write(atomic_xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov);

void xwaop__xws8_t__add(atomic_xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__teq_then_add(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tne_then_add(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgt_then_add(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tge_then_add(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tlt_then_add(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tle_then_add(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgtlt_then_add(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgelt_then_add(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgtle_then_add(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgele_then_add(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

void xwaop__xws8_t__sub(atomic_xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__teq_then_sub(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tne_then_sub(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgt_then_sub(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tge_then_sub(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tlt_then_sub(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tle_then_sub(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgtlt_then_sub(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgelt_then_sub(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgtle_then_sub(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgele_then_sub(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

void xwaop__xws8_t__rsb(atomic_xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__teq_then_rsb(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tne_then_rsb(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgt_then_rsb(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tge_then_rsb(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tlt_then_rsb(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tle_then_rsb(atomic_xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgtlt_then_rsb(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgelt_then_rsb(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgtle_then_rsb(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8_t__tgele_then_rsb(atomic_xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov);

void xwaop__xws8_t__or(atomic_xws8_t * a,
                       xws8_t v,
                       xws8_t * nv, xws8_t * ov);

void xwaop__xws8_t__and(atomic_xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

void xwaop__xws8_t__xor(atomic_xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov);

static __xwlib_inline
void xwaop__xws8_t__c0m(atomic_xws8_t * a,
                        xws8_t m,
                        xws8_t * nv, xws8_t * ov)
{
        xwaop__xwu8_t__c0m((atomic_xwu8_t *)a,
                           (xwu8_t)m,
                           (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t1ma_then_c0m(atomic_xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t1ma_then_c0m((atomic_xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t1mo_then_c0m(atomic_xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t1mo_then_c0m((atomic_xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t0ma_then_s1m(atomic_xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t0ma_then_s1m((atomic_xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8_t__t0mo_then_s1m(atomic_xws8_t * a,
                                    xws8_t m,
                                    xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8_t__t0mo_then_s1m((atomic_xwu8_t *)a,
                                            (xwu8_t)m,
                                            (xwu8_t *)nv, (xwu8_t *)ov);
}

xwer_t xwaop__xws8_t__tst_then_op(atomic_xws8_t * a,
                                  xwaop_tst_f tst, void * tst_args,
                                  xwaop_op_f op, void * op_args,
                                  xws8_t * nv, xws8_t * ov);

#include <xwosimpl_soc_xwaop8.h>

#endif /* xwos/ospl/soc/xwaop8.h */
