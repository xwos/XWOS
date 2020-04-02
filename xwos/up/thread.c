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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <soc_sched.h>
#include <xwos/up/irq.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/rtrq.h>
#if (1 == SOUPRULE_SD_WQ_RT)
  #include <xwos/up/rtwq.h>
#endif /* SOUPRULE_SD_WQ_RT */
#if (1 == SOUPRULE_SD_WQ_PL)
  #include <xwos/up/plwq.h>
#endif /* SOUPRULE_SD_WQ_PL */
#include <xwos/up/tt.h>
#include <xwos/up/thread.h>
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
  #include <xwos/up/lock/mutex.h>
  #include <xwos/up/mutextree.h>
#endif /* XWUPCFG_LOCK_MTX */
#if defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)
  #include <xwos/up/lock/fakemutex.h>
#endif /* XWUPCFG_LOCK_FAKEMTX */
#if defined(XWUPCFG_SYNC_PLSMR) && (1 == XWUPCFG_SYNC_PLSMR)
  #include <xwos/up/sync/plsmr.h>
#endif /* XWUPCFG_SYNC_PLSMR */
#if defined(XWUPCFG_SYNC_RTSMR) && (1 == XWUPCFG_SYNC_RTSMR)
  #include <xwos/up/sync/rtsmr.h>
#endif /* XWUPCFG_SYNC_RTSMR */
#if defined(XWUPCFG_SYNC_CDT) && (1 == XWUPCFG_SYNC_CDT)
  #include <xwos/up/sync/condition.h>
#endif /* XWUPCFG_SYNC_CDT */

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
void xwos_thrd_activate(struct xwos_tcb * tcb,
                        const char * name,
                        xwos_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr);

static __xwos_code
void xwos_thrd_launch(struct xwos_tcb * tcb, xwos_thrd_f mainfunc, void * arg);

#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
static __xwos_code
xwer_t xwos_thrd_terminate_unlock_cb(struct xwos_tcb * tcb);
#endif /* XWUPCFG_SD_THRD_EXIT */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 从线程控制块对象缓存中申请一个对象
 * @return 线程控制块对象的指针
 */
static __xwos_code
struct xwos_tcb * xwos_tcb_alloc(void)
{
        union {
                struct xwos_tcb * tcb;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwos_tcb), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.tcb = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.tcb;
}

/**
 * @brief 释放线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 */
static __xwos_code
void xwos_tcb_free(struct xwos_tcb * tcb)
{
        xwmm_kma_free(tcb);
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
#else
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
#endif
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
#else
        return xwmm_kma_free(stk);
#endif
}

/**
 * @brief 激活线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈的首地址指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，取值：@ref xwos_sdobj_attr_em中的一项
 */
static __xwos_code
void xwos_thrd_activate(struct xwos_tcb * tcb,
                        const char * name,
                        xwos_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        /* base info */
        tcb->state = XWSDOBJ_DST_UNKNOWN;
        tcb->attribute = attr;

#if (1 == SOUPRULE_SD_THRD_FREEZE)
        /* frozen state info */
        xwlib_bclst_init_node(&tcb->frznode);
#endif /* (1 == SOUPRULE_SD_THRD_FREEZE) */

        /* ready state info */
        xwlib_bclst_init_node(&tcb->rqnode);

        /* sleeping state info */
        xwos_ttn_init(&tcb->ttn, (xwptr_t)tcb, XWOS_TTN_TYPE_THREAD);

        /* blocking state info */
        xwos_wqn_init(&tcb->wqn, tcb);

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
        /* mutex tree */
        xwos_mtxtree_init(&tcb->mtxtree);
#endif /* XWUPCFG_LOCK_MTX */

        /* priority */
        if (priority >= XWOS_SD_PRIORITY_RT_NUM) {
                priority = XWOS_SD_PRIORITY_RT_MAX;
        } else if (priority <= XWOS_SD_PRIORITY_INVALID) {
                priority = XWOS_SD_PRIORITY_RT_MIN;
        }
        tcb->prio.s = priority;
        tcb->prio.d = priority;

        /* init stack */
        tcb->stack.name = name;
        tcb->stack.size = stack_size;
        tcb->stack.base = stack;
#if defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK)
        tcb->stack.sp = tcb->stack.base + (stack_size >> 2);
#elif defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK)
        tcb->stack.sp = tcb->stack.base + (stack_size >> 2) - 1;
#elif defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK)
        tcb->stack.sp = tcb->stack.base - 1;
#elif defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK)
        tcb->stack.sp = tcb->stack.base;
#else /* XWMMCFG_EA_STACK */
  #error "Unknown stack type!"
#endif /* !XWMMCFG_EA_STACK */
        /* init completion */
#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
        xwsync_cdt_init(&tcb->completion);
#endif /* XWUPCFG_SD_THRD_EXIT */

        /* init tcb node */
        xwlib_bclst_init_node(&tcb->tcbnode);

        /* add to ready queue */
        if (mainfunc) {
                xwos_thrd_launch(tcb, mainfunc, arg);
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_STANDBY);
        }
}

