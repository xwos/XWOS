/**
 * @file
 * @brief XWOS UP内核同步机制：信号量的虚基类
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_sync_vsem_h__
#define __xwos_up_sync_vsem_h__

#include <xwos/standard.h>
#include <xwos/up/sync/obj.h>

struct xwup_evt;

/**
 * @brief 信号量的虚基类
 */
/* abstract */ struct xwup_vsem {
        struct xwup_synobj synobj; /**< C语言面向对象：继承 `struct xwup_synobj` */
        xwssq_t count; /**< 信号量计数器：如果这个成员的值<0，信号量处于负状态。*/
        xwssq_t max; /**< 信号量计数器的最大值 */
};

/**
 * @brief 信号量的负值
 */
#define XWUP_VSEM_NEGTIVE     ((xwssq_t)(-1))

void xwup_vsem_construct(struct xwup_vsem * vsem);
void xwup_vsem_destruct(struct xwup_vsem * vsem);
xwer_t xwup_vsem_activate(struct xwup_vsem * sem, xwobj_gc_f gcfunc);
xwer_t xwup_vsem_acquire(struct xwup_vsem * sem, xwsq_t tik);
xwer_t xwup_vsem_release(struct xwup_vsem * sem, xwsq_t tik);
xwer_t xwup_vsem_grab(struct xwup_vsem * sem);
xwer_t xwup_vsem_put(struct xwup_vsem * sem);

xwer_t xwup_vsem_bind(struct xwup_vsem * sem, struct xwup_evt * sel, xwsq_t pos);
xwer_t xwup_vsem_unbind(struct xwup_vsem * sem, struct xwup_evt * sel);
xwer_t xwup_vsem_freeze(struct xwup_vsem * sem);
xwer_t xwup_vsem_thaw(struct xwup_vsem * sem);
xwer_t xwup_vsem_get_max(struct xwup_vsem * sem, xwssq_t * max);
xwer_t xwup_vsem_get_value(struct xwup_vsem * sem, xwssq_t * val);

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
xwer_t xwup_vsem_wait_to(struct xwup_vsem * sem, xwtm_t xwtm) = 0;

__xwup_api
xwer_t xwup_vsem_wait_unintr(struct xwup_vsem * sem) = 0;
*/

#endif /* xwos/up/sync/vsem.h */
