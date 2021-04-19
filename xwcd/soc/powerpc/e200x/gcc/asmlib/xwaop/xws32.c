/**
 * @file
 * @brief 原子操作汇编库: xws32_t
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include <xwos/standard.h>
#include <e200x_core.h>

__xwlib_code
xws32_t xwaop__xws32__load(xws32_a * a,
                           const enum xwmb_memory_order_em mo)
{
        xws32_t v;

        switch (mo) {
        case xwmb_modr_relaxed:
                v = *a;
                break;
        case xwmb_modr_consume:
                v = *a;
                xwmb_mp_ddb();
                break;
        case xwmb_modr_acquire:
                v = *a;
                xwmb_mp_mb();
                break;
        case xwmb_modr_release:
                v = *a;
                break;
        case xwmb_modr_acq_rel:
                v = *a;
                xwmb_mp_mb();
                break;
        case xwmb_modr_seq_cst:
                xwmb_mp_mb();
                v = *a;
                xwmb_mp_mb();
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

__xwlib_code
xws32_t xwaop__xws32__store(xws32_a * a,
                            const enum xwmb_memory_order_em mo,
                            xws32_t v)
{
        switch (mo) {
        case xwmb_modr_relaxed:
                *a = v;
                break;
        case xwmb_modr_consume:
                *a = v;
                break;
        case xwmb_modr_acquire:
                *a = v;
                break;
        case xwmb_modr_release:
                xwmb_mp_mb();
                *a = v;
                break;
        case xwmb_modr_acq_rel:
                xwmb_mp_mb();
                *a = v;
                break;
        case xwmb_modr_seq_cst:
                xwmb_mp_mb();
                *a = v;
                xwmb_mp_mb();
                break;
        default:
                *a = v;
                break;
        }
        return v;
}

__xwlib_code
void xwaop__xws32__read(xws32_a * a,
                        xws32_t * ov)
{
        xws32_t o;
        o = xwaop__xws32__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws32__write(xws32_a * a,
                         xws32_t v,
                         xws32_t * ov)
{
        xws32_t o;

        do {
                o = (xws32_t)lwarx(a);
                xwmb_mp_mb();
        } while (stwcx(a, (xwu32_t)v));
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o == t) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tne_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o != t) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tgt_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o > t) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tge_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o >= t) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tlt_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o < t) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tle_then_write(xws32_a * a,
                                    xws32_t t,
                                    xws32_t v,
                                    xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o <= t) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tgtlt_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tgelt_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (o < r)) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tgtle_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xws32__tgele_then_write(xws32_a * a,
                                      xws32_t l, xws32_t r,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_mp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
void xwaop__xws32__add(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)lwarx(a);
                n = o + v;
                xwmb_mp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o == t) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tne_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o != t) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgt_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o > t) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tge_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o >= t) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tlt_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o < t) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tle_then_add(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o <= t) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgtlt_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgelt_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgtle_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgele_then_add(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
void xwaop__xws32__sub(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)lwarx(a);
                n = o - v;
                xwmb_mp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o == t) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tne_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o != t) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgt_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o > t) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tge_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o >= t) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tlt_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o < t) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tle_then_sub(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o <= t) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgtlt_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgelt_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgtle_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgele_then_sub(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
void xwaop__xws32__rsb(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)lwarx(a);
                n = v - o;
                xwmb_mp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__teq_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o == t) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tne_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o != t) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgt_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o > t) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tge_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o != t) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tlt_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o < t) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tle_then_rsb(xws32_a * a,
                                  xws32_t t,
                                  xws32_t v,
                                  xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (o <= t) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgtlt_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgelt_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgtle_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
xwer_t xwaop__xws32__tgele_then_rsb(xws32_a * a,
                                    xws32_t l, xws32_t r,
                                    xws32_t v,
                                    xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        xwmb_mp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_mp_ddb();
                        break;
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

__xwlib_code
void xwaop__xws32__or(xws32_a * a,
                      xws32_t v,
                      xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)lwarx(a);
                n = o | v;
                xwmb_mp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xws32__and(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)lwarx(a);
                n = o & v;
                xwmb_mp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        };
}

__xwlib_code
void xwaop__xws32__xor(xws32_a * a,
                       xws32_t v,
                       xws32_t * nv, xws32_t * ov)
{
        xws32_t n;
        xws32_t o;

        do {
                o = (xws32_t)lwarx(a);
                n = o ^ v;
                xwmb_mp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xws32__tst_then_op(xws32_a * a,
                                 xwaop_tst_f tst, void * tst_args,
                                 xwaop_op_f op, void * op_args,
                                 xws32_t * nv, xws32_t * ov)
{
        xws32_t o;
        xws32_t n;
        xwer_t rc;

        do {
                o = (xws32_t)lwarx(a);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        xwmb_mp_mb();
                                        rc = stwcx(a, (xwu32_t)n);
                                } else {
                                        rc = XWOK;
                                        n = o;
                                        xwmb_mp_mb();
                                        break;
                                }
                        } else {
                                rc = -EACCES;
                                n = o;
                                xwmb_mp_ddb();
                                break;
                        }
                } else {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                xwmb_mp_mb();
                                rc = stwcx(a, (xwu32_t)n);
                        } else {
                                rc = XWOK;
                                n = o;
                                xwmb_mp_mb();
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
