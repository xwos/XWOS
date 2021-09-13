/**
 * @file
 * @brief 玄武OS UP内核同步机制：实时信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SYNC_RTSEM_STDC_MM) && (1 == XWUPCFG_SYNC_RTSEM_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#include <xwos/up/rtwq.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
#  include <xwos/up/sync/evt.h>
#endif
#include <xwos/up/sync/rtsem.h>

static __xwup_code
struct xwup_rtsem * xwup_rtsem_alloc(void);

static __xwup_code
void xwup_rtsem_free(struct xwup_rtsem * sem);

static __xwup_code
void xwup_rtsem_activate(struct xwup_rtsem * sem, xwssq_t val, xwssq_t max);

static __xwup_code
void xwup_rtsem_deactivate(struct xwup_rtsem * sem);

static __xwup_code
xwer_t xwup_rtsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                                 struct xwup_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t flag);

static __xwup_code
xwer_t xwup_rtsem_do_timedwait(struct xwup_rtsem * sem, struct xwup_thd * thd,
                               xwtm_t * xwtm);

static __xwup_code
xwer_t xwup_rtsem_do_blkthd_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                            struct xwup_thd * thd,
                                            xwreg_t flag);

static __xwup_code
xwer_t xwup_rtsem_do_wait_unintr(struct xwup_rtsem * sem, struct xwup_thd * thd);

/**
 * @brief 申请实时信号量对象
 * @return 信号量对象的指针
 */
static __xwup_code
struct xwup_rtsem * xwup_rtsem_alloc(void)
{
#if defined(XWUPCFG_SKD_RTSEM_STDC_MM) && (1 == XWUPCFG_SKD_RTSEM_STDC_MM)
        struct xwup_rtsem * rtsem;

        rtsem = malloc(sizeof(struct xwup_rtsem));
        if (NULL == rtsem) {
                rtsem = err_ptr(-ENOMEM);
        }
        return rtsem;
#else
        union {
                struct xwup_rtsem * sem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_rtsem), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.sem = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.sem;
#endif
}

/**
 * @brief 释放实时信号量对象
 * @param[in] sem: 信号量对象的指针
 */
static __xwup_code
void xwup_rtsem_free(struct xwup_rtsem * sem)
{
#if defined(XWUPCFG_SKD_RTSEM_STDC_MM) && (1 == XWUPCFG_SKD_RTSEM_STDC_MM)
        free(sem);
#else
        xwmm_kma_free(sem);
#endif
}

/**
 * @brief 激活实时信号量
 * @param[in] sem: 信号量对象的指针
 * @param[in] val: 信号量的初始值
 * @param[in] max: 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwup_code
void xwup_rtsem_activate(struct xwup_rtsem * sem, xwssq_t val, xwssq_t max)
{
        xwup_vsem_activate(&sem->vsem);
        xwup_rtwq_init(&sem->rtwq);
        sem->vsem.count = val;
        sem->vsem.max = max;
}

/**
 * @brief 使得实时信号量对象无效
 * @param[in] sem: 信号量对象的指针
 */
static __xwup_code
void xwup_rtsem_deactivate(struct xwup_rtsem * sem)
{
        XWOS_UNUSED(sem);
}

