/**
 * @file
 * @brief XWOS MP内核：调度器的时间树
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_tt_h__
#define __xwos_mp_tt_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mp/irq.h>
#include <xwos/mp/lock/seqlock.h>
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
#  include <xwos/mp/bh.h>
#endif

#define XWMP_TTN_TYPE_MASK (3UL)

struct xwmp_skd;
struct xwmp_tt;

/**
 * @brief 系统硬件定时器
 */
struct xwmp_syshwt {
        xwtm_t timetick; /**< 系统时间 */
        struct xwmp_sqlk lock; /**< 保护本结构体的锁 */
        const xwirq_t * irqrsc; /**< 中断资源 */
        xwsz_t irqs_num; /**< 中断资源的数量 */
};

struct xwmp_ttn;

/**
 * @brief XWOS MP内核时间树
 */
struct xwmp_tt {
        struct xwlib_rbtree rbtree; /**< 红黑树（关键字：时间） */
        xwtm_t deadline; /**< 红黑树中最小（最左边）的关键字 */
        struct xwmp_ttn * leftmost; /**< 指向最小关键字节点的指针 */
        struct xwlib_bclst_head timeout; /**< 超时链表的表头 */
        struct xwmp_sqlk lock; /**< 保护本结构体的自旋锁 */
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        struct xwmp_bh_node bhn; /**< 中断底半部节点 */
#endif
        struct xwmp_syshwt hwt; /**< 硬件定时器 */
};

/**
 * @brief 时间树节点的回调函数
 */
typedef void (* xwmp_tt_cb_f)(struct xwmp_ttn *);

/**
 * @brief 唤醒原因枚举
 */
enum xwmp_ttn_wkup_reasaon_em {
        XWMP_TTN_WKUPRS_UNKNOWN = 0U,
        XWMP_TTN_WKUPRS_TIMEDOUT = 1U, /**< 超时 */
        XWMP_TTN_WKUPRS_INTR = 2U, /**< 被中断 */
};

/**
 * @brief 时间树节点
 */
struct xwmp_ttn {
        struct xwlib_bclst_node rbb; /**< 此有相同关键字的红黑树节点，
                                          组成双循环链表，此为链表节点 */
        struct xwlib_rbtree_node rbn; /**< 红黑树节点 */
        xwtm_t wkup_xwtm; /**< 唤醒时间 */
        atomic_xwsq_t wkuprs; /**< 唤醒原因 */
        xwmp_tt_cb_f cb; /**< 回调函数：NULL表示节点不在时间树上 */
        struct xwmp_tt * xwtt; /**< 时间树 */
};

void xwmp_ttn_init(struct xwmp_ttn * ttn);

xwer_t xwmp_tt_init(struct xwmp_tt * xwtt);
xwer_t xwmp_tt_add_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn,
                          xwreg_t cpuirq);
xwer_t xwmp_tt_remove_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn);
xwer_t xwmp_tt_check_deadline(struct xwmp_tt * xwtt);
struct xwmp_skd * xwmp_tt_get_skd(struct xwmp_tt * xwtt);

xwer_t xwmp_syshwt_init(struct xwmp_syshwt * hwt);
xwer_t xwmp_syshwt_start(struct xwmp_syshwt * hwt);
xwer_t xwmp_syshwt_stop(struct xwmp_syshwt * hwt);
xwtm_t xwmp_syshwt_get_time(struct xwmp_syshwt * hwt);
xwtm_t xwmp_syshwt_get_timestamp(struct xwmp_syshwt * hwt);
xwtk_t xwmp_syshwt_get_tick(struct xwmp_syshwt * hwt);
void xwmp_syshwt_task(struct xwmp_syshwt * hwt);
struct xwmp_tt * xwmp_syshwt_get_tt(struct xwmp_syshwt * hwt);

#endif /* xwos/mp/tt.h */
