/**
 * @file
 * @brief XWOS UP内核同步机制：事件
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
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
#  include <xwos/mm/sma.h>
#elif defined(XWOSCFG_SYNC_EVT_STDC_MM) && (1 == XWOSCFG_SYNC_EVT_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/up/thd.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/sync/cond.h>
#include <xwos/up/sync/obj.h>
#include <xwos/up/sync/evt.h>

/* #define XWOS_EVTLOGF */ /**< 调试日志开关 */
#ifdef XWOS_EVTLOGF
#  define xwos_evtlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_evtlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
/**
 * @brief 结构体 `xwup_evt` 的对象缓存
 */
static __xwup_data struct xwmm_mempool_objcache xwup_evt_cache;

/**
 * @brief 结构体 `xwup_evt` 的对象缓存的名字
 */
const __xwup_rodata char xwup_evt_cache_name[] = "xwup.sync.evt.cache";
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
/**
 * @brief 结构体 `xwup_evt` 的对象缓存
 */
static __xwup_data struct xwmm_memslice xwup_evt_cache;

/**
 * @brief 结构体 `xwup_evt` 的对象缓存的名字
 */
const __xwup_rodata char xwup_evt_cache_name[] = "xwup.sync.evt.cache";
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
static __xwup_data struct xwmm_sma * xwup_evt_cache;
#endif

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
static __xwup_code
struct xwup_evt * xwup_evt_alloc(xwsz_t num);

static __xwup_code
void xwup_evt_free(struct xwup_evt * evt);
#endif

static __xwup_code
void xwup_evt_construct(struct xwup_evt * evt);

static __xwup_code
void xwup_evt_setup(struct xwup_evt * evt, xwsz_t num, xwbmp_t * bmp, xwbmp_t * msk);

static __xwup_code
void xwup_evt_destruct(struct xwup_evt * evt);

static __xwup_code
xwer_t xwup_evt_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
static __xwup_code
xwer_t xwup_evt_dgc(struct xwos_object * obj);
#endif

static __xwup_code
xwer_t xwup_evt_activate(struct xwup_evt * evt, xwsq_t type, xwobj_gc_f gcfunc);

