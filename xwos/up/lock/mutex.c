/**
 * @file
 * @brief XuanWuOS的锁机制：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/up/irq.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/tt.h>
#include <xwos/up/rtwq.h>
#include <xwos/up/mutextree.h>
#include <xwos/up/thread.h>
#include <xwos/mm/kma.h>
#include <xwos/up/lock/mutex.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
struct xwlk_mtx * xwlk_mtx_alloc(void);

static __xwos_code
void xwlk_mtx_free(struct xwlk_mtx * mtx);

static __xwos_code
void xwlk_mtx_activate(struct xwlk_mtx * mtx, xwpr_t sprio);

static __xwos_code
void xwlk_mtx_deactivate(struct xwlk_mtx * mtx);

static __xwos_code
void xwlk_mtx_chprio_once(struct xwlk_mtx * mtx, struct xwos_tcb ** ptcb);

static __xwos_code
void xwlk_mtx_chprio(struct xwlk_mtx * mtx);

static __xwos_code
xwer_t xwlk_mtx_do_timedblkthrd_unlkwq_cpuirqrs(struct xwlk_mtx * mtx,
                                                struct xwos_scheduler * xwsd,
                                                struct xwos_tcb * tcb,
                                                xwtm_t * xwtm,
                                                xwreg_t cpuirq);

static __xwos_code
xwer_t xwlk_mtx_do_timedlock(struct xwlk_mtx * mtx, struct xwos_tcb * tcb,
                             xwtm_t * xwtm);

static __xwos_code
xwer_t xwlk_mtx_do_blkthrd_unlkwq_cpuirqrs(struct xwlk_mtx * mtx,
                                           struct xwos_tcb * tcb,
                                           xwreg_t cpuirq);

static __xwos_code
xwer_t xwlk_mtx_do_lock_unintr(struct xwlk_mtx * mtx, struct xwos_tcb * tcb);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 申请互斥锁对象
 * @return 互斥锁对象的指针
 */
static __xwos_code
struct xwlk_mtx * xwlk_mtx_alloc(void)
{
        union {
                struct xwlk_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwlk_mtx), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.mtx;
}

/**
 * @brief 释放互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwos_code
void xwlk_mtx_free(struct xwlk_mtx * mtx)
{
        xwmm_kma_free(mtx);
}

/**
 * @brief 激活互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 */
static __xwos_code
void xwlk_mtx_activate(struct xwlk_mtx * mtx, xwpr_t sprio)
{
        mtx->sprio = sprio;
        mtx->dprio = sprio;
        xwos_rtwq_init(&mtx->rtwq);
        mtx->ownertree = NULL;
        mtx->reentrant = 0;
        xwlib_rbtree_init_node(&mtx->rbnode);
        xwlib_bclst_init_node(&mtx->rbbuddy);
}

/**
 * @brief 使得互斥锁对象无效
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwos_code
void xwlk_mtx_deactivate(struct xwlk_mtx * mtx)
{
        XWOS_UNUSED(mtx);
}

/**
 * @brief XWOS API：初始化互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwos_api
xwer_t xwlk_mtx_init(struct xwlk_mtx * mtx, xwpr_t sprio)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((sprio >= XWOS_SD_PRIORITY_RT_MIN), "invalid-priority", -EINVAL);
        XWOS_VALIDATE((sprio <= XWOS_SD_PRIORITY_RT_MAX), "invalid-priority", -EINVAL);

        xwlk_mtx_activate(mtx, sprio);
        return XWOK;
}

/**
 * @brief XWOS API：销毁互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwos_api
xwer_t xwlk_mtx_destroy(struct xwlk_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        xwlk_mtx_deactivate(mtx);
        return XWOK;
}

/**
 * @brief XWOS API：动态创建互斥锁
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwlk_mtx_create(struct xwlk_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((sprio >= XWOS_SD_PRIORITY_RT_MIN), "invalid-priority", -EINVAL);
        XWOS_VALIDATE((sprio <= XWOS_SD_PRIORITY_RT_MAX), "invalid-priority", -EINVAL);

        *ptrbuf = NULL;
        mtx = xwlk_mtx_alloc();
        if (__xwcc_unlikely(is_err(mtx))) {
                rc = ptr_err(mtx);
        } else {
                xwlk_mtx_activate(mtx, sprio);
                *ptrbuf = mtx;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwos_api
xwer_t xwlk_mtx_delete(struct xwlk_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        xwlk_mtx_deactivate(mtx);
        xwlk_mtx_free(mtx);
        return XWOK;
}

/**
 * @brief 修改一次互斥锁的动态优先级
 * @param mtx: (I) 互斥锁对象的指针
 * @param ptcb: (O) 指向缓冲区的指针，
 *                  通过此缓冲区返回下一个需要修改动态优先级的线程控制块对象的指针
 * @return 错误码
 */
