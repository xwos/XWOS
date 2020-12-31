/**
 * @file
 * @brief 玄武OS UP内核锁机制：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/tt.h>
#include <xwos/up/rtwq.h>
#include <xwos/up/mtxtree.h>
#include <xwos/up/thd.h>
#include <xwos/mm/kma.h>
#include <xwos/up/lock/mtx.h>

static __xwup_code
struct xwup_mtx * xwup_mtx_alloc(void);

static __xwup_code
void xwup_mtx_free(struct xwup_mtx * mtx);

static __xwup_code
void xwup_mtx_activate(struct xwup_mtx * mtx, xwpr_t sprio);

static __xwup_code
void xwup_mtx_deactivate(struct xwup_mtx * mtx);

static __xwup_code
void xwup_mtx_chprio_once(struct xwup_mtx * mtx, struct xwup_thd ** pthd);

static __xwup_code
void xwup_mtx_chprio(struct xwup_mtx * mtx);

static __xwup_code
xwer_t xwup_mtx_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_mtx * mtx,
                                               struct xwup_skd * xwskd,
                                               struct xwup_thd * thd,
                                               xwtm_t * xwtm,
                                               xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_mtx_do_timedlock(struct xwup_mtx * mtx, struct xwup_thd * thd,
                             xwtm_t * xwtm);

static __xwup_code
xwer_t xwup_mtx_do_blkthd_unlkwq_cpuirqrs(struct xwup_mtx * mtx,
                                          struct xwup_thd * thd,
                                          xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_mtx_do_lock_unintr(struct xwup_mtx * mtx, struct xwup_thd * thd);

/**
 * @brief 申请互斥锁对象
 * @return 互斥锁对象的指针
 */
static __xwup_code
struct xwup_mtx * xwup_mtx_alloc(void)
{
        union {
                struct xwup_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_mtx), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.mtx;
}

/**
 * @brief 释放互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwup_code
void xwup_mtx_free(struct xwup_mtx * mtx)
{
        xwmm_kma_free(mtx);
}

/**
 * @brief 激活互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 */
static __xwup_code
void xwup_mtx_activate(struct xwup_mtx * mtx, xwpr_t sprio)
{
        if (sprio >= XWUP_SKD_PRIORITY_RT_NUM) {
                sprio = XWUP_SKD_PRIORITY_RT_MAX;
        } else if (sprio <= XWUP_SKD_PRIORITY_INVALID) {
                sprio = XWUP_SKD_PRIORITY_RT_MIN;
        }
        mtx->sprio = sprio;
        mtx->dprio = sprio;
        xwup_rtwq_init(&mtx->rtwq);
        mtx->ownertree = NULL;
        mtx->reentrant = 0;
        xwlib_rbtree_init_node(&mtx->rbnode);
        xwlib_bclst_init_node(&mtx->rbbuddy);
}

/**
 * @brief 使得互斥锁对象无效
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwup_code
void xwup_mtx_deactivate(struct xwup_mtx * mtx)
{
        XWOS_UNUSED(mtx);
}

/**
 * @brief XWUP API：初始化互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个互斥锁对象，不可重入
 */
__xwup_api
xwer_t xwup_mtx_init(struct xwup_mtx * mtx, xwpr_t sprio)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((sprio >= XWUP_SKD_PRIORITY_RT_MIN), "invalid-priority", -EINVAL);
        XWOS_VALIDATE((sprio <= XWUP_SKD_PRIORITY_RT_MAX), "invalid-priority", -EINVAL);

        xwup_mtx_activate(mtx, sprio);
        return XWOK;
}

/**
 * @brief XWUP API：销毁互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个互斥锁对象，不可重入
 */
__xwup_api
xwer_t xwup_mtx_destroy(struct xwup_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        xwup_mtx_deactivate(mtx);
        return XWOK;
}

/**
 * @brief XWUP API：动态创建互斥锁
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_mtx_create(struct xwup_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwup_mtx * mtx;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((sprio >= XWUP_SKD_PRIORITY_RT_MIN), "invalid-priority", -EINVAL);
        XWOS_VALIDATE((sprio <= XWUP_SKD_PRIORITY_RT_MAX), "invalid-priority", -EINVAL);

        *ptrbuf = NULL;
        mtx = xwup_mtx_alloc();
        if (__xwcc_unlikely(is_err(mtx))) {
                rc = ptr_err(mtx);
        } else {
                xwup_mtx_activate(mtx, sprio);
                *ptrbuf = mtx;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWUP API：删除动态创建的互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个互斥锁对象，不可重入
 */
