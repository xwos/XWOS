/**
 * @file
 * @brief 玄武OS MP内核：线程
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_SKD_THD_MEMSLICE) && (1 == XWMPCFG_SKD_THD_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWMPCFG_SKD_THD_MEMSLICE */
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/mp/rtrq.h>
#include <xwos/mp/rtwq.h>
#include <xwos/mp/plwq.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/sync/cond.h>
#include <xwos/mp/sync/sem.h>
#include <xwos/mp/mtxtree.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/lock/seqlock.h>
#include <xwos/mp/lock/mtx.h>
#include <xwos/mp/thd.h>

extern __xwmp_code
xwer_t bdl_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);

extern __xwmp_code
xwer_t bdl_thd_stack_pool_free(xwstk_t * stk);

static __xwmp_code
struct xwmp_thd * xwmp_thd_alloc(void);

static __xwmp_code
void xwmp_thd_free(struct xwmp_thd * thd);

static __xwmp_code
xwstk_t * xwmp_thd_stack_alloc(xwsz_t stack_size);

static __xwmp_code
xwer_t xwmp_thd_stack_free(xwstk_t * stk);

static __xwmp_code
void xwmp_thd_construct(struct xwmp_thd * thd);

static __xwmp_code
void xwmp_thd_destruct(struct xwmp_thd * thd);

static __xwmp_code
xwer_t xwmp_thd_gc(void * thd);

static __xwmp_code
xwer_t xwmp_thd_activate(struct xwmp_thd * thd,
                         const char * name,
                         xwmp_thd_f mainfunc, void * arg,
                         xwstk_t * stack, xwsz_t stack_size,
                         xwpr_t priority, xwsq_t attr,
                         xwobj_gc_f gcfunc);

static __xwmp_code
void xwmp_thd_launch(struct xwmp_thd * thd, xwmp_thd_f mainfunc, void * arg);

static __xwmp_code
xwer_t xwmp_thd_stop_unlock_cb(struct xwmp_thd * thd);

static __xwmp_code
xwer_t xwmp_thd_stop_lock_cb(struct xwmp_thd * thd);

static __xwmp_code
void xwmp_thd_outmigrate_frozen_lic(struct xwmp_thd * thd);

static __xwmp_code
xwer_t xwmp_thd_outmigrate_reqfrz_lic(struct xwmp_thd * thd, xwid_t dstcpu);

#if defined(XWMPCFG_SKD_THD_MEMSLICE) && (1 == XWMPCFG_SKD_THD_MEMSLICE)
/**
 * @brief 结构体xwmp_thd的对象缓存
 */
__xwmp_data static struct xwmm_memslice * xwmp_thd_cache = NULL;

/**
 * @brief 结构体xwmp_thd的对象缓存的名字
 */
__xwmp_rodata const char xwmp_thd_cache_name[] = "xwmp.mp.thd.cache";
#endif /* XWMPCFG_SKD_THD_MEMSLICE */

#if defined(XWMPCFG_SKD_THD_MEMSLICE) && (1 == XWMPCFG_SKD_THD_MEMSLICE)
/**
 * @brief XWMP API：初始化结构体xwmp_thd的对象缓存
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_thd_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwmp_thd),
                                  xwmp_thd_cache_name,
                                  (ctor_f)xwmp_thd_construct,
                                  (dtor_f)xwmp_thd_destruct);
        if (__xwcc_unlikely(rc < 0)) {
                xwmp_thd_cache = NULL;
        } else {
                xwmp_thd_cache = msa;
        }
        return rc;
}
#endif /* XWMPCFG_SKD_THD_MEMSLICE */

/**
 * @brief 从线程对象缓存中申请一个对象
 * @return 线程对象的指针
 */
static __xwmp_code
struct xwmp_thd * xwmp_thd_alloc(void)
{
#if defined(XWMPCFG_SKD_THD_MEMSLICE) && (1 == XWMPCFG_SKD_THD_MEMSLICE)
        union {
                struct xwmp_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwmp_thd_cache, &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(rc);
        }/* else {} */
        return mem.thd;
#else /* XWMPCFG_SKD_THD_MEMSLICE */
        union {
                struct xwmp_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmp_thd), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmp_thd_construct(mem.thd);
        } else {
                mem.thd = err_ptr(-ENOMEM);
        }
        return mem.thd;
#endif /* !XWMPCFG_SKD_THD_MEMSLICE */
}

/**
 * @brief 释放线程对象
 * @param thd: (I) 线程对象的指针
 */
static __xwmp_code
void xwmp_thd_free(struct xwmp_thd * thd)
{
#if defined(XWMPCFG_SKD_THD_MEMSLICE) && (1 == XWMPCFG_SKD_THD_MEMSLICE)
        xwmm_memslice_free(xwmp_thd_cache, thd);
#else /* XWMPCFG_SKD_THD_MEMSLICE */
        xwmp_thd_destruct(thd);
        xwmm_kma_free(thd);
#endif /* !XWMPCFG_SKD_THD_MEMSLICE */
}

/**
 * @brief 动态分配线程栈
 * @return 线程栈的首地址或指针类型的错误码或空指针
 */
static __xwmp_code
xwstk_t * xwmp_thd_stack_alloc(xwsz_t stack_size)
{
#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        union {
                xwstk_t * stkbase;
                void * anon;
        } mem;
        xwer_t rc;

        rc = bdl_thd_stack_pool_alloc(stack_size, &mem.stkbase);
        if (rc < 0) {
                mem.stkbase = err_ptr(rc);
        }/* else {} */
        return mem.stkbase;
#else /* BRDCFG_XWSKD_THD_STACK_POOL */
        union {
                xwstk_t * stkbase;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(stack_size, XWMMCFG_STACK_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.stkbase = err_ptr(rc);
        }/* else {} */
        return mem.stkbase;
#endif /* !BRDCFG_XWSKD_THD_STACK_POOL */
}

/**
 * @brief 释放动态分配的线程栈内存
 * @param stk: (I) 线程栈的首地址
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_thd_stack_free(xwstk_t * stk)
{
#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        return bdl_thd_stack_pool_free(stk);
#else /* BRDCFG_XWSKD_THD_STACK_POOL */
        return xwmm_kma_free(stk);
#endif /* !BRDCFG_XWSKD_THD_STACK_POOL */
}

/**
 * @brief 线程对象的构造函数
 * @param thd: (I) 线程对象的指针
 */
static __xwmp_code
void xwmp_thd_construct(struct xwmp_thd * thd)
{
        xwos_object_construct(&thd->xwobj);
}

/**
 * @brief 线程对象的析构函数
 * @param thd: (I) 线程对象的指针
 */
static __xwmp_code
void xwmp_thd_destruct(struct xwmp_thd * thd)
{
        xwos_object_destruct(&thd->xwobj);
}

