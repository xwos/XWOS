/**
 * @file
 * @brief XWRUST FFI：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/lock/mtx.h>

xwer_t xwrustffi_mtx_init(struct xwos_mtx * mtx)
{
        return xwos_mtx_init(mtx, XWOS_SKD_PRIORITY_RT_MAX);
}

xwer_t xwrustffi_mtx_fini(struct xwos_mtx * mtx)
{
        return xwos_mtx_fini(mtx);
}

xwer_t xwrustffi_mtx_grab(struct xwos_mtx * mtx)
{
        return xwos_mtx_grab(mtx);
}

xwer_t xwrustffi_mtx_put(struct xwos_mtx * mtx)
{
        return xwos_mtx_put(mtx);
}

xwsq_t xwrustffi_mtx_get_tik(struct xwos_mtx * mtx)
{
        return xwos_mtx_get_tik(mtx);
}

xwer_t xwrustffi_mtx_acquire(struct xwos_mtx * mtx, xwsq_t tik)
{
        xwos_mtx_d mtxd = {
                .mtx = mtx,
                .tik = tik,
        };
        return xwos_mtx_acquire(mtxd);
}

xwer_t xwrustffi_mtx_release(struct xwos_mtx * mtx, xwsq_t tik)
{
        xwos_mtx_d mtxd = {
                .mtx = mtx,
                .tik = tik,
        };
        return xwos_mtx_release(mtxd);
}

xwer_t xwrustffi_mtx_unlock(struct xwos_mtx * mtx)
{
        return xwos_mtx_unlock(mtx);
}

xwer_t xwrustffi_mtx_lock(struct xwos_mtx * mtx)
{
        return xwos_mtx_lock(mtx);
}

xwer_t xwrustffi_mtx_trylock(struct xwos_mtx * mtx)
{
        return xwos_mtx_trylock(mtx);
}

xwer_t xwrustffi_mtx_lock_to(struct xwos_mtx * mtx, xwtm_t to)
{
        return xwos_mtx_lock_to(mtx, to);
}

xwer_t xwrustffi_mtx_lock_unintr(struct xwos_mtx * mtx)
{
        return xwos_mtx_lock_unintr(mtx);
}

xwer_t xwrustffi_mtx_get_status(struct xwos_mtx * mtx, xwsq_t * lkst)
{
        return xwos_mtx_get_status(mtx, lkst);
}
