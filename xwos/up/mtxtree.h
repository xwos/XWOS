/**
 * @file
 * @brief 玄武OS UP内核：互斥锁树
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_mtxtree_h__
#define __xwos_up_mtxtree_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwup_mtx;

/**
 * @brief 互斥锁树
 */
struct xwup_mtxtree {
        struct xwlib_rbtree rbtree; /**< 红黑树（关键字：优先级） */
        xwpr_t maxprio; /**< 最大关键字 */
        struct xwup_mtx * rightmost; /**< 拥有最大关键字的互斥锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void xwup_mtxtree_init(struct xwup_mtxtree * mt);
void xwup_mtxtree_add(struct xwup_mtxtree * mt, struct xwup_mtx * mtx);
void xwup_mtxtree_remove(struct xwup_mtxtree * mt, struct xwup_mtx * mtx);

#endif /* xwos/up/mtxtree.h */
