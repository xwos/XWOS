/**
 * @file
 * @brief 玄武OS MP锁机制：互斥锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/mtxtree.h>
#include <xwos/mp/thrd.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWMPCFG_LOCK_MTX_MEMSLICE */
#include <xwos/mp/lock/mtx.h>

static __xwmp_code
void xwmp_mtx_construct(struct xwmp_mtx * mtx);

static __xwmp_code
void xwmp_mtx_destruct(struct xwmp_mtx * mtx);

static __xwmp_code
struct xwmp_mtx * xwmp_mtx_alloc(void);

static __xwmp_code
void xwmp_mtx_free(struct xwmp_mtx * mtx);

static __xwmp_code
xwer_t xwmp_mtx_activate(struct xwmp_mtx * mtx, xwpr_t sprio,
                         xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_mtx_gc(void * mtx);

static __xwmp_code
xwer_t xwmp_mtx_chprio_once(struct xwmp_mtx * mtx,
                            struct xwmp_tcb ** ptcb,
                            xwsq_t * seq);

static __xwmp_code
void xwmp_mtx_chprio(struct xwmp_mtx * mtx);

static __xwmp_code
xwer_t xwmp_mtx_do_timedblkthrd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                                struct xwmp_tcb * tcb,
                                                xwtm_t * xwtm,
                                                xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_mtx_do_timedlock(struct xwmp_mtx * mtx,
                             struct xwmp_tcb * tcb,
                             xwtm_t * xwtm);

static __xwmp_code
xwer_t xwmp_mtx_do_blkthrd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                           struct xwmp_tcb * tcb,
                                           xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_mtx_do_lock_unintr(struct xwmp_mtx * mtx, struct xwmp_tcb * tcb);

#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief 结构体xwmp_mtx的对象缓存
 */
static __xwmp_data struct xwmm_memslice * xwmp_mtx_cache = NULL;

/**
 * @brief 结构体xwmp_mtx的对象缓存的名字
 */
__xwmp_rodata const char xwmp_mtx_cache_name[] = "xwos.mp.lk.mtx.cache";
#endif /* XWMPCFG_LOCK_MTX_MEMSLICE */

#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化结构体xwmp_mtx的对象缓存
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_mtx_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwmp_mtx),
                                  xwmp_mtx_cache_name,
                                  (ctor_f)xwmp_mtx_construct,
                                  (dtor_f)xwmp_mtx_destruct);
        if (__xwcc_unlikely(rc < 0)) {
                xwmp_mtx_cache = NULL;
        } else {
                xwmp_mtx_cache = msa;
        }
        return rc;
}
#endif /* XWMPCFG_LOCK_MTX_MEMSLICE */

/**
 * @brief 从互斥锁对象缓存中申请对象
 * @return 互斥锁对象的指针
 */
