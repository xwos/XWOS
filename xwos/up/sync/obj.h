/**
 * @file
 * @brief 玄武OS UP内核同步机制：同步对象
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_sync_obj_h__
#define __xwos_up_sync_obj_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>

struct xwup_evt;

/**
 * @brief 同步对象
 */
struct xwup_synobj {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
        struct {
                struct xwup_evt * evt; /**< 事件对象 */
                xwsq_t pos; /**< 事件对象中的位图位置 */
        } sel; /**< 选择器 */
#endif
};

void xwup_synobj_construct(struct xwup_synobj * synobj);
void xwup_synobj_destruct(struct xwup_synobj * synobj);
xwer_t xwup_synobj_activate(struct xwup_synobj * synobj, xwobj_gc_f gcfunc);
xwer_t xwup_synobj_acquire(struct xwup_synobj * synobj, xwsq_t tik);
xwer_t xwup_synobj_release(struct xwup_synobj * synobj, xwsq_t tik);
xwer_t xwup_synobj_grab(struct xwup_synobj * synobj);
xwer_t xwup_synobj_put(struct xwup_synobj * synobj);


#endif /* xwos/up/sync/obj.h */
