/**
 * @file
 * @brief XWOS MP内核锁机制：互斥锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
#  include <xwos/mm/sma.h>
#elif defined(XWOSCFG_LOCK_MTX_STDC_MM) && (1 == XWOSCFG_LOCK_MTX_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/mtxtree.h>
#include <xwos/mp/thd.h>
#include <xwos/mp/lock/mtx.h>

/* #define XWOS_MTXLOGF */ /**< 调试日志开关 */
#ifdef XWOS_MTXLOGF
#  define xwos_mtxlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_mtxlogf(lv, thd, fmt, ...)
#endif


#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
/**
 * @brief 结构体 `xwmp_mtx` 的对象缓存
 */
static __xwmp_data struct xwmm_mempool_objcache xwmp_mtx_cache;

/**
 * @brief 结构体 `xwmp_mtx` 的对象缓存的名字
 */
const __xwmp_rodata char xwmp_mtx_cache_name[] = "xwmp.lk.mtx.cache";
#elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief 结构体 `xwmp_mtx` 的对象缓存
 */
static __xwmp_data struct xwmm_memslice xwmp_mtx_cache;

/**
 * @brief 结构体 `xwmp_mtx` 的对象缓存的名字
 */
const __xwmp_rodata char xwmp_mtx_cache_name[] = "xwmp.lk.mtx.cache";
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
static __xwmp_data struct xwmm_sma * xwmp_mtx_cache;
#endif

static __xwmp_code
void xwmp_mtx_construct(struct xwmp_mtx * mtx);

static __xwmp_code
void xwmp_mtx_destruct(struct xwmp_mtx * mtx);

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
static __xwmp_code
struct xwmp_mtx * xwmp_mtx_alloc(void);

static __xwmp_code
void xwmp_mtx_free(struct xwmp_mtx * mtx);
#endif

static __xwmp_code
xwer_t xwmp_mtx_activate(struct xwmp_mtx * mtx, xwpr_t sprio,
                         xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_mtx_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
static __xwmp_code
xwer_t xwmp_mtx_dgc(struct xwos_object * obj);
#endif

static __xwmp_code
xwer_t xwmp_mtx_chprio_once(struct xwmp_mtx * mtx,
                            struct xwmp_thd ** pthd,
                            xwsq_t * seq);

static __xwmp_code
void xwmp_mtx_chprio(struct xwmp_mtx * mtx);

static __xwmp_code
xwer_t xwmp_mtx_block_to(struct xwmp_mtx * mtx,
                         struct xwmp_skd * xwskd, struct xwmp_thd * thd,
                         xwtm_t to, xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_mtx_lock_or_block_to(struct xwmp_mtx * mtx,
                                 struct xwmp_skd * xwskd, struct xwmp_thd * thd,
                                 xwtm_t to);

static __xwmp_code
xwer_t xwmp_mtx_block_unintr(struct xwmp_mtx * mtx, struct xwmp_thd * thd,
                             xwreg_t cpuirq);

static __xwmp_code
xwer_t xwmp_mtx_lock_or_block_unintr(struct xwmp_mtx * mtx,
                                     struct xwmp_skd * xwskd, struct xwmp_thd * thd);

#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
/**
 * @brief XWMP INIT CODE：初始化 `struct xwmp_mtx` 的对象缓存
 * @param[in] mp: 内存池
 * @param[in] page_order: 每次预先申请页的数量的阶，几阶就是2的几次方
 * @return 错误码
 * @note
 * + 重入性：只可在系统初始化时使用一次
 * @details
 * 内存池中，每一页内存固定为4096字节，对象缓存每次会预先申请
 * 大小为 `pow(2, page_order) * 4096` 字节的内存页，并建立对象缓存。
 * 当对象使用完后，才会再次申请大小为 `pow(2, page_order) * 4096` 字节的内存页，
 * 并扩展对象缓存。
 */
__xwmp_init_code
xwer_t xwmp_mtx_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwmp_mtx_cache,
                                        &mp->pa,
                                        xwmp_mtx_cache_name,
                                        sizeof(struct xwmp_mtx),
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwmp_mtx_construct,
                                        (dtor_f)xwmp_mtx_destruct);
        return rc;
}
#elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化结构体 `xwmp_mtx` 的对象缓存
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
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
/**
 * @brief XWMP INIT CODE：初始化 `struct xwmp_mtx` 的对象缓存
 * @param[in] sma: 简单内存分配器
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
void xwmp_mtx_cache_init(struct xwmm_sma * sma)
{
        xwmp_mtx_cache = sma;
}
#endif

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
/**
 * @brief 从互斥锁对象缓存中申请对象
 * @return 互斥锁对象的指针
 */
