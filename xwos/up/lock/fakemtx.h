/**
 * @file
 * @brief XWOS UP内核锁机制：虚假互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_lock_fakemtx_h__
#define __xwos_up_lock_fakemtx_h__

#include <xwos/standard.h>
#include <xwos/up/thd.h>
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
#  include <xwos/up/sync/rtsem.h>
#  define XWUP_SEM_API(api, ...) xwup_rtsem_##api(__VA_ARGS__)
#  define xwup_sem xwup_rtsem
#elif defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
#  include <xwos/up/sync/plsem.h>
#  define XWUP_SEM_API(api, ...) xwup_plsem_##api(__VA_ARGS__)
#  define xwup_sem xwup_plsem
#else
#  error "Can't find the configuration of semaphore!"
#endif

/**
 * @brief 虚假互斥锁对象
 */
struct xwup_mtx {
        struct xwup_sem fake;
};

static __xwup_inline_api
xwer_t xwup_mtx_init(struct xwup_mtx * mtx, xwpr_t sprio)
{
        XWOS_UNUSED(sprio);
        return XWUP_SEM_API(init, &mtx->fake, 1, 1);
}

static __xwup_inline_api
xwer_t xwup_mtx_fini(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(fini, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_grab(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(grab, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_put(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(put, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_create(struct xwup_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwup_sem * sem;
        xwer_t rc;

        XWOS_UNUSED(sprio);
        rc = XWUP_SEM_API(create, &sem, 1, 1);
        if (XWOK == rc) {
                *ptrbuf = (void *)sem;
        }
        return rc;
}

static __xwup_inline_api
xwer_t xwup_mtx_delete(struct xwup_mtx * mtx, xwsq_t tik)
{
        return XWUP_SEM_API(delete, &mtx->fake, tik);
}

static __xwup_inline_api
xwer_t xwup_mtx_acquire(struct xwup_mtx * mtx, xwsq_t tik)
{
        return XWUP_SEM_API(acquire, &mtx->fake, tik);
}

static __xwup_inline_api
xwer_t xwup_mtx_release(struct xwup_mtx * mtx, xwsq_t tik)
{
        return XWUP_SEM_API(release, &mtx->fake, tik);
}

static __xwup_inline_api
xwsq_t xwup_mtx_get_tik(struct xwup_mtx * mtx)
{
        return (NULL != mtx) ? mtx->fake.vsem.synobj.xwobj.tik : (xwsq_t)0;
}

static __xwup_inline_api
xwer_t xwup_mtx_unlock(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(post, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_lock(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(wait, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_lock_to(struct xwup_mtx * mtx, xwtm_t to)
{
        return XWUP_SEM_API(wait_to, &mtx->fake, to);
}

static __xwup_inline_api
xwer_t xwup_mtx_lock_unintr(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(wait_unintr, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_trylock(struct xwup_mtx * mtx)
{
        xwer_t rc;
        rc = XWUP_SEM_API(trywait, &mtx->fake);
        if (rc == -ENODATA) {
                rc = -EWOULDBLOCK;
        }
        return rc;
}

static __xwup_inline_api
xwer_t xwup_mtx_get_lkst(struct xwup_mtx * mtx, xwsq_t * lkst)
{
        xwer_t rc;
        xwssq_t val;

        val = (xwssq_t)0;
        rc = XWUP_SEM_API(get_value, &mtx->fake, &val);
        if (val) {
                *lkst = (xwsq_t)XWOS_LKST_LOCKED;
        } else {
                *lkst = (xwsq_t)XWOS_LKST_UNLOCKED;
        }
        return rc;

}

#endif /* xwos/up/lock/fakemtx.h */