__xwup_api
xwer_t xwup_rtsem_init(struct xwup_rtsem * sem, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((NULL != sem), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        xwup_rtsem_activate(sem, val, max);
        return XWOK;
}

__xwup_api
xwer_t xwup_rtsem_fini(struct xwup_rtsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        xwup_rtsem_deactivate(sem);
        return XWOK;
}

__xwup_api
xwer_t xwup_rtsem_create(struct xwup_rtsem ** ptrbuf, xwssq_t val, xwssq_t max)
{
        struct xwup_rtsem * sem;
        xwer_t rc;

        XWOS_VALIDATE((NULL != ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        *ptrbuf = NULL;
        sem = xwup_rtsem_alloc();
        if (__xwcc_unlikely(is_err(sem))) {
                rc = ptr_err(sem);
        } else {
                xwup_rtsem_activate(sem, val, max);
                *ptrbuf = sem;
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_delete(struct xwup_rtsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        xwup_rtsem_fini(sem);
        xwup_rtsem_free(sem);
        return XWOK;
}

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
__xwup_api
xwer_t xwup_rtsem_bind(struct xwup_rtsem * sem, struct xwup_evt * evt,
                       xwsq_t pos)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwup_vsem_bind(&sem->vsem, evt, pos);
}

__xwup_api
xwer_t xwup_rtsem_unbind(struct xwup_rtsem * sem, struct xwup_evt * evt)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwup_vsem_unbind(&sem->vsem, evt);
}
#endif

/**
 * @brief 中断实时信号量等待队列中的一个节点
 * @param[in] sem: 信号量对象的指针
 * @param[in] wqn: 等待队列节点
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_code
xwer_t xwup_rtsem_intr(struct xwup_rtsem * sem, struct xwup_wqn * wqn)
{
        xwup_wqn_f cb;
        xwreg_t flag;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&flag);
        rc = xwup_rtwq_remove(&sem->rtwq, wqn);
        if (XWOK == rc) {
                wqn->wq = NULL;
                wqn->type = XWUP_WQTYPE_UNKNOWN;
                wqn->reason = XWUP_WQN_REASON_INTR;
                cb = wqn->cb;
                wqn->cb = NULL;
                xwospl_cpuirq_restore_lc(flag);
                cb(wqn->owner);
        } else {
                xwospl_cpuirq_restore_lc(flag);
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_post(struct xwup_rtsem * sem)
{
        struct xwup_wqn * wqn;
        xwup_wqn_f cb;
        xwreg_t flag;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        rc = XWOK;
        xwospl_cpuirq_save_lc(&flag);
        if (__xwcc_unlikely(sem->vsem.count < 0)) {
                xwospl_cpuirq_restore_lc(flag);
                rc = -ENEGATIVE;
        } else {
                wqn = xwup_rtwq_choose(&sem->rtwq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(flag);
                        cb(wqn->owner);
                } else {
                        /* add rtsem counter */
                        if (sem->vsem.count < sem->vsem.max) {
                                sem->vsem.count++;
                        } else {
                                rc = -ERANGE;
                        }
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                        if (sem->vsem.count > 0) {
                                struct xwup_evt * evt;

                                evt = sem->vsem.synobj.sel.evt;
                                if (NULL != evt) {
                                        xwup_sel_obj_s1i(evt, &sem->vsem.synobj);
                                }
                        }
#endif
                        xwospl_cpuirq_restore_lc(flag);
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_wait(struct xwup_rtsem * sem)
{
        xwtm_t expected = XWTM_MAX;
        return xwup_rtsem_timedwait(sem, &expected);
}

__xwup_api
xwer_t xwup_rtsem_trywait(struct xwup_rtsem * sem)
{
        xwer_t rc;
        xwreg_t flag;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        rc = XWOK;
        xwospl_cpuirq_save_lc(&flag);
        if (sem->vsem.count > 0) {
                sem->vsem.count--;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
        } else {
                rc = -ENODATA;
        }
        xwospl_cpuirq_restore_lc(flag);
        return rc;
}

static __xwup_code
xwer_t xwup_rtsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                                 struct xwup_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t flag)
{
        xwer_t rc;
        xwtm_t expected, currtick;
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;

        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        currtick = xwup_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);
        XWOS_BUG_ON((XWUP_SKDOBJ_DST_BLOCKING | XWUP_SKDOBJ_DST_SLEEPING |
                     XWUP_SKDOBJ_DST_READY | XWUP_SKDOBJ_DST_STANDBY |
                     XWUP_SKDOBJ_DST_FROZEN)
                    & thd->state);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
        xwup_thd_eq_rtwq(thd, &sem->rtwq, XWUP_WQTYPE_RTSEM);
        xwospl_cpuirq_restore_lc(flag);

        /* 加入时间树 */
        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
        xwup_thd_tt_add_locked(thd, xwtt, expected, flag);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_unlock();
#endif
        xwup_skd_req_swcx();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_lock();
#endif
        xwospl_cpuirq_restore_lc(flag);

        /* 判断唤醒原因 */
        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
                }/* else {} */
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);
                rc = -EINTR;
        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
                }/* else {} */
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);
                rc = XWOK;
        } else if (XWUP_TTN_WKUPRS_TIMEDOUT == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_rtwq_remove(&sem->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(flag);
                        rc = -ETIMEDOUT;
                } else {
                        xwospl_cpuirq_restore_lc(flag);
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
                rc = xwup_rtwq_remove(&sem->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(flag);
                        rc = -EINTR;
                } else {
                        xwospl_cpuirq_restore_lc(flag);
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
xwer_t xwup_rtsem_do_timedwait(struct xwup_rtsem * sem, struct xwup_thd * thd,
                               xwtm_t * xwtm)
{
        xwreg_t flag;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&flag);
        if (sem->vsem.count <= 0) {
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                rc = xwup_skd_wakelock_lock();
                if (__xwcc_unlikely(rc < 0)) {
                        /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR*/
                        xwospl_cpuirq_restore_lc(flag);
                        rc = -EINTR;
                } else {
#endif
                        rc = xwup_rtsem_do_timedblkthd_unlkwq_cpuirqrs(sem, thd,
                                                                       xwtm, flag);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                        xwup_skd_wakelock_unlock();
                }
#endif
        } else {
                sem->vsem.count--;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
                xwospl_cpuirq_restore_lc(flag);
                rc = XWOK;
        }

        return rc;
}

__xwup_api
xwer_t xwup_rtsem_timedwait(struct xwup_rtsem * sem, xwtm_t * xwtm)
{
        struct xwup_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwup_rtsem_trywait(sem);
                if (__xwcc_unlikely(rc < 0)) {
                        if (__xwcc_likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        }/* else {} */
                }/* else {} */
        } else {
                cthd = xwup_skd_get_cthd_lc();
                rc = xwup_rtsem_do_timedwait(sem, cthd, xwtm);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_rtsem_do_blkthd_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                            struct xwup_thd * thd,
                                            xwreg_t flag)
{
        xwer_t rc;

        XWOS_BUG_ON((XWUP_SKDOBJ_DST_BLOCKING | XWUP_SKDOBJ_DST_SLEEPING |
                     XWUP_SKDOBJ_DST_READY | XWUP_SKDOBJ_DST_STANDBY |
                     XWUP_SKDOBJ_DST_FROZEN)
                    & thd->state);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_DST_BLOCKING | XWUP_SKDOBJ_DST_UNINTERRUPTED);
        xwup_thd_eq_rtwq(thd, &sem->rtwq, XWUP_WQTYPE_RTSEM);
        xwospl_cpuirq_restore_lc(flag);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwup_skd_req_swcx();
        xwospl_cpuirq_restore_lc(flag);

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
xwer_t xwup_rtsem_do_wait_unintr(struct xwup_rtsem * sem, struct xwup_thd * thd)
{
        xwreg_t flag;
        xwer_t rc;

        /* test the rtsem */
        xwospl_cpuirq_save_lc(&flag);
        if (sem->vsem.count <= 0) {
                rc = xwup_rtsem_do_blkthd_unlkwq_cpuirqrs(sem, thd, flag);
        } else {
                sem->vsem.count--;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
                xwospl_cpuirq_restore_lc(flag);
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_wait_unintr(struct xwup_rtsem * sem)
{
        struct xwup_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        cthd = xwup_skd_get_cthd_lc();
        rc = xwup_rtsem_do_wait_unintr(sem, cthd);
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_freeze(struct xwup_rtsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwup_vsem_freeze(&sem->vsem);
}

__xwup_api
xwer_t xwup_rtsem_thaw(struct xwup_rtsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwup_vsem_thaw(&sem->vsem);
}

__xwup_api
xwer_t xwup_rtsem_getvalue(struct xwup_rtsem * sem, xwssq_t * sval)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((sval), "nullptr", -EFAULT);

        return xwup_vsem_getvalue(&sem->vsem, sval);
}