/**
 * @brief 使得线程控制块对象无效
 * @param tcb: (I) 线程控制块对象的指针
 */
static __xwos_code
void xwos_thrd_deactivate(struct xwos_tcb * tcb)
{
        XWOS_UNUSED(tcb);
}

/**
 * @brief 加载线程
 * @param tcb: (I) 线程控制块对象的指针
 * @param mainfunc: (I) 线程主函数
 * @param arg: (I) 线程主函数的参数
 */
static __xwos_code
void xwos_thrd_launch(struct xwos_tcb * tcb, xwos_thrd_f mainfunc, void * arg)
{
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        /* add to ready queue */
        xwsd = xwos_scheduler_get_lc();
        tcb->stack.main = mainfunc;
        tcb->stack.arg = arg;
        soc_scheduler_init_sdobj_stack(&tcb->stack, tcb->attribute);
        xwos_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_STANDBY);
        xwos_thrd_rq_add_tail(tcb);
        xwsd->thrd_num++;
        xwlib_bclst_add_tail(&xwsd->tcblist, &tcb->tcbnode);
        xwos_cpuirq_restore_lc(cpuirq);
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
 * - 重入性：对于同一个 *tcb* ，不可重入
 * @note
 * - 静态初始化线程需预先定义线程控制块对象和线程栈数组，通常定义为全局变量；
 * - 栈数组的首地址与大小，必须要满足CPU的ABI规则，例如ARM，就需要8字节对齐，
 *   因此在定义栈数组时需要__aligned(8)来修饰，且大小是8的倍数
 */
__xwos_api
xwer_t xwos_thrd_init(struct xwos_tcb * tcb,
                      const char * name,
                      xwos_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr)
{
        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stack), "nullptr", -EFAULT);

        xwos_thrd_activate(tcb, name,
                           mainfunc, arg,
                           stack, stack_size,
                           priority, attr);
        return OK;
}

/**
 * @brief XWOS API：销毁线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *tcb* ，不可重入
 */
__xwos_api
xwer_t xwos_thrd_destroy(struct xwos_tcb * tcb)
{
        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        xwos_thrd_deactivate(tcb);
        return OK;
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
 */
__xwos_api
xwer_t xwos_thrd_create(struct xwos_tcb ** tcbpbuf,
                        const char * name,
                        xwos_thrd_f mainfunc, void * arg,
                        xwsz_t stack_size, xwpr_t priority,
                        xwsq_t attr)
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

        xwos_thrd_activate(tcb, name,
                           mainfunc, arg,
                           stk, stack_size,
                           priority, attr);
        *tcbpbuf = tcb;
        return OK;

err_tcb_alloc:
        xwos_thrd_stack_free(stk);
err_stack_alloc:
        *tcbpbuf = NULL;
        return rc;
}

/**
 * @brief XWOS API：删除动态创建线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *tcb* ，不可重入
 */
__xwos_api
xwer_t xwos_thrd_delete(struct xwos_tcb * tcb)
{
        xwstk_t * base;

        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        xwos_thrd_deactivate(tcb);
        base = ((struct xwos_tcb *)tcb)->stack.base;
        xwos_thrd_stack_free(base);
        xwos_tcb_free(tcb);
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
#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
        struct xwos_tcb * ctcb;

        ctcb = xwos_scheduler_get_ctcb_lc();
        soc_thrd_exit_lc(ctcb, rc);
#else /* XWUPCFG_SD_THRD_EXIT */
        XWOS_UNUSED(rc);
        xwos_cthrd_wait_exit();
#endif /* !XWUPCFG_SD_THRD_EXIT */
}

