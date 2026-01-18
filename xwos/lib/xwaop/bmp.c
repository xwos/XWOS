/**
 * @file
 * @brief XWOS通用库：位图原子操作
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
bool xwbmpaop_t1i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        atomic_xwbmp_t * pos;
        bool ret;
        xwbmp_t value;

        pos = &bmp[XWBOP_BMP(idx)];
        value = xwaop_load(xwbmp_t, pos, xwaop_mo_consume);
        ret = (bool)(!!(value & XWBOP_BMP_MASK(idx)));
        return ret;
}

__xwlib_code
void xwbmpaop_s1i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);

        xwaop_s1m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

__xwlib_code
void xwbmpaop_c0i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);

        xwaop_c0m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

__xwlib_code
void xwbmpaop_x1i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);

        xwaop_x1m(xwbmp_t, &bmp[i], m, NULL, NULL);
}

__xwlib_code
xwer_t xwbmpaop_t0i_then_s1i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);
        xwbmp_t o;
        xwbmp_t n;
        xwer_t rc;

        do {
                o = xwaop_load(xwbmp_t, &bmp[i], xwaop_mo_consume);
                if (!(o & m)) {
                        n = o | m;
                        rc = XWOK;
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (XWOK != xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return rc;
}

__xwlib_code
xwer_t xwbmpaop_t1i_then_c0i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);
        xwbmp_t o;
        xwbmp_t n;
        xwer_t rc;

        do {
                o = xwaop_load(xwbmp_t, &bmp[i], xwaop_mo_consume);
                if ((xwbmp_t)0 != (o & m)) {
                        n = o & (xwbmp_t)(~m);
                        rc = XWOK;
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (XWOK != xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return rc;
}

__xwlib_code
xwssq_t xwbmpaop_fls_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t o;
        xwbmp_t n;
        xwbmp_t m;
        xwssq_t pos;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                }
                do {
                        i--;
                        o = xwaop_load(xwbmp_t, &bmp[i], xwaop_mo_consume);
                        o &= msk;
                        if ((xwbmp_t)0 != o) {
                                pos = xwbop_fls(xwbmp_t, o);
                                break;
                        }
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                } while (i > (xwsq_t)0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << (xwbmp_t)pos);
                        n = o & (xwbmp_t)(~m);
                        pos += (xwssq_t)i * (xwssq_t)BITS_PER_XWBMP_T;
                }
        } while (XWOK != xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}

__xwlib_code
xwssq_t xwbmpaop_flz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp;
        xwbmp_t o;
        xwbmp_t n;
        xwbmp_t m;
        xwssq_t pos;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                }
                do {
                        i--;
                        o = xwaop_load(xwbmp_t, &bmp[i], xwaop_mo_consume);
                        tmp = (xwbmp_t)(~o) & msk;
                        if ((xwbmp_t)0 != tmp) {
                                pos = xwbop_fls(xwbmp_t, tmp);
                                break;
                        }
                        msk = (xwbmp_t)(~(xwbmp_t)0);
                } while (i > (xwsq_t)0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << (xwbmp_t)pos);
                        n = o | m;
                        pos += (xwssq_t)i * (xwssq_t)BITS_PER_XWBMP_T;
                }
        } while (XWOK != xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}

__xwlib_code
xwssq_t xwbmpaop_ffs_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t o;
        xwbmp_t n;
        xwbmp_t m;
        xwssq_t pos;

        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == (total - (xwsz_t)1)) {
                                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = (xwbmp_t)(~(xwbmp_t)0);
                                }
                        } else {
                                msk = (xwbmp_t)(~(xwbmp_t)0);
                        }
                        o = xwaop_load(xwbmp_t, &bmp[i], xwaop_mo_consume);
                        o &= msk;
                        if ((xwbmp_t)0 != o) {
                                pos = xwbop_ffs(xwbmp_t, o);
                                break;
                        }
                        i++;
                } while (i < total);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << (xwbmp_t)pos);
                        n = o & (xwbmp_t)(~m);
                        pos += (xwssq_t)i * (xwssq_t)BITS_PER_XWBMP_T;
                }
        } while (XWOK != xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}

__xwlib_code
xwssq_t xwbmpaop_ffz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp;
        xwbmp_t o;
        xwbmp_t n;
        xwbmp_t m;
        xwssq_t pos;

        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == (total - (xwsz_t)1)) {
                                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = (xwbmp_t)(~(xwbmp_t)0);
                                }
                        } else {
                                msk = (xwbmp_t)(~(xwbmp_t)0);
                        }
                        o = xwaop_load(xwbmp_t, &bmp[i], xwaop_mo_consume);
                        tmp = (xwbmp_t)(~o) & msk;
                        if ((xwbmp_t)0 != tmp) {
                                pos = xwbop_ffs(xwbmp_t, tmp);
                                break;
                        }
                        i++;
                } while (i < total);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << (xwbmp_t)pos);
                        n = o | m;
                        pos += (xwssq_t)i * (xwssq_t)BITS_PER_XWBMP_T;
                }
        } while (XWOK != xwaop_teq_then_write(xwbmp_t, &bmp[i], o, n, NULL));
        return pos;
}
