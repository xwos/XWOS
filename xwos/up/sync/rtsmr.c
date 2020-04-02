/**
 * @file
 * @brief XuanWuOS的同步机制：实时信号量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/irq.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/tt.h>
#include <xwos/up/thread.h>
#include <xwos/up/rtwq.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
  #include <xwos/up/sync/event.h>
#endif /* XWUPCFG_SYNC_EVT */
#include <xwos/up/sync/rtsmr.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
struct xwsync_rtsmr * xwsync_rtsmr_alloc(void);

static __xwos_code
void xwsync_rtsmr_free(struct xwsync_rtsmr * smr);

static __xwos_code
void xwsync_rtsmr_activate(struct xwsync_rtsmr * smr, xwssq_t val, xwssq_t max);

static __xwos_code
void xwsync_rtsmr_deactivate(struct xwsync_rtsmr * smr);

static __xwos_code
xwer_t xwsync_rtsmr_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_rtsmr * smr,
                                                    struct xwos_tcb * tcb,
                                                    xwtm_t * xwtm,
                                                    xwreg_t flag);

static __xwos_code
xwer_t xwsync_rtsmr_do_timedwait(struct xwsync_rtsmr * smr, struct xwos_tcb * tcb,
                                 xwtm_t * xwtm);

static __xwos_code
xwer_t xwsync_rtsmr_do_blkthrd_unlkwq_cpuirqrs(struct xwsync_rtsmr * smr,
                                               struct xwos_tcb * tcb,
                                               xwreg_t flag);

static __xwos_code
xwer_t xwsync_rtsmr_do_wait_unintr(struct xwsync_rtsmr * smr, struct xwos_tcb * tcb);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 申请实时信号量对象
 * @return 信号量对象的指针
 */
static __xwos_code
struct xwsync_rtsmr * xwsync_rtsmr_alloc(void)
{
        union {
                struct xwsync_rtsmr * smr;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwsync_rtsmr), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.smr = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.smr;
}

/**
 * @brief 释放实时信号量对象
 * @param smr: (I) 信号量对象的指针
 */
static __xwos_code
void xwsync_rtsmr_free(struct xwsync_rtsmr * smr)
{
        xwmm_kma_free(smr);
}

/**
 * @brief 激活实时信号量
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_code
void xwsync_rtsmr_activate(struct xwsync_rtsmr * smr, xwssq_t val, xwssq_t max)
{
        xwsync_vsmr_activate(&smr->vsmr);
        xwos_rtwq_init(&smr->rtwq);
        smr->vsmr.count = val;
        smr->vsmr.max = max;
}

/**
 * @brief 使得实时信号量对象无效
 * @param smr: (I) 信号量对象的指针
 */
static __xwos_code
void xwsync_rtsmr_deactivate(struct xwsync_rtsmr * smr)
{
        XWOS_UNUSED(smr);
}

/**
 * @brief XWOS API：初始化实时信号量
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_rtsmr_init(struct xwsync_rtsmr * smr, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((NULL != smr), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        xwsync_rtsmr_activate(smr, val, max);
        return OK;
}

/**
 * @brief XWOS API：销毁实时信号量对象
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_rtsmr_destroy(struct xwsync_rtsmr * smr)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        xwsync_rtsmr_deactivate(smr);
        return OK;
}

/**
 * @brief XWOS API：动态创建实时信号量
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象的指针
 * @param type: (I) 信号量的类型
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_rtsmr_create(struct xwsync_rtsmr ** ptrbuf, xwssq_t val, xwssq_t max)
{
        struct xwsync_rtsmr * smr;
        xwer_t rc;

        XWOS_VALIDATE((NULL != ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        *ptrbuf = NULL;
        smr = xwsync_rtsmr_alloc();
        if (__unlikely(is_err(smr))) {
                rc = ptr_err(smr);
        } else {
                xwsync_rtsmr_activate(smr, val, max);
                *ptrbuf = smr;
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的实时信号量
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
__xwos_api
xwer_t xwsync_rtsmr_delete(struct xwsync_rtsmr * smr)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        xwsync_rtsmr_destroy(smr);
        xwsync_rtsmr_free(smr);
        return OK;
}

/**
 * @brief 中断实时信号量等待队列中的一个节点
 * @param smr: (I) 信号量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_code
xwer_t xwsync_rtsmr_intr(struct xwsync_rtsmr * smr, struct xwos_wqn * wqn)
{
        xwos_wqn_f cb;
        xwreg_t flag;
        xwer_t rc;

        xwos_cpuirq_save_lc(&flag);
        rc = xwos_rtwq_remove(&smr->rtwq, wqn);
        if (OK == rc) {
                wqn->wq = NULL;
                wqn->type = XWOS_WQTYPE_UNKNOWN;
                wqn->rsmrs = XWOS_WQN_RSMRS_INTR;
                cb = wqn->cb;
                wqn->cb = NULL;
                xwos_cpuirq_restore_lc(flag);
                cb(wqn->owner);
        } else {
                xwos_cpuirq_restore_lc(flag);
        }
        return rc;
}

/**
 * @brief XWOS API：发布实时信号量
 * @param smr: (I) 实时信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 信号量已被冻结
 * @retval -ERANGE: 信号量的值已经最大
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对非已冻结的信号量起作用，已冻结的信号量将得到错误码-ENEGATIVE。
 */