/**
 * @brief XWOS软中断：退出线程
 * @param tcb: (I) 线程控制块对象的指针
 * @param rc: (I) 线程的返回值
 */
__xwos_code
xwer_t xwos_thrd_exit_lic(struct xwos_tcb * tcb, xwer_t rc)
{
#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        xwsd = xwos_scheduler_get_lc();
        xwos_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_EXITING);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_STANDBY);
        tcb->stack.arg = err_ptr(rc);
        xwos_cpuirq_restore_lc(cpuirq);
        xwsync_cdt_broadcast(&tcb->completion);
        xwos_cpuirq_disable_lc();
        xwlib_bclst_del_init(&tcb->tcbnode);
        xwsd->thrd_num--;
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
        if (xwsd->lpm.frz_thrd_cnt == xwsd->thrd_num) {
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_notify_lpm();
        } else {
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_req_swcx();
        }
#else /* XWUPCFG_SD_LPM */
        xwos_cpuirq_restore_lc(cpuirq);
        xwos_scheduler_req_swcx();
#endif /* !XWUPCFG_SD_LPM */
#else /* XWUPCFG_SD_THRD_EXIT */
        XWOS_UNUSED(tcb);
        XWOS_UNUSED(rc);
#endif /* !XWUPCFG_SD_THRD_EXIT */
        return OK;
}

#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
/**
 * @brief @ref xwos_thrd_terminate()中使用的回调锁的解锁函数
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval OK: OK
 */
static __xwos_code
xwer_t xwos_thrd_terminate_unlock_cb(struct xwos_tcb * tcb)
{
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
 * @retval -EFAULT: 空指针
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
        xwos_cpuirq_save_lc(&cpuirq);
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
                }
        }
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}

#else /* XWUPCFG_SD_THRD_EXIT */

/**
 * @brief XWOS API：终止线程并等待它的返回值
 * @param tcb: (I) 线程控制块对象的指针
 * @param rc: (O) 指向缓冲区的指针，通过此缓冲区返回被终止线程的返回值
 * @return 错误码
 * @retval -ENOSYS: 系统配置为不允许线程退出
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_thrd_terminate(struct xwos_tcb * tcb, xwer_t * trc)
{
        XWOS_UNUSED(tcb);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}
#endif /* !XWUPCFG_SD_THRD_EXIT */

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
        xwreg_t cpuirq;

        ctcb = xwos_scheduler_get_ctcb_lc();
        xwos_cpuirq_save_lc(&cpuirq);
#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
        while (!(XWSDOBJ_DST_EXITING & ctcb->state)) {
                xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &ctcb->state, XWSDOBJ_DST_BLOCKING);
                ctcb->wqn.wq = NULL;
                ctcb->wqn.type = XWOS_WQTYPE_NULL;
                ctcb->wqn.rsmrs = XWOS_WQN_RSMRS_UNKNOWN;
                ctcb->wqn.cb = xwos_thrd_wqn_callback;
                xwos_cpuirq_enable_lc();
                xwos_scheduler_req_swcx();
                xwos_cpuirq_disable_lc();
        }
#else /* XWUPCFG_SD_THRD_EXIT */
        while (1) {
                xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &ctcb->state, XWSDOBJ_DST_BLOCKING);
                ctcb->wqn.wq = NULL;
                ctcb->wqn.type = XWOS_WQTYPE_NULL;
                ctcb->wqn.rsmrs = XWOS_WQN_RSMRS_UNKNOWN;
                ctcb->wqn.cb = xwos_thrd_wqn_callback;
                xwos_cpuirq_enable_lc();
                xwos_scheduler_req_swcx();
                xwos_cpuirq_disable_lc();
        }
#endif /* !XWUPCFG_SD_THRD_EXIT */
        xwos_cpuirq_restore_lc(cpuirq);
}

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
/**
 * @brief 改变线程的动态优先级一次
 * @param tcb: (I) 线程控制块对象的指针
 * @param dprio: (I) 动态优先级
 * @param pmtx: (O) 指向缓冲区的指针，通过此缓冲区返回互斥锁对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ESTALE: 状态已经被其他CPU改变
 * @note
 * - 此函数将线程(tcb)的优先级改成(dprio)，并返回接下来需要修改优先级的互斥锁的指针。
 */
