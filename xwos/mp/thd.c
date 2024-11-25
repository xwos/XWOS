/**
 * @file
 * @brief XWOS MP内核：线程
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/lfq.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
#  include <xwos/mm/mempool/allocator.h>
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
#  include <xwos/mm/memslice.h>
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
#  include <xwos/mm/sma.h>
#elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/ospl/tls.h>
#include <xwos/mp/rtrq.h>
#if (1 == XWOSRULE_SKD_WQ_RT)
#  include <xwos/mp/rtwq.h>
#endif
#if (1 == XWOSRULE_SKD_WQ_PL)
#  include <xwos/mp/plwq.h>
#endif
#include <xwos/mp/tt.h>
#include <xwos/mp/sync/sem.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/lock/seqlock.h>
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
#  include <xwos/mp/lock/mtx.h>
#  include <xwos/mp/mtxtree.h>
#endif
#if defined(XWOSCFG_SYNC_COND) && (1 == XWOSCFG_SYNC_COND)
#  include <xwos/mp/sync/cond.h>
#endif
#include <xwos/mp/thd.h>

/* #define XWOS_THDLOGF */ /**< 调试日志开关 */
#ifdef XWOS_THDLOGF
#  define xwos_thdlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_thdlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
/**
 * @brief 结构体 `xwmp_thd` 的对象缓存
 */
static __xwmp_data struct xwmm_mempool_objcache xwmp_thd_cache;

/**
 * @brief 结构体 `xwmp_thd` 的对象缓存的名字
 */
const __xwmp_rodata char xwmp_thd_cache_name[] = "xwmp.thd.cache";
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
/**
 * @brief 结构体 `xwmp_thd` 的对象缓存
 */
static __xwmp_data struct xwmm_memslice xwmp_thd_cache;

/**
 * @brief 结构体 `xwmp_thd` 的对象缓存的名字
 */
const __xwmp_rodata char xwmp_thd_cache_name[] = "xwmp.thd.cache";
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
static __xwmp_data struct xwmm_sma * xwmp_thd_cache;
#endif

#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
extern
xwer_t board_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);

extern
xwer_t board_thd_stack_pool_free(xwstk_t * stk);
#endif

#if defined(BRDCFG_XWSKD_THD_POSTINIT_HOOK) && (1 == BRDCFG_XWSKD_THD_POSTINIT_HOOK)
extern
void board_thd_postinit_hook(struct xwmp_thd * thd);
#endif

#if (1 == XWOSRULE_SKD_THD_CREATE)
static __xwmp_code
struct xwmp_thd * xwmp_thd_alloc(void);

static __xwmp_code
void xwmp_thd_free(struct xwmp_thd * thd);

static __xwmp_code
xwstk_t * xwmp_thd_stack_alloc(xwsz_t stack_size);

static __xwmp_code
xwer_t xwmp_thd_stack_free(xwstk_t * stk);
#endif

static __xwmp_code
void xwmp_thd_construct(struct xwmp_thd * thd);

static __xwmp_code
void xwmp_thd_destruct(struct xwmp_thd * thd);

static __xwmp_code
xwer_t xwmp_thd_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_SKD_THD_CREATE)
static __xwmp_code
xwer_t xwmp_thd_dgc(struct xwos_object * obj);
#endif

static __xwmp_code
xwer_t xwmp_thd_activate(struct xwmp_thd * thd,
                         struct xwmp_thd_attr * attr,
                         xwmp_thd_f thdfunc, void * arg,
                         xwobj_gc_f gcfunc);

static __xwmp_code
void xwmp_thd_launch(struct xwmp_thd * thd, xwmp_thd_f thdfunc, void * arg);

static __xwmp_code
void xwmp_thd_ttn_callback(struct xwmp_ttn * ttn);

static __xwmp_code
void xwmp_thd_wqn_callback(struct xwmp_wqn * wqn);

static __xwmp_code
void xwmp_thd_outmigrate_frozen_lic(struct xwmp_thd * thd);

static __xwmp_code
xwer_t xwmp_thd_outmigrate_reqfrz_lic(struct xwmp_thd * thd, xwid_t dstcpu);

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
/**
 * @brief XWMP INIT CODE：初始化 `struct xwmp_thd` 的对象缓存
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
xwer_t xwmp_thd_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwmp_thd_cache,
                                        &mp->pa,
                                        xwmp_thd_cache_name,
                                        sizeof(struct xwmp_thd),
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwmp_thd_construct,
                                        (dtor_f)xwmp_thd_destruct);
        return rc;
}
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化 `struct xwmp_thd` 的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_thd_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwmp_thd_cache, zone_origin, zone_size,
                                sizeof(struct xwmp_thd),
                                xwmp_thd_cache_name,
                                (ctor_f)xwmp_thd_construct,
                                (dtor_f)xwmp_thd_destruct);
        return rc;
}
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
/**
 * @brief XWMP INIT CODE：初始化 `struct xwmp_thd` 的对象缓存
 * @param[in] sma: 简单内存分配器
 * @note
 * + 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
void xwmp_thd_cache_init(struct xwmm_sma * sma)
{
        xwmp_thd_cache = sma;
}
#endif

#if (1 == XWOSRULE_SKD_THD_CREATE)
/**
 * @brief 从线程对象缓存中申请一个对象
 * @return 线程对象的指针
 */
static __xwmp_code
struct xwmp_thd * xwmp_thd_alloc(void)
{
#  if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
        union {
                struct xwmp_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_objcache_alloc(&xwmp_thd_cache, &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(rc);
        }
        return mem.thd;
#  elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
        union {
                struct xwmp_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwmp_thd_cache, &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(rc);
        }
        return mem.thd;
#  elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
        union {
                struct xwmp_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwmp_thd_cache,
                            sizeof(struct xwmp_thd), XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(rc);
        } else {
                xwmp_thd_construct(mem.thd);
        }
        return mem.thd;
#  elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
        struct xwmp_thd * thd;

        thd = malloc(sizeof(struct xwmp_thd));
        if (NULL == thd) {
                thd = err_ptr(-ENOMEM);
        } else {
                xwmp_thd_construct(thd);
        }
        return thd;
#  else
        return err_ptr(-ENOSYS);
#  endif
}
#endif

#if (1 == XWOSRULE_SKD_THD_CREATE)
/**
 * @brief 释放线程对象
 * @param[in] thd: 线程对象的指针
 */
static __xwmp_code
void xwmp_thd_free(struct xwmp_thd * thd)
{
#  if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
        xwmm_mempool_objcache_free(&xwmp_thd_cache, thd);
#  elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
        xwmm_memslice_free(&xwmp_thd_cache, thd);
#  elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
        xwmm_sma_free(xwmp_thd_cache, thd);
#  elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
        xwmp_thd_destruct(thd);
        free(thd);
#  else
        XWOS_UNUSED(thd);
#  endif
}
#endif

#if (1 == XWOSRULE_SKD_THD_CREATE)
/**
 * @brief 动态分配线程栈
 * @param[in] stack_size: 线程栈的大小
 * @return 线程栈的首地址或指针类型的错误码或空指针
 */
static __xwmp_code
xwstk_t * xwmp_thd_stack_alloc(xwsz_t stack_size)
{
#  if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        union {
                xwstk_t * stkbase;
                void * anon;
        } mem;
        xwer_t rc;

        rc = board_thd_stack_pool_alloc(stack_size, &mem.stkbase);
        if (rc < 0) {
                mem.stkbase = err_ptr(rc);
        }
        return mem.stkbase;
#  elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
        union {
                xwstk_t * stkbase;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwmp_thd_cache,
                            stack_size, XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.stkbase = err_ptr(rc);
        }
        return mem.stkbase;
#  elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
        xwstk_t * stkbase;

