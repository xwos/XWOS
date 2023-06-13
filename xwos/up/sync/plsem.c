/**
 * @file
 * @brief 玄武OS UP内核同步机制：管道信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWOSCFG_SYNC_SEM_STDC_MM) && (1 == XWOSCFG_SYNC_SEM_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#include <xwos/up/plwq.h>
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
#  include <xwos/up/sync/evt.h>
#endif
#include <xwos/up/sync/plsem.h>

static __xwup_code
struct xwup_plsem * xwup_plsem_alloc(void);

static __xwup_code
void xwup_plsem_free(struct xwup_plsem * sem);

static __xwup_code
void xwup_plsem_construct(struct xwup_plsem * sem);

static __xwup_code
void xwup_plsem_destruct(struct xwup_plsem * sem);

static __xwup_code
xwer_t xwup_plsem_sgc(void * sem);

static __xwup_code
xwer_t xwup_plsem_dgc(void * sem);

static __xwup_code
xwer_t xwup_plsem_activate(struct xwup_plsem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc);

static __xwup_code
xwer_t xwup_plsem_blkthd_to_unlkwq_cpuirqrs(struct xwup_plsem * sem,
                                            struct xwup_skd * xwskd,
                                            struct xwup_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_plsem_test(struct xwup_plsem * sem,
                       struct xwup_skd * xwskd, struct xwup_thd * thd,
                       xwtm_t to);

static __xwup_code
xwer_t xwup_plsem_blkthd_unlkwq_cpuirqrs(struct xwup_plsem * sem,
                                         struct xwup_thd * thd,
                                         xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_plsem_test_unintr(struct xwup_plsem * sem, struct xwup_thd * thd);

/**
 * @brief 申请管道信号量对象
 * @return 信号量对象的指针
 */
static __xwup_code
struct xwup_plsem * xwup_plsem_alloc(void)
{
#if defined(XWOSCFG_SKD_PLSEM_STDC_MM) && (1 == XWOSCFG_SKD_PLSEM_STDC_MM)
        struct xwup_plsem * plsem;

        plsem = malloc(sizeof(struct xwup_plsem));
        if (NULL == plsem) {
                plsem = err_ptr(-ENOMEM);
        } else {
                xwup_plsem_construct(plsem);
        }
        return plsem;
#else
        union {
                struct xwup_plsem * sem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_plsem), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.sem = err_ptr(-ENOMEM);
        } else {
                xwup_plsem_construct(mem.sem);
        }
        return mem.sem;
#endif
}

/**
 * @brief 释放管道信号量对象
 * @param[in] sem: 信号量对象的指针
 */
static __xwup_code
void xwup_plsem_free(struct xwup_plsem * sem)
{
#if defined(XWOSCFG_SKD_PLSEM_STDC_MM) && (1 == XWOSCFG_SKD_PLSEM_STDC_MM)
        free(sem);
#else
        xwmm_kma_free(sem);
#endif
}

static __xwup_code
void xwup_plsem_construct(struct xwup_plsem * sem)
{
        xwup_vsem_construct(&sem->vsem);
}

static __xwup_code
void xwup_plsem_destruct(struct xwup_plsem * sem)
{
        xwup_vsem_destruct(&sem->vsem);
}

static __xwup_code
xwer_t xwup_plsem_sgc(void * sem)
{
        xwup_plsem_destruct((struct xwup_plsem *)sem);
        return XWOK;
}

static __xwup_code
xwer_t xwup_plsem_dgc(void * sem)
{
        xwup_plsem_destruct((struct xwup_plsem *)sem);
        xwup_plsem_free((struct xwup_plsem *)sem);
        return XWOK;
}

__xwup_api
xwer_t xwup_plsem_acquire(struct xwup_plsem * sem, xwsq_t tik)
{
        return xwup_vsem_acquire(&sem->vsem, tik);
}

__xwup_api
xwer_t xwup_plsem_release(struct xwup_plsem * sem, xwsq_t tik)
{
        return xwup_vsem_release(&sem->vsem, tik);
}

__xwup_api
xwer_t xwup_plsem_grab(struct xwup_plsem * sem)
{
        return xwup_vsem_grab(&sem->vsem);
}

__xwup_api
xwer_t xwup_plsem_put(struct xwup_plsem * sem)
{
        return xwup_vsem_put(&sem->vsem);
}