static __xwmp_code
struct xwmp_mtx * xwmp_mtx_alloc(void)
{
#  if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
        union {
                struct xwmp_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_objcache_alloc(&xwmp_mtx_cache, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        }
        return mem.mtx;
#  elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
        union {
                struct xwmp_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwmp_mtx_cache, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        }
        return mem.mtx;
#  elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
        union {
                struct xwmp_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwmp_mtx_cache,
                            sizeof(struct xwmp_mtx), XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        } else {
                xwmp_mtx_construct(mem.mtx);
        }
        return mem.mtx;
#  elif defined(XWOSCFG_LOCK_MTX_STDC_MM) && (1 == XWOSCFG_LOCK_MTX_STDC_MM)
        struct xwmp_mtx * mtx;

        mtx = malloc(sizeof(struct xwmp_mtx));
        if (NULL == mtx) {
                mtx = err_ptr(-ENOMEM);
        } else {
                xwmp_mtx_construct(mtx);
        }
        return mtx;
#  else
        return err_ptr(-ENOSYS);;
#  endif
}
#endif

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
/**
 * @brief 释放互斥锁对象
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwmp_code
void xwmp_mtx_free(struct xwmp_mtx * mtx)
{
#  if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
        xwmm_mempool_objcache_free(&xwmp_mtx_cache, mtx);
#  elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
        xwmm_memslice_free(&xwmp_mtx_cache, mtx);
#  elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
        xwmm_sma_free(xwmp_mtx_cache, mtx);
#  elif defined(XWOSCFG_LOCK_MTX_STDC_MM) && (1 == XWOSCFG_LOCK_MTX_STDC_MM)
        xwmp_mtx_destruct(mtx);
        free(mtx);
#  else
        XWOS_UNUSED(mtx);
#  endif
}
#endif

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
 * @brief 静态互斥锁对象的垃圾回收函数
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwmp_code
xwer_t xwmp_mtx_sgc(struct xwos_object * obj)
{
        struct xwmp_mtx * mtx;

        mtx = xwcc_derof(obj, struct xwmp_mtx, xwobj);
        xwmp_mtx_destruct(mtx);
        return XWOK;
}

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
/**
 * @brief 动态互斥锁对象的垃圾回收函数
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwmp_code
xwer_t xwmp_mtx_dgc(struct xwos_object * obj)
{
        struct xwmp_mtx * mtx;

        mtx = xwcc_derof(obj, struct xwmp_mtx, xwobj);
        xwmp_mtx_free(mtx);
        return XWOK;
}
#endif

/**
 * @brief 激活互斥锁对象，并初始化
 * @param[in] mtx: 互斥锁对象的指针
 * @param[in] sprio: 互斥锁的静态优先级
 * @param[in] gcfunc: 垃圾回收函数的指针
 */
static __xwmp_code
xwer_t xwmp_mtx_activate(struct xwmp_mtx * mtx, xwpr_t sprio, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&mtx->xwobj, gcfunc);
        if (rc < 0) {
                goto err_xwobj_activate;
        }
        if (sprio >= XWMP_SKD_PRIORITY_RT_NUM) {
                // cppcheck-suppress [misra-c2012-17.8]
                sprio = XWMP_SKD_PRIORITY_RT_MAX;
        } else if (sprio <= XWMP_SKD_PRIORITY_INVALID) {
                // cppcheck-suppress [misra-c2012-17.8]
                sprio = XWMP_SKD_PRIORITY_RT_MIN;
        } else {}
        mtx->sprio = sprio;
        mtx->dprio = sprio;
        xwmp_rtwq_init(&mtx->rtwq);
        mtx->ownertree = NULL;
        mtx->reentrant = (xwsq_t)0;
        xwlib_rbtree_init_node(&mtx->rbnode);
        xwlib_bclst_init_node(&mtx->rbbuddy);
        return XWOK;

err_xwobj_activate:
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_init(struct xwmp_mtx * mtx, xwpr_t sprio)
{
        xwmp_mtx_construct(mtx);
        return xwmp_mtx_activate(mtx, sprio, xwmp_mtx_sgc);
}

__xwmp_api
xwer_t xwmp_mtx_fini(struct xwmp_mtx * mtx)
{
        return xwmp_mtx_put(mtx);
}

__xwmp_api
xwsq_t xwmp_mtx_get_tik(struct xwmp_mtx * mtx)
{
        return (NULL != mtx) ? mtx->xwobj.tik : (xwsq_t)0;
}

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
__xwmp_api
xwer_t xwmp_mtx_create(struct xwmp_mtx ** mtxbuf, xwpr_t sprio)
{
        struct xwmp_mtx * mtx;
        xwer_t rc;

        *mtxbuf = NULL;
        mtx = xwmp_mtx_alloc();
        if (is_err(mtx)) {
                rc = ptr_err(mtx);
        } else {
                rc = xwmp_mtx_activate(mtx, sprio, xwmp_mtx_dgc);
                if (rc < 0) {
                        xwmp_mtx_free(mtx);
                } else {
                        *mtxbuf = mtx;
                }
        }
        return rc;
}
#endif

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
__xwmp_api
xwer_t xwmp_mtx_delete(struct xwmp_mtx * mtx, xwsq_t tik)
{
        return xwmp_mtx_release(mtx, tik);
}
#endif

__xwmp_api
xwer_t xwmp_mtx_acquire(struct xwmp_mtx * mtx, xwsq_t tik)
{
        return xwos_object_acquire(&mtx->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_mtx_release(struct xwmp_mtx * mtx, xwsq_t tik)
{
        return xwos_object_release(&mtx->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_mtx_grab(struct xwmp_mtx * mtx)
{
        return xwos_object_grab(&mtx->xwobj);
}

__xwmp_api
xwer_t xwmp_mtx_put(struct xwmp_mtx * mtx)
{
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
        if (XWOK == rc) {
                xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
                if (mtx->dprio == mtx->rtwq.max_prio) {
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                } else if (mtx->rtwq.max_prio <= mtx->sprio) {
                        if (mtx->dprio == mtx->sprio) {
                                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        } else {
                                mtx->dprio = mtx->sprio;
                                mt = mtx->ownertree;
                                if (NULL != mt) {
                                        owner = xwcc_derof(mt, struct xwmp_thd, mt);
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
                        if (NULL != mt) {
                                owner = xwcc_derof(mt, struct xwmp_thd, mt);
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
                xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
        }
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
        xwsq_t pseq;
        xwsq_t cseq;
        xwpr_t dprio;
        xwer_t rc;

        while (NULL != mtx) { // cppcheck-suppress [misra-c2012-15.4]
                rc = xwmp_mtx_chprio_once(mtx, &thd, &pseq);
                if (rc < 0) {
                        break;
                }
                if (NULL == thd) {
                        break;
                }
                mt = &thd->mt;
                cseq = xwmp_sqlk_rd_begin(&mt->lock);
                if (cseq == pseq) {
                        dprio = (thd->sprio > mt->maxprio) ?
                                thd->sprio : mt->maxprio;
                        mtx = NULL; // cppcheck-suppress [misra-c2012-17.8]
                        rc = xwmp_thd_chprio_once(thd, dprio, &mtx);
                        if (xwmp_sqlk_rd_retry(&mt->lock, cseq) || (rc < 0)) {
                                break;
                        }
                }
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
        if (XWOK == rc) {
                xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &thd->wqn);
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
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_thd_wakeup(thd); // cppcheck-suppress [misra-c2012-17.7]
                        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
                        xwmp_skd_chkpmpt(xwskd);
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
                }
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_unlock(struct xwmp_mtx * mtx)
{
        xwer_t rc;
        struct xwmp_skd * local;
        struct xwmp_skd * xwskd;
        struct xwmp_wqn * wqn;
        struct xwmp_thd * t;
        struct xwmp_thd * cthd;
        struct xwmp_mtx * tmp;
        struct xwmp_mtxtree * mt;
        xwpr_t dprio;
        xwsq_t seq;
        xwreg_t cpuirq;

        rc = XWOK;
        local = xwmp_skd_dspmpt_lc();
        cthd = xwmp_skd_get_cthd(local);
        mt = &cthd->mt;
        xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree != mt) {
                rc = -EOWNER;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(local); // cppcheck-suppress [misra-c2012-17.7]
        } else if (mtx->reentrant > (xwsq_t)0) {
                mtx->reentrant--;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_skd_enpmpt(local); // cppcheck-suppress [misra-c2012-17.7]
        } else {
                xwmp_mtxtree_remove(mtx, mt);
                wqn = xwmp_rtwq_choose_locked(&mtx->rtwq);
                if (NULL != wqn) {
                        /* Case 1: 等待队列中有线程正在等待互斥锁 */
                        wqn->wq = NULL;
                        wqn->type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &wqn->reason,
                                    xwaop_mo_release, XWMP_WQN_REASON_UP);
                        wqn->cb = NULL;
                        xwmp_splk_unlock(&wqn->lock);
                        t = xwcc_derof(wqn, struct xwmp_thd, wqn);
                        xwmp_splk_lock(&t->stlock);
                        xwbop_c0m(xwsq_t, &t->state,
                                  XWMP_SKDOBJ_ST_BLOCKING |
                                  XWMP_SKDOBJ_ST_UNINTERRUPTED);
                        t->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
                        xwmp_splk_unlock(&t->stlock);
                        mt = &t->mt;
                        /* 修改互斥锁的优先级, 互斥锁此时没有拥有者 */
                        if (mtx->rtwq.max_prio <= mtx->sprio) {
                                mtx->dprio = mtx->sprio;
                        } else {
                                mtx->dprio = mtx->rtwq.max_prio;
                        }
                        xwmp_mtxtree_add(mtx, mt);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        rc = xwmp_thd_wakeup(t);
                        if (rc < 0) {
                                /* 如果 `t` 运行在其他CPU上，可能进入当前case。*/
                                seq = xwmp_sqlk_rd_begin(&mt->lock);
                                dprio = (t->sprio > mt->maxprio) ?
                                        t->sprio : mt->maxprio;
                                tmp = NULL;
                                rc = xwmp_thd_chprio_once(t, dprio, &tmp);
                                if ((!xwmp_sqlk_rd_retry(&mt->lock, seq)) &&
                                    (XWOK == rc) &&
                                    (NULL != tmp)) {
                                        xwmp_mtx_chprio(tmp);
                                }
                        }
                        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_thd_chprio(cthd);
                        xwmp_skd_enpmpt(local); // cppcheck-suppress [misra-c2012-17.7]
                        /* 如果函数在 `xwmp_cond_wait_to()` 中被调用，
                           当前线程已经不是 `XWMP_SKDOBJ_ST_RUNNING` 状态，
                           `xwmp_skd_chkpmpt()` 不起作用。*/
                        xwmp_skd_chkpmpt(local);

                        /* `t` 可能运行在不同CPU上，因此也需要检查此CPU的抢占 */
                        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &t->xwskd);
                        xwmp_skd_chkpmpt(xwskd);
                } else {
                        /* Case 2: 没有线程正在等待互斥锁 */
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_thd_chprio(cthd);
                        xwmp_skd_enpmpt(local); // cppcheck-suppress [misra-c2012-17.7]
                        /* 如果函数在 `xwmp_cond_wait_to()` 中被调用，
                           当前线程已经不是 `XWMP_SKDOBJ_ST_RUNNING` 状态，
                           `xwmp_skd_chkpmpt()` 不起作用。*/
                        xwmp_skd_chkpmpt(local);
                }
        }
        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_block_to(struct xwmp_mtx * mtx,
                         struct xwmp_skd * xwskd, struct xwmp_thd * thd,
                         xwtm_t to, xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwmp_tt * xwtt;
        xwpr_t dprio;
        xwsq_t reason;
        xwsq_t wkuprs;

        xwtt = &xwskd->tt;
        xwmp_splk_lock(&thd->stlock);
        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_BLOCKING |
                                   (xwsq_t)XWMP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWMP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWMP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWMP_SKDOBJ_ST_FROZEN |
                                   (xwsq_t)XWMP_SKDOBJ_ST_MIGRATING) & thd->state));
        /* 检查是否被中断 */
        if ((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE |
                           (xwsq_t)XWMP_SKDOBJ_ST_EXITING) & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
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
        xwmp_thd_eq_rtwq_locked(thd, &mtx->rtwq, XWMP_WQTYPE_MTX, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);

        /* 改变优先级 */
        xwmp_mtx_chprio(mtx);

        /* 加入时间树 */
        if (to > 0) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                xwmp_splk_lock(&thd->stlock);
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                xwmp_splk_unlock(&thd->stlock);
                // cppcheck-suppress [misra-c2012-17.7]
                xwmp_thd_tt_add_locked(thd, xwtt, to, cpuirq);
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        }

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_wakelock_unlock(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_skd_req_swcx(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_skd_wakelock_lock(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        wkuprs = xwaop_load(xwsq_t, &thd->ttn.wkuprs, xwaop_mo_relaxed);
        if ((xwsq_t)XWMP_WQN_REASON_INTR == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if ((xwsq_t)XWMP_WQN_REASON_UP == reason) {
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                }
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if ((xwsq_t)XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &thd->wqn);
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
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        rc = -ETIMEDOUT;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
                        if ((xwsq_t)XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if ((xwsq_t)XWMP_WQN_REASON_UP == reason) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if ((xwsq_t)XWMP_TTN_WKUPRS_INTR == wkuprs) {
                xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
                xwmp_splk_lock(&thd->wqn.lock);
                rc = xwmp_rtwq_remove_locked(&mtx->rtwq, &thd->wqn);
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
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_mtx_chprio(mtx);
                        rc = -EINTR;
                } else {
                        xwmp_splk_unlock(&thd->wqn.lock);
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
                        if ((xwsq_t)XWMP_WQN_REASON_INTR == reason) {
                                rc = -EINTR;
                        } else if ((xwsq_t)XWMP_WQN_REASON_UP == reason) {
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
xwer_t xwmp_mtx_lock_or_block_to(struct xwmp_mtx * mtx,
                                 struct xwmp_skd * xwskd, struct xwmp_thd * thd,
                                 xwtm_t to)
{
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwmp_skd_dspmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        mt = &thd->mt;
        xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        } else if (NULL != mtx->ownertree) {
                rc = xwmp_skd_wakelock_lock(xwskd);
                if (rc < 0) {
                        /* 当前调度器正准备休眠，线程需被冻结，返回-EINTR。*/
                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                        xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                        rc = -EINTR;
                } else {
                        rc = xwmp_mtx_block_to(mtx, xwskd, thd, to, cpuirq);
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_skd_wakelock_unlock(xwskd);
                }
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_thd_chprio(thd);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_skd_chkpmpt(xwskd);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_lock(struct xwmp_mtx * mtx)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        xwer_t rc;

        if (!xwospl_cpuirq_test_lc()) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        if (!xwmp_skd_tstth(xwskd)) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        if (!xwmp_skd_tstpmpt(xwskd)) {
                rc = -EDISPMPT;
                goto err_dis;
        }
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        if (!xwmp_skd_tstbh(xwskd)) {
                rc = -EDISBH;
                goto err_dis;
        }
#endif
        rc = xwmp_mtx_grab(mtx);
        if (rc < 0) {
                goto err_grab;
        }
        rc = xwmp_mtx_lock_or_block_to(mtx, xwskd, cthd, 0);
        if (rc < 0) {
                xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
        }

err_grab:
err_dis:
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_lock_to(struct xwmp_mtx * mtx, xwtm_t to)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        struct xwmp_syshwt * hwt;
        xwtm_t now;
        xwer_t rc;

        if (!xwospl_cpuirq_test_lc()) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        if (!xwmp_skd_tstth(xwskd)) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        if (!xwmp_skd_tstpmpt(xwskd)) {
                rc = -EDISPMPT;
                goto err_dis;
        }
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        if (!xwmp_skd_tstbh(xwskd)) {
                rc = -EDISBH;
                goto err_dis;
        }
#endif
        hwt = &xwskd->tt.hwt;
        now = xwmp_syshwt_get_time(hwt);
        if (xwtm_cmp(to, now) < 0) {
                rc = -ETIMEDOUT;
        } else if (xwtm_cmp(to, now) == 0) {
                rc = xwmp_mtx_trylock(mtx);
                if (rc < 0) {
                        if (-ENODATA == rc) {
                                rc = -ETIMEDOUT;
                        }
                }
        } else {
                rc = xwmp_mtx_grab(mtx);
                if (rc < 0) {
                        goto err_grab;
                }
                rc = xwmp_mtx_lock_or_block_to(mtx, xwskd, cthd, to);
                if (rc < 0) {
                        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
                }
        }

err_grab:
err_dis:
        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_block_unintr(struct xwmp_mtx * mtx, struct xwmp_thd * thd,
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
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_ST_RUNNING);
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = dprio;
        xwbop_s1m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_ST_BLOCKING | XWMP_SKDOBJ_ST_UNINTERRUPTED);
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_eq_rtwq_locked(thd, &mtx->rtwq, XWMP_WQTYPE_MTX, dprio);
        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);

        /* 改变优先级 */
        xwmp_mtx_chprio(mtx);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_skd_req_swcx(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        reason = xwaop_load(xwsq_t, &thd->wqn.reason, xwaop_mo_relaxed);
        if ((xwsq_t)XWMP_WQN_REASON_UP == reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }

        return rc;
}

static __xwmp_code
xwer_t xwmp_mtx_lock_or_block_unintr(struct xwmp_mtx * mtx,
                                     struct xwmp_skd * xwskd, struct xwmp_thd * thd)
{
        struct xwmp_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwmp_skd_dspmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        mt = &thd->mt;
        xwmp_rtwq_lock_cpuirqsv(&mtx->rtwq, &cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        } else if (NULL != mtx->ownertree) {
                rc = xwmp_mtx_block_unintr(mtx, thd, cpuirq);
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                xwmp_thd_chprio(thd);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_skd_chkpmpt(xwskd);
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_lock_unintr(struct xwmp_mtx * mtx)
{
        xwer_t rc;
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;

        if (!xwospl_cpuirq_test_lc()) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        if (!xwmp_skd_tstth(xwskd)) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        if (!xwmp_skd_tstpmpt(xwskd)) {
                rc = -EDISPMPT;
                goto err_dis;
        }
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        if (!xwmp_skd_tstbh(xwskd)) {
                rc = -EDISBH;
                goto err_dis;
        }
#endif
        rc = xwmp_mtx_grab(mtx);
        if (rc < 0) {
                goto err_mtx_grab;
        }
        rc = xwmp_mtx_lock_or_block_unintr(mtx, xwskd, cthd);
        if (rc < 0) {
                goto err_mtx_do_lock_unintr;
        }
        return XWOK;

err_mtx_do_lock_unintr:
        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
err_mtx_grab:
err_dis:
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_trylock(struct xwmp_mtx * mtx)
{
        struct xwmp_skd * xwskd;
        struct xwmp_thd * cthd;
        struct xwmp_mtxtree * mt;
        xwer_t rc;

        rc = xwmp_mtx_grab(mtx);
        if (rc < 0) {
                goto err_mtx_grab;
        }
        rc = XWOK;
        xwskd = xwmp_skd_dspmpt_lc();
        cthd = xwmp_skd_get_cthd(xwskd);
        mt = &cthd->mt;
        xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwmp_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        } else if (NULL != mtx->ownertree) {
                xwmp_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
                rc = -EWOULDBLOCK;
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        } else {
                xwmp_mtxtree_add(mtx, mt);
                xwmp_rtwq_unlock_cpuirq(&mtx->rtwq);
                xwmp_thd_chprio(cthd);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_skd_chkpmpt(xwskd);
        }

err_mtx_grab:
        return rc;
}

__xwmp_api
xwer_t xwmp_mtx_get_status(struct xwmp_mtx * mtx, xwsq_t * status)
{
        xwer_t rc;
        volatile struct xwmp_mtxtree * ownertree;

        rc = xwmp_mtx_grab(mtx);
        if (rc < 0) {
                goto err_mtx_grab;
        }
        xwmb_mp_load_acquire(struct xwmp_mtxtree *, ownertree, &mtx->ownertree);
        if (NULL != ownertree) {
                *status = (xwsq_t)XWOS_LKST_LOCKED;
        } else {
                *status = (xwsq_t)XWOS_LKST_UNLOCKED;
        }
        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
        return XWOK;

err_mtx_grab:
        return rc;
}
