/**
 * @file
 * @brief XWOS UP内核：线程
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
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/rtrq.h>
#if (1 == XWOSRULE_SKD_WQ_RT)
#  include <xwos/up/rtwq.h>
#endif
#if (1 == XWOSRULE_SKD_WQ_PL)
#  include <xwos/up/plwq.h>
#endif
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
#  include <xwos/up/lock/mtx.h>
#  include <xwos/up/mtxtree.h>
#endif
#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
#  include <xwos/up/sync/plsem.h>
#endif
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
#  include <xwos/up/sync/rtsem.h>
#endif
#if defined(XWOSCFG_SYNC_COND) && (1 == XWOSCFG_SYNC_COND)
#  include <xwos/up/sync/cond.h>
#endif

/* #define XWOS_THDLOGF */ /**< 调试日志开关 */
#ifdef XWOS_THDLOGF
#  define xwos_thdlogf(lv, thd, fmt, ...) \
          xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_thdlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
/**
 * @brief 结构体 `xwup_thd` 的对象缓存
 */
static __xwup_data struct xwmm_mempool_objcache xwup_thd_cache;

/**
 * @brief 结构体 `xwup_thd` 的对象缓存的名字
 */
const __xwup_rodata char xwup_thd_cache_name[] = "xwup.thd.cache";
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
/**
 * @brief 结构体 `xwup_thd` 的对象缓存
 */
static __xwup_data struct xwmm_memslice xwup_thd_cache;

/**
 * @brief 结构体 `xwup_thd` 的对象缓存的名字
 */
const __xwup_rodata char xwup_thd_cache_name[] = "xwup.thd.cache";
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
static __xwup_data struct xwmm_sma * xwup_thd_cache;
#endif

static __xwup_code
void xwup_thd_construct(struct xwup_thd * thd);

static __xwup_code
void xwup_thd_destruct(struct xwup_thd * thd);

static __xwup_code
xwer_t xwup_thd_sgc(struct xwos_object * obj);

#if (1 == XWOSRULE_SKD_THD_CREATE)
static __xwup_code
xwer_t xwup_thd_dgc(struct xwos_object * obj);
#endif

#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
extern __xwup_code
xwer_t board_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);

extern __xwup_code
xwer_t board_thd_stack_pool_free(xwstk_t * stk);
#endif

#if defined(BRDCFG_XWSKD_THD_POSTINIT_HOOK) && (1 == BRDCFG_XWSKD_THD_POSTINIT_HOOK)
extern
void board_thd_postinit_hook(struct xwup_thd * thd);
#endif

#if (1 == XWOSRULE_SKD_THD_CREATE)
static __xwup_code
struct xwup_thd * xwup_thd_alloc(void);

static __xwup_code
void xwup_thd_free(struct xwup_thd * thd);

static __xwup_code
xwstk_t * xwup_thd_stack_alloc(xwsz_t stack_size);

static __xwup_code
xwer_t xwup_thd_stack_free(xwstk_t * stk);
#endif

static __xwup_code
xwer_t xwup_thd_activate(struct xwup_thd * thd,
                         struct xwup_thd_attr * attr,
                         xwup_thd_f thdfunc, void * arg,
                         xwobj_gc_f gcfunc);

static __xwup_code
void xwup_thd_ttn_callback(struct xwup_ttn * ttn);

static __xwup_code
void xwup_thd_wqn_callback(struct xwup_wqn * wqn);

static __xwup_code
void xwup_thd_launch(struct xwup_thd * thd, xwup_thd_f thdfunc, void * arg);

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
/**
 * @brief XWUP INIT CODE：初始化 `struct xwup_thd` 的对象缓存
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
xwer_t xwup_thd_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        rc = xwmm_mempool_objcache_init(&xwup_thd_cache,
                                        &mp->pa,
                                        xwup_thd_cache_name,
                                        sizeof(struct xwup_thd),
                                        XWMM_ALIGNMENT,
                                        page_order,
                                        (ctor_f)xwup_thd_construct,
                                        (dtor_f)xwup_thd_destruct);
        return rc;
}
#elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
/**
 * @brief XWUP API：初始化结构体xwup_thd的对象缓存
 * @param[in] zone_origin: 内存区域的首地址
 * @param[in] zone_size: 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwup_init_code
xwer_t xwup_thd_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        xwer_t rc;

        rc = xwmm_memslice_init(&xwup_thd_cache, zone_origin, zone_size,
                                sizeof(struct xwup_thd),
                                xwup_thd_cache_name,
                                (ctor_f)xwup_thd_construct,
                                (dtor_f)xwup_thd_destruct);
        return rc;
}
#elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
__xwup_init_code
void xwup_thd_cache_init(struct xwmm_sma * sma)
{
        xwup_thd_cache = sma;
}
#endif


/**
 * @brief 线程对象的构造函数
 * @param[in] thd: 线程对象的指针
 */
