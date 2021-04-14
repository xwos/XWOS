/**
 * @file
 * @brief 玄武OS UP内核同步机制：管道信号量
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
#if defined(XWUPCFG_SYNC_PLSEM_STDC_MM) && (1 == XWUPCFG_SYNC_PLSEM_STDC_MM)
  #include <stdlib.h>
#endif /* XWUPCFG_SYNC_PLSEM_STDC_MM */
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#include <xwos/up/plwq.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
  #include <xwos/up/sync/evt.h>
#endif /* XWUPCFG_SYNC_EVT */
#include <xwos/up/sync/plsem.h>

static __xwup_code
struct xwup_plsem * xwup_plsem_alloc(void);

static __xwup_code
void xwup_plsem_free(struct xwup_plsem * sem);

static __xwup_code
void xwup_plsem_activate(struct xwup_plsem * sem, xwssq_t val, xwssq_t max);

static __xwup_code
void xwup_plsem_deactivate(struct xwup_plsem * sem);

static __xwup_code
xwer_t xwup_plsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_plsem * sem,
                                                 struct xwup_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t flag);

static __xwup_code
xwer_t xwup_plsem_do_timedwait(struct xwup_plsem * sem, struct xwup_thd * thd,
                               xwtm_t * xwtm);

static __xwup_code
xwer_t xwup_plsem_do_blkthd_unlkwq_cpuirqrs(struct xwup_plsem * sem,
                                            struct xwup_thd * thd,
                                            xwreg_t flag);

static __xwup_code
xwer_t xwup_plsem_do_wait_unintr(struct xwup_plsem * sem, struct xwup_thd * thd);

/**
 * @brief 申请管道信号量对象
 * @return 信号量对象的指针
 */
static __xwup_code
struct xwup_plsem * xwup_plsem_alloc(void)
{
#if defined(XWUPCFG_SKD_PLSEM_STDC_MM) && (1 == XWUPCFG_SKD_PLSEM_STDC_MM)
        struct xwup_plsem * plsem;

        plsem = malloc(sizeof(struct xwup_plsem));
        if (NULL == plsem) {
                plsem = err_ptr(-ENOMEM);
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
        }/* else {} */
        return mem.sem;
#endif
}

/**
 * @brief 释放管道信号量对象
 * @param sem: (I) 信号量对象的指针
 */
static __xwup_code
void xwup_plsem_free(struct xwup_plsem * sem)
{
#if defined(XWUPCFG_SKD_PLSEM_STDC_MM) && (1 == XWUPCFG_SKD_PLSEM_STDC_MM)
        free(sem);
#else
        xwmm_kma_free(sem);
#endif
}

/**
 * @brief 激活管道信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 */
static __xwup_code
void xwup_plsem_activate(struct xwup_plsem * sem, xwssq_t val, xwssq_t max)
{
        xwup_vsem_activate(&sem->vsem);
        xwup_plwq_init(&sem->plwq);
        sem->vsem.count = val;
        sem->vsem.max = max;
}

/**
 * @brief 使得管道信号量对象无效
 * @param sem: (I) 信号量对象的指针
 */
static __xwup_code
void xwup_plsem_deactivate(struct xwup_plsem * sem)
{
        XWOS_UNUSED(sem);
}

