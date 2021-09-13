/**
 * @file
 * @brief 玄武OS UP内核同步机制：实时信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_rtsem_h__
#define __xwos_up_sync_rtsem_h__

#include <xwos/standard.h>
#include <xwos/up/rtwq.h>
#include <xwos/up/sync/vsem.h>

/**
 * @brief 实时信号量对象
 */
struct xwup_rtsem {
        struct xwup_vsem vsem; /**< C语言面向对象：继承struct xwup_vsem */
        struct xwup_rtwq rtwq; /**< 实时等待队列 */
};

xwer_t xwup_rtsem_init(struct xwup_rtsem * sem, xwssq_t val, xwssq_t max);
xwer_t xwup_rtsem_fini(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_create(struct xwup_rtsem ** ptrbuf, xwssq_t val, xwssq_t max);
xwer_t xwup_rtsem_delete(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_intr(struct xwup_rtsem * sem, struct xwup_wqn * wqn);

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
xwer_t xwup_rtsem_bind(struct xwup_rtsem * sem, struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_rtsem_unbind(struct xwup_rtsem * sem, struct xwup_evt * evt);
#else
#  define xwup_rtsem_bind(sem, evt, pos) (-ENOSYS)
#  define xwup_rtsem_unbind(sem, evt) (-ENOSYS)
#endif

xwer_t xwup_rtsem_post(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_wait(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_trywait(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_timedwait(struct xwup_rtsem * sem, xwtm_t * xwtm);
xwer_t xwup_rtsem_wait_unintr(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_freeze(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_thaw(struct xwup_rtsem * sem);
xwer_t xwup_rtsem_getvalue(struct xwup_rtsem * sem, xwssq_t * sval);

#endif /* xwos/up/sync/rtsem.h */
