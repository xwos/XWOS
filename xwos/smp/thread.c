/**
 * @file
 * @brief XuanWuOS内核：线程
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <soc_sched.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWSMPCFG_SD_TCB_MEMSLICE) && (1 == XWSMPCFG_SD_TCB_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWSMPCFG_SD_TCB_MEMSLICE */
#include <xwos/smp/irq.h>
#include <xwos/smp/cpu.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/rtrq.h>
#include <xwos/smp/rtwq.h>
#include <xwos/smp/plwq.h>
#include <xwos/smp/tt.h>
#include <xwos/smp/sync/condition.h>
#include <xwos/smp/sync/semaphore.h>
#include <xwos/smp/mutextree.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/lock/seqlock.h>
#include <xwos/smp/lock/mutex.h>
#include <xwos/smp/thread.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwos_code
xwer_t bdl_thrd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);

extern __xwos_code
xwer_t bdl_thrd_stack_pool_free(xwstk_t * stk);

static __xwos_code
struct xwos_tcb * xwos_tcb_alloc(void);

static __xwos_code
void xwos_tcb_free(struct xwos_tcb * tcb);

static __xwos_code
xwstk_t * xwos_thrd_stack_alloc(xwsz_t stack_size);

static __xwos_code
xwer_t xwos_thrd_stack_free(xwstk_t * stk);

static __xwos_code
void xwos_tcb_construct(struct xwos_tcb * tcb);

static __xwos_code
void xwos_tcb_destruct(struct xwos_tcb * tcb);

static __xwos_code
xwer_t xwos_tcb_gc(void * tcb);

static __xwos_code
xwer_t xwos_thrd_activate(struct xwos_tcb * tcb,
                          const char * name,
                          xwos_thrd_f mainfunc, void * arg,
                          xwstk_t * stack, xwsz_t stack_size,
                          xwpr_t priority, xwsq_t attr,
                          xwobj_gc_f gcfunc);

static __xwos_code
void xwos_thrd_launch(struct xwos_tcb * tcb, xwos_thrd_f mainfunc, void * arg);

static __xwos_code
xwer_t xwos_thrd_terminate_unlock_cb(struct xwos_tcb * tcb);

static __xwos_code
void xwos_thrd_outmigrate_frozen_lic(struct xwos_tcb * tcb);

static __xwos_code
xwer_t xwos_thrd_outmigrate_reqfrz_lic(struct xwos_tcb * tcb, xwid_t dstcpu);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SD_TCB_MEMSLICE) && (1 == XWSMPCFG_SD_TCB_MEMSLICE)
/**
 * @brief 结构体xwos_tcb的对象缓存
 */
__xwos_data static struct xwmm_memslice * xwos_tcb_cache = NULL;

/**
 * @brief 结构体xwos_tcb的对象缓存的名字
 */
__xwos_rodata const char xwos_tcb_cache_name[] = "cache.tcb.xwos";
#endif /* XWSMPCFG_SD_TCB_MEMSLICE */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SD_TCB_MEMSLICE) && (1 == XWSMPCFG_SD_TCB_MEMSLICE)
/**
 * @brief XWOS API：初始化结构体xwos_tcb的对象缓存
 * @param zone_origin: (I) 内存区域的首地址
 * @param zone_size: (I) 内存区域的大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwos_init_code
xwer_t xwos_tcb_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwos_tcb),
                                  xwos_tcb_cache_name,
                                  (ctor_f)xwos_tcb_construct,
                                  (dtor_f)xwos_tcb_destruct);
        if (__unlikely(rc < 0)) {
                xwos_tcb_cache = NULL;
        } else {
                xwos_tcb_cache = msa;
        }
        return rc;
}
#endif /* XWSMPCFG_SD_TCB_MEMSLICE */

/**
 * @brief 从线程控制块对象缓存中申请一个对象
 * @return 线程控制块对象的指针
 */
static __xwos_code
struct xwos_tcb * xwos_tcb_alloc(void)
{
#if defined(XWSMPCFG_SD_TCB_MEMSLICE) && (1 == XWSMPCFG_SD_TCB_MEMSLICE)
        union {
                struct xwos_tcb * tcb;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwos_tcb_cache, &mem.anon);
        if (rc < 0) {
                mem.tcb = err_ptr(rc);
        }/* else {} */
        return mem.tcb;
#else /* XWSMPCFG_SD_TCB_MEMSLICE */
        union {
                struct xwos_tcb * tcb;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwos_tcb), XWMM_ALIGNMENT, &mem.anon);
        if (OK == rc) {
                xwos_tcb_construct(mem.tcb);
        } else {
                mem.tcb = err_ptr(-ENOMEM);
        }
        return mem.tcb;
#endif /* !XWSMPCFG_SD_TCB_MEMSLICE */
}

/**
 * @brief 释放线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 */
static __xwos_code
void xwos_tcb_free(struct xwos_tcb * tcb)
{
#if defined(XWSMPCFG_SD_TCB_MEMSLICE) && (1 == XWSMPCFG_SD_TCB_MEMSLICE)
        xwmm_memslice_free(xwos_tcb_cache, tcb);
#else /* XWSMPCFG_SD_TCB_MEMSLICE */
        xwos_tcb_destruct(tcb);
        xwmm_kma_free(tcb);
#endif /* !XWSMPCFG_SD_TCB_MEMSLICE */
}

/**
 * @brief 动态分配线程栈
 * @return 线程栈的首地址或指针类型的错误码或空指针
 */
static __xwos_code
xwstk_t * xwos_thrd_stack_alloc(xwsz_t stack_size)
{
#if defined(BRDCFG_XWSD_THRD_STACK_POOL) && (1 == BRDCFG_XWSD_THRD_STACK_POOL)
        union {
                xwstk_t * stkbase;
                void * anon;
        } mem;
        xwer_t rc;

        rc = bdl_thrd_stack_pool_alloc(stack_size, &mem.stkbase);
        if (rc < 0) {
                mem.stkbase = err_ptr(rc);
        }/* else {} */
        return mem.stkbase;
#else /* BRDCFG_XWSD_THRD_STACK_POOL */
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
#endif /* !BRDCFG_XWSD_THRD_STACK_POOL */
}

/**
 * @brief 释放动态分配的线程栈内存
 * @param stk: (I) 线程栈的首地址
 * @return 错误码
 */
static __xwos_code
xwer_t xwos_thrd_stack_free(xwstk_t * stk)
{
#if defined(BRDCFG_XWSD_THRD_STACK_POOL) && (1 == BRDCFG_XWSD_THRD_STACK_POOL)
        return bdl_thrd_stack_pool_free(stk);
#else /* BRDCFG_XWSD_THRD_STACK_POOL */
        return xwmm_kma_free(stk);
#endif /* !BRDCFG_XWSD_THRD_STACK_POOL */
}

/**
 * @brief 线程控制块对象的构造函数
 * @param tcb: (I) 线程控制块对象的指针
 */
static __xwos_code
void xwos_tcb_construct(struct xwos_tcb * tcb)
{
        xwos_object_construct(&tcb->xwobj);
}

/**
 * @brief 线程控制块对象的析构函数
 * @param tcb: (I) 线程控制块对象的指针
 */
static __xwos_code
void xwos_tcb_destruct(struct xwos_tcb * tcb)
{
        xwos_object_destruct(&tcb->xwobj);
}

/**
 * @brief 线程控制块对象的垃圾回收函数
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwos_tcb_gc(void * tcb)
{
        xwstk_t * base;

        base = ((struct xwos_tcb *)tcb)->stack.base;
        xwos_thrd_stack_free(base);
        xwos_tcb_free(tcb);
        return OK;
}

/**
 * @brief 激活线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈首地址的指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，@ref xwos_sdobj_attr_em
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非tcb对象的引用计数重新为0
 * @note
 * - 在简单应用中，任务线程不会退出，也不会使用动态内存管理，
 *   因此设置gcfunc为NULL，不做垃圾回收处理。
 */
