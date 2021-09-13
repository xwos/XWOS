/**
 * @file
 * @brief 玄武OS UP内核同步机制：同步对象
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_obj_h__
#define __xwos_up_sync_obj_h__

#include <xwos/standard.h>

struct xwup_evt;

/**
 * @brief 同步对象
 */
struct xwup_synobj {
        struct {
                struct xwup_evt * evt; /**< 事件对象 */
                xwsq_t pos; /**< 事件对象中的位图位置 */
        } sel; /**< 选择器 */
};

static __xwcc_inline
void xwup_synobj_activate(struct xwup_synobj * synobj)
{
        synobj->sel.evt = NULL;
        synobj->sel.pos = 0;
}

#endif /* xwos/up/sync/obj.h */
