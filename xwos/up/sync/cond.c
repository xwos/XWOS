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
#include <xwos/up/lock/seqlock.h>
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/tt.h>
#include <xwos/up/thrd.h>
#include <xwos/up/plwq.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
  #include <xwos/up/sync/object.h>
  #include <xwos/up/sync/evt.h>
#endif /* XWUPCFG_SYNC_EVT */
#include <xwos/up/sync/cond.h>

static __xwup_code
struct xwup_cond * xwup_cond_alloc(void);

static __xwup_code
void xwup_cond_free(struct xwup_cond * cond);

static __xwup_code
xwer_t xwup_cond_broadcast_once(struct xwup_cond * cond, bool * retry);

static __xwup_code
xwer_t xwup_cond_do_timedblkthrd_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                                 struct xwup_tcb * tcb,
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
}

/**
 * @brief XWUP API：释放条件量对象
 * @param cond: (I) 条件量对象的指针
 */
static __xwup_code
void xwup_cond_free(struct xwup_cond * cond)
{
        xwmm_kma_free(cond);
}

/**
 * @brief 激活条件量对象
 * @param cond: (I) 条件量对象的指针
 */
__xwup_code
void xwup_cond_activate(struct xwup_cond * cond)
{
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        xwup_sync_object_activate(&cond->synobj);
#endif /* XWUPCFG_SYNC_EVT */
        xwup_plwq_init(&cond->wq);
        cond->neg = false;
}

/**
 * @brief XWUP API：使得条件量对象无效
 * @param cond: (I) 条件量的指针
 */
__xwup_code
void xwup_cond_deactivate(struct xwup_cond * cond)
{
        XWOS_UNUSED(cond);
}

/**
 * @brief XWUP API：静态始化成条件量
 * @param cond: (I) 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cond* ，不可重入
 */
__xwup_api
xwer_t xwup_cond_init(struct xwup_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwup_cond_activate(cond);
        return XWOK;
}

/**
 * @brief XWUP API：销毁静态方式初始化的条件量对象
 * @param cond: (I) 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cond* ，不可重入
 */
__xwup_api
xwer_t xwup_cond_destroy(struct xwup_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwup_cond_deactivate(cond);
        return XWOK;
}

/**
 * @brief XWUP API：动态创建条件量
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
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

/**
 * @brief XWUP API：删除动态创建的条件量
 * @param cond: (I) 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cond* ，不可重入
 */
__xwup_api
xwer_t xwup_cond_delete(struct xwup_cond * cond)
{
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwup_cond_deactivate(cond);
        xwup_cond_free(cond);
        return XWOK;
}

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
/**
 * @brief XWUP API：绑定条件量到事件对象，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param cond: (I) 条件量对象的指针
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 条件量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cond* ，不可重入
 */
__xwup_api
xwer_t xwup_cond_bind(struct xwup_cond * cond,
                      struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_evt_obj_bind(evt, &cond->synobj, pos, false);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief XWUP API：从事件对象上解绑条件量，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param cond: (I) 条件量对象的指针
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *cond* ，不可重入
 */
__xwup_api
xwer_t xwup_cond_unbind(struct xwup_cond * cond, struct xwup_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_evt_obj_unbind(evt, &cond->synobj, false);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}
#endif /* XWUPCFG_SYNC_EVT */

/**
 * @brief XWUP API：冻结条件量（值设置为负）
 * @param cond: (I) 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
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

/**
 * @brief XWUP API：解冻条件量
 * @param cond: (I) 条件量对象的指针
 * @param val: (I) 条件量的初始值
 * @param max: (I) 条件量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 条件量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的条件量起作用。
 */
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
 * @param cond: (I) 条件量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
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

/**
 * @brief XWUP API：中断条件量等待队列中的所有线程
 * @param cond: (I) 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
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

/**
 * @brief XWUP API：广播条件量，将等待队列中的所有线程唤醒
 * @param cond: (I) 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
__xwup_api
xwer_t xwup_cond_broadcast(struct xwup_cond * cond)
{
        xwreg_t cpuirq;
        bool retry;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        retry = false;
        do {
                rc = xwup_cond_broadcast_once(cond, &retry);
        } while (retry);
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        if (__xwcc_likely(XWOK == rc)) {
                struct xwup_evt * evt;
                struct xwup_sync_object * synobj;

                synobj = &cond->synobj;
                xwospl_cpuirq_save_lc(&cpuirq);
                evt = synobj->sel.evt;
                if (NULL != evt) {
                        xwup_evt_obj_s1i(evt, synobj);
                }
                xwospl_cpuirq_restore_lc(cpuirq);
        }
#endif /* XWUPCFG_SYNC_EVT */
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

/**
 * @brief XWUP API：单播条件量，只会唤醒等待队列最前面的一个线程
 * @param cond: (I) 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 条件量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量起作用，已冻结的条件量将得到错误码-ENEGATIVE。
 */
__xwup_api
xwer_t xwup_cond_unicast(struct xwup_cond * cond)
{
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);

        rc = xwup_cond_do_unicast(cond);
        return rc;
}