static __xwup_code
void xwup_thd_construct(struct xwup_thd * thd)
{
        xwos_object_construct(&thd->xwobj);
}

/**
 * @brief 线程对象的析构函数
 * @param[in] thd: 线程对象的指针
 */
static __xwup_code
void xwup_thd_destruct(struct xwup_thd * thd)
{
        xwos_object_destruct(&thd->xwobj);
}

static __xwup_code
xwer_t xwup_thd_sgc(struct xwos_object * obj)
{
        struct xwup_thd * thd;

        thd = xwcc_derof(obj, struct xwup_thd, xwobj);
        xwup_thd_destruct(thd);
        return XWOK;
}

#if (1 == XWOSRULE_SKD_THD_CREATE)
static __xwup_code
xwer_t xwup_thd_dgc(struct xwos_object * obj)
{
        xwstk_t * base;
        struct xwup_thd * thd;

        thd = xwcc_derof(obj, struct xwup_thd, xwobj);
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_FLAG_ALLOCATED_STACK & thd->stack.flag)) {
                base = ((struct xwup_thd *)thd)->stack.base;
                xwup_thd_stack_free(base); //cppcheck-suppress [misra-c2012-17.7]
        }
        xwup_thd_destruct(thd);
        xwup_thd_free(thd);
        return XWOK;
}
#endif

__xwup_api
xwer_t xwup_thd_acquire(struct xwup_thd * thd, xwsq_t tik)
{
        return xwos_object_acquire(&thd->xwobj, tik);
}

__xwup_api
xwer_t xwup_thd_release(struct xwup_thd * thd, xwsq_t tik)
{
        return xwos_object_release(&thd->xwobj, tik);
}

__xwup_api
xwer_t xwup_thd_grab(struct xwup_thd * thd)
{
        return xwos_object_grab(&thd->xwobj);
}

__xwup_api
xwer_t xwup_thd_put(struct xwup_thd * thd)
{
        return xwos_object_put(&thd->xwobj);
}

#if (1 == XWOSRULE_SKD_THD_CREATE)
/**
 * @brief 从线程对象缓存中申请一个对象
 * @return 线程对象的指针
 */
static __xwup_code
struct xwup_thd * xwup_thd_alloc(void)
{
#  if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
        union {
                struct xwup_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_objcache_alloc(&xwup_thd_cache, &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(rc);
        }
        return mem.thd;
#  elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
        union {
                struct xwup_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(&xwup_thd_cache, &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(rc);
        }
        return mem.thd;
#  elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
        union {
                struct xwup_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_sma_alloc(xwup_thd_cache,
                            sizeof(struct xwup_thd), XWMM_ALIGNMENT,
                            &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(rc);
        } else {
                xwup_thd_construct(mem.thd);
        }
        return mem.thd;
#  elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
        struct xwup_thd * thd;

