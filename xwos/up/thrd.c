/**
 * @file
 * @brief 玄武OS UP内核：线程
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/rtrq.h>
#if (1 == XWUPRULE_SKD_WQ_RT)
  #include <xwos/up/rtwq.h>
#endif /* XWUPRULE_SKD_WQ_RT */
#if (1 == XWUPRULE_SKD_WQ_PL)
  #include <xwos/up/plwq.h>
#endif /* XWUPRULE_SKD_WQ_PL */
#include <xwos/up/tt.h>
#include <xwos/up/thrd.h>
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
  #include <xwos/up/lock/mtx.h>
  #include <xwos/up/mtxtree.h>
#endif /* XWUPCFG_LOCK_MTX */
#if defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)
  #include <xwos/up/lock/fakemtx.h>
#endif /* XWUPCFG_LOCK_FAKEMTX */
#if defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
  #include <xwos/up/sync/plsem.h>
#endif /* XWUPCFG_SYNC_PLSEM */
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
  #include <xwos/up/sync/rtsem.h>
#endif /* XWUPCFG_SYNC_RTSEM */
#if defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND)
  #include <xwos/up/sync/cond.h>
#endif /* XWUPCFG_SYNC_COND */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwup_code
xwer_t bdl_thrd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);

extern __xwup_code
xwer_t bdl_thrd_stack_pool_free(xwstk_t * stk);

static __xwup_code
struct xwup_tcb * xwup_tcb_alloc(void);

static __xwup_code
void xwup_tcb_free(struct xwup_tcb * tcb);

static __xwup_code
xwstk_t * xwup_thrd_stack_alloc(xwsz_t stack_size);

static __xwup_code
xwer_t xwup_thrd_stack_free(xwstk_t * stk);

static __xwup_code
void xwup_thrd_activate(struct xwup_tcb * tcb,
                        const char * name,
                        xwup_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr);

static __xwup_code
void xwup_thrd_launch(struct xwup_tcb * tcb, xwup_thrd_f mainfunc, void * arg);

#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
static __xwup_code
xwer_t xwup_thrd_terminate_unlock_cb(struct xwup_tcb * tcb);
#endif /* XWUPCFG_SKD_THRD_EXIT */

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
static __xwup_code
struct xwup_tcb * xwup_tcb_alloc(void)
{
        union {
                struct xwup_tcb * tcb;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_tcb), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.tcb = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.tcb;
}

/**
 * @brief 释放线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 */
static __xwup_code
void xwup_tcb_free(struct xwup_tcb * tcb)
{
        xwmm_kma_free(tcb);
}

/**
 * @brief 动态分配线程栈
 * @return 线程栈的首地址或指针类型的错误码或空指针
 */
