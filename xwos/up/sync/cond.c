/**
 * @file
 * @brief XWOS UP内核同步机制：条件量
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
#if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_COND_MEMSLICE) && (1 == XWOSCFG_SYNC_COND_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
#  include <xwos/mm/sma.h>
#elif defined(XWOSCFG_SYNC_COND_STDC_MM) && (1 == XWOSCFG_SYNC_COND_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#include <xwos/up/plwq.h>
#include <xwos/up/sync/obj.h>
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
#  include <xwos/up/sync/evt.h>
#endif
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/lock/fakespinlock.h>
#if (defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX))
#  include <xwos/up/lock/mtx.h>
#elif (defined(XWOSCFG_LOCK_FAKEMTX) && (1 == XWOSCFG_LOCK_FAKEMTX))
#  include <xwos/up/lock/fakemtx.h>
#endif
#include <xwos/up/sync/cond.h>

/* #define XWOS_CONDLOGF */ /**< 调试日志开关 */
#ifdef XWOS_CONDLOGF
#  define xwos_condlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_condlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
/**
 * @brief 结构体 `xwup_cond` 的对象缓存
 */
static __xwup_data struct xwmm_mempool_objcache xwup_cond_cache;

/**
 * @brief 结构体 `xwup_cond` 的对象缓存的名字
 */
const __xwup_rodata char xwup_cond_cache_name[] = "xwup.sync.cond.cache";
#elif defined(XWOSCFG_SYNC_COND_MEMSLICE) && (1 == XWOSCFG_SYNC_COND_MEMSLICE)
/**
 * @brief 结构体 `xwup_cond` 的对象缓存
 */
static __xwup_data struct xwmm_memslice xwup_cond_cache;

/**
 * @brief 结构体 `xwup_cond` 的对象缓存的名字
 */
const __xwup_rodata char xwup_cond_cache_name[] = "xwup.sync.cond.cache";
#elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
static __xwup_data struct xwmm_sma * xwup_cond_cache;
#endif

#if (1 == XWOSRULE_SYNC_COND_CREATE_DELETE)
static __xwup_code
struct xwup_cond * xwup_cond_alloc(void);

static __xwup_code
void xwup_cond_free(struct xwup_cond * cond);
#endif

static __xwup_code
xwer_t xwup_cond_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_SYNC_COND_CREATE_DELETE)
static __xwup_code
xwer_t xwup_cond_dgc(struct xwos_object * obj);
#endif

static __xwup_code
xwer_t xwup_cond_broadcast_once(struct xwup_cond * cond, bool * retry);

static __xwup_code
xwer_t xwup_cond_unlock(void * lock, xwsq_t lktype, void * lkdata);

static __xwup_code
xwer_t xwup_cond_lock(void * lock, xwsq_t lktype, xwtm_t to,
                      bool unintr, void * lkdata);

static __xwup_code
xwer_t xwup_cond_test(struct xwup_cond * cond,
                      struct xwup_skd * xwskd, struct xwup_thd * thd,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwtm_t to, xwsq_t * lkst);

static __xwup_code
xwer_t xwup_cond_blkthd_to_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                           struct xwup_skd * xwskd,
                                           struct xwup_thd * thd,
                                           void * lock, xwsq_t lktype,
                                           void * lkdata,
                                           xwtm_t to, xwsq_t * lkst,
                                           xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_cond_blkthd_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                        struct xwup_thd * thd,
                                        void * lock, xwsq_t lktype,
                                        void * lkdata,
                                        xwsq_t * lkst,
                                        xwreg_t cpuirq);

static __xwup_code
xwer_t xwup_cond_test_unintr(struct xwup_cond * cond,
                             struct xwup_thd * thd,
                             void * lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst);

