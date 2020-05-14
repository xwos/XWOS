/**
 * @file
 * @brief XuanWuOS内核：线程
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_thread_h__
#define __xwos_up_thread_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/wqn.h>
#include <xwos/up/tt.h>
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
  #include <xwos/up/mutextree.h>
#endif /* XWUPCFG_LOCK_MTX */
#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
  #if !defined(XWUPCFG_SYNC_CDT) || (1 != XWUPCFG_SYNC_CDT)
    #error "XWUPCFG_SYNC_CDT must be 1 if (1 == XWUPCFG_SD_THRD_EXIT) !"
  #endif
#endif /* XWUPCFG_SD_THRD_EXIT */
#if defined(XWUPCFG_SYNC_CDT) && (1 == XWUPCFG_SYNC_CDT)
  #include <xwos/up/sync/condition.h>
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_scheduler;

#if (1 == SOUPRULE_SD_WQ_RT)
struct xwos_rtwq;
#endif /* (1 == SOUPRULE_SD_WQ_RT) */

#if (1 == SOUPRULE_SD_WQ_PL)
struct xwos_plwq;
#endif /* (1 == SOUPRULE_SD_WQ_PL) */

/**
 * @brief XWOS线程控制块
 */
struct xwos_tcb {
        struct xwos_sdobj_stack_info stack; /**< 栈 */
        struct xwlib_bclst_node tcbnode; /**< 调度器线程链表中的节点 */

        /**< 线程状态 */
        xwsq_t state; /**< 线程状态 */
        xwsq_t attribute; /**< 线程属性 */

#if (1 == SOUPRULE_SD_THRD_FREEZE)
        /* 冻结状态信息 */
        struct xwlib_bclst_node frznode; /**< 冻结链表节点 */
#endif /* (1 == SOUPRULE_SD_THRD_FREEZE) */

        /* 就绪态信息 */
        struct xwlib_bclst_node rqnode; /**< 就绪队列节点，
                                             此成员被锁rtrq->lock保护 */

        /* 睡眠状态信息 */
        struct xwos_ttn ttn; /**< 时间树节点, 被锁tt->lock保护 */

        /* 等待状态信息 */
        struct xwos_wqn wqn; /**< 等待队列 */

        /* 线程优先级信息 */
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
        struct xwos_mtxtree mtxtree; /**< 互斥锁树：线程已经获得的所有互斥锁都
                                          需要加入到互斥锁树 */
        struct {
                xwpr_t s; /**< 线程的固有优先级 */
                xwpr_t d; /**< 线程的动态优先级 */
        } prio; /**< 线程的优先级 */
#else /* XWUPCFG_LOCK_MTX */
        union {
                xwpr_t s; /**< 线程的固有优先级 */
                xwpr_t d; /**< 线程的动态优先级 */
        } prio; /**< 线程的优先级 */
#endif /* !XWUPCFG_LOCK_MTX */

#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
        /* thread completion */
        struct xwsync_cdt completion; /**< 线程退出时的事件信号量 */
#endif /* XWUPCFG_SD_THRD_EXIT */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX))
__xwos_code
void xwos_thrd_chprio_once(struct xwos_tcb * tcb, xwpr_t dprio,
                           struct xwlk_mtx ** pmtx);

__xwos_code
void xwos_thrd_chprio(struct xwos_tcb * tcb);
#endif /* XWUPCFG_LOCK_MTX */

__xwos_code
xwer_t xwos_thrd_exit_lic(struct xwos_tcb * tcb, xwer_t rc);

__xwos_code
xwer_t xwos_thrd_rq_add_head(struct xwos_tcb * tcb);

__xwos_code
xwer_t xwos_thrd_rq_add_tail(struct xwos_tcb * tcb);

__xwos_code
void xwos_thrd_rq_remove(struct xwos_tcb * tcb);

__xwos_code
void xwos_thrd_ttn_callback(void * entry);

__xwos_code
xwer_t xwos_thrd_tt_add_locked(struct xwos_tcb * tcb, struct xwos_tt * xwtt,
                               xwtm_t expected, xwreg_t flag);

__xwos_code
void xwos_thrd_wqn_callback(void * entry);

#if (1 == SOUPRULE_SD_WQ_RT)
__xwos_code
void xwos_thrd_eq_rtwq(struct xwos_tcb * tcb, struct xwos_rtwq * xwrtwq,
                       xwu16_t type);
#endif /* (1 == SOUPRULE_SD_WQ_RT) */

#if (1 == SOUPRULE_SD_WQ_PL)
__xwos_code
void xwos_thrd_eq_plwq(struct xwos_tcb * tcb, struct xwos_plwq * xwplwq,
                       xwu16_t type);
#endif /* (1 == SOUPRULE_SD_WQ_PL) */

#if (1 == SOUPRULE_SD_THRD_DO_UNLOCK)
__xwos_code
xwer_t xwos_thrd_do_unlock(void * lock, xwsq_t lktype, void * lkdata);
#endif /* (1 == SOUPRULE_SD_THRD_DO_UNLOCK) */

#if (1 == SOUPRULE_SD_THRD_DO_LOCK)
__xwos_code
xwer_t xwos_thrd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm,
                         void * lkdata);
#endif /* (1 == SOUPRULE_SD_THRD_DO_LOCK) */

__xwos_code
xwer_t xwos_thrd_freeze_lic(struct xwos_tcb * tcb);

__xwos_code
xwer_t xwos_thrd_thaw_lic(struct xwos_tcb * tcb);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 唤醒一个阻塞/睡眠态的线程，将其加入到调度器的就绪队列
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 */
static __xw_inline
void xwos_thrd_wakeup(struct xwos_tcb * tcb)
{
        xwos_thrd_rq_add_tail(tcb);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwos_thrd_init(struct xwos_tcb * tcb,
                      const char * name,
                      xwos_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr);

__xwos_api
xwer_t xwos_thrd_destroy(struct xwos_tcb * tcb);

__xwos_api
xwer_t xwos_thrd_create(struct xwos_tcb ** tcbpbuf,
                        const char * name,
                        xwos_thrd_f manfunc, void * arg,
                        xwsz_t stack_size, xwpr_t priority,
                        xwsq_t attr);

__xwos_api
xwer_t xwos_thrd_delete(struct xwos_tcb * tcb);

__xwos_api
void xwos_cthrd_yield(void);

__xwos_api
void xwos_cthrd_exit(xwer_t rc);

__xwos_api
xwer_t xwos_thrd_terminate(struct xwos_tcb * tcb, xwer_t * trc);

__xwos_api
void xwos_cthrd_wait_exit(void);

__xwos_api
xwer_t xwos_thrd_intr(struct xwos_tcb * tcb);

__xwos_api
xwer_t xwos_cthrd_sleep(xwtm_t * xwtm);

__xwos_api
xwer_t xwos_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc);

__xwos_api
xwer_t xwos_cthrd_freeze(void);

__xwos_api
bool xwos_cthrd_shld_frz(void);

__xwos_api
bool xwos_cthrd_frz_shld_stop(bool * frozen);

__xwos_api
bool xwos_cthrd_shld_stop(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwos/up/thread.h */
