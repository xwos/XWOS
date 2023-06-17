/**
 * @file
 * @brief XWOS MP内核：等待队列节点
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/wqn.h>

/**
 * @brief 初始化等待队列节点
 * @param[in] wqn: 等待队列节点指针
 * @param[in] owner: 拥有结构体的对象的指针
 */
__xwmp_code
void xwmp_wqn_init(struct xwmp_wqn * wqn, void * owner)
{
        wqn->owner = owner;
        wqn->type = (xwu8_t)XWMP_WQTYPE_UNKNOWN;
        wqn->reason = XWMP_WQN_REASON_UNKNOWN;
        wqn->wq = NULL;
        xwmp_splk_init(&wqn->lock);
        wqn->prio = XWMP_SKD_PRIORITY_INVALID;
        xwlib_rbtree_init_node(&wqn->rbn);
        xwlib_bclst_init_node(&wqn->cln.rbb);
        wqn->cb = NULL;
}