#if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_cond` 的对象缓存
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
xwer_t xwup_cond_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwup_cond_cache,
                                        &mp->pa,
                                        xwup_cond_cache_name,
                                        sizeof(struct xwup_cond),
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwup_cond_construct,
                                        (dtor_f)xwup_cond_destruct);
        return rc;
}
#elif defined(XWOSCFG_SYNC_COND_MEMSLICE) && (1 == XWOSCFG_SYNC_COND_MEMSLICE)
/**
 * @brief XWUP INIT CODE：初始化结构体 `xwup_cond` 的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwup_api
xwer_t xwup_cond_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwup_cond_cache, zone_origin, zone_size,
                                sizeof(struct xwup_cond),
                                xwup_cond_cache_name,
                                (ctor_f)xwup_cond_construct,
                                (dtor_f)xwup_cond_destruct);
        return rc;
}
#elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_cond` 的对象缓存
 * @param[in] sma: 简单内存分配器
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwup_init_code
void xwup_cond_cache_init(struct xwmm_sma * sma)
{
        xwup_cond_cache = sma;
}
#endif

#if (1 == XWOSRULE_SYNC_COND_CREATE_DELETE)
/**
 * @brief 申请条件量对象
 * @return 条件量对象的指针
 */
static __xwup_code
struct xwup_cond * xwup_cond_alloc(void)
{
#  if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
        union {
                struct xwup_cond * cond;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_objcache_alloc(&xwup_cond_cache, &mem.anon);
        if (rc < 0) {
                mem.cond = err_ptr(rc);
        }
        return mem.cond;
#  elif defined(XWOSCFG_SYNC_COND_MEMSLICE) && (1 == XWOSCFG_SYNC_COND_MEMSLICE)
        union {
                struct xwup_cond * cond;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwup_cond_cache, &mem.anon);
        if (rc < 0) {
                mem.cond = err_ptr(rc);
        }
        return mem.cond;
#  elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
        union {
                struct xwup_cond * cond;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwup_cond_cache,
                            sizeof(struct xwup_cond), XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.cond = err_ptr(rc);
        } else {
                xwup_cond_construct(mem.cond);
        }
        return mem.cond;
#  elif defined(XWOSCFG_SYNC_COND_STDC_MM) && (1 == XWOSCFG_SYNC_COND_STDC_MM)
        struct xwup_cond * cond;