/**
 * @brief 线程对象的垃圾回收函数
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_thd_gc(void * thd)
{
        xwstk_t * base;

        base = ((struct xwmp_thd *)thd)->stack.base;
        xwmp_thd_stack_free(base);
        xwmp_thd_free(thd);
        return XWOK;
}

/**
 * @brief XWMP API：检查线程对象的标签并增加引用计数
 * @param thd: (I) 线程对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_thd_acquire(struct xwmp_thd * thd, xwsq_t tik)
{
        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        return xwos_object_acquire(&thd->xwobj, tik);
}

/**
 * @brief XWMP API：检查线程对象的标签并增加引用计数
 * @param thd: (I) 线程对象指针
 * @param tik: (I) 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_thd_release(struct xwmp_thd * thd, xwsq_t tik)
{
        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        return xwos_object_release(&thd->xwobj, tik);
}

/**
 * @brief XWMP API：增加对象的引用计数
 * @param thd: (I) 线程对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_thd_grab(struct xwmp_thd * thd)
{
        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        return xwos_object_grab(&thd->xwobj);
}

/**
 * @brief XWMP API：减少对象的引用计数
 * @param thd: (I) 线程对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_thd_put(struct xwmp_thd * thd)
{
        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        return xwos_object_put(&thd->xwobj);
}

/**
 * @brief 激活线程对象
 * @param thd: (I) 线程对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈首地址的指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，@ref xwmp_skdobj_attr_em
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非thd对象的引用计数重新为0
 * @note
 * - 在简单应用中，任务线程不会退出，也不会使用动态内存管理，
 *   因此设置gcfunc为NULL，不做垃圾回收处理。
 */
static __xwmp_code
xwer_t xwmp_thd_activate(struct xwmp_thd * thd,
                         const char * name,
                         xwmp_thd_f mainfunc, void * arg,
                         xwstk_t * stack, xwsz_t stack_size,
                         xwpr_t priority, xwsq_t attr,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = xwos_object_activate(&thd->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_activate;
        }
        /* base info */
        xwmp_splk_init(&thd->stlock);
        thd->state = XWMP_SKDOBJ_DST_UNKNOWN;
        thd->attribute = attr;
        if (XWMP_SKDATTR_DETACHED & attr) {
                thd->state |= XWMP_SKDOBJ_DST_DETACHED;
        }

        /* frozen state info */
        xwlib_bclst_init_node(&thd->frznode);

        /* migrating state info */
        thd->migration.dst = 0;

        /* ready state info */
        xwlib_bclst_init_node(&thd->rqnode);

        /* sleeping state info */
        xwmp_ttn_init(&thd->ttn, (xwptr_t)thd, XWMP_TTN_TYPE_THD);

        /* blocking state info */
        xwmp_wqn_init(&thd->wqn, thd);

        /* mutex tree */
        xwmp_mtxtree_init(&thd->mtxtree);

        /* priority */
        if (priority >= XWMP_SKD_PRIORITY_RT_NUM) {
                priority = XWMP_SKD_PRIORITY_RT_MAX;
        } else if (priority <= XWMP_SKD_PRIORITY_INVALID) {
                priority = XWMP_SKD_PRIORITY_RT_MIN;
        }
        thd->sprio = priority;
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.rq = XWMP_SKD_PRIORITY_INVALID;
        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;

        thd->stack.name = name;
        stack_size = (stack_size + XWMM_ALIGNMENT_MASK) & (~XWMM_ALIGNMENT_MASK);
        thd->stack.size = stack_size;
        thd->stack.base = stack;
#if (defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK))
        thd->stack.sp = thd->stack.base + (stack_size / sizeof(xwstk_t));
#elif (defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK))
        thd->stack.sp = thd->stack.base + (stack_size / sizeof(xwstk_t)) - 1;
#elif (defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK))
        thd->stack.sp = thd->stack.base - 1;
#elif (defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK))
        thd->stack.sp = thd->stack.base;
#else
#error "Unknown stack type!"
#endif
        /* init completion */
        xwmp_cond_init(&thd->completion);

        /* init thd node */
        xwlib_bclst_init_node(&thd->thdnode);

        /* add to ready queue */
        if (mainfunc) {
                xwmp_thd_launch(thd, mainfunc, arg);
        } else {
                xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_STANDBY);
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        }
        return XWOK;

err_obj_activate:
        return rc;
}

/**
 * @brief 加载就绪态的线程对象
 * @param thd: (I) 线程对象的指针
 * @param mainfunc: (I) 线程主函数
 * @param arg: (I) 线程主函数的参数
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
void xwmp_thd_launch(struct xwmp_thd * thd, xwmp_thd_f mainfunc, void * arg)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        /* add to ready queue */
        rc = xwmp_thd_grab(thd);
        XWOS_BUG_ON(rc < 0);
        /* Add to the thdlist of scheduler */
        xwskd = xwmp_skd_get_lc();
        xwmp_splk_lock_cpuirqsv(&xwskd->thdlistlock, &cpuirq);
        xwskd->thd_num++;
        xwlib_bclst_add_tail(&xwskd->thdlist, &thd->thdnode);
        xwmb_mp_store_release(struct xwmp_skd *, &thd->xwskd, xwskd);
        xwmp_splk_unlock_cpuirqrs(&xwskd->thdlistlock, cpuirq);
        thd->stack.main = mainfunc;
        thd->stack.arg = arg;
        xwospl_skd_init_stack(&thd->stack, xwmp_cthd_exit, thd->attribute);
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_STANDBY);
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);

        /* Add to ready queue */
        rc = xwmp_thd_rq_add_tail(thd, thd->sprio);
        XWOS_BUG_ON(rc < 0);
}

/**
 * @brief XWMP API：静态初始化线程对象
 * @param thd: (I) 线程对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈的首地址指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，取值：@ref xwmp_skdobj_attr_em中的一项
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非thd对象的引用计数重新为0
 * @note
 * - 静态初始化线程需预先定义线程对象和线程栈数组，通常定义为全局变量；
 * - 栈数组的首地址与大小，必须要满足CPU的ABI规则，例如ARM，就需要8字节对齐，
 *   因此在定义栈数组时需要__xwcc_aligned(8)来修饰，且大小是8的倍数。
 */
__xwmp_api
xwer_t xwmp_thd_init(struct xwmp_thd * thd,
                     const char * name,
                     xwmp_thd_f mainfunc, void * arg,
                     xwstk_t * stack, xwsz_t stack_size,
                     xwpr_t priority, xwsq_t attr)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stack), "nullptr", -EFAULT);

        xwmp_thd_construct(thd);
        rc = xwmp_thd_activate(thd,
                               name,
                               mainfunc, arg,
                               stack, stack_size,
                               priority, attr,
                               NULL);
        return rc;
}

/**
 * @brief XWMP API：销毁静态方式初始化的线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 静态方式初始化线程因为没有垃圾回收函数，在线程主函数退出后，需要
 *   由用户将其销毁，释放资源。
 */
__xwmp_api
xwer_t xwmp_thd_destroy(struct xwmp_thd * thd)
{
        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        return xwmp_thd_put(thd);
}