        stkbase = malloc(stack_size);
        if (NULL == stkbase) {
                stkbase = err_ptr(-ENOMEM);
        }
        return stkbase;
#  else
        XWOS_UNUSED(stack_size);
        return err_ptr(-ENOSYS);
#  endif
}
#endif

#if (1 == XWOSRULE_SKD_THD_CREATE)
/**
 * @brief 释放动态分配的线程栈内存
 * @param[in] stk: 线程栈的首地址
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_thd_stack_free(xwstk_t * stk)
{
#  if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        return board_thd_stack_pool_free(stk);
#  elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
        return xwmm_sma_free(xwmp_thd_cache, stk);
#  elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
        free(stk);
        return XWOK;
#  else
        XWOS_UNUSED(stk);
        return -ENOSYS;
#  endif
}
#endif

/**
 * @brief 线程对象的构造函数
 * @param[in] thd: 线程对象的指针
 */
static __xwmp_code
void xwmp_thd_construct(struct xwmp_thd * thd)
{
        xwos_object_construct(&thd->xwobj);
}

/**
 * @brief 线程对象的析构函数
 * @param[in] thd: 线程对象的指针
 */
static __xwmp_code
void xwmp_thd_destruct(struct xwmp_thd * thd)
{
        xwos_object_destruct(&thd->xwobj);
}

/**
 * @brief 静态线程对象的垃圾回收函数
 * @param[in] thd: 线程对象的指针
 */
static __xwmp_code
xwer_t xwmp_thd_sgc(struct xwos_object * obj)
{
        struct xwmp_thd * thd;

        thd = xwcc_derof(obj, struct xwmp_thd, xwobj);
        xwmp_thd_destruct(thd);
        return XWOK;
}

#if (1 == XWOSRULE_SKD_THD_CREATE)
/**
 * @brief 动态线程对象的垃圾回收函数
 * @param[in] thd: 线程对象的指针
 */
static __xwmp_code
xwer_t xwmp_thd_dgc(struct xwos_object * obj)
{
        xwstk_t * base;
        struct xwmp_thd * thd;

        thd = xwcc_derof(obj, struct xwmp_thd, xwobj);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_FLAG_ALLOCATED_STACK & thd->stack.flag)) {
                base = ((struct xwmp_thd *)thd)->stack.base;
                xwmp_thd_stack_free(base); // cppcheck-suppress [misra-c2012-17.7]
        }
        xwmp_thd_free(thd);
        return XWOK;
}
#endif

__xwmp_api
xwer_t xwmp_thd_acquire(struct xwmp_thd * thd, xwsq_t tik)
{
        return xwos_object_acquire(&thd->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_thd_release(struct xwmp_thd * thd, xwsq_t tik)
{
        return xwos_object_release(&thd->xwobj, tik);
}

__xwmp_api
xwer_t xwmp_thd_grab(struct xwmp_thd * thd)
{
        return xwos_object_grab(&thd->xwobj);
}

__xwmp_api
xwer_t xwmp_thd_put(struct xwmp_thd * thd)
{
        return xwos_object_put(&thd->xwobj);
}

/**
 * @brief 激活线程对象
 * @param[in] thd: 线程对象的指针
 * @param[in] attr: 线程属性
 * @param[in] thdfunc: 线程函数的指针
 * @param[in] arg: 线程函数的参数
 * @param[in] gcfunc: 垃圾回收函数的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_thd_activate(struct xwmp_thd * thd,
                         struct xwmp_thd_attr * attr,
                         xwmp_thd_f thdfunc, void * arg,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = xwos_object_activate(&thd->xwobj, gcfunc);
        if (rc < 0) {
                goto err_xwobj_activate;
        }

        xwmp_splk_init(&thd->stlock);
        thd->state = (xwsq_t)XWMP_SKDOBJ_ST_UNKNOWN;
        /* FIXME: Current version supports privileged thread only. */
        if (attr->privileged) {
                xwaop_s1m(xwsq_t, &thd->stack.flag, (xwsq_t)XWMP_SKDOBJ_FLAG_PRIVILEGED,
                          NULL, NULL);
        } else {
                xwaop_s1m(xwsq_t, &thd->stack.flag, (xwsq_t)XWMP_SKDOBJ_FLAG_PRIVILEGED,
                          NULL, NULL);
        }
        if (attr->detached) {
                thd->state |= (xwsq_t)XWMP_SKDOBJ_ST_DETACHED;
        }
        xwlib_bclst_init_node(&thd->frznode);
        thd->migration.dst = (xwid_t)0;
        xwlib_bclst_init_node(&thd->rqnode);
        xwmp_ttn_init(&thd->ttn);
        xwmp_wqn_init(&thd->wqn);
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
        xwmp_mtxtree_init(&thd->mt);
#endif

        /* 优先级 */
        if (attr->priority > XWMP_SKD_PRIORITY_RT_MAX) {
                attr->priority = XWMP_SKD_PRIORITY_RT_MAX;
        } else if (attr->priority < XWMP_SKD_PRIORITY_RT_MIN) {
                attr->priority = XWMP_SKD_PRIORITY_RT_MIN;
        } else {}
        thd->sprio = attr->priority;
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.rq = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;

        /* 栈信息 */
        thd->stack.name = attr->name;
        thd->stack.size = attr->stack_size;
        thd->stack.base = attr->stack;
#if (defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK))
        thd->stack.sp = &thd->stack.base[(attr->stack_size / sizeof(xwstk_t))];
        thd->stack.tls = &thd->stack.base[0];
#elif (defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK))
        thd->stack.sp = &thd->stack.base[(attr->stack_size / sizeof(xwstk_t)) - 1U];
        thd->stack.tls = &thd->stack.base[0];
#elif (defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK))
        thd->stack.sp = &thd->stack.base[-1];
        thd->stack.tls = &thd->stack.base[(attr->stack_size / sizeof(xwstk_t))];
#elif (defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK))
        thd->stack.sp = &thd->stack.base[0];
        thd->stack.tls = &thd->stack.base[(attr->stack_size / sizeof(xwstk_t))];
#else
#  error "Unknown stack type!"
#endif
        thd->stack.guard_base = thd->stack.tls;
        thd->stack.guard = attr->stack_guard_size;

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        xwmp_cond_init(&thd->completion); // cppcheck-suppress [misra-c2012-17.7]
#endif
        xwlib_bclst_init_node(&thd->thdnode);
#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
        for (xwsq_t i = (xwsq_t)0; i < XWOSCFG_SKD_THD_LOCAL_DATA_NUM; i++) {
                thd->data[i] = NULL;
        }
#endif
        thd->libc.error_number = XWOK;

        /* 加入就绪队列 */
        if (NULL != thdfunc) {
                xwmp_thd_launch(thd, thdfunc, arg);
#if defined(BRDCFG_XWSKD_THD_POSTINIT_HOOK) && (1 == BRDCFG_XWSKD_THD_POSTINIT_HOOK)
                board_thd_postinit_hook(thd);
#endif
        } else {
                xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_STANDBY);
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        }
        return XWOK;

err_xwobj_activate:
        return rc;
}

/**
 * @brief 加载就绪态的线程对象
 * @param[in] thd: 线程对象的指针
 * @param[in] thdfunc: 线程主函数
 * @param[in] arg: 线程主函数的参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 无效的参数
 * @retval -EEXIST: 线程已经被加载
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非线程主函数已经退出，线程回到‘STANDBY’状态
 */
