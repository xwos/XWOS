/**
 * @file
 * @brief XWOS通用库：键值对容器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * # 概述
 *   map是按从“小”到“大”排列的键值对容器的集合。在map中，“键”是唯一的。
 */

#ifndef __xwos_lib_map_h__
#define __xwos_lib_map_h__

#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>

/**
 * @brief 键值对容器
 */
struct __xwcc_alignptr xwlib_map_container {
        struct xwlib_rbtree_node rbn; /**< 红黑树节点 */
        struct xwlib_bclst_node bcln; /**< 链表节点 */
        void * key; /**< 键 */
        void * value; /**< 值 */
};

/**
 * @brief 键比较函数类型
 */
typedef xwssq_t (*xwlib_map_cmp_f)(void *, void *);

/**
 * @brief 键值对集合
 */
struct __xwcc_alignptr xwlib_map {
        struct xwlib_rbtree rbtree; /**< 红黑树 */
        struct xwlib_bclst_head bclh; /**< 链表头 */
        xwlib_map_cmp_f cmp; /**< 键比较函数 */
};

/**
 * @brief 初始化map
 * @param m: (I) map指针
 */
static __xwlib_inline
void xwlib_map_init(struct xwlib_map * m, xwlib_map_cmp_f cmp)
{
        xwlib_rbtree_init(&m->rbtree);
        xwlib_bclst_init_head(&m->bclh);
        m->cmp = cmp;
}

/**
 * @brief 初始化map容器
 * @param mc: (I) 键值对容器的指针
 */
static __xwlib_inline
void xwlib_map_init_container(struct xwlib_map_container * mc,
                              void * key, void * value)
{
        xwlib_rbtree_init_node(&mc->rbn);
        xwlib_bclst_init_node(&mc->bcln);
        mc->key = key;
        mc->value = value;
}

/**
 * @brief 向前遍历(从小到大)键值对容器
 * @param m: (I) map指针
 * @param c: (I) 光标指针，指向当前遍历到的键值对容器
 */
#define xwlib_map_itr_forward(map, c) \
        xwlib_bclst_itr_next_entry((c), \
                                   &(map)->bclh, \
                                   struct xwlib_map_container, bcln)

/**
 * @brief 向后遍历(从大到小)键值对容器
 * @param m: (I) map指针
 * @param c: (I) 光标指针，指向当前遍历到的键值对容器
 */
#define xwlib_map_itr_backward(map, c) \
        xwlib_bclst_itr_next_entry((c), \
                                   &(map)->bclh, \
                                   struct xwlib_map_container, bcln)

/**
 * @brief 向前遍历(从小到大)键值对容器，并防止因遍历到的容器被删除而造成的错误
 * @param m: (I) map指针
 * @param c: (I) 光标指针，指向当前遍历到的键值对容器
 * @param n: (I) 键值对容器的指针，用于宏内部暂存数据
 */
#define xwlib_map_itr_forward_safe(map, c, n) \
        xwlib_bclst_itr_next_entry_safe((c), \
                                        &(map)->bclh, \
                                        struct xwlib_map_container, bcln)

/**
 * @brief 向后遍历(从大到小)键值对容器，并防止因遍历到的容器被删除而造成的错误
 * @param m: (I) map指针
 * @param c: (I) 光标指针，指向当前遍历到的键值对容器
 * @param n: (I) 键值对容器的指针，用于宏内部暂存数据
 */
#define xwlib_map_itr_backward_safe(map, c, n) \
        xwlib_bclst_itr_prev_entry_safe((c), (n), \
                                        &(map)->bclh, \
                                        struct xwlib_map_container, bcln)

xwer_t xwlib_map_insert(struct xwlib_map * m, struct xwlib_map_container * newmc);
xwer_t xwlib_map_erase(struct xwlib_map * m, struct xwlib_map_container * mc);
xwer_t xwlib_map_find(struct xwlib_map * m, void * key,
                      struct xwlib_map_container ** mcbuf);

#endif /* xwos/lib/map.h */
