/**
 * @file
 * @brief 玄武OS MP内核同步机制：信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - 锁的顺序：同级的锁不可同时获得
 *   + ① 等待队列的锁（rtwq->lock & plwq->lock）
 *     + ② 等待队列节点的锁（wqn->lock）
 *     + ② 线程控制块的状态锁（thd->stlock）
 *     + ② 事件对象的锁（evt->lock）
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWMPCFG_SYNC_SEM_STDC_MM) && (1 == XWMPCFG_SYNC_SEM_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/plwq.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/lock/seqlock.h>
#include <xwos/mp/lock/mtx.h>
#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
#  include <xwos/mp/sync/evt.h>
#endif
#include <xwos/mp/sync/obj.h>
#include <xwos/mp/sync/sem.h>

#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
/**
 * @brief 结构体xwmp_sem的对象缓存
 */
static __xwmp_data struct xwmm_memslice xwmp_sem_cache;

/**
 * @brief 结构体xwmp_sem的对象缓存的名字
 */
const __xwmp_rodata char xwmp_sem_cache_name[] = "xwos.mp.sync.sem.cache";
#endif

static __xwmp_code
struct xwmp_sem * xwmp_sem_alloc(void);

static __xwmp_code
void xwmp_sem_free(struct xwmp_sem * sem);

static __xwmp_code
void xwmp_sem_construct(struct xwmp_sem * sem);

static __xwmp_code
void xwmp_sem_destruct(struct xwmp_sem * sem);

static __xwmp_code
xwer_t xwmp_sem_sgc(void * sem);

static __xwmp_code
xwer_t xwmp_sem_dgc(void * sem);

#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
static __xwmp_code
xwer_t xwmp_plsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_plsem_blkthd_to_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_skd * xwskd,
                                            struct xwmp_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_plsem_test(struct xwmp_sem * sem, struct xwmp_skd * xwskd,
                       struct xwmp_thd * thd, xwtm_t to);

static __xwmp_code
xwer_t xwmp_plsem_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                         struct xwmp_thd * thd,
                                         struct xwmp_skd * xwskd,
                                         xwreg_t cpuirq);
static __xwmp_code
xwer_t xwmp_plsem_test_unintr(struct xwmp_sem * sem,
                              struct xwmp_thd * thd, struct xwmp_skd * xwskd);
#endif

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
static __xwmp_code
xwer_t xwmp_rtsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_rtsem_blkthd_to_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_skd * xwskd,
                                            struct xwmp_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_rtsem_test(struct xwmp_sem * sem, struct xwmp_skd * xwskd,
                       struct xwmp_thd * thd, xwtm_t to);

static __xwmp_code
xwer_t xwmp_rtsem_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                         struct xwmp_thd * thd,
                                         struct xwmp_skd * xwskd,
                                         xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_rtsem_test_unintr(struct xwmp_sem * sem,
                              struct xwmp_thd * thd, struct xwmp_skd * xwskd);
#endif

#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化结构体xwmp_sem的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_sem_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwmp_sem_cache, zone_origin, zone_size,
                                sizeof(struct xwmp_sem),
                                xwmp_sem_cache_name,
                                (ctor_f)xwmp_sem_construct,
                                (dtor_f)xwmp_sem_destruct);
        return rc;
}
#endif

/**
 * @brief 从信号量对象缓存中申请对象
 * @return 信号量对象的指针
 */
static __xwmp_code
struct xwmp_sem * xwmp_sem_alloc(void)
{
#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
        union {
                struct xwmp_sem * sem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwmp_sem_cache, &mem.anon);
        if (rc < 0) {
                mem.sem = err_ptr(rc);
        }/* else {} */
        return mem.sem;
#elif defined(XWMPCFG_SYNC_SEM_STDC_MM) && (1 == XWMPCFG_SYNC_SEM_STDC_MM)
        struct xwmp_sem * sem;

        sem = malloc(sizeof(struct xwmp_sem));
        if (NULL == sem) {
                sem = err_ptr(-ENOMEM);
        } else {
                xwmp_sem_construct(sem);
        }
        return sem;
#else
        union {
                struct xwmp_sem * sem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmp_sem), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmp_sem_construct(mem.sem);
        } else {
                mem.sem = err_ptr(-ENOMEM);
        }
        return mem.sem;
