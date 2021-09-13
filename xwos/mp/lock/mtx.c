/**
 * @file
 * @brief 玄武OS MP内核锁机制：互斥锁
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
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWMPCFG_LOCK_MTX_STDC_MM) && (1 == XWMPCFG_LOCK_MTX_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/mtxtree.h>
#include <xwos/mp/thd.h>
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
                            struct xwmp_thd ** pthd,
                            xwsq_t * seq);

static __xwmp_code
void xwmp_mtx_chprio(struct xwmp_mtx * mtx);

static __xwmp_code
xwer_t xwmp_mtx_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                               struct xwmp_thd * thd,
                                               xwtm_t * xwtm,
                                               xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_mtx_do_timedlock(struct xwmp_mtx * mtx,
                             struct xwmp_thd * thd,
                             xwtm_t * xwtm);

static __xwmp_code
xwer_t xwmp_mtx_do_blkthd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                          struct xwmp_thd * thd,
                                          xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_mtx_do_lock_unintr(struct xwmp_mtx * mtx, struct xwmp_thd * thd);

#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief 结构体xwmp_mtx的对象缓存
 */
static __xwmp_data struct xwmm_memslice xwmp_mtx_cache;

/**
 * @brief 结构体xwmp_mtx的对象缓存的名字
 */
const __xwmp_rodata char xwmp_mtx_cache_name[] = "xwos.mp.lk.mtx.cache";
#endif

#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化结构体xwmp_mtx的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_mtx_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwmp_mtx_cache, zone_origin, zone_size,
                                sizeof(struct xwmp_mtx),
                                xwmp_mtx_cache_name,
                                (ctor_f)xwmp_mtx_construct,
                                (dtor_f)xwmp_mtx_destruct);
        return rc;
}
#endif

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

        rc = xwmm_memslice_alloc(&xwmp_mtx_cache, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        }/* else {} */
        return mem.mtx;
#elif defined(XWMPCFG_LOCK_MTX_STDC_MM) && (1 == XWMPCFG_LOCK_MTX_STDC_MM)
        struct xwmp_mtx * mtx;

        mtx = malloc(sizeof(struct xwmp_mtx));
        if (NULL == mtx) {
                mtx = err_ptr(-ENOMEM);
        }
        return mtx;
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
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwmp_code
void xwmp_mtx_free(struct xwmp_mtx * mtx)
{
#if defined(XWMPCFG_LOCK_MTX_MEMSLICE) && (1 == XWMPCFG_LOCK_MTX_MEMSLICE)
        xwmm_memslice_free(&xwmp_mtx_cache, mtx);
#elif defined(XWMPCFG_LOCK_MTX_STDC_MM) && (1 == XWMPCFG_LOCK_MTX_STDC_MM)
        xwmp_mtx_destruct(mtx);
        free(mtx);
#else
        xwmp_mtx_destruct(mtx);
        xwmm_kma_free(mtx);
#endif
}

