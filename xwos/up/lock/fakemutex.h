/**
 * @file
 * @brief XuanWuOS的锁机制：虚假互斥锁
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

#ifndef __xwos_up_lock_fakemutex_h__
#define __xwos_up_lock_fakemutex_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#if defined(XWUPCFG_SYNC_RTSMR) && (1 == XWUPCFG_SYNC_RTSMR)
#include <xwos/up/sync/rtsmr.h>
#elif defined(XWUPCFG_SYNC_PLSMR) && (1 == XWUPCFG_SYNC_PLSMR)
#include <xwos/up/sync/plsmr.h>
#else
  #error "Can't find the configuration of semaphore!"
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWUPCFG_SYNC_RTSMR) && (1 == XWUPCFG_SYNC_RTSMR)
  #define XWSDYNC_SMR_API(api, ...)             xwsync_rtsmr_##api(__VA_ARGS__)
  #define xwsync_smr xwsync_rtsmr
#elif defined(XWUPCFG_SYNC_PLSMR) && (1 == XWUPCFG_SYNC_PLSMR)
  #define XWSDYNC_SMR_API(api, ...)             xwsync_plsmr_##api(__VA_ARGS__)
  #define xwsync_smr xwsync_plsmr
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 虚假互斥锁对象
 */
struct xwlk_mtx {
        struct xwsync_smr fake;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_inline_api
xwer_t xwlk_mtx_init(struct xwlk_mtx * mtx, xwpr_t sprio)
{
        XWOS_UNUSED(sprio);
        return XWSDYNC_SMR_API(init, &mtx->fake, 1, 1);
}

static __xwos_inline_api
xwer_t xwlk_mtx_destroy(struct xwlk_mtx * mtx)
{
        return XWSDYNC_SMR_API(destroy, &mtx->fake);
}

static __xwos_inline_api
xwer_t xwlk_mtx_create(struct xwlk_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwsync_smr * smr;
        xwer_t rc;

        XWOS_UNUSED(sprio);
        rc = XWSDYNC_SMR_API(create, &smr, 1, 1);
        if (XWOK == rc) {
                *ptrbuf = (void *)smr;
        }
        return rc;
}

static __xwos_inline_api
xwer_t xwlk_mtx_delete(struct xwlk_mtx * mtx)
{
        return XWSDYNC_SMR_API(delete, (struct xwsync_smr *)mtx);
}

static __xwos_inline_api
xwer_t xwlk_mtx_intr(struct xwlk_mtx * mtx, struct xwos_tcb * tcb)
{
        return XWSDYNC_SMR_API(intr, &mtx->fake, &tcb->wqn);
}

static __xwos_inline_api
xwer_t xwlk_mtx_unlock(struct xwlk_mtx * mtx)
{
        return XWSDYNC_SMR_API(post, &mtx->fake);
}

static __xwos_inline_api
xwer_t xwlk_mtx_trylock(struct xwlk_mtx * mtx)
{
        return XWSDYNC_SMR_API(trywait, &mtx->fake);
}

static __xwos_inline_api
xwer_t xwlk_mtx_lock(struct xwlk_mtx * mtx)
{
        return XWSDYNC_SMR_API(wait, &mtx->fake);
}

static __xwos_inline_api
xwer_t xwlk_mtx_timedlock(struct xwlk_mtx * mtx, xwtm_t * xwtm)
{
        return XWSDYNC_SMR_API(timedwait, &mtx->fake, xwtm);
}

static __xwos_inline_api
xwer_t xwlk_mtx_lock_unintr(struct xwlk_mtx * mtx)
{
        return XWSDYNC_SMR_API(wait_unintr, &mtx->fake);
}

#endif /* xwos/up/lock/fakemutex.h */