static __xwup_code
xwer_t xwup_cond_do_timedblkthrd_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                                 struct xwup_tcb * tcb,
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
                    & tcb->state);

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_BLOCKING);
        xwup_thrd_eq_plwq(tcb, &cond->wq, XWUP_WQTYPE_COND);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 同步解锁 */
        rc = xwup_thrd_do_unlock(lock, lktype, lkdata);
        if (XWOK == rc) {
                *lkst = XWOS_LKST_UNLOCKED;
        }/* else {} */

        /* 加入时间树 */
        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_SLEEPING);
        xwup_thrd_tt_add_locked(tcb, xwtt, expected, cpuirq);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_unlock();
#endif /* XWUPCFG_SKD_PM */
        xwup_skd_req_swcx();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_lock();
#endif /* XWUPCFG_SKD_PM */
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if (XWUP_WQN_REASON_INTR == tcb->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &tcb->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_SLEEPING);
                }/* else {} */
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWUP_WQN_REASON_UP == tcb->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &tcb->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_SLEEPING);
                }/* else {} */
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                        currtick = xwup_syshwt_get_timetick(hwt);
                        *xwtm = xwtm_sub(expected, currtick);
                        rc = xwup_thrd_do_lock(lock, lktype, xwtm, lkdata);
                        if (XWOK == rc) {
                                *lkst = XWOS_LKST_LOCKED;
                        }/* else {} */
                }/* else {} */
                rc = XWOK;
        } else if (XWUP_TTN_WKUPRS_TIMEDOUT == tcb->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&cond->wq, &tcb->wqn);
                if (XWOK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        tcb->wqn.reason = XWUP_WQN_REASON_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_WQN_REASON_INTR == tcb->wqn.reason) {
                                rc = -EINTR;
                        } else if (XWUP_WQN_REASON_UP == tcb->wqn.reason) {
                                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                                        currtick = xwup_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwup_thrd_do_lock(lock, lktype, xwtm,
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
        } else if (XWUP_TTN_WKUPRS_INTR == tcb->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&cond->wq, &tcb->wqn);
                if (XWOK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        tcb->wqn.reason = XWUP_WQN_REASON_INTR;
                        tcb->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -EINTR;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_WQN_REASON_INTR == tcb->wqn.reason) {
                                rc = -EINTR;
                        } else if (XWUP_WQN_REASON_UP == tcb->wqn.reason) {
                                if (__xwcc_likely(XWOS_LKST_UNLOCKED == *lkst)) {
                                        currtick = xwup_syshwt_get_timetick(hwt);
                                        *xwtm = xwtm_sub(expected, currtick);
                                        rc = xwup_thrd_do_lock(lock, lktype, xwtm,
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
                              struct xwup_tcb * tcb,
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
#endif /* XWUPCFG_SKD_PM */
                rc = xwup_cond_do_timedblkthrd_unlkwq_cpuirqrs(cond, tcb,
                                                               lock, lktype, lkdata,
                                                               xwtm, lkst,
                                                               cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                xwup_skd_wakelock_unlock();
        }
#endif /* XWUPCFG_SKD_PM */
        return rc;
}

/**
 * @brief XWUP API：等待条件量
 * @param cond: (I) 条件量对象指针
 * @param lock: (I) (I) 锁的地址
 * @param lktype: (I) 锁的类型，取值：@ref xwos_lock_type_em
 * @param lkdata: (I) 锁的数据
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 无效的参数
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwup_api
xwer_t xwup_cond_wait(struct xwup_cond * cond,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwsq_t * lkst)
{
        xwtm_t expected = XWTM_MAX;
        return xwup_cond_timedwait(cond, lock, lktype, lkdata, &expected, lkst);
}

/**
 * @brief XWUP API：限时等待条件量
 * @param cond: (I) 条件量对象的指针
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型，取值：@ref xwos_lock_type_em
 * @param lkdata: (I) 锁的数据
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval XWOK: 没有错误
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
__xwup_api
xwer_t xwup_cond_timedwait(struct xwup_cond * cond,
                           void * lock, xwsq_t lktype, void * lkdata,
                           xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwup_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((cond), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWOS_LK_NUM)),
                      "invalid-type", -EINVAL);
        XWOS_VALIDATE((-EINTHRD == xwup_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        *lkst = XWOS_LKST_LOCKED;
        ctcb = xwup_skd_get_ctcb_lc();
        if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwup_thrd_do_unlock(lock, lktype, lkdata);
                if (XWOK == rc) {
                        *lkst = XWOS_LKST_UNLOCKED;
                }/* else {} */
                rc = -ETIMEDOUT;
        } else {
                rc = xwup_cond_do_timedwait(cond, ctcb, lock, lktype, lkdata,
                                            xwtm, lkst);
        }
        return rc;
}