__xwos_code
void xwos_thrd_chprio_once(struct xwos_tcb * tcb, xwpr_t dprio,
                           struct xwlk_mtx ** pmtx)
{
        if ((XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_FROZEN) & tcb->state) {
                tcb->prio.d = dprio;
        } else if (XWSDOBJ_DST_READY & tcb->state) {
                if (tcb->prio.d != dprio) {
                        xwos_thrd_rq_remove(tcb);
                        tcb->prio.d = dprio;
                        xwos_thrd_rq_add_tail(tcb);
                }
        } else if (XWSDOBJ_DST_BLOCKING & tcb->state) {
                if (tcb->prio.d == dprio) {
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
                } else if (XWOS_WQTYPE_MTX == tcb->wqn.type) {
                        struct xwlk_mtx * mtx;

                        mtx = container_of(tcb->wqn.wq, struct xwlk_mtx, rtwq);
                        xwos_rtwq_remove(&mtx->rtwq, &tcb->wqn);
                        tcb->prio.d = dprio;
                        xwos_rtwq_add(&mtx->rtwq, &tcb->wqn, dprio);
                        *pmtx = mtx;
#endif /* XWUPCFG_LOCK_MTX */
#if defined(XWUPCFG_SYNC_RTSMR) && (1 == XWUPCFG_SYNC_RTSMR)
                } else if (XWOS_WQTYPE_RTSMR == tcb->wqn.type) {
                        struct xwsync_rtsmr * smr;

                        smr = container_of(tcb->wqn.wq, struct xwsync_rtsmr, rtwq);
                        xwos_rtwq_remove(&smr->rtwq, &tcb->wqn);
                        tcb->prio.d = dprio;
                        xwos_rtwq_add(&smr->rtwq, &tcb->wqn, dprio);
                } else {
#endif /* XWUPCFG_SYNC_RTSMR */
                        tcb->prio.d = dprio;
                }
        } else {
                XWOS_BUG_ON(XWSDOBJ_DST_STANDBY & tcb->state);
                tcb->prio.d = dprio;
        }
}

/**
 * @brief 重新设定线程的动态优先级
 * @param tcb: (I) 线程控制块对象的指针
 */
__xwos_code
void xwos_thrd_chprio(struct xwos_tcb * tcb)
{
        struct xwlk_mtx * unused;
        struct xwos_mtxtree * mt;
        xwpr_t dprio;

        mt = &tcb->mtxtree;
        dprio = tcb->prio.s > mt->maxprio ? tcb->prio.s : mt->maxprio;
        xwos_thrd_chprio_once(tcb, dprio, &unused);
}
#endif /* XWUPCFG_LOCK_MTX */

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param tcb: (I) 线程控制块对象的指针
 * @param prio: (I) 动态优先级
 * @return 错误码
 * @retval OK: OK
 * @retval -EEXIST: 线程已经存在
 */
__xwos_code
xwer_t xwos_thrd_rq_add_head(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        xwer_t rc;

        xwsd = xwos_scheduler_get_lc();
        xwrtrq = &xwsd->rq.rt;
        if ((XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_READY) & tcb->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_READY);
                xwos_rtrq_add_head(xwrtrq, tcb);
                rc = OK;
        }
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param tcb: (I) 线程控制块对象的指针
 * @param prio: (I) 动态优先级
 * @return 错误码
 * @retval OK: OK
 * @retval -EEXIST: 线程已经存在
 */