static __xwup_code
xwstk_t * xwup_thrd_stack_alloc(xwsz_t stack_size)
{
#if defined(BRDCFG_XWSKD_THRD_STACK_POOL) && (1 == BRDCFG_XWSKD_THRD_STACK_POOL)
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
static __xwup_code
xwer_t xwup_thrd_stack_free(xwstk_t * stk)
{
#if defined(BRDCFG_XWSKD_THRD_STACK_POOL) && (1 == BRDCFG_XWSKD_THRD_STACK_POOL)
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
 * @param attr: (I) 线程属性，取值：@ref xwup_skdobj_attr_em中的一项
 */
static __xwup_code
void xwup_thrd_activate(struct xwup_tcb * tcb,
                        const char * name,
                        xwup_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        /* base info */
        tcb->state = XWUP_SKDOBJ_DST_UNKNOWN;
        tcb->attribute = attr;

#if (1 == XWUPRULE_SKD_THRD_FREEZE)
        /* frozen state info */
        xwlib_bclst_init_node(&tcb->frznode);
#endif /* (1 == XWUPRULE_SKD_THRD_FREEZE) */

        /* ready state info */
        xwlib_bclst_init_node(&tcb->rqnode);

        /* sleeping state info */
        xwup_ttn_init(&tcb->ttn, (xwptr_t)tcb, XWUP_TTN_TYPE_THRD);

        /* blocking state info */
        xwup_wqn_init(&tcb->wqn, tcb);

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
        /* mutex tree */
        xwup_mtxtree_init(&tcb->mtxtree);
#endif /* XWUPCFG_LOCK_MTX */

        /* priority */
        if (priority >= XWUP_SKD_PRIORITY_RT_NUM) {
                priority = XWUP_SKD_PRIORITY_RT_MAX;
        } else if (priority <= XWUP_SKD_PRIORITY_INVALID) {
                priority = XWUP_SKD_PRIORITY_RT_MIN;
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
#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
        xwup_cond_init(&tcb->completion);
#endif /* XWUPCFG_SKD_THRD_EXIT */

        /* init tcb node */
        xwlib_bclst_init_node(&tcb->tcbnode);

        /* add to ready queue */
        if (mainfunc) {
                xwup_thrd_launch(tcb, mainfunc, arg);
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_STANDBY);
        }
}

/**
 * @brief 使得线程控制块对象无效
 * @param tcb: (I) 线程控制块对象的指针
 */
static __xwup_code
void xwup_thrd_deactivate(struct xwup_tcb * tcb)
{
        XWOS_UNUSED(tcb);
}

/**
 * @brief 加载线程
 * @param tcb: (I) 线程控制块对象的指针
 * @param mainfunc: (I) 线程主函数
 * @param arg: (I) 线程主函数的参数
 */
static __xwup_code
void xwup_thrd_launch(struct xwup_tcb * tcb, xwup_thrd_f mainfunc, void * arg)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        /* add to ready queue */
        xwskd = xwup_skd_get_lc();
        tcb->stack.main = mainfunc;
        tcb->stack.arg = arg;
        xwospl_skd_init_stack(&tcb->stack, xwup_cthrd_exit, tcb->attribute);
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_STANDBY);
        xwup_thrd_rq_add_tail(tcb);
        xwskd->thrd_num++;
        xwlib_bclst_add_tail(&xwskd->tcblist, &tcb->tcbnode);
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWUP API：静态初始化线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈的首地址指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，取值：@ref xwup_skdobj_attr_em中的一项
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *tcb* ，不可重入
 * @note
 * - 静态初始化线程需预先定义线程控制块对象和线程栈数组，通常定义为全局变量；
 * - 栈数组的首地址与大小，必须要满足CPU的ABI规则，例如ARM，就需要8字节对齐，
 *   因此在定义栈数组时需要__xwcc_aligned(8)来修饰，且大小是8的倍数
 */
__xwup_api
xwer_t xwup_thrd_init(struct xwup_tcb * tcb,
                      const char * name,
                      xwup_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr)
{
        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stack), "nullptr", -EFAULT);

        xwup_thrd_activate(tcb, name,
                           mainfunc, arg,
                           stack, stack_size,
                           priority, attr);
        return XWOK;
}

/**
 * @brief XWUP API：销毁线程控制块对象
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *tcb* ，不可重入
 */
__xwup_api
xwer_t xwup_thrd_destroy(struct xwup_tcb * tcb)
{
        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        xwup_thrd_deactivate(tcb);
        return XWOK;
}

/**
 * @brief XWUP API：动态创建线程
 * @param tcbpbuf: (O) 指向缓冲区的指针，通过此缓冲区返回线程控制块对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，@ref xwup_skdobj_attr_em
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_thrd_create(struct xwup_tcb ** tcbpbuf,
                        const char * name,
                        xwup_thrd_f mainfunc, void * arg,
                        xwsz_t stack_size, xwpr_t priority,
                        xwsq_t attr)
{
        struct xwup_tcb * tcb;
        xwstk_t * stk;
        xwer_t rc;

        XWOS_VALIDATE((NULL != tcbpbuf), "nullptr", -EFAULT);

        stk = xwup_thrd_stack_alloc(stack_size);
        if (__xwcc_unlikely(is_err(stk))) {
                rc = ptr_err(stk);
                goto err_stack_alloc;
        }

        tcb = xwup_tcb_alloc();
        if (is_err(tcb)) {
                rc = ptr_err(tcb);
                goto err_tcb_alloc;
        }

        xwup_thrd_activate(tcb, name,
                           mainfunc, arg,
                           stk, stack_size,
                           priority, attr);
        *tcbpbuf = tcb;
        return XWOK;

err_tcb_alloc:
        xwup_thrd_stack_free(stk);
err_stack_alloc:
        *tcbpbuf = NULL;
        return rc;
}

/**
 * @brief XWUP API：删除动态创建线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *tcb* ，不可重入
 */
__xwup_api
xwer_t xwup_thrd_delete(struct xwup_tcb * tcb)
{
        xwstk_t * base;

        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        xwup_thrd_deactivate(tcb);
        base = ((struct xwup_tcb *)tcb)->stack.base;
        xwup_thrd_stack_free(base);
        xwup_tcb_free(tcb);
        return XWOK;
}

/**
 * @brief XWUP API：当前线程退出
 * @param rc: (I) 线程退出时的返回值
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 调用的线程立即退出并抛出返回值，类似于POSIX线程库中的pthread_exit()函数。
 */
__xwup_api
void xwup_cthrd_exit(xwer_t rc)
{
#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
        struct xwup_tcb * ctcb;

        ctcb = xwup_skd_get_ctcb_lc();
        xwospl_thrd_exit_lc(ctcb, rc);
#else /* XWUPCFG_SKD_THRD_EXIT */
        XWOS_UNUSED(rc);
        xwup_cthrd_wait_exit();
#endif /* !XWUPCFG_SKD_THRD_EXIT */
}

