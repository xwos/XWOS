/**
 * @file
 * @brief XWOS UP内核锁机制：互斥锁
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
#include <xwos/up/skd.h>
#include <xwos/up/tt.h>
#include <xwos/up/rtwq.h>
#include <xwos/up/mtxtree.h>
#include <xwos/up/thd.h>
#include <xwos/up/lock/mtx.h>

/* #define XWOS_MTXLOGF */ /**< 调试日志开关 */
#ifdef XWOS_MTXLOGF
#  define xwos_mtxlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_mtxlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
/**
 * @brief 结构体 `xwup_mtx` 的对象缓存
 */
static __xwup_data struct xwmm_mempool_objcache xwup_mtx_cache;

/**
 * @brief 结构体 `xwup_mtx` 的对象缓存的名字
 */
const __xwup_rodata char xwup_mtx_cache_name[] = "xwup.lk.mtx.cache";
#elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief 结构体 `xwup_mtx` 的对象缓存
 */
static __xwup_data struct xwmm_memslice xwup_mtx_cache;

/**
 * @brief 结构体 `xwup_mtx` 的对象缓存的名字
 */
const __xwup_rodata char xwup_mtx_cache_name[] = "xwup.lk.mtx.cache";
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
static __xwup_data struct xwmm_sma * xwup_mtx_cache;
#endif

static __xwup_code
void xwup_mtx_construct(struct xwup_mtx * mtx);

static __xwup_code
void xwup_mtx_destruct(struct xwup_mtx * mtx);

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
static __xwup_code
struct xwup_mtx * xwup_mtx_alloc(void);

static __xwup_code
void xwup_mtx_free(struct xwup_mtx * mtx);
#endif

static __xwup_code
xwer_t xwup_mtx_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
static __xwup_code
xwer_t xwup_mtx_dgc(struct xwos_object * obj);
#endif

static __xwup_code
xwer_t xwup_mtx_activate(struct xwup_mtx * mtx, xwpr_t sprio, xwobj_gc_f gcfunc);

static __xwup_code
void xwup_mtx_chprio_once(struct xwup_mtx * mtx, struct xwup_thd ** pthd);

static __xwup_code
void xwup_mtx_chprio(struct xwup_mtx * mtx);

static __xwup_code
xwer_t xwup_mtx_block_to(struct xwup_mtx * mtx,
                         struct xwup_skd * xwskd, struct xwup_thd * thd,
                         xwtm_t to, xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_mtx_lock_or_block_to(struct xwup_mtx * mtx, struct xwup_thd * thd,
                                 xwtm_t to);

static __xwup_code
xwer_t xwup_mtx_block_unintr(struct xwup_mtx * mtx, struct xwup_thd * thd,
                             xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_mtx_lock_or_block_unintr(struct xwup_mtx * mtx, struct xwup_thd * thd);

#if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_mtx` 的对象缓存
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
__xwup_init_code
xwer_t xwup_mtx_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwup_mtx_cache,
                                        &mp->pa,
                                        xwup_mtx_cache_name,
                                        sizeof(struct xwup_mtx),
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwup_mtx_construct,
                                        (dtor_f)xwup_mtx_destruct);
        return rc;
}
#elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
/**
 * @brief XWUP INIT CODE：初始化结构体 `xwup_mtx` 的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 只可在系统初始化时使用一次
 */
__xwup_init_code
xwer_t xwup_mtx_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwup_mtx_cache, zone_origin, zone_size,
                                sizeof(struct xwup_mtx),
                                xwup_mtx_cache_name,
                                (ctor_f)xwup_mtx_construct,
                                (dtor_f)xwup_mtx_destruct);
        return rc;
}
#elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_mtx` 的对象缓存
 * @param[in] sma: 简单内存分配器
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwup_init_code
void xwup_mtx_cache_init(struct xwmm_sma * sma)
{
        xwup_mtx_cache = sma;
}
#endif

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
/**
 * @brief 申请互斥锁对象
 * @return 互斥锁对象的指针
 */