static __xwup_code
xwer_t xwup_flg_wait_level(struct xwup_evt * evt,
                           xwsq_t trigger, bool consumption,
                           xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_flg_wait_edge(struct xwup_evt * evt, xwsq_t trigger,
                          xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_flg_wait_to_level(struct xwup_evt * evt,
                              xwsq_t trigger, bool consumption,
                              xwbmp_t origin[], xwbmp_t msk[],
                              xwtm_t to);

static __xwup_code
xwer_t xwup_flg_wait_to_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[],
                             xwtm_t to);

static __xwup_code
xwer_t xwup_flg_trywait_level(struct xwup_evt * evt,
                              xwsq_t trigger, bool consumption,
                              xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_flg_trywait_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_flg_wait_unintr_level(struct xwup_evt * evt,
                                  xwsq_t trigger, bool consumption,
                                  xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_flg_wait_unintr_edge(struct xwup_evt * evt, xwsq_t trigger,
                                 xwbmp_t origin[], xwbmp_t msk[]);

#if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_evt` 的对象缓存
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
xwer_t xwup_evt_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwup_evt_cache,
                                        &mp->pa,
                                        xwup_evt_cache_name,
                                        sizeof(struct xwup_evt) * 3U,
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwup_evt_construct,
                                        (dtor_f)xwup_evt_destruct);
        return rc;
}
#elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
/**
 * @brief XWUP INIT CODE：初始化结构体 `xwup_evt` 的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwup_api
xwer_t xwup_evt_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwup_evt_cache, zone_origin, zone_size,
                                sizeof(struct xwup_evt) * 3U,
                                xwup_evt_cache_name,
                                (ctor_f)xwup_evt_construct,
                                (dtor_f)xwup_evt_destruct);
        return rc;
}
#elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_evt` 的对象缓存
 * @param[in] sma: 简单内存分配器
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwup_init_code
void xwup_evt_cache_init(struct xwmm_sma * sma)
{
        xwup_evt_cache = sma;
}
#endif

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
/**
 * @brief 从事件对象缓存中申请对象
 * @return 事件对象的指针
 */
static __xwup_code
struct xwup_evt * xwup_evt_alloc(xwsz_t num)
{
#  if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
        union {
                struct xwup_evt * evt;
                void * anon;
        } mem;
        xwbmp_t * bmp;
        xwbmp_t * msk;
        xwsz_t bmpnum;
        xwsz_t bmpsize;
        xwer_t rc;

        bmpnum = BITS_TO_XWBMP_T(num);
        bmpsize = bmpnum * sizeof(xwbmp_t);
        if (bmpsize > sizeof(struct xwup_evt)) {
                mem.evt = err_ptr(-ENOMEM);
        } else {
                rc = xwmm_mempool_objcache_alloc(&xwup_evt_cache, &mem.anon);
                if (rc < 0) {
                        mem.evt = err_ptr(rc);
                } else {
                        bmp = (void *)&mem.evt[1];
                        msk = (void *)&mem.evt[2];
                        xwup_evt_construct(mem.evt);
                        xwup_evt_setup(mem.evt, num, bmp, msk);
                }
        }
        return mem.evt;
#  elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
        union {
                struct xwup_evt * evt;
                void * anon;
        } mem;
        xwbmp_t * bmp;
        xwbmp_t * msk;
        xwsz_t bmpnum;
        xwsz_t bmpsize;
        xwer_t rc;

        bmpnum = BITS_TO_XWBMP_T(num);
        bmpsize = bmpnum * sizeof(xwbmp_t);
        if (bmpsize > sizeof(struct xwup_evt)) {
                mem.evt = err_ptr(-ENOMEM);
        } else {
                rc = xwmm_memslice_alloc(&xwup_evt_cache, &mem.anon);
                if (rc < 0) {
                        mem.evt = err_ptr(rc);
                } else {
                        bmp = (void *)&mem.evt[1];
                        msk = (void *)&mem.evt[2];
                        xwup_evt_construct(mem.evt);
                        xwup_evt_setup(mem.evt, num, bmp, msk);
                }
        }
        return mem.evt;
#  elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
        union {
                struct xwup_evt * evt;
                void * anon;
        } mem;
        xwbmp_t * bmp;
        xwbmp_t * msk;
        xwsz_t bmpnum;
        xwsz_t bmpsize;
        xwer_t rc;

        bmpnum = BITS_TO_XWBMP_T(num);
        bmpsize = bmpnum * sizeof(xwbmp_t);
        rc = xwmm_sma_alloc(xwup_evt_cache,
                            sizeof(struct xwup_evt) + bmpsize + bmpsize,
                            XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.evt = err_ptr(rc);
        } else {
                bmp = (void *)&mem.evt[1];
                msk = &bmp[bmpnum];
                xwup_evt_construct(mem.evt);
                xwup_evt_setup(mem.evt, num, bmp, msk);
        }
        return mem.evt;
#  elif defined(XWOSCFG_SYNC_EVT_STDC_MM) && (1 == XWOSCFG_SYNC_EVT_STDC_MM)
        struct xwup_evt * evt;
        xwbmp_t * bmp;
        xwbmp_t * msk;
        xwsz_t bmpnum;
        xwsz_t bmpsize;

        bmpnum = BITS_TO_XWBMP_T(num);
        bmpsize = bmpnum * sizeof(xwbmp_t);
        evt = malloc(sizeof(struct xwup_evt) + bmpsize + bmpsize);
        if (NULL == evt) {
                evt = err_ptr(-ENOMEM);
        } else {
                bmp = (void *)&evt[1];
                msk = &bmp[bmpnum];
                xwup_evt_construct(evt);
                xwup_evt_setup(evt, num, bmp, msk);
        }
        return evt;
#  else
        return -ENOSYS;
#  endif
}
#endif

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
/**
 * @brief 释放事件对象
 * @param[in] evt: 事件对象的指针
 */
static __xwup_code
void xwup_evt_free(struct xwup_evt * evt)
{
#  if defined(XWOSCFG_SYNC_EVT_MEMPOOL) && (1 == XWOSCFG_SYNC_EVT_MEMPOOL)
        xwmm_mempool_objcache_free(&xwup_evt_cache, evt);
#  elif defined(XWOSCFG_SYNC_EVT_MEMSLICE) && (1 == XWOSCFG_SYNC_EVT_MEMSLICE)
        xwmm_memslice_free(&xwup_evt_cache, evt);
#  elif defined(XWOSCFG_SYNC_EVT_SMA) && (1 == XWOSCFG_SYNC_EVT_SMA)
        xwmm_sma_free(xwup_evt_cache, evt);
#  elif defined(XWOSCFG_SYNC_EVT_STDC_MM) && (1 == XWOSCFG_SYNC_EVT_STDC_MM)
        xwup_evt_destruct(evt);
        free(evt);
#  else
        XWOS_UNUSED(evt);
#  endif
}
#endif

/**
 * @brief 事件对象的构造函数
 * @param[in] evt: 事件对象的指针
 */
static __xwup_code
void xwup_evt_construct(struct xwup_evt * evt)
{
        xwup_cond_construct(&evt->cond);
}

/**
 * @brief 为事件对象安装位图数组
 * @param[in] evt: 事件对象的指针
 * @param[in] num: 事件的数量
 * @param[in] bmp: 事件对象记录事件状态的位图数组缓冲区
 * @param[in] msk: 事件对象记录掩码状态的位图数组缓冲区
 */
static __xwup_code
void xwup_evt_setup(struct xwup_evt * evt, xwsz_t num,
                    xwbmp_t * bmp, xwbmp_t * msk)
{
        evt->num = num;
        evt->bmp = bmp;
        evt->msk = msk;
}

/**
 * @brief 事件对象的析构函数
 * @param[in] evt: 事件对象的指针
 */
static __xwup_code
void xwup_evt_destruct(struct xwup_evt * evt)
{
        xwup_cond_destruct(&evt->cond);
}

/**
 * @brief 静态事件对象的垃圾回收函数
 * @param[in] evt: 事件对象的指针
 */
static __xwup_code
xwer_t xwup_evt_sgc(struct xwos_object * obj)
{
        struct xwup_evt * evt;

        evt = xwcc_derof(obj, struct xwup_evt, cond.synobj.xwobj);
        xwup_evt_destruct(evt);
        return XWOK;
}

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
/**
 * @brief 动态事件对象的垃圾回收函数
 * @param[in] evt: 事件对象的指针
 */
static __xwup_code
xwer_t xwup_evt_dgc(struct xwos_object * obj)
{
        struct xwup_evt * evt;

        evt = xwcc_derof(obj, struct xwup_evt, cond.synobj.xwobj);
        xwup_evt_free(evt);
        return XWOK;
}
#endif

/**
 * @brief 激活并初始化事件对象
 * @param[in] evt: 事件对象的指针
 * @param[in] type: 事件的类型，取值：
 *   @arg XWUP_EVT_TYPE_FLG: 事件标志
 *   @arg XWUP_EVT_TYPE_SEL: 信号选择器
 *   @arg XWUP_EVT_TYPE_BR: 线程栅栏
 * @param[in] gcfunc: 垃圾回收函数的指针
 */
static __xwup_code
xwer_t xwup_evt_activate(struct xwup_evt * evt, xwsq_t type, xwobj_gc_f gcfunc)
{
        xwsz_t size;
        xwer_t rc;
        xwsq_t i;

        size = BITS_TO_XWBMP_T(evt->num);
        rc = xwup_cond_activate(&evt->cond, gcfunc);
        if (rc < 0) {
                goto err_cond_activate;
        }
        evt->type = type;
        switch (type & (xwsq_t)XWUP_EVT_TYPE_MASK) {
        case XWUP_EVT_TYPE_BR:
                memset(evt->msk, 0, size); // cppcheck-suppress [misra-c2012-17.7]
                for (i = 0; i < evt->num; i++) {
                        xwbmpop_s1i(evt->msk, i);
                }
                break;
        case XWUP_EVT_TYPE_SEL:
                memset(evt->msk, 0, size); // cppcheck-suppress [misra-c2012-17.7]
                break;
        case XWUP_EVT_TYPE_FLG:
        default:
                memset(evt->msk, 0xFF, size); // cppcheck-suppress [misra-c2012-17.7]
                break;
        }
        memset(evt->bmp, 0, size); // cppcheck-suppress [misra-c2012-17.7]
        xwup_splk_init(&evt->lock);
        return XWOK;

err_cond_activate:
        return rc;
}

__xwup_api
xwer_t xwup_evt_init(struct xwup_evt * evt, xwsq_t type, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk)
{
        xwup_evt_construct(evt);
        xwup_evt_setup(evt, num, bmp, msk);
        return xwup_evt_activate(evt, type, xwup_evt_sgc);
}

__xwup_api
xwer_t xwup_evt_fini(struct xwup_evt * evt)
{
        return xwup_evt_put(evt);
}

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
__xwup_api
xwer_t xwup_evt_create(struct xwup_evt ** ptrbuf, xwsq_t type, xwsz_t num)
{
        struct xwup_evt * evt;
        xwer_t rc;

        *ptrbuf = NULL;
        evt = xwup_evt_alloc(num);
        if (is_err(evt)) {
                rc = ptr_err(evt);
        } else {
                rc = xwup_evt_activate(evt, type, xwup_evt_dgc);
                if (rc < 0) {
                        xwup_evt_free(evt);
                } else {
                        *ptrbuf = evt;
                }
        }
        return rc;
}
#endif

#if (1 == XWOSRULE_SYNC_EVT_CREATE_DELETE)
__xwup_api
xwer_t xwup_evt_delete(struct xwup_evt * evt, xwsq_t tik)
{
        return xwup_evt_release(evt, tik);
}
#endif

__xwup_api
xwer_t xwup_evt_acquire(struct xwup_evt * evt, xwsq_t tik)
{
        return xwup_cond_acquire(&evt->cond, tik);
}

__xwup_api
xwer_t xwup_evt_release(struct xwup_evt * evt, xwsq_t tik)
{
        return xwup_cond_release(&evt->cond, tik);
}

__xwup_api
xwer_t xwup_evt_grab(struct xwup_evt * evt)
{
        return xwup_cond_grab(&evt->cond);
}

__xwup_api
xwer_t xwup_evt_put(struct xwup_evt * evt)
{
        return xwup_cond_put(&evt->cond);
}

__xwup_api
xwer_t xwup_evt_bind(struct xwup_evt * evt, struct xwup_evt * sel, xwsq_t pos)
{
        return xwup_cond_bind(&evt->cond, sel, pos);
}

__xwup_api
xwer_t xwup_evt_unbind(struct xwup_evt * evt, struct xwup_evt * sel)
{
        return xwup_cond_unbind(&evt->cond, sel);
}

__xwup_code
xwer_t xwup_evt_intr_all(struct xwup_evt * evt)
{
        return xwup_cond_intr_all(&evt->cond);
}

__xwup_api
xwer_t xwup_evt_get_num(struct xwup_evt * evt, xwsz_t * numbuf)
{
        *numbuf = evt->num;
        return XWOK;
}

/******** type:XWUP_EVT_TYPE_FLG ********/
__xwup_api
xwer_t xwup_flg_s1m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1m(evt->bmp, msk, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return xwup_cond_broadcast(&evt->cond);
}

__xwup_api
xwer_t xwup_flg_s1i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return xwup_cond_broadcast(&evt->cond);

err_pos_range:
        return rc;
}

