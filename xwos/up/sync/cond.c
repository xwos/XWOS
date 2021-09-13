/**
 * @file
 * @brief 玄武OS UP内核同步机制：条件量
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
#if defined(XWUPCFG_SYNC_COND_STDC_MM) && (1 == XWUPCFG_SYNC_COND_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#include <xwos/up/plwq.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
#  include <xwos/up/sync/obj.h>
#  include <xwos/up/sync/evt.h>
#endif
#include <xwos/up/sync/cond.h>

static __xwup_code
struct xwup_cond * xwup_cond_alloc(void);

static __xwup_code
void xwup_cond_free(struct xwup_cond * cond);

static __xwup_code
xwer_t xwup_cond_broadcast_once(struct xwup_cond * cond, bool * retry);

static __xwup_code
xwer_t xwup_cond_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                                struct xwup_thd * thd,
                                                void * lock, xwsq_t lktype,
                                                void * lkdata,
                                                xwtm_t * xwtm, xwsq_t * lkst,
                                                xwreg_t cpuirq);

/**
 * @brief 申请条件量对象
 * @return 条件量对象的指针
 */
static __xwup_code
struct xwup_cond * xwup_cond_alloc(void)
{
#if defined(XWUPCFG_SKD_COND_STDC_MM) && (1 == XWUPCFG_SKD_COND_STDC_MM)
        struct xwup_cond * cond;

        cond = malloc(sizeof(struct xwup_cond));
        if (NULL == cond) {
                cond = err_ptr(-ENOMEM);
        }
        return cond;
#else
        union {
                struct xwup_cond * cond;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_cond), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.cond = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.cond;
#endif
}

/**
 * @brief XWUP API：释放条件量对象
 * @param[in] cond: 条件量对象的指针
 */
static __xwup_code
void xwup_cond_free(struct xwup_cond * cond)
{
#if defined(XWUPCFG_SKD_COND_STDC_MM) && (1 == XWUPCFG_SKD_COND_STDC_MM)
        free(sem);
#else
        xwmm_kma_free(cond);
#endif
}

/**
 * @brief 激活条件量对象
 * @param[in] cond: 条件量对象的指针
 */
__xwup_code
void xwup_cond_activate(struct xwup_cond * cond)
{
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        xwup_synobj_activate(&cond->synobj);
#endif
        xwup_plwq_init(&cond->wq);
        cond->neg = false;
}

/**
 * @brief XWUP API：使得条件量对象无效
 * @param[in] cond: 条件量的指针
 */
__xwup_code
void xwup_cond_deactivate(struct xwup_cond * cond)
{
        XWOS_UNUSED(cond);
}

__xwup_api
xwer_t xwup_cond_init(struct xwup_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwup_cond_activate(cond);
        return XWOK;
}

__xwup_api
xwer_t xwup_cond_fini(struct xwup_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwup_cond_deactivate(cond);
        return XWOK;
}