static __xwmp_code
struct xwmp_mtx * xwmp_mtx_alloc(void)
{
#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
        union {
                struct xwmp_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwmp_mtx_cache, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        }/* else {} */
        return mem.mtx;
#else
        union {
                struct xwmp_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmp_mtx), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc ) {
                xwmp_mtx_construct(mem.mtx);
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
static __xwmp_code
void xwmp_mtx_free(struct xwmp_mtx * mtx)
{
#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
        xwmm_memslice_free(xwmp_mtx_cache, mtx);
#else /* XWMPCFG_LOCK_MTX_MEMSLICE */
        xwmp_mtx_destruct(mtx);
        xwmm_kma_free(mtx);
#endif /* !XWMPCFG_LOCK_MTX_MEMSLICE */
}

/**
 * @brief 互斥锁对象的构造函数
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwmp_code
void xwmp_mtx_construct(struct xwmp_mtx * mtx)
{
        xwos_object_construct(&mtx->xwobj, XWOS_OBJ_MTX);
}

/**
 * @brief 互斥锁对象的析构函数
 * @param mtx: (I) 互斥锁对象的指针
 */
static __xwmp_code
void xwmp_mtx_destruct(struct xwmp_mtx * mtx)
{
        xwos_object_destruct(&mtx->xwobj);
}

/**
 * @brief 互斥锁对象的垃圾回收函数
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_mtx_gc(void * mtx)
{
        xwmp_mtx_free((struct xwmp_mtx *)mtx);
        return XWOK;
}

/**
 * @brief 增加对象的引用计数
 * @param mtx: (I) 互斥锁对象指针
 * @return 错误码
 */
__xwcc_inline
xwer_t xwmp_mtx_grab(struct xwmp_mtx * mtx)
{
        return xwos_object_grab(&mtx->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param mtx: (I) 互斥锁对象指针
 * @return 错误码
 */
__xwcc_inline
xwer_t xwmp_mtx_put(struct xwmp_mtx * mtx)
{
        return xwos_object_put(&mtx->xwobj);
}

/**
 * @brief 激活互斥锁对象，并初始化
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
static __xwmp_code
xwer_t xwmp_mtx_activate(struct xwmp_mtx * mtx, xwpr_t sprio,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&mtx->xwobj, gcfunc);
        if (__xwcc_likely(XWOK == rc)) {
                if (sprio >= XWMP_SKD_PRIORITY_RT_NUM) {
                        sprio = XWMP_SKD_PRIORITY_RT_MAX;
                } else if (sprio <= XWMP_SKD_PRIORITY_INVALID) {
                        sprio = XWMP_SKD_PRIORITY_RT_MIN;
                }
                mtx->sprio = sprio;
                mtx->dprio = sprio;
                xwmp_rtwq_init(&mtx->rtwq);
                mtx->ownertree = NULL;
                mtx->reentrant = 0;
                xwlib_rbtree_init_node(&mtx->rbnode);
                xwlib_bclst_init_node(&mtx->rbbuddy);
        } /* else {} */
        return rc;
}

/**
 * @brief XWMP API：静态初始化互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwmp_api
xwer_t xwmp_mtx_init(struct xwmp_mtx * mtx, xwpr_t sprio)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        xwmp_mtx_construct(mtx);
        return xwmp_mtx_activate(mtx, sprio, NULL);
}

/**
 * @brief XWMP API：销毁静态初始化的互斥锁对象
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwmp_api
xwer_t xwmp_mtx_destroy(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        return xwmp_mtx_put(mtx);
}

/**
 * @brief XWMP API：动态创建互斥锁
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回对象的指针
 * @param sprio: (I) 互斥锁的静态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwmp_api
xwer_t xwmp_mtx_create(struct xwmp_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwmp_mtx * mtx;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        mtx = xwmp_mtx_alloc();
        if (__xwcc_unlikely(is_err(mtx))) {
                rc = ptr_err(mtx);
        } else {
                rc = xwmp_mtx_activate(mtx, sprio, xwmp_mtx_gc);
                if (__xwcc_unlikely(rc < 0)) {
                        xwmp_mtx_free(mtx);
                        mtx = err_ptr(rc);
                } else {
                        *ptrbuf = mtx;
                }
        }
        return rc;
}

/**
 * @brief XWMP API：删除动态创建的互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *mtx* ，不可重入
 */
__xwmp_api
xwer_t xwmp_mtx_delete(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        return xwmp_mtx_put(mtx);
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
static __xwmp_code
xwer_t xwmp_mtx_chprio_once(struct xwmp_mtx * mtx,
                            struct xwmp_tcb ** ptcb,
                            xwsq_t * seq)
{
        struct xwmp_tcb * owner;
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        *ptcb = NULL;
        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
                if (mtx->dprio == mtx->rtwq.max_prio) {
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                } else if (mtx->rtwq.max_prio <= mtx->sprio) {
                        if (mtx->dprio == mtx->sprio) {
                                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        } else {
                                mtx->dprio = mtx->sprio;
                                mt = mtx->ownertree;
                                if (mt) {
                                        owner = xwcc_baseof(mt, struct xwmp_tcb,
                                                            mtxtree);
                                        xwmp_sqlk_wr_lock(&mt->lock);
                                        *seq = xwmp_sqlk_get_seq(&mt->lock);
                                        xwmp_mtxtree_remove_locked(mtx, mt);
                                        xwmp_mtxtree_add_locked(mtx, mt);
                                        xwmp_sqlk_wr_unlock(&mt->lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        *seq += XWMP_SQLK_GRANULARITY;
                                        *ptcb = owner;
                                } else {
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                }
                        }
                } else {
                        mtx->dprio = mtx->rtwq.max_prio;
                        mt = mtx->ownertree;
                        if (mt) {
                                owner = xwcc_baseof(mt, struct xwmp_tcb, mtxtree);
                                xwmp_sqlk_wr_lock(&mt->lock);
                                *seq = xwmp_sqlk_get_seq(&mt->lock);
                                xwmp_mtxtree_remove_locked(mtx, mt);
                                xwmp_mtxtree_add_locked(mtx, mt);
                                xwmp_sqlk_wr_unlock(&mt->lock);
                                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                *seq += XWMP_SQLK_GRANULARITY;
                                *ptcb = owner;
                        } else {
                                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        }
                }
                xwmp_mtx_put(mtx);
        } /* else {} */
        return rc;
}

/**
 * @brief 从互斥锁开始，修改互斥锁——线程链的动态优先级
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 */
static __xwmp_code
void xwmp_mtx_chprio(struct xwmp_mtx * mtx)
{
        struct xwmp_tcb * tcb;
        struct xwmp_mtxtree * mt;
        xwsq_t pseq, cseq;
        xwpr_t dprio;
        xwer_t rc;

        while (mtx) {
                rc = xwmp_mtx_chprio_once(mtx, &tcb, &pseq);
                if (__xwcc_unlikely(rc < 0)) {
                        break;
                }/* else {} */
                if (__xwcc_likely(NULL == tcb)) {
                        break;
                }/* else {} */
                mt = &tcb->mtxtree;
                cseq = xwmp_sqlk_rd_begin(&mt->lock);
                if (cseq == pseq) {
                        dprio = tcb->sprio > mt->maxprio ?
                                tcb->sprio : mt->maxprio;
                        mtx = NULL;
                        rc = xwmp_thrd_chprio_once(tcb, dprio, &mtx);
                        if (xwmp_sqlk_rd_retry(&mt->lock, cseq) || (rc < 0)) {
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
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwer_t xwmp_mtx_intr(struct xwmp_mtx * mtx, struct xwmp_tcb * tcb)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
                xwmp_splk_lock(&tcb->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &tcb->wqn);
                if (XWOK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        tcb->wqn.cb = NULL;
                        xwmp_splk_unlock(&tcb->wqn.lock);
                        xwmp_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&tcb->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        xwmp_mtx_put(mtx);
                        xwmp_thrd_wakeup(tcb);
                        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &tcb->xwskd);
                        xwmp_skd_chkpmpt(xwskd);
                } else {
                        xwmp_splk_unlock(&tcb->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_put(mtx);
                }
        }/* else {} */
        return rc;
}

/**
 * @brief XWMP API：解锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOWNER: 线程并没有锁定此互斥锁
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_mtx_unlock(struct xwmp_mtx * mtx)
{
        xwer_t rc;
        struct xwmp_skd * local, * xwskd;
        struct xwmp_wqn * wqn;
        struct xwmp_tcb * t, * ctcb;
        struct xwmp_mtx * tmp;
        struct xwmp_mtxtree * mt;
        xwpr_t dprio;
        xwsq_t seq;
        xwreg_t cpuirq;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwmp_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = XWOK;
        local = xwmp_skd_dspmpt_lc();
        ctcb = xwmp_skd_get_ctcb(local);
        mt = &ctcb->mtxtree;
        xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree != mt) {
                rc = -EOWNER;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(local);
        } else if (mtx->reentrant > 0) {
                mtx->reentrant--;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_mtx_put(mtx);
                xwmp_skd_enpmpt(local);
        } else {
                xwmp_mtxtree_remove(mtx, mt);
                wqn = xwmp_rtwq_choose_locked(&mtx->rtwq);
                if (wqn) {
                        /* Case 1: 等待队列中有线程正在等待互斥锁 */
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_UP);
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        t = wqn->owner;
                        xwmp_splk_lock(&t->stlock);
                        xwbop_c0m(xwsq_t, &t->state,
                                  XWMP_SKDOBJ_DST_BLOCKING |
                                  XWMP_SKDOBJ_DST_UNINTERRUPTED);
                        t->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&t->stlock);
                        mt = &t->mtxtree;
                        /* 修改互斥锁的优先级, 互斥锁此时没有拥有者 */
                        if (mtx->rtwq.max_prio <= mtx->sprio) {
                                mtx->dprio = mtx->sprio;
                        } else {
                                mtx->dprio = mtx->rtwq.max_prio;
                        }
                        xwmp_mtxtree_add(mtx, mt);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        rc = xwmp_thrd_wakeup(t);
                        if (__xwcc_unlikely(rc < 0)) {
                                /* 如果t已经在其他CPU开始运行，
                                   它的动态优先级需要被修改。*/
                                seq = xwmp_sqlk_rd_begin(&mt->lock);
                                dprio = t->sprio > mt->maxprio ?
                                        t->sprio : mt->maxprio;
                                tmp = NULL;
                                rc = xwmp_thrd_chprio_once(t, dprio, &tmp);
                                if ((!xwmp_sqlk_rd_retry(&mt->lock, seq)) &&
                                    (XWOK == rc) && (tmp)) {
                                        xwmp_mtx_chprio(tmp);
                                }/* else {} */
                        }/* else {} */
                        xwmp_mtx_put(mtx);
                        /* change the priority of old owner */
                        xwmp_thrd_chprio(ctcb);
                        xwmp_skd_enpmpt(local);
                        /* 如果函数在xwmpsyn_cond_timedwait()中被调用，
                           当前线程已经不是`XWMP_SKDOBJ_DST_RUNNING'状态，
                           xwmp_skd_chkpmpt()不起作用。*/
                        xwmp_skd_chkpmpt(local);

                        /* t可能运行在不同CPU上，因此也需要检查此CPU的抢占 */
                        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &t->xwskd);
                        xwmp_skd_chkpmpt(xwskd);
                } else {
                        /* Case 2: 没有线程正在等待互斥锁 */
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_put(mtx);
                        xwmp_thrd_chprio(ctcb);
                        xwmp_skd_enpmpt(local);
                        /* 如果函数在xwmpsyn_cond_timedwait()中被调用，
                           当前线程已经不是`XWMP_SKDOBJ_DST_RUNNING'状态，
                           xwmp_skd_chkpmpt()不起作用。*/
                        xwmp_skd_chkpmpt(local);
                }
        }
        return rc;
}