__xwos_code
xwer_t xwos_thrd_rq_add_tail(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;
        xwer_t rc;

        xwsd = xwos_scheduler_get_lc();
        xwrtrq = &xwsd->rq.rt;
        if ((XWSDOBJ_DST_RUNNING | XWSDOBJ_DST_READY) & tcb->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_READY);
                xwos_rtrq_add_tail(xwrtrq, tcb);
                rc = OK;
        }
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
void xwos_thrd_rq_remove(struct xwos_tcb * tcb)
{
        struct xwos_scheduler * xwsd;
        struct xwos_rtrq * xwrtrq;

        xwsd = xwos_scheduler_get_lc();
        xwrtrq = &xwsd->rq.rt;
        xwos_rtrq_remove(xwrtrq, tcb);
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_READY);
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
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_cpuirq_save_lc(&cpuirq);
        if (XWSDOBJ_DST_UNINTERRUPTED & tcb->state) {
                rc = -EPERM;
                xwos_cpuirq_restore_lc(cpuirq);
        } else if (XWSDOBJ_DST_BLOCKING & tcb->state) {
                xwos_wqn_f cb;

                if (XWOS_WQTYPE_NULL == tcb->wqn.type) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWOS_WQTYPE_UNKNOWN;
                        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_INTR;
                        cb = tcb->wqn.cb;
                        tcb->wqn.cb = NULL;
                        xwos_cpuirq_restore_lc(cpuirq);
                        cb(tcb);
                        rc = OK;
#if defined(XWUPCFG_SYNC_PLSMR) && (1 == XWUPCFG_SYNC_PLSMR)
                } else if (XWOS_WQTYPE_PLSMR == tcb->wqn.type) {
                        struct xwsync_plsmr * smr;

                        smr = container_of(tcb->wqn.wq,
                                           struct xwsync_plsmr,
                                           plwq);
                        xwos_cpuirq_restore_lc(cpuirq);
                        rc = xwsync_plsmr_intr(smr, &tcb->wqn);
#endif /* XWUPCFG_SYNC_PLSMR */
#if defined(XWUPCFG_SYNC_RTSMR) && (1 == XWUPCFG_SYNC_RTSMR)
                } else if (XWOS_WQTYPE_RTSMR == tcb->wqn.type) {
                        struct xwsync_rtsmr * smr;

                        smr = container_of(tcb->wqn.wq,
                                           struct xwsync_rtsmr,
                                           rtwq);
                        xwos_cpuirq_restore_lc(cpuirq);
                        rc = xwsync_rtsmr_intr(smr, &tcb->wqn);
#endif /* XWUPCFG_SYNC_RTSMR */
#if defined(XWUPCFG_SYNC_CDT) && (1 == XWUPCFG_SYNC_CDT)
                } else if (XWOS_WQTYPE_CDT == tcb->wqn.type) {
                        struct xwsync_cdt * smr;

                        smr = container_of(tcb->wqn.wq,
                                           struct xwsync_cdt,
                                           wq);
                        xwos_cpuirq_restore_lc(cpuirq);
                        rc = xwsync_cdt_intr(smr, &tcb->wqn);
#endif /* XWUPCFG_SYNC_CDT */
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
                } else if (XWOS_WQTYPE_MTX == tcb->wqn.type) {
                        struct xwlk_mtx * mtx;

                        mtx = container_of(tcb->wqn.wq,
                                           struct xwlk_mtx,
                                           rtwq);
                        xwos_cpuirq_restore_lc(cpuirq);
                        rc = xwlk_mtx_intr(mtx, tcb);
#endif /* XWUPCFG_LOCK_MTX */
                } else {
                        xwos_cpuirq_restore_lc(cpuirq);
                        XWOS_BUG();
                }
        } else if (XWSDOBJ_DST_SLEEPING & tcb->state) {
                struct xwos_scheduler * xwsd;
                struct xwos_tt * xwtt;

                xwsd = xwos_scheduler_get_lc();
                xwtt = &xwsd->tt;
                xwlk_sqlk_wr_lock(&xwtt->lock);
                rc = xwos_tt_remove_locked(xwtt, &tcb->ttn);
                if (OK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
                        xwlk_sqlk_wr_unlock(&xwtt->lock);
                        xwos_thrd_wakeup(tcb);
                        xwos_cpuirq_restore_lc(cpuirq);
                        xwos_scheduler_chkpmpt();
                } else {
                        xwlk_sqlk_wr_unlock(&xwtt->lock);
                        xwos_cpuirq_restore_lc(cpuirq);
                }
        } else {
                rc = -EINVAL;
                xwos_cpuirq_restore_lc(cpuirq);
        }
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
        xwos_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_SLEEPING);
        xwos_thrd_wakeup(tcb);
        xwos_cpuirq_restore_lc(cpuirq);
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
        tcb->ttn.wkuprs = XWOS_TTN_WKUPRS_UNKNOWN;
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
        xwos_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state,
                  XWSDOBJ_DST_BLOCKING | XWSDOBJ_DST_UNINTERRUPTED);
        xwos_thrd_wakeup(tcb);
        xwos_cpuirq_restore_lc(cpuirq);
        xwos_scheduler_chkpmpt();
}