static __xwos_code
xwer_t xwos_thrd_activate(struct xwos_tcb * tcb,
                          const char * name,
                          xwos_thrd_f mainfunc, void * arg,
                          xwstk_t * stack, xwsz_t stack_size,
                          xwpr_t priority, xwsq_t attr,
                          xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = xwos_object_activate(&tcb->xwobj, gcfunc);
        if (__unlikely(rc < 0)) {
                goto err_obj_activate;
        }
        /* base info */
        xwlk_splk_init(&tcb->stlock);
        tcb->state = XWSDOBJ_DST_UNKNOWN;
        tcb->attribute = attr;

        /* frozen state info */
        xwlib_bclst_init_node(&tcb->frznode);

        /* migrating state info */
        tcb->migration.dst = 0;

        /* ready state info */
        xwlib_bclst_init_node(&tcb->rqnode);

        /* sleeping state info */
        xwos_ttn_init(&tcb->ttn, (xwptr_t)tcb, XWOS_TTN_TYPE_THREAD);

        /* blocking state info */
        xwos_wqn_init(&tcb->wqn, tcb);
        xwsync_cdt_init(&tcb->self);

        /* mutex tree */
        xwos_mtxtree_init(&tcb->mtxtree);

        /* priority */
        if (priority >= XWOS_SD_PRIORITY_RT_NUM) {
                priority = XWOS_SD_PRIORITY_RT_MAX;
        } else if (priority <= XWOS_SD_PRIORITY_INVALID) {
                priority = XWOS_SD_PRIORITY_RT_MIN;
        }
        tcb->sprio = priority;
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.rq = XWOS_SD_PRIORITY_INVALID;
        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;

        tcb->stack.name = name;
        tcb->stack.size = stack_size;
        tcb->stack.base = stack;
#if (defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK))
        tcb->stack.sp = tcb->stack.base + (stack_size >> 2);
#elif (defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK))
        tcb->stack.sp = tcb->stack.base + (stack_size >> 2) - 1;
#elif (defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK))
        tcb->stack.sp = tcb->stack.base - 1;
#elif (defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK))
        tcb->stack.sp = tcb->stack.base;
#else
  #error "Unknown stack type!"
#endif
        /* init completion */
        xwsync_cdt_init(&tcb->completion);

        /* init tcb node */
        xwlib_bclst_init_node(&tcb->tcbnode);

        /* add to ready queue */
        if (mainfunc) {
                xwos_thrd_launch(tcb, mainfunc, arg);
        } else {
                xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_STANDBY);
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
        }
        return OK;

err_obj_activate:
        return rc;
}

/**
 * @brief 加载就绪态的线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 * @param mainfunc: (I) 线程主函数
 * @param arg: (I) 线程主函数的参数
 * @return 错误码
 * @retval OK: OK
 * @retval -EINVAL: 无效的参数
 * @retval -EEXIST: 线程已经被加载
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非线程主函数已经退出，线程回到‘STANDBY’状态
 */
static __xwos_code
void xwos_thrd_launch(struct xwos_tcb * tcb, xwos_thrd_f mainfunc, void * arg)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        /* add to ready queue */
        rc = xwos_thrd_grab(tcb);
        XWOS_BUG_ON(rc < 0);
        /* Add to the tcblist of scheduler */
        xwsd = xwos_scheduler_get_lc();
        xwlk_splk_lock_cpuirqsv(&xwsd->tcblistlock, &cpuirq);
        xwsd->thrd_num++;
        xwlib_bclst_add_tail(&xwsd->tcblist, &tcb->tcbnode);
        xwmb_smp_store_release(struct xwos_scheduler *, &tcb->xwsd, xwsd);
        xwlk_splk_unlock_cpuirqrs(&xwsd->tcblistlock, cpuirq);
        tcb->stack.main = mainfunc;
        tcb->stack.arg = arg;
        soc_scheduler_init_sdobj_stack(&tcb->stack, tcb->attribute);
        xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_STANDBY);
        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);

        /* Add to ready queue */
        rc = xwos_thrd_rq_add_tail(tcb, tcb->sprio);
        XWOS_BUG_ON(rc < 0);
}

/**
 * @brief XWOS API：静态初始化线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈的首地址指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，取值：@ref xwos_sdobj_attr_em中的一项
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入，除非tcb对象的引用计数重新为0
 * @note
 * - 静态初始化线程需预先定义线程控制块对象和线程栈数组，通常定义为全局变量；
 * - 栈数组的首地址与大小，必须要满足CPU的ABI规则，例如ARM，就需要8字节对齐，
 *   因此在定义栈数组时需要__aligned(8)来修饰，且大小是8的倍数。
 */
__xwos_api
xwer_t xwos_thrd_init(struct xwos_tcb * tcb,
                      const char * name,
                      xwos_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr)
{
        xwer_t rc;

        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stack), "nullptr", -EFAULT);

        xwos_tcb_construct(tcb);
        rc = xwos_thrd_activate(tcb,
                                name,
                                mainfunc, arg,
                                stack, stack_size,
                                priority, attr,
                                NULL);
        return rc;
}

/**
 * @brief XWOS API：销毁静态方式初始化的线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 静态方式初始化线程因为没有垃圾回收函数，在线程主函数退出后，需要
 *   由用户将其销毁，释放资源。
 */
__xwos_api
xwer_t xwos_thrd_destroy(struct xwos_tcb * tcb)
{
        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        return xwos_thrd_put(tcb);
}

/**
 * @brief XWOS API：动态创建线程
 * @param tcbpbuf: (O) 指向缓冲区的指针，通过此缓冲区返回线程控制块对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，@ref xwos_sdobj_attr_em
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 动态创建线程采用@ref xwos_tcb_gc()作为其对象的垃圾回收函数，当线程
 *   控制块对象引用计数为0时，系统会自动调用垃圾回收函数回收其资源。
 */
__xwos_api
xwer_t xwos_thrd_create(struct xwos_tcb ** tcbpbuf,
                        const char * name,
                        xwos_thrd_f mainfunc, void * arg,
                        xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        struct xwos_tcb * tcb;
        xwstk_t * stk;
        xwer_t rc;

        XWOS_VALIDATE((NULL != tcbpbuf), "nullptr", -EFAULT);

        stk = xwos_thrd_stack_alloc(stack_size);
        if (__unlikely(is_err(stk))) {
                rc = ptr_err(stk);
                goto err_stack_alloc;
        }

        tcb = xwos_tcb_alloc();
        if (is_err(tcb)) {
                rc = ptr_err(tcb);
                goto err_tcb_alloc;
        }

        rc = xwos_thrd_activate(tcb,
                                name,
                                mainfunc, arg,
                                stk, stack_size,
                                priority, attr,
                                xwos_tcb_gc);
        if (__unlikely(rc < 0)) {
                goto err_thrd_activate;
        }

        *tcbpbuf = tcb;
        return OK;

err_thrd_activate:
        xwos_tcb_free(tcb);
err_tcb_alloc:
        xwos_thrd_stack_free(stk);
err_stack_alloc:
        *tcbpbuf = NULL;
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_thrd_delete(struct xwos_tcb * tcb)
{
        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        return xwos_thrd_put(tcb);
}

/**
 * @brief 执行退出线程的函数
 * @param tcb: (I) 线程控制块对象的指针
 * @param rc: (I) 线程的返回值
 */
__xwos_code
xwer_t xwos_thrd_exit_lic(struct xwos_tcb * tcb, xwer_t rc)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state,
                  (XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_EXITING));
        tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_STANDBY);
        tcb->stack.arg = err_ptr(rc);
        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
        xwsync_cdt_broadcast(&tcb->completion);
        xwlk_splk_lock_cpuirq(&xwsd->pm.lock);
        xwlk_splk_lock(&xwsd->tcblistlock);
        xwlib_bclst_del_init(&tcb->tcbnode);
        xwsd->thrd_num--;
        if (xwsd->pm.frz_thrd_cnt == xwsd->thrd_num) {
                xwlk_splk_unlock(&xwsd->tcblistlock);
                xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                xwos_thrd_put(tcb);
                xwos_scheduler_notify_allfrz_lic(xwsd);
        } else {
                xwlk_splk_unlock(&xwsd->tcblistlock);
                xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                xwos_thrd_put(tcb);
                xwos_scheduler_req_swcx(xwsd);
        }
        return OK;
}

/**
 * @brief XWOS API：当前线程退出
 * @param rc: (I) 线程退出时的返回值
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 调用的线程立即退出并抛出返回值，类似于POSIX线程库中的pthread_exit()函数。
 */