#endif
}

/**
 * @brief 释放信号量对象
 * @param[in] sem: 信号量对象的指针
 */
static __xwmp_code
void xwmp_sem_free(struct xwmp_sem * sem)
{
#if defined(XWMPCFG_SYNC_SEM_MEMSLICE) && (1 == XWMPCFG_SYNC_SEM_MEMSLICE)
        xwmm_memslice_free(&xwmp_sem_cache, sem);
#elif defined(XWMPCFG_SYNC_SEM_STDC_MM) && (1 == XWMPCFG_SYNC_SEM_STDC_MM)
        xwmp_sem_destruct(sem);
        free(sem);
#else
        xwmp_sem_destruct(sem);
        xwmm_kma_free(sem);
#endif
}

/**
 * @brief 信号量对象的构造函数
 * @param[in] sem: 信号量对象的指针
 */
static __xwmp_code
void xwmp_sem_construct(struct xwmp_sem * sem)
{
        xwmp_synobj_construct(&sem->synobj);
}

/**
 * @brief 信号量对象的析构函数
 * @param[in] sem: 信号量对象的指针
 */
static __xwmp_code
void xwmp_sem_destruct(struct xwmp_sem * sem)
{
        xwmp_synobj_destruct(&sem->synobj);
}

/**
 * @brief 静态信号量对象的垃圾回收函数
 * @param[in] sem: 信号量对象的指针
 */
static __xwmp_code
xwer_t xwmp_sem_sgc(void * sem)
{
        xwmp_sem_destruct((struct xwmp_sem *)sem);
        return XWOK;
}

/**
 * @brief 动态信号量对象的垃圾回收函数
 * @param[in] sem: 信号量对象的指针
 */
static __xwmp_code
xwer_t xwmp_sem_dgc(void * sem)
{
        xwmp_sem_free((struct xwmp_sem *)sem);
        return XWOK;
}

__xwmp_api
xwer_t xwmp_sem_acquire(struct xwmp_sem * sem, xwsq_t tik)
{
        return xwmp_synobj_acquire(&sem->synobj, tik);
}

__xwmp_api
xwer_t xwmp_sem_release(struct xwmp_sem * sem, xwsq_t tik)
{
        return xwmp_synobj_release(&sem->synobj, tik);
}

__xwmp_api
xwer_t xwmp_sem_grab(struct xwmp_sem * sem)
{
        return xwmp_synobj_grab(&sem->synobj);
}

__xwmp_api
xwer_t xwmp_sem_put(struct xwmp_sem * sem)
{
        return xwmp_synobj_put(&sem->synobj);
}