#if (1 == SOUPRULE_SD_WQ_RT)
/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param tcb: (I) 线程控制块对象的指针
 * @param xwrtwq: (I) 实时（红黑树）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwos_code
void xwos_thrd_eq_rtwq(struct xwos_tcb * tcb, struct xwos_rtwq * xwrtwq,
                       xwu16_t type)
{
        tcb->wqn.wq = xwrtwq;
        tcb->wqn.type = type;
        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_UNKNOWN;
        tcb->wqn.cb = xwos_thrd_wqn_callback;
        xwos_rtwq_add(xwrtwq, &tcb->wqn, tcb->prio.d);
}
#endif /* (1 == SOUPRULE_SD_WQ_RT) */

#if (1 == SOUPRULE_SD_WQ_PL)
/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param tcb: (I) 线程控制块对象的指针
 * @param xwplwq: (I) 管道（双循环链表）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwos_code
void xwos_thrd_eq_plwq(struct xwos_tcb * tcb, struct xwos_plwq * xwplwq,
                       xwu16_t type)
{
        tcb->wqn.wq = xwplwq;
        tcb->wqn.type = type;
        tcb->wqn.rsmrs = XWOS_WQN_RSMRS_UNKNOWN;
        tcb->wqn.cb = xwos_thrd_wqn_callback;
        xwos_plwq_add_tail(xwplwq, &tcb->wqn);
}
#endif /* (1 == SOUPRULE_SD_WQ_PL) */

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

        ctcb = xwos_scheduler_get_ctcb_lc();
        xwos_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
        xwos_thrd_rq_add_tail(ctcb);
        xwos_cpuirq_enable_lc();
        xwos_scheduler_req_swcx();
        xwos_cpuirq_restore_lc(cpuirq);
}

#if (1 == SOUPRULE_SD_THRD_DO_UNLOCK)
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
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
    (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX))
                rc = xwlk_mtx_unlock(ulk.xwos.mtx);
#endif /* XWUPCFG_LOCK_MTX || XWUPCFG_LOCK_FAKEMTX */
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
#endif /* (1 == SOUPRULE_SD_THRD_DO_UNLOCK) */

#if (1 == SOUPRULE_SD_THRD_DO_LOCK)
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
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
    (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX))
                if (xwtm) {
                        rc = xwlk_mtx_timedlock(ulk.xwos.mtx, xwtm);
                } else {
                        rc = xwlk_mtx_lock(ulk.xwos.mtx);
                }
#endif /* XWUPCFG_LOCK_MTX || XWUPCFG_LOCK_FAKEMTX */
                break;
        case XWLK_TYPE_MTX_UNINTR:
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
    (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX))
                rc = xwlk_mtx_lock_unintr(ulk.xwos.mtx);
#endif /* XWUPCFG_LOCK_MTX || XWUPCFG_LOCK_FAKEMTX */
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
                XWOS_UNUSED(xwtm);
                break;
        }
        return rc;
}
#endif /* (1 == SOUPRULE_SD_THRD_DO_LOCK) */

#if defined(XWUPCFG_SYNC_CDT) && (1 == XWUPCFG_SYNC_CDT)
/**
 * @brief 继续已经暂停的线程
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
                }
                rc = -ETIMEDOUT;
        } else {
                rc = xwsync_cdt_do_timedwait(&ctcb->self, ctcb,
                                             lock, lktype, lkdata, datanum,
                                             xwtm, lkst);
        }
        return rc;
}
#endif /* XWUPCFG_SYNC_CDT */

