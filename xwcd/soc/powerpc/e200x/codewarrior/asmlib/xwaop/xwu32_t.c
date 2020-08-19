/**
 * @file
 * @brief 原子操作汇编库: xwu32_t
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <e200x_core.h>
#include <xwos/lib/xwaop32.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwlib_code
xwu32_t xwaop__xwu32_t__load(__xwcc_atomic xwu32_t * a,
                             const enum xwmb_memory_order_em mo)
{
        xwu32_t v;

        switch (mo) {
        case xwmb_modr_relaxed:
                v = *a;
                break;
        case xwmb_modr_consume:
                v = *a;
                xwmb_smp_ddb();
                break;
        case xwmb_modr_acquire:
                v = *a;
                xwmb_smp_mb();
                break;
        case xwmb_modr_release:
                v = *a;
                break;
        case xwmb_modr_acq_rel:
                v = *a;
                xwmb_smp_mb();
                break;
        case xwmb_modr_seq_cst:
                xwmb_smp_mb();
                v = *a;
                xwmb_smp_mb();
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

__xwlib_code
xwu32_t xwaop__xwu32_t__store(__xwcc_atomic xwu32_t * a,
                              const enum xwmb_memory_order_em mo,
                              xwu32_t v)
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
                xwmb_smp_mb();
                *a = v;
                break;
        case xwmb_modr_acq_rel:
                xwmb_smp_mb();
                *a = v;
                break;
        case xwmb_modr_seq_cst:
                xwmb_smp_mb();
                *a = v;
                xwmb_smp_mb();
                break;
        default:
                *a = v;
                break;
        }
        return v;
}

__xwlib_code
void xwaop__xwu32_t__read(__xwcc_atomic xwu32_t * a,
                          xwu32_t * ov)
{
        xwu32_t o;
        o = xwaop__xwu32_t__load(a, xwmb_modr_acquire);
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu32_t__write(__xwcc_atomic xwu32_t * a,
                           xwu32_t v,
                           xwu32_t * ov)
{
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)v));
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o == t) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tne_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o != t) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tgt_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o > t) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tge_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o >= t) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tlt_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o < t) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tle_then_write(__xwcc_atomic xwu32_t * a,
                                      xwu32_t t,
                                      xwu32_t v,
                                      xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o <= t) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tgtlt_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tgelt_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (o < r)) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tgtle_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
xwer_t xwaop__xwu32_t__tgele_then_write(__xwcc_atomic xwu32_t * a,
                                        xwu32_t l, xwu32_t r,
                                        xwu32_t v,
                                        xwu32_t * ov)
{
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_smp_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

__xwlib_code
void xwaop__xwu32_t__add(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                n = o + v;
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o == t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tne_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o != t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgt_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o > t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tge_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o >= t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tlt_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o < t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tle_then_add(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o <= t) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgtlt_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgelt_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgtle_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgele_then_add(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
void xwaop__xwu32_t__sub(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                n = o - v;
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o == t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tne_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o != t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgt_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o > t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tge_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o >= t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tlt_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o < t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tle_then_sub(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o <= t) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgtlt_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgelt_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgtle_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgele_then_sub(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
void xwaop__xwu32_t__rsb(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                n = v - o;
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu32_t__teq_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o == t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tne_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o != t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgt_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o > t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tge_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o != t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tlt_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o < t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tle_then_rsb(__xwcc_atomic xwu32_t * a,
                                    xwu32_t t,
                                    xwu32_t v,
                                    xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o <= t) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgtlt_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgelt_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgtle_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tgele_then_rsb(__xwcc_atomic xwu32_t * a,
                                      xwu32_t l, xwu32_t r,
                                      xwu32_t v,
                                      xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
void xwaop__xwu32_t__or(__xwcc_atomic xwu32_t * a,
                        xwu32_t v,
                        xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                n = o | v;
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu32_t__and(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                n = o & v;
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu32_t__xor(__xwcc_atomic xwu32_t * a,
                         xwu32_t v,
                         xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                n = o ^ v;
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
void xwaop__xwu32_t__c0m(__xwcc_atomic xwu32_t * a,
                         xwu32_t m,
                         xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;

        do {
                o = (xwu32_t)lwarx(a);
                n = o & (~m);
                xwmb_smp_mb();
        } while (stwcx(a, (xwu32_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

__xwlib_code
xwer_t xwaop__xwu32_t__t1ma_then_c0m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o & m) == m) {
                        n = o & (~m);
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__t1mo_then_c0m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (o & m) {
                        n = o & (~m);
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__t0ma_then_s1m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (!(o & m)) {
                        n = o | m;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__t0mo_then_s1m(__xwcc_atomic xwu32_t * a,
                                     xwu32_t m,
                                     xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if ((o & m) != m) {
                        n = o | m;
                        xwmb_smp_mb();
                        rc = stwcx(a, (xwu32_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
                        xwmb_smp_ddb();
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
xwer_t xwaop__xwu32_t__tst_then_op(__xwcc_atomic xwu32_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t o;
        xwu32_t n;
        xwer_t rc;

        do {
                o = (xwu32_t)lwarx(a);
                if (tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        xwmb_smp_mb();
                                        rc = stwcx(a, (xwu32_t)n);
                                } else {
                                        rc = XWOK;
                                        n = o;
                                        xwmb_smp_mb();
                                        break;
                                }
                        } else {
                                rc = -EACCES;
                                n = o;
                                xwmb_smp_ddb();
                                break;
                        }
                } else {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                xwmb_smp_mb();
                                rc = stwcx(a, (xwu32_t)n);
                        } else {
                                rc = XWOK;
                                n = o;
                                xwmb_smp_mb();
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
