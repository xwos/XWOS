/**
 * @file
 * @brief XWOS MP内核同步机制：同步对象
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
#include <xwos/mp/sync/obj.h>

__xwmp_code
void xwmp_synobj_construct(struct xwmp_synobj * synobj)
{
        xwos_object_construct(&synobj->xwobj);
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
        synobj->sel.evt = NULL;
        synobj->sel.pos = (xwsq_t)0;
#endif
}

__xwmp_code
void xwmp_synobj_destruct(struct xwmp_synobj * synobj)
{
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
        synobj->sel.evt = NULL;
        synobj->sel.pos = (xwsq_t)0;
#endif
        xwos_object_destruct(&synobj->xwobj);
}

__xwmp_code
xwer_t xwmp_synobj_activate(struct xwmp_synobj * synobj, xwobj_gc_f gcfunc)
{
        return xwos_object_activate(&synobj->xwobj, gcfunc);
}

__xwmp_code
xwer_t xwmp_synobj_acquire(struct xwmp_synobj * synobj, xwsq_t tik)
{
        return xwos_object_acquire(&synobj->xwobj, tik);
}

__xwmp_code
xwer_t xwmp_synobj_release(struct xwmp_synobj * synobj, xwsq_t tik)
{
        return xwos_object_release(&synobj->xwobj, tik);
}

__xwmp_code
xwer_t xwmp_synobj_grab(struct xwmp_synobj * synobj)
{
        return xwos_object_grab(&synobj->xwobj);
}

__xwmp_code
xwer_t xwmp_synobj_put(struct xwmp_synobj * synobj)
{
        return xwos_object_put(&synobj->xwobj);
}