/**
 * @brief XWOS API：线程睡眠一段时间
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EINTR: 睡眠过程被中断
 * @retval -ETIMEDOUT: 时间戳为负数
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
        xwreg_t cpuirq;

        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);

        if (__unlikely(xwtm_cmp(*xwtm, 0) <= 0)) {
                rc = -ETIMEDOUT;
        } else {
                ctcb = xwos_scheduler_get_ctcb_lc();
                xwsd = xwos_scheduler_get_lc();
                xwtt = &xwsd->tt;
                hwt = &xwtt->hwt;
                currtick = xwos_syshwt_get_timetick(hwt);
                expected = xwtm_add_safely(currtick, *xwtm);
                xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
                rc = xwos_scheduler_wakelock_lock();
                if (__unlikely(rc < 0)) {
                        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        /* The scheduler is in hibernation state. Current thread need to
                           be frozen. So the blocking state needs to be interrupted. */
                        rc = -EINTR;
                } else
#endif /* XWUPCFG_SD_LPM */
                {
                        XWOS_BUG_ON((XWSDOBJ_DST_SLEEPING | XWSDOBJ_DST_READY |
                                     XWSDOBJ_DST_STANDBY | XWSDOBJ_DST_FROZEN)
                                    & ctcb->state);
                        /* set sleeping state */
                        xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
                        xwbop_s1m(xwsq_t, &ctcb->state, XWSDOBJ_DST_SLEEPING);
                        /* add to time tree */
                        xwos_thrd_tt_add_locked(ctcb, xwtt, expected, cpuirq);
                        /* enable local CPU IRQ to enable schedule */
                        xwlk_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
                        xwos_scheduler_wakelock_unlock();
#endif /* XWUPCFG_SD_LPM */
                        xwos_scheduler_req_swcx();
                        xwos_cpuirq_restore_lc(cpuirq);
                        if (XWOS_TTN_WKUPRS_TIMEDOUT == ctcb->ttn.wkuprs) {
                                rc = OK;
                        } else if (XWOS_TTN_WKUPRS_INTR == ctcb->ttn.wkuprs) {
                                rc = -EINTR;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
                currtick = xwos_syshwt_get_timetick(hwt);
                *xwtm = xwtm_sub(expected, currtick);
        }
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
        xwer_t rc;

        XWOS_VALIDATE((NULL != origin), "nullptr", -EFAULT);

        ctcb = xwos_scheduler_get_ctcb_lc();
        xwsd = xwos_scheduler_get_lc();
        xwtt = &xwsd->tt;
        hwt = &xwtt->hwt;
        expected = xwtm_add_safely(*origin, inc);
        xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
        rc = xwos_scheduler_wakelock_lock();
        if (__unlikely(rc < 0)) {
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* The scheduler is in hibernation state. Current thread need to
                   be frozen. So the blocking state needs to be interrupted. */
                rc = -EINTR;
        } else
#endif /* XWUPCFG_SD_LPM */
        {
                XWOS_BUG_ON((XWSDOBJ_DST_SLEEPING | XWSDOBJ_DST_READY |
                             XWSDOBJ_DST_STANDBY | XWSDOBJ_DST_FROZEN)
                            & ctcb->state);
                /* set the sleeping state */
                xwbop_c0m(xwsq_t, &ctcb->state, XWSDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &ctcb->state, XWSDOBJ_DST_SLEEPING);
                /* add to time tree */
                xwos_thrd_tt_add_locked(ctcb, xwtt, expected, cpuirq);
                /* enable local CPU IRQ to enable schedule */
                xwlk_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SD_LPM) && (1 == XWUPCFG_SD_LPM)
                xwos_scheduler_wakelock_unlock();
#endif /* XWUPCFG_SD_LPM */
                xwos_scheduler_req_swcx();
                xwos_cpuirq_restore_lc(cpuirq);
                if (XWOS_TTN_WKUPRS_TIMEDOUT == ctcb->ttn.wkuprs) {
                        rc = OK;
                } else if (XWOS_TTN_WKUPRS_INTR == ctcb->ttn.wkuprs) {
                        rc = -EINTR;
                } else {
                        XWOS_BUG();
                        rc = -EBUG;
                }
                *origin = xwos_syshwt_get_timetick(hwt);
        }
        return rc;
}