/**
 * @brief XWUP API：初始化管道信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwup_api
xwer_t xwup_plsem_init(struct xwup_plsem * sem, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        xwup_plsem_activate(sem, val, max);
        return XWOK;
}

/**
 * @brief XWUP API：销毁管道信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwup_api
xwer_t xwup_plsem_destroy(struct xwup_plsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        xwup_plsem_deactivate(sem);
        return XWOK;
}

/**
 * @brief XWUP API：动态创建管道信号量
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象的指针
 * @param type: (I) 信号量的类型
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
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
xwer_t xwup_plsem_create(struct xwup_plsem ** ptrbuf, xwssq_t val, xwssq_t max)
{
        struct xwup_plsem * sem;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        *ptrbuf = NULL;
        sem = xwup_plsem_alloc();
        if (__xwcc_unlikely(is_err(sem))) {
                rc = ptr_err(sem);
        } else {
                xwup_plsem_activate(sem, val, max);
                *ptrbuf = sem;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWUP API：删除动态创建的管道信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwup_api
xwer_t xwup_plsem_delete(struct xwup_plsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        xwup_plsem_deactivate(sem);
        xwup_plsem_free(sem);
        return XWOK;
}

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
/**
 * @brief XWUP API：绑定管道信号量到事件对象，
 *                  事件对象类型为XWUP_EVT_TYPE_SEL
 * @param sem: (I) 管道信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 管道信号量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwup_api
xwer_t xwup_plsem_bind(struct xwup_plsem * sem, struct xwup_evt * evt,
                       xwsq_t pos)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwup_vsem_bind(&sem->vsem, evt, pos);
}

/**
 * @brief XWUP API：从事件对象上解绑管道信号量，
 *                  事件对象类型为XWUP_EVT_TYPE_SEL
 * @param sem: (I) 管道信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
__xwup_api
xwer_t xwup_plsem_unbind(struct xwup_plsem * sem, struct xwup_evt * evt)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwup_vsem_unbind(&sem->vsem, evt);
}
#endif /* XWUPCFG_SYNC_EVT */

/**
 * @brief 中断管道信号量等待队列中的一个节点
 * @param sem: (I) 信号量对象的指针
 * @param wqn: (I) 等待队列节点
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_code
xwer_t xwup_plsem_intr(struct xwup_plsem * sem, struct xwup_wqn * wqn)
{
        xwup_wqn_f cb;
        xwreg_t flag;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&flag);
        rc = xwup_plwq_remove(&sem->plwq, wqn);
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

/**
 * @brief XWUP API：发布管道信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
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
__xwup_api
xwer_t xwup_plsem_post(struct xwup_plsem * sem)
{
        struct xwup_wqn * wqn;
        xwup_wqn_f cb;
        xwreg_t flag;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        rc = XWOK;
        xwospl_cpuirq_save_lc(&flag);
        if (sem->vsem.count < 0) {
                xwospl_cpuirq_restore_lc(flag);
                rc = -ENEGATIVE;
        } else {
                wqn = xwup_plwq_choose(&sem->plwq);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(flag);
                        cb(wqn->owner);
                } else {
                        /* add plsem counter */
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
#endif /* XWUPCFG_SYNC_EVT */
                        xwospl_cpuirq_restore_lc(flag);
                }
        }
        return rc;
}

/**
 * @brief XWUP API：等待并获取管道信号量
 * @param sem: (I) 信号量对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwup_api
xwer_t xwup_plsem_wait(struct xwup_plsem * sem)
{
        xwtm_t expected = XWTM_MAX;
        return xwup_plsem_timedwait(sem, &expected);
}

/**
 * @brief XWUP API：尝试获取管道信号量，不会阻塞调用线程
 * @param sem: (I) 信号量对象的指针
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 信号量资源不可用（信号量不可被获取）
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数不会阻塞调用线程，因此可以在中断上下文中使用。
 */
__xwup_api
xwer_t xwup_plsem_trywait(struct xwup_plsem * sem)
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
#endif /* XWUPCFG_SYNC_EVT */
        } else {
                rc = -ENODATA;
        }
        xwospl_cpuirq_restore_lc(flag);
        return rc;
}