static __xwos_code
void xwlk_mtx_chprio_once(struct xwlk_mtx * mtx, struct xwos_tcb ** ptcb)
{
        struct xwos_tcb * owner;
        struct xwos_mtxtree * mt;
        xwreg_t cpuirq;

        *ptcb = NULL;
        xwos_cpuirq_save_lc(&cpuirq);
        if (mtx->dprio == mtx->rtwq.max_prio) {
                xwos_cpuirq_restore_lc(cpuirq);
        } else if (mtx->rtwq.max_prio <= mtx->sprio) {
                if (mtx->dprio == mtx->sprio) {
                        xwos_cpuirq_restore_lc(cpuirq);
                } else {
                        mtx->dprio = mtx->sprio;
                        mt = mtx->ownertree;
                        if (mt) {
                                owner = xwcc_baseof(mt, struct xwos_tcb, mtxtree);
                                xwos_mtxtree_remove(mt, mtx);
                                xwos_mtxtree_add(mt, mtx);
                                xwos_cpuirq_restore_lc(cpuirq);
                                *ptcb = owner;
                        } else {
                                xwos_cpuirq_restore_lc(cpuirq);
                        }
                }
        } else {
                mtx->dprio = mtx->rtwq.max_prio;
                mt = mtx->ownertree;
                if (mt) {
                        owner = xwcc_baseof(mt, struct xwos_tcb, mtxtree);
                        xwos_mtxtree_remove(mt, mtx);
                        xwos_mtxtree_add(mt, mtx);
                        xwos_cpuirq_restore_lc(cpuirq);
                        *ptcb = owner;
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                }
        }
}

/**
 * @brief 从互斥锁开始，修改互斥锁-线程链的动态优先级
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 */
static __xwos_code
void xwlk_mtx_chprio(struct xwlk_mtx * mtx)
{
        struct xwos_tcb * tcb;
        struct xwos_mtxtree * mt;
        xwpr_t dprio;
        xwreg_t cpuirq;

        while (mtx) {
                xwlk_mtx_chprio_once(mtx, &tcb);
                if (__xwcc_likely(NULL == tcb)) {
                        break;
                }/* else {} */
                mtx = NULL;
                mt = &tcb->mtxtree;
                xwos_cpuirq_save_lc(&cpuirq);
                dprio = tcb->prio.s > mt->maxprio ? tcb->prio.s : mt->maxprio;
                xwos_thrd_chprio_once(tcb, dprio, &mtx);
                xwos_cpuirq_restore_lc(cpuirq);
        }
}