/**
 * @brief 执行退出线程
 * @param tcb: (I) 线程控制块对象的指针
 * @param rc: (I) 线程的返回值
 */
__xwup_code
xwer_t xwup_thrd_exit_lic(struct xwup_tcb * tcb, xwer_t rc)
{
#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state,
                  XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_EXITING);
        xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_STANDBY);
        tcb->stack.arg = err_ptr(rc);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_cond_broadcast(&tcb->completion);
        xwospl_cpuirq_disable_lc();
        xwlib_bclst_del_init(&tcb->tcbnode);
        xwskd->thrd_num--;
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        if (xwskd->pm.frz_thrd_cnt == xwskd->thrd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_notify_allfrz_lic();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_req_swcx();
        }
#else /* XWUPCFG_SKD_PM */
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_req_swcx();
#endif /* !XWUPCFG_SKD_PM */
#else /* XWUPCFG_SKD_THRD_EXIT */
        XWOS_UNUSED(tcb);
        XWOS_UNUSED(rc);
#endif /* !XWUPCFG_SKD_THRD_EXIT */
        return XWOK;
}

#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
/**
 * @brief @ref xwup_thrd_terminate()中使用的回调锁的解锁函数
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
static __xwup_code
xwer_t xwup_thrd_terminate_unlock_cb(struct xwup_tcb * tcb)
{
        xwup_thrd_intr(tcb);
        return XWOK;
}

/**
 * @brief XWUP API：终止线程并等待它的返回值
 * @param tcb: (I) 线程控制块对象的指针
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回被终止线程的返回值，
 *                 可为NULL，表示不需要返回
 * @return 错误码
 * @retval XWOK: 没有错误
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
 *   玄武OS的线程在此函数调用之前就返回并不会引起错误，此函数的返回值是
 *   小于0的错误码，指针trc指向的缓冲区依然可以获取线程之前的返回值。
 */
__xwup_api
xwer_t xwup_thrd_terminate(struct xwup_tcb * tcb, xwer_t * trc)
{
        struct xwos_cblk lockcb;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != tcb), "nullptr", -EFAULT);

        lockcb.unlock = (xwer_t (*)(void *))xwup_thrd_terminate_unlock_cb;
        lockcb.lock = NULL;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_STANDBY & tcb->state) {
                rc = -EALREADY;
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)tcb->stack.arg;
                }
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_EXITING);
                rc = xwup_cond_wait(&tcb->completion,
                                    &lockcb, XWOS_LK_CALLBACK, tcb,
                                    &lkst);
                if (XWOK == rc) {
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)tcb->stack.arg;
                        }
                }
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

#else /* XWUPCFG_SKD_THRD_EXIT */

/**
 * @brief XWUP API：终止线程并等待它的返回值
 * @param tcb: (I) 线程控制块对象的指针
 * @param rc: (O) 指向缓冲区的指针，通过此缓冲区返回被终止线程的返回值
 * @return 错误码
 * @retval -ENOSYS: 系统配置为不允许线程退出
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_thrd_terminate(struct xwup_tcb * tcb, xwer_t * trc)
{
        XWOS_UNUSED(tcb);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}
#endif /* !XWUPCFG_SKD_THRD_EXIT */

/**
 * @brief XWUP API：判断线程是否可以退出，若不是，线程就阻塞直到它被终止
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数由子线程调用，等待父线程发出退出信号。
 * - 在父线程需要同步获取子线程运行结果的应用场景，子线程不可在父线程
 *   调用@ref xwup_thrd_terminate()前退出。但有时子线程运行得很快，
 *   即将退出，父线程此时还没来得及调用xwup_thrd_terminate()。子线程通过调用
 *   此函数阻塞自身并等待父线程调用@ref xwup_thrd_terminate()。
 * - 如果父线程不关心子线程的返回值，也不会调用@ref xwup_thrd_terminate()，
 *   子线程可以直接退出。
 */
__xwup_api
void xwup_cthrd_wait_exit(void)
{
        struct xwup_tcb * ctcb;
        xwreg_t cpuirq;

        ctcb = xwup_skd_get_ctcb_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
        while (!(XWUP_SKDOBJ_DST_EXITING & ctcb->state)) {
                xwbop_c0m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_BLOCKING);
                ctcb->wqn.wq = NULL;
                ctcb->wqn.type = XWUP_WQTYPE_NULL;
                ctcb->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
                ctcb->wqn.cb = xwup_thrd_wqn_callback;
                xwospl_cpuirq_enable_lc();
                xwup_skd_req_swcx();
                if (xwup_cthrd_shld_frz()) {
                        xwup_cthrd_freeze();
                }
                xwospl_cpuirq_disable_lc();
        }