static __xwmp_code
void xwmp_thd_launch(struct xwmp_thd * thd, xwmp_thd_f thdfunc, void * arg)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        rc = xwmp_thd_grab(thd);
        XWOS_BUG_ON(rc < 0);
        xwskd = xwmp_skd_get_lc();
        xwmp_splk_lock_cpuirqsv(&xwskd->thdlistlock, &cpuirq);
        xwskd->thd_num++;
        xwlib_bclst_add_tail(&xwskd->thdlist, &thd->thdnode);
        xwmb_mp_store_release(struct xwmp_skd *, &thd->xwskd, xwskd);
        xwmp_splk_unlock_cpuirqrs(&xwskd->thdlistlock, cpuirq);
        thd->stack.main = thdfunc;
        thd->stack.arg = arg;
        xwospl_skd_init_stack(&thd->stack, xwmp_cthd_return);
        xwospl_tls_init(&thd->stack);
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_STANDBY);
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        rc = xwmp_thd_rq_add_tail(thd, thd->sprio); /* Add to ready queue */
        XWOS_BUG_ON(rc < 0);
}

__xwmp_api
void xwmp_thd_attr_init(struct xwmp_thd_attr * attr)
{
        if (NULL != attr) {
                attr->name = NULL;
                attr->stack = NULL;
                attr->stack_size = XWMMCFG_STACK_SIZE_DEFAULT;
                attr->stack_guard_size = XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
                attr->priority = XWMP_SKD_PRIORITY_RT_MIN;
                attr->detached = false;
                attr->privileged = XWOSCFG_SKD_THD_PRIVILEGED_DEFAULT;
        }
}

__xwmp_api
void xwmp_thd_get_attr(struct xwmp_thd * thd, struct xwmp_thd_attr * attr)
{
        attr->name = thd->stack.name;
        attr->stack = thd->stack.base;
        attr->stack_size = thd->stack.size;
        attr->stack_guard_size = thd->stack.guard;
        attr->priority = thd->sprio;
        attr->detached = !!(thd->state & (xwsq_t)XWMP_SKDOBJ_ST_DETACHED);
        attr->privileged = !!(thd->stack.flag & (xwsq_t)XWMP_SKDOBJ_FLAG_PRIVILEGED);
}

__xwmp_api
xwer_t xwmp_thd_init(struct xwmp_thd * thd,
                     const struct xwmp_thd_attr * inattr,
                     xwmp_thd_f thdfunc, void * arg)
{
        xwer_t rc;
        struct xwmp_thd_attr attr;

        xwmp_thd_construct(thd);
        attr = *inattr;
        thd->stack.flag = (xwsq_t)0;
        rc = xwmp_thd_activate(thd, &attr, thdfunc, arg, xwmp_thd_sgc);
        if (XWOK == rc) {
                xwos_thdlogf(INFO, thd, "Init Thread, "
                             "stack: {.base = 0x%lX, .size = 0x%lX}\r\n",
                             thd->stack.base, thd->stack.size);
        } else {
                xwos_thdlogf(ERR, thd, "Init Thread ... %d\r\n", rc);
        }
        return rc;
}

#if (1 == XWOSRULE_SKD_THD_CREATE)
__xwmp_api
xwer_t xwmp_thd_create(struct xwmp_thd ** thdpbuf,
                       const struct xwmp_thd_attr * inattr,
                       xwmp_thd_f thdfunc, void * arg)
{
        struct xwmp_thd * thd;
        struct xwmp_thd_attr attr;
        bool allocated_stack;
        xwer_t rc;

        if (NULL != inattr) {
                attr = *inattr;
                if ((xwsz_t)0 == attr.stack_size) {
                        attr.stack_size = XWMMCFG_STACK_SIZE_DEFAULT;
                } else if (attr.stack_size < XWMMCFG_STACK_SIZE_MIN) {
                        attr.stack_size = XWMMCFG_STACK_SIZE_MIN;
                } else {
                        attr.stack_size &= XWMM_STACK_ALIGNMENT_MASK;
                }
                if (NULL == attr.stack) {
                        attr.stack = xwmp_thd_stack_alloc(attr.stack_size);
                        if (is_err(attr.stack)) {
                                rc = ptr_err(attr.stack);
                                goto err_stack_alloc;
                        }
                        allocated_stack = true;
                } else {
                        allocated_stack = false;
                }
                if ((xwsz_t)0 == attr.stack_guard_size) {
                        attr.stack_guard_size = XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
                }
        } else {
                attr.name = NULL;
                attr.stack_size = XWMMCFG_STACK_SIZE_DEFAULT;
                attr.stack = xwmp_thd_stack_alloc(attr.stack_size);
                if (is_err(attr.stack)) {
                        rc = ptr_err(attr.stack);
                        goto err_stack_alloc;
                }
                allocated_stack = true;
                attr.stack_guard_size = XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
                attr.priority = XWMP_SKD_PRIORITY_RT_MIN;
                attr.detached = false;
                attr.privileged = XWOSCFG_SKD_THD_PRIVILEGED_DEFAULT;
        }

        thd = xwmp_thd_alloc();
        if (is_err(thd)) {
                rc = ptr_err(thd);
                goto err_thd_alloc;
        }

        if (allocated_stack) {
                thd->stack.flag = (xwsq_t)XWMP_SKDOBJ_FLAG_ALLOCATED_STACK;
        } else {
                thd->stack.flag = (xwsq_t)0;
        }

        rc = xwmp_thd_activate(thd, &attr, thdfunc, arg, xwmp_thd_dgc);
        if (rc < 0) {
                xwos_thdlogf(ERR, thd, "Activate Thread ... %d\r\n", rc);
                goto err_thd_activate;
        }
        xwos_thdlogf(INFO, thd, "Create Thread, "
                     "stack: {.base = 0x%lX, .size = 0x%lX}\r\n",
                     thd->stack.base, thd->stack.size);

        *thdpbuf = thd;
        return XWOK;

err_thd_activate:
        xwmp_thd_free(thd);
err_thd_alloc:
        if (allocated_stack) {
                xwmp_thd_stack_free(attr.stack); // cppcheck-suppress [misra-c2012-17.7]
        }
err_stack_alloc:
        *thdpbuf = NULL;
        return rc;
}
#endif

/**
 * @brief 执行退出线程的本地中断函数
 * @param[in] thd: 线程对象的指针
 * @param[in] rc: 线程的返回值
 */
__xwmp_code
xwer_t xwmp_thd_exit_lic(struct xwmp_thd * thd, xwer_t rc)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwmp_skd * xwskd;
        bool detached;
        xwreg_t cpuirq;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, ((xwsq_t)XWMP_SKDOBJ_ST_RUNNING |
                                        (xwsq_t)XWMP_SKDOBJ_ST_EXITING));
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_STANDBY);
        thd->stack.arg = err_ptr(rc);
        detached = !!((xwsq_t)XWMP_SKDOBJ_ST_DETACHED & thd->state);
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        xwmp_cond_broadcast(&thd->completion); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_splk_lock_cpuirq(&xwskd->pm.lock);
        xwmp_splk_lock(&xwskd->thdlistlock);
        xwlib_bclst_del_init(&thd->thdnode);
        xwskd->thd_num--;
        if (detached) {
                xwlib_bclst_add_tail(&xwskd->thdelist, &thd->thdnode);
        }
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwmp_splk_unlock(&xwskd->thdlistlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                // cppcheck-suppress [misra-c2012-17.7]
                xwmp_skd_notify_allfrz_lic(xwskd);
        } else {
                xwmp_splk_unlock(&xwskd->thdlistlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                xwmp_skd_req_swcx(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        }
#else
        XWOS_UNUSED(thd);
        XWOS_UNUSED(rc);
#endif
        return XWOK;
}

/**
 * @brief 线程主函数返回后的入口
 * @param[in] rc: 线程的返回值
 */
__xwmp_code
void xwmp_cthd_return(xwer_t rc)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        xwos_thdlogf(INFO, cthd, "return:%d\r\n", rc);
        xwospl_thd_exit_lc(cthd, rc);
#else
        XWOS_UNUSED(rc);
        while (true) {
        }
#endif
}

