/**
 * @file
 * @brief newlib适配代码：锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/mtx.h>
#include <sys/lock.h>

struct __lock {
        struct xwos_mtx xwmtx;
};

struct __lock __lock___sinit_recursive_mutex;
struct __lock __lock___sfp_recursive_mutex;
struct __lock __lock___atexit_recursive_mutex;
struct __lock __lock___at_quick_exit_mutex;
struct __lock __lock___malloc_recursive_mutex;
struct __lock __lock___env_recursive_mutex;
struct __lock __lock___tz_mutex;
struct __lock __lock___dd_hash_mutex;
struct __lock __lock___arc4random_mutex;

void newlibac_lock_init(void)
{
        xwos_mtx_init(&__lock___sinit_recursive_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___sfp_recursive_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___atexit_recursive_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___at_quick_exit_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___malloc_recursive_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___env_recursive_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___tz_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___dd_hash_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
        xwos_mtx_init(&__lock___arc4random_mutex.xwmtx, XWOS_SKD_PRIORITY_RT_MAX);
}

void __retarget_lock_init(_LOCK_T * lock)
{
        _LOCK_T mtx;
        xwer_t rc;

        if (NULL != lock) {
                rc = xwos_mtx_create((struct xwos_mtx **)&mtx,
                                     XWOS_SKD_PRIORITY_RT_MAX);
                if (rc < 0) {
                        *lock = (_LOCK_T)NULL;
                } else {
                        *lock = mtx;
                }
        }
}

void __retarget_lock_init_recursive(_LOCK_T * lock)
{
        __retarget_lock_init(lock);
}

void __retarget_lock_close(_LOCK_T lock)
{
        if (NULL != lock) {
                xwos_mtx_delete(&lock->xwmtx);
        }
}

void __retarget_lock_close_recursive(_LOCK_T lock)
{
        __retarget_lock_close(lock);
}

void __retarget_lock_acquire(_LOCK_T lock)
{
        if (NULL != lock) {
                xwsq_t ctx;

                xwos_skd_get_context_lc(&ctx, NULL);
                if (XWOS_SKD_CONTEXT_THD == ctx) {
                        xwos_mtx_lock_unintr(&lock->xwmtx);
                } else if (XWMP_SKD_CONTEXT_INIT_EXIT == ctx) {
                } else {
                        XWOS_BUG();
                }
        }
}

void __retarget_lock_acquire_recursive(_LOCK_T lock)
{
        __retarget_lock_acquire(lock);
}

int __retarget_lock_try_acquire(_LOCK_T lock)
{
        xwer_t rc;

        if (NULL != lock) {
                xwsq_t ctx;

                xwos_skd_get_context_lc(&ctx, NULL);
                if (XWOS_SKD_CONTEXT_THD == ctx) {
                        rc = xwos_mtx_trylock(&lock->xwmtx);
                } else if (XWMP_SKD_CONTEXT_INIT_EXIT == ctx) {
                        rc = XWOK;
                } else {
                        rc = -ENOTINTHD;
                }
        } else {
                rc = -EFAULT;
        }
        return (int)rc;
}

int __retarget_lock_try_acquire_recursive(_LOCK_T lock)
{
        return __retarget_lock_try_acquire(lock);
}

void __retarget_lock_release(_LOCK_T lock)
{
        if (NULL != lock) {
                xwsq_t ctx;

                xwos_skd_get_context_lc(&ctx, NULL);
                if (XWOS_SKD_CONTEXT_THD == ctx) {
                        xwos_mtx_unlock(&lock->xwmtx);
                } else if (XWMP_SKD_CONTEXT_INIT_EXIT == ctx) {
                } else {
                        XWOS_BUG();
                }
        }
}

void __retarget_lock_release_recursive(_LOCK_T lock)
{
        __retarget_lock_release(lock);
}
