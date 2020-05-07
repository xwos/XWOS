/**
 * @file
 * @brief XuanWuOS的同步机制：条件量
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
#include <xwos/up/plwq.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
  #include <xwos/up/sync/object.h>
  #include <xwos/up/sync/event.h>
#endif /* XWUPCFG_SYNC_EVT */
#include <xwos/up/sync/condition.h>

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
struct xwsync_cdt * xwsync_cdt_alloc(void);

static __xwos_code
void xwsync_cdt_free(struct xwsync_cdt * cdt);

static __xwos_code
xwer_t xwsync_cdt_broadcast_once(struct xwsync_cdt * cdt, bool * retry);

static __xwos_code
xwer_t xwsync_cdt_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_cdt * cdt,
                                                  struct xwos_tcb * tcb,
                                                  void * lock, xwsq_t lktype,
                                                  void * lkdata, xwsz_t datanum,
                                                  xwtm_t * xwtm, xwsq_t * lkst,
                                                  xwreg_t cpuirq);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 申请条件量对象
 * @return 条件量对象的指针
 */
static __xwos_code
struct xwsync_cdt * xwsync_cdt_alloc(void)
{
        union {
                struct xwsync_cdt * cdt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwsync_cdt), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.cdt = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.cdt;
}

/**
 * @brief XWOS API：释放条件量对象
 * @param cdt: (I) 条件量对象的指针
 */
static __xwos_code
void xwsync_cdt_free(struct xwsync_cdt * cdt)
{
        xwmm_kma_free(cdt);
}

/**
 * @brief 激活条件量对象
 * @param cdt: (I) 条件量对象的指针
 */
__xwos_code
void xwsync_cdt_activate(struct xwsync_cdt * cdt)
{
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        xwsync_object_activate(&cdt->xwsyncobj);
#endif /* XWUPCFG_SYNC_EVT */
        xwos_plwq_init(&cdt->wq);
        cdt->neg = false;
}

/**
 * @brief XWOS API：使得条件量对象无效
 * @param cdt: (I) 条件量的指针
 */
__xwos_code
void xwsync_cdt_deactivate(struct xwsync_cdt * cdt)
{
        XWOS_UNUSED(cdt);
}

/**
 * @brief XWOS API：静态始化成条件量
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_init(struct xwsync_cdt * cdt)
{
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        xwsync_cdt_activate(cdt);
        return OK;
}

/**
 * @brief XWOS API：销毁条件量对象
 * @param cdt: (I) 条件量的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_destroy(struct xwsync_cdt * cdt)
{
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        xwsync_cdt_deactivate(cdt);
        return OK;
}

/**
 * @brief XWOS API：动态创建条件量
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象的指针
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
xwer_t xwsync_cdt_create(struct xwsync_cdt ** ptrbuf)
{
        struct xwsync_cdt * cdt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        cdt = xwsync_cdt_alloc();
        if (__unlikely(is_err(cdt))) {
                rc = ptr_err(cdt);
        } else {
                xwsync_cdt_activate(cdt);
                *ptrbuf = cdt;
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的条件量
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_delete(struct xwsync_cdt * cdt)
{
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        xwsync_cdt_deactivate(cdt);
        xwsync_cdt_free(cdt);
        return OK;
}

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
/**
 * @brief XWOS API：绑定条件量到事件对象，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param cdt: (I) 条件量对象的指针
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 条件量对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 事件对象或条件量类型错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_bind(struct xwsync_cdt * cdt, struct xwsync_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        xwos_cpuirq_save_lc(&cpuirq);
        rc = xwsync_evt_obj_bind(evt, &cdt->xwsyncobj, pos, false);
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief XWOS API：从事件对象上解绑条件量，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param cdt: (I) 条件量对象的指针
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 事件对象或条件量类型错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cdt* ，不可重入
 */
__xwos_api
xwer_t xwsync_cdt_unbind(struct xwsync_cdt * cdt, struct xwsync_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwos_cpuirq_save_lc(&cpuirq);
        rc = xwsync_evt_obj_unbind(evt, &cdt->xwsyncobj, false);
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}
#endif /* XWUPCFG_SYNC_EVT */

/**
 * @brief XWOS API：冻结条件量（值设置为负）
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的条件量不允许单播或广播，但可以被等待，
 *   测试条件量的线程会加入到条件量等待队列中阻塞等待。
 */
