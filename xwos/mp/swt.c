/**
 * @file
 * @brief XWOS MP内核：软件定时器
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
#include <xwos/mm/common.h>
#if defined(XWOSCFG_SKD_SWT_MEMPOOL) && (1 == XWOSCFG_SKD_SWT_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SKD_SWT_MEMSLICE) && (1 == XWOSCFG_SKD_SWT_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_SKD_SWT_SMA) && (1 == XWOSCFG_SKD_SWT_SMA)
#  include <xwos/mm/sma.h>
#elif defined(XWOSCFG_SKD_SWT_STDC_MM) && (1 == XWOSCFG_SKD_SWT_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/swt.h>

/* #define XWOS_SWTLOGF */ /**< 调试日志开关 */
#ifdef XWOS_SWTLOGF
#  define xwos_swtlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_swtlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_SKD_SWT_MEMPOOL) && (1 == XWOSCFG_SKD_SWT_MEMPOOL)
/**
 * @brief 软件定时器对象缓存
 */
static __xwmp_data struct xwmm_mempool_objcache xwmp_swt_cache;

/**
 * @brief 软件定时器对象缓存的名字
 */
const __xwmp_rodata char xwmp_swt_cache_name[] = "xwmp.swt.cache";
#elif defined(XWOSCFG_SKD_SWT_MEMSLICE) && (1 == XWOSCFG_SKD_SWT_MEMSLICE)
/**
 * @brief 软件定时器对象缓存
 */
static __xwmp_data struct xwmm_memslice xwmp_swt_cache;

/**
 * @brief 软件定时器对象缓存的名字
 */
const __xwmp_rodata char xwmp_swt_cache_name[] = "xwmp.swt.cache";
#elif defined(XWOSCFG_SKD_SWT_SMA) && (1 == XWOSCFG_SKD_SWT_SMA)
static __xwmp_data struct xwmm_sma * xwmp_swt_cache;
#endif

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
static __xwmp_code
struct xwmp_swt * xwmp_swt_alloc(void);

static __xwmp_code
void xwmp_swt_free(struct xwmp_swt * swt);
#endif

static __xwmp_code
void xwmp_swt_construct(struct xwmp_swt * swt);

static __xwmp_code
void xwmp_swt_destruct(struct xwmp_swt * swt);