/**
 * @brief XWMP API：动态创建线程
 * @param thdpbuf: (O) 指向缓冲区的指针，通过此缓冲区返回线程对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，@ref xwmp_skdobj_attr_em
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 动态创建线程采用@ref xwmp_thd_gc()作为其对象的垃圾回收函数，当线程
 *   控制块对象引用计数为0时，系统会自动调用垃圾回收函数回收其资源。
 */
__xwmp_api
xwer_t xwmp_thd_create(struct xwmp_thd ** thdpbuf,
                       const char * name,
                       xwmp_thd_f mainfunc, void * arg,
                       xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        struct xwmp_thd * thd;
        xwstk_t * stk;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thdpbuf), "nullptr", -EFAULT);

        stk = xwmp_thd_stack_alloc(stack_size);
        if (__xwcc_unlikely(is_err(stk))) {
                rc = ptr_err(stk);
                goto err_stack_alloc;
        }

        thd = xwmp_thd_alloc();
        if (is_err(thd)) {
                rc = ptr_err(thd);
                goto err_thd_alloc;
        }

        rc = xwmp_thd_activate(thd,
                               name,
                               mainfunc, arg,
                               stk, stack_size,
                               priority, attr,
                               xwmp_thd_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_thd_activate;
        }

        *thdpbuf = thd;
        return XWOK;

err_thd_activate:
        xwmp_thd_free(thd);
err_thd_alloc:
        xwmp_thd_stack_free(stk);
err_stack_alloc:
        *thdpbuf = NULL;
        return rc;
}

/**
 * @brief XWMP API：删除动态创建的线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_thd_delete(struct xwmp_thd * thd)
{
        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        return xwmp_thd_put(thd);
}

/**
 * @brief 执行退出线程的函数
 * @param thd: (I) 线程对象的指针
 * @param rc: (I) 线程的返回值
 */
__xwmp_code
xwer_t xwmp_thd_exit_lic(struct xwmp_thd * thd, xwer_t rc)
{
        struct xwmp_skd * xwskd;
        xwreg_t cpuirq;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state,
                  (XWMP_SKDOBJ_DST_RUNNING | XWMP_SKDOBJ_DST_EXITING));
        thd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_STANDBY);
        thd->stack.arg = err_ptr(rc);
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        xwmp_cond_broadcast(&thd->completion);
        xwmp_splk_lock_cpuirq(&xwskd->pm.lock);
        xwmp_splk_lock(&xwskd->thdlistlock);
        xwlib_bclst_del_init(&thd->thdnode);
        xwskd->thd_num--;
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwmp_splk_unlock(&xwskd->thdlistlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                xwmp_thd_put(thd);
                if (XWMP_SKDATTR_DETACHED & thd->attribute) {
                        xwmp_thd_delete(thd);
                }
                xwmp_skd_notify_allfrz_lic(xwskd);
        } else {
                xwmp_splk_unlock(&xwskd->thdlistlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                xwmp_thd_put(thd);
                if (XWMP_SKDATTR_DETACHED & thd->attribute) {
                        xwmp_thd_delete(thd);
                }
                xwmp_skd_req_swcx(xwskd);
        }
        return XWOK;
}

/**
 * @brief XWMP API：当前线程退出
 * @param rc: (I) 线程退出时的返回值
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 调用的线程立即退出并抛出返回值，类似于POSIX线程库中的pthread_exit()函数。
 */
__xwmp_api
void xwmp_cthd_exit(xwer_t rc)
{
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        xwospl_thd_exit_lc((struct xwospl_thd *)cthd, rc);
}

/**
 * @brief @ref xwmp_thd_stop()中使用的回调锁的解锁函数
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_thd_stop_unlock_cb(struct xwmp_thd * thd)
{
        xwmp_splk_unlock(&thd->stlock);
        xwmp_thd_intr(thd);
        return XWOK;
}

/**
 * @brief @ref xwmp_thd_stop()中使用的回调锁的上锁函数
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_thd_stop_lock_cb(struct xwmp_thd * thd)
{
        xwmp_splk_lock(&thd->stlock);
        return XWOK;
}

/**
 * @brief XWMP API：终止线程并等待它的返回值，最后回收线程资源
 * @param thd: (I) 线程对象的指针
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回子线程的返回值，
 *                 可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 线程不是Joinable的
 * @retval -EPERM: 线程不可stop自己
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数由父线程调用，用于终止Joinable的子线程，父线程会一直阻塞等待子线程退出，
 *   并获取子线程的返回值，最后释放子线程资源，此函数类似于POSIX线程库
 *   中的pthread_cancel() + pthread_join()组合；
 * - 子线程收到终止信号后，并不会立即退出，退出的时机由子线程自己控制；
 * - 不可对Detached态的线程使用此函数。
 */
__xwmp_api
xwer_t xwmp_thd_stop(struct xwmp_thd * thd, xwer_t * trc)
{
        struct xwmp_thd * cthd;
        struct xwos_cblk lockcb;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        cthd = xwmp_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_stop_self;
        }
        rc = xwmp_thd_grab(thd);
        if (rc < 0) {
                goto err_thd_grab;
        }
        lockcb.unlock = (xwer_t (*)(void *))xwmp_thd_stop_unlock_cb;
        lockcb.lock = (xwer_t (*)(void *))xwmp_thd_stop_lock_cb;
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if (XWMP_SKDOBJ_DST_DETACHED & thd->state) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                xwmp_thd_put(thd);
                rc = -EINVAL;
        } else if (XWMP_SKDOBJ_DST_STANDBY & thd->state) {
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_DETACHED);
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                xwmp_thd_put(thd);
                rc = xwmp_thd_delete(thd);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_EXITING);
                rc = xwmp_cond_wait(&thd->completion,
                                    &lockcb, XWOS_LK_CALLBACK, thd,
                                    &lkst);
                if (XWOK == rc) {
                        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_DETACHED);
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                } else {
                        xwmp_cpuirq_restore_lc(cpuirq);
                }
                xwmp_thd_put(thd);
                if (XWOK == rc) {
                        rc = xwmp_thd_delete(thd);
                }
        }

err_thd_grab:
err_stop_self:
        return rc;
}

/**
 * @brief XWMP API：取消线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数功能类似于pthread_cancel()，通知子线程退出；
 * - 此函数可中断子线程的阻塞态与睡眠态；
 * - 此函数与xwmp_thd_stop()不同，不会阻塞调用者，也不会回收子线程资源，因此
 *   可在中断中调用。
 */
__xwmp_api
xwer_t xwmp_thd_cancel(struct xwmp_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        rc = xwmp_thd_grab(thd);
        if (rc < 0) {
                goto err_thd_grab;
        }
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if (XWMP_SKDOBJ_DST_STANDBY & thd->state) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = XWOK;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_EXITING);
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                xwmp_thd_intr(thd);
        }
        xwmp_thd_put(thd);

err_thd_grab:
        return rc;
}