__xwmp_api
xwer_t xwmp_sem_create(struct xwmp_sem ** sembuf, xwid_t type,
                       xwssq_t val, xwssq_t max)
{
        struct xwmp_sem * sem;
        xwer_t rc;

        *sembuf = NULL;
        sem = xwmp_sem_alloc();
        if (__xwcc_unlikely(is_err(sem))) {
                rc = ptr_err(sem);
        } else {
                rc = -ETYPE;
                switch (type) {
#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
                case XWMP_SEM_TYPE_PIPELINE:
                        rc = xwmp_plsem_activate(sem, val, max, xwmp_sem_dgc);
                        break;
#endif
#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
                case XWMP_SEM_TYPE_RT:
                        rc = xwmp_rtsem_activate(sem, val, max, xwmp_sem_dgc);
                        break;
#endif
                }
                if (__xwcc_unlikely(rc < 0)) {
                        xwmp_sem_free(sem);
                } else {
                        *sembuf = sem;
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_sem_delete(struct xwmp_sem * sem, xwsq_t tik)
{
        return xwmp_sem_release(sem, tik);
}

__xwmp_api
xwer_t xwmp_sem_fini(struct xwmp_sem * sem)
{
        return xwmp_sem_put(sem);
}

#if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
__xwmp_api
xwer_t xwmp_sem_bind(struct xwmp_sem * sem, struct xwmp_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_sem_grab(sem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_grab;
        }

        switch (sem->type) {
#  if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
        case XWMP_SEM_TYPE_PIPELINE:
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                rc = xwmp_sel_obj_bind(evt, &sem->synobj, pos, true);
                if ((XWOK == rc) && (sem->count > 0)) {
                        rc = xwmp_sel_obj_s1i(evt, &sem->synobj);
                }
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                break;
#  endif
#  if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
        case XWMP_SEM_TYPE_RT:
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                rc = xwmp_sel_obj_bind(evt, &sem->synobj, pos, true);
                if ((XWOK == rc) && (sem->count > 0)) {
                        rc = xwmp_sel_obj_s1i(evt, &sem->synobj);
                }
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                break;
#  endif
        default:
                rc = -ETYPE;
                xwmp_sem_put(sem);
                break;
        }

err_sem_grab:
        return rc;
}

__xwmp_api
xwer_t xwmp_sem_unbind(struct xwmp_sem * sem, struct xwmp_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        switch (sem->type) {
#  if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
        case XWMP_SEM_TYPE_PIPELINE:
                xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
                rc = xwmp_sel_obj_unbind(evt, &sem->synobj, true);
                if (XWOK == rc) {
                        rc = xwmp_sel_obj_c0i(evt, &sem->synobj);
                }
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                xwmp_sem_put(sem);
                break;
#  endif
#  if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
        case XWMP_SEM_TYPE_RT:
                xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
                rc = xwmp_sel_obj_unbind(evt, &sem->synobj, true);
                if (XWOK == rc) {
                        rc = xwmp_sel_obj_c0i(evt, &sem->synobj);
                }
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                xwmp_sem_put(sem);
                break;
#  endif
        default:
                rc = -ETYPE;
                break;
        }
        return rc;
}
#endif

#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
/**
 * @brief XWMP API：激活并初始化管道信号量对象
 * @param[in] sem: 信号量对象的指针
 * @param[in] val: 信号量的初始值
 * @param[in] max: 信号量的最大值
 * @param[in] gcfunc: 垃圾回收函数的指针
 */
static __xwmp_code
xwer_t xwmp_plsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwmp_synobj_activate(&sem->synobj, gcfunc);
        if (__xwcc_likely(rc < 0)) {
                goto err_synobj_activate;
        }
        sem->max = max;
        xwmp_plwq_init(&sem->wq.pl);
        sem->type = XWMP_SEM_TYPE_PIPELINE;
        sem->count = val;
        return XWOK;

err_synobj_activate:
        return rc;
}

__xwmp_api
xwer_t xwmp_plsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max)
{
        xwmp_sem_construct(sem);
        return xwmp_plsem_activate(sem, val, max, xwmp_sem_sgc);
}

__xwmp_api
xwer_t xwmp_plsem_freeze(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (__xwcc_unlikely(sem->count < 0)) {
                rc = -EALREADY;
        } else {
                sem->count = XWMP_SEM_NEGTIVE;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                struct xwmp_evt * evt;
                struct xwmp_synobj * synobj;

                synobj = &sem->synobj;
                xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                if (NULL != evt) {
                        xwmp_sel_obj_c0i(evt, synobj);
                }
#  endif
        }
        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
        return rc;
}

__xwmp_api
xwer_t xwmp_plsem_thaw(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (__xwcc_unlikely(sem->count >= 0)) {
                rc = -EALREADY;
        } else {
                sem->type = XWMP_SEM_TYPE_PIPELINE;
                sem->count = 0;
        }
        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
        return rc;
}

/**
 * @brief 中断管道信号量等待队列中的一个节点
 * @param[in] sem: 信号量对象的指针
 * @param[in] wqn: 等待队列节点
 */
__xwmp_code
xwer_t xwmp_plsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        xwmp_splk_lock(&wqn->lock);
        rc = xwmp_plwq_remove_locked(&sem->wq.pl, wqn);
        if (XWOK == rc) {
                wqn->wq = NULL;
                wqn->type = XWMP_WQTYPE_UNKNOWN;
                xwaop_store(xwsq_t, &wqn->reason,
                            xwaop_mo_release, XWMP_WQN_REASON_INTR);
                cb = wqn->cb;
                wqn->cb = NULL;
                xwmp_splk_unlock(&wqn->lock);
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                cb(wqn->owner);
        } else {
                xwmp_splk_unlock(&wqn->lock);
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_plsem_post(struct xwmp_sem * sem)
{
        struct xwmp_wqn * wqn;
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (sem->count < 0) {
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwmp_plwq_choose_locked(&sem->wq.pl);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_UP);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        cb(wqn->owner);
                        rc = XWOK;
                } else {
                        if (sem->count < sem->max) {
                                sem->count++;
                                rc = XWOK;
                        } else {
                                rc = -ERANGE;
                        }
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                        if (sem->count > 0) {
                                struct xwmp_evt * evt;
                                struct xwmp_synobj * synobj;

                                synobj = &sem->synobj;
                                xwmb_mp_load_acquire(struct xwmp_evt *,
                                                     evt,
                                                     &synobj->sel.evt);
                                if (NULL != evt) {
                                        xwmp_sel_obj_s1i(evt, synobj);
                                }
                        }
#  endif
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_plsem_wait(struct xwmp_sem * sem)
{
        return xwmp_plsem_wait_to(sem, XWTM_MAX);
}

static __xwmp_code
xwer_t xwmp_plsem_blkthd_to_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_skd * xwskd,
                                            struct xwmp_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq)
{
        struct xwmp_tt * xwtt;
        xwpr_t dprio;
        xwsq_t reason;
        xwsq_t wkuprs;
        xwer_t rc;

        xwtt = &xwskd->tt;

        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_SLEEPING |
                     XWMP_SKDOBJ_ST_READY | XWMP_SKDOBJ_ST_STANDBY |
                     XWMP_SKDOBJ_ST_FROZEN | XWMP_SKDOBJ_ST_MIGRATING)
                    & thd->state);
        /* 检查是否被中断 */
        if ((XWMP_SKDOBJ_ST_FREEZABLE | XWMP_SKDOBJ_ST_EXITING) & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                rc = -EINTR;
                goto err_intr;
        }
        /* 加入等待队列 */
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_plwq_locked(thd, &sem->wq.pl, XWMP_WQTYPE_PLSEM);
        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);

        /* 加入时间树 */
        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwmp_splk_lock(&thd->stlock);
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_tt_add_locked(thd, xwtt, to, cpuirq);
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_wakelock_unlock(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwmp_skd_wakelock_lock(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        wkuprs = xwaop_load(xwsq_t, &thd->ttn.wkuprs, xwaop_mo_relaxed);
        if (XWMP_WQN_REASON_INTR == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWMP_WQN_REASON_UP == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_plwq_lock_cpuirq(&sem->wq.pl);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_plwq_remove_locked(&sem->wq.pl, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &thd->wqn.reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                xwmp_plwq_lock_cpuirq(&sem->wq.pl);
                xwmp_splk_unlock(&thd->wqn.lock);
                rc = xwmp_plwq_remove_locked(&sem->wq.pl, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &thd->wqn.reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
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

static __xwmp_code
xwer_t xwmp_plsem_test(struct xwmp_sem * sem, struct xwmp_skd * xwskd,
                       struct xwmp_thd * thd, xwtm_t to)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_skd_wakelock_lock(xwskd);
                if (__xwcc_unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                        rc = -EINTR;
                } else {
                        rc = xwmp_plsem_blkthd_to_unlkwq_cpuirqrs(sem, xwskd, thd,
                                                                  to, cpuirq);
                        xwmp_skd_wakelock_unlock(xwskd);
                }
        } else {
                sem->count--;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
#  endif
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                rc = XWOK;
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_plsem_wait_to(struct xwmp_sem * sem, xwtm_t to)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        struct xwmp_syshwt * hwt;
        xwtm_t now;
        xwer_t rc;

        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        hwt = &xwskd->tt.hwt;
        now = xwmp_syshwt_get_timetick(hwt);
        if (__xwcc_unlikely(xwtm_cmp(to, now) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__xwcc_unlikely(xwtm_cmp(to, now) == 0)) {
                rc = xwmp_plsem_trywait(sem);
                if (__xwcc_unlikely(rc < 0)) {
                        if (-ENODATA == rc) {
                                rc = -ETIMEDOUT;
                        }
                }
        } else if (!xwmp_skd_tstpmpt(xwskd)) {
                rc = -ECANNOTPMPT;
#  if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        } else if (!xwmp_skd_tstbh(xwskd)) {
                rc = -ECANNOTBH;
#  endif
        } else {
                rc = xwmp_plsem_test(sem, xwskd, cthd, to);
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_plsem_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                         struct xwmp_thd * thd,
                                         struct xwmp_skd * xwskd,
                                         xwreg_t cpuirq)
{
        xwpr_t dprio;
        xwsq_t reason;
        xwer_t rc;

        /* 加入等待队列 */
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_SLEEPING |
                     XWMP_SKDOBJ_ST_READY | XWMP_SKDOBJ_ST_STANDBY |
                     XWMP_SKDOBJ_ST_FROZEN | XWMP_SKDOBJ_ST_MIGRATING)
                    & thd->state);
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_UNINTERRUPTED);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_plwq_locked(thd, &sem->wq.pl, XWMP_WQTYPE_PLSEM);
        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        if (XWMP_WQN_REASON_UP == reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_plsem_test_unintr(struct xwmp_sem * sem,
                              struct xwmp_thd * thd, struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_plsem_blkthd_unlkwq_cpuirqrs(sem, thd, xwskd, cpuirq);
        } else {
                sem->count--;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
#  endif
                xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
                rc = XWOK;
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_plsem_wait_unintr(struct xwmp_sem * sem)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        xwer_t rc;

        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        if (!xwmp_skd_tstpmpt(xwskd)) {
                rc = -ECANNOTPMPT;
#  if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        } else if (!xwmp_skd_tstbh(xwskd)) {
                rc = -ECANNOTBH;
#  endif
        } else {
                rc = xwmp_plsem_test_unintr(sem, cthd, xwskd);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_plsem_trywait(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwmp_plwq_lock_cpuirqsv(&sem->wq.pl, &cpuirq);
        if (sem->count > 0) {
                sem->count--;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt,
                                             &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
#  endif
        } else {
                rc = -ENODATA;
        }
        xwmp_plwq_unlock_cpuirqrs(&sem->wq.pl, cpuirq);
        return rc;
}
#endif

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
/**
 * @brief 激活并初始化实时信号量对象
 * @param[in] sem: 信号量对象的指针
 * @param[in] val: 信号量的初始值
 * @param[in] max: 信号量的最大值
 * @param[in] gcfunc: 垃圾回收函数的指针
 */
static __xwmp_code
xwer_t xwmp_rtsem_activate(struct xwmp_sem * sem, xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwmp_synobj_activate(&sem->synobj, gcfunc);
        if (__xwcc_likely(rc < 0)) {
                goto err_synobj_activate;
        }
        sem->max = max;
        xwmp_rtwq_init(&sem->wq.rt);
        sem->type = XWMP_SEM_TYPE_RT;
        sem->count = val;
        return XWOK;

err_synobj_activate:
        return rc;
}

__xwmp_api
xwer_t xwmp_rtsem_init(struct xwmp_sem * sem, xwssq_t val, xwssq_t max)
{
        xwmp_sem_construct(sem);
        return xwmp_rtsem_activate(sem, val, max, xwmp_sem_sgc);
}

__xwmp_api
xwer_t xwmp_rtsem_freeze(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (__xwcc_unlikely(sem->count < 0)) {
                rc = -EALREADY;
        } else {
                sem->count = XWMP_SEM_NEGTIVE;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                struct xwmp_evt * evt;
                struct xwmp_synobj * synobj;

                synobj = &sem->synobj;
                xwmb_mp_load_acquire(struct xwmp_evt *,
                                     evt, &synobj->sel.evt);
                if (NULL != evt) {
                        xwmp_sel_obj_c0i(evt, synobj);
                }
#  endif
        }
        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);

        return rc;
}

__xwmp_api
xwer_t xwmp_rtsem_thaw(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (__xwcc_unlikely(sem->count >= 0)) {
                rc = -EALREADY;
        } else {
                sem->type = XWMP_SEM_TYPE_RT;
                sem->count = 0;
        }
        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
        return rc;
}

/**
 * @brief 中断实时信号量等待队列中的一个节点
 * @param[in] sem: 信号量对象的指针
 * @param[in] wqn: 等待队列节点
 */
__xwmp_code
xwer_t xwmp_rtsem_intr(struct xwmp_sem * sem, struct xwmp_wqn * wqn)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        xwmp_splk_lock(&wqn->lock);
        rc = xwmp_rtwq_remove_locked(&sem->wq.rt, wqn);
        if (XWOK == rc) {
                wqn->wq = NULL;
                wqn->type = XWMP_WQTYPE_UNKNOWN;
                xwaop_store(xwsq_t, &wqn->reason,
                            xwaop_mo_release, XWMP_WQN_REASON_INTR);
                cb = wqn->cb;
                wqn->cb = NULL;
                xwmp_splk_unlock(&wqn->lock);
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                cb(wqn->owner);
        } else {
                xwmp_splk_unlock(&wqn->lock);
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_rtsem_post(struct xwmp_sem * sem)
{
        struct xwmp_wqn * wqn;
        xwreg_t cpuirq;
        xwmp_wqn_f cb;
        xwer_t rc;

        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (__xwcc_unlikely(sem->count < 0)) {
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwmp_rtwq_choose_locked(&sem->wq.rt);
                if (wqn) {
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_UP);
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        cb(wqn->owner);
                        rc = XWOK;
                } else {
                        if (sem->count < sem->max) {
                                sem->count++;
                                rc = XWOK;
                        } else {
                                rc = -ERANGE;
                        }
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                        if (sem->count > 0) {
                                struct xwmp_evt * evt;
                                struct xwmp_synobj * synobj;

                                synobj = &sem->synobj;
                                xwmb_mp_load_acquire(struct xwmp_evt *,
                                                     evt,
                                                     &synobj->sel.evt);
                                if (NULL != evt) {
                                        xwmp_sel_obj_s1i(evt, synobj);
                                }
                        }
#  endif
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_rtsem_wait(struct xwmp_sem * sem)
{
        return xwmp_rtsem_wait_to(sem, XWTM_MAX);
}

static __xwmp_code
xwer_t xwmp_rtsem_blkthd_to_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                            struct xwmp_skd * xwskd,
                                            struct xwmp_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwmp_tt * xwtt;
        xwsq_t reason;
        xwsq_t wkuprs;
        xwpr_t dprio;

        xwtt = &xwskd->tt;

        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_SLEEPING |
                     XWMP_SKDOBJ_ST_READY | XWMP_SKDOBJ_ST_STANDBY |
                     XWMP_SKDOBJ_ST_FROZEN | XWMP_SKDOBJ_ST_MIGRATING)
                    & thd->state);
        /* 检查是否被中断 */
        if (XWMP_SKDOBJ_ST_FREEZABLE & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                rc = -EINTR;
                goto err_intr;
        }
        /* 加入等待队列 */
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_rtwq_locked(thd, &sem->wq.rt, XWMP_WQTYPE_RTSEM, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);

        /* 加入时间树 */
        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwmp_splk_lock(&thd->stlock);
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_tt_add_locked(thd, xwtt, to, cpuirq);
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_wakelock_unlock(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwmp_skd_wakelock_lock(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        wkuprs = xwaop_load(xwsq_t, &thd->ttn.wkuprs, xwaop_mo_relaxed);
        if (XWMP_WQN_REASON_INTR == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWMP_WQN_REASON_UP == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&sem->wq.rt);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&sem->wq.rt, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &thd->wqn.reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&sem->wq.rt);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&sem->wq.rt, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &thd->wqn.reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
                        if (XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if (XWMP_WQN_REASON_UP == reason) {
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

static __xwmp_code
xwer_t xwmp_rtsem_test(struct xwmp_sem * sem, struct xwmp_skd * xwskd,
                       struct xwmp_thd * thd, xwtm_t to)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_skd_wakelock_lock(xwskd);
                if (__xwcc_unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                        rc = -EINTR;
                } else {
                        rc = xwmp_rtsem_blkthd_to_unlkwq_cpuirqrs(sem, xwskd, thd,
                                                                  to, cpuirq);
                        xwmp_skd_wakelock_unlock(xwskd);
                }
        } else {
                sem->count--;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
#  endif
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                rc = XWOK;
        }

        return rc;
}

__xwmp_api
xwer_t xwmp_rtsem_wait_to(struct xwmp_sem * sem, xwtm_t to)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        struct xwmp_syshwt * hwt;
        xwtm_t now;
        xwer_t rc;

        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        hwt = &xwskd->tt.hwt;
        now = xwmp_syshwt_get_timetick(hwt);
        if (__xwcc_unlikely(xwtm_cmp(to, now) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__xwcc_unlikely(xwtm_cmp(to, now) == 0)) {
                rc = xwmp_rtsem_trywait(sem);
                if (__xwcc_unlikely(rc < 0)) {
                        if (-ENODATA == rc) {
                                rc = -ETIMEDOUT;
                        }
                }
        } else if (!xwmp_skd_tstpmpt(xwskd)) {
                rc = -ECANNOTPMPT;
#  if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        } else if (!xwmp_skd_tstbh(xwskd)) {
                rc = -ECANNOTBH;
#  endif
        } else {
                rc = xwmp_rtsem_test(sem, xwskd, cthd, to);
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_rtsem_blkthd_unlkwq_cpuirqrs(struct xwmp_sem * sem,
                                         struct xwmp_thd * thd,
                                         struct xwmp_skd * xwskd,
                                         xwreg_t cpuirq)
{
        xwer_t rc;
        xwsq_t reason;
        xwpr_t dprio;

        /* 加入等待队列 */
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_SLEEPING |
                     XWMP_SKDOBJ_ST_READY | XWMP_SKDOBJ_ST_STANDBY |
                     XWMP_SKDOBJ_ST_FROZEN | XWMP_SKDOBJ_ST_MIGRATING)
                    & thd->state);
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_UNINTERRUPTED);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_rtwq_locked(thd, &sem->wq.rt, XWMP_WQTYPE_RTSEM, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        if (XWMP_WQN_REASON_UP == reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_rtsem_test_unintr(struct xwmp_sem * sem,
                              struct xwmp_thd * thd, struct xwmp_skd * xwskd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (sem->count <= 0) {
                rc = xwmp_rtsem_blkthd_unlkwq_cpuirqrs(sem, thd, xwskd, cpuirq);
        } else {
                sem->count--;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *, evt, &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
#  endif
                xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                rc = XWOK;
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_rtsem_wait_unintr(struct xwmp_sem * sem)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        xwer_t rc;

        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        if (!xwmp_skd_tstpmpt(xwskd)) {
                rc = -ECANNOTPMPT;
#  if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        } else if (!xwmp_skd_tstbh(xwskd)) {
                rc = -ECANNOTBH;
#  endif
        } else {
                rc = xwmp_rtsem_test_unintr(sem, cthd, xwskd);
        }
        return rc;
}
__xwmp_api
xwer_t xwmp_rtsem_trywait(struct xwmp_sem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwmp_rtwq_lock_cpuirqsv(&sem->wq.rt, &cpuirq);
        if (sem->count > 0) {
                sem->count--;
#  if defined(XWMPCFG_SYNC_EVT) && (1 == XWMPCFG_SYNC_EVT)
                if (0 == sem->count) {
                        struct xwmp_evt * evt;
                        struct xwmp_synobj * synobj;

                        synobj = &sem->synobj;
                        xwmb_mp_load_acquire(struct xwmp_evt *,
                                             evt,
                                             &synobj->sel.evt);
                        if (NULL != evt) {
                                xwmp_sel_obj_c0i(evt, synobj);
                        }
                }
#  endif
        } else {
                rc = -ENODATA;
        }
        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
        return rc;
}
#endif

__xwmp_api
xwer_t xwmp_sem_getvalue(struct xwmp_sem * sem, xwssq_t * sval)
{
        *sval = sem->count;
        return XWOK;
}

/**
 * @brief XWMP API：获取信号量的类型
 * @param[in] sem: 信号量对象的指针
 * @param[out] type: 指向缓冲区的指针，通过此缓冲区返回类型值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
__xwmp_api
xwer_t xwmp_sem_gettype(struct xwmp_sem * sem, xwid_t * type)
{
        *type = sem->type;
        return XWOK;
}