__xwos_api
void xwos_cthrd_exit(xwer_t rc)
{
        struct xwos_tcb * ctcb;

        ctcb = xwos_scheduler_get_ctcb_lc();
        soc_thrd_exit_lc(ctcb, rc);
}

/**
 * @brief @ref xwos_thrd_terminate()中使用的回调锁的解锁函数
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 */
static __xwos_code
xwer_t xwos_thrd_terminate_unlock_cb(struct xwos_tcb * tcb)
{
        xwlk_splk_unlock(&tcb->stlock);
        xwos_thrd_intr(tcb);
        return OK;
}

/**
 * @brief XWOS API：终止线程并等待它的返回值
 * @param tcb: (I) 线程控制块对象的指针
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回被终止线程的返回值，
 *                 可为NULL，表示不需要返回
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 线程在此之前已经退出
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数通常由父线程调用，用于终止子线程：父线程会一直阻塞等待子线程退出，
 *   并获取子线程的返回值，此函数类似于POSIX线程库中的pthread_join()函数。
 * - 注意：与其他操作系统不同（POSIX的pthread_join()和Linux的kthread_stop()），
 *   XuanWuOS的线程在此函数调用之前就返回并不会引起错误，此函数的返回值是
 *   小于0的错误码，指针trc指向的缓冲区依然可以获取线程之前的返回值。
 */
__xwos_api
xwer_t xwos_thrd_terminate(struct xwos_tcb * tcb, xwer_t * trc)
{
        struct xwlk_cblk lockcb;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        lockcb.unlock = (xwer_t (*)(void *))xwos_thrd_terminate_unlock_cb;
        lockcb.lock = NULL;
        xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
        if (XWSDOBJ_DST_STANDBY & tcb->state) {
                rc = -EALREADY;
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)tcb->stack.arg;
                }
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_EXITING);
                rc = xwsync_cdt_wait(&tcb->completion,
                                     &lockcb, XWLK_TYPE_CALLBACK,
                                     tcb, XWOS_UNUSED_ARGUMENT, &lkst);
                if (OK == rc) {
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)tcb->stack.arg;
                        }
                } else {
                        if (XWLK_STATE_LOCKED == lkst) {
                                xwlk_splk_unlock(&tcb->stlock);
                        }
                }
        }
        xwos_cpuirq_restore_lc(cpuirq);

        return rc;
}

/**
 * @brief XWOS API：判断线程是否可以退出，若不是，线程就阻塞直到它被终止
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数由子线程调用，等待父线程发出退出信号。
 * - 在父线程需要同步获取子线程运行结果的应用场景，子线程不可在父线程
 *   调用@ref xwos_thrd_terminate()前退出。但有时子线程运行得很快，
 *   即将退出，父线程此时还没来得及调用xwos_thrd_terminate()。子线程通过调用
 *   此函数阻塞自身并等待父线程调用@ref xwos_thrd_terminate()。
 * - 如果父线程不关心子线程的返回值，也不会调用@ref xwos_thrd_terminate()，
 *   子线程可以直接退出。
 */
__xwos_api
void xwos_cthrd_wait_exit(void)
{
        struct xwos_tcb * ctcb;
        xwpr_t dprio;
        xwreg_t cpuirq;

        ctcb = xwos_scheduler_get_ctcb_lc();
        xwlk_splk_lock_cpuirqsv(&ctcb->wqn.lock, &cpuirq);
        xwlk_splk_lock(&ctcb->stlock);
        while (!(XWSDOBJ_DST_EXITING & ctcb->state)) {
                dprio = ctcb->dprio.r;
                xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
                ctcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
                ctcb->dprio.wq = dprio;
                xwbop_s1m(xwsq_t, &ctcb->state, XWSDOBJ_DST_BLOCKING);
                xwlk_splk_unlock(&ctcb->stlock);
                ctcb->wqn.type = XWOS_WQTYPE_NULL;
                xwaop_store(xwsq_t, &ctcb->wqn.rsmrs,
                            xwmb_modr_release, XWOS_WQN_RSMRS_UNKNOWN);
                ctcb->wqn.wq = NULL;
                ctcb->wqn.cb = xwos_thrd_wqn_callback;
                xwlk_splk_unlock_cpuirq(&ctcb->wqn.lock);
                xwos_scheduler_req_swcx(ctcb->xwsd);
                xwlk_splk_lock_cpuirq(&ctcb->wqn.lock);
                xwlk_splk_lock(&ctcb->stlock);
        }
        xwlk_splk_unlock(&ctcb->stlock);
        xwlk_splk_unlock_cpuirqrs(&ctcb->wqn.lock, cpuirq);
}

/**
 * @brief 改变线程的动态优先级一次
 * @param tcb: (I) 线程控制块对象的指针
 * @param dprio: (I) 动态优先级
 * @param pmtx: (O) 指向缓冲区的指针，通过此缓冲区返回互斥锁对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ESTALE: 状态已经被其他CPU改变
 * @note
 * - 此函数将线程(tcb)的优先级改成(dprio)，并返回接下来需要修改的互斥锁(*pmtx)
 *   的优先级。
 */
__xwos_code
xwer_t xwos_thrd_chprio_once(struct xwos_tcb * tcb, xwpr_t dprio,
                             struct xwlk_mtx ** pmtx)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
        if ((XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_FROZEN) & tcb->state) {
                tcb->dprio.r = dprio;
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                rc = OK;
        } else if (XWSDOBJ_DST_READY & tcb->state) {
                if (tcb->dprio.rq == dprio) {
                        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                        rc = OK;
                } else {
                        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                        rc = xwos_thrd_rq_remove(tcb);
                        if (OK == rc) {
                                rc = xwos_thrd_rq_add_tail(tcb, dprio);
                        }/* else {} */
                }
        } else if (XWSDOBJ_DST_BLOCKING & tcb->state) {
                if (tcb->dprio.wq == dprio) {
                        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                        rc = OK;
                } else {
                        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                        /* because of the lock sequence.
                           The program may be interrupted and must
                           re-check the condition */
                        xwlk_splk_lock_cpuirq(&tcb->wqn.lock);
                        if (XWOS_WQTYPE_MTX == tcb->wqn.type) {
                                struct xwlk_mtx * mtx;

                                mtx = container_of(tcb->wqn.wq,
                                                   struct xwlk_mtx,
                                                   rtwq);
                                xwlk_mtx_grab(mtx);
                                xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                                /* because of the lock sequence.
                                   The program may be interrupted and must
                                   re-check the condition */
                                xwos_rtwq_lock_cpuirq(&mtx->rtwq);
                                xwlk_splk_lock(&tcb->wqn.lock);
                                if (NULL == tcb->wqn.cb) {
                                        xwlk_splk_unlock(&tcb->wqn.lock);
                                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        rc = -ESTALE;
                                } else if (tcb->wqn.wq != &mtx->rtwq) {
                                        xwlk_splk_unlock(&tcb->wqn.lock);
                                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        rc = -ESTALE;
                                } else {
                                        xwos_rtwq_remove_locked(&mtx->rtwq,
                                                                &tcb->wqn);
                                        xwlk_splk_lock(&tcb->stlock);
                                        tcb->dprio.wq = dprio;
                                        xwlk_splk_unlock(&tcb->stlock);
                                        xwos_rtwq_add_locked(&mtx->rtwq,
                                                             &tcb->wqn,
                                                             dprio);
                                        xwlk_splk_unlock(&tcb->wqn.lock);
                                        xwos_rtwq_unlock_cpuirqrs(&mtx->rtwq, cpuirq);
                                        *pmtx = mtx;
                                        rc = OK;
                                }
                                xwlk_mtx_put(mtx);
#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
                        } else if (XWOS_WQTYPE_RTSMR == tcb->wqn.type) {
                                struct xwsync_smr * smr;

                                smr = container_of(tcb->wqn.wq,
                                                   struct xwsync_smr,
                                                   wq.rt);
                                xwsync_smr_grab(smr);
                                xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                                /* because of the lock sequence.
                                   The program may be interrupted and must
                                   re-check the condition */
                                xwos_rtwq_lock_cpuirq(&smr->wq.rt);
                                xwlk_splk_lock(&tcb->wqn.lock);
                                if (NULL == tcb->wqn.cb) {
                                        xwlk_splk_unlock(&tcb->wqn.lock);
                                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                                        rc = -ESTALE;
                                } else if (tcb->wqn.wq != &smr->wq.rt) {
                                        xwlk_splk_unlock(&tcb->wqn.lock);
                                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                                        rc = -ESTALE;
                                } else {
                                        xwos_rtwq_remove_locked(&smr->wq.rt,
                                                                &tcb->wqn);
                                        xwlk_splk_lock(&tcb->stlock);
                                        tcb->dprio.wq = dprio;
                                        xwlk_splk_unlock(&tcb->stlock);
                                        xwos_rtwq_add_locked(&smr->wq.rt,
                                                             &tcb->wqn,
                                                             dprio);
                                        xwlk_splk_unlock(&tcb->wqn.lock);
                                        xwos_rtwq_unlock_cpuirqrs(&smr->wq.rt, cpuirq);
                                        rc = OK;
                                }
                                xwsync_smr_put(smr);
#endif /* XWSMPCFG_SYNC_RTSMR */
                        } else {
                                xwlk_splk_lock(&tcb->stlock);
                                tcb->dprio.wq = dprio;
                                xwlk_splk_unlock(&tcb->stlock);
                                xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                                rc = OK;
                        }
                }
        } else if (XWSDOBJ_DST_SLEEPING & tcb->state) {
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                rc = OK;
        } else {
                XWOS_BUG_ON(XWSDOBJ_DST_STANDBY & tcb->state);
                rc = -ESTALE;
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
        }
        return rc;
}