/**
 * @brief XWMP API：等待线程结束并获取它的返回值，最后回收线程资源
 * @param thd: (I) 线程对象的指针
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回子线程的返回值，
 *                 可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 线程不是Joinable的
 * @retval -EPERM: 线程不可join自己
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数由父线程调用，父线程会一直阻塞等待子线程退出，
 *   并获取子线程的返回值，最后释放子线程资源，此函数类似于POSIX线程库
 *   pthread_join()函数；
 * - 不可对Detached态的线程使用此函数；
 * - 此函数与xwos_thd_stop()不同，只会等待子线程退出，不会通知子线程退出。
 */
__xwmp_api
xwer_t xwmp_thd_join(struct xwmp_thd * thd, xwer_t * trc)
{
        struct xwmp_thd * cthd;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        cthd = xwmp_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_join_self;
        }
        rc = xwmp_thd_grab(thd);
        if (rc < 0) {
                goto err_thd_grab;
        }
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if (XWMP_SKDOBJ_DST_DETACHED & thd->state) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                xwmp_thd_put(thd);
                rc = -EINVAL;
        } else if (XWMP_SKDOBJ_DST_STANDBY & thd->state) {
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_DETACHED);
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                xwmp_thd_put(thd);
                rc = xwmp_thd_delete(thd);
        } else {
                rc = xwmp_cond_wait(&thd->completion,
                                    &thd->stlock, XWOS_LK_SPLK, NULL,
                                    &lkst);
                if (XWOK == rc) {
                        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_DETACHED);
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                } else {
                        xwmp_cpuirq_restore_lc(cpuirq);
                }
                xwmp_thd_put(thd);
                if (XWOK == rc) {
                        rc = xwmp_thd_delete(thd);
                }
        }

err_thd_grab:
err_join_self:
        return rc;
}

/**
 * @brief XWMP API：分离线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数功能类似于pthread_detach()，处于分离态的线程退出后，系统自动回收资源，
 *   不需要父线程join()或stop()。
 */
__xwmp_api
xwer_t xwmp_thd_detach(struct xwmp_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        rc = xwmp_thd_grab(thd);
        if (rc < 0) {
                goto err_thd_grab;
        }
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_DETACHED);
        if (XWMP_SKDOBJ_DST_STANDBY & thd->state) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                xwmp_thd_put(thd);
                xwmp_thd_delete(thd);
        } else {
                thd->attribute |= XWMP_SKDATTR_DETACHED;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                xwmp_thd_put(thd);
        }

err_thd_grab:
        return rc;
}

/**
 * @brief 改变线程的动态优先级一次
 * @param thd: (I) 线程对象的指针
 * @param dprio: (I) 动态优先级
 * @param pmtx: (O) 指向缓冲区的指针，通过此缓冲区返回互斥锁对象的指针
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
        if ((XWMP_SKDOBJ_DST_RUNNING | XWMP_SKDOBJ_DST_FROZEN) & thd->state) {
                thd->dprio.r = dprio;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = XWOK;
        } else if (XWMP_SKDOBJ_DST_READY & thd->state) {
                if (thd->dprio.rq == dprio) {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        rc = XWOK;
                } else {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        rc = xwmp_thd_rq_remove(thd);
                        if (XWOK == rc) {
                                rc = xwmp_thd_rq_add_tail(thd, dprio);
                        }/* else {} */
                }
        } else if (XWMP_SKDOBJ_DST_BLOCKING & thd->state) {
                if (thd->dprio.wq == dprio) {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        rc = XWOK;
                } else {
                        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                        /* 因为锁的顺序此处必须解锁重新上锁，程序可能被中断打断，
                           所以需要重新检查条件。
                        */
                        xwmp_splk_lock_cpuirq(&thd->wqn.lock);
                        if (XWMP_WQTYPE_MTX == thd->wqn.type) {
                                struct xwmp_mtx * mtx;

                                mtx = xwcc_baseof(thd->wqn.wq,
                                                  struct xwmp_mtx,
                                                  rtwq);
                                xwmp_mtx_grab(mtx);
                                xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                                /* 因为锁的顺序此处必须解锁重新上锁，
                                   程序可能被中断打断，所以需要重新检查条件。
                                */
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
                                        xwmp_rtwq_remove_locked(&mtx->rtwq,
                                                                &thd->wqn);
                                        xwmp_splk_lock(&thd->stlock);
                                        thd->dprio.wq = dprio;
                                        xwmp_splk_unlock(&thd->stlock);
                                        xwmp_rtwq_add_locked(&mtx->rtwq,
                                                             &thd->wqn,
                                                             dprio);
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        *pmtx = mtx;
                                        rc = XWOK;
                                }
                                xwmp_mtx_put(mtx);
#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
                        } else if (XWMP_WQTYPE_RTSEM == thd->wqn.type) {
                                struct xwmp_sem * sem;

                                sem = xwcc_baseof(thd->wqn.wq,
                                                  struct xwmp_sem,
                                                  wq.rt);
                                xwmp_sem_grab(sem);
                                xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                                /* 因为锁的顺序此处必须解锁重新上锁，
                                   程序可能被中断打断，所以需要重新检查条件。
                                */
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
                                        xwmp_rtwq_remove_locked(&sem->wq.rt,
                                                                &thd->wqn);
                                        xwmp_splk_lock(&thd->stlock);
                                        thd->dprio.wq = dprio;
                                        xwmp_splk_unlock(&thd->stlock);
                                        xwmp_rtwq_add_locked(&sem->wq.rt,
                                                             &thd->wqn,
                                                             dprio);
                                        xwmp_splk_unlock(&thd->wqn.lock);
                                        xwmp_rtwq_unlock_cpuirqrs(&sem->wq.rt, cpuirq);
                                        rc = XWOK;
                                }
                                xwmp_sem_put(sem);
#endif /* XWMPCFG_SYNC_RTSEM */
                        } else {
                                xwmp_splk_lock(&thd->stlock);
                                thd->dprio.wq = dprio;
                                xwmp_splk_unlock(&thd->stlock);
                                xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                                rc = XWOK;
                        }
                }
        } else if (XWMP_SKDOBJ_DST_SLEEPING & thd->state) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                rc = XWOK;
        } else {
                XWOS_BUG_ON(XWMP_SKDOBJ_DST_STANDBY & thd->state);
                rc = -ESTALE;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        }
        return rc;
}

/**
 * @brief 重新设定线程的动态优先级
 * @param thd: (I) 线程对象的指针
 */