/**
 * @brief 中断互斥锁等待队列中的一个节点
 * @param mtx: (I) 互斥锁对象的指针
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwlk_mtx_intr(struct xwlk_mtx * mtx, struct xwos_tcb * tcb)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_cpuirq_save_lc(&cpuirq);
        rc = xwos_rtwq_remove(&mtx->rtwq, &tcb->wqn);
        if (XWOK == rc) {
                tcb->wqn.wq = NULL;
                tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                tcb->wqn.cb = NULL;
                xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                xwos_thrd_wakeup(tcb);
                xwos_cpuirq_restore_lc(cpuirq);
                xwlk_mtx_chprio(mtx);
                xwos_scheduler_chkpmpt();
        } else {
                xwos_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

/**
 * @brief XWOS API：解锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOWNER: 线程并没有锁定此互斥锁
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwlk_mtx_unlock(struct xwlk_mtx * mtx)
{
        xwer_t rc;
        struct xwos_wqn * wqn;
        struct xwos_tcb * t, * ctcb;
        struct xwos_mtxtree * mt;
        xwreg_t cpuirq;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = XWOK;
        xwos_scheduler_dspmpt_lc();
        ctcb = xwos_scheduler_get_ctcb_lc();
        mt = &ctcb->mtxtree;
        xwos_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree != mt) {
                rc = -EOWNER;
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
        } else if (mtx->reentrant > 0) {
                mtx->reentrant--;
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
        } else {
                xwos_mtxtree_remove(mt, mtx);
                wqn = xwos_rtwq_choose(&mtx->rtwq);
                if (wqn) {
                        /* Case 1: 等待队列中有线程正在等待互斥锁 */
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        wqn->rsmrs = XWOS_WQN_RSMRS_UP;
                        wqn->cb = NULL;
                        t = wqn->owner;
                        xwbop_c0m(xwsq_t, &t->state,
                                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
                        mt = &t->mtxtree;
                        /* 修改互斥锁的优先级, 互斥锁此时没有拥有者 */
                        if (mtx->rtwq.max_prio <= mtx->sprio) {
                                mtx->dprio = mtx->sprio;
                        } else {
                                mtx->dprio = mtx->rtwq.max_prio;
                        }
                        xwos_mtxtree_add(mt, mtx);
                        t->prio.d = t->prio.s > mt->maxprio ? t->prio.s : mt->maxprio;
                        xwos_thrd_wakeup(t);
                        xwos_thrd_chprio(ctcb);
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_scheduler_enpmpt_lc();
                        /* 如果函数在xwsync_cdt_timedwait()中被调用，
                           当前线程已经不是`XWSDOBJ_DST_RUNNING'状态，
                           xwos_scheduler_chkpmpt()不起作用。 */
                        xwos_scheduler_chkpmpt();
                } else {
                        /* Case 2: 没有线程在等待互斥锁 */
                        xwos_thrd_chprio(ctcb);
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_scheduler_enpmpt_lc();
                        /* 如果函数在xwsync_cdt_timedwait()中被调用，
                           当前线程已经不是`XWSDOBJ_DST_RUNNING'状态，
                           xwos_scheduler_chkpmpt()不起作用。 */
                        xwos_scheduler_chkpmpt();
                }
        }
        return rc;
}

/**
 * @brief XWOS API：尝试获取互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 获取锁失败
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数虽然不会阻塞调用者，但也不可在中断上下文中使用，
 *   因为互斥锁需要记录自己的拥有者（线程控制块对象的指针），
 *   中断上下文不存在对应的线程控制块对象。
 */
__xwos_api
xwer_t xwlk_mtx_trylock(struct xwlk_mtx * mtx)
{
        struct xwos_tcb * ctcb;
        struct xwos_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = XWOK;
        xwos_scheduler_dspmpt_lc();
        ctcb = xwos_scheduler_get_ctcb_lc();
        mt = &ctcb->mtxtree;
        xwos_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
        } else if (mtx->ownertree) {
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -ENODATA;
                xwos_scheduler_enpmpt_lc();
        } else {
                xwos_mtxtree_add(mt, mtx);
                xwos_thrd_chprio(ctcb);
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
                xwos_scheduler_chkpmpt();
        }
        return rc;
}

