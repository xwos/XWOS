/**
 * @file
 * @brief 玄武OS UP内核锁机制：虚假互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 单核(UP)系统中并不需要自旋锁，为了满足xwosal中API的风格，
 *   源码中实现的自旋锁是虚假的。
 */

#ifndef __xwos_up_lock_fakemtx_h__
#define __xwos_up_lock_fakemtx_h__

#include <xwos/standard.h>
#include <xwos/up/thd.h>
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
#  include <xwos/up/sync/rtsem.h>
#elif defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
#  include <xwos/up/sync/plsem.h>
#else
#  error "Can't find the configuration of semaphore!"
#endif

#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
#  define XWUP_SEM_API(api, ...) xwup_rtsem_##api(__VA_ARGS__)
#  define xwup_sem xwup_rtsem
#elif defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
#  define XWUP_SEM_API(api, ...) xwup_plsem_##api(__VA_ARGS__)
#  define xwup_sem xwup_plsem
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
xwer_t xwup_mtx_create(struct xwup_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwup_sem * sem;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        XWOS_UNUSED(sprio);
        rc = XWUP_SEM_API(create, &sem, 1, 1);
        if (XWOK == rc) {
                *ptrbuf = (void *)sem;
        }
        return rc;
}

static __xwup_inline_api
xwer_t xwup_mtx_delete(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(delete, (struct xwup_sem *)mtx);
}

static __xwup_inline_api
xwer_t xwup_mtx_intr(struct xwup_mtx * mtx, struct xwup_thd * thd)
{
        return XWUP_SEM_API(intr, &mtx->fake, &thd->wqn);
}

static __xwup_inline_api
xwer_t xwup_mtx_unlock(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(post, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_trylock(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(trywait, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_lock(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(wait, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_timedlock(struct xwup_mtx * mtx, xwtm_t * xwtm)
{
        return XWUP_SEM_API(timedwait, &mtx->fake, xwtm);
}

static __xwup_inline_api
xwer_t xwup_mtx_lock_unintr(struct xwup_mtx * mtx)
{
        return XWUP_SEM_API(wait_unintr, &mtx->fake);
}

static __xwup_inline_api
xwer_t xwup_mtx_getlkst(struct xwup_mtx * mtx, xwsq_t * lkst)
{
        xwer_t rc;
        xwssq_t val;

        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);

        val = 0;
        rc = XWUP_SEM_API(getvalue, &mtx->fake, &val);
        if (val) {
                *lkst = XWOS_LKST_LOCKED;
        } else {
                *lkst = XWOS_LKST_UNLOCKED;
        }
        return rc;

}

#endif /* xwos/up/lock/fakemtx.h */
