/**
 * @file
 * @brief XWOS UP内核同步机制：实时信号量
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
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
#  include <xwos/mm/sma.h>
#elif defined(XWOSCFG_SYNC_SEM_STDC_MM) && (1 == XWOSCFG_SYNC_SEM_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#include <xwos/up/rtwq.h>
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
#  include <xwos/up/sync/evt.h>
#endif
#include <xwos/up/sync/rtsem.h>

/* #define XWOS_SEMLOGF */ /**< 调试日志开关 */
#ifdef XWOS_SEMLOGF
#  define xwos_semlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_semlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
/**
 * @brief 结构体 `xwup_rtsem` 的对象缓存
 */
static __xwup_data struct xwmm_mempool_objcache xwup_rtsem_cache;

/**
 * @brief 结构体 `xwup_rtsem` 的对象缓存的名字
 */
const __xwup_rodata char xwup_rtsem_cache_name[] = "xwup.sync.rtsem.cache";
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
/**
 * @brief 结构体 `xwup_rtsem` 的对象缓存
 */
static __xwup_data struct xwmm_memslice xwup_rtsem_cache;

/**
 * @brief 结构体 `xwup_rtsem` 的对象缓存的名字
 */
const __xwup_rodata char xwup_rtsem_cache_name[] = "xwup.sync.rtsem.cache";
#elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
static __xwup_data struct xwmm_sma * xwup_rtsem_cache;
#endif

#if (1 == XWOSRULE_SYNC_SEM_CREATE_DELETE)
static __xwup_code
struct xwup_rtsem * xwup_rtsem_alloc(void);

static __xwup_code
void xwup_rtsem_free(struct xwup_rtsem * sem);
#endif

static __xwup_code
void xwup_rtsem_construct(struct xwup_rtsem * sem);

static __xwup_code
void xwup_rtsem_destruct(struct xwup_rtsem * sem);

static __xwup_code
xwer_t xwup_rtsem_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_SYNC_SEM_CREATE_DELETE)
static __xwup_code
xwer_t xwup_rtsem_dgc(struct xwos_object * obj);
#endif

static __xwup_code
xwer_t xwup_rtsem_activate(struct xwup_rtsem * sem,
                           xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc);

static __xwup_code
xwer_t xwup_rtsem_blkthd_to_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                            struct xwup_skd * xwskd,
                                            struct xwup_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_rtsem_test(struct xwup_rtsem * sem,
                       struct xwup_skd * xwskd, struct xwup_thd * thd,
                       xwtm_t to);

static __xwup_code
xwer_t xwup_rtsem_blkthd_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                         struct xwup_thd * thd,
                                         xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_rtsem_test_unintr(struct xwup_rtsem * sem, struct xwup_thd * thd);

#if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_rtsem` 的对象缓存
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
xwer_t xwup_rtsem_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwup_rtsem_cache,
                                        &mp->pa,
                                        xwup_rtsem_cache_name,
                                        sizeof(struct xwup_rtsem),
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwup_rtsem_construct,
                                        (dtor_f)xwup_rtsem_destruct);
        return rc;
}
#elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
/**
 * @brief XWUP INIT CODE：初始化结构体 `xwup_rtsem` 的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwup_api
xwer_t xwup_rtsem_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwup_rtsem_cache, zone_origin, zone_size,
                                sizeof(struct xwup_rtsem),
                                xwup_rtsem_cache_name,
                                (ctor_f)xwup_rtsem_construct,
                                (dtor_f)xwup_rtsem_destruct);
        return rc;
}
#elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_rtsem` 的对象缓存
 * @param[in] sma: 简单内存分配器
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwup_init_code
void xwup_rtsem_cache_init(struct xwmm_sma * sma)
{
        xwup_rtsem_cache = sma;
}
#endif

#if (1 == XWOSRULE_SYNC_SEM_CREATE_DELETE)
/**
 * @brief 申请实时信号量对象
 * @return 信号量对象的指针
 */
