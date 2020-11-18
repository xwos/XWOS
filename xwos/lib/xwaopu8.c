/**
 * @file
 * @brief XWOS通用库：无符号8位原子操作函数库
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
xwu8_t xwaop__xwu8_t__load(__xwcc_atomic xwu8_t * a,
                           const enum xwmb_memory_order_em mo)
{
        xwu8_t v;

        v = atomic_load_explicit(a, (int)mo);
        return v;
}

__xwlib_code
xwu8_t xwaop__xwu8_t__store(__xwcc_atomic xwu8_t * a,
                            const enum xwmb_memory_order_em mo,
                            xwu8_t v)
{
        atomic_store_explicit(a, v, (int)mo);
        return v;
}

__xwlib_code
void xwaop__xwu8_t__read(__xwcc_atomic xwu8_t * a,
                         xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_load_explicit(a, memory_order_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8_t__write(__xwcc_atomic xwu8_t * a,
                          xwu8_t v,
                          xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_exchange_explicit(a, v, memory_order_acq_rel);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8_t__teq_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tne_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tgt_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tge_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tlt_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tle_then_write(__xwcc_atomic xwu8_t * a,
                                     xwu8_t t,
                                     xwu8_t v,
                                     xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tgtlt_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tgelt_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tgtle_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov)
{
        xwu8_t o;
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
xwer_t xwaop__xwu8_t__tgele_then_write(__xwcc_atomic xwu8_t * a,
                                       xwu8_t l, xwu8_t r,
                                       xwu8_t v,
                                       xwu8_t * ov)
{
        xwu8_t o;
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
void xwaop__xwu8_t__add(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_fetch_add_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o + v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8_t__teq_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tne_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgt_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tge_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tlt_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tle_then_add(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgtlt_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgelt_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgtle_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgele_then_add(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
void xwaop__xwu8_t__sub(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_fetch_sub_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o - v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8_t__teq_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tne_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgt_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tge_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tlt_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tle_then_sub(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgtlt_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgelt_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgtle_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgele_then_sub(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
void xwaop__xwu8_t__rsb(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__teq_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tne_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgt_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tge_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tlt_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tle_then_rsb(__xwcc_atomic xwu8_t * a,
                                   xwu8_t t,
                                   xwu8_t v,
                                   xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgtlt_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgelt_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgtle_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
xwer_t xwaop__xwu8_t__tgele_then_rsb(__xwcc_atomic xwu8_t * a,
                                     xwu8_t l, xwu8_t r,
                                     xwu8_t v,
                                     xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
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
void xwaop__xwu8_t__or(__xwcc_atomic xwu8_t * a,
                       xwu8_t v,
                       xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_fetch_or_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o | v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8_t__and(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_fetch_and_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o & v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8_t__xor(__xwcc_atomic xwu8_t * a,
                        xwu8_t v,
                        xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_fetch_xor_explicit(a, v, memory_order_acq_rel);
        if (nv) {
                *nv = o ^ v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu8_t__c0m(__xwcc_atomic xwu8_t * a,
                        xwu8_t m,
                        xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;

        o = atomic_fetch_and_explicit(a, ~m, memory_order_acq_rel);
        if (nv) {
                *nv = o & (~m);
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu8_t__t1ma_then_c0m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o & m) == m) {
                        n = o & (~m);
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
xwer_t xwaop__xwu8_t__t1mo_then_c0m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (o & m) {
                        n = o & (~m);
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
xwer_t xwaop__xwu8_t__t0ma_then_s1m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (!(o & m)) {
                        n = o | m;
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
xwer_t xwaop__xwu8_t__t0mo_then_s1m(__xwcc_atomic xwu8_t * a,
                                    xwu8_t m,
                                    xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if ((o & m) != m) {
                        n = o | m;
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

/******** ******** test and operation ******** ********/
__xwlib_code
xwer_t xwaop__xwu8_t__tst_then_op(__xwcc_atomic xwu8_t * a,
                                  xwaop_tst_f tst, void * tst_args,
                                  xwaop_op_f op, void * op_args,
                                  xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = atomic_load_explicit(a, memory_order_consume);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
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
                        } else {
                                rc = -EACCES;
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

#endif /* __STDC_VERSION__ >= 201112L */