__xwup_api
xwer_t xwup_mtx_delete(struct xwup_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        xwup_mtx_deactivate(mtx);
        xwup_mtx_free(mtx);
        return XWOK;
}

/**
 * @brief 修改一次互斥锁的动态优先级
 * @param mtx: (I) 互斥锁对象的指针
 * @param pthd: (O) 指向缓冲区的指针，
 *                  通过此缓冲区返回下一个需要修改动态优先级的线程控制块对象的指针
 * @return 错误码
 */
static __xwup_code
void xwup_mtx_chprio_once(struct xwup_mtx * mtx, struct xwup_thd ** pthd)
{
        struct xwup_thd * owner;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;

        *pthd = NULL;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->dprio == mtx->rtwq.max_prio) {
                xwospl_cpuirq_restore_lc(cpuirq);
        } else if (mtx->rtwq.max_prio <= mtx->sprio) {
                if (mtx->dprio == mtx->sprio) {
                        xwospl_cpuirq_restore_lc(cpuirq);
                } else {
                        mtx->dprio = mtx->sprio;
                        mt = mtx->ownertree;
                        if (mt) {
                                owner = xwcc_baseof(mt, struct xwup_thd, mtxtree);
                                xwup_mtxtree_remove(mt, mtx);
                                xwup_mtxtree_add(mt, mtx);
                                xwospl_cpuirq_restore_lc(cpuirq);
                                *pthd = owner;
                        } else {
                                xwospl_cpuirq_restore_lc(cpuirq);
                        }
                }
        } else {
                mtx->dprio = mtx->rtwq.max_prio;
                mt = mtx->ownertree;
                if (mt) {
                        owner = xwcc_baseof(mt, struct xwup_thd, mtxtree);
                        xwup_mtxtree_remove(mt, mtx);
                        xwup_mtxtree_add(mt, mtx);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        *pthd = owner;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        }
}

/**
 * @brief 从互斥锁开始，修改互斥锁-线程链的动态优先级
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 */
static __xwup_code
void xwup_mtx_chprio(struct xwup_mtx * mtx)
{
        struct xwup_thd * thd;
        struct xwup_mtxtree * mt;
        xwpr_t dprio;
        xwreg_t cpuirq;

        while (mtx) {
                xwup_mtx_chprio_once(mtx, &thd);
                if (__xwcc_likely(NULL == thd)) {
                        break;
                }/* else {} */
                mtx = NULL;
                mt = &thd->mtxtree;
                xwospl_cpuirq_save_lc(&cpuirq);
                dprio = thd->prio.s > mt->maxprio ? thd->prio.s : mt->maxprio;
                xwup_thd_chprio_once(thd, dprio, &mtx);
                xwospl_cpuirq_restore_lc(cpuirq);
        }
}