__xwmp_api
void xwmp_cthd_exit(xwer_t rc)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        xwos_thdlogf(INFO, cthd, "exit:%d\r\n", rc);
        xwospl_thd_exit_lc(cthd, rc);
#else
        XWOS_UNUSED(rc);
        while (true) {
        }
#endif
}

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
__xwmp_api
void xwmp_thd_quit(struct xwmp_thd * thd)
{
        xwreg_t cpuirq;

        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_STANDBY & thd->state)) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_EXITING);
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                xwmp_thd_intr(thd); // cppcheck-suppress [misra-c2012-17.7]
        }
}

__xwmp_api
xwer_t xwmp_thd_join(struct xwmp_thd * thd, xwer_t * trc)
{
        struct xwmp_thd * cthd;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        cthd = xwmp_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_join_self;
        }
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_DETACHED & thd->state)) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = -EINVAL;
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_JOINED & thd->state)) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = -EALREADY;
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_STANDBY & thd->state)) {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_JOINED);
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                rc = xwmp_thd_put(thd);
                XWOS_BUG_ON(rc < 0);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_JOINED);
                rc = xwmp_cond_wait(&thd->completion,
                                    &thd->stlock, XWOS_LK_SPLK, NULL,
                                    &lkst);
                if (XWOK == rc) {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                        rc = xwmp_thd_put(thd);
                        XWOS_BUG_ON(rc < 0);
                } else {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_JOINED);
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                }
        }

err_join_self:
        return rc;
}

__xwmp_api
xwer_t xwmp_thd_detach(struct xwmp_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_JOINED & thd->state)) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = -EINVAL;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_DETACHED);
                if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_STANDBY & thd->state)) {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                } else {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                }
                rc = XWOK;
        }
        return rc;
}
#else
__xwmp_api
void xwmp_thd_quit(struct xwmp_thd * thd)
{
        XWOS_UNUSED(thd);
}

__xwmp_api
xwer_t xwmp_thd_join(struct xwmp_thd * thd, xwer_t * trc)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}

__xwmp_api
xwer_t xwmp_thd_detach(struct xwmp_thd * thd)
{
        XWOS_UNUSED(thd);
        return -ENOSYS;
}
#endif

#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
/**
 * @brief 改变线程的动态优先级一次
 * @param[in] thd: 线程对象的指针
 * @param[in] dprio: 动态优先级
 * @param[out] pmtx: 指向缓冲区的指针，通过此缓冲区返回互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESTALE: 状态已经被其他CPU改变
 * @note
 * - 此函数将线程(thd)的优先级改成(dprio)，并返回接下来需要修改的互斥锁(*pmtx)
 *   的优先级。
 */
__xwmp_code
xwer_t xwmp_thd_chprio_once(struct xwmp_thd * thd, xwpr_t dprio,
                            struct xwmp_mtx ** pmtx)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if ((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_RUNNING |
                           (xwsq_t)XWMP_SKDOBJ_ST_FROZEN) & thd->state)) {
                thd->dprio.r = dprio;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = XWOK;
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_READY & thd->state)) {
                if (thd->dprio.rq == dprio) {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        rc = XWOK;
                } else {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        rc = xwmp_thd_rq_remove(thd);
                        if (XWOK == rc) {
                                rc = xwmp_thd_rq_add_tail(thd, dprio);
                        }
                }
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_BLOCKING & thd->state)) {
                if (thd->dprio.wq == dprio) {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        rc = XWOK;
                } else {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        /* 必须满足上锁顺序的要求。
                           此处必须解锁重新上锁，程序可能被中断打断。
                           因此，需要重新检查条件。*/
                        xwmp_splk_lock_cpuirq(&thd->wqn.lock);
                        if ((xwsq_t)XWMP_WQTYPE_MTX == thd->wqn.type) {
                                struct xwmp_mtx * mtx;

                                mtx = xwcc_derof(thd->wqn.wq, struct xwmp_mtx, rtwq);
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwmp_mtx_grab(mtx);
                                xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                                /* 必须满足上锁顺序的要求。
                                   此处必须解锁重新上锁，程序可能被中断打断。
                                   因此，需要重新检查条件。*/
                                xwmp_rtwq_lock_cpuirq(&mtx->rtwq);
                                xwmp_splk_lock(&thd->wqn.lock);
                                if (NULL == thd->wqn.cb) {
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        rc = -ESTALE;
                                } else if (thd->wqn.wq != &mtx->rtwq) {
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        rc = -ESTALE;
                                } else {
                                        // cppcheck-suppress [misra-c2012-17.7]
                                        xwmp_rtwq_remove_locked(&mtx->rtwq, &thd->wqn);
                                        xwmp_splk_lock(&thd->stlock);
                                        thd->dprio.wq = dprio;
                                        xwmp_splk_unlock(&thd->stlock);
                                        xwmp_rtwq_add_locked(&mtx->rtwq, &thd->wqn,
                                                             dprio);
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        *pmtx = mtx;
                                        rc = XWOK;
                                }
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwmp_mtx_put(mtx);
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
                        } else if ((xwsq_t)XWMP_WQTYPE_RTSEM == thd->wqn.type) {
                                struct xwmp_sem * sem;

                                sem = xwcc_derof(thd->wqn.wq, struct xwmp_sem, wq.rt);
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwmp_sem_grab(sem);
                                xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                                /* 必须满足上锁顺序的要求。
                                   此处必须解锁重新上锁，程序可能被中断打断。
                                   因此，需要重新检查条件。*/
                                xwmp_rtwq_lock_cpuirq(&sem->wq.rt);
                                xwmp_splk_lock(&thd->wqn.lock);
                                if (NULL == thd->wqn.cb) {
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                                        rc = -ESTALE;
                                } else if (thd->wqn.wq != &sem->wq.rt) {
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                                        rc = -ESTALE;
                                } else {
                                        // cppcheck-suppress [misra-c2012-17.7]
                                        xwmp_rtwq_remove_locked(&sem->wq.rt, &thd->wqn);
                                        xwmp_splk_lock(&thd->stlock);
                                        thd->dprio.wq = dprio;
                                        xwmp_splk_unlock(&thd->stlock);
                                        xwmp_rtwq_add_locked(&sem->wq.rt, &thd->wqn,
                                                             dprio);
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                                        rc = XWOK;
                                }
                                // cppcheck-suppress [misra-c2012-17.7]
                                xwmp_sem_put(sem);
#endif
                        } else {
                                xwmp_splk_lock(&thd->stlock);
                                thd->dprio.wq = dprio;
                                xwmp_splk_unlock(&thd->stlock);
                                xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                                rc = XWOK;
                        }
                }
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_SLEEPING & thd->state)) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = XWOK;
        } else {
                XWOS_BUG_ON((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_STANDBY & thd->state));
                rc = -ESTALE;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        }
        return rc;
}
#endif

#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
/**
 * @brief 重新设定线程的动态优先级
 * @param[in] thd: 线程对象的指针
 */