#else /* XWUPCFG_SKD_THRD_EXIT */
        while (1) {
                xwbop_c0m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_BLOCKING);
                ctcb->wqn.wq = NULL;
                ctcb->wqn.type = XWUP_WQTYPE_NULL;
                ctcb->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
                ctcb->wqn.cb = xwup_thrd_wqn_callback;
                xwospl_cpuirq_enable_lc();
                xwup_skd_req_swcx();
                if (xwup_cthrd_shld_frz()) {
                        xwup_cthrd_freeze();
                }
                xwospl_cpuirq_disable_lc();
        }
#endif /* !XWUPCFG_SKD_THRD_EXIT */
        xwospl_cpuirq_restore_lc(cpuirq);
}

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
/**
 * @brief 改变线程的动态优先级一次
 * @param tcb: (I) 线程控制块对象的指针
 * @param dprio: (I) 动态优先级
 * @param pmtx: (O) 指向缓冲区的指针，通过此缓冲区返回互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESTALE: 状态已经被其他CPU改变
 * @note
 * - 此函数将线程(tcb)的优先级改成(dprio)，并返回接下来需要修改优先级的互斥锁的指针。
 */
__xwup_code
void xwup_thrd_chprio_once(struct xwup_tcb * tcb, xwpr_t dprio,
                           struct xwup_mtx ** pmtx)
{
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_FROZEN) & tcb->state) {
                tcb->prio.d = dprio;
        } else if (XWUP_SKDOBJ_DST_READY & tcb->state) {
                if (tcb->prio.d != dprio) {
                        xwup_thrd_rq_remove(tcb);
                        tcb->prio.d = dprio;
                        xwup_thrd_rq_add_tail(tcb);
                }
        } else if (XWUP_SKDOBJ_DST_BLOCKING & tcb->state) {
                if (tcb->prio.d == dprio) {
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
                } else if (XWUP_WQTYPE_MTX == tcb->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_baseof(tcb->wqn.wq, struct xwup_mtx, rtwq);
                        xwup_rtwq_remove(&mtx->rtwq, &tcb->wqn);
                        tcb->prio.d = dprio;
                        xwup_rtwq_add(&mtx->rtwq, &tcb->wqn, dprio);
                        *pmtx = mtx;
#endif /* XWUPCFG_LOCK_MTX */
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
                } else if (XWUP_WQTYPE_RTSEM == tcb->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_baseof(tcb->wqn.wq, struct xwup_rtsem, rtwq);
                        xwup_rtwq_remove(&sem->rtwq, &tcb->wqn);
                        tcb->prio.d = dprio;
                        xwup_rtwq_add(&sem->rtwq, &tcb->wqn, dprio);
                } else {
#endif /* XWUPCFG_SYNC_RTSEM */
                        tcb->prio.d = dprio;
                }
        } else {
                XWOS_BUG_ON(XWUP_SKDOBJ_DST_STANDBY & tcb->state);
                tcb->prio.d = dprio;
        }
}

/**
 * @brief 重新设定线程的动态优先级
 * @param tcb: (I) 线程控制块对象的指针
 */
__xwup_code
void xwup_thrd_chprio(struct xwup_tcb * tcb)
{
        struct xwup_mtx * unused;
        struct xwup_mtxtree * mt;
        xwpr_t dprio;

        mt = &tcb->mtxtree;
        dprio = tcb->prio.s > mt->maxprio ? tcb->prio.s : mt->maxprio;
        xwup_thrd_chprio_once(tcb, dprio, &unused);
}
#endif /* XWUPCFG_LOCK_MTX */

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param tcb: (I) 线程控制块对象的指针
 * @param prio: (I) 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 */
__xwup_code
xwer_t xwup_thrd_rq_add_head(struct xwup_tcb * tcb)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_READY) & tcb->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_READY);
                xwup_rtrq_add_head(xwrtrq, tcb);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param tcb: (I) 线程控制块对象的指针
 * @param prio: (I) 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 */
__xwup_code
xwer_t xwup_thrd_rq_add_tail(struct xwup_tcb * tcb)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_READY) & tcb->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_READY);
                xwup_rtrq_add_tail(xwrtrq, tcb);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程从就绪队列中删除
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 没有这个线程
 */
__xwup_code
void xwup_thrd_rq_remove(struct xwup_tcb * tcb)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        xwup_rtrq_remove(xwrtrq, tcb);
        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_READY);
}

