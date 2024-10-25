/**
 * @file
 * @brief XWOS UP内核：调度器的时间树
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_tt_h__
#define __xwos_up_tt_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/lock/seqlock.h>
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
#  include <xwos/up/bh.h>
#endif

#define XWUP_TTN_TYPE_MASK            (3UL)

struct xwup_skd;
struct xwup_tt;

/**
 * @brief 系统硬件定时器
 */
struct xwup_syshwt {
        xwtm_t timetick; /**< 系统时间 */
        struct xwup_sqlk lock; /**< 保护本结构体的锁 */
        const xwirq_t * irqrsc; /**< 中断资源 */
        xwsz_t irqs_num; /**< 中断资源的数量 */
};

struct xwup_ttn;

/**
 * @brief XWOS UP内核时间树
 */
struct xwup_tt {
        struct xwlib_rbtree rbtree; /**< 红黑树（关键字：时间） */
        xwtm_t deadline; /**< 红黑树中最小（最左边）的关键字 */
        struct xwup_ttn * leftmost; /**< 指向最小关键字节点的指针 */
        struct xwlib_bclst_head timeout; /**< 超时链表的表头 */
        struct xwup_sqlk lock; /**< 保护本结构体的自旋锁 */
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        struct xwup_bh_node bhn; /**< 中断底半部节点 */
#endif
        struct xwup_syshwt hwt; /**< 硬件定时器 */
};

/**
 * @brief 时间树节点的回调函数
 */
typedef void (* xwup_tt_cb_f)(struct xwup_ttn * ttn);

/**
 * @brief 唤醒原因枚举
 */
enum xwup_ttn_wkup_reasaon_em {
        XWUP_TTN_WKUPRS_UNKNOWN = 0U,
        XWUP_TTN_WKUPRS_TIMEDOUT = 1U, /**< 超时 */
        XWUP_TTN_WKUPRS_INTR = 2U, /**< 被中断 */
};

/**
 * @brief 时间树节点
 */
struct xwup_ttn {
        struct xwlib_bclst_node rbb; /**< 此有相同关键字的红黑树节点，
                                          组成双循环链表，此为链表节点 */
        struct xwlib_rbtree_node rbn; /**< 红黑树节点 */
        xwtm_t wkup_xwtm; /**< 唤醒时间 */
        xwsq_t wkuprs; /**< 唤醒原因 */
        xwup_tt_cb_f cb; /**< 回调函数：NULL表示节点不在时间树上 */
        struct xwup_tt * xwtt; /**< 时间树 */
};

void xwup_ttn_init(struct xwup_ttn * ttn);

xwer_t xwup_tt_init(struct xwup_tt * xwtt);
xwer_t xwup_tt_add_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn,
                          xwreg_t cpuirq);
xwer_t xwup_tt_remove_locked(struct xwup_tt * xwtt, struct xwup_ttn * ttn);
xwer_t xwup_tt_check_deadline(struct xwup_tt * xwtt);
struct xwup_skd * xwup_tt_get_skd(struct xwup_tt * xwtt);

xwer_t xwup_syshwt_init(struct xwup_syshwt * hwt);
xwer_t xwup_syshwt_start(struct xwup_syshwt * hwt);
xwer_t xwup_syshwt_stop(struct xwup_syshwt * hwt);
xwtm_t xwup_syshwt_get_time(struct xwup_syshwt * hwt);
xwtm_t xwup_syshwt_get_timestamp(struct xwup_syshwt * hwt);
xwtk_t xwup_syshwt_get_tick(struct xwup_syshwt * hwt);
void xwup_syshwt_task(struct xwup_syshwt * hwt);
struct xwup_tt * xwup_syshwt_get_tt(struct xwup_syshwt * hwt);

#endif /* xwos/up/tt.h */