/**
 * @brief 重新设定线程的动态优先级
 * @param tcb: (I) 线程控制块对象的指针
 */
__xwos_code
void xwos_thrd_chprio(struct xwos_tcb * tcb)
{
        struct xwos_mtxtree * mt;
        struct xwlk_mtx * tmp;
        xwpr_t dprio;
        xwsq_t seq;
        xwer_t rc;

        mt = &tcb->mtxtree;
        do {
                seq = xwlk_sqlk_rd_begin(&mt->lock);
                dprio = tcb->sprio > mt->maxprio ?
                        tcb->sprio : mt->maxprio;
                rc = xwos_thrd_chprio_once(tcb, dprio, &tmp);
        } while (xwlk_sqlk_rd_retry(&mt->lock, seq) || (rc < 0));
}

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param tcb: (I) 线程控制块对象的指针
 * @param prio: (I) 动态优先级
 * @return 错误码
 * @retval OK: OK
 * @retval -EEXIST: 线程已经存在
 * @retval -ESTALE: 状态已经发生改变(当前线程的状态可能已经被其他CPU修改)
 */
__xwos_code
xwer_t xwos_thrd_rq_add_head(struct xwos_tcb * tcb, xwpr_t prio)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwrtrq = &xwsd->rq.rt;
        xwlk_splk_lock_cpuirqsv(&xwrtrq->lock, &cpuirq);
        xwlk_splk_lock(&tcb->stlock);
        if (((XWSDOBJ_DST_BLOCKING & tcb->state) &&
             (XWSDOBJ_DST_SLEEPING & tcb->state)) ||
            (XWSDOBJ_DST_FROZEN & tcb->state)) {
                xwlk_splk_unlock(&tcb->stlock);
                rc = -ESTALE;
        } else if ((XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_READY) & tcb->state) {
                xwlk_splk_unlock(&tcb->stlock);
                rc = -EEXIST;
        } else {
                tcb->dprio.rq = prio;
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_READY);
                xwlk_splk_unlock(&tcb->stlock);
                rc = xwos_rtrq_add_head_locked(xwrtrq, tcb);
        }
        xwlk_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param tcb: (I) 线程控制块对象的指针
 * @param prio: (I) 动态优先级
 * @return 错误码
 * @retval OK: OK
 * @retval -EEXIST: 线程已经存在
 * @retval -ESTALE: 状态已经发生改变(当前线程的状态可能已经被其他CPU修改)
 */
__xwos_code
xwer_t xwos_thrd_rq_add_tail(struct xwos_tcb * tcb, xwpr_t prio)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwrtrq = &xwsd->rq.rt;
        xwlk_splk_lock_cpuirqsv(&xwrtrq->lock, &cpuirq);
        xwlk_splk_lock(&tcb->stlock);
        if (((XWSDOBJ_DST_BLOCKING & tcb->state) &&
             (XWSDOBJ_DST_SLEEPING & tcb->state)) ||
            (XWSDOBJ_DST_FROZEN & tcb->state)) {
                xwlk_splk_unlock(&tcb->stlock);
                rc = -ESTALE;
        } else if ((XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_READY) & tcb->state) {
                xwlk_splk_unlock(&tcb->stlock);
                rc = -EEXIST;
        } else {
                tcb->dprio.rq = prio;
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_READY);
                xwlk_splk_unlock(&tcb->stlock);
                rc = xwos_rtrq_add_tail_locked(xwrtrq, tcb);
        }
        xwlk_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief 将线程从就绪队列中删除
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ESRCH: 没有这个线程
 */
__xwos_code
xwer_t xwos_thrd_rq_remove(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwrtrq = &xwsd->rq.rt;
        xwlk_splk_lock_cpuirqsv(&xwrtrq->lock, &cpuirq);
        rc = xwos_rtrq_remove_locked(xwrtrq, tcb);
        if (OK == rc) {
                xwlk_splk_lock(&tcb->stlock);
                xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_READY);
                tcb->dprio.rq = XWOS_SD_PRIORITY_INVALID;
                xwlk_splk_unlock(&tcb->stlock);
        }
        xwlk_splk_unlock_cpuirqrs(&xwrtrq->lock, cpuirq);
        return rc;
}

/**
 * @brief XWOS API：中断线程的睡眠或阻塞状态
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EPERM: 线程不可被中断
 * @retval -EINVAL: 无效的线程态
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_thrd_intr(struct xwos_tcb * tcb)
{
        xwos_wqn_f cb;
        xwreg_t cpuirq;
        xwer_t rc;

        xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
        if (XWSDOBJ_DST_UNINTERRUPTED & tcb->state) {
                rc = -EPERM;
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
        } else if (XWSDOBJ_DST_BLOCKING & tcb->state) {
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                /* 上锁必须按照锁的顺序，否则会出现死锁。
                   一旦“解锁-重新上锁”以后就需要重新检查条件。 */
                xwlk_splk_lock_cpuirq(&tcb->wqn.lock);
                if (XWOS_WQTYPE_NULL == tcb->wqn.type) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                                    xwmb_modr_release, XWOS_WQN_RSMRS_INTR);
                        cb = tcb->wqn.cb;
                        tcb->wqn.cb = NULL;
                        xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                        cb(tcb);
                        rc = OK;
#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
                } else if (XWOS_WQTYPE_PLSMR == tcb->wqn.type) {
                        struct xwsync_smr * smr;

                        smr = container_of(tcb->wqn.wq, struct xwsync_smr, wq.pl);
                        xwsync_smr_grab(smr);
                        xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                        rc = xwsync_plsmr_intr(smr, &tcb->wqn);
                        xwsync_smr_put(smr);
#endif /* XWSMPCFG_SYNC_PLSMR */
#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
                } else if (XWOS_WQTYPE_RTSMR == tcb->wqn.type) {
                        struct xwsync_smr * smr;

                        smr = container_of(tcb->wqn.wq, struct xwsync_smr, wq.rt);
                        xwsync_smr_grab(smr);
                        xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                        rc = xwsync_rtsmr_intr(smr, &tcb->wqn);
                        xwsync_smr_put(smr);
#endif /* XWSMPCFG_SYNC_RTSMR */
                } else if (XWOS_WQTYPE_CDT == tcb->wqn.type) {
                        struct xwsync_cdt * cdt;

                        cdt = container_of(tcb->wqn.wq, struct xwsync_cdt, wq.pl);
                        xwsync_cdt_grab(cdt);
                        xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                        rc = xwsync_cdt_intr(cdt, &tcb->wqn);
                        xwsync_cdt_put(cdt);
                } else if (XWOS_WQTYPE_MTX == tcb->wqn.type) {
                        struct xwlk_mtx * mtx;

                        mtx = container_of(tcb->wqn.wq, struct xwlk_mtx, rtwq);
                        xwlk_mtx_grab(mtx);
                        xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                        rc = xwlk_mtx_intr(mtx, tcb);
                        xwlk_mtx_put(mtx);
                } else {
                        xwlk_splk_unlock_cpuirqrs(&tcb->wqn.lock, cpuirq);
                        XWOS_BUG();
                }
        } else if (XWSDOBJ_DST_SLEEPING & tcb->state) {
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
                /* IRQs may happen. */
                struct xwos_scheduler * xwsd;
                struct xwos_tt * xwtt;

                xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
                xwtt = &xwsd->tt;
                xwlk_sqlk_wr_lock_cpuirq(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwlk_splk_lock(&tcb->stlock);
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_splk_unlock(&tcb->stlock);
                        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        xwos_thrd_wakeup(tcb);
                        xwos_scheduler_chkpmpt(tcb->xwsd);
                } else {
                        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                }
        } else {
                rc = -EINVAL;
                xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
        }
        return rc;
}