static __xwup_code
struct xwup_mtx * xwup_mtx_alloc(void)
{
#  if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
        union {
                struct xwup_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_objcache_alloc(&xwup_mtx_cache, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        }
        return mem.mtx;
#  elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
        union {
                struct xwup_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwup_mtx_cache, &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        }
        return mem.mtx;
#  elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
        union {
                struct xwup_mtx * mtx;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwup_mtx_cache,
                            sizeof(struct xwup_mtx), XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.mtx = err_ptr(rc);
        } else {
                xwup_mtx_construct(mem.mtx);
        }
        return mem.mtx;
#  elif defined(XWOSCFG_LOCK_MTX_STDC_MM) && (1 == XWOSCFG_LOCK_MTX_STDC_MM)
        struct xwup_mtx * mtx;

        mtx = malloc(sizeof(struct xwup_mtx));
        if (NULL == mtx) {
                mtx = err_ptr(-ENOMEM);
        } else {
                xwup_mtx_construct(mtx);
        }
        return mtx;
#  else
        return err_ptr(-ENOSYS);
#  endif
}
#endif

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
/**
 * @brief 释放互斥锁对象
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwup_code
void xwup_mtx_free(struct xwup_mtx * mtx)
{
#  if defined(XWOSCFG_LOCK_MTX_MEMPOOL) && (1 == XWOSCFG_LOCK_MTX_MEMPOOL)
        xwmm_mempool_objcache_free(&xwup_mtx_cache, mtx);
#  elif defined(XWOSCFG_LOCK_MTX_MEMSLICE) && (1 == XWOSCFG_LOCK_MTX_MEMSLICE)
        xwmm_memslice_free(&xwup_mtx_cache, mtx);
#  elif defined(XWOSCFG_LOCK_MTX_SMA) && (1 == XWOSCFG_LOCK_MTX_SMA)
        xwmm_sma_free(xwup_mtx_cache, mtx);
#  elif defined(XWOSCFG_LOCK_MTX_STDC_MM) && (1 == XWOSCFG_LOCK_MTX_STDC_MM)
        xwup_mtx_destruct(mtx);
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
static __xwup_code
void xwup_mtx_construct(struct xwup_mtx * mtx)
{
        xwos_object_construct(&mtx->xwobj);
}

/**
 * @brief 互斥锁对象的析构函数
 * @param[in] mtx: 互斥锁对象的指针
 */
static __xwup_code
void xwup_mtx_destruct(struct xwup_mtx * mtx)
{
        xwos_object_destruct(&mtx->xwobj);
}

static __xwup_code
xwer_t xwup_mtx_sgc(struct xwos_object * obj)
{
        struct xwup_mtx * mtx;

        mtx = xwcc_derof(obj, struct xwup_mtx, xwobj);
        xwup_mtx_destruct(mtx);
        return XWOK;
}

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
static __xwup_code
xwer_t xwup_mtx_dgc(struct xwos_object * obj)
{
        struct xwup_mtx * mtx;

        mtx = xwcc_derof(obj, struct xwup_mtx, xwobj);
        xwup_mtx_free(mtx);
        return XWOK;
}
#endif

/**
 * @brief 激活互斥锁对象
 * @param[in] mtx: 互斥锁对象的指针
 * @param[in] sprio: 互斥锁的静态优先级
 * @param[in] gcfunc: 垃圾回收函数的指针
 */
static __xwup_code
xwer_t xwup_mtx_activate(struct xwup_mtx * mtx, xwpr_t sprio, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&mtx->xwobj, gcfunc);
        if (rc < 0) {
                goto err_xwobj_activate;
        }
        if (sprio >= XWUP_SKD_PRIORITY_RT_NUM) {
                // cppcheck-suppress [misra-c2012-17.8]
                sprio = XWUP_SKD_PRIORITY_RT_MAX;
        } else if (sprio <= XWUP_SKD_PRIORITY_INVALID) {
                // cppcheck-suppress [misra-c2012-17.8]
                sprio = XWUP_SKD_PRIORITY_RT_MIN;
        } else {}
        mtx->sprio = sprio;
        mtx->dprio = sprio;
        xwup_rtwq_init(&mtx->rtwq);
        mtx->ownertree = NULL;
        mtx->reentrant = (xwsq_t)0;
        xwlib_rbtree_init_node(&mtx->rbnode);
        xwlib_bclst_init_node(&mtx->rbbuddy);
        return XWOK;

err_xwobj_activate:
        return rc;
}

