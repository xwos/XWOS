/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_lock_mtx_h__
#define __xwos_up_osdl_lock_mtx_h__

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
#  include <xwos/up/lock/mtx.h>
#elif defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)
#  include <xwos/up/lock/fakemtx.h>
#else
#  error "Can't find the mtx configuration!"
#endif

#define xwosdl_mtx xwup_mtx

typedef struct {
        struct xwosdl_mtx * mtx;
        xwsq_t tik;
} xwosdl_mtx_d;

#define XWOSDL_MTX_NILD ((xwosdl_mtx_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_mtx_init(struct xwosdl_mtx * mtx, xwpr_t sprio)
{
        return xwup_mtx_init(mtx, sprio);
}

static __xwcc_inline
xwer_t xwosdl_mtx_fini(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_fini(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_grab(struct xwosdl_mtx * mtx)
{
        XWOS_UNUSED(mtx);
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return XWOK;
}

static __xwcc_inline
xwer_t xwosdl_mtx_put(struct xwosdl_mtx * mtx)
{
        XWOS_UNUSED(mtx);
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return XWOK;
}

xwer_t xwosdl_mtx_create(xwosdl_mtx_d * mtxd, xwpr_t sprio);

static __xwcc_inline
xwer_t xwosdl_mtx_delete(struct xwosdl_mtx * mtx, xwsq_t tik)
{
        XWOS_UNUSED(tik);
        return xwup_mtx_delete(mtx);
}

xwer_t xwosdl_mtx_acquire(struct xwosdl_mtx * mtx, xwsq_t tik);

xwer_t xwosdl_mtx_release(struct xwosdl_mtx * mtx, xwsq_t tik);

static __xwcc_inline
xwsq_t xwosdl_mtx_gettik(struct xwosdl_mtx * mtx)
{
        XWOS_UNUSED(mtx);
        return 1;
}

static __xwcc_inline
xwer_t xwosdl_mtx_unlock(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_unlock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_lock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_trylock(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_trylock(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock_to(struct xwosdl_mtx * mtx, xwtm_t to)
{
        return xwup_mtx_lock_to(mtx, to);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock_unintr(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_lock_unintr(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_getlkst(struct xwosdl_mtx * mtx, xwsq_t * lkst)
{
        return xwup_mtx_getlkst(mtx, lkst);
}

#endif /* xwos/up/osdl/lock/mtx.h */
