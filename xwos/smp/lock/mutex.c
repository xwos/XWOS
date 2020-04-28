/**
 * @file
 * @brief XuanWuOS的锁机制：互斥锁
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
#include <xwos/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/smp/irq.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/tt.h>
#include <xwos/smp/rtwq.h>
#include <xwos/smp/mutextree.h>
#include <xwos/smp/thread.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWSMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWSMPCFG_LOCK_MTX_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWSMPCFG_LOCK_MTX_MEMSLICE */
#include <xwos/smp/lock/mutex.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
void xwlk_mtx_construct(struct xwlk_mtx * mtx);

static __xwos_code
void xwlk_mtx_destruct(struct xwlk_mtx * mtx);

static __xwos_code
struct xwlk_mtx * xwlk_mtx_alloc(void);

static __xwos_code
void xwlk_mtx_free(struct xwlk_mtx * mtx);

static __xwos_code
xwer_t xwlk_mtx_activate(struct xwlk_mtx * mtx, xwpr_t sprio,
                         xwobj_gc_f gcfunc);

static __xwos_code
xwer_t xwlk_mtx_gc(void * mtx);

static __xwos_code
xwer_t xwlk_mtx_chprio_once(struct xwlk_mtx * mtx,
                            struct xwos_tcb ** ptcb,
                            xwsq_t * seq);

static __xwos_code
void xwlk_mtx_chprio(struct xwlk_mtx * mtx);

static __xwos_code
xwer_t xwlk_mtx_do_timedblkthrd_unlkwq_cpuirqrs(struct xwlk_mtx * mtx,
                                                struct xwos_tcb * tcb,
                                                xwtm_t * xwtm,
                                                xwreg_t cpuirq);

static __xwos_code
xwer_t xwlk_mtx_do_timedlock(struct xwlk_mtx * mtx,
                             struct xwos_tcb * tcb,
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
#if defined(XWSMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWSMPCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief 结构体xwlk_mtx的对象缓存
 */
static __xwos_data struct xwmm_memslice * xwlk_mtx_cache = NULL;

/**
 * @brief 结构体xwlk_mtx的对象缓存的名字
 */
__xwos_rodata const char xwlk_mtx_cache_name[] = "cache.mtx.xwlk";
#endif /* XWSMPCFG_LOCK_MTX_MEMSLICE */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWSMPCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief XWOS INIT CODE：初始化结构体xwlk_mtx的对象缓存
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 只可在系统初始化时使用一次
 */
__xwos_init_code
xwer_t xwlk_mtx_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwlk_mtx),
                                  xwlk_mtx_cache_name,
                                  (ctor_f)xwlk_mtx_construct,
                                  (dtor_f)xwlk_mtx_destruct);
        if (__unlikely(rc < 0)) {
                xwlk_mtx_cache = NULL;
        } else {
                xwlk_mtx_cache = msa;
        }
        return rc;
}
#endif /* XWSMPCFG_LOCK_MTX_MEMSLICE */

/**
 * @brief 从互斥锁对象缓存中申请对象
 * @return 互斥锁对象的指针
 */
static __xwos_code
struct xwlk_mtx * xwlk_mtx_alloc(void)
{
#if defined(XWSMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWSMPCFG_LOCK_MTX_MEMSLICE)
        union {
                struct xwlk_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwlk_mtx_cache, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        }/* else {} */
        return mem.mtx;
#else
        union {
                struct xwlk_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwlk_mtx), XWMM_ALIGNMENT, &mem.anon);
        if (OK == rc ) {
                xwlk_mtx_construct(mem.mtx);
        } else {
                mem.mtx = err_ptr(-ENOMEM);
        }
        return mem.mtx;
#endif
}

/**
 * @brief 释放互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwos_code
void xwlk_mtx_free(struct xwlk_mtx * mtx)
{
#if defined(XWSMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWSMPCFG_LOCK_MTX_MEMSLICE)
        xwmm_memslice_free(xwlk_mtx_cache, mtx);
#else /* XWSMPCFG_LOCK_MTX_MEMSLICE */
        xwlk_mtx_destruct(mtx);
        xwmm_kma_free(mtx);
