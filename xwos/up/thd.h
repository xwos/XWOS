/**
 * @file
 * @brief XWOS UP内核：线程
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_thd_h__
#define __xwos_up_thd_h__

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
#include <xwos/up/skd.h>
#include <xwos/up/wqn.h>
#include <xwos/up/tt.h>
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
#  include <xwos/up/mtxtree.h>
#endif
#if defined(XWOSCFG_SYNC_COND) && (1 == XWOSCFG_SYNC_COND)
#  include <xwos/up/sync/cond.h>
#endif

struct xwup_skd;

#if (1 == XWOSRULE_SKD_WQ_RT)
struct xwup_rtwq;
#endif

#if (1 == XWOSRULE_SKD_WQ_PL)
struct xwup_plwq;
#endif

/**
 * @brief XWOS UP线程属性
 */
struct xwup_thd_attr {
        const char * name; /**< 线程的名字 */
        xwstk_t * stack; /**< 栈内存的基地址 */
        xwsz_t stack_size; /**< 线程栈的大小，以字节(byte)为单位，
                                注意与CPU的ABI接口规定的内存边界对齐 */
        xwsz_t stack_guard_size; /**< 栈内存警戒线 */
        xwpr_t priority; /**< 优先级 */
        bool detached; /**< 是否为分离态 */
        bool privileged; /**< 是否为特权线程 */
};

/**
 * @brief XWOS UP线程对象
 * @details
 * 在语义上，相当于C++的
 * ```C++
 * class xwup_thd :
 *   public xwos_object,
 *   public xwup_skdobj_stack,
 *   public xwup_ttn,
 *   public xwup_wqn,
 *   public xwup_mtxtree
 * ```
 */
struct xwup_thd {
        struct xwos_object xwobj; /**< C语言面向对象：继承 `struct xwos_object` */
        struct xwup_skdobj_stack stack; /**< 栈 */
        struct xwlib_bclst_node thdnode; /**< 调度器线程链表中的节点 */

        xwsq_t state; /**< 线程状态 */

#if (1 == XWOSRULE_SKD_THD_FREEZE)
        /* 冻结态 */
        struct xwlib_bclst_node frznode; /**< 冻结链表节点 */
#endif

        /* 就绪态 */
        struct xwlib_bclst_node rqnode; /**< 就绪队列节点，被锁 `rtrq->lock` 保护 */

        /* 睡眠态 */
        struct xwup_ttn ttn; /**< 时间树节点, 被锁 `tt->lock` 保护 */

        /* 等待态 */
        struct xwup_wqn wqn; /**< 等待队列 */

        /* 线程优先级 */
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
        struct xwup_mtxtree mt; /**< 互斥锁树 */
        struct {
                xwpr_t s; /**< 线程的固有优先级 */
                xwpr_t d; /**< 线程的动态优先级 */
        } prio; /**< 线程的优先级 */
#else
        union {
                xwpr_t s; /**< 线程的固有优先级 */
                xwpr_t d; /**< 线程的动态优先级 */
        } prio; /**< 线程的优先级 */
#endif

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        /* 退出态 */
        struct xwup_cond completion; /**< 线程退出时的事件信号量 */
#endif

#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
        /* 线程私有数据 */
        void * data[XWOSCFG_SKD_THD_LOCAL_DATA_NUM];
#endif
        struct {
                int error_number; /**< 错误码 */
        } libc;
};

#if (defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX))
void xwup_thd_chprio_once(struct xwup_thd * thd, xwpr_t dprio,
                          struct xwup_mtx ** pmtx);
void xwup_thd_chprio(struct xwup_thd * thd);
#endif

xwer_t xwup_thd_exit_lic(struct xwup_thd * thd, xwer_t rc);
void xwup_cthd_return(xwer_t rc);
xwer_t xwup_thd_rq_add_head(struct xwup_thd * thd);
xwer_t xwup_thd_rq_add_tail(struct xwup_thd * thd);
void xwup_thd_rq_remove(struct xwup_thd * thd);
xwer_t xwup_thd_tt_add_locked(struct xwup_thd * thd, struct xwup_tt * xwtt,
                              xwtm_t to, xwreg_t flag);

#if (1 == XWOSRULE_SKD_WQ_RT)
void xwup_thd_eq_rtwq(struct xwup_thd * thd, struct xwup_rtwq * xwrtwq,
                      xwu16_t type);
#endif

#if (1 == XWOSRULE_SKD_WQ_PL)
void xwup_thd_eq_plwq(struct xwup_thd * thd, struct xwup_plwq * xwplwq,
                      xwu16_t type);
#endif

xwer_t xwup_thd_freeze_lic(struct xwup_thd * thd);
xwer_t xwup_thd_thaw_lic(struct xwup_thd * thd);
xwer_t xwup_thd_drop(struct xwup_thd * thd);

/**
 * @brief 唤醒一个阻塞/睡眠态的线程，将其加入到调度器的就绪队列
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 */
static __xwcc_inline
void xwup_thd_wakeup(struct xwup_thd * thd)
{
        xwup_thd_rq_add_tail(thd); // cppcheck-suppress [misra-c2012-17.7]
}

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
xwer_t xwup_thd_cache_init(struct xwmm_mempool * mp, xwsq_t page_order);
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
xwer_t xwup_thd_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
void xwup_thd_cache_init(struct xwmm_sma * sma);
#endif

void xwup_thd_attr_init(struct xwup_thd_attr * attr);
xwer_t xwup_thd_init(struct xwup_thd * thd,
                     const struct xwup_thd_attr * inattr,
                     xwup_thd_f thdfunc, void * arg);
xwer_t xwup_thd_create(struct xwup_thd ** thdpbuf,
                       const struct xwup_thd_attr * inattr,
                       xwup_thd_f manfunc, void * arg);
xwer_t xwup_thd_acquire(struct xwup_thd * thd, xwsq_t tik);
xwer_t xwup_thd_release(struct xwup_thd * thd, xwsq_t tik);
xwer_t xwup_thd_grab(struct xwup_thd * thd);
xwer_t xwup_thd_put(struct xwup_thd * thd);
void xwup_thd_get_attr(struct xwup_thd * thd, struct xwup_thd_attr * attr);
void xwup_cthd_yield(void);
void xwup_cthd_exit(xwer_t rc);
void xwup_thd_quit(struct xwup_thd * thd);
xwer_t xwup_thd_join(struct xwup_thd * thd, xwer_t * trc);
xwer_t xwup_thd_detach(struct xwup_thd * thd);
xwer_t xwup_thd_intr(struct xwup_thd * thd);
xwer_t xwup_cthd_sleep_to(xwtm_t to);
xwer_t xwup_cthd_sleep_from(xwtm_t * from, xwtm_t dur);
xwer_t xwup_cthd_freeze(void);
bool xwup_cthd_shld_frz(void);
bool xwup_cthd_frz_shld_stop(bool * frozen);
bool xwup_cthd_shld_stop(void);

#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
xwer_t xwup_thd_set_data(struct xwup_thd * thd, xwsq_t pos, void * data);
xwer_t xwup_thd_get_data(struct xwup_thd * thd, xwsq_t pos, void ** databuf);
xwer_t xwup_cthd_set_data(xwsq_t pos, void * data);
xwer_t xwup_cthd_get_data(xwsq_t pos, void ** databuf);
#endif

#endif /* xwos/up/thd.h */