static __xwup_code
struct xwup_rtsem * xwup_rtsem_alloc(void)
{
#  if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
        union {
                struct xwup_rtsem * rtsem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_objcache_alloc(&xwup_rtsem_cache, &mem.anon);
        if (rc < 0) {
                mem.rtsem = err_ptr(rc);
        }
        return mem.rtsem;
#  elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
        union {
                struct xwup_rtsem * rtsem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwup_rtsem_cache, &mem.anon);
        if (rc < 0) {
                mem.rtsem = err_ptr(rc);
        }
        return mem.rtsem;
#  elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
        union {
                struct xwup_rtsem * rtsem;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwup_rtsem_cache,
                            sizeof(struct xwup_rtsem), XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.rtsem = err_ptr(rc);
        } else {
                xwup_rtsem_construct(mem.rtsem);
        }
        return mem.rtsem;
#  elif defined(XWOSCFG_SYNC_RTSEM_STDC_MM) && (1 == XWOSCFG_SYNC_RTSEM_STDC_MM)
        struct xwup_rtsem * rtsem;

        rtsem = malloc(sizeof(struct xwup_rtsem));
        if (NULL == rtsem) {
                rtsem = err_ptr(-ENOMEM);
        } else {
                xwup_rtsem_construct(rtsem);
        }
        return rtsem;
#  else
        return err_ptr(-ENOSYS);
#  endif
}
#endif

#if (1 == XWOSRULE_SYNC_SEM_CREATE_DELETE)
/**
 * @brief 释放实时信号量对象
 * @param[in] sem: 信号量对象的指针
 */
static __xwup_code
void xwup_rtsem_free(struct xwup_rtsem * sem)
{
#  if defined(XWOSCFG_SYNC_SEM_MEMPOOL) && (1 == XWOSCFG_SYNC_SEM_MEMPOOL)
        xwmm_mempool_objcache_free(&xwup_rtsem_cache, sem);
#  elif defined(XWOSCFG_SYNC_SEM_MEMSLICE) && (1 == XWOSCFG_SYNC_SEM_MEMSLICE)
        xwmm_memslice_free(&xwup_rtsem_cache, sem);
#  elif defined(XWOSCFG_SYNC_SEM_SMA) && (1 == XWOSCFG_SYNC_SEM_SMA)
        xwmm_sma_free(xwup_rtsem_cache, sem);
#  elif defined(XWOSCFG_SYNC_RTSEM_STDC_MM) && (1 == XWOSCFG_SYNC_RTSEM_STDC_MM)
        xwup_rtsem_destruct(sem);
        free(sem);
#  else
        XWOS_UNUSED(sem);
#  endif
}
#endif

static __xwup_code
void xwup_rtsem_construct(struct xwup_rtsem * sem)
{
        xwup_vsem_construct(&sem->vsem);
}

static __xwup_code
void xwup_rtsem_destruct(struct xwup_rtsem * sem)
{
        xwup_vsem_destruct(&sem->vsem);
}

static __xwup_code
xwer_t xwup_rtsem_sgc(struct xwos_object * obj)
{
        struct xwup_rtsem * rtsem;

        rtsem = xwcc_derof(obj, struct xwup_rtsem, vsem.synobj.xwobj);
        xwup_rtsem_destruct(rtsem);
        return XWOK;
}

#if (1 == XWOSRULE_SYNC_SEM_CREATE_DELETE)
static __xwup_code
xwer_t xwup_rtsem_dgc(struct xwos_object * obj)
{
        struct xwup_rtsem * rtsem;

        rtsem = xwcc_derof(obj, struct xwup_rtsem, vsem.synobj.xwobj);
        xwup_rtsem_free(rtsem);
        return XWOK;
}
#endif

__xwup_api
xwer_t xwup_rtsem_acquire(struct xwup_rtsem * sem, xwsq_t tik)
{
        return xwup_vsem_acquire(&sem->vsem, tik);
}

__xwup_api
xwer_t xwup_rtsem_release(struct xwup_rtsem * sem, xwsq_t tik)
{
        return xwup_vsem_release(&sem->vsem, tik);
}

__xwup_api
xwer_t xwup_rtsem_grab(struct xwup_rtsem * sem)
{
        return xwup_vsem_grab(&sem->vsem);
}

__xwup_api
xwer_t xwup_rtsem_put(struct xwup_rtsem * sem)
{
        return xwup_vsem_put(&sem->vsem);
}

static __xwup_code
xwer_t xwup_rtsem_activate(struct xwup_rtsem * sem,
                           xwssq_t val, xwssq_t max,
                           xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwup_vsem_activate(&sem->vsem, gcfunc);
        if (rc < 0) {
                goto err_vsem_activate;
        }
        xwup_rtwq_init(&sem->rtwq);
        sem->vsem.count = val;
        sem->vsem.max = max;
        return XWOK;

err_vsem_activate:
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_init(struct xwup_rtsem * sem, xwssq_t val, xwssq_t max)
{
        xwup_rtsem_construct(sem);
        return xwup_rtsem_activate(sem, val, max, xwup_rtsem_sgc);
}

__xwup_api
xwer_t xwup_rtsem_fini(struct xwup_rtsem * sem)
{
        return xwup_rtsem_put(sem);
}

#if (1 == XWOSRULE_SYNC_SEM_CREATE_DELETE)
__xwup_api
xwer_t xwup_rtsem_create(struct xwup_rtsem ** ptrbuf, xwssq_t val, xwssq_t max)
{
        struct xwup_rtsem * sem;
        xwer_t rc;

        *ptrbuf = NULL;
        sem = xwup_rtsem_alloc();
        if (is_err(sem)) {
                rc = ptr_err(sem);
        } else {
                rc = xwup_rtsem_activate(sem, val, max, xwup_rtsem_dgc);
                if (rc < 0) {
                        xwup_rtsem_free(sem);
                } else {
                        *ptrbuf = sem;
                }
        }
        return rc;
}
#endif

#if (1 == XWOSRULE_SYNC_SEM_CREATE_DELETE)
__xwup_api
xwer_t xwup_rtsem_delete(struct xwup_rtsem * sem, xwsq_t tik)
{
        return xwup_rtsem_release(sem, tik);
}
#endif

__xwup_api
xwer_t xwup_rtsem_bind(struct xwup_rtsem * sem, struct xwup_evt * sel, xwsq_t pos)
{
        return xwup_vsem_bind(&sem->vsem, sel, pos);
}

__xwup_api
xwer_t xwup_rtsem_unbind(struct xwup_rtsem * sem, struct xwup_evt * sel)
{
        return xwup_vsem_unbind(&sem->vsem, sel);
}

/**
 * @brief 中断实时信号量等待队列中的一个节点
 * @param[in] sem: 信号量对象的指针
 * @param[in] wqn: 等待队列节点
 * @return 错误码
 * @note
 * + 上下文：任意
 */
__xwup_code
xwer_t xwup_rtsem_intr(struct xwup_rtsem * sem, struct xwup_wqn * wqn)
{
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_rtwq_remove(&sem->rtwq, wqn);
        if (XWOK == rc) {
                wqn->wq = NULL;
                wqn->type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                wqn->reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                cb = wqn->cb;
                wqn->cb = NULL;
                xwospl_cpuirq_restore_lc(cpuirq);
                cb(wqn);
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_freeze(struct xwup_rtsem * sem)
{
        return xwup_vsem_freeze(&sem->vsem);
}

__xwup_api
xwer_t xwup_rtsem_thaw(struct xwup_rtsem * sem)
{
        return xwup_vsem_thaw(&sem->vsem);
}

__xwup_api
xwer_t xwup_rtsem_post(struct xwup_rtsem * sem)
{
        struct xwup_wqn * wqn;
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count < (xwssq_t)0) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwup_rtwq_choose(&sem->rtwq);
                if (NULL != wqn) {
                        wqn->wq = NULL;
                        wqn->type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = (xwu16_t)XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(wqn);
                } else {
                        /* add rtsem counter */
                        if (sem->vsem.count < sem->vsem.max) {
                                sem->vsem.count++;
                        } else {
                                rc = -ERANGE;
                        }
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                        if (sem->vsem.count > (xwssq_t)0) {
                                struct xwup_evt * evt;

                                evt = sem->vsem.synobj.sel.evt;
                                if (NULL != evt) {
                                        // cppcheck-suppress [misra-c2012-17.7]
                                        xwup_sel_obj_s1i(evt, &sem->vsem.synobj);
                                }
                        }
#endif
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_wait(struct xwup_rtsem * sem)
{
        return xwup_rtsem_wait_to(sem, XWTM_MAX);
}

static __xwup_code
xwer_t xwup_rtsem_blkthd_to_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                            struct xwup_skd * xwskd,
                                            struct xwup_thd * thd,
                                            xwtm_t to,
                                            xwreg_t cpuirq)
{
        xwer_t rc;
        struct xwup_tt * xwtt;

        xwtt = &xwskd->tt;
        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_FROZEN) & thd->state));

        /* 检查是否被中断 */
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_EXITING & thd->state)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINTR;
                goto err_intr;
        }
        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
        xwup_thd_eq_rtwq(thd, &sem->rtwq, (xwu16_t)XWUP_WQTYPE_RTSEM);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 加入时间树 */
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
                rc = xwup_rtwq_remove(&sem->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
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
                rc = xwup_rtwq_remove(&sem->rtwq, &thd->wqn);
                if (XWOK == rc) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                        thd->wqn.cb = NULL;
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
                        xwospl_cpuirq_restore_lc(cpuirq);
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
        return rc;

err_intr:
        return rc;
}