#endif /* !XWSMPCFG_LOCK_MTX_MEMSLICE */
}

/**
 * @brief 互斥锁对象的构造函数
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwos_code
void xwlk_mtx_construct(struct xwlk_mtx * mtx)
{
        xwos_object_construct(&mtx->xwobj);
}

/**
 * @brief 互斥锁对象的析构函数
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwos_code
void xwlk_mtx_destruct(struct xwlk_mtx * mtx)
{
        xwos_object_destruct(&mtx->xwobj);
}

/**
 * @brief 互斥锁对象的垃圾回收函数
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwlk_mtx_gc(void * mtx)
{
        xwlk_mtx_free((struct xwlk_mtx *)mtx);
        return OK;
}

/**
 * @brief 激活互斥锁对象，并初始化
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
static __xwos_code
xwer_t xwlk_mtx_activate(struct xwlk_mtx * mtx, xwpr_t sprio,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&mtx->xwobj, gcfunc);
        if (__likely(OK == rc)) {
                mtx->sprio = sprio;
                mtx->dprio = sprio;
                xwos_rtwq_init(&mtx->rtwq);
                mtx->ownertree = NULL;
                mtx->reentrant = 0;
                xwlib_rbtree_init_node(&mtx->rbnode);
                xwlib_bclst_init_node(&mtx->rbbuddy);
        } /* else {} */
        return rc;
}

/**
 * @brief XWOS API：静态初始化互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval OK: OK
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
        XWOS_VALIDATE((sprio >= XWOS_SD_PRIORITY_RT_MIN),
                      "invalid-priority", -EINVAL);
        XWOS_VALIDATE((sprio <= XWOS_SD_PRIORITY_RT_MAX),
                      "invalid-priority", -EINVAL);

        xwlk_mtx_construct(mtx);
        return xwlk_mtx_activate(mtx, sprio, NULL);
}

/**
 * @brief XWOS API：销毁静态初始化的互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval OK: OK
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

        return xwlk_mtx_put(mtx);
}

/**
 * @brief XWOS API：动态创建互斥锁
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwos_api
xwer_t xwlk_mtx_create(struct xwlk_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwlk_mtx * mtx;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((sprio >= XWOS_SD_PRIORITY_RT_MIN),
                      "invalid-priority", -EINVAL);
        XWOS_VALIDATE((sprio <= XWOS_SD_PRIORITY_RT_MAX),
                      "invalid-priority", -EINVAL);

        *ptrbuf = NULL;
        mtx = xwlk_mtx_alloc();
        if (__unlikely(is_err(mtx))) {
                rc = ptr_err(mtx);
        } else {
                rc = xwlk_mtx_activate(mtx, sprio, xwlk_mtx_gc);
                if (__unlikely(rc < 0)) {
                        xwlk_mtx_free(mtx);
                        mtx = err_ptr(rc);
                } else {
                        *ptrbuf = mtx;
                }
        }
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval OK: OK
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

        return xwlk_mtx_put(mtx);
}

/**
 * @brief 修改一次互斥锁的动态优先级
 * @param mtx: (I) 互斥锁对象的指针
 * @param ptcb: (O) 指向缓冲区的指针，通过此缓冲区返回下一个需要修改动态优先级的
 *                  线程控制块对象的指针
 * @param pseq: (O) 指向缓冲区的指针，通过此缓冲区返回线程控制块中互斥锁树中的
 *                  顺序锁的顺序值
 * @return 错误码
 */