        cond = malloc(sizeof(struct xwup_cond));
        if (NULL == cond) {
                cond = err_ptr(-ENOMEM);
        } else {
                xwup_cond_construct(cond);
        }
        return cond;
#  else
        return err_ptr(-ENOSYS);
#  endif
}
#endif

#if (1 == XWOSRULE_SYNC_COND_CREATE_DELETE)
/**
 * @brief XWUP API：释放条件量对象
 * @param[in] cond: 条件量对象的指针
 */
static __xwup_code
void xwup_cond_free(struct xwup_cond * cond)
{
#  if defined(XWOSCFG_SYNC_COND_MEMPOOL) && (1 == XWOSCFG_SYNC_COND_MEMPOOL)
        xwmm_mempool_objcache_free(&xwup_cond_cache, cond);
#  elif defined(XWOSCFG_SYNC_COND_MEMSLICE) && (1 == XWOSCFG_SYNC_COND_MEMSLICE)
        xwmm_memslice_free(&xwup_cond_cache, cond);
#  elif defined(XWOSCFG_SYNC_COND_SMA) && (1 == XWOSCFG_SYNC_COND_SMA)
        xwmm_sma_free(xwup_cond_cache, cond);
#  elif defined(XWOSCFG_SYNC_COND_STDC_MM) && (1 == XWOSCFG_SYNC_COND_STDC_MM)
        xwup_cond_destruct(cond);
        free(cond);
#  else
        XWOS_UNUSED(cond);
#  endif
}
#endif

__xwup_code
void xwup_cond_construct(struct xwup_cond * cond)
{
        xwup_synobj_construct(&cond->synobj);
}

__xwup_code
void xwup_cond_destruct(struct xwup_cond * cond)
{
        xwup_synobj_destruct(&cond->synobj);
}

static __xwup_code
xwer_t xwup_cond_sgc(struct xwos_object * obj)
{
        struct xwup_cond * cond;

        cond = xwcc_derof(obj, struct xwup_cond, synobj.xwobj);
        xwup_cond_destruct(cond);
        return XWOK;
}

#if (1 == XWOSRULE_SYNC_COND_CREATE_DELETE)
static __xwup_code
xwer_t xwup_cond_dgc(struct xwos_object * obj)
{
        struct xwup_cond * cond;

        cond = xwcc_derof(obj, struct xwup_cond, synobj.xwobj);
        xwup_cond_free((struct xwup_cond *)cond);
        return XWOK;
}
#endif

__xwup_api
xwer_t xwup_cond_acquire(struct xwup_cond * cond, xwsq_t tik)
{
        return xwup_synobj_acquire(&cond->synobj, tik);
}

__xwup_api
xwer_t xwup_cond_release(struct xwup_cond * cond, xwsq_t tik)
{
        return xwup_synobj_release(&cond->synobj, tik);
}

__xwup_api
xwer_t xwup_cond_grab(struct xwup_cond * cond)
{
        return xwup_synobj_grab(&cond->synobj);
}

__xwup_api
xwer_t xwup_cond_put(struct xwup_cond * cond)
{
        return xwup_synobj_put(&cond->synobj);
}

__xwup_code
xwer_t xwup_cond_activate(struct xwup_cond * cond, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwup_synobj_activate(&cond->synobj, gcfunc);
        if (rc < 0) {
                goto err_synobj_activate;
        }
        xwup_plwq_init(&cond->wq);
        cond->neg = false;
        return XWOK;

err_synobj_activate:
        return rc;
}

__xwup_api
xwer_t xwup_cond_init(struct xwup_cond * cond)
{
        xwup_synobj_construct(&cond->synobj);
        return xwup_cond_activate(cond, xwup_cond_sgc);
}

__xwup_api
xwer_t xwup_cond_fini(struct xwup_cond * cond)
{
        return xwup_cond_put(cond);
}

#if (1 == XWOSRULE_SYNC_COND_CREATE_DELETE)
__xwup_api
xwer_t xwup_cond_create(struct xwup_cond ** ptrbuf)
{
        struct xwup_cond * cond;
        xwer_t rc;

        *ptrbuf = NULL;
        cond = xwup_cond_alloc();
        if (is_err(cond)) {
                rc = ptr_err(cond);
        } else {
                rc = xwup_cond_activate(cond, xwup_cond_dgc);
                if (rc < 0) {
                        xwup_cond_free(cond);
                } else {
                        *ptrbuf = cond;
                }
        }
        return rc;
}
#endif

#if (1 == XWOSRULE_SYNC_COND_CREATE_DELETE)
__xwup_api
xwer_t xwup_cond_delete(struct xwup_cond * cond, xwsq_t tik)
{
        return xwup_cond_release(cond, tik);
}
#endif

#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
__xwup_api
xwer_t xwup_cond_bind(struct xwup_cond * cond, struct xwup_evt * sel, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_bind(sel, &cond->synobj, pos, false);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_cond_unbind(struct xwup_cond * cond, struct xwup_evt * sel)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_unbind(sel, &cond->synobj, false);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}
#else
__xwup_api
xwer_t xwup_cond_bind(struct xwup_cond * cond, struct xwup_evt * sel, xwsq_t pos)
{
        XWOS_UNUSED(cond);
        XWOS_UNUSED(sel);
        XWOS_UNUSED(pos);
        return -ENOSYS;
}

__xwup_api
xwer_t xwup_cond_unbind(struct xwup_cond * cond, struct xwup_evt * sel)
{
        XWOS_UNUSED(cond);
        XWOS_UNUSED(sel);
        return -ENOSYS;
}
#endif

__xwup_api
xwer_t xwup_cond_freeze(struct xwup_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (cond->neg) {
                rc = -EALREADY;
        } else {
                cond->neg = true;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_cond_thaw(struct xwup_cond * cond)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (cond->neg) {
                cond->neg = false;
        } else {
                rc = -EALREADY;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief 中断条件量等待队列中的一个节点
 * @param[in] cond: 条件量对象的指针
 * @param[in] wqn: 等待队列节点
 */
__xwup_code
xwer_t xwup_cond_intr(struct xwup_cond * cond, struct xwup_wqn * wqn)
{
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_plwq_remove(&cond->wq, wqn);
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

/**
 * @brief 中断条件量等待队列中的所有节点
 * @param[in] cond: 条件量对象的指针
 */
__xwup_code
xwer_t xwup_cond_intr_all(struct xwup_cond * cond)
{
        struct xwup_wqn * c;
        xwup_wqn_f cb;
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        xwup_plwq_itr_wqn_rm(c, &cond->wq) {
                xwup_plwq_remove(&cond->wq, c); // cppcheck-suppress [misra-c2012-17.7]
                c->wq = NULL;
                c->type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                c->reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                cb = c->cb;
                c->cb = NULL;
                xwospl_cpuirq_restore_lc(cpuirq);
                cb(c);
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
        if (cond->neg) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
                *retry = false;
        } else {
                wqn = xwup_plwq_choose(&cond->wq);
                if (NULL != wqn) {
                        wqn->wq = NULL;
                        wqn->type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = (xwu16_t)XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(wqn);
                        *retry = true;
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        *retry = false;
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_cond_broadcast(struct xwup_cond * cond)
{
        bool retry;
        xwer_t rc;

        retry = false;
        do {
                rc = xwup_cond_broadcast_once(cond, &retry);
        } while (retry);
#if defined(XWOSCFG_SYNC_EVT) && (1 == XWOSCFG_SYNC_EVT)
        if (XWOK == rc) {
                xwreg_t cpuirq;
                struct xwup_evt * evt;
                struct xwup_synobj * synobj;

                synobj = &cond->synobj;
                xwospl_cpuirq_save_lc(&cpuirq);
                evt = synobj->sel.evt;
                if (NULL != evt) {
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwup_sel_obj_s1i(evt, synobj);
                }
                xwospl_cpuirq_restore_lc(cpuirq);
        }
#endif
        return rc;
}

__xwup_api
xwer_t xwup_cond_unicast(struct xwup_cond * cond)
{
        struct xwup_wqn * wqn;
        xwup_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (cond->neg) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -ENEGATIVE;
        } else {
                wqn = xwup_plwq_choose(&cond->wq);
                if (NULL != wqn) {
                        wqn->wq = NULL;
                        wqn->type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        wqn->reason = (xwu16_t)XWUP_WQN_REASON_UP;
                        cb = wqn->cb;
                        wqn->cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(wqn);
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 解锁给定类型的锁
 * @param[in] lock: 锁的地址
 * @param[in] lktype: 锁的类型
 * @param[in] lkdata: 锁的数据
 */
__xwup_code
xwer_t xwup_cond_unlock(void * lock, xwsq_t lktype, void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;
        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
#if ((defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)) || \
     (defined(XWOSCFG_LOCK_FAKEMTX) && (1 == XWOSCFG_LOCK_FAKEMTX)))
                rc = xwup_mtx_unlock(ulk.xwup.mtx);
#endif
                break;
        case XWOS_LK_SPLK:
                xwup_splk_unlock(ulk.xwup.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwup_sqlk_wr_unlock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwup_sqlk_rdex_unlock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (NULL != ulk.cb->unlock) {
                        rc = ulk.cb->unlock(lkdata);
                }
                break;
        case XWOS_LK_NONE:
        default:
                break;
        }
        return rc;
}

/**
 * @brief 上锁给定类型的锁
 * @param[in] lock: 锁的地址
 * @param[in] lktype: 锁的类型
 * @param[in] to: 期望唤醒的时间点
 * @param[in] unintr: 是否不可中断
 * @param[in] lkdata: 锁的数据
 */
static __xwup_code
xwer_t xwup_cond_lock(void * lock, xwsq_t lktype, xwtm_t to, bool unintr,
                      void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;
        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
#if ((defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)) || \
     (defined(XWOSCFG_LOCK_FAKEMTX) && (1 == XWOSCFG_LOCK_FAKEMTX)))
                if (unintr) {
                        rc = xwup_mtx_lock_unintr(ulk.xwup.mtx);
                } else {
                        rc = xwup_mtx_lock_to(ulk.xwup.mtx, to);
                }
#else
                XWOS_UNUSED(to);
                XWOS_UNUSED(unintr);
#endif
                break;
        case XWOS_LK_SPLK:
                xwup_splk_lock(ulk.xwup.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwup_sqlk_wr_lock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwup_sqlk_rdex_lock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (NULL != ulk.cb->lock) {
                        rc = ulk.cb->lock(lkdata);
                }
                break;
        case XWOS_LK_NONE:
        default:
                XWOS_UNUSED(to);
                XWOS_UNUSED(unintr);
                break;
        }
        return rc;
}

static __xwup_code
xwer_t xwup_cond_blkthd_to_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                           struct xwup_skd * xwskd,
                                           struct xwup_thd * thd,
                                           void * lock, xwsq_t lktype,
                                           void * lkdata,
                                           xwtm_t to, xwsq_t * lkst,
                                           xwreg_t cpuirq)
{
        struct xwup_tt * xwtt;
        xwsq_t pmpt;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwsq_t bh;
#endif
        xwer_t rc;

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
        xwup_thd_eq_plwq(thd, &cond->wq, (xwu16_t)XWUP_WQTYPE_COND);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 同步解锁 */
        rc = xwup_cond_unlock(lock, lktype, lkdata);
        if (XWOK == rc) {
                *lkst = (xwsq_t)XWOS_LKST_UNLOCKED;
        }

        /* 加入时间树 */
        xwup_sqlk_wr_lock_cpuirq(&xwtt->lock);
        xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING);
        // cppcheck-suppress [misra-c2012-17.7]
        xwup_thd_tt_add_locked(thd, xwtt, to, cpuirq);
        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

        /* 调度 */
        xwup_skd_svpmpt_lc(&pmpt); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_rspmpt_lc(0); // cppcheck-suppress [misra-c2012-17.7]
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwup_skd_svbh_lc(&bh); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_rsbh_lc(0); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwospl_cpuirq_enable_lc();
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_unlock(); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_lock(); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwospl_cpuirq_restore_lc(cpuirq);
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwup_skd_rsbh_lc(bh); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwup_skd_rspmpt_lc(pmpt); // cppcheck-suppress [misra-c2012-17.7]

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
                if ((xwsq_t)XWOS_LKST_UNLOCKED == *lkst) {
                        rc = xwup_cond_lock(lock, lktype, to, false, lkdata);
                        if (XWOK == rc) {
                                *lkst = (xwsq_t)XWOS_LKST_LOCKED;
                        }
                }
                rc = XWOK;
        } else if ((xwsq_t)XWUP_TTN_WKUPRS_TIMEDOUT == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&cond->wq, &thd->wqn);
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
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == *lkst) {
                                        rc = xwup_cond_lock(lock, lktype, to,
                                                            false, lkdata);
                                        if (XWOK == rc) {
                                                *lkst = (xwsq_t)XWOS_LKST_LOCKED;
                                        }
                                }
                                rc = XWOK;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
        } else if ((xwsq_t)XWUP_TTN_WKUPRS_INTR == thd->ttn.wkuprs) {
                xwospl_cpuirq_disable_lc();
                rc = xwup_plwq_remove(&cond->wq, &thd->wqn);
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
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == *lkst) {
                                        rc = xwup_cond_lock(lock, lktype, to,
                                                            false, lkdata);
                                        if (XWOK == rc) {
                                                *lkst = (xwsq_t)XWOS_LKST_LOCKED;
                                        }
                                }
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
xwer_t xwup_cond_test(struct xwup_cond * cond,
                      struct xwup_skd * xwskd, struct xwup_thd * thd,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwtm_t to, xwsq_t * lkst)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        rc = xwup_skd_wakelock_lock();
        if (rc < 0) {
                /* 系统准备进入低功耗模式，线程需被冻结，返回-EINTR。*/
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINTR;
        } else {
#endif
                rc = xwup_cond_blkthd_to_unlkwq_cpuirqrs(cond, xwskd, thd,
                                                         lock, lktype, lkdata,
                                                         to, lkst,
                                                         cpuirq);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                xwup_skd_wakelock_unlock(); // cppcheck-suppress [misra-c2012-17.7]
        }
#endif
        return rc;
}

__xwup_api
xwer_t xwup_cond_wait(struct xwup_cond * cond,
                      void * lock, xwsq_t lktype, void * lkdata,
                      xwsq_t * lkst)
{
        return xwup_cond_wait_to(cond, lock, lktype, lkdata, XWTM_MAX, lkst);
}

__xwup_api
xwer_t xwup_cond_wait_to(struct xwup_cond * cond,
                         void * lock, xwsq_t lktype, void * lkdata,
                         xwtm_t to, xwsq_t * lkst)
{
        struct xwup_thd * cthd;
        struct xwup_skd * xwskd;
        struct xwup_syshwt * hwt;
        xwtm_t now;
        xwer_t rc;

        *lkst = (xwsq_t)XWOS_LKST_LOCKED;
        cthd = xwup_skd_get_cthd_lc();
        xwskd = xwup_skd_get_lc();
        hwt = &xwskd->tt.hwt;
        now = xwup_syshwt_get_time(hwt);
        if (xwtm_cmp(to, now) < 0) {
                rc = -ETIMEDOUT;
        } else if (xwtm_cmp(to, now) == 0) {
                rc = xwup_cond_unlock(lock, lktype, lkdata);
                if (XWOK == rc) {
                        *lkst = (xwsq_t)XWOS_LKST_UNLOCKED;
                }
                rc = -ETIMEDOUT;
        } else {
                rc = xwup_cond_test(cond, xwskd, cthd, lock, lktype, lkdata, to, lkst);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_cond_blkthd_unlkwq_cpuirqrs(struct xwup_cond * cond,
                                        struct xwup_thd * thd,
                                        void * lock, xwsq_t lktype,
                                        void * lkdata,
                                        xwsq_t * lkst,
                                        xwreg_t cpuirq)
{
        xwer_t rc;
        xwsq_t pmpt;
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwsq_t bh;
#endif

        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_FROZEN) & thd->state));
        /* 加入等待队列 */
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_BLOCKING);
        xwup_thd_eq_plwq(thd, &cond->wq, (xwu16_t)XWUP_WQTYPE_COND);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 同步解锁 */
        rc = xwup_cond_unlock(lock, lktype, lkdata);
        if (XWOK == rc) {
                *lkst = (xwsq_t)XWOS_LKST_UNLOCKED;
        }

        /* 调度 */
        xwup_skd_svpmpt_lc(&pmpt); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_rspmpt_lc(0); // cppcheck-suppress [misra-c2012-17.7]
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwup_skd_svbh_lc(&bh); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_rsbh_lc(0); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwospl_cpuirq_enable_lc();
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        xwup_skd_rsbh_lc(bh); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwup_skd_rspmpt_lc(pmpt); // cppcheck-suppress [misra-c2012-17.7]

        /* 判断唤醒原因 */
        if ((xwu16_t)XWUP_WQN_REASON_UP == thd->wqn.reason) {
                if ((xwsq_t)XWOS_LKST_UNLOCKED == *lkst) {
                        rc = xwup_cond_lock(lock, lktype, 0, true, lkdata);
                        if (XWOK == rc) {
                                *lkst = (xwsq_t)XWOS_LKST_LOCKED;
                        }
                }
                rc = XWOK;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;
}

static __xwup_code
xwer_t xwup_cond_test_unintr(struct xwup_cond * cond,
                             struct xwup_thd * thd,
                             void * lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_cond_blkthd_unlkwq_cpuirqrs(cond, thd,
                                              lock, lktype, lkdata,
                                              lkst, cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_cond_wait_unintr(struct xwup_cond * cond,
                             void * lock, xwsq_t lktype, void * lkdata,
                             xwsq_t * lkst)
{
        struct xwup_thd * cthd;
        xwer_t rc;

        *lkst = (xwsq_t)XWOS_LKST_LOCKED;
        cthd = xwup_skd_get_cthd_lc();
        rc = xwup_cond_test_unintr(cond, cthd, lock, lktype, lkdata, lkst);
        return rc;
}