        thd = malloc(sizeof(struct xwup_thd));
        if (NULL == thd) {
                thd = err_ptr(-ENOMEM);
        } else {
                xwup_thd_construct(thd);
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
static __xwup_code
void xwup_thd_free(struct xwup_thd * thd)
{
#  if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
        xwmm_mempool_objcache_free(&xwup_thd_cache, thd);
#  elif defined(XWOSCFG_SKD_THD_MEMSLICE) && (1 == XWOSCFG_SKD_THD_MEMSLICE)
        xwmm_memslice_free(&xwup_thd_cache, thd);
#  elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
        xwmm_sma_free(xwup_thd_cache, thd);
#  elif defined(XWOSCFG_SKD_THD_STDC_MM) && (1 == XWOSCFG_SKD_THD_STDC_MM)
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
static __xwup_code
xwstk_t * xwup_thd_stack_alloc(xwsz_t stack_size)
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

        rc = xwmm_sma_alloc(xwup_thd_cache,
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
static __xwup_code
xwer_t xwup_thd_stack_free(xwstk_t * stk)
{
#  if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        return board_thd_stack_pool_free(stk);
#  elif defined(XWOSCFG_SKD_THD_SMA) && (1 == XWOSCFG_SKD_THD_SMA)
        return xwmm_sma_free(xwup_thd_cache, stk);
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
 * @brief 激活线程对象
 * @param[in] thd: 线程对象的指针
 * @param[in] attr: 线程属性
 * @param[in] thdfunc: 线程函数的指针
 * @param[in] arg: 线程函数的参数
 */
static __xwup_code
xwer_t xwup_thd_activate(struct xwup_thd * thd,
                         struct xwup_thd_attr * attr,
                         xwup_thd_f thdfunc, void * arg,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&thd->xwobj, gcfunc);
        if (rc < 0) {
                goto err_obj_activate;
        }

        thd->state = (xwsq_t)XWUP_SKDOBJ_ST_UNKNOWN;
        if (attr->privileged) {
                thd->stack.flag |= (xwsq_t)XWUP_SKDOBJ_FLAG_PRIVILEGED;
        }
        if (attr->detached) {
                thd->state |= (xwsq_t)XWUP_SKDOBJ_ST_DETACHED;
        }
#if (1 == XWOSRULE_SKD_THD_FREEZE)
        xwlib_bclst_init_node(&thd->frznode);
#endif
        xwlib_bclst_init_node(&thd->rqnode);
        xwup_ttn_init(&thd->ttn);
        xwup_wqn_init(&thd->wqn);
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
        xwup_mtxtree_init(&thd->mt);
#endif

        /* 优先级 */
        if (attr->priority > XWUP_SKD_PRIORITY_RT_MAX) {
                attr->priority = XWUP_SKD_PRIORITY_RT_MAX;
        } else if (attr->priority < XWUP_SKD_PRIORITY_RT_MIN) {
                attr->priority = XWUP_SKD_PRIORITY_RT_MIN;
        } else {}
        thd->prio.s = attr->priority;
        thd->prio.d = attr->priority;

        /* 栈信息 */
        thd->stack.name = attr->name;
        thd->stack.size = attr->stack_size;
        thd->stack.base = attr->stack;
#if defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK)
        thd->stack.sp =  &thd->stack.base[(attr->stack_size / sizeof(xwstk_t))];
        thd->stack.tls = &thd->stack.base[0];
#elif defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK)
        thd->stack.sp = &thd->stack.base[(attr->stack_size / sizeof(xwstk_t)) - 1U];
        thd->stack.tls = &thd->stack.base[0];
#elif defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK)
        thd->stack.sp = &thd->stack.base[-1];
        thd->stack.tls = &thd->stack.base[(attr->stack_size / sizeof(xwstk_t))];
#elif defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK)
        thd->stack.sp =  &thd->stack.base[0];
        thd->stack.tls = &thd->stack.base[(attr->stack_size / sizeof(xwstk_t))];
#else
#  error "Unknown stack type!"
#endif
        thd->stack.guard_base = thd->stack.tls;
        thd->stack.guard = attr->stack_guard_size;

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        xwup_cond_init(&thd->completion); //cppcheck-suppress [misra-c2012-17.7]
#endif
        xwlib_bclst_init_node(&thd->thdnode);
#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
        for (xwsq_t i = (xwsq_t)0; i < XWOSCFG_SKD_THD_LOCAL_DATA_NUM; i++) {
                thd->data[i] = NULL;
        }
#endif
        thd->libc.error_number = XWOK;

        if (NULL != thdfunc) {
                xwup_thd_launch(thd, thdfunc, arg);
#if defined(BRDCFG_XWSKD_THD_POSTINIT_HOOK) && (1 == BRDCFG_XWSKD_THD_POSTINIT_HOOK)
                board_thd_postinit_hook(thd);
#endif
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_STANDBY);
        }
        return XWOK;

err_obj_activate:
        return rc;
}

/**
 * @brief 加载线程
 * @param[in] thd: 线程对象的指针
 * @param[in] thdfunc: 线程主函数
 * @param[in] arg: 线程主函数的参数
 */
static __xwup_code
void xwup_thd_launch(struct xwup_thd * thd, xwup_thd_f thdfunc, void * arg)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        xwup_thd_grab(thd); // cppcheck-suppress [misra-c2012-17.7]
        xwskd = xwup_skd_get_lc();
        thd->stack.main = thdfunc;
        thd->stack.arg = arg;
        xwospl_skd_init_stack(&thd->stack, xwup_cthd_return);
        xwospl_tls_init(&thd->stack);
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_STANDBY);
        xwup_thd_rq_add_tail(thd); //cppcheck-suppress [misra-c2012-17.7]
        xwskd->thd_num++;
        xwlib_bclst_add_tail(&xwskd->thdlist, &thd->thdnode);
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwup_api
void xwup_thd_attr_init(struct xwup_thd_attr * attr)
{
        if (NULL != attr) {
                attr->name = NULL;
                attr->stack = NULL;
                attr->stack_size = XWMMCFG_STACK_SIZE_DEFAULT;
                attr->stack_guard_size = XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
                attr->priority = XWUP_SKD_PRIORITY_RT_MIN;
                attr->detached = false;
                attr->privileged = XWOSCFG_SKD_THD_PRIVILEGED_DEFAULT;
        }
}

__xwup_api
void xwup_thd_get_attr(struct xwup_thd * thd, struct xwup_thd_attr * attr)
{
        attr->name = thd->stack.name;
        attr->stack = thd->stack.base;
        attr->stack_size = thd->stack.size;
        attr->stack_guard_size = thd->stack.guard;
        attr->priority = thd->prio.s;
        attr->detached = !!(thd->state & (xwsq_t)XWUP_SKDOBJ_ST_DETACHED);
        attr->privileged = !!(thd->stack.flag & (xwsq_t)XWUP_SKDOBJ_FLAG_PRIVILEGED);
}

__xwup_api
xwer_t xwup_thd_init(struct xwup_thd * thd,
                     const struct xwup_thd_attr * inattr,
                     xwup_thd_f thdfunc, void * arg)
{
        xwer_t rc;
        struct xwup_thd_attr attr;

        xwup_thd_construct(thd);
        attr = *inattr;
        thd->stack.flag = (xwsq_t)0;
        rc = xwup_thd_activate(thd, &attr, thdfunc, arg, xwup_thd_sgc);
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
__xwup_api
xwer_t xwup_thd_create(struct xwup_thd ** thdpbuf,
                       const struct xwup_thd_attr * inattr,
                       xwup_thd_f thdfunc, void * arg)
{
        struct xwup_thd * thd;
        struct xwup_thd_attr attr;
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
                        attr.stack = xwup_thd_stack_alloc(attr.stack_size);
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
                attr.stack = xwup_thd_stack_alloc(attr.stack_size);
                if (is_err(attr.stack)) {
                        rc = ptr_err(attr.stack);
                        goto err_stack_alloc;
                }
                allocated_stack = true;
                attr.stack_guard_size = XWMMCFG_STACK_GUARD_SIZE_DEFAULT;
                attr.priority = XWUP_SKD_PRIORITY_RT_MIN;
                attr.detached = false;
                attr.privileged = XWOSCFG_SKD_THD_PRIVILEGED_DEFAULT;
        }

        thd = xwup_thd_alloc();
        if (is_err(thd)) {
                rc = ptr_err(thd);
                goto err_thd_alloc;
        }

        if (allocated_stack) {
                thd->stack.flag = (xwsq_t)XWUP_SKDOBJ_FLAG_ALLOCATED_STACK;
        } else {
                thd->stack.flag = 0;
        }

        rc = xwup_thd_activate(thd, &attr, thdfunc, arg, xwup_thd_dgc);
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
        xwup_thd_free(thd);
err_thd_alloc:
        if (allocated_stack) {
                xwup_thd_stack_free(attr.stack); //cppcheck-suppress [misra-c2012-17.7]
        }
err_stack_alloc:
        *thdpbuf = NULL;
        return rc;
}
#endif

/**
 * @brief 执行退出线程
 * @param[in] thd: 线程对象的指针
 * @param[in] rc: 线程的返回值
 */
__xwup_code
xwer_t xwup_thd_exit_lic(struct xwup_thd * thd, xwer_t rc)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwup_skd * xwskd;
        bool detached;
        xwreg_t cpuirq;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, ((xwsq_t)XWUP_SKDOBJ_ST_RUNNING |
                                        (xwsq_t)XWUP_SKDOBJ_ST_EXITING));
        xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_STANDBY);
        thd->stack.arg = err_ptr(rc);
        detached = !!((xwsq_t)XWUP_SKDOBJ_ST_DETACHED & thd->state);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_cond_broadcast(&thd->completion); //cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_disable_lc();
        xwlib_bclst_del_init(&thd->thdnode);
        xwskd->thd_num--;
        if (detached) {
                xwlib_bclst_add_tail(&xwskd->thdelist, &thd->thdnode);
        }
#  if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                xwup_skd_notify_allfrz_lic(); // cppcheck-suppress [misra-c2012-17.7]
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        }
#  else
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
#  endif
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
__xwup_code
void xwup_cthd_return(xwer_t rc)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        xwos_thdlogf(INFO, cthd, "return:%d\r\n", rc);
        xwospl_thd_exit_lc((struct xwospl_thd *)cthd, rc);
#else
        XWOS_UNUSED(rc);
        while (true) {
        }
#endif
}

__xwup_api
void xwup_cthd_exit(xwer_t rc)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        xwos_thdlogf(INFO, cthd, "exit:%d\r\n", rc);
        xwospl_thd_exit_lc((struct xwospl_thd *)cthd, rc);
#else
        XWOS_UNUSED(rc);
        while (true) {
        }
#endif
}