/**
 * @brief XWUP API：中断线程的睡眠或阻塞状态
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EPERM: 线程不可被中断
 * @retval -EINVAL: 无效的线程态
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_thrd_intr(struct xwup_tcb * tcb)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_UNINTERRUPTED & tcb->state) {
                rc = -EPERM;
                xwospl_cpuirq_restore_lc(cpuirq);
        } else if (XWUP_SKDOBJ_DST_BLOCKING & tcb->state) {
                xwup_wqn_f cb;

                if (XWUP_WQTYPE_NULL == tcb->wqn.type) {
                        tcb->wqn.wq = NULL;
                        tcb->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        tcb->wqn.reason = XWUP_WQN_REASON_INTR;
                        cb = tcb->wqn.cb;
                        tcb->wqn.cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(tcb);
                        rc = XWOK;
#if defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
                } else if (XWUP_WQTYPE_PLSEM == tcb->wqn.type) {
                        struct xwup_plsem * sem;

                        sem = xwcc_baseof(tcb->wqn.wq,
                                          struct xwup_plsem,
                                          plwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_plsem_intr(sem, &tcb->wqn);
#endif /* XWUPCFG_SYNC_PLSEM */
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
                } else if (XWUP_WQTYPE_RTSEM == tcb->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_baseof(tcb->wqn.wq,
                                          struct xwup_rtsem,
                                          rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_rtsem_intr(sem, &tcb->wqn);
#endif /* XWUPCFG_SYNC_RTSEM */
#if defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND)
                } else if (XWUP_WQTYPE_COND == tcb->wqn.type) {
                        struct xwup_cond * sem;

                        sem = xwcc_baseof(tcb->wqn.wq,
                                          struct xwup_cond,
                                          wq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_cond_intr(sem, &tcb->wqn);
#endif /* XWUPCFG_SYNC_COND */
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
                } else if (XWUP_WQTYPE_MTX == tcb->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_baseof(tcb->wqn.wq,
                                          struct xwup_mtx,
                                          rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_mtx_intr(mtx, tcb);
#endif /* XWUPCFG_LOCK_MTX */
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        XWOS_BUG();
                }
        } else if (XWUP_SKDOBJ_DST_SLEEPING & tcb->state) {
                struct xwup_skd * xwskd;
                struct xwup_tt * xwtt;

                xwskd = xwup_skd_get_lc();
                xwtt = &xwskd->tt;
                xwup_sqlk_wr_lock(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &tcb->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_SLEEPING);
                        xwup_sqlk_wr_unlock(&xwtt->lock);
                        xwup_thrd_wakeup(tcb);
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
 * @param entry: (I) 线程控制块对象的地址
 */
__xwup_code
void xwup_thrd_ttn_callback(void * entry)
{
        struct xwup_tcb * tcb;
        xwreg_t cpuirq;

        tcb = entry;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_SLEEPING);
        xwup_thrd_wakeup(tcb);
        xwospl_cpuirq_restore_lc(cpuirq);
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
__xwup_code
xwer_t xwup_thrd_tt_add_locked(struct xwup_tcb * tcb, struct xwup_tt * xwtt,
                               xwtm_t expected, xwreg_t cpuirq)
{
        xwer_t rc;

        /* add to time tree */
        tcb->ttn.wkup_xwtm = expected;
        tcb->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
        tcb->ttn.xwtt = xwtt;
        tcb->ttn.cb = xwup_thrd_ttn_callback;
        rc = xwup_tt_add_locked(xwtt, &tcb->ttn, cpuirq);
        return rc;
}

/**
 * @brief 线程的等待队列节点回调函数
 * @param entry: (I) 线程控制块对象的地址
 */
__xwup_code
void xwup_thrd_wqn_callback(void * entry)
{
        struct xwup_tcb * tcb;
        xwreg_t cpuirq;

        tcb = entry;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state,
                  XWUP_SKDOBJ_DST_BLOCKING | XWUP_SKDOBJ_DST_UNINTERRUPTED);
        xwup_thrd_wakeup(tcb);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_chkpmpt();
}

#if (1 == XWUPRULE_SKD_WQ_RT)
/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param tcb: (I) 线程控制块对象的指针
 * @param xwrtwq: (I) 实时（红黑树）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwup_code
void xwup_thrd_eq_rtwq(struct xwup_tcb * tcb, struct xwup_rtwq * xwrtwq,
                       xwu16_t type)
{
        tcb->wqn.wq = xwrtwq;
        tcb->wqn.type = type;
        tcb->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
        tcb->wqn.cb = xwup_thrd_wqn_callback;
        xwup_rtwq_add(xwrtwq, &tcb->wqn, tcb->prio.d);
}
#endif /* (1 == XWUPRULE_SKD_WQ_RT) */

#if (1 == XWUPRULE_SKD_WQ_PL)
/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param tcb: (I) 线程控制块对象的指针
 * @param xwplwq: (I) 管道（双循环链表）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwup_code
void xwup_thrd_eq_plwq(struct xwup_tcb * tcb, struct xwup_plwq * xwplwq,
                       xwu16_t type)
{
        tcb->wqn.wq = xwplwq;
        tcb->wqn.type = type;
        tcb->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
        tcb->wqn.cb = xwup_thrd_wqn_callback;
        xwup_plwq_add_tail(xwplwq, &tcb->wqn);
}
#endif /* (1 == XWUPRULE_SKD_WQ_PL) */

/**
 * @brief XWUP API：让出当前线程的CPU，调度器重新选择同优先级的其他线程运行
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数只能将CPU让给优先级与当前线程相同且已经就绪的其他线程。
 *   如果当前线程已经是系统中唯一的最高优先级线程，此函数无效。
 */
__xwup_api
void xwup_cthrd_yield(void)
{
        struct xwup_tcb * ctcb;
        xwreg_t cpuirq;

        ctcb = xwup_skd_get_ctcb_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_RUNNING);
        xwup_thrd_rq_add_tail(ctcb);
        xwospl_cpuirq_enable_lc();
        xwup_skd_req_swcx();
        xwospl_cpuirq_restore_lc(cpuirq);
}

#if (1 == XWUPRULE_SKD_THRD_DO_UNLOCK)
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
__xwup_code
xwer_t xwup_thrd_do_unlock(void * lock, xwsq_t lktype, void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
        case XWOS_LK_MTX_UNINTR:
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
    (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX))
                rc = xwup_mtx_unlock(ulk.xwup.mtx);
#endif /* XWUPCFG_LOCK_MTX || XWUPCFG_LOCK_FAKEMTX */
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
                if (ulk.cb->unlock) {
                        rc = ulk.cb->unlock(lkdata);
                }
                break;
        default:
                break;
        }
        return rc;
}
#endif /* (1 == XWUPRULE_SKD_THRD_DO_UNLOCK) */