__xwmp_code
void xwmp_thd_chprio(struct xwmp_thd * thd)
{
        struct xwmp_mtxtree * mt;
        struct xwmp_mtx * tmp;
        xwpr_t dprio;
        xwsq_t seq;
        xwer_t rc;

        mt = &thd->mt;
        do {
                seq = xwmp_sqlk_rd_begin(&mt->lock);
                dprio = (thd->sprio > mt->maxprio) ? thd->sprio : mt->maxprio;
                rc = xwmp_thd_chprio_once(thd, dprio, &tmp);
        } while (xwmp_sqlk_rd_retry(&mt->lock, seq) || (rc < 0));
}
#endif

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param[in] thd: 线程对象的指针
 * @param[in] prio: 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 * @retval -ESTALE: 状态已经发生改变(当前线程的状态可能已经被其他CPU修改)
 */
__xwmp_code
xwer_t xwmp_thd_rq_add_head(struct xwmp_thd * thd, xwpr_t prio)
{
        struct xwmp_skd * xwskd;
        struct xwmp_rtrq * xwrtrq;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwrtrq = &xwskd->rq.rt;
        xwmp_splk_lock_cpuirqsv(&xwrtrq->lock, &cpuirq);
        xwmp_splk_lock(&thd->stlock);
        if ((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_RUNNING |
                           (xwsq_t)XWMP_SKDOBJ_ST_READY) & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EEXIST;
        } else {
                thd->dprio.rq = prio;
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_READY);
                xwmp_splk_unlock(&thd->stlock);
                rc = xwmp_rtrq_add_head_locked(xwrtrq, thd);
        }
        xwmp_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param[in] thd: 线程对象的指针
 * @param[in] prio: 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 * @retval -ESTALE: 状态已经发生改变(当前线程的状态可能已经被其他CPU修改)
 */
__xwmp_code
xwer_t xwmp_thd_rq_add_tail(struct xwmp_thd * thd, xwpr_t prio)
{
        struct xwmp_skd * xwskd;
        struct xwmp_rtrq * xwrtrq;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwrtrq = &xwskd->rq.rt;
        xwmp_splk_lock_cpuirqsv(&xwrtrq->lock, &cpuirq);
        xwmp_splk_lock(&thd->stlock);
        if ((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_RUNNING |
                           (xwsq_t)XWMP_SKDOBJ_ST_READY) & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EEXIST;
        } else {
                thd->dprio.rq = prio;
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_READY);
                xwmp_splk_unlock(&thd->stlock);
                rc = xwmp_rtrq_add_tail_locked(xwrtrq, thd);
        }
        xwmp_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief 将线程从就绪队列中删除
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 没有这个线程
 */
__xwmp_code
xwer_t xwmp_thd_rq_remove(struct xwmp_thd * thd)
{
        struct xwmp_skd * xwskd;
        struct xwmp_rtrq * xwrtrq;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwrtrq = &xwskd->rq.rt;
        xwmp_splk_lock_cpuirqsv(&xwrtrq->lock, &cpuirq);
        rc = xwmp_rtrq_remove_locked(xwrtrq, thd);
        if (XWOK == rc) {
                xwmp_splk_lock(&thd->stlock);
                xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_READY);
                thd->dprio.rq = XWMP_SKD_PRIORITY_INVALID;
                xwmp_splk_unlock(&thd->stlock);
        }
        xwmp_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

__xwmp_api
xwer_t xwmp_thd_intr(struct xwmp_thd * thd)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_UNINTERRUPTED & thd->state)) {
                rc = -EPERM;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_BLOCKING & thd->state)) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                /* 上锁必须按照锁的顺序，否则会出现死锁。
                   一旦“解锁-重新上锁”以后就需要重新检查条件。 */
                xwmp_splk_lock_cpuirq(&thd->wqn.lock);
                if ((xwsq_t)XWMP_WQTYPE_NULL == thd->wqn.type) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = (xwsq_t)XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &thd->wqn.reason,
                                    xwaop_mo_release, (xwsq_t)XWMP_WQN_REASON_INTR);
                        cb = thd->wqn.cb;
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        cb(&thd->wqn);
                        rc = XWOK;
#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
                } else if ((xwsq_t)XWMP_WQTYPE_PLSEM == thd->wqn.type) {
                        struct xwmp_sem * sem;

                        sem = xwcc_derof(thd->wqn.wq, struct xwmp_sem, wq.pl);
                        xwmp_sem_grab(sem); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_plsem_intr(sem, &thd->wqn);
                        xwmp_sem_put(sem); // cppcheck-suppress [misra-c2012-17.7]
#endif
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
                } else if ((xwsq_t)XWMP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwmp_sem * sem;

                        sem = xwcc_derof(thd->wqn.wq, struct xwmp_sem, wq.rt);
                        xwmp_sem_grab(sem); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_rtsem_intr(sem, &thd->wqn);
                        xwmp_sem_put(sem); // cppcheck-suppress [misra-c2012-17.7]
#endif
#if defined(XWOSCFG_SYNC_COND) && (1 == XWOSCFG_SYNC_COND)
                } else if ((xwsq_t)XWMP_WQTYPE_COND == thd->wqn.type) {
                        struct xwmp_cond * cond;

                        cond = xwcc_derof(thd->wqn.wq, struct xwmp_cond, wq.pl);
                        xwmp_cond_grab(cond); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_cond_intr(cond, &thd->wqn);
                        xwmp_cond_put(cond); // cppcheck-suppress [misra-c2012-17.7]
#endif
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
                } else if ((xwsq_t)XWMP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwmp_mtx * mtx;

                        mtx = xwcc_derof(thd->wqn.wq, struct xwmp_mtx, rtwq);
                        xwmp_mtx_grab(mtx); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_mtx_intr(mtx, thd);
                        xwmp_mtx_put(mtx); // cppcheck-suppress [misra-c2012-17.7]
#endif
                } else {
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        XWOS_BUG();
                        rc = -EBUG;
                }
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_SLEEPING & thd->state)) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                /* IRQs may happen. */
                struct xwmp_skd * xwskd;
                struct xwmp_tt * xwtt;

                xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
                xwtt = &xwskd->tt;
                xwmp_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwmp_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwmp_splk_lock(&thd->stlock);
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwmp_thd_wakeup(thd); // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_skd_chkpmpt(thd->xwskd);
                } else {
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                }
        } else {
                rc = -EINVAL;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        }
        return rc;
}


#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
/**
 * @brief 唤醒一个阻塞/睡眠态的线程，将其加入到调度器的就绪队列
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_thd_wakeup(struct xwmp_thd * thd)
{
        struct xwmp_mtxtree * mt;
        xwpr_t dprio;
        xwreg_t cpuirq;
        xwer_t rc;

        mt = &thd->mt;
        xwmp_sqlk_rdex_lock_cpuirqsv(&mt->lock, &cpuirq);
        dprio = (thd->sprio > mt->maxprio) ? thd->sprio : mt->maxprio;
        rc = xwmp_thd_rq_add_tail(thd, dprio);
        xwmp_sqlk_rdex_unlock_cpuirqrs(&mt->lock, cpuirq);
        return rc;
}
#else
/**
 * @brief 唤醒一个阻塞/睡眠态的线程，将其加入到调度器的就绪队列
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_thd_wakeup(struct xwmp_thd * thd)
{
        return xwmp_thd_rq_add_tail(thd, thd->sprio);
}
#endif

/**
 * @brief 线程的时间树节点回调函数
 * @param[in] entry: 线程对象的地址
 */
static __xwmp_code
void xwmp_thd_ttn_callback(struct xwmp_ttn * ttn)
{
        struct xwmp_thd * thd;
        xwreg_t cpuirq;

        thd = xwcc_derof(ttn, struct xwmp_thd, ttn);
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_SLEEPING);
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        xwmp_thd_wakeup(thd); // cppcheck-suppress [misra-c2012-17.7]
}