#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
__xwup_api
void xwup_thd_quit(struct xwup_thd * thd)
{
        xwreg_t cpuirq;

        xwospl_cpuirq_save_lc(&cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_STANDBY & thd->state)) {
                xwospl_cpuirq_restore_lc(cpuirq);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_EXITING);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_intr(thd); //cppcheck-suppress [misra-c2012-17.7]
        }
}

__xwup_api
xwer_t xwup_thd_join(struct xwup_thd * thd, xwer_t * trc)
{
        struct xwup_thd * cthd;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        cthd = xwup_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_join_self;
        }
        xwospl_cpuirq_save_lc(&cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_DETACHED & thd->state)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINVAL;
        } else if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_JOINED & thd->state)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EALREADY;
        } else if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_STANDBY & thd->state)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                xwup_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_JOINED);
                rc = xwup_cond_wait(&thd->completion,
                                    NULL, XWOS_LK_NONE, NULL,
                                    &lkst);
                if (XWOK == rc) {
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                } else {
                        xwospl_cpuirq_disable_lc();
                        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_JOINED);
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        }

err_join_self:
        return rc;
}

__xwup_api
xwer_t xwup_thd_detach(struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_JOINED & thd->state)) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINVAL;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_DETACHED);
                if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_STANDBY & thd->state)) {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_thd_put(thd); // cppcheck-suppress [misra-c2012-17.7]
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
                rc = XWOK;
        }
        return rc;
}