/**
 * @brief XWOS API：冻结当前线程
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_cthrd_freeze(void)
{
#if (1 == SOUPRULE_SD_THRD_FREEZE)
        struct xwos_tcb * ctcb;
        xwer_t rc;

        xwos_scheduler_dspmpt_lc();
        ctcb = xwos_scheduler_get_ctcb_lc();
        rc = soc_thrd_freeze_lc(ctcb);
        return rc;
#else /* (1 == SOUPRULE_SD_THRD_FREEZE) */
        return -ENOSYS;
#endif /* !(1 == SOUPRULE_SD_THRD_FREEZE) */
}

/**
 * @brief 冻结线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在中断上下文中调用并且这个中断只能由@ref xwos_cthrd_freeze()触发。
 */
__xwos_code
xwer_t xwos_thrd_freeze_lic(struct xwos_tcb * tcb)
{
#if (1 == SOUPRULE_SD_THRD_FREEZE)
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;

        XWOS_BUG_ON(XWSDOBJ_DST_RUNNING != (tcb->state & (XWSDOBJ_DST_MASK)));
        xwsd = xwos_scheduler_get_lc();
        xwos_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &tcb->state, XWSDOBJ_DST_FROZEN);
        xwsd->lpm.frz_thrd_cnt++;
        xwlib_bclst_add_tail(&xwsd->lpm.frzlist, &tcb->frznode);
        xwos_scheduler_enpmpt_lc();
        if (xwsd->lpm.frz_thrd_cnt == xwsd->thrd_num) {
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_notify_lpm();
        } else {
                xwos_cpuirq_restore_lc(cpuirq);
                xwos_scheduler_req_swcx();
        }
        return OK;
#else /* (1 == SOUPRULE_SD_THRD_FREEZE) */
        XWOS_UNUSED(tcb);
        return -ENOSYS;
#endif /* (1 != SOUPRULE_SD_THRD_FREEZE) */
}

/**
 * @brief 解冻线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 */
__xwos_code
xwer_t xwos_thrd_thaw_lic(struct xwos_tcb * tcb)
{
#if (1 == SOUPRULE_SD_THRD_FREEZE)
        struct xwos_scheduler * xwsd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwsd = xwos_scheduler_get_lc();
        xwos_cpuirq_save_lc(&cpuirq);
        XWOS_BUG_ON(!(XWSDOBJ_DST_FROZEN & tcb->state));
        xwlib_bclst_del_init(&tcb->frznode);
        xwsd->lpm.frz_thrd_cnt--;
        xwbop_c0m(xwsq_t, &tcb->state, XWSDOBJ_DST_FROZEN);
        rc = xwos_thrd_rq_add_tail(tcb);
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
#else /* (1 == SOUPRULE_SD_THRD_FREEZE) */
        XWOS_UNUSED(tcb);
        return -ENOSYS;
#endif /* !(1 == SOUPRULE_SD_THRD_FREEZE) */
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
#if (1 == SOUPRULE_SD_THRD_FREEZE)
        return xwos_scheduler_tst_lpm();
#else /* (1 == SOUPRULE_SD_THRD_FREEZE) */
        return false;
#endif /* !(1 == SOUPRULE_SD_THRD_FREEZE) */
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
#if defined(XWUPCFG_SD_THRD_EXIT) && (1 == XWUPCFG_SD_THRD_EXIT)
        struct xwos_tcb * ctcb;

        ctcb = xwos_scheduler_get_ctcb_lc();
        return !!(XWSDOBJ_DST_EXITING & ctcb->state);
#else /* XWUPCFG_SD_THRD_EXIT */
        return false;
#endif /* !XWUPCFG_SD_THRD_EXIT */
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
 * ```
 */
__xwos_api
bool xwos_cthrd_frz_shld_stop(bool * frozen)
{
#if (1 == SOUPRULE_SD_THRD_FREEZE)
        bool frz;
        xwer_t rc;

        frz = false;
        if (xwos_cthrd_shld_frz()) {
                rc = xwos_cthrd_freeze();
                if (OK == rc) {
                        frz = true;
                }
                /* else {} */
        }/* else {} */
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }/* else {} */
#else /* (1 == SOUPRULE_SD_THRD_FREEZE) */
        if (!(is_err_or_null(frozen))) {
                *frozen = false;
        }/* else {} */
#endif /* !(1 == SOUPRULE_SD_THRD_FREEZE) */
        return xwos_cthrd_shld_stop();
}