__xwos_api
xwer_t xwsync_rtsmr_post(struct xwsync_rtsmr * smr)
{
        struct xwos_wqn * wqn;
        xwos_wqn_f cb;
        xwreg_t flag;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        rc = OK;
        xwos_cpuirq_save_lc(&flag);
        if (__unlikely(smr->vsmr.count < 0)) {
                xwos_cpuirq_restore_lc(flag);
                rc = -ENEGATIVE;
        } else {
                wqn = xwos_rtwq_choose(&smr->rtwq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        wqn->rsmrs = XWOS_WQN_RSMRS_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwos_cpuirq_restore_lc(flag);
                        cb(wqn->owner);
                } else {
                        /* add rtsmr counter */
                        if (smr->vsmr.count < smr->vsmr.max) {
                                smr->vsmr.count++;
                        } else {
                                rc = -ERANGE;
                        }
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                        if (smr->vsmr.count > 0) {
                                struct xwsync_evt * evt;

                                evt = smr->vsmr.xwsyncobj.selector.evt;
                                if (NULL != evt) {
                                        xwsync_evt_obj_s1i(evt, &smr->vsmr.xwsyncobj);
                                }
                        }
#endif /* XWUPCFG_SYNC_EVT */
                        xwos_cpuirq_restore_lc(flag);
                }
        }
        return rc;
}

/**
 * @brief XWOS API：尝试获取实时信号量
 * @param smr: (I) 信号量对象的指针
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 信号量资源不可用（信号量不可被获取）
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数不会阻塞调用者，此函数可在中断上下文中使用。
 */
__xwos_api
xwer_t xwsync_rtsmr_trywait(struct xwsync_rtsmr * smr)
{
        xwer_t rc;
        xwreg_t flag;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        rc = OK;
        xwos_cpuirq_save_lc(&flag);
        if (smr->vsmr.count > 0) {
                smr->vsmr.count--;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (0 == smr->vsmr.count) {
                        struct xwsync_evt * evt;

                        evt = smr->vsmr.xwsyncobj.selector.evt;
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, &smr->vsmr.xwsyncobj);
                        }
                }
#endif /* XWUPCFG_SYNC_EVT */
        } else {
                rc = -ENODATA;
        }
        xwos_cpuirq_restore_lc(flag);
        return rc;
}

static __xwos_code
xwer_t xwsync_rtsmr_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_rtsmr * smr,
                                                    struct xwos_tcb * tcb,
                                                    xwtm_t * xwtm,
                                                    xwreg_t flag)
{
        xwer_t rc;
        xwtm_t expected, currtick;
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;

        xwsd = xwos_scheduler_get_lc();
        xwtt = &xwsd->tt;
        hwt = &xwtt->hwt;
        currtick = xwos_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);
        XWOS_BUG_ON((XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_SLEEPING |
                     XWSDOBJ_DST_READY | XWSDOBJ_DST_STANDBY |
                     XWSDOBJ_DST_FROZEN)
                    & tcb->state);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
        xwos_thrd_eq_rtwq(tcb, &smr->rtwq, XWOS_WQTYPE_RTSMR);
        xwos_cpuirq_restore_lc(flag);

        /* 加入时间树 */
        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
        xwos_thrd_tt_add_locked(tcb, xwtt, expected, flag);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);

        /* 调度 */
        xwos_cpuirq_enable_lc();
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
        xwos_scheduler_wakelock_unlock();
#endif /* XWUPCFG_SD_LPM */
        xwos_scheduler_req_swcx();
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
        xwos_scheduler_wakelock_lock();
