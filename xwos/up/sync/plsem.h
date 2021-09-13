/**
 * @file
 * @brief 玄武OS UP内核同步机制：管道信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_plsem_h__
#define __xwos_up_sync_plsem_h__

#include <xwos/standard.h>
#include <xwos/up/plwq.h>
#include <xwos/up/sync/vsem.h>

/**
 * @brief 管道信号量对象
 */
struct xwup_plsem {
        struct xwup_vsem vsem; /**< C语言面向对象：继承struct xwup_vsem */
        struct xwup_plwq plwq; /**< 实时等待队列 */
};

xwer_t xwup_plsem_init(struct xwup_plsem * sem, xwssq_t val, xwssq_t max);
xwer_t xwup_plsem_fini(struct xwup_plsem * sem);
xwer_t xwup_plsem_create(struct xwup_plsem ** ptrbuf, xwssq_t val, xwssq_t max);
xwer_t xwup_plsem_delete(struct xwup_plsem * sem);
xwer_t xwup_plsem_intr(struct xwup_plsem * sem, struct xwup_wqn * wqn);

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
xwer_t xwup_plsem_bind(struct xwup_plsem * sem, struct xwup_evt * evt, xwsq_t pos);
xwer_t xwup_plsem_unbind(struct xwup_plsem * sem, struct xwup_evt * evt);
#else
#  define xwup_plsem_bind(sem, evt, pos) (-ENOSYS)
#  define xwup_plsem_unbind(sem, evt) (-ENOSYS)
#endif

xwer_t xwup_plsem_post(struct xwup_plsem * sem);
xwer_t xwup_plsem_wait(struct xwup_plsem * sem);
xwer_t xwup_plsem_trywait(struct xwup_plsem * sem);
xwer_t xwup_plsem_timedwait(struct xwup_plsem * sem, xwtm_t * xwtm);
xwer_t xwup_plsem_wait_unintr(struct xwup_plsem * sem);
xwer_t xwup_plsem_freeze(struct xwup_plsem * sem);
xwer_t xwup_plsem_thaw(struct xwup_plsem * sem);
xwer_t xwup_plsem_getvalue(struct xwup_plsem * sem, xwssq_t * sval);

#endif /* xwos/up/sync/plsem.h */