static __xwos_code
xwer_t xwlk_mtx_chprio_once(struct xwlk_mtx * mtx,
                            struct xwos_tcb ** ptcb,
                            xwsq_t * seq)
{
        struct xwos_tcb * owner;
        struct xwos_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        *ptcb = NULL;
        rc = xwlk_mtx_grab(mtx);
        if (__likely(OK == rc)) {
                xwos_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
                if (mtx->dprio == mtx->rtwq.max_prio) {
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                } else if (mtx->rtwq.max_prio <= mtx->sprio) {
                        if (mtx->dprio == mtx->sprio) {
                                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        } else {
                                mtx->dprio = mtx->sprio;
                                mt = mtx->ownertree;
                                if (mt) {
                                        owner = container_of(mt, struct xwos_tcb,
                                                             mtxtree);
                                        xwlk_sqlk_wr_lock(&mt->lock);
                                        *seq = xwlk_sqlk_get_seq(&mt->lock);
                                        xwos_mtxtree_remove_locked(mtx, mt);
                                        xwos_mtxtree_add_locked(mtx, mt);
                                        xwlk_sqlk_wr_unlock(&mt->lock);
                                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        *seq += XWLK_SQLK_GRANULARITY;
                                        *ptcb = owner;
                                } else {
                                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                }
                        }
                } else {
                        mtx->dprio = mtx->rtwq.max_prio;
                        mt = mtx->ownertree;
                        if (mt) {
                                owner = container_of(mt, struct xwos_tcb, mtxtree);
                                xwlk_sqlk_wr_lock(&mt->lock);
                                *seq = xwlk_sqlk_get_seq(&mt->lock);
                                xwos_mtxtree_remove_locked(mtx, mt);
                                xwos_mtxtree_add_locked(mtx, mt);
                                xwlk_sqlk_wr_unlock(&mt->lock);
                                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                *seq += XWLK_SQLK_GRANULARITY;
                                *ptcb = owner;
                        } else {
                                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        }
                }
                xwlk_mtx_put(mtx);
        } /* else {} */
        return rc;
}

/**
 * @brief 从互斥锁开始，修改互斥锁——线程链的动态优先级
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 */
static __xwos_code
void xwlk_mtx_chprio(struct xwlk_mtx * mtx)
{
        struct xwos_tcb * tcb;
        struct xwos_mtxtree * mt;
        xwsq_t pseq, cseq;
        xwpr_t dprio;
        xwer_t rc;

        while (mtx) {
                rc = xwlk_mtx_chprio_once(mtx, &tcb, &pseq);
                if (__unlikely(rc < 0)) {
                        break;
                }/* else {} */
                if (__likely(NULL == tcb)) {
                        break;
                }/* else {} */
                mt = &tcb->mtxtree;
                cseq = xwlk_sqlk_rd_begin(&mt->lock);
                if (cseq == pseq) {
                        dprio = tcb->sprio > mt->maxprio ?
                                tcb->sprio : mt->maxprio;
                        mtx = NULL;
                        rc = xwos_thrd_chprio_once(tcb, dprio, &mtx);
                        if (xwlk_sqlk_rd_retry(&mt->lock, cseq) || (rc < 0)) {
                                break;
                        }/* else {} */
                }/* else {} */
        }
}

