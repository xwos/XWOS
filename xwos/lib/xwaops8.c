/**
 * @file
 * @brief XWOS通用库：有符号8位原子操作函数库
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#ifdef __GNUC__
__xwlib_code
xws8_t xwaop__xws8_t__load(__atomic xws8_t * a,
                           const enum xwmb_memory_order_em mo)
{
        xws8_t v;

        v = __atomic_load_n(a, (int)mo);
        return v;
}

__xwlib_code
xws8_t xwaop__xws8_t__store(__atomic xws8_t * a,
                            const enum xwmb_memory_order_em mo,
                            xws8_t v)
{
        __atomic_store_n(a, v, (int)mo);
        return v;
}

__xwlib_code
void xwaop__xws8_t__read(__atomic xws8_t * a,
                         xws8_t * ov)
{
        xws8_t o;

        o = xwaop__xws8_t__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws8_t__write(__atomic xws8_t * a,
                          xws8_t v,
                          xws8_t * ov)
{
        xws8_t o;

        o = __atomic_exchange_n(a, v, __ATOMIC_ACQ_REL);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        o = t;
        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                 false,
                                                 __ATOMIC_ACQ_REL,
                                                 __ATOMIC_CONSUME);
        if ((bool)rc) {
                rc = OK;
        } else {
                rc = -EACCES;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_write(__atomic xws8_t * a,
                                     xws8_t t,
                                     xws8_t v,
                                     xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_write(__atomic xws8_t * a,
                                       xws8_t l, xws8_t r,
                                       xws8_t v,
                                       xws8_t * ov)
{
        xws8_t o;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, v,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc != OK);
        if (ov) {
                *ov = o;
        }
        return rc;
}

/******** ******** add ******** ********/
__xwlib_code
void xwaop__xws8_t__add(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t o;

        o = __atomic_fetch_add(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o + v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        o = t;
        n = o + v;
        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                 false,
                                                 __ATOMIC_ACQ_REL,
                                                 __ATOMIC_CONSUME);
        if ((bool)rc) {
                rc = OK;
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
xwer_t xwaop__xws8_t__tne_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_add(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_add(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
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
void xwaop__xws8_t__sub(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t o;

        o = __atomic_fetch_sub(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o - v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        o = t;
        n = o - v;
        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                 false,
                                                 __ATOMIC_ACQ_REL,
                                                 __ATOMIC_CONSUME);
        if ((bool)rc) {
                rc = OK;
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
xwer_t xwaop__xws8_t__tne_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_sub(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_sub(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
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
void xwaop__xws8_t__rsb(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        bool r;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                n = v - o;
                r = __atomic_compare_exchange_n(a, &o, n,
                                                false,
                                                __ATOMIC_ACQ_REL,
                                                __ATOMIC_CONSUME);
        } while (!r);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws8_t__teq_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o == t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tne_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o != t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgt_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o > t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tge_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o >= t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tlt_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o < t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tle_then_rsb(__atomic xws8_t * a,
                                   xws8_t t,
                                   xws8_t v,
                                   xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (o <= t) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtlt_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgelt_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgtle_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws8_t__tgele_then_rsb(__atomic xws8_t * a,
                                     xws8_t l, xws8_t r,
                                     xws8_t v,
                                     xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = (xwer_t)__atomic_compare_exchange_n(a, &o, n,
                                                                 false,
                                                                 __ATOMIC_ACQ_REL,
                                                                 __ATOMIC_CONSUME);
                        if ((bool)rc) {
                                rc = OK;
                        }
                } else {
                        rc = -EACCES;
                        n = o;
                        break;
                }
        } while (rc != OK);
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
void xwaop__xws8_t__or(__atomic xws8_t * a,
                       xws8_t v,
                       xws8_t * nv, xws8_t * ov)
{
        xws8_t o;

        o = __atomic_fetch_or(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o | v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws8_t__and(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t o;

        o = __atomic_fetch_and(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o & v;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws8_t__xor(__atomic xws8_t * a,
                        xws8_t v,
                        xws8_t * nv, xws8_t * ov)
{
        xws8_t o;

        o = __atomic_fetch_xor(a, v, __ATOMIC_ACQ_REL);
        if (nv) {
                *nv = o ^ v;
        }
        if (ov) {
                *ov = o;
        }
}

/******** ******** test and operation ******** ********/
__xwlib_code
xwer_t xwaop__xws8_t__tst_then_op(__atomic xws8_t * a,
                                  xwaop_tst_f tst, void * tst_args,
                                  xwaop_op_f op, void * op_args,
                                  xws8_t * nv, xws8_t * ov)
{
        xws8_t o;
        xws8_t n;
        xwer_t rc;

        do {
                o = __atomic_load_n(a, __ATOMIC_CONSUME);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = (xwer_t)__atomic_compare_exchange_n(
                                                a, &o, n,
                                                false,
                                                __ATOMIC_ACQ_REL,
                                                __ATOMIC_CONSUME);
                                } else {
                                        rc = OK;
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
                                rc = (xwer_t)__atomic_compare_exchange_n(
                                        a, &o, n,
                                        false,
                                        __ATOMIC_ACQ_REL,
                                        __ATOMIC_CONSUME);
                        } else {
                                rc = OK;
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

#endif /* #ifdef __GNUC__ */
