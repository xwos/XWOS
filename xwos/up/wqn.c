/**
 * @file
 * @brief 玄武OS UP内核：等待队列节点
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>
#include <xwos/up/skd.h>
#include <xwos/up/wqn.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化等待队列节点
 * @param wqn: (I) 等待队列节点指针
 * @param owner: (I) 拥有结构体的对象的指针
 */
__xwup_code
void xwup_wqn_init(struct xwup_wqn * wqn, void * owner)
{
        wqn->owner = owner;
        wqn->type = (xwu8_t)XWUP_WQTYPE_UNKNOWN;
        wqn->reason = XWUP_WQN_REASON_UNKNOWN;
        wqn->wq = NULL;
        wqn->prio = XWUP_SKD_PRIORITY_INVALID;
        xwlib_rbtree_init_node(&wqn->rbn);
        xwlib_bclst_init_node(&wqn->cln.rbb);
        wqn->cb = NULL;
}