/**
 * @brief 唤醒一个阻塞/睡眠态的线程，将其加入到调度器的就绪队列
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 */
__xwos_code
xwer_t xwos_thrd_wakeup(struct xwos_tcb * tcb)
{
        struct xwos_mtxtree * mt;
        xwpr_t dprio;
        xwreg_t cpuirq;
        xwer_t rc;

        mt = &tcb->mtxtree;
        xwlk_sqlk_rdex_lock_cpuirqsv(&mt->lock, &cpuirq);
        dprio = tcb->sprio > mt->maxprio ? tcb->sprio : mt->maxprio;
        rc = xwos_thrd_rq_add_tail(tcb, dprio);
        xwlk_sqlk_rdex_unlock_cpuirqrs(&mt->lock, cpuirq);
        return rc;
}

/**
 * @brief 线程的时间树节点回调函数
 * @param entry: (I) 线程控制块对象的地址
 */
__xwos_code
void xwos_thrd_ttn_callback(void * entry)
{
        struct xwos_tcb * tcb;
        xwreg_t cpuirq;

        tcb = entry;
        xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
        xwos_thrd_wakeup(tcb);
}

/**
 * @brief 将线程加入到时间树上
 * @param tcb: (I) 线程控制块对象的指针
 * @param xwtt: (I) 时间树的指针
 * @param expected: (I) 期望被唤醒的时间
 * @param cpuirq: (I) 本地CPU的中断标志
 * @return 错误码
 * @note
 * - 此函数只能在取得写锁xwtt->lock以及关闭本地CPU的中断时才可调用。
 */
__xwos_code
xwer_t xwos_thrd_tt_add_locked(struct xwos_tcb * tcb, struct xwos_tt * xwtt,
                               xwtm_t expected, xwreg_t cpuirq)
{
        xwer_t rc;

        /* add to time tree */
        tcb->ttn.wkup_xwtm = expected;
        xwaop_store(xwsq_t, &tcb->ttn.wkuprs,
                    xwmb_modr_release, XWOS_TTN_WKUPRS_UNKNOWN);
        tcb->ttn.xwtt = xwtt;
        tcb->ttn.cb = xwos_thrd_ttn_callback;
        rc = xwos_tt_add_locked(xwtt, &tcb->ttn, cpuirq);
        return rc;
}

/**
 * @brief 线程的等待队列节点回调函数
 * @param entry: (I) 线程控制块对象的地址
 */
__xwos_code
void xwos_thrd_wqn_callback(void * entry)
{
        struct xwos_tcb * tcb;
        xwreg_t cpuirq;

        tcb = entry;
        xwlk_splk_lock_cpuirqsv(&tcb->stlock, &cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state,
                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
        tcb->dprio.wq = XWOS_SD_PRIORITY_INVALID;
        xwlk_splk_unlock_cpuirqrs(&tcb->stlock, cpuirq);
        xwos_thrd_wakeup(tcb);
        xwos_scheduler_chkpmpt(tcb->xwsd);
}

/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param tcb: (I) 线程控制块对象的指针
 * @param xwrtwq: (I) 实时（红黑树）等待队列
 * @param type: (I) 等待队列的类型
 * @param dprio: (I) 加入时线程的动态优先级
 */
__xwos_code
void xwos_thrd_eq_rtwq_locked(struct xwos_tcb * tcb,
                              struct xwos_rtwq * xwrtwq,
                              xwu16_t type, xwpr_t dprio)
{
        xwlk_splk_lock(&tcb->wqn.lock);
        tcb->wqn.wq = xwrtwq;
        tcb->wqn.type = type;
        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                    xwmb_modr_release, XWOS_WQN_RSMRS_UNKNOWN);
        tcb->wqn.cb = xwos_thrd_wqn_callback;
        xwos_rtwq_add_locked(xwrtwq, &tcb->wqn, dprio);
        xwlk_splk_unlock(&tcb->wqn.lock);
}

/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param tcb: (I) 线程控制块对象的指针
 * @param xwplwq: (I) 管道（双循环链表）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwos_code
void xwos_thrd_eq_plwq_locked(struct xwos_tcb * tcb,
                              struct xwos_plwq * xwplwq,
                              xwu16_t type)
{
        xwlk_splk_lock(&tcb->wqn.lock);
        tcb->wqn.wq = xwplwq;
        tcb->wqn.type = type;
        xwaop_store(xwsq_t, &tcb->wqn.rsmrs,
                    xwmb_modr_release, XWOS_WQN_RSMRS_UNKNOWN);
        tcb->wqn.cb = xwos_thrd_wqn_callback;
        xwos_plwq_add_tail_locked(xwplwq, &tcb->wqn);
        xwlk_splk_unlock(&tcb->wqn.lock);
}

/**
 * @brief XWOS API：让出当前线程的CPU，调度器重新选择同优先级的其他线程运行
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数只能将CPU让给优先级与当前线程相同且已经就绪的其他线程。
 *   如果当前线程已经是系统中唯一的最高优先级线程，此函数无效。
 */
