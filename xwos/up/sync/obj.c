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
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① 等待队列的锁（rtwq->lock & plwq->lock）
 *     + ② 等待队列节点的锁（wqn->lock）
 *     + ② 线程控制块的状态锁（thd->stlock）
 *     + ② 事件对象的锁（evt->lock）
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/up/sync/obj.h>

__xwup_code
void xwup_synobj_construct(struct xwup_synobj * synobj)
{
        xwos_object_construct(&synobj->xwobj);
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        synobj->sel.evt = NULL;
        synobj->sel.pos = 0;
#endif
}

__xwup_code
void xwup_synobj_destruct(struct xwup_synobj * synobj)
{
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        synobj->sel.evt = NULL;
        synobj->sel.pos = 0;
#endif
        xwos_object_destruct(&synobj->xwobj);
}

__xwup_code
xwer_t xwup_synobj_activate(struct xwup_synobj * synobj, xwobj_gc_f gcfunc)
{
        return xwos_object_activate(&synobj->xwobj, gcfunc);
}

__xwup_code
xwer_t xwup_synobj_acquire(struct xwup_synobj * synobj, xwsq_t tik)
{
        return xwos_object_acquire(&synobj->xwobj, tik);
}

__xwup_code
xwer_t xwup_synobj_release(struct xwup_synobj * synobj, xwsq_t tik)
{
        return xwos_object_release(&synobj->xwobj, tik);
}

__xwup_code
xwer_t xwup_synobj_grab(struct xwup_synobj * synobj)
{
        return xwos_object_grab(&synobj->xwobj);
}

__xwup_code
xwer_t xwup_synobj_put(struct xwup_synobj * synobj)
{
        return xwos_object_put(&synobj->xwobj);
}
