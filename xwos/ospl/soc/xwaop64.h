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
xwu64_t xwaop__xwu64__load(xwu64_a * a,
                           const enum xwmb_memory_order_em mo);

xwu64_t xwaop__xwu64__store(xwu64_a * a,
                            const enum xwmb_memory_order_em mo,
                            xwu64_t v);

void xwaop__xwu64__read(xwu64_a * a,
                        xwu64_t * ov);

void xwaop__xwu64__write(xwu64_a * a,
                         xwu64_t v,
                         xwu64_t * ov);

xwer_t xwaop__xwu64__teq_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov);

xwer_t xwaop__xwu64__tne_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov);

xwer_t xwaop__xwu64__tgt_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov);

xwer_t xwaop__xwu64__tge_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov);

xwer_t xwaop__xwu64__tlt_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov);

xwer_t xwaop__xwu64__tle_then_write(xwu64_a * a,
                                    xwu64_t t,
                                    xwu64_t v,
                                    xwu64_t * ov);

xwer_t xwaop__xwu64__tgtlt_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64__tgelt_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64__tgtle_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov);

xwer_t xwaop__xwu64__tgele_then_write(xwu64_a * a,
                                      xwu64_t l, xwu64_t r,
                                      xwu64_t v,
                                      xwu64_t * ov);

void xwaop__xwu64__add(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__teq_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tne_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgt_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tge_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tlt_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tle_then_add(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgtlt_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgelt_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgtle_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgele_then_add(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64__sub(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__teq_then_sub(xwu64_a * a,
                                  xwu64_t t, xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tne_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgt_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tge_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tlt_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tle_then_sub(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgtlt_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgelt_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgtle_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgele_then_sub(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64__rsb(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__teq_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tne_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgt_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tge_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tlt_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tle_then_rsb(xwu64_a * a,
                                  xwu64_t t,
                                  xwu64_t v,
                                  xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgtlt_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgelt_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgtle_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tgele_then_rsb(xwu64_a * a,
                                    xwu64_t l, xwu64_t r,
                                    xwu64_t v,
                                    xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64__or(xwu64_a * a,
                      xwu64_t v,
                      xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64__and(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64__xor(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov);

void xwaop__xwu64__c0m(xwu64_a * a,
                       xwu64_t v,
                       xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__t1ma_then_c0m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__t1mo_then_c0m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__t0ma_then_s1m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__t0mo_then_s1m(xwu64_a * a,
                                   xwu64_t m,
                                   xwu64_t * nv, xwu64_t * ov);

xwer_t xwaop__xwu64__tst_then_op(xwu64_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xwu64_t * nv, xwu64_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws64_t xwaop__xws64__load(xws64_a * a,
                           const enum xwmb_memory_order_em mo);

xws64_t xwaop__xws64__store(xws64_a * a,
                            const enum xwmb_memory_order_em mo,
                            xws64_t v);

void xwaop__xws64__read(xws64_a * a,
                        xws64_t * ov);

void xwaop__xws64__write(xws64_a * a,
                         xws64_t v,
                         xws64_t * ov);

xwer_t xwaop__xws64__teq_then_write(xws64_a * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * ov);

xwer_t xwaop__xws64__tne_then_write(xws64_a * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * ov);

xwer_t xwaop__xws64__tgt_then_write(xws64_a * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * ov);

xwer_t xwaop__xws64__tge_then_write(xws64_a * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * ov);

xwer_t xwaop__xws64__tlt_then_write(xws64_a * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * ov);

xwer_t xwaop__xws64__tle_then_write(xws64_a * a,
                                    xws64_t t,
                                    xws64_t v,
                                    xws64_t * ov);

xwer_t xwaop__xws64__tgtlt_then_write(xws64_a * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64__tgelt_then_write(xws64_a * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64__tgtle_then_write(xws64_a * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * ov);

xwer_t xwaop__xws64__tgele_then_write(xws64_a * a,
                                      xws64_t l, xws64_t r,
                                      xws64_t v,
                                      xws64_t * ov);

void xwaop__xws64__add(xws64_a * a,
                       xws64_t v,
                       xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__teq_then_add(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tne_then_add(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgt_then_add(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tge_then_add(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tlt_then_add(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tle_then_add(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgtlt_then_add(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgelt_then_add(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgtle_then_add(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgele_then_add(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

void xwaop__xws64__sub(xws64_a * a,
                       xws64_t v,
                       xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__teq_then_sub(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tne_then_sub(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgt_then_sub(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tge_then_sub(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tlt_then_sub(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tle_then_sub(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgtlt_then_sub(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgelt_then_sub(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgtle_then_sub(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgele_then_sub(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

void xwaop__xws64__rsb(xws64_a * a,
                       xws64_t v,
                       xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__teq_then_rsb(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tne_then_rsb(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgt_then_rsb(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tge_then_rsb(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tlt_then_rsb(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tle_then_rsb(xws64_a * a,
                                  xws64_t t,
                                  xws64_t v,
                                  xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgtlt_then_rsb(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgelt_then_rsb(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgtle_then_rsb(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

xwer_t xwaop__xws64__tgele_then_rsb(xws64_a * a,
                                    xws64_t l, xws64_t r,
                                    xws64_t v,
                                    xws64_t * nv, xws64_t * ov);

void xwaop__xws64__or(xws64_a * a,
                      xws64_t v,
                      xws64_t * nv, xws64_t * ov);

void xwaop__xws64__and(xws64_a * a,
                       xws64_t v,
                       xws64_t * nv, xws64_t * ov);

void xwaop__xws64__xor(xws64_a * a,
                       xws64_t v,
                       xws64_t * nv, xws64_t * ov);

static __xwlib_inline
void xwaop__xws64__c0m(xws64_a * a,
                       xws64_t m,
                       xws64_t * nv, xws64_t * ov)
{
        xwaop__xwu64__c0m((xwu64_a *)a,
                          (xwu64_t)m,
                          (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64__t1ma_then_c0m(xws64_a * a,
                                   xws64_t m,
                                   xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64__t1ma_then_c0m((xwu64_a *)a,
                                           (xwu64_t)m,
                                           (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64__t1mo_then_c0m(xws64_a * a,
                                   xws64_t m,
                                   xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64__t1mo_then_c0m((xwu64_a *)a,
                                           (xwu64_t)m,
                                           (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64__t0ma_then_s1m(xws64_a * a,
                                   xws64_t m,
                                   xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64__t0ma_then_s1m((xwu64_a *)a,
                                           (xwu64_t)m,
                                           (xwu64_t *)nv, (xwu64_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws64__t0mo_then_s1m(xws64_a * a,
                                   xws64_t m,
                                   xws64_t * nv, xws64_t * ov)
{
        return xwaop__xwu64__t0mo_then_s1m((xwu64_a *)a,
                                           (xwu64_t)m,
                                           (xwu64_t *)nv, (xwu64_t *)ov);
}

xwer_t xwaop__xws64__tst_then_op(xws64_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xws64_t * nv, xws64_t * ov);

#include <xwosimpl_soc_xwaop64.h>

#endif /* xwos/ospl/soc/xwaop64.h */