__xwos_api
void xwos_cthrd_yield(void)
{
        struct xwos_tcb * ctcb;
        xwreg_t cpuirq;
        xwpr_t prio;
        xwer_t rc;

        ctcb = xwos_scheduler_get_ctcb_lc();
        xwlk_rawly_lock_cpuirqsv(&ctcb->stlock, &cpuirq);
        prio = ctcb->dprio.r;
        xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
        ctcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        xwlk_rawly_unlock_cpuirq(&ctcb->stlock);
        rc = xwos_thrd_rq_add_tail(ctcb, prio);
        XWOS_BUG_ON(rc < 0);
        xwos_scheduler_req_swcx(ctcb->xwsd);
        xwos_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief 解锁给定类型的锁
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 */
__xwos_code
xwer_t xwos_thrd_do_unlock(void * lock, xwsq_t lktype,
                           void * lkdata, xwsz_t datanum)
{
        xwer_t rc;
        union xwlk_ulock ulk;
        struct xwos_irq_resource * irqrsc;

        ulk.anon = lock;
        irqrsc = lkdata;

        rc = OK;
        switch (lktype) {
        case XWLK_TYPE_MTX:
        case XWLK_TYPE_MTX_UNINTR:
                rc = xwlk_mtx_unlock(ulk.xwos.mtx);
                break;
        case XWLK_TYPE_SPLK:
                xwlk_splk_unlock(ulk.xwos.splk);
                break;
        case XWLK_TYPE_SPLK_IRQS:
                xwlk_splk_unlock_irqs(ulk.xwos.splk, irqrsc, datanum);
                break;
        case XWLK_TYPE_SPLK_CPUIRQ:
                xwlk_splk_unlock_cpuirq(ulk.xwos.splk);
                break;
        case XWLK_TYPE_SQLK_WR:
                xwlk_sqlk_wr_unlock(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_SQLK_WR_IRQS:
                xwlk_sqlk_wr_unlock_irqs(ulk.xwos.sqlk, irqrsc, datanum);
                break;
        case XWLK_TYPE_SQLK_WR_CPUIRQ:
                xwlk_sqlk_wr_unlock_cpuirq(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_SQLK_RDEX:
                xwlk_sqlk_rdex_unlock(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_SQLK_RDEX_IRQS:
                xwlk_sqlk_rdex_unlock_irqs(ulk.xwos.sqlk, irqrsc, datanum);
                break;
        case XWLK_TYPE_SQLK_RDEX_CPUIRQ:
                xwlk_sqlk_rdex_unlock_cpuirq(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_CALLBACK:
                if (ulk.cb->unlock) {
                        rc = ulk.cb->unlock(lkdata);
                }
                break;
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
 * @param datanum: (I) 锁的数据数量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 */
__xwos_code
xwer_t xwos_thrd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm,
                         void * lkdata, xwsz_t datanum)
{
        xwer_t rc;
        union xwlk_ulock ulk;
        struct xwos_irq_resource * irqrsc;

        ulk.anon = lock;
        irqrsc = lkdata;

        rc = OK;
        switch (lktype) {
        case XWLK_TYPE_MTX:
                if (xwtm) {
                        rc = xwlk_mtx_timedlock(ulk.xwos.mtx, xwtm);
                } else {
                        rc = xwlk_mtx_lock(ulk.xwos.mtx);
                }
                break;
        case XWLK_TYPE_MTX_UNINTR:
                rc = xwlk_mtx_lock_unintr(ulk.xwos.mtx);
                break;
        case XWLK_TYPE_SPLK:
                xwlk_splk_lock(ulk.xwos.splk);
                break;
        case XWLK_TYPE_SPLK_IRQS:
                xwlk_splk_lock_irqs(ulk.xwos.splk, irqrsc, datanum);
                break;
        case XWLK_TYPE_SPLK_CPUIRQ:
                xwlk_splk_lock_cpuirq(ulk.xwos.splk);
                break;
        case XWLK_TYPE_SQLK_WR:
                xwlk_sqlk_wr_lock(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_SQLK_WR_IRQS:
                xwlk_sqlk_wr_lock_irqs(ulk.xwos.sqlk, irqrsc, datanum);
                break;
        case XWLK_TYPE_SQLK_WR_CPUIRQ:
                xwlk_sqlk_wr_lock_cpuirq(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_SQLK_RDEX:
                xwlk_sqlk_rdex_lock(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_SQLK_RDEX_IRQS:
                xwlk_sqlk_wr_lock_irqs(ulk.xwos.sqlk, irqrsc, datanum);
                break;
        case XWLK_TYPE_SQLK_RDEX_CPUIRQ:
                xwlk_sqlk_rdex_lock_cpuirq(ulk.xwos.sqlk);
                break;
        case XWLK_TYPE_CALLBACK:
                if (ulk.cb->lock) {
                        rc = ulk.cb->lock(lkdata);
                }
                break;
        default:
                break;
        }
        return rc;
}

/**
 * @brief XWOS API: 继续已经暂停的线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EPERM: 线程未被暂停
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 线程通过调用@ref xwos_cthrd_pause()或@ref xwos_cthrd_timedpause()暂停自己
 * - 中断或其他线程可以调用此函数使已暂停的线程继续运行。
 */
__xwos_api
xwer_t xwos_thrd_continue(struct xwos_tcb * tcb)
{
        xwer_t rc;

        XWOS_VALIDATE((tcb), "nullptr", -EFAULT);

        rc = xwsync_cdt_do_unicast(&tcb->self);
        return rc;
}

/**
 * @brief XWOS API：限时暂停当前线程
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @retuan 错误码
 * @retval OK: OK
 * @retval -EINTR: 暂停过程被中断
 * @retval -ETIMEDOUT: 超时
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * - 等待超时后将以返回值-ETIMEDOUT返回，并且 *xwtm* 指向缓冲区返回0。
 */
__xwos_api
xwer_t xwos_cthrd_timedpause(void * lock, xwsq_t lktype,
                             void * lkdata, xwsz_t datanum,
                             xwtm_t * xwtm, xwsq_t * lkst)
{
        struct xwos_tcb * ctcb;
        xwer_t rc;

        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((lkst), "nullptr", -EFAULT);
        XWOS_VALIDATE((xwtm_cmp(*xwtm, 0) >= 0), "out-of-time", -ETIMEDOUT);
        XWOS_VALIDATE(((NULL == lock) || (lktype < XWLK_TYPE_NUM)),
                      "invalid-type", -EINVAL);
        XWOS_VALIDATE((-EINTHRD == xwos_irq_get_id(NULL)),
                      "not-in-thrd", -ENOTINTHRD);

        *lkst = XWLK_STATE_LOCKED;
        ctcb = xwos_scheduler_get_ctcb_lc();
        if (__unlikely(0 == xwtm_cmp(*xwtm, 0))) {
                rc = xwos_thrd_do_unlock(lock, lktype, lkdata, datanum);
                if (OK == rc) {
                        *lkst = XWLK_STATE_UNLOCKED;
                }/* else {} */
                rc = -ETIMEDOUT;
        } else {
                rc = xwsync_cdt_do_timedwait(&ctcb->self, ctcb, lock, lktype,
                                             lkdata, datanum,
                                             xwtm, lkst);
        }
        return rc;
}

/**
 * @brief XWOS API：线程睡眠一段时间
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EINTR: 睡眠过程被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * - 超时后将以返回值OK返回，并且 *xwtm* 指向缓冲区返回0。
 */
__xwos_api
xwer_t xwos_cthrd_sleep(xwtm_t * xwtm)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;
        struct xwos_tcb * ctcb;
        xwtm_t expected, currtick;
        xwer_t rc;
        xwsq_t wkuprs;
        xwreg_t cpuirq;

        if (__unlikely(xwtm_cmp(*xwtm, 0) <= 0)) {
                rc = -ETIMEDOUT;
                goto err_xwtm;
        }

        ctcb = xwos_scheduler_get_ctcb_lc();
        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &ctcb->xwsd);
        xwtt = &xwsd->tt;
        hwt = &xwtt->hwt;
        currtick = xwos_syshwt_get_timetick(hwt);
        expected = xwtm_add_safely(currtick, *xwtm);
        xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwos_scheduler_wakelock_lock(xwsd);
        if (__unlikely(rc < 0)) {
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，
                   阻塞/睡眠函数将返回-EINTR。*/
                rc = -EINTR;
                goto err_needfrz;
        }
        xwlk_splk_lock(&ctcb->stlock);
        XWOS_BUG_ON((XWSDOBJ_DST_SLEEPING | XWSDOBJ_DST_READY |
                     XWSDOBJ_DST_STANDBY | XWSDOBJ_DST_FROZEN |
                     XWSDOBJ_DST_MIGRATING)
                    & ctcb->state);
        if (XWSDOBJ_DST_FREEZABLE & ctcb->state) {
                xwlk_splk_unlock(&ctcb->stlock);
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                xwos_scheduler_wakelock_unlock(xwsd);
                rc = -EINTR;
                goto err_needfrz;
        }
        /* set sleeping state */
        xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
        ctcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &ctcb->state, XWSDOBJ_DST_SLEEPING);
        xwlk_splk_unlock(&ctcb->stlock);
        /* add to time tree */
        xwos_thrd_tt_add_locked(ctcb, xwtt, expected, cpuirq);
        /* enable local CPU IRQ to enable schedule */
        xwlk_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        xwos_scheduler_wakelock_unlock(xwsd);
        xwos_scheduler_req_swcx(xwsd);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_smp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        wkuprs = xwaop_load(xwsq_t, &ctcb->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWOS_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_INTR == wkuprs) {
                rc = -EINTR;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        currtick = xwos_syshwt_get_timetick(hwt);
        *xwtm = xwtm_sub(expected, currtick);

err_needfrz:
err_xwtm:
        return rc;
}

/**
 * @brief XWOS API：线程睡眠到指定的系统时间
 * @param origin: 指向缓冲区的指针，此缓冲区：
 *                (I) 作为输入时，作为时间起点
 *                (O) 作为输出时，返回线程被唤醒的时间
 *                    （可作为下一次时间起点，形成精确的周期）
 * @param inc: (I) 期望被唤醒的时间增量（相对于时间原点）
 * @return 错误码
 * @retval OK: OK
 * @retval -EINTR: 睡眠过程被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        struct xwos_syshwt * hwt;
        struct xwos_tcb * ctcb;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwsq_t wkuprs;
        xwer_t rc;

        ctcb = xwos_scheduler_get_ctcb_lc();
        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &ctcb->xwsd);
        xwtt = &xwsd->tt;
        hwt = &xwtt->hwt;
        expected = xwtm_add_safely(*origin, inc);
        xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwos_scheduler_wakelock_lock(xwsd);
        if (__unlikely(rc < 0)) {
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，
                   阻塞/睡眠函数将返回-EINTR。*/
                rc = -EINTR;
                goto err_needfrz;
        }
        xwlk_splk_lock(&ctcb->stlock);
        XWOS_BUG_ON((XWSDOBJ_DST_SLEEPING | XWSDOBJ_DST_READY |
                     XWSDOBJ_DST_STANDBY | XWSDOBJ_DST_FROZEN |
                     XWSDOBJ_DST_MIGRATING)
                    & ctcb->state);
        if (XWSDOBJ_DST_FREEZABLE & ctcb->state) {
                xwlk_splk_unlock(&ctcb->stlock);
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                xwos_scheduler_wakelock_unlock(xwsd);
                rc = -EINTR;
                goto err_needfrz;
        }
        /* set the sleeping state */
        xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
        ctcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
        xwbop_s1m(xwsq_t, &ctcb->state, XWSDOBJ_DST_SLEEPING);
        xwlk_splk_unlock(&ctcb->stlock);
        /* add to time tree */
        xwos_thrd_tt_add_locked(ctcb, xwtt, expected, cpuirq);
        /* enable local CPU IRQ to enable schedule */
        xwlk_sqlk_wr_unlock_cpuirq(&xwtt->lock);
        xwos_scheduler_wakelock_unlock(xwsd);
        xwos_scheduler_req_swcx(xwsd);
        xwos_cpuirq_restore_lc(cpuirq);

        /* 判断唤醒原因 */
        xwmb_smp_rmb();
        /* 确保对唤醒原因的读取操作发生在线程状态切换之后 */
        wkuprs = xwaop_load(xwsq_t, &ctcb->ttn.wkuprs, xwmb_modr_relaxed);
        if (XWOS_TTN_WKUPRS_TIMEDOUT == wkuprs) {
                rc = OK;
        } else if (XWOS_TTN_WKUPRS_INTR == wkuprs) {
                rc = -EINTR;
        } else {
                XWOS_BUG();
                rc = -EBUG;
        }
        *origin = xwos_syshwt_get_timetick(hwt);

err_needfrz:
        return rc;
}

/**
 * @brief 申请冻结当前CPU中的线程
 * @param tcb: (I) 线程控制块
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 线程已经被冻结
 * @note
 * - 此函数只可在线程所属的CPU的中断上下文中被调用；
 * - 此函数假设线程控制块对象已经被引用，执行过程中不会成为野指针。
 */
__xwos_code
xwer_t xwos_thrd_reqfrz_lic(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwlk_splk_lock_cpuirqsv(&xwsd->pm.lock, &cpuirq);
        xwlk_splk_lock(&tcb->stlock);
        if (XWSDOBJ_DST_FROZEN & tcb->state) {
                xwlk_splk_unlock(&tcb->stlock);
                rc = -EALREADY;
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_FREEZABLE);
                xwlk_splk_unlock(&tcb->stlock);
                rc = OK;
        }
        xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
        return rc;
}

/**
 * @brief 执行冻结线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在线程所在的CPU的本地软中断执行；
 * - 此中断只可从函数@ref xwos_cthrd_freeze()中进入。
 */
__xwos_code
xwer_t xwos_thrd_freeze_lic(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwid_t dstcpu;
        xwer_t rc;

        XWOS_BUG_ON(!(XWSDOBJ_DST_RUNNING & tcb->state));

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwlk_splk_lock_cpuirqsv(&xwsd->pm.lock, &cpuirq);
        xwlk_splk_lock(&tcb->stlock);
        if (XWSDOBJ_DST_FREEZABLE & tcb->state) {
                xwbop_c0m(xwsq_t, &tcb->state,
                          XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_FREEZABLE);
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_FROZEN);
                if (XWSDOBJ_DST_MIGRATING & tcb->state) {
                        xwlk_splk_unlock(&tcb->stlock);
                        xwos_thrd_outmigrate_frozen_lic(tcb);
                        dstcpu = tcb->migration.dst;
                        xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                        soc_thrd_immigrate(tcb, dstcpu);
                        xwlk_splk_lock_cpuirqsv(&xwsd->pm.lock, &cpuirq);
                } else {
                        xwlk_splk_unlock(&tcb->stlock);
                        xwsd->pm.frz_thrd_cnt++;
                        xwlib_bclst_add_tail(&xwsd->pm.frzlist, &tcb->frznode);
                }
                xwlk_splk_lock(&xwsd->tcblistlock);
                if (xwsd->pm.frz_thrd_cnt == xwsd->thrd_num) {
                        xwlk_splk_unlock(&xwsd->tcblistlock);
                        xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                        xwos_scheduler_enpmpt(xwsd);
                        xwos_scheduler_notify_allfrz_lic(xwsd);
                } else {
                        xwlk_splk_unlock(&xwsd->tcblistlock);
                        xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                        xwos_scheduler_enpmpt(xwsd);
                        xwos_scheduler_req_swcx(xwsd);
                }
                rc = OK;
        } else {
                xwlk_splk_unlock(&tcb->stlock);
                xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                rc = -ECANCELED;
        }
        return rc;
}

/**
 * @brief XWOS API：线程自我冻结
 * @return 错误码
 * @retval OK: OK
 * @retval -EPERM: 不允许冻结
 * @retval -ECANCELED: 冻结被取消
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 冻结操作，只可由线程自身执行
 */
__xwos_api
xwer_t xwos_cthrd_freeze(void)
{
        struct xwos_tcb * ctcb;
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = xwos_scheduler_dspmpt_lc();
        ctcb = xwos_scheduler_get_ctcb(xwsd);
        xwlk_splk_lock_cpuirqsv(&ctcb->stlock, &cpuirq);
        if (XWSDOBJ_DST_FREEZABLE & ctcb->state) {
                xwlk_splk_unlock_cpuirqrs(&ctcb->stlock, cpuirq);
                rc = soc_thrd_freeze_lc(ctcb);
        } else {
                xwlk_splk_unlock_cpuirqrs(&ctcb->stlock, cpuirq);
                xwos_scheduler_enpmpt(xwsd);
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief 解冻线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在线程所属的CPU的中断中执行。
 * - 此函数只能在持有锁xwsd->pm.lock时调用。
 */
__xwos_code
xwer_t xwos_thrd_thaw_lic_pmlk(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        xwpr_t prio;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        XWOS_BUG_ON(xwsd != xwos_scheduler_get_lc());

        xwlk_splk_lock(&tcb->stlock);
        if (XWSDOBJ_DST_FREEZABLE & tcb->state) {
                xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_FREEZABLE);
                xwlk_splk_unlock(&tcb->stlock);
                rc = -ECANCELED;
        } else if (XWSDOBJ_DST_FROZEN & tcb->state) {
                prio = tcb->dprio.r;
                xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_FROZEN);
                tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
                xwlk_splk_unlock(&tcb->stlock);
                xwlib_bclst_del_init(&tcb->frznode);
                xwsd->pm.frz_thrd_cnt--;
                rc = xwos_thrd_rq_add_tail(tcb, prio);
        } else {
                xwlk_splk_unlock(&tcb->stlock);
                rc = -EPERM;
        }
        return rc;
}

/**
 * @brief XWOS API：判断当前线程是否可以冻结
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_api
bool xwos_cthrd_shld_frz(void)
{
        struct xwos_tcb * ctcb;
        bool ret;

        ctcb = xwos_scheduler_get_ctcb_lc();
        ret = !!(ctcb->state & XWSDOBJ_DST_FREEZABLE);
        return ret;
}

/**
 * @brief XWOS API：判断当前线程是否可以退出
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数类似于@ref xwos_cthrd_wait_exit()，但不会阻塞，只会立即返回
 *   true或false，通常用在线程主循环的循环条件中。例如：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwos_cthrd_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 */
__xwos_api
bool xwos_cthrd_shld_stop(void)
{
        struct xwos_tcb * ctcb;

        ctcb = xwos_scheduler_get_ctcb_lc();
        return !!(XWSDOBJ_DST_EXITING & ctcb->state);
}

/**
 * @brief XWOS API：判断当前线程是否可以冻结，如果是就冻结线程，
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
 * - 此函数在@ref xwos_cthrd_shld_stop()的基础上增加了对冻结条件是否满足
 *   的判断，如果可以冻结，就在函数中将线程冻结。
 * - 通常线程冻结用于处理系统的低功耗事件或将线程在不同的CPU中进行迁移。
 * - 需要考虑线程的冻结，通常将线程的主循环写成：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwos_cthrd_frz_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 */
__xwos_api
bool xwos_cthrd_frz_shld_stop(bool * frozen)
{
        struct xwos_tcb * ctcb;
        bool frz;
        xwer_t rc;

        frz = false;
        if (xwos_cthrd_shld_frz()) {
                rc = xwos_cthrd_freeze();
                if (OK == rc) {
                        frz = true;
                }/* else {} */
        }/* else {} */
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }/* else {} */
        ctcb = xwos_scheduler_get_ctcb_lc();
        return !!(XWSDOBJ_DST_EXITING & ctcb->state);
}

/**
 * @brief 将线程迁移进目标CPU
 * @param tcb: (I) 线程控制块对象的指针
 * @note
 * - 此函数只能在目标CPU的中断上下文中执行；
 * - 此函数假设线程控制块对象已经被引用，执行过程中不会成为野指针
 */
__xwos_code
void xwos_thrd_immigrate_lic(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * new;
        xwreg_t cpuirq;
        xwpr_t prio;
        xwer_t rc;

        XWOS_BUG_ON(!(XWSDOBJ_DST_FROZEN & tcb->state));
        XWOS_BUG_ON(!(XWSDOBJ_DST_MIGRATING & tcb->state));

        new = xwos_scheduler_get_lc();
        xwlk_splk_lock_cpuirqsv(&new->pm.lock, &cpuirq);
        xwlk_splk_lock(&new->tcblistlock);
        tcb->migration.dst = 0;
        xwlib_bclst_add_tail(&new->tcblist, &tcb->tcbnode);
        new->thrd_num++;
        xwmb_smp_store_release(struct xwos_scheduler *, &tcb->xwsd, new);
        xwlk_splk_unlock(&new->tcblistlock);
        if (xwos_scheduler_get_pm_state(new) <= XWOS_SCHEDULER_WKLKCNT_SUSPENDING) {
                xwlk_splk_lock(&tcb->stlock);
                xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_MIGRATING);
                xwlk_splk_unlock(&tcb->stlock);
                xwlib_bclst_add_tail(&new->pm.frzlist, &tcb->frznode);
                new->pm.frz_thrd_cnt++;
                xwlk_splk_unlock_cpuirqrs(&new->pm.lock, cpuirq);
        } else {
                xwlk_splk_lock(&tcb->stlock);
                prio = tcb->dprio.r;
                xwbop_c0m(xwsq_t, &tcb->state,
                          XWSDOBJ_DST_FROZEN | XWSDOBJ_DST_MIGRATING);
                tcb->dprio.r = XWOS_SD_PRIORITY_INVALID;
                xwlk_splk_unlock(&tcb->stlock);
                xwlk_splk_unlock_cpuirqrs(&new->pm.lock, cpuirq);
                rc = xwos_thrd_rq_add_tail(tcb, prio);
                XWOS_BUG_ON(rc < 0);
                xwos_scheduler_chkpmpt(new);
        }
}

/**
 * @brief 将线程从当前CPU中迁出
 * @param tcb: (I) 线程控制块对象的指针
 * @note
 * - 此函数只能在线程所属的CPU的中断上下文中执行；
 * - 此函数被调用时需要获得当前CPU调度器的锁pm.lock并且关闭本地CPU的中断；
 * - 此函数假设线程控制块对象已经被引用，执行过程中不会成为野指针；
 */
static __xwos_code
void xwos_thrd_outmigrate_frozen_lic(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * old;

        XWOS_BUG_ON(!(XWSDOBJ_DST_FROZEN & tcb->state));
        XWOS_BUG_ON(!(XWSDOBJ_DST_MIGRATING & tcb->state));

        xwmb_smp_load_acquire(struct xwos_scheduler *, old, &tcb->xwsd);
        XWOS_BUG_ON(old != xwos_scheduler_get_lc());
        xwlk_splk_lock(&old->tcblistlock);
        xwlib_bclst_del_init(&tcb->tcbnode);
        old->thrd_num--;
        xwmb_smp_store_release(struct xwos_scheduler *, &tcb->xwsd, NULL);
        xwlk_splk_unlock(&old->tcblistlock);
}

/**
 * @brief 申请冻结线程，以便将线程从本地CPU迁移到另一个CPU
 * @param tcb: (I) 线程控制块
 * @param dstcpu: (I) 目标CPU的ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EINPROGRESS: 线程处于迁移的过程中
 * @retval -EALREADY: 线程已经被冻结
 * @note
 * - 此函数只可在线程所属的CPU的中断上下文中被调用；
 * - 此函数假设线程控制块对象已被引用，执行过程中不会成为野指针。
 */
static __xwos_code
xwer_t xwos_thrd_outmigrate_reqfrz_lic(struct xwos_tcb * tcb, xwid_t dstcpu)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        xwlk_splk_lock_cpuirqsv(&xwsd->pm.lock, &cpuirq);
        xwlk_splk_lock(&tcb->stlock);
        if (XWSDOBJ_DST_MIGRATING & tcb->state) {
                xwlk_splk_unlock(&tcb->stlock);
                xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                rc = -EINPROGRESS;
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_MIGRATING);
                if (XWSDOBJ_DST_FROZEN & tcb->state) {
                        xwlk_splk_unlock(&tcb->stlock);
                        tcb->migration.dst = dstcpu;
                        xwos_thrd_outmigrate_frozen_lic(tcb);
                        xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                        soc_thrd_immigrate(tcb, dstcpu);
                } else {
                        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_FREEZABLE);
                        xwlk_splk_unlock(&tcb->stlock);
                        tcb->migration.dst = dstcpu;
                        xwlk_splk_unlock_cpuirqrs(&xwsd->pm.lock, cpuirq);
                }
                rc = OK;
        }
        return rc;
}

