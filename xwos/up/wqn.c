/**
 * @file
 * @brief XWOS UP内核：等待队列节点
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>
#include <xwos/up/skd.h>
#include <xwos/up/wqn.h>

/**
 * @brief 初始化等待队列节点
 * @param[in] wqn: 等待队列节点指针
 */
__xwup_code
void xwup_wqn_init(struct xwup_wqn * wqn)
{
        wqn->type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
        wqn->reason = (xwu16_t)XWUP_WQN_REASON_UNKNOWN;
        wqn->wq = NULL;
        wqn->prio = (xwpr_t)XWUP_SKD_PRIORITY_INVALID;
        xwlib_rbtree_init_node(&wqn->rbn);
        xwlib_bclst_init_node(&wqn->cln.rbb);
        wqn->cb = NULL;
}
