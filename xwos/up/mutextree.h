/**
 * @file
 * @brief XuanWuOS内核：互斥锁树
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_mutextree_h__
#define __xwos_up_mutextree_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwlk_mtx;

/**
 * @brief 互斥锁树
 */
struct xwos_mtxtree {
        struct xwlib_rbtree rbtree; /**< 红黑树（关键字：优先级） */
        xwpr_t maxprio; /**< 最大关键字 */
        struct xwlk_mtx * rightmost; /**< 拥有最大关键字的互斥锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwos_mtxtree_init(struct xwos_mtxtree * mt);

__xwos_code
void xwos_mtxtree_do_add(struct xwos_mtxtree * mt, struct xwlk_mtx * mtx);

__xwos_code
xwer_t xwos_mtxtree_add(struct xwos_mtxtree * mt, struct xwlk_mtx * mtx);

__xwos_code
void xwos_mtxtree_do_remove(struct xwos_mtxtree * mt, struct xwlk_mtx * mtx);

__xwos_code
xwer_t xwos_mtxtree_remove(struct xwos_mtxtree * mt, struct xwlk_mtx * mtx);

#endif /* xwos/up/mutextree.h */