__xwup_api
xwer_t xwup_flg_c0m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0m(evt->bmp, msk, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return xwup_cond_broadcast(&evt->cond);
}

__xwup_api
xwer_t xwup_flg_c0i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return xwup_cond_broadcast(&evt->cond);

err_pos_range:
        return rc;
}

__xwup_api
xwer_t xwup_flg_x1m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1m(evt->bmp, msk, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return xwup_cond_broadcast(&evt->cond);
}

__xwup_api
xwer_t xwup_flg_x1i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return xwup_cond_broadcast(&evt->cond);

err_pos_range:
        return rc;
}

__xwup_api
xwer_t xwup_flg_read(struct xwup_evt * evt, xwbmp_t out[])
{
        xwreg_t cpuirq;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(out, evt->bmp, evt->num);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;
}

static __xwup_code
xwer_t xwup_flg_wait_level(struct xwup_evt * evt,
                           xwsq_t trigger, bool consumption,
                           xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                if (NULL != origin) {
                        xwbmpop_assign(origin, evt->bmp, evt->num);
                }
                if (consumption) {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_wait(&evt->cond,
                                            &evt->lock, XWOS_LK_SPLK, NULL,
                                            &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_wait_edge(struct xwup_evt * evt, xwsq_t trigger,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc = XWOK;
        xwbmpop_define(cur, evt->num);
        xwbmpop_define(tmp, evt->num);

        xwbmpop_and(origin, msk, evt->num);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                xwbmpop_assign(cur, evt->bmp, evt->num);
                xwbmpop_and(cur, msk, evt->num);
                if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, evt->num);
                        xwbmpop_xor(tmp, origin, evt->num);
                        cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                        if ((xwssq_t)0 == cmprc) {
                                triggered = true;
                                rc = XWOK;
                        } else {
                                triggered = false;
                        }
                } else if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, evt->num);
                        if ((xwssq_t)0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = XWOK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_wait(&evt->cond,
                                            &evt->lock, XWOS_LK_SPLK, NULL,
                                            &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_flg_wait(struct xwup_evt * evt,
                     xwsq_t trigger, bool consumption,
                     xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        if (trigger <= (xwsq_t)XWUP_FLG_TRIGGER_CLR_ANY) {
                rc = xwup_flg_wait_level(evt, trigger, consumption, origin, msk);
        } else {
                rc = xwup_flg_wait_edge(evt, trigger, origin, msk);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_wait_to_level(struct xwup_evt * evt,
                              xwsq_t trigger, bool consumption,
                              xwbmp_t origin[], xwbmp_t msk[],
                              xwtm_t to)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                if (NULL != origin) {
                        xwbmpop_assign(origin, evt->bmp, evt->num);
                }
                if (consumption) {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_wait_to(&evt->cond,
                                               &evt->lock, XWOS_LK_SPLK, NULL,
                                               to, &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_wait_to_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[],
                             xwtm_t to)
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc = XWOK;
        xwbmpop_define(cur, evt->num);
        xwbmpop_define(tmp, evt->num);

        xwbmpop_and(origin, msk, evt->num);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                xwbmpop_assign(cur, evt->bmp, evt->num);
                xwbmpop_and(cur, msk, evt->num);
                if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, evt->num);
                        xwbmpop_xor(tmp, origin, evt->num);
                        cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                        if ((xwssq_t)0 == cmprc) {
                                triggered = true;
                                rc = XWOK;
                        } else {
                                triggered = false;
                        }
                } else if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, evt->num);
                        if ((xwssq_t)0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = XWOK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_wait_to(&evt->cond,
                                               &evt->lock, XWOS_LK_SPLK, NULL,
                                               to, &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_flg_wait_to(struct xwup_evt * evt,
                        xwsq_t trigger, bool consumption,
                        xwbmp_t origin[], xwbmp_t msk[],
                        xwtm_t to)
{
        xwer_t rc;

        if (trigger <= (xwsq_t)XWUP_FLG_TRIGGER_CLR_ANY) {
                rc = xwup_flg_wait_to_level(evt, trigger, consumption, origin, msk, to);
        } else {
                rc = xwup_flg_wait_to_edge(evt, trigger, origin, msk, to);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_trywait_level(struct xwup_evt * evt,
                              xwsq_t trigger, bool consumption,
                              xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwer_t rc;

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        if (NULL != origin) {
                xwbmpop_assign(origin, evt->bmp, evt->num);
        }
        if (consumption) {
                switch (trigger) {
                case XWUP_FLG_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk, evt->num);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        } else {
                switch (trigger) {
                case XWUP_FLG_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                        break;
                case XWUP_FLG_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        }
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        if (!triggered) {
                rc = -ENODATA;
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_trywait_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwbmpop_define(cur, evt->num);
        xwbmpop_define(tmp, evt->num);

        xwbmpop_and(origin, msk, evt->num);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(cur, evt->bmp, evt->num);
        xwbmpop_and(cur, msk, evt->num);
        if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ALL == trigger) {
                xwbmpop_assign(tmp, cur, evt->num);
                xwbmpop_xor(tmp, origin, evt->num);
                cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                if ((xwssq_t)0 == cmprc) {
                        triggered = true;
                        rc = XWOK;
                } else {
                        triggered = false;
                        rc = -ENODATA;
                }
        } else if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ANY == trigger) {
                cmprc = xwbmpop_cmp(origin, cur, evt->num);
                if ((xwssq_t)0 == cmprc) {
                        triggered = false;
                        rc = -ENODATA;
                } else {
                        triggered = true;
                        rc = XWOK;
                }
        } else {
                triggered = true;
                rc = -EINVAL;
        }
        if (triggered) {
                xwbmpop_assign(origin, cur, evt->num);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        } else {
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

__xwup_api
xwer_t xwup_flg_trywait(struct xwup_evt * evt,
                        xwsq_t trigger, bool consumption,
                        xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        if (trigger <= (xwsq_t)XWUP_FLG_TRIGGER_CLR_ANY) {
                rc = xwup_flg_trywait_level(evt, trigger, consumption, origin, msk);
        } else {
                rc = xwup_flg_trywait_edge(evt, trigger, origin, msk);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_wait_unintr_level(struct xwup_evt * evt,
                                  xwsq_t trigger, bool consumption,
                                  xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                if (NULL != origin) {
                        xwbmpop_assign(origin, evt->bmp, evt->num);
                }
                if (consumption) {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWUP_FLG_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk, evt->num);
                                break;
                        case XWUP_FLG_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk, evt->num);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_wait_unintr(&evt->cond,
                                                   &evt->lock, XWOS_LK_SPLK, NULL,
                                                   &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static __xwup_code
xwer_t xwup_flg_wait_unintr_edge(struct xwup_evt * evt, xwsq_t trigger,
                                 xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc = XWOK;
        xwbmpop_define(cur, evt->num);
        xwbmpop_define(tmp, evt->num);

        xwbmpop_and(origin, msk, evt->num);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                xwbmpop_assign(cur, evt->bmp, evt->num);
                xwbmpop_and(cur, msk, evt->num);
                if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, evt->num);
                        xwbmpop_xor(tmp, origin, evt->num);
                        cmprc = xwbmpop_cmp(tmp, msk, evt->num);
                        if ((xwssq_t)0 == cmprc) {
                                triggered = true;
                                rc = XWOK;
                        } else {
                                triggered = false;
                        }
                } else if ((xwsq_t)XWUP_FLG_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, evt->num);
                        if ((xwssq_t)0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = XWOK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_wait_unintr(&evt->cond,
                                                   &evt->lock, XWOS_LK_SPLK, NULL,
                                                   &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_flg_wait_unintr(struct xwup_evt * evt,
                            xwsq_t trigger, bool consumption,
                            xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        if (trigger <= (xwsq_t)XWUP_FLG_TRIGGER_CLR_ANY) {
                rc = xwup_flg_wait_unintr_level(evt, trigger, consumption, origin, msk);
        } else {
                rc = xwup_flg_wait_unintr_edge(evt, trigger, origin, msk);
        }
        return rc;
}

/******** type:XWUP_EVT_TYPE_SEL ********/
/**
 * @brief 绑定同步对象到事件对象，事件对象类型为 `XWUP_EVT_TYPE_SEL`
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @param[in] pos: 同步对象映射到位图中的位置
 * @param[in] exclusive: 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 */
__xwup_code
xwer_t xwup_sel_obj_bind(struct xwup_evt * evt,
                         struct xwup_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        bool existed;
        xwer_t rc;

        if (pos >= evt->num) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (NULL != owner) {
                rc = -EALREADY;
                goto err_already;
        }
        existed = xwbmpop_t1i(evt->msk, pos);
        if (existed) {
                rc = -EBUSY;
                goto err_busy;
        }
        if (exclusive) {
                xwbmpop_s1i(evt->msk, pos);
        }
        synobj->sel.evt = evt;
        synobj->sel.pos = pos;
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_busy:
err_already:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
err_pos_range:
        return rc;
}

/**
 * @brief 从事件对象上解绑同步对象，事件对象类型为 `XWUP_EVT_TYPE_SEL`
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @param[in] exclusive: 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_sel_obj_unbind(struct xwup_evt * evt,
                           struct xwup_synobj * synobj,
                           bool exclusive)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        if (exclusive) {
                xwbmpop_c0i(evt->msk, synobj->sel.pos);
        }
        synobj->sel.evt = NULL;
        synobj->sel.pos = 0;
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief 在事件对象上设置同步对象的标志位，事件对象类型为 `XWUP_EVT_TYPE_SEL`
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_sel_obj_s1i(struct xwup_evt * evt, struct xwup_synobj * synobj)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_s1i(evt->bmp, synobj->sel.pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond); // cppcheck-suppress [misra-c2012-17.7]
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief 在事件对象上清除同步对象的标志位，事件对象类型为 `XWUP_EVT_TYPE_SEL`
 * @param[in] evt: 事件对象的指针
 * @param[in] obj: 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_sel_obj_c0i(struct xwup_evt * evt, struct xwup_synobj * synobj)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_c0i(evt->bmp, synobj->sel.pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_sel_select(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, evt->num);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, evt->num);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = XWOK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        rc = xwup_cond_wait(&evt->cond,
                                            &evt->lock, XWOS_LK_SPLK, NULL,
                                            &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, evt->num);
                                }
                                break;
                        }
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_sel_select_to(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                          xwtm_t to)
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, evt->num);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, evt->num);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = XWOK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        rc = xwup_cond_wait_to(&evt->cond,
                                               &evt->lock, XWOS_LK_SPLK, NULL,
                                               to, &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, evt->num);
                                }
                                break;
                        }
                }
        }
        return rc;
}

__xwup_api
xwer_t xwup_sel_tryselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
        if (triggered) {
                if (NULL != trg) {
                        xwbmpop_assign(trg, evt->bmp, evt->num);
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        xwbmpop_and(trg, msk, evt->num);
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                }
                rc = XWOK;
        } else {
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                rc = -ENODATA;
        }
        return rc;
}

__xwup_api
xwer_t xwup_sel_select_unintr(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) { // cppcheck-suppress [misra-c2012-15.4]
                triggered = xwbmpop_t1mo(evt->bmp, msk, evt->num);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, evt->num);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, evt->num);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, evt->num);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = XWOK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, evt->num);
                        rc = xwup_cond_wait_unintr(&evt->cond,
                                                   &evt->lock, XWOS_LK_SPLK, NULL,
                                                   &lkst);
                        if (XWOK == rc) {
                                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwup_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, evt->num);
                                }
                                break;
                        }
                }
        }
        return rc;
}

/******** type:XWUP_EVT_TYPE_BR ********/
__xwup_api
xwer_t xwup_br_wait(struct xwup_evt * evt)
{
        xwreg_t cpuirq;
        xwssq_t pos;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        pos = xwbmpop_ffz(evt->bmp, evt->num);
        if ((pos < 0) || (pos >= (xwssq_t)evt->num)) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwbmpop_s1i(evt->bmp, (xwsq_t)pos);
        triggered = xwbmpop_t1ma(evt->bmp, evt->msk, evt->num);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, (xwsq_t)pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwup_cond_broadcast(&evt->cond); // cppcheck-suppress [misra-c2012-17.7]
                xwup_cthd_yield();
                rc = XWOK;
        } else {
                rc = xwup_cond_wait(&evt->cond,
                                    &evt->lock, XWOS_LK_SPLK, NULL,
                                    &lkst);
                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                        xwup_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, (xwsq_t)pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;

err_pos_range:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_br_wait_to(struct xwup_evt * evt, xwtm_t to)
{
        xwreg_t cpuirq;
        xwssq_t pos;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        pos = xwbmpop_ffz(evt->bmp, evt->num);
        if ((pos < 0) || (pos >= (xwssq_t)evt->num)) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwbmpop_s1i(evt->bmp, (xwsq_t)pos);
        triggered = xwbmpop_t1ma(evt->bmp, evt->msk, evt->num);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, (xwsq_t)pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwup_cond_broadcast(&evt->cond); // cppcheck-suppress [misra-c2012-17.7]
                xwup_cthd_yield();
                rc = XWOK;
        } else {
                rc = xwup_cond_wait_to(&evt->cond,
                                       &evt->lock, XWOS_LK_SPLK, NULL,
                                       to, &lkst);
                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                        xwup_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, (xwsq_t)pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;

err_pos_range:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

__xwup_api
xwer_t xwup_br_wait_unintr(struct xwup_evt * evt)
{
        xwreg_t cpuirq;
        xwssq_t pos;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        pos = xwbmpop_ffz(evt->bmp, evt->num);
        if ((pos < 0) || (pos >= (xwssq_t)evt->num)) {
                rc = -ECHRNG;
                goto err_pos_range;
        }
        xwbmpop_s1i(evt->bmp, (xwsq_t)pos);
        triggered = xwbmpop_t1ma(evt->bmp, evt->msk, evt->num);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, (xwsq_t)pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwup_cond_broadcast(&evt->cond); // cppcheck-suppress [misra-c2012-17.7]
                xwup_cthd_yield();
                rc = XWOK;
        } else {
                rc = xwup_cond_wait_unintr(&evt->cond,
                                           &evt->lock, XWOS_LK_SPLK, NULL,
                                           &lkst);
                if ((xwsq_t)XWOS_LKST_UNLOCKED == lkst) {
                        xwup_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, (xwsq_t)pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;

err_pos_range:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}