/**
 * @brief XWMP API：等待上锁互斥锁
 * @param mtx: (I) 互斥锁对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwmp_inline_api
xwer_t xwmp_mtx_lock(struct xwmp_mtx * mtx)
{
        xwtm_t expected;

        expected = XWTM_MAX;
        return xwmp_mtx_timedlock(mtx, &expected);
}

/**
 * @brief XWMP API：尝试上锁互斥锁，不会阻塞调用则
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
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
__xwmp_api
xwer_t xwmp_mtx_trylock(struct xwmp_mtx * mtx)
{
        struct xwmp_skd * xwskd;
        struct xwmp_tcb * ctcb;
        struct xwmp_mtxtree * mt;
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwmp_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        rc = XWOK;
        xwskd = xwmp_skd_dspmpt_lc();
        ctcb = xwmp_skd_get_ctcb(xwskd);
        mt = &ctcb->mtxtree;
        xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwmp_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwmp_skd_enpmpt(xwskd);
        } else if (mtx->ownertree) {
                xwmp_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwmp_mtx_put(mtx);
                rc = -ENODATA;
                xwmp_skd_enpmpt(xwskd);
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwmp_thrd_chprio(ctcb);
                xwmp_skd_enpmpt(xwskd);
                xwmp_skd_chkpmpt(xwskd);
        }

err_mtx_grab:
        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_do_timedblkthrd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                                struct xwmp_tcb * tcb,
                                                xwtm_t * xwtm,
                                                xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;
        xwtm_t expected, currtick;
        xwpr_t dprio;
        xwsq_t reason;
        xwsq_t wkuprs;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &tcb->xwskd);
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        currtick = xwmp_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);

        /* 加入等待队列 */
        xwmp_splk_lock(&tcb->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_SLEEPING |
                     XWMP_SKDOBJ_DST_READY | XWMP_SKDOBJ_DST_STANDBY |
                     XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_MIGRATING)
                    & tcb->state);
        if (XWMP_SKDOBJ_DST_FREEZABLE & tcb->state) {
                xwmp_splk_unlock(&tcb->stlock);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_RUNNING);
        tcb->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_BLOCKING);
        xwmp_splk_unlock(&tcb->stlock);
        xwmp_thrd_eq_rtwq_locked(tcb, &mtx->rtwq, XWMP_WQTYPE_MTX, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);

        /* 改变优先级 */
        xwmp_mtx_chprio(mtx);

        /* 加入时间树 */
        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwmp_splk_lock(&tcb->stlock);
        xwbop_s1m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_SLEEPING);
        xwmp_splk_unlock(&tcb->stlock);
        xwmp_thrd_tt_add_locked(tcb, xwtt, expected, cpuirq);
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_wakelock_unlock(xwskd);
        xwmp_skd_enpmpt(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwmp_skd_wakelock_lock(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq_t, &tcb->wqn.reason, xwmb_modr_relaxed);
        wkuprs = xwaop_load(xwsq_t, &tcb->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWMP_WQN_REASON_INTR == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &tcb->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_SLEEPING);
                        xwmp_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWMP_WQN_REASON_UP == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &tcb->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_SLEEPING);
                        xwmp_splk_unlock(&tcb->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
                xwmp_splk_lock(&tcb->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &tcb->wqn);
                if (XWOK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        tcb->wqn.cb = NULL;
                        xwmp_splk_unlock(&tcb->wqn.lock);
                        xwmp_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&tcb->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&tcb->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        reason = xwaop_load(xwsq_t, &tcb->wqn.reason,
                                            xwmb_modr_relaxed);
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
                xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
                xwmp_splk_lock(&tcb->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &tcb->wqn);
                if (XWOK == rc) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        tcb->wqn.cb = NULL;
                        xwmp_splk_unlock(&tcb->wqn.lock);
                        xwmp_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_BLOCKING);
                        tcb->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&tcb->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&tcb->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        reason = xwaop_load(xwsq_t, &tcb->wqn.reason,
                                            xwmb_modr_relaxed);
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
        currtick = xwmp_syshwt_get_timetick(hwt);
        *xwtm = xwtm_sub(expected, currtick);
err_needfrz:
        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_do_timedlock(struct xwmp_mtx * mtx,
                             struct xwmp_tcb * tcb,
                             xwtm_t * xwtm)
{
        struct xwmp_skd * xwskd;
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        /* 当线程执行此处代码时，不可能拥有状态XWMP_SKDOBJ_DST_MIGRATING，
           因为，迁移永远发生在线程处于冻结点时。也因此可在锁xwskd->tcblistlock外
           读取tcb->xwskd。*/
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &tcb->xwskd);
        xwmp_skd_dspmpt(xwskd);
        mt = &tcb->mtxtree;
        xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(xwskd);
        } else if (mtx->ownertree) {
                rc = xwmp_skd_wakelock_lock(xwskd);
                if (__xwcc_unlikely(rc < 0)) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_skd_enpmpt(xwskd);
                        rc = -EINTR;
                } else {
                        rc = xwmp_mtx_do_timedblkthrd_unlkwq_cpuirqrs(mtx, tcb,
                                                                      xwtm, cpuirq);
                        xwmp_skd_wakelock_unlock(xwskd);
                }
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_thrd_chprio(tcb);
                xwmp_skd_enpmpt(xwskd);
                xwmp_skd_chkpmpt(xwskd);
        }
        return rc;
}