#else

__xwup_api
void xwup_thd_quit(struct xwup_thd * thd)
{
        XWOS_UNUSED(thd);
}

__xwup_api
xwer_t xwup_thd_join(struct xwup_thd * thd, xwer_t * trc)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}

__xwup_api
xwer_t xwup_thd_detach(struct xwup_thd * thd)
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
 * - 此函数将线程(thd)的优先级改成(dprio)，并返回接下来需要修改优先级的互斥锁的指针。
 */
__xwup_code
void xwup_thd_chprio_once(struct xwup_thd * thd, xwpr_t dprio,
                          struct xwup_mtx ** pmtx)
{
        if ((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_RUNNING |
                           (xwsq_t)XWUP_SKDOBJ_ST_FROZEN) & thd->state)) {
                thd->prio.d = dprio;
        } else if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_READY & thd->state)) {
                if (thd->prio.d != dprio) {
                        xwup_thd_rq_remove(thd);
                        thd->prio.d = dprio;
                        //cppcheck-suppress [misra-c2012-17.7]
                        xwup_thd_rq_add_tail(thd);
                }
        } else if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING & thd->state)) {
                if (thd->prio.d == dprio) {
                } else if ((xwu16_t)XWUP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_derof(thd->wqn.wq, struct xwup_mtx, rtwq);
                        //cppcheck-suppress [misra-c2012-17.7]
                        xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
                        thd->prio.d = dprio;
                        xwup_rtwq_add(&mtx->rtwq, &thd->wqn, dprio);
                        *pmtx = mtx;
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
                } else if ((xwu16_t)XWUP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_derof(thd->wqn.wq, struct xwup_rtsem, rtwq);
                        //cppcheck-suppress [misra-c2012-17.7]
                        xwup_rtwq_remove(&sem->rtwq, &thd->wqn);
                        thd->prio.d = dprio;
                        xwup_rtwq_add(&sem->rtwq, &thd->wqn, dprio);
                } else {
#endif
                        thd->prio.d = dprio;
                }
        } else {
                XWOS_BUG_ON((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_STANDBY & thd->state));
                thd->prio.d = dprio;
        }
}

/**
 * @brief 重新设定线程的动态优先级
 * @param[in] thd: 线程对象的指针
 */
__xwup_code
void xwup_thd_chprio(struct xwup_thd * thd)
{
        struct xwup_mtx * unused;
        struct xwup_mtxtree * mt;
        xwpr_t dprio;

        mt = &thd->mt;
        dprio = (thd->prio.s > mt->maxprio) ? thd->prio.s : mt->maxprio;
        xwup_thd_chprio_once(thd, dprio, &unused);
}
#endif

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param[in] thd: 线程对象的指针
 * @param[in] prio: 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 */
__xwup_code
xwer_t xwup_thd_rq_add_head(struct xwup_thd * thd)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        if ((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_RUNNING |
                           (xwsq_t)XWUP_SKDOBJ_ST_READY) & thd->state)) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_READY);
                xwup_rtrq_add_head(xwrtrq, thd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param[in] thd: 线程对象的指针
 * @param[in] prio: 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 */
__xwup_code
xwer_t xwup_thd_rq_add_tail(struct xwup_thd * thd)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        if ((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_RUNNING |
                           (xwsq_t)XWUP_SKDOBJ_ST_READY) & thd->state)) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_READY);
                xwup_rtrq_add_tail(xwrtrq, thd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程从就绪队列中删除
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 没有这个线程
 */
__xwup_code
void xwup_thd_rq_remove(struct xwup_thd * thd)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        xwup_rtrq_remove(xwrtrq, thd);
        xwbop_c0m(xwsq_t, &thd->state, (xwsq_t)XWUP_SKDOBJ_ST_READY);
}