__xwmp_code
void xwmp_thd_chprio(struct xwmp_thd * thd)
{
        struct xwmp_mtxtree * mt;
        struct xwmp_mtx * tmp;
        xwpr_t dprio;
        xwsq_t seq;
        xwer_t rc;

        mt = &thd->mtxtree;
        do {
                seq = xwmp_sqlk_rd_begin(&mt->lock);
                dprio = thd->sprio > mt->maxprio ?
                        thd->sprio : mt->maxprio;
                rc = xwmp_thd_chprio_once(thd, dprio, &tmp);
        } while (xwmp_sqlk_rd_retry(&mt->lock, seq) || (rc < 0));
}

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param thd: (I) 线程对象的指针
 * @param prio: (I) 动态优先级
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
        if (((XWMP_SKDOBJ_DST_BLOCKING & thd->state) &&
             (XWMP_SKDOBJ_DST_SLEEPING & thd->state)) ||
            (XWMP_SKDOBJ_DST_FROZEN & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -ESTALE;
        } else if ((XWMP_SKDOBJ_DST_RUNNING | XWMP_SKDOBJ_DST_READY) & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EEXIST;
        } else {
                thd->dprio.rq = prio;
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_READY);
                xwmp_splk_unlock(&thd->stlock);
                rc = xwmp_rtrq_add_head_locked(xwrtrq, thd);
        }
        xwmp_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param thd: (I) 线程对象的指针
 * @param prio: (I) 动态优先级
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
        if (((XWMP_SKDOBJ_DST_BLOCKING & thd->state) &&
             (XWMP_SKDOBJ_DST_SLEEPING & thd->state)) ||
            (XWMP_SKDOBJ_DST_FROZEN & thd->state)) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -ESTALE;
        } else if ((XWMP_SKDOBJ_DST_RUNNING | XWMP_SKDOBJ_DST_READY) & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EEXIST;
        } else {
                thd->dprio.rq = prio;
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_READY);
                xwmp_splk_unlock(&thd->stlock);
                rc = xwmp_rtrq_add_tail_locked(xwrtrq, thd);
        }
        xwmp_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief 将线程从就绪队列中删除
 * @param thd: (I) 线程对象的指针
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
                xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_READY);
                thd->dprio.rq = XWMP_SKD_PRIORITY_INVALID;
                xwmp_splk_unlock(&thd->stlock);
        }
        xwmp_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief XWMP API：中断线程的睡眠或阻塞状态
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EPERM: 线程不可被中断
 * @retval -EINVAL: 无效的线程态
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_thd_intr(struct xwmp_thd * thd)
{
        xwmp_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        if (XWMP_SKDOBJ_DST_UNINTERRUPTED & thd->state) {
                rc = -EPERM;
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        } else if (XWMP_SKDOBJ_DST_BLOCKING & thd->state) {
                xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
                /* 上锁必须按照锁的顺序，否则会出现死锁。
                   一旦“解锁-重新上锁”以后就需要重新检查条件。 */
                xwmp_splk_lock_cpuirq(&thd->wqn.lock);
                if (XWMP_WQTYPE_NULL == thd->wqn.type) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWMP_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq, &thd->wqn.reason,
                                    xwmb_modr_release, XWMP_WQN_REASON_INTR);
                        cb = thd->wqn.cb;
                        thd->wqn.cb = NULL;
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        cb(thd);
                        rc = XWOK;
#if defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)
                } else if (XWMP_WQTYPE_PLSEM == thd->wqn.type) {
                        struct xwmp_sem * sem;

                        sem = xwcc_baseof(thd->wqn.wq, struct xwmp_sem, wq.pl);
                        xwmp_sem_grab(sem);
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_plsem_intr(sem, &thd->wqn);
                        xwmp_sem_put(sem);
#endif /* XWMPCFG_SYNC_PLSEM */
#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
                } else if (XWMP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwmp_sem * sem;

                        sem = xwcc_baseof(thd->wqn.wq, struct xwmp_sem, wq.rt);
                        xwmp_sem_grab(sem);
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_rtsem_intr(sem, &thd->wqn);
                        xwmp_sem_put(sem);
#endif /* XWMPCFG_SYNC_RTSEM */
                } else if (XWMP_WQTYPE_COND == thd->wqn.type) {
                        struct xwmp_cond * cond;

                        cond = xwcc_baseof(thd->wqn.wq, struct xwmp_cond, wq.pl);
                        xwmp_cond_grab(cond);
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_cond_intr(cond, &thd->wqn);
                        xwmp_cond_put(cond);
                } else if (XWMP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwmp_mtx * mtx;

                        mtx = xwcc_baseof(thd->wqn.wq, struct xwmp_mtx, rtwq);
                        xwmp_mtx_grab(mtx);
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        rc = xwmp_mtx_intr(mtx, thd);
                        xwmp_mtx_put(mtx);
                } else {
                        xwmp_splk_unlock_cpuirqrs(&thd->wqn.lock, cpuirq);
                        XWOS_BUG();
                        rc = -EBUG;
                }
        } else if (XWMP_SKDOBJ_DST_SLEEPING & thd->state) {
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
                        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwmp_thd_wakeup(thd);
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

/**
 * @brief 唤醒一个阻塞/睡眠态的线程，将其加入到调度器的就绪队列
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_thd_wakeup(struct xwmp_thd * thd)
{
        struct xwmp_mtxtree * mt;
        xwpr_t dprio;
        xwreg_t cpuirq;
        xwer_t rc;

        mt = &thd->mtxtree;
        xwmp_sqlk_rdex_lock_cpuirqsv(&mt->lock, &cpuirq);
        dprio = thd->sprio > mt->maxprio ? thd->sprio : mt->maxprio;
        rc = xwmp_thd_rq_add_tail(thd, dprio);
        xwmp_sqlk_rdex_unlock_cpuirqrs(&mt->lock, cpuirq);
        return rc;
}

/**
 * @brief 线程的时间树节点回调函数
 * @param entry: (I) 线程对象的地址
 */
__xwmp_code
void xwmp_thd_ttn_callback(void * entry)
{
        struct xwmp_thd * thd;
        xwreg_t cpuirq;

        thd = entry;
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_SLEEPING);
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        xwmp_thd_wakeup(thd);
}

/**
 * @brief 将线程加入到时间树上
 * @param thd: (I) 线程对象的指针
 * @param xwtt: (I) 时间树的指针
 * @param expected: (I) 期望被唤醒的时间
 * @param cpuirq: (I) 本地CPU的中断标志
 * @return 错误码
 * @note
 * - 此函数只能在取得写锁xwtt->lock以及关闭本地CPU的中断时才可调用。
 */
__xwmp_code
xwer_t xwmp_thd_tt_add_locked(struct xwmp_thd * thd, struct xwmp_tt * xwtt,
                              xwtm_t expected, xwreg_t cpuirq)
{
        xwer_t rc;

        /* add to time tree */
        thd->ttn.wkup_xwtm = expected;
        xwaop_store(xwsq, &thd->ttn.wkuprs,
                    xwmb_modr_release, XWMP_TTN_WKUPRS_UNKNOWN);
        thd->ttn.xwtt = xwtt;
        thd->ttn.cb = xwmp_thd_ttn_callback;
        rc = xwmp_tt_add_locked(xwtt, &thd->ttn, cpuirq);
        return rc;
}

/**
 * @brief 线程的等待队列节点回调函数
 * @param entry: (I) 线程对象的地址
 */
