/**
 * @file
 * @brief 玄武OS移植层：SOC 32位原子操作
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_xwaop32_h__
#define __xwos_ospl_soc_xwaop32_h__

#include <xwos/standard.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
xwu32_t xwaop__xwu32__load(xwu32_a * a,
                           const enum xwmb_memory_order_em mo);

xwu32_t xwaop__xwu32__store(xwu32_a * a,
                            const enum xwmb_memory_order_em mo,
                            xwu32_t v);

void xwaop__xwu32__read(xwu32_a * a,
                        xwu32_t * ov);

void xwaop__xwu32__write(xwu32_a * a,
                         xwu32_t v,
                         xwu32_t * ov);

xwer_t xwaop__xwu32__teq_then_write(xwu32_a * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * ov);

xwer_t xwaop__xwu32__tne_then_write(xwu32_a * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * ov);

xwer_t xwaop__xwu32__tgt_then_write(xwu32_a * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * ov);

xwer_t xwaop__xwu32__tge_then_write(xwu32_a * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * ov);

xwer_t xwaop__xwu32__tlt_then_write(xwu32_a * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * ov);

xwer_t xwaop__xwu32__tle_then_write(xwu32_a * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * ov);

xwer_t xwaop__xwu32__tgtlt_then_write(xwu32_a * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32__tgelt_then_write(xwu32_a * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32__tgtle_then_write(xwu32_a * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * ov);

xwer_t xwaop__xwu32__tgele_then_write(xwu32_a * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * ov);

void xwaop__xwu32__add(xwu32_a * a,
                       xwu32_t v,
                       xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__teq_then_add(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tne_then_add(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgt_then_add(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tge_then_add(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tlt_then_add(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tle_then_add(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgtlt_then_add(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgelt_then_add(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgtle_then_add(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgele_then_add(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32__sub(xwu32_a * a,
                       xwu32_t v,
                       xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__teq_then_sub(xwu32_a * a,
                                  xwu32_t t, xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tne_then_sub(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgt_then_sub(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tge_then_sub(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tlt_then_sub(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tle_then_sub(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgtlt_then_sub(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgelt_then_sub(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgtle_then_sub(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgele_then_sub(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32__rsb(xwu32_a * a,
                       xwu32_t v,
                       xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__teq_then_rsb(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tne_then_rsb(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgt_then_rsb(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tge_then_rsb(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tlt_then_rsb(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tle_then_rsb(xwu32_a * a,
                                  xwu32_t t,
                                  xwu32_t v,
                                  xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgtlt_then_rsb(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgelt_then_rsb(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgtle_then_rsb(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tgele_then_rsb(xwu32_a * a,
                                    xwu32_t l, xwu32_t r,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32__or(xwu32_a * a,
                      xwu32_t v,
                      xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32__and(xwu32_a * a,
                       xwu32_t v,
                       xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32__xor(xwu32_a * a,
                       xwu32_t v,
                       xwu32_t * nv, xwu32_t * ov);

void xwaop__xwu32__c0m(xwu32_a * a,
                       xwu32_t v,
                       xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__t1ma_then_c0m(xwu32_a * a,
                                   xwu32_t m,
                                   xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__t1mo_then_c0m(xwu32_a * a,
                                   xwu32_t m,
                                   xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__t0ma_then_s1m(xwu32_a * a,
                                   xwu32_t m,
                                   xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__t0mo_then_s1m(xwu32_a * a,
                                   xwu32_t m,
                                   xwu32_t * nv, xwu32_t * ov);

xwer_t xwaop__xwu32__tst_then_op(xwu32_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xwu32_t * nv, xwu32_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws32_t xwaop__xws32__load(xws32_a * a,
                           const enum xwmb_memory_order_em mo);

xws32_t xwaop__xws32__store(xws32_a * a,
                            const enum xwmb_memory_order_em mo,
                            xws32_t v);

void xwaop__xws32__read(xws32_a * a,
                        xws32_t * ov);

void xwaop__xws32__write(xws32_a * a,
                         xws32_t v,
                         xws32_t * ov);

xwer_t xwaop__xws32__teq_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov);

xwer_t xwaop__xws32__tne_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov);

xwer_t xwaop__xws32__tgt_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov);

xwer_t xwaop__xws32__tge_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov);

xwer_t xwaop__xws32__tlt_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov);

xwer_t xwaop__xws32__tle_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov);

xwer_t xwaop__xws32__tgtlt_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32__tgelt_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32__tgtle_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov);

xwer_t xwaop__xws32__tgele_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov);

void xwaop__xws32__add(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__teq_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tne_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgt_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tge_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tlt_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tle_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgtlt_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgelt_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgtle_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgele_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

void xwaop__xws32__sub(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__teq_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tne_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgt_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tge_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tlt_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tle_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgtlt_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgelt_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgtle_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgele_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

void xwaop__xws32__rsb(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__teq_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tne_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgt_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tge_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tlt_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tle_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgtlt_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgelt_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgtle_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

xwer_t xwaop__xws32__tgele_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov);

void xwaop__xws32__or(xws32_a * a,
                      xws32_t v,
                      xws32_t * nv, xws32_t * ov);

void xwaop__xws32__and(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov);

void xwaop__xws32__xor(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov);

static __xwlib_inline
void xwaop__xws32__c0m(xws32_a * a,
                       xws32_t m,
                       xws32_t * nv, xws32_t * ov)
{
        xwaop__xwu32__c0m((xwu32_a *)a,
                          (xwu32_t)m,
                          (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32__t1ma_then_c0m(xws32_a * a,
                                   xws32_t m,
                                   xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32__t1ma_then_c0m((xwu32_a *)a,
                                           (xwu32_t)m,
                                           (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32__t1mo_then_c0m(xws32_a * a,
                                   xws32_t m,
                                   xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32__t1mo_then_c0m((xwu32_a *)a,
                                           (xwu32_t)m,
                                           (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32__t0ma_then_s1m(xws32_a * a,
                                   xws32_t m,
                                   xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32__t0ma_then_s1m((xwu32_a *)a,
                                           (xwu32_t)m,
                                           (xwu32_t *)nv, (xwu32_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws32__t0mo_then_s1m(xws32_a * a,
                                   xws32_t m,
                                   xws32_t * nv, xws32_t * ov)
{
        return xwaop__xwu32__t0mo_then_s1m((xwu32_a *)a,
                                           (xwu32_t)m,
                                           (xwu32_t *)nv, (xwu32_t *)ov);
}

xwer_t xwaop__xws32__tst_then_op(xws32_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xws32_t * nv, xws32_t * ov);

#include <xwosimpl_soc_xwaop32.h>

#endif /* xwos/ospl/soc/xwaop32.h */
