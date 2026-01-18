/**
 * @file
 * @brief XWOS移植实现层：SOC：位图原子操作
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
#include <xwos/lib/xwbop.h>
#include <xwcd/soc/arm64/v8a/arch_isa.h>

xwer_t xwbmpaop_t0i_then_s1i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);
        xwbmp_t ov;
        xwbmp_t nv;
        xwer_t rc;

        armv8a_prefetch_before_aop(*bmp);
        do {
                ov = armv8a_load_acquire_exclusively_64b(&bmp[i]);
                if (!(ov & m)) {
                        nv = ov | m;
                        rc = armv8a_store_release_exclusively_64b(&bmp[i], nv);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        return rc;
}

xwer_t xwbmpaop_t1i_then_c0i(atomic_xwbmp_t * bmp, xwsq_t idx)
{
        xwsq_t i = XWBOP_BMP(idx);
        xwbmp_t m = XWBOP_BMP_MASK(idx);
        xwbmp_t ov;
        xwbmp_t nv;
        xwer_t rc;

        armv8a_prefetch_before_aop(*bmp);
        do {
                ov = armv8a_load_acquire_exclusively_64b(&bmp[i]);
                if (ov & m) {
                        nv = ov & (~m);
                        rc = armv8a_store_release_exclusively_64b(&bmp[i], nv);
                } else {
                        rc = -EACCES;
                        break;
                }
        } while (rc);
        return rc;
}

xwssq_t xwbmpaop_fls_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t ov;
        xwbmp_t nv;
        xwbmp_t m;
        xwssq_t pos;
        xwer_t rc;

        armv8a_prefetch_before_aop(*bmp);
        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = ~(xwbmp_t)0;
                }
                do {
                        i--;
                        ov = armv8a_load_acquire_exclusively_64b(&bmp[i]) & msk;
                        if (ov) {
                                pos = xwbop_fls(xwbmp_t, ov);
                                break;
                        }
                        msk = ~(xwbmp_t)0;
                } while (i > 0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << (xwbmp_t)pos);
                        nv = ov & (~m);
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = armv8a_store_release_exclusively_64b(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

xwssq_t xwbmpaop_flz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp;
        xwbmp_t ov;
        xwbmp_t nv;
        xwbmp_t m;
        xwssq_t pos;
        xwer_t rc;

        armv8a_prefetch_before_aop(*bmp);
        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = ~(xwbmp_t)0;
                }
                do {
                        i--;
                        ov = armv8a_load_acquire_exclusively_64b(&bmp[i]);
                        tmp = (xwbmp_t)(~ov) & msk;
                        if (tmp) {
                                pos = xwbop_flz(xwbmp_t, tmp);
                                break;
                        }
                        msk = ~(xwbmp_t)0;
                } while (i > 0);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << (xwbmp_t)pos);
                        nv = ov | m;
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = armv8a_store_release_exclusively_64b(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

xwssq_t xwbmpaop_ffs_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t ov;
        xwbmp_t nv;
        xwbmp_t m;
        xwssq_t pos;
        xwer_t rc;

        armv8a_prefetch_before_aop(*bmp);
        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == total - 1) {
                                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = ~(xwbmp_t)0;
                                }
                        } else {
                                msk = ~(xwbmp_t)0;
                        }
                        ov = armv8a_load_acquire_exclusively_64b(&bmp[i]) & msk;
                        if (ov) {
                                pos = xwbop_ffs(xwbmp_t, ov);
                                break;
                        }
                        i++;
                } while (i < total);
                if (pos < 0) {
                        pos = -ENODATA;
                        break;
                } else {
                        m = ((xwbmp_t)1 << (xwbmp_t)pos);
                        nv = ov & (xwbmp_t)(~m);
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = armv8a_store_release_exclusively_64b(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}

xwssq_t xwbmpaop_ffz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp;
        xwbmp_t ov;
        xwbmp_t nv;
        xwbmp_t m;
        xwssq_t pos;
        xwer_t rc;

        armv8a_prefetch_before_aop(*bmp);
        do {
                i = 0;
                pos = -ENODATA;
                do {
                        if (i == (total - 1)) {
                                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) -
                                       (xwbmp_t)1);
                                if ((xwbmp_t)0 == msk) {
                                        msk = ~(xwbmp_t)0;
                                }
                        } else {
                                msk = ~(xwbmp_t)0;
                        }
                        ov = armv8a_load_acquire_exclusively_64b(&bmp[i]);
                        tmp = (~ov) & msk;
                        if (tmp) {
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
                        nv = ov | m;
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        rc = armv8a_store_release_exclusively_64b(&bmp[i], nv);
                }
        } while (rc);
        return pos;
}
