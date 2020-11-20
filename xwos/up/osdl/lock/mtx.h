/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_lock_mtx_h__
#define __xwos_up_osdl_lock_mtx_h__

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
  #include <xwos/up/lock/mtx.h>
#elif defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)
  #include <xwos/up/lock/fakemtx.h>
#else
  #error "Can't find the mtx configuration!"
#endif

#define xwosdl_mtx xwup_mtx

static __xwcc_inline
xwer_t xwosdl_mtx_init(struct xwosdl_mtx * mtx, xwpr_t sprio)
{
        return xwup_mtx_init(mtx, sprio);
}

static __xwcc_inline
xwer_t xwosdl_mtx_destroy(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_destroy(mtx);
}

static __xwcc_inline
xwer_t xwosdl_mtx_create(struct xwosdl_mtx ** mtxp, xwpr_t sprio)
{
        xwer_t rc;

        if (NULL != mtxp) {
                *mtxp = NULL;
                rc = xwup_mtx_create(mtxp, sprio);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_mtx_delete(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_delete(mtx);
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
xwer_t xwosdl_mtx_timedlock(struct xwosdl_mtx * mtx, xwtm_t * xwtm)
{
        return xwup_mtx_timedlock(mtx, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_mtx_lock_unintr(struct xwosdl_mtx * mtx)
{
        return xwup_mtx_lock_unintr(mtx);
}

#endif /* xwos/up/osdl/lock/mtx.h */