/**
 * @brief 从本地CPU中移出线程，并准备迁移入目标CPU
 * @param tcb: (I) 线程控制块的指针
 * @param dstcpu: (I) 目标CPU的ID
 * @return 错误码
 * @note
 * - 此函数只能在线程所属的CPU中执行。
 * - 此函数假设线程控制块对象已经被引用，执行过程中不会成为野指针。
 */
__xwos_code
xwer_t xwos_thrd_outmigrate_lic(struct xwos_tcb * tcb, xwid_t dstcpu)
{
        struct xwos_scheduler * xwsd, * local;
        xwer_t rc;

        xwmb_smp_load_acquire(struct xwos_scheduler *, xwsd, &tcb->xwsd);
        local = xwos_scheduler_get_lc();
        XWOS_BUG_ON(xwsd != local);
        rc = xwos_thrd_outmigrate_reqfrz_lic(tcb, dstcpu);
        return rc;
}

/**
 * @brief XWOS API：将线程从本地CPU迁移到其他CPU
 * @param tcb: (I) 线程控制块的指针
 * @param dstcpu: (I) 目标CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个tid，不可重入
 */
__xwos_api
xwer_t xwos_thrd_migrate(struct xwos_tcb * tcb, xwid_t dstcpu)
{
        xwer_t rc;

        if (__unlikely(dstcpu >= CPUCFG_CPU_NUM)) {
                rc = -ENODEV;
                goto err_badcpuid;
        }
        rc = soc_thrd_outmigrate(tcb, dstcpu);
        if (__unlikely(rc < 0)) {
                goto err_soc_thrd_outmigrate;
        }
        return OK;

err_soc_thrd_outmigrate:
err_badcpuid:
        return rc;
}
