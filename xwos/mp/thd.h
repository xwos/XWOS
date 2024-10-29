/**
 * @file
 * @brief XWOS MP内核：线程
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_thd_h__
#define __xwos_mp_thd_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
#  include <xwos/mm/sma.h>
#endif
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/wqn.h>
#include <xwos/mp/tt.h>
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
#  include <xwos/mp/mtxtree.h>
#endif
#if defined(XWOSCFG_SYNC_COND) && (1 == XWOSCFG_SYNC_COND)
#  include <xwos/mp/sync/cond.h>
#endif

struct xwmp_skd;
struct xwmp_thd;
#if (1 == XWOSRULE_SKD_WQ_RT)
struct xwmp_rtwq;
#endif
#if (1 == XWOSRULE_SKD_WQ_PL)
struct xwmp_plwq;
#endif

/**
 * @brief XWOS MP线程属性
 */
struct xwmp_thd_attr {
        const char * name; /**< 线程的名字 */
        xwstk_t * stack; /**< 栈内存的基地址 */
        xwsz_t stack_size; /**< 线程栈的大小 */
        xwsz_t stack_guard_size; /**< 栈内存警戒线 */
        xwpr_t priority; /**< 优先级 */
        bool detached; /**< 是否为分离态 */
        bool privileged; /**< 是否为特权线程 */
};

/**
 * @brief XWOS MP线程对象
 * @details
 * 在语义上，相当于C++的
 * ```C++
 * class xwmp_thd :
 *   public xwos_object,
 *   public xwmp_skdobj_stack,
 *   public xwmp_ttn,
 *   public xwmp_wqn,
 *   public xwmp_mtxtree
 * ```
 */
struct xwmp_thd {
        struct xwos_object xwobj; /**< C语言面向对象：继承 `struct xwos_object` */
        struct xwmp_skd * xwskd; /**< 当前线程所属于的调度器 */
        struct xwmp_skdobj_stack stack; /**< 栈 */
        struct xwlib_bclst_node thdnode; /**< 调度器线程链表中的节点 */

        struct xwmp_splk stlock; /**< 保护状态state的自旋锁 */
        xwsq_t state; /**< 线程状态 */

        /* 冻结态 */
        struct xwlib_bclst_node frznode; /**< 冻结链表节点 */

        /* 迁移态 */
        struct {
                xwid_t dst; /**< 目的CPU的ID */
        } migration; /**< 迁移状态成员 */

        /* 就绪态 */
        struct xwlib_bclst_node rqnode; /**< 就绪队列节点，被锁 `rtrq->lock` 保护 */

        /* 睡眠态 */
        struct xwmp_ttn ttn; /**< 继承：时间树节点, 被锁 `tt->lock` 保护 */

        /* 阻塞态 */
        struct xwmp_wqn wqn; /**< 等待队列 */

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        /* 退出态 */
        struct xwmp_cond completion; /**< 线程退出时的事件信号量 */
#endif

        /* 线程优先级 */
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
        struct xwmp_mtxtree mt; /**< 互斥锁树 */
#endif
        xwpr_t sprio; /**< 线程的固有优先级 */
        struct {
                xwpr_t r; /**< 线程的运行态优先级 */
                xwpr_t rq; /**< 线程的就绪态优先级 */
                xwpr_t wq; /**< 线程的等待态优先级 */
        } dprio; /**< 线程的动态优先级，被 `stlock` 保护 */

#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
        /* 线程私有数据 */
        void * data[XWOSCFG_SKD_THD_LOCAL_DATA_NUM];
#endif
        struct {
                int error_number; /**< 错误码 */
        } libc;
};

#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
xwer_t xwmp_thd_chprio_once(struct xwmp_thd * thd, xwpr_t dprio,
                            struct xwmp_mtx ** pmtx);
void xwmp_thd_chprio(struct xwmp_thd * thd);
#endif