__xwos_api
xwer_t xwsync_cdt_freeze(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = OK;
        xwos_cpuirq_save_lc(&cpuirq);
        if (__unlikely(cdt->neg)) {
                rc = -EALREADY;
        } else {
                cdt->neg = true;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief XWOS API：解冻条件量，并重新初始化
 * @param cdt: (I) 条件量对象的指针
 * @param val: (I) 条件量的初始值
 * @param max: (I) 条件量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 条件量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的条件量起作用。
 */
__xwos_api
xwer_t xwsync_cdt_thaw(struct xwsync_cdt * cdt)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = OK;
        xwos_cpuirq_save_lc(&cpuirq);
        if (__likely(cdt->neg)) {
                cdt->neg = false;
        } else {
                rc = -EALREADY;
        }
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief 中断条件量等待队列中的一个节点
 * @param cdt: (I) 条件量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwsync_cdt_intr(struct xwsync_cdt * cdt, struct xwos_wqn * wqn)
{
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_cpuirq_save_lc(&cpuirq);
        rc = xwos_plwq_remove(&cdt->wq, wqn);
        if (OK == rc) {
                wqn->wq = NULL;
                wqn->type = XWOS_WQTYPE_UNKNOWN;
                wqn->rsmrs = XWOS_WQN_RSMRS_INTR;
                cb = wqn->cb;
                wqn->cb = NULL;
                xwos_cpuirq_restore_lc(cpuirq);
                cb(wqn->owner);
        } else {
                xwos_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

/**
 * @brief XWOS API：中断条件量等待队列中的所有线程
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwsync_cdt_intr_all(struct xwsync_cdt * cdt)
{
        struct xwos_wqn * c;
        xwos_wqn_f cb;
        xwreg_t cpuirq;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        xwos_cpuirq_save_lc(&cpuirq);
        xwos_plwq_itr_wqn_rm(c, &cdt->wq) {
                xwos_plwq_remove(&cdt->wq, c);
                c->wq = NULL;
                c->type = XWOS_WQTYPE_UNKNOWN;
                c->rsmrs = XWOS_WQN_RSMRS_INTR;
                cb = c->cb;
                c->cb = NULL;
                xwos_cpuirq_restore_lc(cpuirq);
                cb(c->owner);
                xwos_cpuirq_disable_lc();
        }
        xwos_cpuirq_restore_lc(cpuirq);
        return OK;
}

static __xwos_code
xwer_t xwsync_cdt_broadcast_once(struct xwsync_cdt * cdt, bool * retry)
{
        struct xwos_wqn * wqn;
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_save_lc(&cpuirq);
        if (__unlikely(cdt->neg)) {
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
                *retry = false;
        } else {
                wqn = xwos_plwq_choose(&cdt->wq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        wqn->rsmrs = XWOS_WQN_RSMRS_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwos_cpuirq_restore_lc(cpuirq);
                        cb(wqn->owner);
                        *retry = true;
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                        *retry = false;
                }
        }
        return rc;
}

/**
 * @brief XWOS API：广播条件量，将等待队列中的所有线程唤醒
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
__xwos_api
xwer_t xwsync_cdt_broadcast(struct xwsync_cdt * cdt)
{
        xwreg_t cpuirq;
        bool retry;
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        retry = false;
        do {
                rc = xwsync_cdt_broadcast_once(cdt, &retry);
        } while (retry);
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (__likely(OK == rc)) {
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &cdt->xwsyncobj;
                        xwos_cpuirq_save_lc(&cpuirq);
                        evt = xwsyncobj->selector.evt;
                        if (NULL != evt) {
                                xwsync_evt_obj_s1i(evt, xwsyncobj);
                        }
                        xwos_cpuirq_restore_lc(cpuirq);
                }
#endif /* XWUPCFG_SYNC_EVT */
        return rc;
}

__xwos_code
xwer_t xwsync_cdt_do_unicast(struct xwsync_cdt * cdt)
{
        struct xwos_wqn * wqn;
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_save_lc(&cpuirq);
        if (__unlikely(cdt->neg)) {
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwos_plwq_choose(&cdt->wq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        wqn->rsmrs = XWOS_WQN_RSMRS_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwos_cpuirq_restore_lc(cpuirq);
                        cb(wqn->owner);
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                }
                rc = OK;
        }
        return rc;
}

/**
 * @brief XWOS API：单播条件量，只会唤醒等待队列最前面的一个线程
 * @param cdt: (I) 条件量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note；
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
__xwos_api
xwer_t xwsync_cdt_unicast(struct xwsync_cdt * cdt)
{
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);

        rc = xwsync_cdt_do_unicast(cdt);
        return rc;
}

static __xwos_code
xwer_t xwsync_cdt_do_timedblkthrd_unlkwq_cpuirqrs(struct xwsync_cdt * cdt,
                                                  struct xwos_tcb * tcb,
                                                  void * lock, xwsq_t lktype,
                                                  void * lkdata, xwsz_t datanum,
                                                  xwtm_t * xwtm, xwsq_t * lkst,
                                                  xwreg_t cpuirq)
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
        xwos_thrd_eq_plwq(tcb, &cdt->wq, XWOS_WQTYPE_CDT);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 同步解锁 */
        rc = xwos_thrd_do_unlock(lock, lktype, lkdata, datanum);
        if (OK == rc) {
                *lkst = XWLK_STATE_UNLOCKED;
        }/* else {} */

        /* 加入时间树 */
        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
        xwos_thrd_tt_add_locked(tcb, xwtt, expected, cpuirq);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwos_cpuirq_enable_lc();
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
        xwos_scheduler_wakelock_unlock();
#endif /* XWUPCFG_SD_PM */
        xwos_scheduler_req_swcx();
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
        xwos_scheduler_wakelock_lock();
#endif /* XWUPCFG_SD_PM */
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                if (__likely(XWLK_STATE_UNLOCKED == *lkst)) {
                        currtick = xwos_syshwt_get_timetick(hwt);
                        *xwtm = xwtm_sub(expected, currtick);
                        rc = xwos_thrd_do_lock(lock, lktype, xwtm, lkdata, datanum);
                        if (OK == rc) {
                                *lkst = XWLK_STATE_LOCKED;
                        }/* else {} */
                }/* else {} */
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_TIMEDOUT == tcb->ttn.wkuprs) {
                xwos_cpuirq_disable_lc();
                rc = xwos_plwq_remove(&cdt->wq, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        xwos_cpuirq_restore_lc(cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                if (__likely(XWLK_STATE_UNLOCKED == *lkst)) {
                                        currtick = xwos_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwos_thrd_do_lock(lock, lktype, xwtm,
                                                               lkdata, datanum);
                                        if (OK == rc)
                                                *lkst = XWLK_STATE_LOCKED;
                                }
                                rc = OK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWOS_TTN_WKUPRS_INTR == tcb->ttn.wkuprs) {
                xwos_cpuirq_disable_lc();
                rc = xwos_plwq_remove(&cdt->wq, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        xwos_cpuirq_restore_lc(cpuirq);
                        rc = -EINTR;
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                        if (XWOS_WQN_RSMRS_INTR == tcb->wqn.rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == tcb->wqn.rsmrs) {
                                if (__likely(XWLK_STATE_UNLOCKED == *lkst)) {
                                        currtick = xwos_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwos_thrd_do_lock(lock, lktype, xwtm,
                                                               lkdata, datanum);
                                        if (OK == rc) {
                                                *lkst = XWLK_STATE_LOCKED;
                                        }/* else {} */
                                }/* else {} */
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

__xwos_code
xwer_t xwsync_cdt_do_timedwait(struct xwsync_cdt * cdt,
                               struct xwos_tcb * tcb,
                               void * lock, xwsq_t lktype,
                               void * lkdata, xwsz_t datanum,
                               xwtm_t * xwtm, xwsq_t * lkst)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_cpuirq_save_lc(&cpuirq);
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
        rc = xwos_scheduler_wakelock_lock();
        if (__unlikely(rc < 0)) {
                /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR。*/
                xwos_cpuirq_restore_lc(cpuirq);
                rc = -EINTR;
        } else {
#endif /* XWUPCFG_SD_PM */
                rc = xwsync_cdt_do_timedblkthrd_unlkwq_cpuirqrs(cdt, tcb,
                                                                lock, lktype,
                                                                lkdata, datanum,
                                                                xwtm, lkst,
                                                                cpuirq);
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
                xwos_scheduler_wakelock_unlock();
        }
#endif /* XWUPCFG_SD_PM */
        return rc;
}

/**
 * @brief XWOS API：限时等待条件量
 * @param cdt: (I) 条件量对象的指针
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
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
xwer_t xwsync_cdt_timedwait(struct xwsync_cdt * cdt,
                            void * lock, xwsq_t lktype,
                            void * lkdata, xwsz_t datanum,
                            xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((cdt), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWLK_TYPE_NUM)),
                      "invalid-type", -EINVAL);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        *lkst = XWLK_STATE_LOCKED;
        ctcb = xwos_scheduler_get_ctcb_lc();
        if (__unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwos_thrd_do_unlock(lock, lktype, lkdata, datanum);
                if (OK == rc) {
                        *lkst = XWLK_STATE_UNLOCKED;
                }/* else {} */
                rc = -ETIMEDOUT;
        } else {
                rc = xwsync_cdt_do_timedwait(cdt, ctcb, lock, lktype,
                                             lkdata, datanum,
                                             xwtm, lkst);
        }
        return rc;
}