#if (1 == XWUPRULE_SKD_THRD_DO_LOCK)
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
__xwup_code
xwer_t xwup_thrd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm,
                         void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
    (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX))
                if (xwtm) {
                        rc = xwup_mtx_timedlock(ulk.xwup.mtx, xwtm);
                } else {
                        rc = xwup_mtx_lock(ulk.xwup.mtx);
                }
#endif /* XWUPCFG_LOCK_MTX || XWUPCFG_LOCK_FAKEMTX */
                break;
        case XWOS_LK_MTX_UNINTR:
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
    (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX))
                rc = xwup_mtx_lock_unintr(ulk.xwup.mtx);
#endif /* XWUPCFG_LOCK_MTX || XWUPCFG_LOCK_FAKEMTX */
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
#endif /* (1 == XWUPRULE_SKD_THRD_DO_LOCK) */

/**
 * @brief XWUP API：线程睡眠一段时间
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 睡眠过程被中断
 * @retval -ETIMEDOUT: 时间戳为负数
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * - 超时后将以返回值OK返回，并且 *xwtm* 指向缓冲区返回0。
 */
__xwup_api
xwer_t xwup_cthrd_sleep(xwtm_t * xwtm)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_tcb * ctcb;
        xwtm_t expected, currtick;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);

        if (__xwcc_unlikely(xwtm_cmp(*xwtm, 0) <= 0)) {
                rc = -ETIMEDOUT;
        } else {
                ctcb = xwup_skd_get_ctcb_lc();
                xwskd = xwup_skd_get_lc();
                xwtt = &xwskd->tt;
                hwt = &xwtt->hwt;
                currtick = xwup_syshwt_get_timetick(hwt);
                expected = xwtm_add_safely(currtick, *xwtm);
                xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                rc = xwup_skd_wakelock_lock();
                if (__xwcc_unlikely(rc < 0)) {
                        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        /* The skd is in hibernation state. Current thread need to
                           be frozen. So the blocking state needs to be interrupted. */
                        rc = -EINTR;
                } else
#endif /* XWUPCFG_SKD_PM */
                {
                        XWOS_BUG_ON((XWUP_SKDOBJ_DST_SLEEPING | XWUP_SKDOBJ_DST_READY |
                                     XWUP_SKDOBJ_DST_STANDBY | XWUP_SKDOBJ_DST_FROZEN)
                                    & ctcb->state);
                        /* set sleeping state */
                        xwbop_c0m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_RUNNING);
                        xwbop_s1m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_SLEEPING);
                        /* add to time tree */
                        xwup_thrd_tt_add_locked(ctcb, xwtt, expected, cpuirq);
                        /* enable local CPU IRQ to enable schedule */
                        xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                        xwup_skd_wakelock_unlock();
