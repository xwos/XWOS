/**
 * @file
 * @brief 玄武OS MP内核：调度器的时间树
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_tt_h__
#define __xwos_mp_tt_h__

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mp/irq.h>
#include <xwos/mp/lock/seqlock.h>
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
#  include <xwos/mp/bh.h>
#endif

#define XWMP_TTN_TYPE_MASK            (3UL)

struct xwmp_skd;
struct xwmp_tt;

/**
 * @brief 系统硬件定时器
 */
struct xwmp_syshwt {
        xwtm_t timetick; /**< 系统时间 */
        struct xwmp_sqlk lock; /**< 保护本结构体的锁 */
        const struct xwos_irq_resource * irqrsc; /**< 中断资源 */
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
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        struct xwmp_bh_node bhn; /**< 中断底半部节点 */
#endif
        struct xwmp_syshwt hwt; /**< 硬件定时器 */
};

/**
 * @brief 时间树节点的回调函数
 */
typedef void (* xwmp_tt_cb_f)(void *);

/**
 * @brief 唤醒原因枚举
 */
enum xwmp_ttn_wkup_reasaon_em {
        XWMP_TTN_WKUPRS_UNKNOWN = 0,
        XWMP_TTN_WKUPRS_TIMEDOUT, /**< 超时 */
        XWMP_TTN_WKUPRS_INTR, /**< 被中断 */
};

/**
 * @brief 时间树节点的类型枚举
 */
enum xwmp_ttn_type_em {
        XWMP_TTN_TYPE_THD = 0, /**< 线程 */
        XWMP_TTN_TYPE_SWT, /**< 软件定时器 */
};

/**
 * @brief 时间树节点
 */
struct xwmp_ttn {
        struct xwlib_bclst_node rbb; /**< 此有相同关键字的红黑树节点，
                                          组成双循环链表，此为链表节点 */
        struct xwlib_rbtree_node rbn; /**< 红黑树节点 */
        xwtm_t wkup_xwtm; /**< 唤醒时间 */
        xwsq_a wkuprs; /**< 唤醒原因 */
        xwmp_tt_cb_f cb; /**< 回调函数：NULL表示节点不在时间树上 */
        struct xwmp_tt * xwtt; /**< 时间树 */
        union {
                xwptr_t addr; /**< XWOS对象要求至少4字节对齐
                                   （若为64位系统则要求8字节对齐），
                                   因此，低2位始终为0 */
                xwptr_t type; /**< 低2位作为类型标识 */
        } entry; /**< 时间树节点所属的对象 */
};

void xwmp_ttn_init(struct xwmp_ttn * ttn, xwptr_t entry, xwptr_t type);
xwer_t xwmp_ttn_grab(struct xwmp_ttn * ttn);
xwer_t xwmp_ttn_put(struct xwmp_ttn * ttn);

xwer_t xwmp_tt_init(struct xwmp_tt * xwtt);
xwer_t xwmp_tt_add_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn,
                          xwreg_t cpuirq);
xwer_t xwmp_tt_remove_locked(struct xwmp_tt * xwtt, struct xwmp_ttn * ttn);
xwer_t xwmp_tt_check_deadline(struct xwmp_tt * xwtt);
struct xwmp_skd * xwmp_tt_get_skd(struct xwmp_tt * xwtt);

xwer_t xwmp_syshwt_init(struct xwmp_syshwt * hwt);
xwer_t xwmp_syshwt_start(struct xwmp_syshwt * hwt);
xwer_t xwmp_syshwt_stop(struct xwmp_syshwt * hwt);
xwtm_t xwmp_syshwt_get_timetick(struct xwmp_syshwt * hwt);
xwu64_t xwmp_syshwt_get_tickcount(struct xwmp_syshwt * hwt);
xwtm_t xwmp_syshwt_get_timestamp(struct xwmp_syshwt * hwt);
void xwmp_syshwt_task(struct xwmp_syshwt * hwt);
struct xwmp_tt * xwmp_syshwt_get_tt(struct xwmp_syshwt * hwt);

/**
 * @brief 获取时间树节点所属的对象的地址
 * @param[in] ttn: 时间树节点的指针
 * @return 时间树节点所属的对象的地址
 */
static __xwmp_inline
void * xwmp_ttn_get_entry(struct xwmp_ttn * ttn)
{
        return (void *)(ttn->entry.addr & ~XWMP_TTN_TYPE_MASK);
}

/**
 * @brief 获取时间树节点的类型
 * @param[in] ttn: 时间树节点的指针
 * @return 类型
 */
static __xwmp_inline
xwptr_t xwmp_ttn_get_type(struct xwmp_ttn * ttn)
{
        return ttn->entry.type & XWMP_TTN_TYPE_MASK;
}

#endif /* xwos/mp/tt.h */