__xwup_api
xwer_t xwup_cond_create(struct xwup_cond ** ptrbuf)
{
        struct xwup_cond * cond;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        cond = xwup_cond_alloc();
        if (__xwcc_unlikely(is_err(cond))) {
                rc = ptr_err(cond);
        } else {
                xwup_cond_activate(cond);
                *ptrbuf = cond;
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_cond_delete(struct xwup_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwup_cond_deactivate(cond);
        xwup_cond_free(cond);
        return XWOK;
}

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
__xwup_api
xwer_t xwup_cond_bind(struct xwup_cond * cond,
                      struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_bind(evt, &cond->synobj, pos, false);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_cond_unbind(struct xwup_cond * cond, struct xwup_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_unbind(evt, &cond->synobj, false);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}
#endif

__xwup_api
xwer_t xwup_cond_freeze(struct xwup_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_unlikely(cond->neg)) {
                rc = -EALREADY;
        } else {
                cond->neg = true;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_cond_thaw(struct xwup_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_likely(cond->neg)) {
                cond->neg = false;
        } else {
                rc = -EALREADY;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief 中断条件量等待队列中的一个节点
 * @param[in] cond: 条件量对象的指针
 * @param[in] wqn: 等待队列节点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_code
xwer_t xwup_cond_intr(struct xwup_cond * cond, struct xwup_wqn * wqn)
{
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_plwq_remove(&cond->wq, wqn);
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
xwer_t xwup_cond_intr_all(struct xwup_cond * cond)
{
        struct xwup_wqn * c;
        xwup_wqn_f cb;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        xwup_plwq_itr_wqn_rm(c, &cond->wq) {
                xwup_plwq_remove(&cond->wq, c);
                c->wq = NULL;
                c->type = XWUP_WQTYPE_UNKNOWN;
                c->reason = XWUP_WQN_REASON_INTR;
                cb = c->cb;
                c->cb = NULL;
                xwospl_cpuirq_restore_lc(cpuirq);
                cb(c->owner);
                xwospl_cpuirq_disable_lc();
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return XWOK;
}

static __xwup_code
xwer_t xwup_cond_broadcast_once(struct xwup_cond * cond, bool * retry)
{
        struct xwup_wqn * wqn;
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_unlikely(cond->neg)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
                *retry = false;
        } else {
                wqn = xwup_plwq_choose(&cond->wq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(wqn->owner);
                        *retry = true;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        *retry = false;
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_cond_broadcast(struct xwup_cond * cond)
{
        bool retry;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        retry = false;
        do {
                rc = xwup_cond_broadcast_once(cond, &retry);
        } while (retry);
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        if (__xwcc_likely(XWOK == rc)) {
                xwreg_t cpuirq;
                struct xwup_evt * evt;
                struct xwup_synobj * synobj;

                synobj = &cond->synobj;
                xwospl_cpuirq_save_lc(&cpuirq);
                evt = synobj->sel.evt;
                if (NULL != evt) {
                        xwup_sel_obj_s1i(evt, synobj);
                }
                xwospl_cpuirq_restore_lc(cpuirq);
        }
#endif
        return rc;
}

__xwup_code
xwer_t xwup_cond_do_unicast(struct xwup_cond * cond)
{
        struct xwup_wqn * wqn;
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_unlikely(cond->neg)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwup_plwq_choose(&cond->wq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(wqn->owner);
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_cond_unicast(struct xwup_cond * cond)
{
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwup_cond_do_unicast(cond);
        return rc;
}

static __xwup_code
xwer_t xwup_cond_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                                struct xwup_thd * thd,
                                                void * lock, xwsq_t lktype,
                                                void * lkdata,
                                                xwtm_t * xwtm, xwsq_t * lkst,
                                                xwreg_t cpuirq)
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
        xwup_thd_eq_plwq(thd, &cond->wq, XWUP_WQTYPE_COND);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 同步解锁 */
        rc = xwup_thd_do_unlock(lock, lktype, lkdata);
        if (XWOK == rc) {
                *lkst = XWOS_LKST_UNLOCKED;
        }/* else {} */

        /* 加入时间树 */
        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
        xwup_thd_tt_add_locked(thd, xwtt, expected, cpuirq);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_unlock();
#endif
        xwup_skd_req_swcx();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_lock();
#endif
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
                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                        currtick = xwup_syshwt_get_timetick(hwt);
                        *xwtm = xwtm_sub(expected, currtick);
                        rc = xwup_thd_do_lock(lock, lktype, xwtm, lkdata);
                        if (XWOK == rc) {
                                *lkst = XWOS_LKST_LOCKED;
                        }/* else {} */
                }/* else {} */
                rc = XWOK;
        } else if (XWUP_TTN_WKUPRS_TIMEDOUT == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&cond->wq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                                rc = -EINTR;
                        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                                        currtick = xwup_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwup_thd_do_lock(lock, lktype, xwtm,
                                                              lkdata);
                                        if (XWOK == rc)
                                                *lkst = XWOS_LKST_LOCKED;
                                }
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWUP_TTN_WKUPRS_INTR == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&cond->wq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -EINTR;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                                rc = -EINTR;
                        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                                        currtick = xwup_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwup_thd_do_lock(lock, lktype, xwtm,
                                                              lkdata);
                                        if (XWOK == rc) {
                                                *lkst = XWOS_LKST_LOCKED;
                                        }/* else {} */
                                }/* else {} */
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

__xwup_code
xwer_t xwup_cond_do_timedwait(struct xwup_cond * cond,
                              struct xwup_thd * thd,
                              void * lock, xwsq_t lktype, void * lkdata,
                              xwtm_t * xwtm, xwsq_t * lkst)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        rc = xwup_skd_wakelock_lock();
        if (__xwcc_unlikely(rc < 0)) {
                /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR。*/
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINTR;
        } else {
#endif
                rc = xwup_cond_do_timedblkthd_unlkwq_cpuirqrs(cond, thd,
                                                              lock, lktype, lkdata,
                                                              xwtm, lkst,
                                                              cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                xwup_skd_wakelock_unlock();
        }
#endif
        return rc;
}

__xwup_api
xwer_t xwup_cond_wait(struct xwup_cond * cond,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwsq_t * lkst)
{
        xwtm_t expected = XWTM_MAX;
        return xwup_cond_timedwait(cond, lock, lktype, lkdata, &expected, lkst);
}

__xwup_api
xwer_t xwup_cond_timedwait(struct xwup_cond * cond,
                           void * lock, xwsq_t lktype, void * lkdata,
                           xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwup_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((lktype < XWOS_LK_NUM), "invalid-type", -EINVAL);
        XWOS_VALIDATE((((NULL == lock) && (XWOS_LK_NONE == lktype)) ||
                       ((lock) && (lktype > XWOS_LK_NONE))),
                      "invalid-lock", -EINVAL);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        *lkst = XWOS_LKST_LOCKED;
        cthd = xwup_skd_get_cthd_lc();
        if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwup_thd_do_unlock(lock, lktype, lkdata);
                if (XWOK == rc) {
                        *lkst = XWOS_LKST_UNLOCKED;
                }/* else {} */
                rc = -ETIMEDOUT;
        } else {
                rc = xwup_cond_do_timedwait(cond, cthd, lock, lktype, lkdata,
                                            xwtm, lkst);
        }
        return rc;
}