__xwup_api
xwer_t xwup_mtx_init(struct xwup_mtx * mtx, xwpr_t sprio)
{
        xwup_mtx_construct(mtx);
        return xwup_mtx_activate(mtx, sprio, xwup_mtx_sgc);
}

__xwup_api
xwer_t xwup_mtx_fini(struct xwup_mtx * mtx)
{
        return xwup_mtx_put(mtx);
}

__xwup_api
xwsq_t xwup_mtx_get_tik(struct xwup_mtx * mtx)
{
        return (NULL != mtx) ? mtx->xwobj.tik : (xwsq_t)0;
}

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
__xwup_api
xwer_t xwup_mtx_create(struct xwup_mtx ** ptrbuf, xwpr_t sprio)
{
        struct xwup_mtx * mtx;
        xwer_t rc;

        *ptrbuf = NULL;
        mtx = xwup_mtx_alloc();
        if (is_err(mtx)) {
                rc = ptr_err(mtx);
        } else {
                rc = xwup_mtx_activate(mtx, sprio, xwup_mtx_dgc);
                if (rc < 0) {
                        xwup_mtx_free(mtx);
                } else {
                        *ptrbuf = mtx;
                }
        }
        return rc;
}
#endif

#if (1 == XWOSRULE_LOCK_MTX_CREATE_DELETE)
__xwup_api
xwer_t xwup_mtx_delete(struct xwup_mtx * mtx, xwsq_t tik)
{
        return xwup_mtx_release(mtx, tik);
}
#endif

__xwup_api
xwer_t xwup_mtx_acquire(struct xwup_mtx * mtx, xwsq_t tik)
{
        return xwos_object_acquire(&mtx->xwobj, tik);
}

__xwup_api
xwer_t xwup_mtx_release(struct xwup_mtx * mtx, xwsq_t tik)
{
        return xwos_object_release(&mtx->xwobj, tik);
}

__xwup_api
xwer_t xwup_mtx_grab(struct xwup_mtx * mtx)
{
        return xwos_object_grab(&mtx->xwobj);
}

__xwup_api
xwer_t xwup_mtx_put(struct xwup_mtx * mtx)
{
        return xwos_object_put(&mtx->xwobj);
}

/**
 * @brief 修改一次互斥锁的动态优先级
 * @param[in] mtx: 互斥锁对象的指针
 * @param[out] pthd: 指向缓冲区的指针，通过此缓冲区返回下一个需要修改
 *                   动态优先级的线程控制块对象的指针
 * @return 错误码
 */
static __xwup_code
void xwup_mtx_chprio_once(struct xwup_mtx * mtx, struct xwup_thd ** pthd)
{
        struct xwup_thd * owner;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;

        *pthd = NULL;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->dprio == mtx->rtwq.max_prio) {
                xwospl_cpuirq_restore_lc(cpuirq);
        } else if (mtx->rtwq.max_prio <= mtx->sprio) {
                if (mtx->dprio == mtx->sprio) {
                        xwospl_cpuirq_restore_lc(cpuirq);
                } else {
                        mtx->dprio = mtx->sprio;
                        mt = mtx->ownertree;
                        if (NULL != mt) {
                                owner = xwcc_derof(mt, struct xwup_thd, mt);
                                xwup_mtxtree_remove(mt, mtx);
                                xwup_mtxtree_add(mt, mtx);
                                xwospl_cpuirq_restore_lc(cpuirq);
                                *pthd = owner;
                        } else {
                                xwospl_cpuirq_restore_lc(cpuirq);
                        }
                }
        } else {
                mtx->dprio = mtx->rtwq.max_prio;
                mt = mtx->ownertree;
                if (NULL != mt) {
                        owner = xwcc_derof(mt, struct xwup_thd, mt);
                        xwup_mtxtree_remove(mt, mtx);
                        xwup_mtxtree_add(mt, mtx);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        *pthd = owner;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        }
}