/**
 * @brief 将线程加入到时间树上
 * @param[in] thd: 线程对象的指针
 * @param[in] xwtt: 时间树的指针
 * @param[in] to: 期望唤醒的时间点
 * @param[in] cpuirq: 本地CPU的中断标志
 * @return 错误码
 * @note
 * - 此函数只能在取得写锁xwtt->lock以及关闭本地CPU的中断时才可调用。
 */
__xwmp_code
xwer_t xwmp_thd_tt_add_locked(struct xwmp_thd * thd, struct xwmp_tt * xwtt,
                              xwtm_t to, xwreg_t cpuirq)
{
        xwer_t rc;

        /* add to time tree */
        thd->ttn.wkup_xwtm = to;
        xwaop_store(xwsq_t, &thd->ttn.wkuprs,
                    xwaop_mo_release, (xwsq_t)XWMP_TTN_WKUPRS_UNKNOWN);
        thd->ttn.xwtt = xwtt;
        thd->ttn.cb = xwmp_thd_ttn_callback;
        rc = xwmp_tt_add_locked(xwtt, &thd->ttn, cpuirq);
        return rc;
}

/**
 * @brief 线程的等待队列节点回调函数
 * @param[in] entry: 线程对象的地址
 */
__xwmp_code
void xwmp_thd_wqn_callback(struct xwmp_wqn * wqn)
{
        struct xwmp_thd * thd;
        xwreg_t cpuirq;

        thd = xwcc_derof(wqn, struct xwmp_thd, wqn);
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, ((xwsq_t)XWMP_SKDOBJ_ST_BLOCKING |
                                        (xwsq_t)XWMP_SKDOBJ_ST_UNINTERRUPTED));
        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        xwmp_thd_wakeup(thd); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_skd_chkpmpt(thd->xwskd);
}

#if (1 == XWOSRULE_SKD_WQ_RT)
/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param[in] thd: 线程对象的指针
 * @param[in] xwrtwq: 实时（红黑树）等待队列
 * @param[in] type: 等待队列的类型
 * @param[in] dprio: 加入时线程的动态优先级
 */
__xwmp_code
void xwmp_thd_eq_rtwq_locked(struct xwmp_thd * thd,
                             struct xwmp_rtwq * xwrtwq,
                             xwu16_t type, xwpr_t dprio)
{
        xwmp_splk_lock(&thd->wqn.lock);
        thd->wqn.wq = xwrtwq;
        thd->wqn.type = type;
        xwaop_store(xwsq_t, &thd->wqn.reason,
                    xwaop_mo_release, (xwsq_t)XWMP_WQN_REASON_UNKNOWN);
        thd->wqn.cb = xwmp_thd_wqn_callback;
        xwmp_rtwq_add_locked(xwrtwq, &thd->wqn, dprio);
        xwmp_splk_unlock(&thd->wqn.lock);
}
#endif

#if (1 == XWOSRULE_SKD_WQ_PL)
/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param[in] thd: 线程对象的指针
 * @param[in] xwplwq: 管道（双循环链表）等待队列
 * @param[in] type: 等待队列的类型
 */
__xwmp_code
void xwmp_thd_eq_plwq_locked(struct xwmp_thd * thd,
                             struct xwmp_plwq * xwplwq,
                             xwu16_t type)
{
        xwmp_splk_lock(&thd->wqn.lock);
        thd->wqn.wq = xwplwq;
        thd->wqn.type = type;
        xwaop_store(xwsq_t, &thd->wqn.reason,
                    xwaop_mo_release, (xwsq_t)XWMP_WQN_REASON_UNKNOWN);
        thd->wqn.cb = xwmp_thd_wqn_callback;
        xwmp_plwq_add_tail_locked(xwplwq, &thd->wqn);
        xwmp_splk_unlock(&thd->wqn.lock);
}
#endif

__xwmp_api
void xwmp_cthd_yield(void)
{
        struct xwmp_thd * cthd;
        xwreg_t cpuirq;
        xwpr_t prio;
        xwer_t rc;

        cthd = xwmp_skd_get_cthd_lc();
        xwmp_rawly_lock_cpuirqsv(&cthd->stlock, &cpuirq);
        prio = cthd->dprio.r;
        xwbop_c0m(xwsq_t, &cthd->state, (xwsq_t)XWMP_SKDOBJ_ST_RUNNING);
        cthd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwmp_rawly_unlock_cpuirq(&cthd->stlock);
        rc = xwmp_thd_rq_add_tail(cthd, prio);
        XWOS_BUG_ON(rc < 0);
        xwmp_skd_req_swcx(cthd->xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwmp_api
xwer_t xwmp_cthd_sleep_to(xwtm_t to)
{
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;
        struct xwmp_thd * cthd;
        xwtm_t now;
        xwer_t rc;
        xwsq_t wkuprs;
        xwreg_t cpuirq;

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
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        now = xwmp_syshwt_get_time(hwt);
        if (xwtm_cmp(to, now) < 0) {
                rc = -ETIMEDOUT;
                goto err_timedout;
        }
        if (xwtm_cmp(to, now) == 0) {
                rc = XWOK;
                goto err_timedout;
        }
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        /* 检查是否被中断 */
        rc = xwmp_skd_wakelock_lock(xwskd);
        if (rc < 0) {
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，返回-EINTR。*/
                rc = -EINTR;
                goto err_intr;
        }
        xwmp_splk_lock(&cthd->stlock);
        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWMP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWMP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWMP_SKDOBJ_ST_FROZEN |
                                   (xwsq_t)XWMP_SKDOBJ_ST_MIGRATING) & cthd->state));
        if ((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE |
                           (xwsq_t)XWMP_SKDOBJ_ST_EXITING) & cthd->state)) {
                xwmp_splk_unlock(&cthd->stlock);
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                xwmp_skd_wakelock_unlock(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                rc = -EINTR;
                goto err_intr;
        }
        /* 设置线程的睡眠态 */
        xwbop_c0m(xwsq_t, &cthd->state, (xwsq_t)XWMP_SKDOBJ_ST_RUNNING);
        cthd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &cthd->state, (xwsq_t)XWMP_SKDOBJ_ST_SLEEPING);
        xwmp_splk_unlock(&cthd->stlock);
        // cppcheck-suppress [misra-c2012-17.7]
        xwmp_thd_tt_add_locked(cthd, xwtt, to, cpuirq);
        xwmp_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        xwmp_skd_wakelock_unlock(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_skd_req_swcx(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        wkuprs = xwaop_load(xwsq_t, &cthd->ttn.wkuprs, xwaop_mo_relaxed);
        if ((xwsq_t)XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                rc = XWOK;
        } else if ((xwsq_t)XWMP_TTN_WKUPRS_INTR == wkuprs) {
                rc = -EINTR;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        return rc;

err_intr:
err_timedout:
err_dis:
        return rc;
}

__xwmp_api
xwer_t xwmp_cthd_sleep_from(xwtm_t * from, xwtm_t dur)
{
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;
        struct xwmp_thd * cthd;
        xwtm_t to;
        xwreg_t cpuirq;
        xwsq_t wkuprs;
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
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        to = xwtm_add_safely(*from, dur);
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        /* 检查是否被中断 */
        rc = xwmp_skd_wakelock_lock(xwskd);
        if (rc < 0) {
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，返回-EINTR。*/
                rc = -EINTR;
                goto err_intr;
        }
        xwmp_splk_lock(&cthd->stlock);
        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWMP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWMP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWMP_SKDOBJ_ST_FROZEN |
                                   (xwsq_t)XWMP_SKDOBJ_ST_MIGRATING) & cthd->state));
        if ((xwsq_t)0 != (((xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE |
                           (xwsq_t)XWMP_SKDOBJ_ST_EXITING) & cthd->state)) {
                xwmp_splk_unlock(&cthd->stlock);
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                xwmp_skd_wakelock_unlock(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                rc = -EINTR;
                goto err_intr;
        }
        /* 设置线程的睡眠态 */
        xwbop_c0m(xwsq_t, &cthd->state, (xwsq_t)XWMP_SKDOBJ_ST_RUNNING);
        cthd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &cthd->state, (xwsq_t)XWMP_SKDOBJ_ST_SLEEPING);
        xwmp_splk_unlock(&cthd->stlock);
        // cppcheck-suppress [misra-c2012-17.7]
        xwmp_thd_tt_add_locked(cthd, xwtt, to, cpuirq);
        xwmp_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        xwmp_skd_wakelock_unlock(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwmp_skd_req_swcx(xwskd); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        wkuprs = xwaop_load(xwsq_t, &cthd->ttn.wkuprs, xwaop_mo_relaxed);
        if ((xwsq_t)XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                rc = XWOK;
        } else if ((xwsq_t)XWMP_TTN_WKUPRS_INTR == wkuprs) {
                rc = -EINTR;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }

err_intr:
        *from = xwmp_syshwt_get_time(hwt);
err_dis:
        return rc;
}

/**
 * @brief 申请冻结当前CPU中的线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EALREADY: 线程已经被冻结
 * @note
 * - 此函数只可在线程所属的CPU的中断上下文中被调用；
 * - 此函数假设线程对象已经被引用，执行过程中不会成为野指针。
 */
__xwmp_code
xwer_t xwmp_thd_reqfrz_lic(struct xwmp_thd * thd)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_splk_lock_cpuirqsv(&xwskd->pm.lock, &cpuirq);
        xwmp_splk_lock(&thd->stlock);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_FROZEN & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EALREADY;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE);
                xwmp_splk_unlock(&thd->stlock);
                rc = XWOK;
        }
        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
        return rc;
}

