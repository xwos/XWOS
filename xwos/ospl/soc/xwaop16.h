/**
 * @file
 * @brief 玄武OS移植层：SOC 16位原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_xwaop16_h__
#define __xwos_ospl_soc_xwaop16_h__

#include <xwos/standard.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
xwu16_t xwaop__xwu16__load(xwu16_a * a,
                           const enum xwmb_memory_order_em mo);

xwu16_t xwaop__xwu16__store(xwu16_a * a,
                            const enum xwmb_memory_order_em mo,
                            xwu16_t v);

void xwaop__xwu16__read(xwu16_a * a,
                        xwu16_t * ov);

void xwaop__xwu16__write(xwu16_a * a,
                         xwu16_t v,
                         xwu16_t * ov);

xwer_t xwaop__xwu16__teq_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov);

xwer_t xwaop__xwu16__tne_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov);

xwer_t xwaop__xwu16__tgt_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov);

xwer_t xwaop__xwu16__tge_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov);

xwer_t xwaop__xwu16__tlt_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov);

xwer_t xwaop__xwu16__tle_then_write(xwu16_a * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * ov);

xwer_t xwaop__xwu16__tgtlt_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16__tgelt_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16__tgtle_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov);

xwer_t xwaop__xwu16__tgele_then_write(xwu16_a * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * ov);

void xwaop__xwu16__add(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__teq_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tne_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgt_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tge_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tlt_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tle_then_add(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgtlt_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgelt_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgtle_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgele_then_add(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16__sub(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__teq_then_sub(xwu16_a * a,
                                  xwu16_t t, xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tne_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgt_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tge_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tlt_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tle_then_sub(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgtlt_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgelt_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgtle_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgele_then_sub(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16__rsb(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__teq_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tne_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgt_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tge_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tlt_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tle_then_rsb(xwu16_a * a,
                                  xwu16_t t,
                                  xwu16_t v,
                                  xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgtlt_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgelt_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgtle_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tgele_then_rsb(xwu16_a * a,
                                    xwu16_t l, xwu16_t r,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16__or(xwu16_a * a,
                      xwu16_t v,
                      xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16__and(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16__xor(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov);

void xwaop__xwu16__c0m(xwu16_a * a,
                       xwu16_t v,
                       xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__t1ma_then_c0m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__t1mo_then_c0m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__t0ma_then_s1m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__t0mo_then_s1m(xwu16_a * a,
                                   xwu16_t m,
                                   xwu16_t * nv, xwu16_t * ov);

xwer_t xwaop__xwu16__tst_then_op(xwu16_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xwu16_t * nv, xwu16_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws16_t xwaop__xws16__load(xws16_a * a,
                           const enum xwmb_memory_order_em mo);

xws16_t xwaop__xws16__store(xws16_a * a,
                            const enum xwmb_memory_order_em mo,
                            xws16_t v);

void xwaop__xws16__read(xws16_a * a,
                        xws16_t * ov);

void xwaop__xws16__write(xws16_a * a,
                         xws16_t v,
                         xws16_t * ov);

xwer_t xwaop__xws16__teq_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov);

xwer_t xwaop__xws16__tne_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov);

xwer_t xwaop__xws16__tgt_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov);

xwer_t xwaop__xws16__tge_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov);

xwer_t xwaop__xws16__tlt_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov);

xwer_t xwaop__xws16__tle_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov);

xwer_t xwaop__xws16__tgtlt_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16__tgelt_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16__tgtle_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov);

xwer_t xwaop__xws16__tgele_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov);

void xwaop__xws16__add(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__teq_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tne_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgt_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tge_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tlt_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tle_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgtlt_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgelt_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgtle_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgele_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

void xwaop__xws16__sub(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__teq_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tne_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgt_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tge_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tlt_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tle_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgtlt_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgelt_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgtle_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgele_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

void xwaop__xws16__rsb(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__teq_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tne_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgt_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tge_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tlt_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tle_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgtlt_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgelt_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgtle_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

xwer_t xwaop__xws16__tgele_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov);

void xwaop__xws16__or(xws16_a * a,
                      xws16_t v,
                      xws16_t * nv, xws16_t * ov);

void xwaop__xws16__and(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov);

void xwaop__xws16__xor(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov);

static __xwlib_inline
void xwaop__xws16__c0m(xws16_a * a,
                       xws16_t m,
                       xws16_t * nv, xws16_t * ov)
{
        xwaop__xwu16__c0m((xwu16_a *)a,
                          (xwu16_t)m,
                          (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16__t1ma_then_c0m(xws16_a * a,
                                   xws16_t m,
                                   xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16__t1ma_then_c0m((xwu16_a *)a,
                                           (xwu16_t)m,
                                           (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16__t1mo_then_c0m(xws16_a * a,
                                   xws16_t m,
                                   xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16__t1mo_then_c0m((xwu16_a *)a,
                                           (xwu16_t)m,
                                           (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16__t0ma_then_s1m(xws16_a * a,
                                   xws16_t m,
                                   xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16__t0ma_then_s1m((xwu16_a *)a,
                                           (xwu16_t)m,
                                           (xwu16_t *)nv, (xwu16_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws16__t0mo_then_s1m(xws16_a * a,
                                   xws16_t m,
                                   xws16_t * nv, xws16_t * ov)
{
        return xwaop__xwu16__t0mo_then_s1m((xwu16_a *)a,
                                           (xwu16_t)m,
                                           (xwu16_t *)nv, (xwu16_t *)ov);
}

xwer_t xwaop__xws16__tst_then_op(xws16_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xws16_t * nv, xws16_t * ov);

#include <xwosimpl_soc_xwaop16.h>

#endif /* xwos/ospl/soc/xwaop16.h */