__xwup_api
xwer_t xwup_thd_intr(struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_UNINTERRUPTED & thd->state)) {
                rc = -EPERM;
                xwospl_cpuirq_restore_lc(cpuirq);
        } else if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_BLOCKING & thd->state)) {
                xwup_wqn_f cb;

                if ((xwu16_t)XWUP_WQTYPE_NULL == thd->wqn.type) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = (xwu16_t)XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_INTR;
                        cb = thd->wqn.cb;
                        thd->wqn.cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(&thd->wqn);
                        rc = XWOK;
#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
                } else if ((xwu16_t)XWUP_WQTYPE_PLSEM == thd->wqn.type) {
                        struct xwup_plsem * sem;

                        sem = xwcc_derof(thd->wqn.wq, struct xwup_plsem, plwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_plsem_intr(sem, &thd->wqn);
#endif
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
                } else if ((xwu16_t)XWUP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_derof(thd->wqn.wq, struct xwup_rtsem, rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_rtsem_intr(sem, &thd->wqn);
#endif
#if defined(XWOSCFG_SYNC_COND) && (1 == XWOSCFG_SYNC_COND)
                } else if ((xwu16_t)XWUP_WQTYPE_COND == thd->wqn.type) {
                        struct xwup_cond * sem;

                        sem = xwcc_derof(thd->wqn.wq, struct xwup_cond, wq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_cond_intr(sem, &thd->wqn);
#endif
#if defined(XWOSCFG_LOCK_MTX) && (1 == XWOSCFG_LOCK_MTX)
                } else if ((xwu16_t)XWUP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_derof(thd->wqn.wq, struct xwup_mtx, rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_mtx_intr(mtx, thd);
#endif
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -EBUG;
                        XWOS_BUG();
                }
        } else if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_SLEEPING & thd->state)) {
                struct xwup_skd * xwskd;
                struct xwup_tt * xwtt;

                xwskd = xwup_skd_get_lc();
                xwtt = &xwskd->tt;
                xwup_sqlk_wr_lock(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_SLEEPING);
                        xwup_sqlk_wr_unlock(&xwtt->lock);
                        xwup_thd_wakeup(thd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_chkpmpt();
                } else {
                        xwup_sqlk_wr_unlock(&xwtt->lock);
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        } else {
                rc = -EINVAL;
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

/**
 * @brief 线程的时间树节点回调函数
 * @param[in] entry: 线程对象的地址
 */
static __xwup_code
void xwup_thd_ttn_callback(struct xwup_ttn * ttn)
{
        struct xwup_thd * thd;
        xwreg_t cpuirq;

        thd = xwcc_derof(ttn, struct xwup_thd, ttn);
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_SLEEPING);
        xwup_thd_wakeup(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
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
__xwup_code
xwer_t xwup_thd_tt_add_locked(struct xwup_thd * thd, struct xwup_tt * xwtt,
                              xwtm_t to, xwreg_t cpuirq)
{
        xwer_t rc;

        /* add to time tree */
        thd->ttn.wkup_xwtm = to;
        thd->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
        thd->ttn.xwtt = xwtt;
        thd->ttn.cb = xwup_thd_ttn_callback;
        rc = xwup_tt_add_locked(xwtt, &thd->ttn, cpuirq);
        return rc;
}

/**
 * @brief 线程的等待队列节点回调函数
 * @param[in] entry: 线程对象的地址
 */
static __xwup_code
void xwup_thd_wqn_callback(struct xwup_wqn * wqn)
{
        struct xwup_thd * thd;
        xwreg_t cpuirq;

        thd = xwcc_derof(wqn, struct xwup_thd, wqn);
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_ST_BLOCKING | XWUP_SKDOBJ_ST_UNINTERRUPTED);
        xwup_thd_wakeup(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_chkpmpt();
}

#if (1 == XWOSRULE_SKD_WQ_RT)
/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param[in] thd: 线程对象的指针
 * @param[in] xwrtwq: 实时（红黑树）等待队列
 * @param[in] type: 等待队列的类型
 */
__xwup_code
void xwup_thd_eq_rtwq(struct xwup_thd * thd, struct xwup_rtwq * xwrtwq,
                      xwu16_t type)
{
        thd->wqn.wq = xwrtwq;
        thd->wqn.type = type;
        thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_UNKNOWN;
        thd->wqn.cb = xwup_thd_wqn_callback;
        xwup_rtwq_add(xwrtwq, &thd->wqn, thd->prio.d);
}
#endif

#if (1 == XWOSRULE_SKD_WQ_PL)
/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param[in] thd: 线程对象的指针
 * @param[in] xwplwq: 管道（双循环链表）等待队列
 * @param[in] type: 等待队列的类型
 */
__xwup_code
void xwup_thd_eq_plwq(struct xwup_thd * thd, struct xwup_plwq * xwplwq,
                      xwu16_t type)
{
        thd->wqn.wq = xwplwq;
        thd->wqn.type = type;
        thd->wqn.reason = (xwu16_t)XWUP_WQN_REASON_UNKNOWN;
        thd->wqn.cb = xwup_thd_wqn_callback;
        xwup_plwq_add_tail(xwplwq, &thd->wqn);
}
#endif

__xwup_api
void xwup_cthd_yield(void)
{
        struct xwup_thd * cthd;
        xwreg_t cpuirq;

        cthd = xwup_skd_get_cthd_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_ST_RUNNING);
        xwup_thd_rq_add_tail(cthd); //cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_enable_lc();
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwup_api
xwer_t xwup_cthd_sleep_to(xwtm_t to)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_thd * cthd;
        xwtm_t now;
        xwer_t rc;
        xwreg_t cpuirq;

        if (!xwospl_cpuirq_test_lc()) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        if (!xwup_skd_tstth_lc()) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        if (!xwup_skd_tstpmpt_lc()) {
                rc = -EDISPMPT;
                goto err_dis;
        }
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        if (!xwup_skd_tstbh_lc()) {
                rc = -EDISBH;
                goto err_dis;
        }
#endif
        cthd = xwup_skd_get_cthd_lc();
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        now = xwup_syshwt_get_time(hwt);
        if (xwtm_cmp(to, now) < 0) {
                rc = -ETIMEDOUT;
                goto err_timedout;
        }
        if (xwtm_cmp(to, now) == 0) {
                rc = XWOK;
                goto err_timedout;
        }
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        /* 检查是否被中断 */
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        rc = xwup_skd_wakelock_lock();
        if (rc < 0) {
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 调度器处于休眠态，线程需要被冻结，返回-EINTR。*/
                rc = -EINTR;
                goto err_intr;
        }
#endif
        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_FROZEN) & cthd->state));
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_EXITING & cthd->state)) {
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                xwup_skd_wakelock_unlock(); //cppcheck-suppress [misra-c2012-17.7]
#endif
                rc = -EINTR;
                goto err_intr;
        }
        /* 设置线程的睡眠态 */
        xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &cthd->state, XWUP_SKDOBJ_ST_SLEEPING);
        //cppcheck-suppress [misra-c2012-17.7]
        xwup_thd_tt_add_locked(cthd, xwtt, to, cpuirq);
        xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_unlock(); //cppcheck-suppress [misra-c2012-17.7]
