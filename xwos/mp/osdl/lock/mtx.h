/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
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
xwer_t xwosdl_mtx_fini(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_fini(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_create(struct xwosdl_mtx ** mtxbuf, xwpr_t sprio)
{
        return xwmp_mtx_create(mtxbuf, sprio);
}

static __xwcc_inline
xwer_t xwosdl_mtx_delete(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_delete(mtx);
}

static __xwcc_inline
xwsq_t xwosdl_mtx_gettik(struct xwosdl_mtx * mtx)
{
        xwsq_t tik;

        if (mtx) {
                tik = mtx->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

static __xwcc_inline
xwer_t xwosdl_mtx_acquire(struct xwosdl_mtx * mtx, xwsq_t tik)
{
        return xwmp_mtx_acquire(mtx, tik);
}

static __xwcc_inline
xwer_t xwosdl_mtx_release(struct xwosdl_mtx * mtx, xwsq_t tik)
{
        return xwmp_mtx_release(mtx, tik);
}

static __xwcc_inline
xwer_t xwosdl_mtx_grab(struct xwosdl_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwmp_mtx_grab(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_put(struct xwosdl_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwmp_mtx_put(mtx);
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
xwer_t xwosdl_mtx_lock_to(struct xwosdl_mtx * mtx, xwtm_t to)
{
        return xwmp_mtx_lock_to(mtx, to);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock_unintr(struct xwosdl_mtx * mtx)
{
        return xwmp_mtx_lock_unintr(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_getlkst(struct xwosdl_mtx * mtx, xwsq_t * lkst)
{
        return xwmp_mtx_getlkst(mtx, lkst);
}

#endif /* xwos/mp/osdl/lock/mtx.h */
