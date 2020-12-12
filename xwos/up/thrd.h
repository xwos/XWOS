/**
 * @file
 * @brief 玄武OS UP内核：线程
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_thrd_h__
#define __xwos_up_thrd_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/up/skd.h>
#include <xwos/up/wqn.h>
#include <xwos/up/tt.h>
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
  #include <xwos/up/mtxtree.h>
#endif /* XWUPCFG_LOCK_MTX */
#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
  #if !defined(XWUPCFG_SYNC_COND) || (1 != XWUPCFG_SYNC_COND)
    #error "XWUPCFG_SYNC_COND must be 1 if (1 == XWUPCFG_SKD_THRD_EXIT) !"
  #endif
#endif /* XWUPCFG_SKD_THRD_EXIT */
#if defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND)
  #include <xwos/up/sync/cond.h>
#endif

struct xwup_skd;

#if (1 == XWUPRULE_SKD_WQ_RT)
struct xwup_rtwq;
#endif /* (1 == XWUPRULE_SKD_WQ_RT) */

#if (1 == XWUPRULE_SKD_WQ_PL)
struct xwup_plwq;
#endif /* (1 == XWUPRULE_SKD_WQ_PL) */

/**
 * @brief XWOS UP线程控制块
 */
struct xwup_tcb {
        struct xwup_skd_stack_info stack; /**< 栈 */
        struct xwlib_bclst_node tcbnode; /**< 调度器线程链表中的节点 */

        xwsq_t state; /**< 线程状态 */
        xwsq_t attribute; /**< 线程属性 */

#if (1 == XWUPRULE_SKD_THRD_FREEZE)
        /* 冻结状态信息 */
        struct xwlib_bclst_node frznode; /**< 冻结链表节点 */
#endif /* (1 == XWUPRULE_SKD_THRD_FREEZE) */

        /* 就绪态信息 */
        struct xwlib_bclst_node rqnode; /**< 就绪队列节点，
                                             此成员被锁rtrq->lock保护 */

        /* 睡眠状态信息 */
        struct xwup_ttn ttn; /**< 时间树节点, 被锁tt->lock保护 */

        /* 等待状态信息 */
        struct xwup_wqn wqn; /**< 等待队列 */

        /* 线程优先级信息 */
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
        struct xwup_mtxtree mtxtree; /**< 互斥锁树：线程已经获得的所有互斥锁都
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

#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
        /* thread completion */
        struct xwup_cond completion; /**< 线程退出时的事件信号量 */
#endif /* XWUPCFG_SKD_THRD_EXIT */

#if defined(XWUPCFG_SKD_TCB_LOCAL_DATA_NUM) && (XWUPCFG_SKD_TCB_LOCAL_DATA_NUM > 0U)
        /* 线程本地数据 */
        void * data[XWUPCFG_SKD_TCB_LOCAL_DATA_NUM];
#endif /* XWUPCFG_SKD_TCB_LOCAL_DATA_NUM */
};

#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX))
void xwup_thrd_chprio_once(struct xwup_tcb * tcb, xwpr_t dprio,
                           struct xwup_mtx ** pmtx);
void xwup_thrd_chprio(struct xwup_tcb * tcb);
#endif /* XWUPCFG_LOCK_MTX */

xwer_t xwup_thrd_exit_lic(struct xwup_tcb * tcb, xwer_t rc);
xwer_t xwup_thrd_rq_add_head(struct xwup_tcb * tcb);
xwer_t xwup_thrd_rq_add_tail(struct xwup_tcb * tcb);
void xwup_thrd_rq_remove(struct xwup_tcb * tcb);
void xwup_thrd_ttn_callback(void * entry);
xwer_t xwup_thrd_tt_add_locked(struct xwup_tcb * tcb, struct xwup_tt * xwtt,
                               xwtm_t expected, xwreg_t flag);
void xwup_thrd_wqn_callback(void * entry);

#if (1 == XWUPRULE_SKD_WQ_RT)
void xwup_thrd_eq_rtwq(struct xwup_tcb * tcb, struct xwup_rtwq * xwrtwq,
                       xwu16_t type);
#endif /* (1 == XWUPRULE_SKD_WQ_RT) */

#if (1 == XWUPRULE_SKD_WQ_PL)
void xwup_thrd_eq_plwq(struct xwup_tcb * tcb, struct xwup_plwq * xwplwq,
                       xwu16_t type);
#endif /* (1 == XWUPRULE_SKD_WQ_PL) */

#if (1 == XWUPRULE_SKD_THRD_DO_UNLOCK)
xwer_t xwup_thrd_do_unlock(void * lock, xwsq_t lktype, void * lkdata);
#endif /* (1 == XWUPRULE_SKD_THRD_DO_UNLOCK) */

#if (1 == XWUPRULE_SKD_THRD_DO_LOCK)
xwer_t xwup_thrd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm,
                         void * lkdata);
#endif /* (1 == XWUPRULE_SKD_THRD_DO_LOCK) */

xwer_t xwup_thrd_freeze_lic(struct xwup_tcb * tcb);
xwer_t xwup_thrd_thaw_lic(struct xwup_tcb * tcb);

/**
 * @brief 唤醒一个阻塞/睡眠态的线程，将其加入到调度器的就绪队列
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 */
static __xwcc_inline
void xwup_thrd_wakeup(struct xwup_tcb * tcb)
{
        xwup_thrd_rq_add_tail(tcb);
}

xwer_t xwup_thrd_init(struct xwup_tcb * tcb,
                      const char * name,
                      xwup_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr);
xwer_t xwup_thrd_destroy(struct xwup_tcb * tcb);
xwer_t xwup_thrd_create(struct xwup_tcb ** tcbpbuf,
                        const char * name,
                        xwup_thrd_f manfunc, void * arg,
                        xwsz_t stack_size, xwpr_t priority,
                        xwsq_t attr);
xwer_t xwup_thrd_delete(struct xwup_tcb * tcb);
void xwup_cthrd_yield(void);
void xwup_cthrd_exit(xwer_t rc);
xwer_t xwup_thrd_stop(struct xwup_tcb * tcb, xwer_t * trc);
xwer_t xwup_thrd_cancel(struct xwup_tcb * tcb);
xwer_t xwup_thrd_join(struct xwup_tcb * tcb, xwer_t * trc);
xwer_t xwup_thrd_detach(struct xwup_tcb * tcb);
xwer_t xwup_thrd_intr(struct xwup_tcb * tcb);
xwer_t xwup_cthrd_sleep(xwtm_t * xwtm);
xwer_t xwup_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc);
xwer_t xwup_cthrd_freeze(void);
bool xwup_cthrd_shld_frz(void);
bool xwup_cthrd_frz_shld_stop(bool * frozen);
bool xwup_cthrd_shld_stop(void);

#if defined(XWUPCFG_SKD_TCB_LOCAL_DATA_NUM) && (XWUPCFG_SKD_TCB_LOCAL_DATA_NUM > 0U)
xwer_t xwup_thrd_set_data(struct xwup_tcb * tcb, xwsq_t pos, void * data);
xwer_t xwup_thrd_get_data(struct xwup_tcb * tcb, xwsq_t pos, void ** databuf);
xwer_t xwup_cthrd_set_data(xwsq_t pos, void * data);
xwer_t xwup_cthrd_get_data(xwsq_t pos, void ** databuf);
#endif /* XWUPCFG_SKD_TCB_LOCAL_DATA_NUM */

#endif /* xwos/up/thrd.h */
