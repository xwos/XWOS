/**
 * @file
 * @brief 玄武OS UP内核同步机制：信号量的虚基类
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_vsem_h__
#define __xwos_up_sync_vsem_h__

#include <xwos/standard.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
#  include <xwos/up/sync/obj.h>
#endif

struct xwup_evt;

/**
 * @brief 信号量的虚基类
 */
/* abstract */ struct xwup_vsem {
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        struct xwup_synobj synobj; /**< C语言面向对象：继承struct xwup_synobj */
#endif
        xwssq_t count; /**< 信号量计数器：如果这个成员的值<0，信号量处于负状态。*/
        xwssq_t max; /**< 信号量计数器的最大值 */
};

/**
 * @brief 信号量的负值
 */
#define XWUP_VSEM_NEGTIVE     ((xwssq_t)(-1))

void xwup_vsem_activate(struct xwup_vsem * sem);

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
xwer_t xwup_vsem_bind(struct xwup_vsem * sem, struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_vsem_unbind(struct xwup_vsem * sem, struct xwup_evt * evt);
#else
#  define xwup_vsem_bind(sem, evt, pos) (-ENOSYS)
#  define xwup_vsem_unbind(sem, evt) (-ENOSYS)
#endif

xwer_t xwup_vsem_freeze(struct xwup_vsem * sem);
xwer_t xwup_vsem_thaw(struct xwup_vsem * sem);
xwer_t xwup_vsem_getvalue(struct xwup_vsem * sem, xwssq_t * sval);

/*
__xwup_api
xwer_t xwup_vsem_init(struct xwup_vsem * sem, xwssq_t val, xwssq_t max) = 0;

__xwup_api
xwer_t xwup_vsem_fini(struct xwup_vsem * sem) = 0;

__xwup_api
xwer_t xwup_vsem_create(struct xwup_vsem ** ptrbuf, xwssq_t val, xwssq_t max) = 0;

__xwup_api
xwer_t xwup_vsem_delete(struct xwup_vsem * sem) = 0;

__xwup_api
xwer_t xwup_vsem_post(struct xwup_vsem * sem) = 0;

__xwup_api
xwer_t xwup_vsem_trywait(struct xwup_vsem * sem) = 0;

__xwup_api
xwer_t xwup_vsem_timedwait(struct xwup_vsem * sem, xwtm_t * xwtm) = 0;

__xwup_api
xwer_t xwup_vsem_wait_unintr(struct xwup_vsem * sem) = 0;
*/

#endif /* xwos/up/sync/vsem.h */
