/**
 * @file
 * @brief XuanWuOS的同步机制：同步对象
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_sync_object_h__
#define __xwos_smp_sync_object_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwsync_evt;

/**
 * @brief 同步对象
 */
struct xwsync_object {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
        struct {
                struct xwsync_evt * evt; /**< 事件对象 */
                xwsq_t pos; /**< 事件对象中的位图位置 */
        } selector; /**< 选择器 */
#endif /* XWSMPCFG_SYNC_EVT */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xw_inline
void xwsync_object_construct(struct xwsync_object * xwsyncobj)
{
        xwos_object_construct(&xwsyncobj->xwobj);
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
        xwsyncobj->selector.evt = NULL;
        xwsyncobj->selector.pos = 0;
#endif /* XWSMPCFG_SYNC_EVT */
}

static __xw_inline
void xwsync_object_destruct(struct xwsync_object * xwsyncobj)
{
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
        xwsyncobj->selector.evt = NULL;
        xwsyncobj->selector.pos = 0;
#endif /* XWSMPCFG_SYNC_EVT */
        xwos_object_destruct(&xwsyncobj->xwobj);
}

static __xw_inline
xwer_t xwsync_object_activate(struct xwsync_object * xwsyncobj, xwobj_gc_f gcfunc)
{
        return xwos_object_activate(&xwsyncobj->xwobj, gcfunc);
}

static __xw_inline
xwer_t xwsync_object_grab(struct xwsync_object * xwsyncobj)
{
        return xwos_object_grab(&xwsyncobj->xwobj);
}

static __xw_inline
xwer_t xwsync_object_put(struct xwsync_object * xwsyncobj)
{
        return xwos_object_put(&xwsyncobj->xwobj);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/smp/sync/object.h */
