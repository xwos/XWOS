/**
 * @file
 * @brief XuanWuOS内核：等待队列节点
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 锁的顺序：
 *   + ① xwos_rtwq.lock
 *     + ② xwos_wqn.lock
 */

#ifndef __xwos_smp_wqn_h__
#define __xwos_smp_wqn_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/rbtree.h>
#include <xwos/lib/bclst.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/lock/spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 等待队列类型枚举
 */
enum xwos_wqtype_em {
        XWOS_WQTYPE_UNKNOWN = 0,
        XWOS_WQTYPE_NULL,
        XWOS_WQTYPE_PLSMR,
        XWOS_WQTYPE_RTSMR,
        XWOS_WQTYPE_CDT,
        XWOS_WQTYPE_EVENT,
        XWOS_WQTYPE_MTX,
};

/**
 * @brief 等待队列节点所属对象枚举
 */
enum xwos_wqowner_em {
        XWOS_WQOWNER_UNKNOWN = 0,
        XWOS_WQOWNER_THRD,
};

/**
 * @brief 等待队列节点唤醒原因枚举
 */
enum xwos_wqn_rsmrs_em {
        XWOS_WQN_RSMRS_UNKNOWN = 0,
        XWOS_WQN_RSMRS_UP, /**< 资源可用 */
        XWOS_WQN_RSMRS_INTR, /**< 等待被中断 */
};

/**
 * @brief 等待队列节点回调函数指针类型
 */
typedef void (*xwos_wqn_f)(void *);

/**
 * @brief 等待队列节点
 */
struct xwos_wqn {
        xwu16_t type; /**< 等待队列类型（信号量、互斥锁） */
        volatile xwu16_t rsmrs; /**< 唤醒原因 */
        void * wq; /**< 指向所属的等待队列的指针 */
        xwos_wqn_f cb; /**< 被唤醒时的回调函数 */
        struct xwlk_splk lock; /**< 保护此结构体的锁 */
        void * owner; /**< 拥有结构体的对象的指针 */
        xwpr_t prio; /**< 优先级 */
        struct xwlib_rbtree_node rbn; /**> 红黑树节点 */
        union {
                struct xwlib_bclst_node rbb;
                struct xwlib_bclst_node pl;
        } cln; /**< 双循环链表节点 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化等待队列节点
 * @param wqn: (I) 等待队列节点指针
 * @param owner: (I) 拥有结构体的对象的指针
 */
static __xw_inline
void xwos_wqn_init(struct xwos_wqn * wqn, void * owner)
{
        wqn->owner = owner;
        wqn->type = (xwu8_t)XWOS_WQTYPE_UNKNOWN;
        wqn->rsmrs = XWOS_WQN_RSMRS_UNKNOWN;
        wqn->wq = NULL;
        xwlk_splk_init(&wqn->lock);
        wqn->prio = XWOS_SD_PRIORITY_INVALID;
        xwlib_rbtree_init_node(&wqn->rbn);
        xwlib_bclst_init_node(&wqn->cln.rbb);
        wqn->cb = NULL;
}

#endif /* xwos/smp/wqn.h */
