/**
 * @file
 * @brief XWOS通用库：有符号16位原子操作函数库
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>

#if __STDC_VERSION__ >= 201112L

#include <stdatomic.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xws16_t xwaop__xws16__load(xws16_a * a,
                           const enum xwmb_memory_order_em mo)
{
        xws16_t v;

        v = atomic_load_explicit(a, (int)mo);
        return v;
}

__xwlib_code
xws16_t xwaop__xws16__store(xws16_a * a,
                            const enum xwmb_memory_order_em mo,
                            xws16_t v)
{
        atomic_store_explicit(a, v, (int)mo);
        return v;
}

__xwlib_code
void xwaop__xws16__read(xws16_a * a,
                        xws16_t * ov)
{
        xws16_t o;

        o = atomic_load_explicit(a, memory_order_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws16__write(xws16_a * a,
                         xws16_t v,
                         xws16_t * ov)
{
        xws16_t o;

        o = atomic_exchange_explicit(a, v, memory_order_acq_rel);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws16__teq_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        o = t;
        rc = (xwer_t)atomic_compare_exchange_strong_explicit(a, &o, v,
                                                             memory_order_acq_rel,
                                                             memory_order_consume);
        if ((bool)rc) {
                rc = XWOK;
        } else {
                rc = -EACCES;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tne_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o != t) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgt_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o > t) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tge_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o >= t) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tlt_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o < t) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tle_then_write(xws16_a * a,
                                    xws16_t t,
                                    xws16_t v,
                                    xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o <= t) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtlt_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o < r)) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgelt_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o < r)) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtle_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o <= r)) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgele_then_write(xws16_a * a,
                                      xws16_t l, xws16_t r,
                                      xws16_t v,
                                      xws16_t * ov)
{
        xws16_t o;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o <= r)) {
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, v,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != XWOK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** add ******** ********/
__xwlib_code
void xwaop__xws16__add(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov)
{
        xws16_t o;

        o = atomic_fetch_add_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o + v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws16__teq_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        o = t;
        n = o + v;
        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                a, &o, n,
                memory_order_acq_rel,
                memory_order_consume);
        if ((bool)rc) {
                rc = XWOK;
        } else {
                rc = -EACCES;
                n = o;
        }
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tne_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o != t) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgt_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o > t) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tge_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o >= t) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tlt_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o < t) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tle_then_add(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o <= t) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtlt_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgelt_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtle_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgele_then_add(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** subtract ******** ********/
__xwlib_code
void xwaop__xws16__sub(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov)
{
        xws16_t o;

        o = atomic_fetch_sub_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o - v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws16__teq_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        o = t;
        n = o - v;
        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                a, &o, n,
                memory_order_acq_rel,
                memory_order_consume);
        if ((bool)rc) {
                rc = XWOK;
        } else {
                rc = -EACCES;
                n = o;
        }
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tne_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o != t) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgt_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o > t) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tge_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o >= t) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tlt_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o < t) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tle_then_sub(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o <= t) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtlt_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgelt_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o < r)) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtle_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgele_then_sub(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** reverse subtract ******** ********/
__xwlib_code
void xwaop__xws16__rsb(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        bool r;

        do {
                o = atomic_load_explicit(a, memory_order_relaxed);
                n = v - o;
                r = atomic_compare_exchange_strong_explicit(
                        a, &o, n,
                        memory_order_acq_rel,
                        memory_order_relaxed);
        } while (!r);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws16__teq_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o == t) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tne_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o != t) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgt_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o > t) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tge_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o >= t) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tlt_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o < t) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tle_then_rsb(xws16_a * a,
                                  xws16_t t,
                                  xws16_t v,
                                  xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o <= t) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtlt_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgelt_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgtle_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws16__tgele_then_rsb(xws16_a * a,
                                    xws16_t l, xws16_t r,
                                    xws16_t v,
                                    xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                a, &o, n,
                                memory_order_acq_rel,
                                memory_order_consume);
                        if ((bool)rc) {
                                rc = XWOK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != XWOK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** bit operations ******** ********/
__xwlib_code
void xwaop__xws16__or(xws16_a * a,
                      xws16_t v,
                      xws16_t * nv, xws16_t * ov)
{
        xws16_t o;

        o = atomic_fetch_or_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o | v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws16__and(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov)
{
        xws16_t o;

        o = atomic_fetch_and_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o & v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws16__xor(xws16_a * a,
                       xws16_t v,
                       xws16_t * nv, xws16_t * ov)
{
        xws16_t o;

        o = atomic_fetch_xor_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o ^ v;
        }
        if (ov) {
                *ov = o;
        }
}

/******** ******** test and operation ******** ********/
__xwlib_code
xwer_t xwaop__xws16__tst_then_op(xws16_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xws16_t * nv, xws16_t * ov)
{
        xws16_t o;
        xws16_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (tst) {
                        rc = tst((const void *)&o, tst_args);
                        if (XWOK == rc) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                                a, &o, n,
                                                memory_order_acq_rel,
                                                memory_order_consume);
                                } else {
                                        n = o;
                                        break;
                                }
                        } else {
                                n = o;
                                break;
                        }
                } else {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                rc = (xwer_t)atomic_compare_exchange_strong_explicit(
                                        a, &o, n,
                                        memory_order_acq_rel,
                                        memory_order_consume);
                        } else {
                                rc = XWOK;
                                n = o;
                                break;
                        }
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

#endif