#endif
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if ((xwsq_t)XWUP_TTN_WKUPRS_TIMEDOUT == cthd->ttn.wkuprs) {
                rc = XWOK;
        } else if ((xwsq_t)XWUP_TTN_WKUPRS_INTR == cthd->ttn.wkuprs) {
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

__xwup_api
xwer_t xwup_cthd_sleep_from(xwtm_t * from, xwtm_t dur)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_thd * cthd;
        xwtm_t to;
        xwreg_t cpuirq;
        xwer_t rc;

        if (!xwospl_cpuirq_test_lc()) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        if (!xwup_skd_tstth_lc()) {
                rc = -EDISIRQ;
                goto err_dis;
        }
        if (!xwup_skd_tstpmpt_lc()) {
                rc = -EDISPMPT;
                goto err_dis;
        }
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        if (!xwup_skd_tstbh_lc()) {
                rc = -EDISBH;
                goto err_dis;
        }
#endif
        cthd = xwup_skd_get_cthd_lc();
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        to = xwtm_add_safely(*from, dur);
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        /* 检查是否被中断 */
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        rc = xwup_skd_wakelock_lock();
        if (rc < 0) {
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，
                   阻塞/睡眠函数将返回-EINTR。*/
                rc = -EINTR;
                goto err_intr;
        }
#endif
        XWOS_BUG_ON((xwsq_t)0 != (((xwsq_t)XWUP_SKDOBJ_ST_SLEEPING |
                                   (xwsq_t)XWUP_SKDOBJ_ST_READY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_STANDBY |
                                   (xwsq_t)XWUP_SKDOBJ_ST_FROZEN) & cthd->state));
        if ((xwsq_t)0 != ((xwsq_t)XWUP_SKDOBJ_ST_EXITING & cthd->state)) {
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
                xwup_skd_wakelock_unlock(); //cppcheck-suppress [misra-c2012-17.7]
#endif
                rc = -EINTR;
                goto err_intr;
        }
        /* 设置线程的睡眠态 */
        xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &cthd->state, XWUP_SKDOBJ_ST_SLEEPING);
        //cppcheck-suppress [misra-c2012-17.7]
        xwup_thd_tt_add_locked(cthd, xwtt, to, cpuirq);
        xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWOSCFG_SKD_PM) && (1 == XWOSCFG_SKD_PM)
        xwup_skd_wakelock_unlock(); //cppcheck-suppress [misra-c2012-17.7]
