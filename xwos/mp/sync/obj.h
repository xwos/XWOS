/**
 * @file
 * @brief 玄武OS MP内核同步机制：同步对象
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_sync_object_h__
#define __xwos_mp_sync_object_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>

struct xwmp_evt;

/**
 * @brief 同步对象
 */
struct xwmp_synobj {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
        struct {
                struct xwmp_evt * evt; /**< 事件对象 */
                xwsq_t pos; /**< 事件对象中的位图位置 */
        } sel; /**< 选择器 */
#endif
};

void xwmp_synobj_construct(struct xwmp_synobj * synobj);
void xwmp_synobj_destruct(struct xwmp_synobj * synobj);
xwer_t xwmp_synobj_activate(struct xwmp_synobj * synobj, xwobj_gc_f gcfunc);
xwer_t xwmp_synobj_acquire(struct xwmp_synobj * synobj, xwsq_t tik);
xwer_t xwmp_synobj_release(struct xwmp_synobj * synobj, xwsq_t tik);
xwer_t xwmp_synobj_grab(struct xwmp_synobj * synobj);
xwer_t xwmp_synobj_put(struct xwmp_synobj * synobj);

#endif /* xwos/mp/sync/object.h */