static __xwmp_code
xwer_t xwmp_swt_activate(struct xwmp_swt * swt, xwsq_t flag, xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_swt_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
static __xwmp_code
xwer_t xwmp_swt_dgc(struct xwos_object * obj);
#endif

static __xwmp_code
void xwmp_swt_ttn_callback(struct xwmp_ttn * ttn);


#if defined(XWOSCFG_SKD_SWT_MEMPOOL) && (1 == XWOSCFG_SKD_SWT_MEMPOOL)
/**
 * @brief XWMP INIT CODE：初始化软件定时器对象缓存
 * @param[in] zone_origin: 内存区域首地址
 * @param[in] zone_size: 内存区域大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_swt_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwmp_swt_cache,
                                        &mp->pa,
                                        xwmp_swt_cache_name,
                                        sizeof(struct xwmp_swt),
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwmp_swt_construct,
                                        (dtor_f)xwmp_swt_destruct);
        return rc;
}
#elif defined(XWOSCFG_SKD_SWT_MEMSLICE) && (1 == XWOSCFG_SKD_SWT_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化软件定时器对象缓存
 * @param[in] zone_origin: 内存区域首地址
 * @param[in] zone_size: 内存区域大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_swt_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwmp_swt_cache, zone_origin, zone_size,
                                sizeof(struct xwmp_swt),
                                xwmp_swt_cache_name,
                                (ctor_f)xwmp_swt_construct,
                                (dtor_f)xwmp_swt_destruct);
        return rc;
}
#elif defined(XWOSCFG_SKD_SWT_SMA) && (1 == XWOSCFG_SKD_SWT_SMA)
/**
 * @brief XWMP INIT CODE：初始化 `struct xwmp_swt` 的对象缓存
 * @param[in] sma: 简单内存分配器
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
void xwmp_swt_cache_init(struct xwmm_sma * sma)
{
        xwmp_swt_cache = sma;
}
#endif

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
/**
 * @brief 从软件定时器对象缓存中申请一个对象
 * @return 软件定时器对象的指针
 */
static __xwmp_code
struct xwmp_swt * xwmp_swt_alloc(void)
{
#  if defined(XWOSCFG_SKD_SWT_MEMPOOL) && (1 == XWOSCFG_SKD_SWT_MEMPOOL)
        union {
                struct xwmp_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_objcache_alloc(&xwmp_swt_cache, &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(rc);
        }
        return mem.swt;
#  elif defined(XWOSCFG_SKD_SWT_MEMSLICE) && (1 == XWOSCFG_SKD_SWT_MEMSLICE)
        union {
                struct xwmp_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwmp_swt_cache, &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(rc);
        }
        return mem.swt;
#  elif defined(XWOSCFG_SKD_SWT_SMA) && (1 == XWOSCFG_SKD_SWT_SMA)
        union {
                struct xwmp_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwmp_swt_cache,
                            sizeof(struct xwmp_swt), XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(rc);
        } else {
                xwmp_swt_construct(mem.swt);
        }
        return mem.swt;
#  elif defined(XWOSCFG_SKD_SWT_STDC_MM) && (1 == XWOSCFG_SKD_SWT_STDC_MM)
        struct xwmp_swt * swt;

        swt = malloc(sizeof(struct xwmp_swt));
        if (NULL == swt) {
                swt = err_ptr(-ENOMEM);
        } else {
                xwmp_swt_construct(swt);
        }
        return swt;
#  else
        return err_ptr(-ENOSYS);
#  endif
}
#endif

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
/**
 * @brief 释放软件定时器对象
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_free(struct xwmp_swt * swt)
{
#  if defined(XWOSCFG_SKD_SWT_MEMPOOL) && (1 == XWOSCFG_SKD_SWT_MEMPOOL)
        xwmm_mempool_objcache_free(&xwmp_swt_cache, swt);
#  elif defined(XWOSCFG_SKD_SWT_MEMSLICE) && (1 == XWOSCFG_SKD_SWT_MEMSLICE)
        xwmm_memslice_free(&xwmp_swt_cache, swt);
#  elif defined(XWOSCFG_SKD_SWT_SMA) && (1 == XWOSCFG_SKD_SWT_SMA)
        xwmm_sma_free(xwmp_swt_cache, swt);
#  elif defined(XWOSCFG_SKD_SWT_STDC_MM) && (1 == XWOSCFG_SKD_SWT_STDC_MM)
        xwmp_swt_destruct(swt);
        free(swt);
#  else
        XWOS_UNUSED(swt);
#  endif
}
#endif

/**
 * @brief 软件定时器对象的构造函数
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_construct(struct xwmp_swt * swt)
{
        xwos_object_construct(&swt->xwobj);
        swt->xwskd = NULL;
        swt->flag = (xwsq_t)0;
        xwmp_ttn_init(&swt->ttn);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = (xwtm_t)0;
}

/**
 * @brief 软件定时器对象的析构函数
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_destruct(struct xwmp_swt * swt)
{
        xwos_object_destruct(&swt->xwobj);
}

/**
 * @brief 静态软件定时器对象的垃圾回收函数
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
xwer_t xwmp_swt_sgc(struct xwos_object * obj)
{
        struct xwmp_swt * swt;

        swt = xwcc_derof(obj, struct xwmp_swt, xwobj);
        xwmp_swt_destruct(swt);
        return XWOK;
}

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
/**
 * @brief 动态软件定时器对象的垃圾回收函数
 * @param[in] swt: 软件定时器对象的指针
 */
static __xwmp_code
xwer_t xwmp_swt_dgc(struct xwos_object * obj)
{
        struct xwmp_swt * swt;

        swt = xwcc_derof(obj, struct xwmp_swt, xwobj);
        xwmp_swt_free(swt);
        return XWOK;
}
#endif

/**
 * @brief 激活软件定时器对象
 * @param[in] swt: 软件定时器对象的指针
 * @param[in] flag: 标志
 * @param[in] gcfunc: 垃圾回收函数：当对象应用计数为0，调用此函数回收资源
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_swt_activate(struct xwmp_swt * swt,
                         xwsq_t flag,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&swt->xwobj, gcfunc);
        if (rc < 0) {
                goto err_xwobj_activate;
        }
        swt->xwskd = xwmp_skd_get_lc();
        swt->flag = flag;
        return XWOK;

err_xwobj_activate:
        return rc;
}

__xwmp_api
xwer_t xwmp_swt_init(struct xwmp_swt * swt, xwsq_t flag)
{
        xwmp_swt_construct(swt);
        return xwmp_swt_activate(swt, flag, xwmp_swt_sgc);
}

__xwmp_api
xwer_t xwmp_swt_fini(struct xwmp_swt * swt)
{
        return xwmp_swt_put(swt);
}

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
__xwmp_api
xwer_t xwmp_swt_create(struct xwmp_swt ** swtbuf, xwsq_t flag)
{
        struct xwmp_swt * swt;
        xwer_t rc;

        *swtbuf = NULL;
        swt = xwmp_swt_alloc();
        if (is_err(swt)) {
                rc = ptr_err(swt);
                goto err_swt_alloc;
        }
        rc = xwmp_swt_activate(swt, flag, xwmp_swt_dgc);
        if (rc < 0) {
                goto err_swt_activate;
        }
        *swtbuf = swt;
        return XWOK;

err_swt_activate:
        xwmp_swt_free(swt);
err_swt_alloc:
        return rc;
}
#endif

#if (1 == XWOSRULE_SKD_SWT_CREATE_DELETE)
__xwmp_api
xwer_t xwmp_swt_delete(struct xwmp_swt * swt, xwsq_t tik)
{
        return xwmp_swt_release(swt, tik);
}
#endif

__xwmp_api
xwer_t xwmp_swt_acquire(struct xwmp_swt * swt, xwsq_t tik)
{
        return xwos_object_acquire(&swt->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_swt_release(struct xwmp_swt * swt, xwsq_t tik)
{
        return xwos_object_release(&swt->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_swt_grab(struct xwmp_swt * swt)
{
        return xwos_object_grab(&swt->xwobj);
}

__xwmp_api
xwer_t xwmp_swt_put(struct xwmp_swt * swt)
{
        return xwos_object_put(&swt->xwobj);
}

/**
 * @brief 软件定时器的时间树节点回调函数
 * @param[in] entry: 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_ttn_callback(struct xwmp_ttn * ttn)
{
        struct xwmp_swt * swt;
        struct xwmp_tt * xwtt;
        xwsq_t refcnt;
        xwtm_t to;
        xwreg_t cpuirq;

        swt = xwcc_derof(ttn, struct xwmp_swt, ttn);
        xwtt = &swt->xwskd->tt;
        swt->cb(swt, swt->arg);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SWT_FLAG_RESTART & swt->flag)) {
                to = xwtm_add_safely(swt->ttn.wkup_xwtm, swt->period);
                /* 其他CPU在此时调用 `xwmp_swt_stop()` 会导致引用计数的值小于3。*/
                xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
                refcnt = xwos_object_get_refcnt(&swt->xwobj);
                if (refcnt >= (xwsq_t)3) {
                        swt->ttn.wkup_xwtm = to;
                        xwaop_write(xwsq_t, &swt->ttn.wkuprs,
                                    (xwsq_t)XWMP_TTN_WKUPRS_UNKNOWN, NULL);
                        swt->ttn.cb = xwmp_swt_ttn_callback;
                        /* 在本地CPU执行 `xwmp_tt_add_locked()` 的过程中，
                           其他CPU可能会执行 `xwmp_swt_stop() + xwmp_swt_delete()` ，
                           导致 `swt` 成为野指针。
                           因此，需要在 `xwmp_tt_add_locked()` 之前增加对 `swt` 的引用。
                           + 如果 **本地CPU** 的 `xwmp_tt_add_locked()` 在
                             **其他CPU** 的 `xwmp_swt_stop()` 之后执行成功，
                             软件定时器将在下一次进入回调函数后彻底停止。
                           + 如果 **本地CPU** 的 `xwmp_tt_add_locked()` 执行失败，
                             软件定时器立即停止。
                           + 如果 **本地CPU** 的 `xwmp_tt_add_locked()` 执行成功之后，
                             **其他CPU** `xwmp_swt_stop()` 也执行成功，
                             软件定时器不会再进入回调函数。 */
                        xwmp_swt_grab(swt); // cppcheck-suppress [misra-c2012-17.7]
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_tt_add_locked(xwtt, &swt->ttn, cpuirq);
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwmp_swt_put(swt); // cppcheck-suppress [misra-c2012-17.7]
                } else {
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwmp_swt_put(swt); // cppcheck-suppress [misra-c2012-17.7]
                }
        } else {
                /* 其他CPU在此时调用 `xwmp_swt_stop()` 会失败，
                   `xwmp_swt_stop()` 中减少引用计数的代码不会执行。 */
                xwmp_swt_put(swt); // cppcheck-suppress [misra-c2012-17.7]
        }
}

