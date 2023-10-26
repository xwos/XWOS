/**
 * @file
 * @brief XWOS移植层：SOC：原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_soc_xwaop_h__
#define __xwos_ospl_soc_xwaop_h__

#include <xwos/standard.h>
#include <xwosimpl_soc_xwaop.h>

/******** ******** ******** ******** unsigned 8bit ******** ******** ******** ********/
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

/******** ******** ******** ******** signed 8bit ******** ******** ******** ********/
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

/******** ******** ******** ******** unsigned 16bit ******** ******** ******** ********/
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

/******** ******** ******** ******** signed 16bit ******** ******** ******** ********/
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

/******** ******** ******** ******** unsigned 32bit ******** ******** ******** ********/
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

/******** ******** ******** ******** signed 323bit ******** ******** ******** ********/
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

/******** ******** ******** ******** unsigned 64bit ******** ******** ******** ********/
xwu64_t xwaop__xwu64_t__load(atomic_xwu64_t * a,
                             const enum xwaop_memory_order_em mo);

xwu64_t xwaop__xwu64_t__store(atomic_xwu64_t * a,
                              const enum xwaop_memory_order_em mo,
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

/******** ******** ******** ******** signed 64bit ******** ******** ******** ********/
xws64_t xwaop__xws64_t__load(atomic_xws64_t * a,
                             const enum xwaop_memory_order_em mo);

xws64_t xwaop__xws64_t__store(atomic_xws64_t * a,
                              const enum xwaop_memory_order_em mo,
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

/******** ******** ******** ******** bitmap ******** ******** ******** ********/
bool xwbmpaop_t1i(atomic_xwbmp_t * bmp, xwsq_t idx);
void xwbmpaop_s1i(atomic_xwbmp_t * bmp, xwsq_t idx);
void xwbmpaop_c0i(atomic_xwbmp_t * bmp, xwsq_t idx);
void xwbmpaop_x1i(atomic_xwbmp_t * bmp, xwsq_t idx);
xwer_t xwbmpaop_t0i_then_s1i(atomic_xwbmp_t * bmp, xwsq_t idx);
xwer_t xwbmpaop_t1i_then_c0i(atomic_xwbmp_t * bmp, xwsq_t idx);
xwssq_t xwbmpaop_fls_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num);
xwssq_t xwbmpaop_flz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num);
xwssq_t xwbmpaop_ffs_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num);
xwssq_t xwbmpaop_ffz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num);

#endif /* xwos/ospl/soc/xwaop.h */