static __xwos_code
xwer_t xwlk_mtx_do_timedblkthrd_unlkwq_cpuirqrs(struct xwlk_mtx * mtx,
                                                struct xwos_scheduler * xwsd,
                                                struct xwos_tcb * tcb,
                                                xwtm_t * xwtm,
                                                xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;
        xwtm_t expected, currtick;

        xwtt = &xwsd->tt;
        hwt = &xwtt->hwt;
        currtick = xwos_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
        xwos_thrd_eq_rtwq(tcb, &mtx->rtwq, XWOS_WQTYPE_MTX);
        xwos_cpuirq_restore_lc(cpuirq);
        xwlk_mtx_chprio(mtx);

        /* 加入到时间树 */
        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
        xwos_thrd_tt_add_locked(tcb, xwtt, expected, cpuirq);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwos_cpuirq_enable_lc();
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
        xwos_scheduler_wakelock_unlock();
#endif /* XWUPCFG_SD_PM */
        xwos_scheduler_enpmpt_lc();
        xwos_scheduler_req_swcx();
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
        xwos_scheduler_wakelock_lock();
#endif /* XWUPCFG_SD_PM */
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if (XWOS_TTN_WKUPRS_TIMEDOUT == tcb->ttn.wkuprs) {
                xwos_cpuirq_disable_lc();
                rc = xwos_rtwq_remove(&mtx->rtwq, &tcb->wqn);
                if (XWOK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwlk_mtx_chprio(mtx);
                        rc = -ETIMEDOUT;
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWOS_TTN_WKUPRS_INTR == tcb->ttn.wkuprs) {
                xwos_cpuirq_disable_lc();
                rc = xwos_rtwq_remove(&mtx->rtwq, &tcb->wqn);
                if (XWOK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwlk_mtx_chprio(mtx);
                        rc = -EINTR;
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        currtick = xwos_syshwt_get_timetick(hwt);
        *xwtm = xwtm_sub(expected, currtick);
        return rc;
}

static __xwos_code
xwer_t xwlk_mtx_do_timedlock(struct xwlk_mtx * mtx,
                             struct xwos_tcb * tcb,
                             xwtm_t * xwtm)
{
        struct xwos_scheduler * xwsd;
        struct xwos_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwsd = xwos_scheduler_dspmpt_lc();
        mt = &tcb->mtxtree;
        xwos_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
        } else if (mtx->ownertree) {
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
                rc = xwos_scheduler_wakelock_lock();
                if (__xwcc_unlikely(rc < 0)) {
                        /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR。*/
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_scheduler_enpmpt_lc();
                        rc = -EINTR;
                } else {
#endif /* XWUPCFG_SD_PM */
                        rc = xwlk_mtx_do_timedblkthrd_unlkwq_cpuirqrs(mtx, xwsd, tcb,
                                                                      xwtm, cpuirq);
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
                        xwos_scheduler_wakelock_unlock();
                }
#endif /* XWUPCFG_SD_PM */
        } else {
                xwos_mtxtree_add(mt, mtx);
                xwos_thrd_chprio(tcb);
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
                xwos_scheduler_chkpmpt();
        }
        return rc;
}

/**
 * @brief XWOS API：限时等待并上锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 如果线程没能获取互斥锁，会加入到互斥锁的等待队列中等待，
 *   等待超时后将以返回值-ETIMEDOUT返回。
 * - 指针参数xwtm有两个作用：
 *   + 调用函数时，作为输入，表示期望等待的时间；
 *   + 函数返回时，作为输出，表示期望等待时间的剩余值
 *     超时返回-ETIMEDOUT时，期望等待时间会减为0
 */
__xwos_api
xwer_t xwlk_mtx_timedlock(struct xwlk_mtx * mtx, xwtm_t * xwtm)
{
        xwer_t rc;
        struct xwos_tcb * ctcb;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        if (__xwcc_unlikely(xwtm_cmp(*xwtm, 0) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__xwcc_unlikely(0 == xwtm_cmp(* xwtm, 0))) {
                rc = xwlk_mtx_trylock(mtx);
                if (__xwcc_unlikely(rc < 0)) {
                        if (__xwcc_likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        }/* else {} */
                }/* else {} */
        } else {
                ctcb = xwos_scheduler_get_ctcb_lc();
                rc = xwlk_mtx_do_timedlock(mtx, ctcb, xwtm);
        }
        return rc;
}

static __xwos_code
xwer_t xwlk_mtx_do_blkthrd_unlkwq_cpuirqrs(struct xwlk_mtx * mtx,
                                           struct xwos_tcb * tcb,
                                           xwreg_t cpuirq)
{
        xwer_t rc;

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &tcb->state,
                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
        xwos_thrd_eq_rtwq(tcb, &mtx->rtwq, XWOS_WQTYPE_MTX);
        xwos_cpuirq_restore_lc(cpuirq);
        xwlk_mtx_chprio(mtx);

        /* 调度 */
        xwos_cpuirq_enable_lc();
        xwos_scheduler_enpmpt_lc();
        xwos_scheduler_req_swcx();
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwos_code
xwer_t xwlk_mtx_do_lock_unintr(struct xwlk_mtx * mtx,
                               struct xwos_tcb * tcb)
{
        struct xwos_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwos_scheduler_dspmpt_lc();
        mt = &tcb->mtxtree;
        xwos_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
        } else if (mtx->ownertree) {
                rc = xwlk_mtx_do_blkthrd_unlkwq_cpuirqrs(mtx, tcb, cpuirq);
        } else {
                xwos_mtxtree_add(mt, mtx);
                xwos_thrd_chprio(tcb);
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_enpmpt_lc();
                xwos_scheduler_chkpmpt();
        }
        return rc;
}

/**
 * @brief XWOS API：等待并上锁互斥锁，且等待不可被中断
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * -
 */
__xwos_api
xwer_t xwlk_mtx_lock_unintr(struct xwlk_mtx * mtx)
{
        xwer_t rc;
        struct xwos_tcb * ctcb;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        ctcb = xwos_scheduler_get_ctcb_lc();
        rc = xwlk_mtx_do_lock_unintr(mtx, ctcb);
        return rc;
}