/**
 * @brief 互斥锁对象的构造函数
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwmp_code
void xwmp_mtx_construct(struct xwmp_mtx * mtx)
{
        xwos_object_construct(&mtx->xwobj);
}

/**
 * @brief 互斥锁对象的析构函数
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwmp_code
void xwmp_mtx_destruct(struct xwmp_mtx * mtx)
{
        xwos_object_destruct(&mtx->xwobj);
}

/**
 * @brief 互斥锁对象的垃圾回收函数
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_mtx_gc(void * mtx)
{
        xwmp_mtx_free((struct xwmp_mtx *)mtx);
        return XWOK;
}

/**
 * @brief 激活互斥锁对象，并初始化
 * @param[in] mtx: 互斥锁对象的指针
 * @param[in] sprio: 互斥锁的静态优先级
 * @param[in] gcfunc: 垃圾回收函数的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非对象的引用计数重新为0
 * @note
 * - 静态初始化的对象所有资源都是由用户自己提供的，
 *   因此当对象销毁时，垃圾回收函数也需要用户自己提供。
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

__xwmp_api
xwer_t xwmp_mtx_init(struct xwmp_mtx * mtx, xwpr_t sprio)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        xwmp_mtx_construct(mtx);
        return xwmp_mtx_activate(mtx, sprio, NULL);
}

__xwmp_api
xwer_t xwmp_mtx_fini(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        return xwmp_mtx_put(mtx);
}

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

__xwmp_api
xwer_t xwmp_mtx_delete(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);

        return xwmp_mtx_put(mtx);
}

__xwmp_api
xwer_t xwmp_mtx_acquire(struct xwmp_mtx * mtx, xwsq_t tik)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_acquire(&mtx->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_mtx_release(struct xwmp_mtx * mtx, xwsq_t tik)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_release(&mtx->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_mtx_grab(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_grab(&mtx->xwobj);
}

__xwmp_api
xwer_t xwmp_mtx_put(struct xwmp_mtx * mtx)
{
        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        return xwos_object_put(&mtx->xwobj);
}

/**
 * @brief 修改一次互斥锁的动态优先级
 * @param[in] mtx: 互斥锁对象的指针
 * @param[out] pthd: 指向缓冲区的指针，通过此缓冲区返回下一个需要修改动态优先级的
 *                   线程控制块对象的指针
 * @param[out] pseq: 指向缓冲区的指针，通过此缓冲区返回线程控制块中互斥锁树中的
 *                   顺序锁的顺序值
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_mtx_chprio_once(struct xwmp_mtx * mtx,
                            struct xwmp_thd ** pthd,
                            xwsq_t * seq)
{
        struct xwmp_thd * owner;
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        *pthd = NULL;
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
                                        owner = xwcc_baseof(mt, struct xwmp_thd,
                                                            mtxtree);
                                        xwmp_sqlk_wr_lock(&mt->lock);
                                        *seq = xwmp_sqlk_get_seq(&mt->lock);
                                        xwmp_mtxtree_remove_locked(mtx, mt);
                                        xwmp_mtxtree_add_locked(mtx, mt);
                                        xwmp_sqlk_wr_unlock(&mt->lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        *seq += XWMP_SQLK_GRANULARITY;
                                        *pthd = owner;
                                } else {
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                }
                        }
                } else {
                        mtx->dprio = mtx->rtwq.max_prio;
                        mt = mtx->ownertree;
                        if (mt) {
                                owner = xwcc_baseof(mt, struct xwmp_thd, mtxtree);
                                xwmp_sqlk_wr_lock(&mt->lock);
                                *seq = xwmp_sqlk_get_seq(&mt->lock);
                                xwmp_mtxtree_remove_locked(mtx, mt);
                                xwmp_mtxtree_add_locked(mtx, mt);
                                xwmp_sqlk_wr_unlock(&mt->lock);
                                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                *seq += XWMP_SQLK_GRANULARITY;
                                *pthd = owner;
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
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 */
static __xwmp_code
void xwmp_mtx_chprio(struct xwmp_mtx * mtx)
{
        struct xwmp_thd * thd;
        struct xwmp_mtxtree * mt;
        xwsq_t pseq, cseq;
        xwpr_t dprio;
        xwer_t rc;

        while (mtx) {
                rc = xwmp_mtx_chprio_once(mtx, &thd, &pseq);
                if (__xwcc_unlikely(rc < 0)) {
                        break;
                }/* else {} */
                if (__xwcc_likely(NULL == thd)) {
                        break;
                }/* else {} */
                mt = &thd->mtxtree;
                cseq = xwmp_sqlk_rd_begin(&mt->lock);
                if (cseq == pseq) {
                        dprio = thd->sprio > mt->maxprio ?
                                thd->sprio : mt->maxprio;
                        mtx = NULL;
                        rc = xwmp_thd_chprio_once(thd, dprio, &mtx);
                        if (xwmp_sqlk_rd_retry(&mt->lock, cseq) || (rc < 0)) {
                                break;
                        }/* else {} */
                }/* else {} */
        }
}