/**
 * @brief 中断互斥锁等待队列中的一个节点
 * @param mtx: (I) 互斥锁对象的指针
 * @param thd: (I) 线程控制块对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_code
xwer_t xwup_mtx_intr(struct xwup_mtx * mtx, struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
        if (XWOK == rc) {
                thd->wqn.wq = NULL;
                thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                thd->wqn.reason = XWUP_WQN_REASON_INTR;
                thd->wqn.cb = NULL;
                xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
                xwup_thd_wakeup(thd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_mtx_chprio(mtx);
                xwup_skd_chkpmpt();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

/**
 * @brief XWUP API：解锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOWNER: 线程并没有锁定此互斥锁
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_mtx_unlock(struct xwup_mtx * mtx)
{
        xwer_t rc;
        struct xwup_wqn * wqn;
        struct xwup_thd * t, * cthd;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = XWOK;
        xwup_skd_dspmpt_lc();
        cthd = xwup_skd_get_cthd_lc();
        mt = &cthd->mtxtree;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree != mt) {
                rc = -EOWNER;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
        } else if (mtx->reentrant > 0) {
                mtx->reentrant--;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
        } else {
                xwup_mtxtree_remove(mt, mtx);
                wqn = xwup_rtwq_choose(&mtx->rtwq);
                if (wqn) {
                        /* Case 1: 等待队列中有线程正在等待互斥锁 */
                        wqn->wq = NULL;
                        wqn->type = XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = XWUP_WQN_REASON_UP;
                        wqn->cb = NULL;
                        t = wqn->owner;
                        xwbop_c0m(xwsq_t, &t->state,
                                  XWUP_SKDOBJ_DST_BLOCKING |
                                  XWUP_SKDOBJ_DST_UNINTERRUPTED);
                        mt = &t->mtxtree;
                        /* 修改互斥锁的优先级, 互斥锁此时没有拥有者 */
                        if (mtx->rtwq.max_prio <= mtx->sprio) {
                                mtx->dprio = mtx->sprio;
                        } else {
                                mtx->dprio = mtx->rtwq.max_prio;
                        }
                        xwup_mtxtree_add(mt, mtx);
                        t->prio.d = t->prio.s > mt->maxprio ? t->prio.s : mt->maxprio;
                        xwup_thd_wakeup(t);
                        xwup_thd_chprio(cthd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_enpmpt_lc();
                        /* 如果函数在xwsync_cond_timedwait()中被调用，
                           当前线程已经不是`XWUP_SKDOBJ_DST_RUNNING'状态，
                           xwup_skd_chkpmpt()不起作用。 */
                        xwup_skd_chkpmpt();
                } else {
                        /* Case 2: 没有线程在等待互斥锁 */
                        xwup_thd_chprio(cthd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_enpmpt_lc();
                        /* 如果函数在xwsync_cond_timedwait()中被调用，
                           当前线程已经不是`XWUP_SKDOBJ_DST_RUNNING'状态，
                           xwup_skd_chkpmpt()不起作用。 */
                        xwup_skd_chkpmpt();
                }
        }
        return rc;
}

/**
 * @brief XWUP API：等待并上锁互斥锁
 * @param mtx: (I) 互斥锁对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_mtx_lock(struct xwup_mtx * mtx)
{
        xwtm_t expected;

        expected = XWTM_MAX;
        return xwup_mtx_timedlock(mtx, &expected);
}

/**
 * @brief XWUP API：尝试获取互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 获取锁失败
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数虽然不会阻塞调用者，但也不可在中断上下文中使用，
 *   因为互斥锁需要记录自己的拥有者（线程控制块对象的指针），
 *   中断上下文不存在对应的线程控制块对象。
 */
__xwup_api
xwer_t xwup_mtx_trylock(struct xwup_mtx * mtx)
{
        struct xwup_thd * cthd;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = XWOK;
        xwup_skd_dspmpt_lc();
        cthd = xwup_skd_get_cthd_lc();
        mt = &cthd->mtxtree;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
        } else if (mtx->ownertree) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -ENODATA;
                xwup_skd_enpmpt_lc();
        } else {
                xwup_mtxtree_add(mt, mtx);
                xwup_thd_chprio(cthd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
                xwup_skd_chkpmpt();
        }
        return rc;
}

static __xwup_code
xwer_t xwup_mtx_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_mtx * mtx,
                                               struct xwup_skd * xwskd,
                                               struct xwup_thd * thd,
                                               xwtm_t * xwtm,
                                               xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        xwtm_t expected, currtick;

        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        currtick = xwup_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
        xwup_thd_eq_rtwq(thd, &mtx->rtwq, XWUP_WQTYPE_MTX);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_mtx_chprio(mtx);

        /* 加入到时间树 */
        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
        xwup_thd_tt_add_locked(thd, xwtt, expected, cpuirq);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_unlock();
#endif /* XWUPCFG_SKD_PM */
        xwup_skd_enpmpt_lc();
        xwup_skd_req_swcx();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_lock();
#endif /* XWUPCFG_SKD_PM */
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
                }/* else {} */
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
                }/* else {} */
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if (XWUP_TTN_WKUPRS_TIMEDOUT == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_mtx_chprio(mtx);
                        rc = -ETIMEDOUT;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                                rc = -EINTR;
                        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWUP_TTN_WKUPRS_INTR == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_mtx_chprio(mtx);
                        rc = -EINTR;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                                rc = -EINTR;
                        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
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
        currtick = xwup_syshwt_get_timetick(hwt);
        *xwtm = xwtm_sub(expected, currtick);
        return rc;
}