/**
 * @brief 执行冻结线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在线程所属的CPU的调度器服务中断中执行；
 * - 此中断只可从函数@ref xwmp_cthd_freeze()中进入。
 */
__xwmp_code
xwer_t xwmp_thd_freeze_lic(struct xwmp_thd * thd)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwid_t srccpu;
        xwid_t dstcpu;
        xwer_t rc;

        XWOS_BUG_ON((xwsq_t)0 == ((xwsq_t)XWMP_SKDOBJ_ST_RUNNING & thd->state));
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        srccpu = xwskd->id;
        xwmp_splk_lock_cpuirqsv(&xwskd->pm.lock, &cpuirq);
        xwmp_splk_lock(&thd->stlock);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE & thd->state)) {
                xwbop_c0m(xwsq_t, &thd->state, ((xwsq_t)XWMP_SKDOBJ_ST_RUNNING |
                                                (xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE));
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_FROZEN);
                if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_MIGRATING & thd->state)) {
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_thd_outmigrate_frozen_lic(thd);
                        dstcpu = thd->migration.dst;
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        if (srccpu == dstcpu) {
                                xwmp_thd_immigrate_lic(thd);
                        } else {
                                xwospl_thd_immigrate(thd, dstcpu);
                        }
                        xwmp_splk_lock_cpuirqsv(&xwskd->pm.lock, &cpuirq);
                } else {
                        xwmp_splk_unlock(&thd->stlock);
                        xwskd->pm.frz_thd_cnt++;
                        xwlib_bclst_add_tail(&xwskd->pm.frzlist, &thd->frznode);
                }
                xwmp_splk_lock(&xwskd->thdlistlock);
                if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                        xwmp_splk_unlock(&xwskd->thdlistlock);
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_skd_notify_allfrz_lic(xwskd);
                } else {
                        xwmp_splk_unlock(&xwskd->thdlistlock);
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmp_skd_req_swcx(xwskd);
                }
                rc = XWOK;
        } else {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                rc = -ECANCELED;
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cthd_freeze(void)
{
        struct xwmp_thd * cthd;
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = xwmp_skd_dspmpt_lc();
        cthd = xwmp_skd_get_cthd(xwskd);
        xwmp_splk_lock_cpuirqsv(&cthd->stlock, &cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE & cthd->state)) {
                xwmp_splk_unlock_cpuirqrs(&cthd->stlock, cpuirq);
                rc = xwospl_thd_freeze_lc(cthd);
        } else {
                xwmp_splk_unlock_cpuirqrs(&cthd->stlock, cpuirq);
                xwmp_skd_enpmpt(xwskd); // cppcheck-suppress [misra-c2012-17.7]
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief 解冻线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在线程所属的CPU的中断中执行。
 * - 此函数只能在持有锁xwskd->pm.lock时调用。
 */
__xwmp_code
xwer_t xwmp_thd_thaw_lic_pmlk(struct xwmp_thd * thd)
{
        struct xwmp_skd * xwskd;
        xwpr_t prio;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        XWOS_BUG_ON(xwskd != xwmp_skd_get_lc());

        xwmp_splk_lock(&thd->stlock);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE & thd->state)) {
                xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE);
                xwmp_splk_unlock(&thd->stlock);
                rc = -ECANCELED;
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_MIGRATING & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EPERM;
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_FROZEN & thd->state)) {
                prio = thd->dprio.r;
                xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_FROZEN);
                thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
                xwmp_splk_unlock(&thd->stlock);
                xwlib_bclst_del_init(&thd->frznode);
                xwskd->pm.frz_thd_cnt--;
                rc = xwmp_thd_rq_add_tail(thd, prio);
        } else {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EPERM;
        }
        return rc;
}

__xwmp_api
bool xwmp_cthd_shld_frz(void)
{
        struct xwmp_thd * cthd;
        bool ret;

        cthd = xwmp_skd_get_cthd_lc();
        ret = !!(cthd->state & (xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE);
        return ret;
}

__xwmp_api
bool xwmp_cthd_shld_stop(void)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        return !!((xwsq_t)XWMP_SKDOBJ_ST_EXITING & cthd->state);
#else
        return false;
#endif
}

__xwmp_api
bool xwmp_cthd_frz_shld_stop(bool * frozen)
{
        struct xwmp_thd * cthd;
        bool frz;
        xwer_t rc;

        frz = false;
        if (xwmp_cthd_shld_frz()) {
                rc = xwmp_cthd_freeze();
                if (XWOK == rc) {
                        frz = true;
                }
        }
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }
        cthd = xwmp_skd_get_cthd_lc();
        return !!((xwsq_t)XWMP_SKDOBJ_ST_EXITING & cthd->state);
}

/**
 * @brief 将线程迁移进目标CPU
 * @param[in] thd: 线程对象的指针
 * @note
 * - 此函数只能在目标CPU的中断上下文中执行；
 * - 此函数假设线程对象已经被引用，执行过程中不会成为野指针
 */