/**
 * @brief 中断互斥锁等待队列中的一个线程
 * @param[in] mtx: 互斥锁对象的指针
 * @param[in] thd: 线程控制块对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_code
xwer_t xwmp_mtx_intr(struct xwmp_mtx * mtx, struct xwmp_thd * thd)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &thd->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        xwmp_mtx_put(mtx);
                        xwmp_thd_wakeup(thd);
                        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
                        xwmp_skd_chkpmpt(xwskd);
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_put(mtx);
                }
        }/* else {} */
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_unlock(struct xwmp_mtx * mtx)
{
        xwer_t rc;
        struct xwmp_skd * local, * xwskd;
        struct xwmp_wqn * wqn;
        struct xwmp_thd * t, * cthd;
        struct xwmp_mtx * tmp;
        struct xwmp_mtxtree * mt;
        xwpr_t dprio;
        xwsq_t seq;
        xwreg_t cpuirq;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = XWOK;
        local = xwmp_skd_dspmpt_lc();
        cthd = xwmp_skd_get_cthd(local);
        mt = &cthd->mtxtree;
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
                        xwaop_store(xwsq, &wqn->reason,
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
                        rc = xwmp_thd_wakeup(t);
                        if (__xwcc_unlikely(rc < 0)) {
                                /* 如果t已经在其他CPU开始运行，
                                   它的动态优先级需要被修改。*/
                                seq = xwmp_sqlk_rd_begin(&mt->lock);
                                dprio = t->sprio > mt->maxprio ?
                                        t->sprio : mt->maxprio;
                                tmp = NULL;
                                rc = xwmp_thd_chprio_once(t, dprio, &tmp);
                                if ((!xwmp_sqlk_rd_retry(&mt->lock, seq)) &&
                                    (XWOK == rc) && (tmp)) {
                                        xwmp_mtx_chprio(tmp);
                                }/* else {} */
                        }/* else {} */
                        xwmp_mtx_put(mtx);
                        /* change the priority of old owner */
                        xwmp_thd_chprio(cthd);
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
                        xwmp_thd_chprio(cthd);
                        xwmp_skd_enpmpt(local);
                        /* 如果函数在xwmpsyn_cond_timedwait()中被调用，
                           当前线程已经不是`XWMP_SKDOBJ_DST_RUNNING'状态，
                           xwmp_skd_chkpmpt()不起作用。*/
                        xwmp_skd_chkpmpt(local);
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_lock(struct xwmp_mtx * mtx)
{
        xwtm_t expected;

        expected = XWTM_MAX;
        return xwmp_mtx_timedlock(mtx, &expected);
}

__xwmp_api
xwer_t xwmp_mtx_trylock(struct xwmp_mtx * mtx)
{
        struct xwmp_skd * xwskd;
        struct xwmp_thd * cthd;
        struct xwmp_mtxtree * mt;
        xwer_t rc;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        rc = XWOK;
        xwskd = xwmp_skd_dspmpt_lc();
        cthd = xwmp_skd_get_cthd(xwskd);
        mt = &cthd->mtxtree;
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
                xwmp_thd_chprio(cthd);
                xwmp_skd_enpmpt(xwskd);
                xwmp_skd_chkpmpt(xwskd);
        }

err_mtx_grab:
        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_do_timedblkthd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                               struct xwmp_thd * thd,
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

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        currtick = xwmp_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);

        /* 加入等待队列 */
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_SLEEPING |
                     XWMP_SKDOBJ_DST_READY | XWMP_SKDOBJ_DST_STANDBY |
                     XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_MIGRATING)
                    & thd->state);
        if (XWMP_SKDOBJ_DST_FREEZABLE & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                rc = -EINTR;
                goto err_needfrz;
        }
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_rtwq_locked(thd, &mtx->rtwq, XWMP_WQTYPE_MTX, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);

        /* 改变优先级 */
        xwmp_mtx_chprio(mtx);

        /* 加入时间树 */
        xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwmp_splk_lock(&thd->stlock);
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_tt_add_locked(thd, xwtt, expected, cpuirq);
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
        reason = xwaop_load(xwsq, &thd->wqn.reason, xwmb_modr_relaxed);
        wkuprs = xwaop_load(xwsq, &thd->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWMP_WQN_REASON_INTR == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if (XWMP_WQN_REASON_UP == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }/* else {} */
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &thd->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        reason = xwaop_load(xwsq, &thd->wqn.reason,
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
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &thd->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_BLOCKING);
                        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        reason = xwaop_load(xwsq, &thd->wqn.reason,
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
                             struct xwmp_thd * thd,
                             xwtm_t * xwtm)
{
        struct xwmp_skd * xwskd;
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_skd_dspmpt(xwskd);
        mt = &thd->mtxtree;
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
                        rc = xwmp_mtx_do_timedblkthd_unlkwq_cpuirqrs(mtx, thd,
                                                                     xwtm, cpuirq);
                        xwmp_skd_wakelock_unlock(xwskd);
                }
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_thd_chprio(thd);
                xwmp_skd_enpmpt(xwskd);
                xwmp_skd_chkpmpt(xwskd);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_timedlock(struct xwmp_mtx * mtx, xwtm_t * xwtm)
{
        xwer_t rc;
        struct xwmp_thd * cthd;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

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
                        cthd = xwmp_skd_get_cthd_lc();
                        rc = xwmp_mtx_do_timedlock(mtx, cthd, xwtm);
                        if (__xwcc_unlikely(rc < 0)) {
                                xwmp_mtx_put(mtx);
                        } /* else {} */
                }/* else {} */
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_do_blkthd_unlkwq_cpuirqrs(struct xwmp_mtx * mtx,
                                          struct xwmp_thd * thd,
                                          xwreg_t cpuirq)
{
        struct xwmp_skd * xwskd;
        xwsq_t reason;
        xwpr_t dprio;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        /* 加入等待队列 */
        xwmp_splk_lock(&thd->stlock);
        dprio = thd->dprio.r;
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_UNINTERRUPTED);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_rtwq_locked(thd, &mtx->rtwq, XWMP_WQTYPE_MTX, dprio);
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
        reason = xwaop_load(xwsq, &thd->wqn.reason, xwmb_modr_relaxed);
        if (XWMP_WQN_REASON_UP == reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }

        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_do_lock_unintr(struct xwmp_mtx * mtx, struct xwmp_thd * thd)
{
        struct xwmp_skd * xwskd;
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_skd_dspmpt(xwskd);
        mt = &thd->mtxtree;
        xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(xwskd);
        } else if (mtx->ownertree) {
                rc = xwmp_mtx_do_blkthd_unlkwq_cpuirqrs(mtx, thd, cpuirq);
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_thd_chprio(thd);
                xwmp_skd_enpmpt(xwskd);
                xwmp_skd_chkpmpt(xwskd);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_lock_unintr(struct xwmp_mtx * mtx)
{
        xwer_t rc;
        struct xwmp_thd * cthd;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwmp_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        cthd = xwmp_skd_get_cthd_lc();
        rc = xwmp_mtx_do_lock_unintr(mtx, cthd);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_do_lock_unintr;
        }
        return XWOK;

err_mtx_do_lock_unintr:
        xwmp_mtx_put(mtx);
err_mtx_grab:
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_getlkst(struct xwmp_mtx * mtx, xwsq_t * lkst)
{
        xwer_t rc;
        volatile struct xwmp_mtxtree * ownertree;

        XWOS_VALIDATE((mtx), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);

        rc = xwmp_mtx_grab(mtx);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_mtx_grab;
        }
        xwmb_mp_load_acquire(struct xwmp_mtxtree *, ownertree, &mtx->ownertree);
        if (ownertree) {
                *lkst = XWOS_LKST_LOCKED;
        } else {
                *lkst = XWOS_LKST_UNLOCKED;
        }
        xwmp_mtx_put(mtx);
        return XWOK;

err_mtx_grab:
        return rc;
}