/**
 * @brief 从互斥锁开始，修改互斥锁-线程链的动态优先级
 * @param[in] mtx: 互斥锁对象的指针
 * @return 错误码
 */
static __xwup_code
void xwup_mtx_chprio(struct xwup_mtx * mtx)
{
        struct xwup_thd * thd;
        struct xwup_mtxtree * mt;
        xwpr_t dprio;
        xwreg_t cpuirq;

        while (NULL != mtx) {
                xwup_mtx_chprio_once(mtx, &thd);
                if (NULL == thd) {
                        break;
                }
                mtx = NULL; // cppcheck-suppress [misra-c2012-17.8]
                mt = &thd->mt;
                xwospl_cpuirq_save_lc(&cpuirq);
                dprio = (thd->prio.s > mt->maxprio) ? thd->prio.s : mt->maxprio;
                xwup_thd_chprio_once(thd, dprio, &mtx);
                xwospl_cpuirq_restore_lc(cpuirq);
        }
}

/**
 * @brief 中断互斥锁等待队列中的一个节点
 * @param[in] mtx: 互斥锁对象的指针
 * @param[in] thd: 线程控制块对象的指针
 */
__xwup_code
xwer_t xwup_mtx_intr(struct xwup_mtx * mtx, struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
        if (XWOK == rc) {
                thd->wqn.wq = NULL;
                thd->wqn.type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                thd->wqn.cb = NULL;
                xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
                xwup_thd_wakeup(thd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_mtx_chprio(mtx);
                xwup_skd_chkpmpt();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

__xwup_api
xwer_t xwup_mtx_unlock(struct xwup_mtx * mtx)
{
        xwer_t rc;
        struct xwup_wqn * wqn;
        struct xwup_thd * t;
        struct xwup_thd * cthd;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;

        rc = XWOK;
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        cthd = xwup_skd_get_cthd_lc();
        mt = &cthd->mt;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree != mt) {
                rc = -EOWNER;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        } else if (mtx->reentrant > (xwsq_t)0) {
                mtx->reentrant--;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        } else {
                xwup_mtxtree_remove(mt, mtx);
                wqn = xwup_rtwq_choose(&mtx->rtwq);
                if (NULL != wqn) {
                        /* Case 1: 等待队列中有线程正在等待互斥锁 */
                        wqn->wq = NULL;
                        wqn->type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = (xwu16_t)XWUP_WQN_REASON_UP;
                        wqn->cb = NULL;
                        t = xwcc_derof(wqn, struct xwup_thd, wqn);
                        xwbop_c0m(xwsq_t, &t->state,
                                  ((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_UNINTERRUPTED));
                        mt = &t->mt;
                        /* 修改互斥锁的优先级, 互斥锁此时没有拥有者 */
                        if (mtx->rtwq.max_prio <= mtx->sprio) {
                                mtx->dprio = mtx->sprio;
                        } else {
                                mtx->dprio = mtx->rtwq.max_prio;
                        }
                        xwup_mtxtree_add(mt, mtx);
                        t->prio.d = (t->prio.s > mt->maxprio) ? t->prio.s : mt->maxprio;
                        xwup_thd_wakeup(t);
                        xwup_thd_chprio(cthd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
                        /* 如果函数在 `xwsync_cond_wait_to()` 中被调用，
                           当前线程已经不是 `XWUP_SKDOBJ_ST_RUNNING` 状态，
                           `xwup_skd_chkpmpt()` 不起作用。 */
                        xwup_skd_chkpmpt();
                } else {
                        /* Case 2: 没有线程在等待互斥锁 */
                        xwup_thd_chprio(cthd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
                        /* 如果函数在 `xwsync_cond_wait_to()` 中被调用，
                           当前线程已经不是 `XWUP_SKDOBJ_ST_RUNNING` 状态，
                           `xwup_skd_chkpmpt()` 不起作用。 */
                        xwup_skd_chkpmpt();
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_mtx_lock(struct xwup_mtx * mtx)
{
        return xwup_mtx_lock_to(mtx, XWTM_MAX);
}

static __xwup_code
xwer_t xwup_mtx_block_to(struct xwup_mtx * mtx,
                         struct xwup_skd * xwskd, struct xwup_thd * thd,
                         xwtm_t to, xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwup_tt * xwtt;

        xwtt = &xwskd->tt;

        /* 加入等待队列 */
        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_FROZEN) & thd->state));

        /* 检查是否被中断 */
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_EXITING & thd->state)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
                rc = -EINTR;
                goto err_intr;
        }
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
        xwup_thd_eq_rtwq(thd, &mtx->rtwq, (xwu16_t)XWUP_WQTYPE_MTX);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_mtx_chprio(mtx);

        /* 加入到时间树 */
        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING);
        // cppcheck-suppress [misra-c2012-17.7]
        xwup_thd_tt_add_locked(thd, xwtt, to, cpuirq);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_unlock(); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_lock(); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if ((xwu16_t)XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING);
                }
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = -EINTR;
        } else if ((xwu16_t)XWUP_WQN_REASON_UP == thd->wqn.reason) {
                xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING);
                }
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                rc = XWOK;
        } else if ((xwsq_t)XWUP_TTN_WKUPRS_TIMEDOUT == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_mtx_chprio(mtx);
                        rc = -ETIMEDOUT;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if ((xwu16_t)XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                                rc = -EINTR;
                        } else if ((xwu16_t)XWUP_WQN_REASON_UP == thd->wqn.reason) {
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if ((xwsq_t)XWUP_TTN_WKUPRS_INTR == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_mtx_chprio(mtx);
                        rc = -EINTR;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if ((xwu16_t)XWUP_WQN_REASON_INTR == thd->wqn.reason) {
                                rc = -EINTR;
                        } else if ((xwu16_t)XWUP_WQN_REASON_UP == thd->wqn.reason) {
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

err_intr:
        return rc;
}

static __xwup_code
xwer_t xwup_mtx_lock_or_block_to(struct xwup_mtx * mtx, struct xwup_thd * thd,
                                 xwtm_t to)
{
        struct xwup_skd * xwskd;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwskd = xwup_skd_dspmpt_lc();
        mt = &thd->mt;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        } else if (NULL != mtx->ownertree) {
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                rc = xwup_skd_wakelock_lock();
                if (rc < 0) {
                        /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR。*/
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
                        rc = -EINTR;
                } else {
#endif
                        rc = xwup_mtx_block_to(mtx, xwskd, thd, to, cpuirq);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwup_skd_wakelock_unlock();
                }
#endif
        } else {
                xwup_mtxtree_add(mt, mtx);
                xwup_thd_chprio(thd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
                xwup_skd_chkpmpt();
        }
        return rc;
}

__xwup_api
xwer_t xwup_mtx_lock_to(struct xwup_mtx * mtx, xwtm_t to)
{
        struct xwup_thd * cthd;
        struct xwup_skd * xwskd;
        struct xwup_syshwt * hwt;
        xwtm_t now;
        xwer_t rc;

        if (!xwospl_cpuirq_test_lc()) {
                rc = -EDISIRQ;
                goto err_disirq;
        }
        if (!xwup_skd_tstth_lc()) {
                rc = -EDISIRQ;
                goto err_disirq;
        }
        if (!xwup_skd_tstpmpt_lc()) {
                rc = -EDISPMPT;
                goto err_dispmpt;
        }
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        if (!xwup_skd_tstbh_lc()) {
                rc = -EDISBH;
                goto err_disbh;
        }
#endif
        cthd = xwup_skd_get_cthd_lc();
        xwskd = xwup_skd_get_lc();
        hwt = &xwskd->tt.hwt;
        now = xwup_syshwt_get_time(hwt);
        if (xwtm_cmp(to, now) < 0) {
                rc = -ETIMEDOUT;
        } else if (xwtm_cmp(to, now) == 0) {
                rc = xwup_mtx_trylock(mtx);
                if (rc < 0) {
                        if (-ENODATA == rc) {
                                rc = -ETIMEDOUT;
                        }
                }
        } else {
                rc = xwup_mtx_lock_or_block_to(mtx, cthd, to);
        }

err_disbh:
err_dispmpt:
err_disirq:
        return rc;
}

static __xwup_code
xwer_t xwup_mtx_block_unintr(struct xwup_mtx * mtx, struct xwup_thd * thd,
                             xwreg_t cpuirq)
{
        xwer_t rc;

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, ((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING |
                                        (xwsq_t)XWUP_SKDOBJ_ST_UNINTERRUPTED));
        xwup_thd_eq_rtwq(thd, &mtx->rtwq, (xwu16_t)XWUP_WQTYPE_MTX);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_mtx_chprio(mtx);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
        xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if ((xwu16_t)XWUP_WQN_REASON_UP == thd->wqn.reason) {
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwup_code
xwer_t xwup_mtx_lock_or_block_unintr(struct xwup_mtx * mtx, struct xwup_thd * thd)
{
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        mt = &thd->mt;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        } else if (NULL != mtx->ownertree) {
                rc = xwup_mtx_block_unintr(mtx, thd, cpuirq);
        } else {
                xwup_mtxtree_add(mt, mtx);
                xwup_thd_chprio(thd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
                xwup_skd_chkpmpt();
        }
        return rc;
}

__xwup_api
xwer_t xwup_mtx_lock_unintr(struct xwup_mtx * mtx)
{
        xwer_t rc;
        struct xwup_thd * cthd;

        if (!xwospl_cpuirq_test_lc()) {
                rc = -EDISIRQ;
        } else if (!xwup_skd_tstth_lc()) {
                rc = -EDISIRQ;
        } else if (!xwup_skd_tstpmpt_lc()) {
                rc = -EDISPMPT;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        } else if (!xwup_skd_tstbh_lc()) {
                rc = -EDISBH;
#endif
        } else {
                cthd = xwup_skd_get_cthd_lc();
                rc = xwup_mtx_lock_or_block_unintr(mtx, cthd);
        }
        return rc;
}

__xwup_api
xwer_t xwup_mtx_trylock(struct xwup_mtx * mtx)
{
        struct xwup_thd * cthd;
        struct xwup_mtxtree * mt;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwup_skd_dspmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        cthd = xwup_skd_get_cthd_lc();
        mt = &cthd->mt;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (mtx->ownertree == mt) {
                mtx->reentrant++;
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        } else if (NULL != mtx->ownertree) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EWOULDBLOCK;
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
        } else {
                xwup_mtxtree_add(mt, mtx);
                xwup_thd_chprio(cthd);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_enpmpt_lc(); // cppcheck-suppress [misra-c2012-17.7]
                xwup_skd_chkpmpt();
        }
        return rc;
}

__xwup_api
xwer_t xwup_mtx_get_status(struct xwup_mtx * mtx, xwsq_t * status)
{
        volatile struct xwup_mtxtree * ownertree;

        // cppcheck-suppress [misra-c2012-17.7]
        xwmb_read(struct xwup_mtxtree *, ownertree, &mtx->ownertree);
        if (NULL != ownertree) {
                *status = (xwsq_t)XWOS_LKST_LOCKED;
        } else {
                *status = (xwsq_t)XWOS_LKST_UNLOCKED;
        }
        return XWOK;
}