static __xwup_code
xwer_t xwup_rtsem_test(struct xwup_rtsem * sem,
                       struct xwup_skd * xwskd, struct xwup_thd * thd,
                       xwtm_t to)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count <= (xwssq_t)0) {
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                rc = xwup_skd_wakelock_lock();
                if (rc < 0) {
                        /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR*/
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -EINTR;
                } else {
#endif
                        rc = xwup_rtsem_blkthd_to_unlkwq_cpuirqrs(sem, xwskd, thd,
                                                                  to, cpuirq);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwup_skd_wakelock_unlock();
                }
#endif
        } else {
                sem->vsem.count--;
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                if ((xwssq_t)0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
        }

        return rc;
}

__xwup_api
xwer_t xwup_rtsem_wait_to(struct xwup_rtsem * sem, xwtm_t to)
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
                rc = xwup_rtsem_trywait(sem);
                if (rc < 0) {
                        if (-ENODATA == rc) {
                                rc = -ETIMEDOUT;
                        }
                }
        } else {
                rc = xwup_rtsem_test(sem, xwskd, cthd, to);
        }

err_disbh:
err_dispmpt:
err_disirq:
        return rc;
}

static __xwup_code
xwer_t xwup_rtsem_blkthd_unlkwq_cpuirqrs(struct xwup_rtsem * sem,
                                         struct xwup_thd * thd,
                                         xwreg_t cpuirq)
{
        xwer_t rc;

        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_FROZEN) & thd->state));

        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, ((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING |
                                        (xwsq_t)XWUP_SKDOBJ_ST_UNINTERRUPTED));
        xwup_thd_eq_rtwq(thd, &sem->rtwq, (xwu16_t)XWUP_WQTYPE_RTSEM);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 调度 */
        xwospl_cpuirq_enable_lc();
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
xwer_t xwup_rtsem_test_unintr(struct xwup_rtsem * sem, struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count <= (xwssq_t)0) {
                rc = xwup_rtsem_blkthd_unlkwq_cpuirqrs(sem, thd, cpuirq);
        } else {
                sem->vsem.count--;
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                if ((xwssq_t)0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_wait_unintr(struct xwup_rtsem * sem)
{
        struct xwup_thd * cthd;
        xwer_t rc;

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
                rc = xwup_rtsem_test_unintr(sem, cthd);
        }
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_trywait(struct xwup_rtsem * sem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (sem->vsem.count > (xwssq_t)0) {
                sem->vsem.count--;
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
                if ((xwssq_t)0 == sem->vsem.count) {
                        struct xwup_evt * evt;

                        evt = sem->vsem.synobj.sel.evt;
                        if (NULL != evt) {
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwup_sel_obj_c0i(evt, &sem->vsem.synobj);
                        }
                }
#endif
        } else {
                rc = -ENODATA;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_rtsem_get_max(struct xwup_rtsem * sem, xwssq_t * max)
{
        return xwup_vsem_get_max(&sem->vsem, max);
}

__xwup_api
xwer_t xwup_rtsem_get_value(struct xwup_rtsem * sem, xwssq_t * val)
{
        return xwup_vsem_get_value(&sem->vsem, val);
}