static __xwup_code
xwer_t xwup_plsem_activate(struct xwup_plsem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwup_vsem_activate(&sem->vsem, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_vsem_activate;
        }
        xwup_plwq_init(&sem->plwq);
        sem->vsem.count = val;
        sem->vsem.max = max;
        return XWOK;

err_vsem_activate:
        return rc;
}

__xwup_api
xwer_t xwup_plsem_init(struct xwup_plsem * sem, xwssq_t val, xwssq_t max)
{
        xwup_plsem_construct(sem);
        xwup_plsem_activate(sem, val, max, xwup_plsem_sgc);
        return XWOK;
}

__xwup_api
xwer_t xwup_plsem_fini(struct xwup_plsem * sem)
{
        return xwup_plsem_put(sem);
}

__xwup_api
xwer_t xwup_plsem_create(struct xwup_plsem ** ptrbuf, xwssq_t val, xwssq_t max)
{
        struct xwup_plsem * sem;
        xwer_t rc;

        *ptrbuf = NULL;
        sem = xwup_plsem_alloc();
        if (__xwcc_unlikely(is_err(sem))) {
                rc = ptr_err(sem);
        } else {
                rc = xwup_plsem_activate(sem, val, max, xwup_plsem_dgc);
                if (__xwcc_unlikely(rc < 0)) {
                        xwup_plsem_free(sem);
                } else {
                        *ptrbuf = sem;
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_plsem_delete(struct xwup_plsem * sem, xwsq_t tik)
{
        return xwup_plsem_release(sem, tik);
}

#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
__xwup_api
xwer_t xwup_plsem_bind(struct xwup_plsem * sem, struct xwup_evt * evt,
                       xwsq_t pos)
{
        return xwup_vsem_bind(&sem->vsem, evt, pos);
}

__xwup_api
xwer_t xwup_plsem_unbind(struct xwup_plsem * sem, struct xwup_evt * evt)
{
        return xwup_vsem_unbind(&sem->vsem, evt);
}
#endif

/**
 * @brief 中断管道信号量等待队列中的一个节点
 * @param[in] sem: 信号量对象的指针
 * @param[in] wqn: 等待队列节点
 * @return 错误码
 * @note
 * + 上下文：任意
 */
__xwup_code
xwer_t xwup_plsem_intr(struct xwup_plsem * sem, struct xwup_wqn * wqn)
{
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_plwq_remove(&sem->plwq, wqn);
        if (XWOK == rc) {
                wqn->wq = NULL;
                wqn->type = XWUP_WQTYPE_UNKNOWN;
                wqn->reason = XWUP_WQN_REASON_INTR;
                cb = wqn->cb;
                wqn->cb = NULL;
                xwospl_cpuirq_restore_lc(cpuirq);
                cb(wqn->owner);
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

__xwup_api
xwer_t xwup_plsem_freeze(struct xwup_plsem * sem)
{
        return xwup_vsem_freeze(&sem->vsem);
}

__xwup_api
xwer_t xwup_plsem_thaw(struct xwup_plsem * sem)
{
        return xwup_vsem_thaw(&sem->vsem);
}

__xwup_api
xwer_t xwup_plsem_post(struct xwup_plsem * sem)
{
        struct xwup_wqn * wqn;
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count < 0) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwup_plwq_choose(&sem->plwq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(wqn->owner);
                } else {
                        /* add plsem counter */
                        if (sem->vsem.count < sem->vsem.max) {
                                sem->vsem.count++;
                        } else {
                                rc = -ERANGE;
                        }
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                        if (sem->vsem.count > 0) {
                                struct xwup_evt * evt;

                                evt = sem->vsem.synobj.sel.evt;
                                if (NULL != evt) {
                                        xwup_sel_obj_s1i(evt, &sem->vsem.synobj);
                                }
                        }
#endif
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_plsem_wait(struct xwup_plsem * sem)
{
        return xwup_plsem_wait_to(sem, XWTM_MAX);
}

static __xwup_code
xwer_t xwup_plsem_blkthd_to_unlkwq_cpuirqrs(struct xwup_plsem * sem,
                                            struct xwup_skd * xwskd,
                                            struct xwup_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq)
{
        struct xwup_tt * xwtt;
        xwer_t rc;

        xwtt = &xwskd->tt;
        XWOS_BUG_ON((XWUP_SKDOBJ_ST_BLOCKING | XWUP_SKDOBJ_ST_SLEEPING |
                     XWUP_SKDOBJ_ST_READY | XWUP_SKDOBJ_ST_STANDBY |
                     XWUP_SKDOBJ_ST_FROZEN)
                    & thd->state);

        /* 检查是否被中断 */
        if (XWUP_SKDOBJ_ST_EXITING & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINTR;
                goto err_intr;
        }
        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_BLOCKING);
        xwup_thd_eq_plwq(thd, &sem->plwq, XWUP_WQTYPE_PLSEM);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 加入时间树 */
        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_SLEEPING);
        xwup_thd_tt_add_locked(thd, xwtt, to, cpuirq);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_unlock();
#endif
        xwup_skd_req_swcx();
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_lock();
#endif
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_SLEEPING);
                }
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_SLEEPING);
                }
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if (XWUP_TTN_WKUPRS_TIMEDOUT == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&sem->plwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
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
                rc = xwup_plwq_remove(&sem->plwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
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
        return rc;

err_intr:
        return rc;
}

static __xwup_code
xwer_t xwup_plsem_test(struct xwup_plsem * sem,
                       struct xwup_skd * xwskd, struct xwup_thd * thd,
                       xwtm_t to)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count <= 0) {
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                rc = xwup_skd_wakelock_lock();
                if (__xwcc_unlikely(rc < 0)) {
                        /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR*/
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -EINTR;
                } else {
#endif
                        rc = xwup_plsem_blkthd_to_unlkwq_cpuirqrs(sem, xwskd, thd,
                                                                  to, cpuirq);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                        xwup_skd_wakelock_unlock();
                }
#endif
        } else {
                sem->vsem.count--;
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                if (0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_plsem_wait_to(struct xwup_plsem * sem, xwtm_t to)
{
        struct xwup_thd * cthd;
        struct xwup_skd * xwskd;
        struct xwup_syshwt * hwt;
        xwtm_t now;
        xwer_t rc;

        cthd = xwup_skd_get_cthd_lc();
        xwskd = xwup_skd_get_lc();
        hwt = &xwskd->tt.hwt;
        now = xwup_syshwt_get_timetick(hwt);
        if (__xwcc_unlikely(xwtm_cmp(to, now) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__xwcc_unlikely(xwtm_cmp(to, now) == 0)) {
                rc = xwup_plsem_trywait(sem);
                if (__xwcc_unlikely(rc < 0)) {
                        if (-ENODATA == rc) {
                                rc = -ETIMEDOUT;
                        }
                }
        } else if (!xwup_skd_tstpmpt_lc()) {
                rc = -ECANNOTPMPT;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        } else if (!xwup_skd_tstbh_lc()) {
                rc = -ECANNOTBH;
#endif
        } else {
                rc = xwup_plsem_test(sem, xwskd, cthd, to);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_plsem_blkthd_unlkwq_cpuirqrs(struct xwup_plsem * sem,
                                         struct xwup_thd * thd,
                                         xwreg_t cpuirq)
{
        xwer_t rc;

        XWOS_BUG_ON((XWUP_SKDOBJ_ST_BLOCKING | XWUP_SKDOBJ_ST_SLEEPING |
                     XWUP_SKDOBJ_ST_READY | XWUP_SKDOBJ_ST_STANDBY |
                     XWUP_SKDOBJ_ST_FROZEN)
                    & thd->state);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_ST_BLOCKING | XWUP_SKDOBJ_ST_UNINTERRUPTED);
        xwup_thd_eq_plwq(thd, &sem->plwq, XWUP_WQTYPE_PLSEM);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
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
xwer_t xwup_plsem_test_unintr(struct xwup_plsem * sem, struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count <= 0) {
                rc = xwup_plsem_blkthd_unlkwq_cpuirqrs(sem, thd, cpuirq);
        } else {
                sem->vsem.count--;
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                if (0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_plsem_wait_unintr(struct xwup_plsem * sem)
{
        struct xwup_thd * cthd;
        xwer_t rc;

        if (!xwup_skd_tstpmpt_lc()) {
                rc = -ECANNOTPMPT;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        } else if (!xwup_skd_tstbh_lc()) {
                rc = -ECANNOTBH;
#endif
        } else {
                cthd = xwup_skd_get_cthd_lc();
                rc = xwup_plsem_test_unintr(sem, cthd);
        }
        return rc;
}

__xwup_api
xwer_t xwup_plsem_trywait(struct xwup_plsem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count > 0) {
                sem->vsem.count--;
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
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
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_plsem_getvalue(struct xwup_plsem * sem, xwssq_t * sval)
{
        return xwup_vsem_getvalue(&sem->vsem, sval);
}
