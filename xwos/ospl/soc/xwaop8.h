/**
 * @file
 * @brief 玄武OS移植层：SOC 8位原子操作
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_xwaop8_h__
#define __xwos_ospl_soc_xwaop8_h__

#include <xwos/standard.h>

/******** ******** ******** ******** unsigned ******** ******** ******** ********/
xwu8_t xwaop__xwu8__load(xwu8_a * a,
                         const enum xwmb_memory_order_em mo);

xwu8_t xwaop__xwu8__store(xwu8_a * a,
                          const enum xwmb_memory_order_em mo,
                          xwu8_t v);

void xwaop__xwu8__read(xwu8_a * a,
                       xwu8_t * ov);

void xwaop__xwu8__write(xwu8_a * a,
                        xwu8_t v,
                        xwu8_t * ov);

xwer_t xwaop__xwu8__teq_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov);

xwer_t xwaop__xwu8__tne_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov);

xwer_t xwaop__xwu8__tgt_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov);

xwer_t xwaop__xwu8__tge_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov);

xwer_t xwaop__xwu8__tlt_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov);

xwer_t xwaop__xwu8__tle_then_write(xwu8_a * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * ov);

xwer_t xwaop__xwu8__tgtlt_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8__tgelt_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8__tgtle_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov);

xwer_t xwaop__xwu8__tgele_then_write(xwu8_a * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * ov);

void xwaop__xwu8__add(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__teq_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tne_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgt_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tge_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tlt_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tle_then_add(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgtlt_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgelt_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgtle_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgele_then_add(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8__sub(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__teq_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tne_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgt_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tge_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tlt_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tle_then_sub(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgtlt_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgelt_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgtle_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgele_then_sub(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8__rsb(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__teq_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tne_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgt_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tge_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tlt_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tle_then_rsb(xwu8_a * a,
                                 xwu8_t t,
                                 xwu8_t v,
                                 xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgtlt_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgelt_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgtle_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tgele_then_rsb(xwu8_a * a,
                                   xwu8_t l, xwu8_t r,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8__or(xwu8_a * a,
                     xwu8_t v,
                     xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8__and(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8__xor(xwu8_a * a,
                      xwu8_t v,
                      xwu8_t * nv, xwu8_t * ov);

void xwaop__xwu8__c0m(xwu8_a * a,
                      xwu8_t m,
                      xwu8_t * nv, xwu8_t * ov);


xwer_t xwaop__xwu8__t1ma_then_c0m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__t1mo_then_c0m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__t0ma_then_s1m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__t0mo_then_s1m(xwu8_a * a,
                                  xwu8_t m,
                                  xwu8_t * nv, xwu8_t * ov);

xwer_t xwaop__xwu8__tst_then_op(xwu8_a * a,
                                xwaop_tst_f tst, void * tst_args,
                                xwaop_op_f op, void * op_args,
                                xwu8_t * nv, xwu8_t * ov);

/******** ******** ******** ******** signed ******** ******** ******** ********/
xws8_t xwaop__xws8__load(xws8_a * a,
                         const enum xwmb_memory_order_em mo);

xws8_t xwaop__xws8__store(xws8_a * a,
                          const enum xwmb_memory_order_em mo,
                          xws8_t v);

void xwaop__xws8__read(xws8_a * a,
                       xws8_t * ov);

void xwaop__xws8__write(xws8_a * a,
                        xws8_t v,
                        xws8_t * ov);

xwer_t xwaop__xws8__teq_then_write(xws8_a * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * ov);

xwer_t xwaop__xws8__tne_then_write(xws8_a * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * ov);

xwer_t xwaop__xws8__tgt_then_write(xws8_a * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * ov);

xwer_t xwaop__xws8__tge_then_write(xws8_a * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * ov);

xwer_t xwaop__xws8__tlt_then_write(xws8_a * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * ov);

xwer_t xwaop__xws8__tle_then_write(xws8_a * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * ov);

xwer_t xwaop__xws8__tgtlt_then_write(xws8_a * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8__tgelt_then_write(xws8_a * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8__tgtle_then_write(xws8_a * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * ov);

xwer_t xwaop__xws8__tgele_then_write(xws8_a * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * ov);

void xwaop__xws8__add(xws8_a * a,
                      xws8_t v,
                      xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__teq_then_add(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tne_then_add(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgt_then_add(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tge_then_add(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tlt_then_add(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tle_then_add(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgtlt_then_add(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgelt_then_add(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgtle_then_add(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgele_then_add(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

void xwaop__xws8__sub(xws8_a * a,
                      xws8_t v,
                      xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__teq_then_sub(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tne_then_sub(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgt_then_sub(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tge_then_sub(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tlt_then_sub(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tle_then_sub(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgtlt_then_sub(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgelt_then_sub(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgtle_then_sub(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgele_then_sub(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

void xwaop__xws8__rsb(xws8_a * a,
                      xws8_t v,
                      xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__teq_then_rsb(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tne_then_rsb(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgt_then_rsb(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tge_then_rsb(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tlt_then_rsb(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tle_then_rsb(xws8_a * a,
                                 xws8_t t,
                                 xws8_t v,
                                 xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgtlt_then_rsb(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgelt_then_rsb(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgtle_then_rsb(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

xwer_t xwaop__xws8__tgele_then_rsb(xws8_a * a,
                                   xws8_t l, xws8_t r,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov);

void xwaop__xws8__or(xws8_a * a,
                     xws8_t v,
                     xws8_t * nv, xws8_t * ov);

void xwaop__xws8__and(xws8_a * a,
                      xws8_t v,
                      xws8_t * nv, xws8_t * ov);

void xwaop__xws8__xor(xws8_a * a,
                      xws8_t v,
                      xws8_t * nv, xws8_t * ov);

static __xwlib_inline
void xwaop__xws8__c0m(xws8_a * a,
                      xws8_t m,
                      xws8_t * nv, xws8_t * ov)
{
        xwaop__xwu8__c0m((xwu8_a *)a,
                         (xwu8_t)m,
                         (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8__t1ma_then_c0m(xws8_a * a,
                                  xws8_t m,
                                  xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8__t1ma_then_c0m((xwu8_a *)a,
                                          (xwu8_t)m,
                                          (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8__t1mo_then_c0m(xws8_a * a,
                                  xws8_t m,
                                  xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8__t1mo_then_c0m((xwu8_a *)a,
                                          (xwu8_t)m,
                                          (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8__t0ma_then_s1m(xws8_a * a,
                                  xws8_t m,
                                  xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8__t0ma_then_s1m((xwu8_a *)a,
                                          (xwu8_t)m,
                                          (xwu8_t *)nv, (xwu8_t *)ov);
}

static __xwlib_inline
xwer_t xwaop__xws8__t0mo_then_s1m(xws8_a * a,
                                  xws8_t m,
                                  xws8_t * nv, xws8_t * ov)
{
        return xwaop__xwu8__t0mo_then_s1m((xwu8_a *)a,
                                          (xwu8_t)m,
                                          (xwu8_t *)nv, (xwu8_t *)ov);
}

xwer_t xwaop__xws8__tst_then_op(xws8_a * a,
                                xwaop_tst_f tst, void * tst_args,
                                xwaop_op_f op, void * op_args,
                                xws8_t * nv, xws8_t * ov);

#include <xwosimpl_soc_xwaop8.h>

#endif /* xwos/ospl/soc/xwaop8.h */
