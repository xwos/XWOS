/**
 * @file
 * @brief 玄武OS MP内核：互斥锁树
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_mtxtree_h__
#define __xwos_mp_mtxtree_h__

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mp/lock/seqlock.h>

struct xwmp_mtx;

/**
 * @brief 互斥锁树
 */
struct xwmp_mtxtree {
        struct xwlib_rbtree rbtree; /**< 红黑树（关键字：优先级） */
        xwpr_t maxprio; /**< 最大关键字 */
        struct xwmp_mtx * rightmost; /**< 拥有最大关键字的互斥锁 */
        struct xwmp_sqlk lock; /* 保护结构体的锁 */
};

void xwmp_mtxtree_init(struct xwmp_mtxtree * mtxtree);
void xwmp_mtxtree_add_locked(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt);
void xwmp_mtxtree_add(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt);
void xwmp_mtxtree_remove_locked(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt);
void xwmp_mtxtree_remove(struct xwmp_mtx * mtx, struct xwmp_mtxtree * mt);

#endif /* xwos/mp/mtxtree.h */