#endif
        xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        if ((xwsq_t)XWUP_TTN_WKUPRS_TIMEDOUT == cthd->ttn.wkuprs) {
                rc = XWOK;
        } else if ((xwsq_t)XWUP_TTN_WKUPRS_INTR == cthd->ttn.wkuprs) {
                rc = -EINTR;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }

err_intr:
        *from = xwup_syshwt_get_time(hwt);
err_dis:
        return rc;
}

__xwup_api
xwer_t xwup_cthd_freeze(void)
{
#if (1 == XWOSRULE_SKD_THD_FREEZE)
        struct xwup_thd * cthd;
        xwer_t rc;

        xwup_skd_dspmpt_lc(); //cppcheck-suppress [misra-c2012-17.7]
        cthd = xwup_skd_get_cthd_lc();
        rc = xwospl_thd_freeze_lc(cthd);
        return rc;
#else
        return -ENOSYS;
#endif
}

/**
 * @brief 冻结线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在中断上下文中调用并且这个中断只能由 @ref xwup_cthd_freeze() 触发。
 */
__xwup_code
xwer_t xwup_thd_freeze_lic(struct xwup_thd * thd)
{
#if (1 == XWOSRULE_SKD_THD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        XWOS_BUG_ON((xwsq_t)0 == ((xwsq_t)XWUP_SKDOBJ_ST_RUNNING & thd->state));
        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_FROZEN);
        xwskd->pm.frz_thd_cnt++;
        xwlib_bclst_add_tail(&xwskd->pm.frzlist, &thd->frznode);
        xwup_skd_enpmpt_lc(); //cppcheck-suppress [misra-c2012-17.7]
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                // cppcheck-suppress [misra-c2012-17.7]
                xwup_skd_notify_allfrz_lic();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_req_swcx(); // cppcheck-suppress [misra-c2012-17.7]
        }
        return XWOK;
#else
        XWOS_UNUSED(thd);
        return -ENOSYS;
#endif
}

/**
 * @brief 解冻线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 */
__xwup_code
xwer_t xwup_thd_thaw_lic(struct xwup_thd * thd)
{
#if (1 == XWOSRULE_SKD_THD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        XWOS_BUG_ON((xwsq_t)0 == ((xwsq_t)XWUP_SKDOBJ_ST_FROZEN & thd->state));
        xwlib_bclst_del_init(&thd->frznode);
        xwskd->pm.frz_thd_cnt--;
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_ST_FROZEN);
        rc = xwup_thd_rq_add_tail(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
#else
        XWOS_UNUSED(thd);
        return -ENOSYS;
#endif
}

__xwup_api
bool xwup_cthd_shld_frz(void)
{
#if (1 == XWOSRULE_SKD_THD_FREEZE)
        return !!(xwup_skd_get_pm_stage() < (xwsq_t)XWUP_PM_STAGE_RUNNING);
#else
        return false;
#endif
}

__xwup_api
bool xwup_cthd_shld_stop(void)
{
#if defined(XWOSCFG_SKD_THD_EXIT) && (1 == XWOSCFG_SKD_THD_EXIT)
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return !!((xwsq_t)XWUP_SKDOBJ_ST_EXITING & cthd->state);
#else
        return false;
#endif
}

__xwup_api
bool xwup_cthd_frz_shld_stop(bool * frozen)
{
#if (1 == XWOSRULE_SKD_THD_FREEZE)
        bool frz;
        xwer_t rc;

        frz = false;
        if (xwup_cthd_shld_frz()) {
                rc = xwup_cthd_freeze();
                if (XWOK == rc) {
                        frz = true;
                }
        }
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }
#else
        if (!(is_err_or_null(frozen))) {
                *frozen = false;
        }
#endif
        return xwup_cthd_shld_stop();
}

#if defined(XWOSCFG_SKD_THD_LOCAL_DATA_NUM) && (XWOSCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
__xwup_api
xwer_t xwup_thd_set_data(struct xwup_thd * thd, xwsq_t pos, void * data)
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

__xwup_api
xwer_t xwup_thd_get_data(struct xwup_thd * thd, xwsq_t pos, void ** databuf)
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

__xwup_api
xwer_t xwup_cthd_set_data(xwsq_t pos, void * data)
{
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return xwup_thd_set_data(cthd, pos, data);
}

__xwup_api
xwer_t xwup_cthd_get_data(xwsq_t pos, void ** databuf)
{
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return xwup_thd_get_data(cthd, pos, databuf);

}
#endif