/**
 * @brief XWMP API：限时等待上锁互斥锁
 * @param mtx: (I) 互斥锁对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
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
__xwmp_api
xwer_t xwmp_mtx_timedlock(struct xwmp_mtx * mtx, xwtm_t * xwtm)
{
        xwer_t rc;
        struct xwmp_tcb * ctcb;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwmp_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        if (__xwcc_unlikely(xwtm_cmp(*xwtm, 0) < 0)) {
                rc = -ETIMEDOUT;
        } else if (__xwcc_unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwmp_mtx_trylock(mtx);
                if (__xwcc_unlikely(rc < 0)) {
                        if (__xwcc_likely(-ENODATA == rc)) {
                                rc = -ETIMEDOUT;
                        } /* else {} */
                }/* else {} */
        } else {
                rc = xwmp_mtx_grab(mtx);
                if (__xwcc_likely(XWOK == rc)) {
                        ctcb = xwmp_skd_get_ctcb_lc();
                        rc = xwmp_mtx_do_timedlock(mtx, ctcb, xwtm);
                        if (__xwcc_unlikely(rc < 0)) {
                                xwmp_mtx_put(mtx);
                        } /* else {} */
                }/* else {} */
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_do_blkthrd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                           struct xwmp_tcb * tcb,
                                           xwreg_t cpuirq)
{
        struct xwmp_skd * xwskd;
        xwsq_t reason;
        xwpr_t dprio;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &tcb->xwskd);
        /* 加入等待队列 */
        xwmp_splk_lock(&tcb->stlock);
        dprio = tcb->dprio.r;
        xwbop_c0m(xwsq_t, &tcb->state, XWMP_SKDOBJ_DST_RUNNING);
        tcb->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        tcb->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &tcb->state,
                  XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_UNINTERRUPTED);
        xwmp_splk_unlock(&tcb->stlock);
        xwmp_thrd_eq_rtwq_locked(tcb, &mtx->rtwq, XWMP_WQTYPE_MTX, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);

        /* 改变优先级 */
        xwmp_mtx_chprio(mtx);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_enpmpt(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        reason = xwaop_load(xwsq_t, &tcb->wqn.reason, xwmb_modr_relaxed);
        if (XWMP_WQN_REASON_UP == reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }

        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_do_lock_unintr(struct xwmp_mtx * mtx, struct xwmp_tcb * tcb)
{
        struct xwmp_skd * xwskd;
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &tcb->xwskd);
        xwmp_skd_dspmpt(xwskd);
        mt = &tcb->mtxtree;
        xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(xwskd);
        } else if (mtx->ownertree) {
                rc = xwmp_mtx_do_blkthrd_unlkwq_cpuirqrs(mtx, tcb, cpuirq);
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_thrd_chprio(tcb);
                xwmp_skd_enpmpt(xwskd);
                xwmp_skd_chkpmpt(xwskd);
        }
        return rc;
}

/**
 * @brief XWMP API：等待上锁互斥锁，且等待不可被中断
 * @param mtx: (I) 互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_mtx_lock_unintr(struct xwmp_mtx * mtx)
{
        xwer_t rc;
        struct xwmp_tcb * ctcb;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHRD == xwmp_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        ctcb = xwmp_skd_get_ctcb_lc();
        rc = xwmp_mtx_do_lock_unintr(mtx, ctcb);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_do_lock_unintr;
        }
        return XWOK;

err_mtx_do_lock_unintr:
        xwmp_mtx_put(mtx);
err_mtx_grab:
        return rc;
}