__xwmp_api
xwer_t xwmp_swt_start(struct xwmp_swt * swt,
                      xwtm_t origin, xwtm_t period,
                      xwmp_swt_f cb, void * arg)
{
        struct xwmp_tt * xwtt;
        xwtm_t to;
        xwer_t rc;
        xwreg_t cpuirq;

        rc = xwmp_swt_grab(swt);
        if (rc < 0) {
                goto err_swt_grab;
        }
        xwtt = &swt->xwskd->tt;
        to = xwtm_add_safely(origin, period);
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        if (NULL != swt->ttn.cb) {
                rc = -EALREADY;
                goto err_already;
        }
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SWT_FLAG_RESTART & swt->flag)) {
                /* 多增加一次引用计数作为继续重启定时器的依据。 */
                xwmp_swt_grab(swt); // cppcheck-suppress [misra-c2012-17.7]
        }
        swt->cb = cb;
        swt->arg = arg;
        swt->period = period;
        swt->ttn.wkup_xwtm = to;
        xwaop_write(xwsq_t, &swt->ttn.wkuprs, (xwsq_t)XWMP_TTN_WKUPRS_UNKNOWN, NULL);
        swt->ttn.xwtt = xwtt;
        swt->ttn.cb = xwmp_swt_ttn_callback;
        rc = xwmp_tt_add_locked(xwtt, &swt->ttn, cpuirq);
        if (rc < 0) {
                goto err_swt_add;
        }
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return XWOK;

err_swt_add:
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SWT_FLAG_RESTART & swt->flag)) {
                xwmp_swt_put(swt); // cppcheck-suppress [misra-c2012-17.7]
        }
err_already:
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        xwmp_swt_put(swt); // cppcheck-suppress [misra-c2012-17.7]
err_swt_grab:
        return rc;
}

__xwmp_api
xwer_t xwmp_swt_stop(struct xwmp_swt * swt)
{
        xwreg_t cpuirq;
        xwer_t rc;
        struct xwmp_tt * xwtt;

        xwtt = &swt->xwskd->tt;
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwmp_tt_remove_locked(xwtt, &swt->ttn);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SWT_FLAG_RESTART & swt->flag)) {
                xwmp_swt_put(swt); // cppcheck-suppress [misra-c2012-17.7]
        }
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        if (XWOK == rc) {
                /* 从时间树上删除成功，回调函数不可能再被执行。
                   回调函数中减少引用计数的代码也不会执行 */
                xwmp_swt_put(swt); // cppcheck-suppress [misra-c2012-17.7]
        }
        return rc;
}
