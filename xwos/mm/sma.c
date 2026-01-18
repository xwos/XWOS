/**
 * @file
 * @brief XWOS内存管理：简单内存分配器
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
#include <xwos/mm/common.h>
#include <xwos/mm/sma.h>

__xwos_api
xwer_t xwmm_sma_init(struct xwmm_sma * sa,
                     xwptr_t origin, xwsz_t size, xwsq_t pos,
                     const char * name)
{
        xwer_t rc;

        // cppcheck-suppress [misra-c2012-17.8]
        pos = XWBOP_ALIGN(pos, (xwsz_t)XWMM_ALIGNMENT);
        if (size < pos) {
                rc = -EINVAL;
        } else {
                rc = XWOK;
                sa->zone.origin = origin;
                sa->zone.size = size;
                sa->pos = pos;
                sa->name = name;
        }
        return rc;
}

__xwos_api
xwer_t xwmm_sma_alloc(struct xwmm_sma * sa, xwsz_t size, xwsz_t aligned,
                      void ** membuf)
{
        xwsq_t cpos;
        xwsq_t npos;
        xwsq_t tmp;
        xwer_t rc;

        XWOS_VALIDATE((sa), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        if ((xwsz_t)0 == size) {
                rc = -EINVAL;
                *membuf = NULL;
                goto err_inval;
        }

        if ((xwsz_t)0 != (aligned & XWMM_UNALIGNED_MASK)) {
                rc = -EINVAL;
                *membuf = NULL;
                goto err_inval;
        }

        if ((xwsz_t)0 == aligned) {
                aligned = XWMM_ALIGNMENT; // cppcheck-suppress [misra-c2012-17.8]
        }

        // cppcheck-suppress [misra-c2012-17.8]
        size = XWBOP_ALIGN(size, (xwsz_t)XWMM_ALIGNMENT);
        do {
                cpos = xwaop_load(xwsq_t, &sa->pos, xwaop_mo_acquire);
                tmp = XWBOP_ALIGN((sa->zone.origin + cpos), aligned);
                npos = size + tmp - sa->zone.origin;
                if (npos > sa->zone.size) {
                        rc = -ENOMEM;
                        *membuf = NULL;
                        goto err_nomem;
                }
                *membuf = (void *)tmp;
        } while (XWOK != xwaop_teq_then_write(xwsq_t, &sa->pos, cpos, npos, NULL));
        return XWOK;

err_nomem:
err_inval:
        return rc;
}

__xwos_api
xwer_t xwmm_sma_free(struct xwmm_sma * sa, void * mem)
{
        XWOS_UNUSED(sa);
        XWOS_UNUSED(mem);
        return XWOK;
}
