/**
 * @file
 * @brief 玄武OS MP同步机制：同步对象
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/object.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwmp_evt;

/**
 * @brief 同步对象
 */
struct xwmp_sync_object {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
        struct {
                struct xwmp_evt * evt; /**< 事件对象 */
                xwsq_t pos; /**< 事件对象中的位图位置 */
        } sel; /**< 选择器 */
#endif /* XWMPCFG_SYNC_EVT */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void xwmp_sync_object_construct(struct xwmp_sync_object * synobj, xwid_t type);
void xwmp_sync_object_destruct(struct xwmp_sync_object * synobj);
xwer_t xwmp_sync_object_activate(struct xwmp_sync_object * synobj, xwobj_gc_f gcfunc);
xwer_t xwmp_sync_object_grab(struct xwmp_sync_object * synobj);
xwer_t xwmp_sync_object_put(struct xwmp_sync_object * synobj);

#endif /* xwos/mp/sync/object.h */
