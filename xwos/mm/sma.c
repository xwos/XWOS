/**
 * @file
 * @brief XuanWuOS的内存管理机制：简单内存分配器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/sma.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwmm_sma_init(struct xwmm_sma * sa,
                     xwptr_t origin, xwsz_t size, xwsq_t pos,
                     const char * name)
{
        xwer_t rc;

        origin = ALIGN(origin, XWMM_ALIGNMENT);
        size = ROUND(size, XWMM_ALIGNMENT);
        pos = ALIGN(pos, XWMM_ALIGNMENT);
        if (size < pos) {
                rc = -EINVAL;
        } else {
                rc = OK;
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
        xwsq_t cpos, npos;
        xwsq_t tmp;
        xwer_t rc;

        XWOS_VALIDATE((sa), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);

        if (__unlikely(0 == size)) {
                rc = -EINVAL;
                *membuf = NULL;
                goto err_inval;
        }

        if (aligned & XWMM_ALIGNMENT_MASK) {
                rc = -EINVAL;
                *membuf = NULL;
                goto err_inval;
        }

        if (0 == aligned) {
                aligned = XWMM_ALIGNMENT;
        }/* else {} */

        size = ALIGN(size, XWMM_ALIGNMENT);
        do {
                cpos = xwaop_load(xwsq_t, &sa->pos, xwmb_modr_acquire);
                tmp = ALIGN((sa->zone.origin + cpos), aligned);
                npos = size + tmp - sa->zone.origin;
                if (npos > sa->zone.size) {
                        rc = -ENOMEM;
                        *membuf = NULL;
                        goto err_nomem;
                }
                *membuf = (void *)tmp;
        } while (xwaop_teq_then_write(xwsq_t, &sa->pos, cpos, npos, NULL));
        return OK;

err_nomem:
err_inval:
        return rc;
}

__xwos_api
xwer_t xwmm_sma_free(struct xwmm_sma * sa, void * mem)
{
        XWOS_UNUSED(sa);
        XWOS_UNUSED(mem);
        return OK;
}
