/**
 * @file
 * @brief 原子操作库: xwaop__xwu16_t
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#include <xwcd/soc/arm64/v8a/arch_isa.h>

xwu16_t xwaop__xwu16_t__load(atomic_xwu16_t * a,
                             const enum xwaop_memory_order_em mo)
{
        xwu16_t v;

        switch (mo) {
        case xwaop_mo_relaxed:
                v = *a;
                break;
        case xwaop_mo_consume:
                v = *a;
                break;
        case xwaop_mo_acquire:
                v = armv8a_load_acquire_16b(a);
                break;
        case xwaop_mo_release:
                v = *a;
                break;
        case xwaop_mo_acq_rel:
                v = armv8a_load_acquire_16b(a);
                break;
        case xwaop_mo_seq_cst:
                xwmb_mb();
                v = *a;
                xwmb_mb();
                break;
        default:
                v = *a;
                break;
        }
        return v;
}

xwu16_t xwaop__xwu16_t__store(atomic_xwu16_t * a,
                              const enum xwaop_memory_order_em mo,
                              xwu16_t v)
{
        switch (mo) {
        case xwaop_mo_relaxed:
                *a = v;
                break;
        case xwaop_mo_consume:
                *a = v;
                break;
        case xwaop_mo_acquire:
                *a = v;
                break;
        case xwaop_mo_release:
                armv8a_store_release_16b(a, v);
                break;
        case xwaop_mo_acq_rel:
                armv8a_store_release_16b(a, v);
                break;
        case xwaop_mo_seq_cst:
                xwmb_mb();
                *a = v;
                xwmb_mb();
                break;
        }
        return v;
}

void xwaop__xwu16_t__read(atomic_xwu16_t * a,
                          xwu16_t * ov)
{
        xwu16_t o;

        o = xwaop__xwu16_t__load(a, xwaop_mo_acquire);
        if (ov) {
                *ov = o;
        }
}

void xwaop__xwu16_t__write(atomic_xwu16_t * a,
                           xwu16_t v,
                           xwu16_t * ov)
{
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)v));
        if (ov) {
                *ov = o;
        }
}

xwer_t xwaop__xwu16_t__teq_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o == t) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tne_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o != t) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tgt_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o > t) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tge_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o >= t) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tlt_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o < t) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tle_then_write(atomic_xwu16_t * a,
                                      xwu16_t t,
                                      xwu16_t v,
                                      xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o <= t) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tgtlt_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o < r)) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tgelt_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (o < r)) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tgtle_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o <= r)) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

xwer_t xwaop__xwu16_t__tgele_then_write(atomic_xwu16_t * a,
                                        xwu16_t l, xwu16_t r,
                                        xwu16_t v,
                                        xwu16_t * ov)
{
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (o <= r)) {
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)v);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}

void xwaop__xwu16_t__add(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                n = o + v;
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

xwer_t xwaop__xwu16_t__teq_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o == t) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tne_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o != t) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgt_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o > t) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tge_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o >= t) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tlt_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o < t) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tle_then_add(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o <= t) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgtlt_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o < r)) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgelt_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (o < r)) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgtle_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o <= r)) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgele_then_add(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (o <= r)) {
                        n = o + v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

void xwaop__xwu16_t__sub(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                n = o - v;
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

xwer_t xwaop__xwu16_t__teq_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o == t) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tne_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o != t) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgt_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o > t) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tge_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o >= t) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tlt_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o < t) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tle_then_sub(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o <= t) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgtlt_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o < r)) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgelt_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (l < r)) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgtle_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o <= r)) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgele_then_sub(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (o <= r)) {
                        n = o - v;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

void xwaop__xwu16_t__rsb(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                n = v - o;
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

xwer_t xwaop__xwu16_t__teq_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o == t) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tne_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o != t) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgt_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o > t) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tge_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o != t) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tlt_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o < t) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tle_then_rsb(atomic_xwu16_t * a,
                                    xwu16_t t,
                                    xwu16_t v,
                                    xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o <= t) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgtlt_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o < r)) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgelt_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (o < r)) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgtle_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o > l) && (o <= r)) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tgele_then_rsb(atomic_xwu16_t * a,
                                      xwu16_t l, xwu16_t r,
                                      xwu16_t v,
                                      xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o >= l) && (o <= r)) {
                        n = v - o;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

void xwaop__xwu16_t__or(atomic_xwu16_t * a,
                        xwu16_t v,
                        xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                n = o | v;
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

void xwaop__xwu16_t__and(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                n = o & v;
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

void xwaop__xwu16_t__xor(atomic_xwu16_t * a,
                         xwu16_t v,
                         xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                n = o ^ v;
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

void xwaop__xwu16_t__c0m(atomic_xwu16_t * a,
                         xwu16_t m,
                         xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                n = o & (~m);
        } while (armv8a_store_release_exclusively_16b(a, (xwu16_t)n));
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
}

xwer_t xwaop__xwu16_t__t1ma_then_c0m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o & m) == m) {
                        n = o & (~m);
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__t1mo_then_c0m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (o & m) {
                        n = o & (~m);
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__t0ma_then_s1m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (!(o & m)) {
                        n = o | m;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__t0mo_then_s1m(atomic_xwu16_t * a,
                                     xwu16_t m,
                                     xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t n;
        xwu16_t o;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if ((o & m) != m) {
                        n = o | m;
                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                } else {
                        rc = -EACCES;
                        n = o;
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

xwer_t xwaop__xwu16_t__tst_then_op(atomic_xwu16_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu16_t * nv, xwu16_t * ov)
{
        xwu16_t o;
        xwu16_t n;
        xwer_t rc;

        armv8a_prefetch_before_aop(*a);
        do {
                o = (xwu16_t)armv8a_load_acquire_exclusively_16b(a);
                if (NULL != tst) {
                        if (tst((const void *)&o, tst_args)) {
                                if (NULL != op) {
                                        op(&n, (const void *)&o, op_args);
                                        rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                                } else {
                                        rc = 0;
                                        n = o;
                                        break;
                                }
                        } else {
                                rc = -EACCES;
                                n = o;
                                break;
                        }
                } else {
                        if (NULL != op) {
                                op(&n, (const void *)&o, op_args);
                                rc = armv8a_store_release_exclusively_16b(a, (xwu16_t)n);
                        } else {
                                rc = 0;
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