/**
 * @brief 中断互斥锁等待队列中的一个线程
 * @param mtx: (I) 互斥锁对象的指针
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwlk_mtx_intr(struct xwlk_mtx * mtx, struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwlk_mtx_grab(mtx);
        if (__likely(OK == rc)) {
                xwos_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_rtwq_remove_locked(&mtx->rtwq, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwlk_mtx_chprio(mtx);
                        xwlk_mtx_put(mtx);
                        xwos_thrd_wakeup(tcb);
                        xwmb_smp_load_acquire(struct xwos_scheduler *,
                                              xwsd, &tcb->xwsd);
                        xwos_scheduler_chkpmpt(xwsd);
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwlk_mtx_put(mtx);
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWOS API：解锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval OK: OK
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
        struct xwos_scheduler * local, * xwsd;
        struct xwos_wqn * wqn;
        struct xwos_tcb * t, * ctcb;
        struct xwlk_mtx * tmp;
        struct xwos_mtxtree * mt;
        xwpr_t dprio;
        xwsq_t seq;
        xwreg_t cpuirq;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = OK;
        local = xwos_scheduler_dspmpt_lc();
        ctcb = xwos_scheduler_get_ctcb(local);
        mt = &ctcb->mtxtree;
        xwos_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree != mt) {
                rc = -EOWNER;
                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwos_scheduler_enpmpt(local);
        } else if (mtx->reentrant > 0) {
                mtx->reentrant--;
                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwlk_mtx_put(mtx);
                xwos_scheduler_enpmpt(local);
        } else {
                xwos_mtxtree_remove(mtx, mt);
                wqn = xwos_rtwq_choose_locked(&mtx->rtwq);
                if (wqn) {
                        /* Case 1: 等待队列中有线程正在等待互斥锁 */
                        wqn->wq = NULL;
                        wqn->type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_UP);
                        wqn->cb = NULL;
                        xwlk_splk_unlock(&wqn->lock);
                        t = wqn->owner;
                        xwlk_splk_lock(&t->stlock);
                        xwbop_c0m(xwsq_t, &t->state,
                                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
                        t->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&t->stlock);
                        mt = &t->mtxtree;
                        /* 修改互斥锁的优先级, 互斥锁此时没有拥有者 */
                        if (mtx->rtwq.max_prio <= mtx->sprio) {
                                mtx->dprio = mtx->sprio;
                        } else {
                                mtx->dprio = mtx->rtwq.max_prio;
                        }
                        xwos_mtxtree_add(mtx, mt);
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        rc = xwos_thrd_wakeup(t);
                        if (__unlikely(rc < 0)) {
                                /* 如果t已经在其他CPU开始运行，
                                   它的动态优先级需要被修改。*/
                                seq = xwlk_sqlk_rd_begin(&mt->lock);
                                dprio = t->sprio > mt->maxprio ?
                                        t->sprio : mt->maxprio;
                                tmp = NULL;
                                rc = xwos_thrd_chprio_once(t, dprio, &tmp);
                                if ((!xwlk_sqlk_rd_retry(&mt->lock, seq)) &&
                                    (OK == rc) && (tmp)) {
                                        xwlk_mtx_chprio(tmp);
                                }/* else {} */
                        }/* else {} */
                        xwlk_mtx_put(mtx);
                        /* change the priority of old owner */
                        xwos_thrd_chprio(ctcb);
                        xwos_scheduler_enpmpt(local);
                        /* 如果函数在xwsync_cdt_timedwait() 或
                           xwos_cthrd_timedpause()中被调用，
                           当前线程已经不是`XWSDOBJ_DST_RUNNING'状态，
                           xwos_scheduler_chkpmpt()不起作用。*/
                        xwos_scheduler_chkpmpt(local);

                        /* t可能运行在不同CPU上，因此也需要检查此CPU的抢占 */
                        xwmb_smp_load_acquire(struct xwos_scheduler *,
                                              xwsd, &t->xwsd);
                        xwos_scheduler_chkpmpt(xwsd);
                } else {
                        /* Case 2: 没有线程正在等待互斥锁 */
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwlk_mtx_put(mtx);
                        xwos_thrd_chprio(ctcb);
                        xwos_scheduler_enpmpt(local);
                        /* 如果函数在xwsync_cdt_timedwait() 或
                           xwos_cthrd_timedpause()中被调用，
                           当前线程已经不是`XWSDOBJ_DST_RUNNING'状态，
                           xwos_scheduler_chkpmpt()不起作用。*/
                        xwos_scheduler_chkpmpt(local);
                }
        }
        return rc;
}