__xwmp_code
void xwmp_thd_wqn_callback(void * entry)
{
        struct xwmp_thd * thd;
        xwreg_t cpuirq;

        thd = entry;
        xwmp_splk_lock_cpuirqsv(&thd->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &thd->state,
                  XWMP_SKDOBJ_DST_BLOCKING | XWMP_SKDOBJ_DST_UNINTERRUPTED);
        thd->dprio.wq = XWMP_SKD_PRIORITY_INVALID;
        xwmp_splk_unlock_cpuirqrs(&thd->stlock, cpuirq);
        xwmp_thd_wakeup(thd);
        xwmp_skd_chkpmpt(thd->xwskd);
}

/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param thd: (I) 线程对象的指针
 * @param xwrtwq: (I) 实时（红黑树）等待队列
 * @param type: (I) 等待队列的类型
 * @param dprio: (I) 加入时线程的动态优先级
 */
__xwmp_code
void xwmp_thd_eq_rtwq_locked(struct xwmp_thd * thd,
                             struct xwmp_rtwq * xwrtwq,
                             xwu16_t type, xwpr_t dprio)
{
        xwmp_splk_lock(&thd->wqn.lock);
        thd->wqn.wq = xwrtwq;
        thd->wqn.type = type;
        xwaop_store(xwsq, &thd->wqn.reason,
                    xwmb_modr_release, XWMP_WQN_REASON_UNKNOWN);
        thd->wqn.cb = xwmp_thd_wqn_callback;
        xwmp_rtwq_add_locked(xwrtwq, &thd->wqn, dprio);
        xwmp_splk_unlock(&thd->wqn.lock);
}

/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param thd: (I) 线程对象的指针
 * @param xwplwq: (I) 管道（双循环链表）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwmp_code
void xwmp_thd_eq_plwq_locked(struct xwmp_thd * thd,
                             struct xwmp_plwq * xwplwq,
                             xwu16_t type)
{
        xwmp_splk_lock(&thd->wqn.lock);
        thd->wqn.wq = xwplwq;
        thd->wqn.type = type;
        xwaop_store(xwsq, &thd->wqn.reason,
                    xwmb_modr_release, XWMP_WQN_REASON_UNKNOWN);
        thd->wqn.cb = xwmp_thd_wqn_callback;
        xwmp_plwq_add_tail_locked(xwplwq, &thd->wqn);
        xwmp_splk_unlock(&thd->wqn.lock);
}