static __xwup_code
xwer_t xwup_plsem_do_timedblkthd_unlkwq_cpuirqrs(struct xwup_plsem * sem,
                                                 struct xwup_thd * thd,
                                                 xwtm_t * xwtm,
                                                 xwreg_t flag)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        xwtm_t expected, currtick;
        xwer_t rc;

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
        xwup_thd_eq_plwq(thd, &sem->plwq, XWUP_WQTYPE_PLSEM);
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
#endif /* XWUPCFG_SKD_PM */
        xwup_skd_req_swcx();
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        xwup_skd_wakelock_lock();
#endif /* XWUPCFG_SKD_PM */
        xwospl_cpuirq_restore_lc(flag);

        /* 判断唤醒原因 */
        if (XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
                }
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);
                rc = -EINTR;
        } else if (XWUP_WQN_REASON_UP == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
                }
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, flag);
                rc = XWOK;
        } else if (XWUP_TTN_WKUPRS_TIMEDOUT == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&sem->plwq, &thd->wqn);
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
                rc = xwup_plwq_remove(&sem->plwq, &thd->wqn);
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
xwer_t xwup_plsem_do_timedwait(struct xwup_plsem * sem, struct xwup_thd * thd,
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
#endif /* XWUPCFG_SKD_PM */
                        rc = xwup_plsem_do_timedblkthd_unlkwq_cpuirqrs(sem,
                                                                       thd,
                                                                       xwtm,
                                                                       flag);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                        xwup_skd_wakelock_unlock();
                }
#endif /* XWUPCFG_SKD_PM */
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
#endif /* XWUPCFG_SYNC_EVT */
                xwospl_cpuirq_restore_lc(flag);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWUP API：限时等待并获取管道信号量
 * @param sem: (I) 信号量对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwup_api
xwer_t xwup_plsem_timedwait(struct xwup_plsem * sem, xwtm_t * xwtm)
{
        struct xwup_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwup_plsem_trywait(sem);
                if (__xwcc_unlikely(rc < 0)) {
                        if (__xwcc_likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        }/* else {} */
                }/* else {} */
        } else {
                cthd = xwup_skd_get_cthd_lc();
                rc = xwup_plsem_do_timedwait(sem, cthd, xwtm);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_plsem_do_blkthd_unlkwq_cpuirqrs(struct xwup_plsem * sem,
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
        xwup_thd_eq_plwq(thd, &sem->plwq, XWUP_WQTYPE_PLSEM);
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
xwer_t xwup_plsem_do_wait_unintr(struct xwup_plsem * sem, struct xwup_thd * thd)
{
        xwreg_t flag;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&flag);
        if (sem->vsem.count <= 0) {
                rc = xwup_plsem_do_blkthd_unlkwq_cpuirqrs(sem, thd, flag);
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
#endif /* XWUPCFG_SYNC_EVT */
                xwospl_cpuirq_restore_lc(flag);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWUP API：等待并获取管道信号量，且等待不可被中断
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_plsem_wait_unintr(struct xwup_plsem * sem)
{
        struct xwup_thd * cthd;
        xwer_t rc;

        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        cthd = xwup_skd_get_cthd_lc();
        rc = xwup_plsem_do_wait_unintr(sem, cthd);
        return rc;
}

/**
 * @brief XWUP API：冻结管道信号量（值设置为负）
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 信号量已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)。
 *   测试的线程会被加入到信号量的等待队列。
 */
__xwup_api
xwer_t xwup_plsem_freeze(struct xwup_plsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwup_vsem_freeze(&sem->vsem);
}

/**
 * @brief XWUP API：解冻管道信号量
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 信号量未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用，对未冻结的信号量调用此函数将返回错误码。
 */
__xwup_api
xwer_t xwup_plsem_thaw(struct xwup_plsem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwup_vsem_thaw(&sem->vsem);
}

/**
 * @brief XWUP API：获取管道信号量计数器的值
 * @param sem: (I) 信号量对象的指针
 * @param sval: (O) 指向缓冲区的指针，通过此缓冲区返回信号量计数器的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_plsem_getvalue(struct xwup_plsem * sem, xwssq_t * sval)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((sval), "nullptr", -EFAULT);

        return xwup_vsem_getvalue(&sem->vsem, sval);
}