#endif /* XWUPCFG_SD_LPM */
        xwos_cpuirq_restore_lc(flag);

        /* 判断唤醒原因 */
        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);
                rc = -EINTR;
        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_TIMEDOUT == tcb->ttn.wkuprs) {
                xwos_cpuirq_disable_lc();
                rc = xwos_rtwq_remove(&smr->rtwq, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        xwos_cpuirq_restore_lc(flag);
                        rc = -ETIMEDOUT;
                } else {
                        xwos_cpuirq_restore_lc(flag);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                rc = OK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWOS_TTN_WKUPRS_INTR == tcb->ttn.wkuprs) {
                xwos_cpuirq_disable_lc();
                rc = xwos_rtwq_remove(&smr->rtwq, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        xwos_cpuirq_restore_lc(flag);
                        rc = -EINTR;
                } else {
                        xwos_cpuirq_restore_lc(flag);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                rc = OK;
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
xwer_t xwsync_rtsmr_do_timedwait(struct xwsync_rtsmr * smr, struct xwos_tcb * tcb,
                                 xwtm_t * xwtm)
{
        xwreg_t flag;
        xwer_t rc;

        xwos_cpuirq_save_lc(&flag);
        if (smr->vsmr.count <= 0) {
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
                rc = xwos_scheduler_wakelock_lock();
                if (__unlikely(rc < 0)) {
                        /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR*/
                        xwos_cpuirq_restore_lc(flag);
                        rc = -EINTR;
                } else {
#endif /* XWUPCFG_SD_LPM */
                        rc = xwsync_rtsmr_do_timedblkthrd_unlkwq_cpuirqrs(smr,
                                                                          tcb,
                                                                          xwtm,
                                                                          flag);
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
                        xwos_scheduler_wakelock_unlock();
                }
#endif /* XWUPCFG_SD_LPM */
        } else {
                smr->vsmr.count--;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (0 == smr->vsmr.count) {
                        struct xwsync_evt * evt;

                        evt = smr->vsmr.xwsyncobj.selector.evt;
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, &smr->vsmr.xwsyncobj);
                        }
                }
#endif /* XWUPCFG_SYNC_EVT */
                xwos_cpuirq_restore_lc(flag);
                rc = OK;
        }

        return rc;
}

/**
 * @brief XWOS API：限时等待获取管道信号量
 * @param smr: (I) 信号量对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwos_api
xwer_t xwsync_rtsmr_timedwait(struct xwsync_rtsmr * smr, xwtm_t * xwtm)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        if (__unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwsync_rtsmr_trywait(smr);
                if (__unlikely(rc < 0)) {
                        if (__likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        }/* else {} */
                }/* else {} */
        } else {
                ctcb = xwos_scheduler_get_ctcb_lc();
                rc = xwsync_rtsmr_do_timedwait(smr, ctcb, xwtm);
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_rtsmr_do_blkthrd_unlkwq_cpuirqrs(struct xwsync_rtsmr * smr,
                                               struct xwos_tcb * tcb,
                                               xwreg_t flag)
{
        xwer_t rc;

        XWOS_BUG_ON((XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_SLEEPING |
                     XWSDOBJ_DST_READY | XWSDOBJ_DST_STANDBY |
                     XWSDOBJ_DST_FROZEN)
                    & tcb->state);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &tcb->state,
                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
        xwos_thrd_eq_rtwq(tcb, &smr->rtwq, XWOS_WQTYPE_RTSMR);
        xwos_cpuirq_restore_lc(flag);

        /* 调度 */
        xwos_cpuirq_enable_lc();
        xwos_scheduler_req_swcx();
        xwos_cpuirq_restore_lc(flag);

        /* 判断唤醒原因 */
        if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                rc = OK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwos_code
xwer_t xwsync_rtsmr_do_wait_unintr(struct xwsync_rtsmr * smr, struct xwos_tcb * tcb)
{
        xwreg_t flag;
        xwer_t rc;

        /* test the rtsmr */
        xwos_cpuirq_save_lc(&flag);
        if (smr->vsmr.count <= 0) {
                rc = xwsync_rtsmr_do_blkthrd_unlkwq_cpuirqrs(smr, tcb, flag);
        } else {
                smr->vsmr.count--;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (0 == smr->vsmr.count) {
                        struct xwsync_evt * evt;

                        evt = smr->vsmr.xwsyncobj.selector.evt;
                        if (NULL != evt) {
                                xwsync_evt_obj_c0i(evt, &smr->vsmr.xwsyncobj);
                        }
                }
#endif /* XWUPCFG_SYNC_EVT */
                xwos_cpuirq_restore_lc(flag);
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：等待并获取实时信号量，且等待不可被中断
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_rtsmr_wait_unintr(struct xwsync_rtsmr * smr)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        ctcb = xwos_scheduler_get_ctcb_lc();
        rc = xwsync_rtsmr_do_wait_unintr(smr, ctcb);
        return rc;
}
