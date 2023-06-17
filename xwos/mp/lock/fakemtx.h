/**
 * @file
 * @brief XWOS MP内核锁机制：虚假互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_lock_fakemtx_h__
#define __xwos_mp_lock_fakemtx_h__

#include <xwos/standard.h>
#include <xwos/mp/thd.h>
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
#  include <xwos/mp/sync/sem.h>
#  define XWMP_SEM_API(api, ...) xwmp_rtsem_##api(__VA_ARGS__)
#elif defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
#  include <xwos/mp/sync/sem.h>
#  define XWMP_SEM_API(api, ...) xwmp_plsem_##api(__VA_ARGS__)
#else
#  error "Can't find the configuration of semaphore!"
#endif

/**
 * @brief 虚假互斥锁对象
 */
struct xwmp_mtx {
        struct xwmp_sem fake;
};

static __xwmp_inline_api
xwer_t xwmp_mtx_init(struct xwmp_mtx * mtx, xwpr_t sprio)
{
        XWOS_UNUSED(sprio);
        return XWMP_SEM_API(init, &mtx->fake, 1, 1);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_fini(struct xwmp_mtx * mtx)
{
        return xwmp_sem_fini(&mtx->fake);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_grab(struct xwmp_mtx * mtx)
{
        return xwmp_sem_grab(&mtx->fake);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_put(struct xwmp_mtx * mtx)
{
        return xwmp_sem_put(&mtx->fake);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_create(struct xwmp_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwmp_sem * sem;
        xwer_t rc;

        XWOS_UNUSED(sprio);
        rc = XWMP_SEM_API(create, &sem, 1, 1);
        if (XWOK == rc) {
                *ptrbuf = (void *)sem;
        }
        return rc;
}

static __xwmp_inline_api
xwer_t xwmp_mtx_delete(struct xwmp_mtx * mtx, xwsq_t tik)
{
        return xwmp_sem_delete(&mtx->fake, tik);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_acquire(struct xwmp_mtx * mtx, xwsq_t tik)
{
        return xwmp_sem_acquire(&mtx->fake, tik);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_release(struct xwmp_mtx * mtx, xwsq_t tik)
{
        return xwmp_sem_release(&mtx->fake, tik);
}

static __xwmp_inline_api
xwsq_t xwmp_mtx_gettik(struct xwmp_mtx * mtx)
{
        return mtx ? mtx->fake.synobj.xwobj.tik : 0;
}

static __xwmp_inline_api
xwer_t xwmp_mtx_unlock(struct xwmp_mtx * mtx)
{
        return XWMP_SEM_API(post, &mtx->fake);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_lock(struct xwmp_mtx * mtx)
{
        return XWMP_SEM_API(wait, &mtx->fake);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_lock_to(struct xwmp_mtx * mtx, xwtm_t to)
{
        return XWMP_SEM_API(wait_to, &mtx->fake, to);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_lock_unintr(struct xwmp_mtx * mtx)
{
        return XWMP_SEM_API(wait_unintr, &mtx->fake);
}

static __xwmp_inline_api
xwer_t xwmp_mtx_trylock(struct xwmp_mtx * mtx)
{
        xwer_t rc;
        rc = XWMP_SEM_API(trywait, &mtx->fake);
        if (rc == -ENODATA) {
                rc = -EWOULDBLOCK;
        }
        return rc;
}

static __xwmp_inline_api
xwer_t xwmp_mtx_getlkst(struct xwmp_mtx * mtx, xwsq_t * lkst)
{
        xwer_t rc;
        xwssq_t val;

        val = 0;
        rc = xwmp_sem_getvalue(&mtx->fake, &val);
        if (val) {
                *lkst = XWOS_LKST_LOCKED;
        } else {
                *lkst = XWOS_LKST_UNLOCKED;
        }
        return rc;

}

#endif /* xwos/mp/lock/fakemtx.h */
