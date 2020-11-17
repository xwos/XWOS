/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_lock_mtx_h__
#define __xwos_mp_osdl_lock_mtx_h__

#include <xwos/mp/lock/mtx.h>

#define xwosdl_mtx xwmp_mtx

static __xwcc_inline
xwer_t xwosdl_mtx_init(struct xwosdl_mtx * mtx, xwpr_t sprio)
{
        return xwmp_mtx_init(mtx, sprio);
}

static __xwcc_inline
xwer_t xwosdl_mtx_destroy(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_destroy(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_create(struct xwosdl_mtx ** mtxp, xwpr_t sprio)
{
        struct xwosdl_mtx * mtx;
        xwer_t rc;

        rc = xwmp_mtx_create(&mtx, sprio);
        if (XWOK == rc) {
                *mtxp = (struct xwosdl_mtx *)mtx;
        } else {
                *mtxp = NULL;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_mtx_delete(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_delete(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_unlock(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_unlock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_lock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_trylock(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_trylock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_timedlock(struct xwosdl_mtx * mtx, xwtm_t * xwtm)
{
        return xwmp_mtx_timedlock(mtx, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock_unintr(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_lock_unintr(mtx);
}

#endif /* xwos/mp/osdl/lock/mtx.h */