__xwmp_code
void xwmp_thd_immigrate_lic(struct xwmp_thd * thd)
{
        struct xwmp_skd * new;
        xwreg_t cpuirq;
        xwpr_t prio;
        xwer_t rc;

        XWOS_BUG_ON((xwsq_t)0 == ((xwsq_t)XWMP_SKDOBJ_ST_FROZEN & thd->state));
        XWOS_BUG_ON((xwsq_t)0 == ((xwsq_t)XWMP_SKDOBJ_ST_MIGRATING & thd->state));
        new = xwmp_skd_get_lc();
        xwmp_splk_lock_cpuirqsv(&new->pm.lock, &cpuirq);
        xwmp_splk_lock(&new->thdlistlock);
        thd->migration.dst = 0;
        xwlib_bclst_add_tail(&new->thdlist, &thd->thdnode);
        new->thd_num++;
        xwmb_mp_store_release(struct xwmp_skd *, &thd->xwskd, new);
        xwmp_splk_unlock(&new->thdlistlock);
        if (xwmp_skd_get_pm_state(new) < (xwsq_t)XWMP_SKD_WKLKCNT_RUNNING) {
                xwmp_splk_lock(&thd->stlock);
                xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_MIGRATING);
                xwmp_splk_unlock(&thd->stlock);
                xwlib_bclst_add_tail(&new->pm.frzlist, &thd->frznode);
                new->pm.frz_thd_cnt++;
                xwmp_splk_unlock_cpuirqrs(&new->pm.lock, cpuirq);
        } else {
                xwmp_splk_lock(&thd->stlock);
                prio = thd->dprio.r;
                xwbop_c0m(xwsq_t, &thd->state, ((xwsq_t)XWMP_SKDOBJ_ST_FROZEN |
                                                (xwsq_t)XWMP_SKDOBJ_ST_MIGRATING));
                thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
                xwmp_splk_unlock(&thd->stlock);
                xwmp_splk_unlock_cpuirqrs(&new->pm.lock, cpuirq);
                rc = xwmp_thd_rq_add_tail(thd, prio);
                XWOS_BUG_ON(rc < 0);
                xwmp_skd_chkpmpt(new);
        }
}

/**
 * @brief 将线程从当前CPU中迁出
 * @param[in] thd: 线程对象的指针
 * @note
 * - 此函数只能在线程所属的CPU的中断上下文中执行；
 * - 此函数被调用时需要获得当前CPU调度器的锁 `pm.lock` 并且关闭本地CPU的中断；
 * - 此函数假设线程对象已经被引用，执行过程中不会成为野指针；
 */
static __xwmp_code
void xwmp_thd_outmigrate_frozen_lic(struct xwmp_thd * thd)
{
        struct xwmp_skd * old;

        XWOS_BUG_ON((xwsq_t)0 == ((xwsq_t)XWMP_SKDOBJ_ST_FROZEN & thd->state));
        XWOS_BUG_ON((xwsq_t)0 == ((xwsq_t)XWMP_SKDOBJ_ST_MIGRATING & thd->state));
        xwmb_mp_load_acquire(struct xwmp_skd *, old, &thd->xwskd);
        XWOS_BUG_ON(old != xwmp_skd_get_lc());
        xwmp_splk_lock(&old->thdlistlock);
        xwlib_bclst_del_init(&thd->thdnode);
        old->thd_num--;
        xwmb_mp_store_release(struct xwmp_skd *, &thd->xwskd, NULL);
        xwmp_splk_unlock(&old->thdlistlock);
}

/**
 * @brief 申请冻结线程，以便将线程从本地CPU迁移到另一个CPU
 * @param[in] thd: 线程对象的指针
 * @param[in] dstcpu: 目标CPU的ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINPROGRESS: 线程处于迁移的过程中
 * @retval -EALREADY: 线程已经被冻结
 * @note
 * - 此函数只可在线程所属的CPU的中断上下文中被调用；
 * - 此函数假设线程对象已被引用，执行过程中不会成为野指针。
 */
static __xwmp_code
xwer_t xwmp_thd_outmigrate_reqfrz_lic(struct xwmp_thd * thd, xwid_t dstcpu)
{
        struct xwmp_skd * xwskd;
        xwid_t srccpu;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        srccpu = xwskd->id;
        xwmp_splk_lock_cpuirqsv(&xwskd->pm.lock, &cpuirq);
        xwmp_splk_lock(&thd->stlock);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_STANDBY & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                rc = -ESHUTDOWN;
        } else if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_MIGRATING & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                rc = -EINPROGRESS;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWMP_SKDOBJ_ST_MIGRATING);
                if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_FROZEN & thd->state)) {
                        xwmp_splk_unlock(&thd->stlock);
                        thd->migration.dst = dstcpu;
                        xwmp_thd_outmigrate_frozen_lic(thd);
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                        if (srccpu == dstcpu) {
                                xwmp_thd_immigrate_lic(thd);
                        } else {
                                xwospl_thd_immigrate(thd, dstcpu);
                        }
                } else {
                        xwbop_s1m(xwsq_t, &thd->state,
                                  (xwsq_t)XWMP_SKDOBJ_ST_FREEZABLE);
                        xwmp_splk_unlock(&thd->stlock);
                        thd->migration.dst = dstcpu;
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 从本地CPU中移出线程，并准备迁移入目标CPU
 * @param[in] thd: 线程对象的指针
 * @param[in] dstcpu: 目标CPU的ID
 * @return 错误码
 * @note
 * - 此函数只能在线程所属的CPU中执行。
 * - 此函数假设线程对象已经被引用，执行过程中不会成为野指针。
 */
__xwmp_code
xwer_t xwmp_thd_outmigrate_lic(struct xwmp_thd * thd, xwid_t dstcpu)
{
        struct xwmp_skd * xwskd;
        struct xwmp_skd * local;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        local = xwmp_skd_get_lc();
        XWOS_BUG_ON(xwskd != local);
        rc = xwmp_thd_outmigrate_reqfrz_lic(thd, dstcpu);
        return rc;
}

__xwmp_api
xwer_t xwmp_thd_migrate(struct xwmp_thd * thd, xwid_t dstcpu)
{
        struct xwmp_skd * xwskd;
        xwid_t srccpu;
        xwid_t localcpu;
        xwsq_t state;
        xwer_t rc;

        if (dstcpu >= CPUCFG_CPU_NUM) {
                rc = -ENODEV;
                goto err_badcpuid;
        }
        rc = xwmp_thd_grab(thd);
        if (rc < 0) {
                goto err_thd_grab;
        }
        xwmb_mp_load_acquire(xwsq_t, state, &thd->state);
        if ((xwsq_t)0 != ((xwsq_t)XWMP_SKDOBJ_ST_STANDBY & state)) {
                rc = -ESHUTDOWN;
                goto err_thd_standby;
        }

        localcpu = xwmp_skd_get_cpuid_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        srccpu = xwskd->id;
        if (localcpu == srccpu) {
                if (XWOK == xwmp_irq_get_id(NULL)) {
                        rc = xwmp_thd_outmigrate_lic(thd, dstcpu);
                } else {
                        rc = xwospl_thd_outmigrate(thd, dstcpu);
                }
        } else {
                rc = xwospl_thd_outmigrate(thd, dstcpu);
        }
        return rc;

err_thd_standby:
        xwmp_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
err_thd_grab:
err_badcpuid:
        return rc;
}

#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
__xwmp_api
xwer_t xwmp_thd_set_data(struct xwmp_thd * thd, xwsq_t pos, void * data)
{
        xwer_t rc;

        if (pos < XWOSCFG_SKD_THD_LOCAL_DATA_NUM) {
                thd->data[pos] = data;
                rc = XWOK;
        } else {
                rc = -ECHRNG;
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_thd_get_data(struct xwmp_thd * thd, xwsq_t pos, void ** databuf)
{
        xwer_t rc;

        if (pos < XWOSCFG_SKD_THD_LOCAL_DATA_NUM) {
                *databuf = thd->data[pos];
                rc = XWOK;
        } else {
                *databuf = NULL;
                rc = -ECHRNG;
        }
        return rc;
}

__xwmp_api
xwer_t xwmp_cthd_set_data(xwsq_t pos, void * data)
{
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        return xwmp_thd_set_data(cthd, pos, data);
}

__xwmp_api
xwer_t xwmp_cthd_get_data(xwsq_t pos, void ** databuf)
{
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        return xwmp_thd_get_data(cthd, pos, databuf);

}
#endif