#endif /* XWUPCFG_SKD_PM */
                        xwup_skd_req_swcx();
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_TTN_WKUPRS_TIMEDOUT == ctcb->ttn.wkuprs) {
                                rc = XWOK;
                        } else if (XWUP_TTN_WKUPRS_INTR == ctcb->ttn.wkuprs) {
                                rc = -EINTR;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
                currtick = xwup_syshwt_get_timetick(hwt);
                *xwtm = xwtm_sub(expected, currtick);
        }
        return rc;
}

/**
 * @brief XWUP API：线程睡眠到指定的系统时间
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
__xwup_api
xwer_t xwup_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_tcb * ctcb;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != origin), "nullptr", -EFAULT);

        ctcb = xwup_skd_get_ctcb_lc();
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        expected = xwtm_add_safely(*origin, inc);
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        rc = xwup_skd_wakelock_lock();
        if (__xwcc_unlikely(rc < 0)) {
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，
                   阻塞/睡眠函数将返回-EINTR。*/
                rc = -EINTR;
        } else
#endif /* XWUPCFG_SKD_PM */
        {
                XWOS_BUG_ON((XWUP_SKDOBJ_DST_SLEEPING | XWUP_SKDOBJ_DST_READY |
                             XWUP_SKDOBJ_DST_STANDBY | XWUP_SKDOBJ_DST_FROZEN)
                            & ctcb->state);
                /* set the sleeping state */
                xwbop_c0m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &ctcb->state, XWUP_SKDOBJ_DST_SLEEPING);
                /* add to time tree */
                xwup_thrd_tt_add_locked(ctcb, xwtt, expected, cpuirq);
                /* enable local CPU IRQ to enable schedule */
                xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                xwup_skd_wakelock_unlock();
#endif /* XWUPCFG_SKD_PM */
                xwup_skd_req_swcx();
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWUP_TTN_WKUPRS_TIMEDOUT == ctcb->ttn.wkuprs) {
                        rc = XWOK;
                } else if (XWUP_TTN_WKUPRS_INTR == ctcb->ttn.wkuprs) {
                        rc = -EINTR;
                } else {
                        XWOS_BUG();
                        rc = -EBUG;
                }
                *origin = xwup_syshwt_get_timetick(hwt);
        }
        return rc;
}

/**
 * @brief XWUP API：冻结当前线程
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_cthrd_freeze(void)
{
#if (1 == XWUPRULE_SKD_THRD_FREEZE)
        struct xwup_tcb * ctcb;
        xwer_t rc;

        xwup_skd_dspmpt_lc();
        ctcb = xwup_skd_get_ctcb_lc();
        rc = xwospl_thrd_freeze_lc(ctcb);
        return rc;
#else /* (1 == XWUPRULE_SKD_THRD_FREEZE) */
        return -ENOSYS;
#endif /* !(1 == XWUPRULE_SKD_THRD_FREEZE) */
}

/**
 * @brief 冻结线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在中断上下文中调用并且这个中断只能由@ref xwup_cthrd_freeze()触发。
 */
__xwup_code
xwer_t xwup_thrd_freeze_lic(struct xwup_tcb * tcb)
{
#if (1 == XWUPRULE_SKD_THRD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        XWOS_BUG_ON(XWUP_SKDOBJ_DST_RUNNING != (tcb->state & (XWUP_SKDOBJ_DST_MASK)));
        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_FROZEN);
        xwskd->pm.frz_thrd_cnt++;
        xwlib_bclst_add_tail(&xwskd->pm.frzlist, &tcb->frznode);
        xwup_skd_enpmpt_lc();
        if (xwskd->pm.frz_thrd_cnt == xwskd->thrd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_notify_allfrz_lic();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_req_swcx();
        }
        return XWOK;
#else /* (1 == XWUPRULE_SKD_THRD_FREEZE) */
        XWOS_UNUSED(tcb);
        return -ENOSYS;
#endif /* (1 != XWUPRULE_SKD_THRD_FREEZE) */
}

/**
 * @brief 解冻线程
 * @param tcb: (I) 线程控制块对象的指针
 * @return 错误码
 */
__xwup_code
xwer_t xwup_thrd_thaw_lic(struct xwup_tcb * tcb)
{
#if (1 == XWUPRULE_SKD_THRD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        XWOS_BUG_ON(!(XWUP_SKDOBJ_DST_FROZEN & tcb->state));
        xwlib_bclst_del_init(&tcb->frznode);
        xwskd->pm.frz_thrd_cnt--;
        xwbop_c0m(xwsq_t, &tcb->state, XWUP_SKDOBJ_DST_FROZEN);
        rc = xwup_thrd_rq_add_tail(tcb);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
#else /* (1 == XWUPRULE_SKD_THRD_FREEZE) */
        XWOS_UNUSED(tcb);
        return -ENOSYS;
#endif /* !(1 == XWUPRULE_SKD_THRD_FREEZE) */
}