/**
 * @brief XWMP API：让出当前线程的CPU，调度器重新选择同优先级的其他线程运行
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数只能将CPU让给优先级与当前线程相同且已经就绪的其他线程。
 *   如果当前线程已经是系统中唯一的最高优先级线程，此函数无效。
 */
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
        xwbop_c0m(xwsq_t, &cthd->state, XWMP_SKDOBJ_DST_RUNNING);
        cthd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwmp_rawly_unlock_cpuirq(&cthd->stlock);
        rc = xwmp_thd_rq_add_tail(cthd, prio);
        XWOS_BUG_ON(rc < 0);
        xwmp_skd_req_swcx(cthd->xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief 解锁给定类型的锁
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_thd_do_unlock(void * lock, xwsq_t lktype, void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
        case XWOS_LK_MTX_UNINTR:
                rc = xwmp_mtx_unlock(ulk.xwmp.mtx);
                break;
        case XWOS_LK_SPLK:
                xwmp_splk_unlock(ulk.xwmp.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwmp_sqlk_wr_unlock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwmp_sqlk_rdex_unlock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (ulk.cb->unlock) {
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
 * @brief 锁定给定类型的锁
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_thd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm, void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
                if (xwtm) {
                        rc = xwmp_mtx_timedlock(ulk.xwmp.mtx, xwtm);
                } else {
                        rc = xwmp_mtx_lock(ulk.xwmp.mtx);
                }
                break;
        case XWOS_LK_MTX_UNINTR:
                rc = xwmp_mtx_lock_unintr(ulk.xwmp.mtx);
                break;
        case XWOS_LK_SPLK:
                xwmp_splk_lock(ulk.xwmp.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwmp_sqlk_wr_lock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwmp_sqlk_rdex_lock(ulk.xwmp.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (ulk.cb->lock) {
                        rc = ulk.cb->lock(lkdata);
                }
                break;
        case XWOS_LK_NONE:
        default:
                break;
        }
        return rc;
}

/**
 * @brief XWMP API：线程睡眠一段时间
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 睡眠过程被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * - 超时后将以返回值OK返回，并且 *xwtm* 指向缓冲区返回0。
 */
__xwmp_api
xwer_t xwmp_cthd_sleep(xwtm_t * xwtm)
{
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;
        struct xwmp_thd * cthd;
        xwtm_t expected, currtick;
        xwer_t rc;
        xwsq_t wkuprs;
        xwreg_t cpuirq;

        if (__xwcc_unlikely(xwtm_cmp(*xwtm, 0) <= 0)) {
                rc = -ETIMEDOUT;
                goto err_xwtm;
        }

        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        currtick = xwmp_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwmp_skd_wakelock_lock(xwskd);
        if (__xwcc_unlikely(rc < 0)) {
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，
                   阻塞/睡眠函数将返回-EINTR。*/
                rc = -EINTR;
                goto err_needfrz;
        }
        xwmp_splk_lock(&cthd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_DST_SLEEPING | XWMP_SKDOBJ_DST_READY |
                     XWMP_SKDOBJ_DST_STANDBY | XWMP_SKDOBJ_DST_FROZEN |
                     XWMP_SKDOBJ_DST_MIGRATING)
                    & cthd->state);
        if (XWMP_SKDOBJ_DST_FREEZABLE & cthd->state) {
                xwmp_splk_unlock(&cthd->stlock);
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                xwmp_skd_wakelock_unlock(xwskd);
                rc = -EINTR;
                goto err_needfrz;
        }
        /* set sleeping state */
        xwbop_c0m(xwsq_t, &cthd->state, XWMP_SKDOBJ_DST_RUNNING);
        cthd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &cthd->state, XWMP_SKDOBJ_DST_SLEEPING);
        xwmp_splk_unlock(&cthd->stlock);
        /* add to time tree */
        xwmp_thd_tt_add_locked(cthd, xwtt, expected, cpuirq);
        /* enable local CPU IRQ to enable schedule */
        xwmp_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        xwmp_skd_wakelock_unlock(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        wkuprs = xwaop_load(xwsq, &cthd->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                rc = -EINTR;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        currtick = xwmp_syshwt_get_timetick(hwt);
        *xwtm = xwtm_sub(expected, currtick);

err_needfrz:
err_xwtm:
        return rc;
}

/**
 * @brief XWMP API：当前线程从一个时间起点睡眠到另一个时间点
 * @param origin: 指向缓冲区的指针，此缓冲区：
 *                (I) 作为输入时，作为时间起点
 *                (O) 作为输出时，返回线程被唤醒的时间
 *                    （可作为下一次时间起点，形成精确的周期）
 * @param inc: (I) 期望被唤醒的时间增量（相对于时间原点）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 睡眠过程被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        struct xwmp_skd * xwskd;
        struct xwmp_tt * xwtt;
        struct xwmp_syshwt * hwt;
        struct xwmp_thd * cthd;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwsq_t wkuprs;
        xwer_t rc;

        cthd = xwmp_skd_get_cthd_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &cthd->xwskd);
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        expected = xwtm_add_safely(*origin, inc);
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwmp_skd_wakelock_lock(xwskd);
        if (__xwcc_unlikely(rc < 0)) {
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，
                   阻塞/睡眠函数将返回-EINTR。*/
                rc = -EINTR;
                goto err_needfrz;
        }
        xwmp_splk_lock(&cthd->stlock);
        XWOS_BUG_ON((XWMP_SKDOBJ_DST_SLEEPING | XWMP_SKDOBJ_DST_READY |
                     XWMP_SKDOBJ_DST_STANDBY | XWMP_SKDOBJ_DST_FROZEN |
                     XWMP_SKDOBJ_DST_MIGRATING)
                    & cthd->state);
        if (XWMP_SKDOBJ_DST_FREEZABLE & cthd->state) {
                xwmp_splk_unlock(&cthd->stlock);
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                xwmp_skd_wakelock_unlock(xwskd);
                rc = -EINTR;
                goto err_needfrz;
        }
        /* set the sleeping state */
        xwbop_c0m(xwsq_t, &cthd->state, XWMP_SKDOBJ_DST_RUNNING);
        cthd->dprio.r = XWMP_SKD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &cthd->state, XWMP_SKDOBJ_DST_SLEEPING);
        xwmp_splk_unlock(&cthd->stlock);
        /* add to time tree */
        xwmp_thd_tt_add_locked(cthd, xwtt, expected, cpuirq);
        /* enable local CPU IRQ to enable schedule */
        xwmp_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        xwmp_skd_wakelock_unlock(xwskd);
        xwmp_skd_req_swcx(xwskd);
        xwospl_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_mp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        wkuprs = xwaop_load(xwsq, &cthd->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWMP_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                rc = XWOK;
        } else if (XWMP_TTN_WKUPRS_INTR == wkuprs) {
                rc = -EINTR;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        *origin = xwmp_syshwt_get_timetick(hwt);

err_needfrz:
        return rc;
}

/**
 * @brief 申请冻结当前CPU中的线程
 * @param thd: (I) 线程对象的指针
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
        if (XWMP_SKDOBJ_DST_FROZEN & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EALREADY;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_FREEZABLE);
                xwmp_splk_unlock(&thd->stlock);
                rc = XWOK;
        }
        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
        return rc;
}

/**
 * @brief 执行冻结线程
 * @param thd: (I) 线程对象的指针
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

        XWOS_BUG_ON(!(XWMP_SKDOBJ_DST_RUNNING & thd->state));

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        srccpu = xwskd->id;
        xwmp_splk_lock_cpuirqsv(&xwskd->pm.lock, &cpuirq);
        xwmp_splk_lock(&thd->stlock);
        if (XWMP_SKDOBJ_DST_FREEZABLE & thd->state) {
                xwbop_c0m(xwsq_t, &thd->state,
                          XWMP_SKDOBJ_DST_RUNNING | XWMP_SKDOBJ_DST_FREEZABLE);
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_FROZEN);
                if (XWMP_SKDOBJ_DST_MIGRATING & thd->state) {
                        xwmp_splk_unlock(&thd->stlock);
                        xwmp_thd_outmigrate_frozen_lic(thd);
                        dstcpu = thd->migration.dst;
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        if (srccpu == dstcpu) {
                                xwmp_thd_immigrate_lic(thd);
                        } else {
                                xwospl_thd_immigrate((struct xwospl_thd *)thd, dstcpu);
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
                        xwmp_skd_enpmpt(xwskd);
                        xwmp_skd_notify_allfrz_lic(xwskd);
                } else {
                        xwmp_splk_unlock(&xwskd->thdlistlock);
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        xwmp_skd_enpmpt(xwskd);
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

/**
 * @brief XWMP API：线程自我冻结
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EPERM: 不允许冻结
 * @retval -ECANCELED: 冻结被取消
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 冻结操作，只可由线程自身执行
 */
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
        if (XWMP_SKDOBJ_DST_FREEZABLE & cthd->state) {
                xwmp_splk_unlock_cpuirqrs(&cthd->stlock, cpuirq);
                rc = xwospl_thd_freeze_lc((struct xwospl_thd *)cthd);
        } else {
                xwmp_splk_unlock_cpuirqrs(&cthd->stlock, cpuirq);
                xwmp_skd_enpmpt(xwskd);
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief 解冻线程
 * @param thd: (I) 线程对象的指针
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
        if (XWMP_SKDOBJ_DST_FREEZABLE & thd->state) {
                xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_FREEZABLE);
                xwmp_splk_unlock(&thd->stlock);
                rc = -ECANCELED;
        } else if (XWMP_SKDOBJ_DST_MIGRATING & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                rc = -EPERM;
        } else if (XWMP_SKDOBJ_DST_FROZEN & thd->state) {
                prio = thd->dprio.r;
                xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_FROZEN);
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

/**
 * @brief XWMP API：判断当前线程是否可以冻结
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmp_api
bool xwmp_cthd_shld_frz(void)
{
        struct xwmp_thd * cthd;
        bool ret;

        cthd = xwmp_skd_get_cthd_lc();
        ret = !!(cthd->state & XWMP_SKDOBJ_DST_FREEZABLE);
        return ret;
}

/**
 * @brief XWMP API：判断当前线程是否可以退出
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数常用在线程主循环的循环条件中。例如：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwmp_cthd_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 */
__xwmp_api
bool xwmp_cthd_shld_stop(void)
{
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        return !!(XWMP_SKDOBJ_DST_EXITING & cthd->state);
}

/**
 * @brief XWMP API：判断当前线程是否可以冻结，如果是就冻结线程，
 *                  之后再判断线程是否可以退出
 * @param frozen (O) 指向缓冲区的指针，通过此缓冲区返回线程是否被冻结过的状态
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数在@ref xwmp_cthd_shld_stop()的基础上增加了对冻结条件是否满足
 *   的判断，如果可以冻结，就在函数中将线程冻结。
 * - 通常线程冻结用于处理系统的低功耗事件或将线程在不同的CPU中进行迁移。
 * - 需要考虑线程的冻结，通常将线程的主循环写成：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwmp_cthd_frz_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 */
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
                }/* else {} */
        }/* else {} */
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }/* else {} */
        cthd = xwmp_skd_get_cthd_lc();
        return !!(XWMP_SKDOBJ_DST_EXITING & cthd->state);
}