/**
 * @brief XWOS API：尝试上锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval OK: OK
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
        struct xwos_scheduler * xwsd;
        struct xwos_tcb * ctcb;
        struct xwos_mtxtree * mt;
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwlk_mtx_grab(mtx);
        if (__unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        rc = OK;
        xwsd = xwos_scheduler_dspmpt_lc();
        ctcb = xwos_scheduler_get_ctcb(xwsd);
        mt = &ctcb->mtxtree;
        xwos_rtwq_lock_cpuirq(&mtx->rtwq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwos_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwos_scheduler_enpmpt(xwsd);
        } else if (mtx->ownertree) {
                xwos_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwlk_mtx_put(mtx);
                rc = -ENODATA;
                xwos_scheduler_enpmpt(xwsd);
        } else {
                xwos_mtxtree_add(mtx, mt);
                xwos_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwos_thrd_chprio(ctcb);
                xwos_scheduler_enpmpt(xwsd);
                xwos_scheduler_chkpmpt(xwsd);
        }

err_mtx_grab:
        return rc;
}

static __xwos_code
xwer_t xwlk_mtx_do_timedblkthrd_unlkwq_cpuirqrs(struct xwlk_mtx * mtx,
                                                struct xwos_tcb * tcb,
                                                xwtm_t * xwtm,
                                                xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;
        xwtm_t expected, currtick;
        xwpr_t dprio;
        xwsq_t rsmrs;
        xwsq_t wkuprs;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwtt = &xwsd->tt;
        hwt = &xwtt->hwt;
        currtick = xwos_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);

        /* 加入等待队列 */
        xwlk_splk_lock(&tcb->stlock);
        XWOS_BUG_ON((XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_SLEEPING |
                     XWSDOBJ_DST_READY | XWSDOBJ_DST_STANDBY |
                     XWSDOBJ_DST_FROZEN | XWSDOBJ_DST_MIGRATING)
                    & tcb->state);
        if (XWSDOBJ_DST_FREEZABLE & tcb->state) {
                xwlk_splk_unlock(&tcb->stlock);
                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_eq_rtwq_locked(tcb, &mtx->rtwq, XWOS_WQTYPE_MTX, dprio);
        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);

        /* 改变优先级 */
        xwlk_mtx_chprio(mtx);

        /* 加入时间树 */
        xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwlk_splk_lock(&tcb->stlock);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_tt_add_locked(tcb, xwtt, expected, cpuirq);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwos_cpuirq_enable_lc();
        xwos_scheduler_wakelock_unlock(xwsd);
        xwos_scheduler_enpmpt(xwsd);
        xwos_scheduler_req_swcx(xwsd);
        xwos_scheduler_wakelock_lock(xwsd);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_smp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
        wkuprs = xwaop_load(xwsq_t, &tcb->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwos_rtwq_lock_cpuirq(&mtx->rtwq);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_rtwq_remove_locked(&mtx->rtwq, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwlk_mtx_chprio(mtx);
                        rc = -ETIMEDOUT;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
                        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
                                rc = OK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if (XWOS_TTN_WKUPRS_INTR == wkuprs) {
                xwos_rtwq_lock_cpuirq(&mtx->rtwq);
                xwlk_splk_lock(&tcb->wqn.lock);
                rc = xwos_rtwq_remove_locked(&mtx->rtwq, &tcb->wqn);
                if (OK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwlk_mtx_chprio(mtx);
                        rc = -EINTR;
                } else {
                        xwlk_splk_unlock(&tcb->wqn.lock);
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
                        if (XWOS_WQN_RSMRS_INTR == rsmrs) {
                                rc = -EINTR;
                        } else if (XWOS_WQN_RSMRS_UP == rsmrs) {
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
err_needfrz:
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

        rc = OK;
        /* 当线程执行此处代码时，不可能拥有状态XWSDOBJ_DST_MIGRATING，
           因为，迁移永远发生在线程处于冻结点时。也因此可在锁xwsd->tcblistlock外
           读取tcb->xwsd。*/
        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwos_scheduler_dspmpt(xwsd);
        mt = &tcb->mtxtree;
        xwos_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwos_scheduler_enpmpt(xwsd);
        } else if (mtx->ownertree) {
                rc = xwos_scheduler_wakelock_lock(xwsd);
                if (__unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwos_scheduler_enpmpt(xwsd);
                        rc = -EINTR;
                } else {
                        rc = xwlk_mtx_do_timedblkthrd_unlkwq_cpuirqrs(mtx, tcb,
                                                                      xwtm, cpuirq);
                        xwos_scheduler_wakelock_unlock(xwsd);
                }
        } else {
                xwos_mtxtree_add(mtx, mt);
                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwos_thrd_chprio(tcb);
                xwos_scheduler_enpmpt(xwsd);
                xwos_scheduler_chkpmpt(xwsd);
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
 * @retval OK: OK
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
 *     超时返回-ETIMEDOUT时，期望等待时间会减为0。
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

        if (__unlikely(xwtm_cmp(*xwtm, 0) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwlk_mtx_trylock(mtx);
                if (__unlikely(rc < 0)) {
                        if (__likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        } /* else {} */
                }/* else {} */
        } else {
                rc = xwlk_mtx_grab(mtx);
                if (__likely(OK == rc)) {
                        ctcb = xwos_scheduler_get_ctcb_lc();
                        rc = xwlk_mtx_do_timedlock(mtx, ctcb, xwtm);
                        if (__unlikely(rc < 0)) {
                                xwlk_mtx_put(mtx);
                        } /* else {} */
                }/* else {} */
        }
        return rc;
}

static __xwos_code
xwer_t xwlk_mtx_do_blkthrd_unlkwq_cpuirqrs(struct xwlk_mtx * mtx,
                                           struct xwos_tcb * tcb,
                                           xwreg_t cpuirq)
{
        struct xwos_scheduler * xwsd;
        xwsq_t rsmrs;
        xwpr_t dprio;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        /* 加入等待队列 */
        xwlk_splk_lock(&tcb->stlock);
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state,
                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_eq_rtwq_locked(tcb, &mtx->rtwq, XWOS_WQTYPE_MTX, dprio);
        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);

        /* 改变优先级 */
        xwlk_mtx_chprio(mtx);

        /* 调度 */
        xwos_cpuirq_enable_lc();
        xwos_scheduler_enpmpt(xwsd);
        xwos_scheduler_req_swcx(xwsd);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_smp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        rsmrs = xwaop_load(xwsq_t, &tcb->wqn.rsmrs, xwmb_modr_relaxed);
        if (XWOS_WQN_RSMRS_UP == rsmrs) {
                rc = OK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }

        return rc;
}

static __xwos_code
xwer_t xwlk_mtx_do_lock_unintr(struct xwlk_mtx * mtx, struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        struct xwos_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = OK;
        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwos_scheduler_dspmpt(xwsd);
        mt = &tcb->mtxtree;
        xwos_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwos_scheduler_enpmpt(xwsd);
        } else if (mtx->ownertree) {
                rc = xwlk_mtx_do_blkthrd_unlkwq_cpuirqrs(mtx, tcb, cpuirq);
        } else {
                xwos_mtxtree_add(mtx, mt);
                xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwos_thrd_chprio(tcb);
                xwos_scheduler_enpmpt(xwsd);
                xwos_scheduler_chkpmpt(xwsd);
        }
        return rc;
}

/**
 * @brief XWOS API：等待并上锁互斥锁，且等待不可被中断
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwlk_mtx_lock_unintr(struct xwlk_mtx * mtx)
{
        xwer_t rc;
        struct xwos_tcb * ctcb;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        rc = xwlk_mtx_grab(mtx);
        if (__unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        ctcb = xwos_scheduler_get_ctcb_lc();
        rc = xwlk_mtx_do_lock_unintr(mtx, ctcb);
        if (__unlikely(rc < 0)) {
                goto err_mtx_do_lock_unintr;
        }
        return OK;

err_mtx_do_lock_unintr:
        xwlk_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