xwer_t xwmp_thd_rq_add_head(struct xwmp_thd * thd, xwpr_t prio);
xwer_t xwmp_thd_rq_add_tail(struct xwmp_thd * thd, xwpr_t prio);
xwer_t xwmp_thd_rq_remove(struct xwmp_thd * thd);
xwer_t xwmp_thd_wakeup(struct xwmp_thd * thd);
xwer_t xwmp_thd_tt_add_locked(struct xwmp_thd * thd, struct xwmp_tt * xwtt,
                              xwtm_t to, xwreg_t cpuirq);

#if (1 == XWOSRULE_SKD_WQ_RT)
void xwmp_thd_eq_rtwq_locked(struct xwmp_thd * thd,
                             struct xwmp_rtwq * xwrtwq,
                             xwu16_t type,
                             xwpr_t dprio);
#endif
#if (1 == XWOSRULE_SKD_WQ_PL)
void xwmp_thd_eq_plwq_locked(struct xwmp_thd * thd,
                             struct xwmp_plwq * xwplwq,
                             xwu16_t type);
#endif
xwer_t xwmp_thd_reqfrz_lic(struct xwmp_thd * thd);
xwer_t xwmp_thd_thaw_lic_pmlk(struct xwmp_thd * thd);

xwer_t xwmp_thd_exit_lic(struct xwmp_thd * thd, xwer_t rc);
void xwmp_cthd_return(xwer_t rc);
xwer_t xwmp_thd_freeze_lic(struct xwmp_thd * thd);
void xwmp_thd_immigrate_lic(struct xwmp_thd * thd);
xwer_t xwmp_thd_outmigrate_lic(struct xwmp_thd * thd, xwid_t dstcpu);

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
xwer_t xwmp_thd_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
xwer_t xwmp_thd_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
void xwmp_thd_cache_init(struct xwmm_sma * sma);
#endif

void xwmp_thd_attr_init(struct xwmp_thd_attr * attr);
xwer_t xwmp_thd_init(struct xwmp_thd * thd,
                     const struct xwmp_thd_attr * inattr,
                     xwmp_thd_f thdfunc, void * arg);
xwer_t xwmp_thd_create(struct xwmp_thd ** thdpbuf,
                       const struct xwmp_thd_attr * inattr,
                       xwmp_thd_f manfunc, void * arg);
xwer_t xwmp_thd_acquire(struct xwmp_thd * thd, xwsq_t tik);
xwer_t xwmp_thd_release(struct xwmp_thd * thd, xwsq_t tik);
xwer_t xwmp_thd_grab(struct xwmp_thd * thd);
xwer_t xwmp_thd_put(struct xwmp_thd * thd);
void xwmp_thd_get_attr(struct xwmp_thd * thd, struct xwmp_thd_attr * attr);
void xwmp_cthd_yield(void);
void xwmp_cthd_exit(xwer_t rc);
void xwmp_thd_quit(struct xwmp_thd * thd);
xwer_t xwmp_thd_join(struct xwmp_thd * thd, xwer_t * trc);
xwer_t xwmp_thd_detach(struct xwmp_thd * thd);
xwer_t xwmp_thd_intr(struct xwmp_thd * thd);
xwer_t xwmp_cthd_sleep_to(xwtm_t to);
xwer_t xwmp_cthd_sleep_from(xwtm_t * from, xwtm_t dur);
xwer_t xwmp_cthd_freeze(void);
bool xwmp_cthd_shld_frz(void);
bool xwmp_cthd_shld_stop(void);
bool xwmp_cthd_frz_shld_stop(bool * frozen);
xwer_t xwmp_thd_migrate(struct xwmp_thd * thd, xwid_t dstcpu);

#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
xwer_t xwmp_thd_set_data(struct xwmp_thd * thd, xwsq_t pos, void * data);
xwer_t xwmp_thd_get_data(struct xwmp_thd * thd, xwsq_t pos, void ** databuf);
xwer_t xwmp_cthd_set_data(xwsq_t pos, void * data);
xwer_t xwmp_cthd_get_data(xwsq_t pos, void ** databuf);
#endif

#endif /* xwos/mp/thd.h */