/**
 * @brief XWUP API：判断当前线程是否可以冻结
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
bool xwup_cthrd_shld_frz(void)
{
#if (1 == XWUPRULE_SKD_THRD_FREEZE)
        return !!(xwup_skd_get_pm_stage() < XWUP_PM_STAGE_RUNNING);
#else /* (1 == XWUPRULE_SKD_THRD_FREEZE) */
        return false;
#endif /* !(1 == XWUPRULE_SKD_THRD_FREEZE) */
}

/**
 * @brief XWUP API：判断当前线程是否可以退出
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数类似于@ref xwup_cthrd_wait_exit()，但不会阻塞，只会立即返回
 *   true或false，通常用在线程主循环的循环条件中。例如：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwup_cthrd_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 */
__xwup_api
bool xwup_cthrd_shld_stop(void)
{
#if defined(XWUPCFG_SKD_THRD_EXIT) && (1 == XWUPCFG_SKD_THRD_EXIT)
        struct xwup_tcb * ctcb;

        ctcb = xwup_skd_get_ctcb_lc();
        return !!(XWUP_SKDOBJ_DST_EXITING & ctcb->state);
#else /* XWUPCFG_SKD_THRD_EXIT */
        return false;
#endif /* !XWUPCFG_SKD_THRD_EXIT */
}

/**
 * @brief XWUP API：判断当前线程是否可以冻结，如果是就冻结线程，
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
 * - 此函数在@ref xwup_cthrd_shld_stop()的基础上增加了对冻结条件是否满足
 *   的判断，如果可以冻结，就在函数中将线程冻结。
 * - 通常线程冻结用于处理系统的低功耗事件或将线程在不同的CPU中进行迁移。
 * - 需要考虑线程的冻结，通常将线程的主循环写成：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwup_cthrd_frz_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 * ```
 */
__xwup_api
bool xwup_cthrd_frz_shld_stop(bool * frozen)
{
#if (1 == XWUPRULE_SKD_THRD_FREEZE)
        bool frz;
        xwer_t rc;

        frz = false;
        if (xwup_cthrd_shld_frz()) {
                rc = xwup_cthrd_freeze();
                if (XWOK == rc) {
                        frz = true;
                }
                /* else {} */
        }/* else {} */
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }/* else {} */
#else /* (1 == XWUPRULE_SKD_THRD_FREEZE) */
        if (!(is_err_or_null(frozen))) {
                *frozen = false;
        }/* else {} */
#endif /* !(1 == XWUPRULE_SKD_THRD_FREEZE) */
        return xwup_cthrd_shld_stop();
}

#if defined(XWUPCFG_SKD_TCB_LOCAL_DATA_NUM) && (XWUPCFG_SKD_TCB_LOCAL_DATA_NUM > 0U)
/**
 * @brief XWUP API：设置线程的本地数据指针
 * @param tcb: (I) 线程控制块的指针
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
__xwup_api
xwer_t xwup_thrd_set_data(struct xwup_tcb * tcb, xwsq_t pos, void * data)
{
        xwer_t rc;

        XWOS_VALIDATE((tcb), "nullptr", -EFAULT);

        if (pos < XWUPCFG_SKD_TCB_LOCAL_DATA_NUM) {
                tcb->data[pos] = data;
                rc = XWOK;
        } else {
                rc = -ECHRNG;
        }
        return rc;
}

/**
 * @brief XWUP API：获取线程的本地数据指针
 * @param tcb: (I) 线程控制块的指针
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
__xwup_api
xwer_t xwup_thrd_get_data(struct xwup_tcb * tcb, xwsq_t pos, void ** databuf)
{
        xwer_t rc;

        XWOS_VALIDATE((tcb), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        if (pos < XWUPCFG_SKD_TCB_LOCAL_DATA_NUM) {
                *databuf = tcb->data[pos];
                rc = XWOK;
        } else {
                *databuf = NULL;
                rc = -ECHRNG;
        }
        return rc;
}

/**
 * @brief XWUP API：设置当前线程的本地数据指针
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
__xwup_api
xwer_t xwup_cthrd_set_data(xwsq_t pos, void * data)
{
        struct xwup_tcb * ctcb;

        ctcb = xwup_skd_get_ctcb_lc();
        return xwup_thrd_set_data(ctcb, pos, data);
}

/**
 * @brief XWUP API：获取当前线程的本地数据指针
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
__xwup_api
xwer_t xwup_cthrd_get_data(xwsq_t pos, void ** databuf)
{
        struct xwup_tcb * ctcb;

        ctcb = xwup_skd_get_ctcb_lc();
        return xwup_thrd_set_data(ctcb, pos, databuf);

}
#endif /* XWUPCFG_SKD_TCB_LOCAL_DATA_NUM */