static __xwup_code
xwer_t xwup_mtx_do_timedlock(struct xwup_mtx * mtx,
                             struct xwup_thd * thd,
                             xwtm_t * xwtm)
{
        struct xwup_skd * xwskd;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwskd = xwup_skd_dspmpt_lc();
        mt = &thd->mtxtree;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
        } else if (mtx->ownertree) {
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                rc = xwup_skd_wakelock_lock();
                if (__xwcc_unlikely(rc < 0)) {
                        /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR。*/
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_enpmpt_lc();
                        rc = -EINTR;
                } else {
#endif /* XWUPCFG_SKD_PM */
                        rc = xwup_mtx_do_timedblkthd_unlkwq_cpuirqrs(mtx, xwskd, thd,
                                                                     xwtm, cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                        xwup_skd_wakelock_unlock();
                }
#endif /* XWUPCFG_SKD_PM */
        } else {
                xwup_mtxtree_add(mt, mtx);
                xwup_thd_chprio(thd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
                xwup_skd_chkpmpt();
        }
        return rc;
}

/**
 * @brief XWUP API：限时等待并上锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTINTHD: 不在线程上下文中
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
__xwup_api
xwer_t xwup_mtx_timedlock(struct xwup_mtx * mtx, xwtm_t * xwtm)
{
        xwer_t rc;
        struct xwup_thd * cthd;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (__xwcc_unlikely(xwtm_cmp(*xwtm, 0) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__xwcc_unlikely(0 == xwtm_cmp(* xwtm, 0))) {
                rc = xwup_mtx_trylock(mtx);
                if (__xwcc_unlikely(rc < 0)) {
                        if (__xwcc_likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        }/* else {} */
                }/* else {} */
        } else {
                cthd = xwup_skd_get_cthd_lc();
                rc = xwup_mtx_do_timedlock(mtx, cthd, xwtm);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_mtx_do_blkthd_unlkwq_cpuirqrs(struct xwup_mtx * mtx,
                                          struct xwup_thd * thd,
                                          xwreg_t cpuirq)
{
        xwer_t rc;

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_DST_BLOCKING | XWUP_SKDOBJ_DST_UNINTERRUPTED);
        xwup_thd_eq_rtwq(thd, &mtx->rtwq, XWUP_WQTYPE_MTX);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_mtx_chprio(mtx);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwup_skd_enpmpt_lc();
        xwup_skd_req_swcx();
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwup_code
xwer_t xwup_mtx_do_lock_unintr(struct xwup_mtx * mtx,
                               struct xwup_thd * thd)
{
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwup_skd_dspmpt_lc();
        mt = &thd->mtxtree;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
        } else if (mtx->ownertree) {
                rc = xwup_mtx_do_blkthd_unlkwq_cpuirqrs(mtx, thd, cpuirq);
        } else {
                xwup_mtxtree_add(mt, mtx);
                xwup_thd_chprio(thd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc();
                xwup_skd_chkpmpt();
        }
        return rc;
}

/**
 * @brief XWUP API：等待并上锁互斥锁，且等待不可被中断
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * -
 */
__xwup_api
xwer_t xwup_mtx_lock_unintr(struct xwup_mtx * mtx)
{
        xwer_t rc;
        struct xwup_thd * cthd;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        cthd = xwup_skd_get_cthd_lc();
        rc = xwup_mtx_do_lock_unintr(mtx, cthd);
        return rc;
}

/**
 * @brief XWUP API：获取锁的状态
 * @param mtx: (I) 互斥锁对象的指针
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_mtx_getlkst(struct xwup_mtx * mtx, xwsq_t * lkst)
{
        xwer_t rc;
        volatile struct xwup_mtxtree * ownertree;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);

        xwmb_read(struct xwup_mtxtree *, ownertree, &mtx->ownertree);
        if (ownertree) {
                *lkst = XWOS_LKST_LOCKED;
        } else {
                *lkst = XWOS_LKST_UNLOCKED;
        }
        return XWOK;
}