/**
 * @brief 将线程迁移进目标CPU
 * @param thd: (I) 线程对象的指针
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

        XWOS_BUG_ON(!(XWMP_SKDOBJ_DST_FROZEN & thd->state));
        XWOS_BUG_ON(!(XWMP_SKDOBJ_DST_MIGRATING & thd->state));

        new = xwmp_skd_get_lc();
        xwmp_splk_lock_cpuirqsv(&new->pm.lock, &cpuirq);
        xwmp_splk_lock(&new->thdlistlock);
        thd->migration.dst = 0;
        xwlib_bclst_add_tail(&new->thdlist, &thd->thdnode);
        new->thd_num++;
        xwmb_mp_store_release(struct xwmp_skd *, &thd->xwskd, new);
        xwmp_splk_unlock(&new->thdlistlock);
        if (xwmp_skd_get_pm_state(new) < XWMP_SKD_WKLKCNT_RUNNING) {
                xwmp_splk_lock(&thd->stlock);
                xwbop_c0m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_MIGRATING);
                xwmp_splk_unlock(&thd->stlock);
                xwlib_bclst_add_tail(&new->pm.frzlist, &thd->frznode);
                new->pm.frz_thd_cnt++;
                xwmp_splk_unlock_cpuirqrs(&new->pm.lock, cpuirq);
        } else {
                xwmp_splk_lock(&thd->stlock);
                prio = thd->dprio.r;
                xwbop_c0m(xwsq_t, &thd->state,
                          XWMP_SKDOBJ_DST_FROZEN | XWMP_SKDOBJ_DST_MIGRATING);
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
 * @param thd: (I) 线程对象的指针
 * @note
 * - 此函数只能在线程所属的CPU的中断上下文中执行；
 * - 此函数被调用时需要获得当前CPU调度器的锁pm.lock并且关闭本地CPU的中断；
 * - 此函数假设线程对象已经被引用，执行过程中不会成为野指针；
 */
static __xwmp_code
void xwmp_thd_outmigrate_frozen_lic(struct xwmp_thd * thd)
{
        struct xwmp_skd * old;

        XWOS_BUG_ON(!(XWMP_SKDOBJ_DST_FROZEN & thd->state));
        XWOS_BUG_ON(!(XWMP_SKDOBJ_DST_MIGRATING & thd->state));

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
 * @param thd: (I) 线程对象的指针
 * @param dstcpu: (I) 目标CPU的ID
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
        if (XWMP_SKDOBJ_DST_MIGRATING & thd->state) {
                xwmp_splk_unlock(&thd->stlock);
                xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                rc = -EINPROGRESS;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_MIGRATING);
                if (XWMP_SKDOBJ_DST_FROZEN & thd->state) {
                        xwmp_splk_unlock(&thd->stlock);
                        thd->migration.dst = dstcpu;
                        xwmp_thd_outmigrate_frozen_lic(thd);
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                        if (srccpu == dstcpu) {
                                xwmp_thd_immigrate_lic(thd);
                        } else {
                                xwospl_thd_immigrate((struct xwospl_thd *)thd, dstcpu);
                        }
                } else {
                        xwbop_s1m(xwsq_t, &thd->state, XWMP_SKDOBJ_DST_FREEZABLE);
                        xwmp_splk_unlock(&thd->stlock);
                        thd->migration.dst = dstcpu;
                        xwmp_splk_unlock_cpuirqrs(&xwskd->pm.lock, cpuirq);
                }
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 从本地CPU中移出线程，并准备迁移入目标CPU
 * @param thd: (I) 线程对象的指针
 * @param dstcpu: (I) 目标CPU的ID
 * @return 错误码
 * @note
 * - 此函数只能在线程所属的CPU中执行。
 * - 此函数假设线程对象已经被引用，执行过程中不会成为野指针。
 */
__xwmp_code
xwer_t xwmp_thd_outmigrate_lic(struct xwmp_thd * thd, xwid_t dstcpu)
{
        struct xwmp_skd * xwskd, * local;
        xwer_t rc;

        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        local = xwmp_skd_get_lc();
        XWOS_BUG_ON(xwskd != local);
        rc = xwmp_thd_outmigrate_reqfrz_lic(thd, dstcpu);
        return rc;
}

/**
 * @brief XWMP API：迁移线程到其他CPU
 * @param thd: (I) 线程对象的指针
 * @param dstcpu: (I) 目标CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个thd，不可重入
 */
__xwmp_api
xwer_t xwmp_thd_migrate(struct xwmp_thd * thd, xwid_t dstcpu)
{
        struct xwmp_skd * xwskd;
        xwid_t srccpu;
        xwid_t localcpu;
        xwer_t rc;

        if (__xwcc_unlikely(dstcpu >= CPUCFG_CPU_NUM)) {
                rc = -ENODEV;
                goto err_badcpuid;
        }

        localcpu = xwmp_skd_id_lc();
        xwmb_mp_load_acquire(struct xwmp_skd *, xwskd, &thd->xwskd);
        srccpu = xwskd->id;
        if (localcpu == srccpu) {
                if (XWOK == xwmp_irq_get_id(NULL)) {
                        rc = xwmp_thd_outmigrate_lic(thd, dstcpu);
                } else {
                        rc = xwospl_thd_outmigrate((struct xwospl_thd *)thd, dstcpu);
                }
        } else {
                rc = xwospl_thd_outmigrate((struct xwospl_thd *)thd, dstcpu);
        }

err_badcpuid:
        return rc;
}

#if defined(XWMPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWMPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
/**
 * @brief XWMP API：设置线程的本地数据指针
 * @param thd: (I) 线程对象的指针
 * @param pos: (I) 数据存放位置的索引
 * @param data: (I) 数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_thd_set_data(struct xwmp_thd * thd, xwsq_t pos, void * data)
{
        xwer_t rc;

        XWOS_VALIDATE((thd), "nullptr", -EFAULT);

        if (pos < XWMPCFG_SKD_THD_LOCAL_DATA_NUM) {
                thd->data[pos] = data;
                rc = XWOK;
        } else {
                rc = -ECHRNG;
        }
        return rc;
}

/**
 * @brief XWMP API：获取线程的本地数据指针
 * @param thd: (I) 线程对象的指针
 * @param pos: (I) 数据存放位置的索引
 * @param databuf: (O) 指向缓冲区的指针，通过此缓冲区返回数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_thd_get_data(struct xwmp_thd * thd, xwsq_t pos, void ** databuf)
{
        xwer_t rc;

        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        if (pos < XWMPCFG_SKD_THD_LOCAL_DATA_NUM) {
                *databuf = thd->data[pos];
                rc = XWOK;
        } else {
                *databuf = NULL;
                rc = -ECHRNG;
        }
        return rc;
}

/**
 * @brief XWMP API：设置当前线程的本地数据指针
 * @param pos: (I) 数据存放位置的索引
 * @param data: (I) 数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_cthd_set_data(xwsq_t pos, void * data)
{
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        return xwmp_thd_set_data(cthd, pos, data);
}

/**
 * @brief XWMP API：获取当前线程的本地数据指针
 * @param pos: (I) 数据存放位置的索引
 * @param databuf: (O) 指向缓冲区的指针，通过此缓冲区返回数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_cthd_get_data(xwsq_t pos, void ** databuf)
{
        struct xwmp_thd * cthd;

        cthd = xwmp_skd_get_cthd_lc();
        return xwmp_thd_set_data(cthd, pos, databuf);

}
#endif /* XWMPCFG_SKD_THD_LOCAL_DATA_NUM */
