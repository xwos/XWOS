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
#include <xwos/up/thd.h>
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

extern __xwup_code
xwer_t bdl_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);

extern __xwup_code
xwer_t bdl_thd_stack_pool_free(xwstk_t * stk);

static __xwup_code
struct xwup_thd * xwup_thd_alloc(void);

static __xwup_code
void xwup_thd_free(struct xwup_thd * thd);

static __xwup_code
xwstk_t * xwup_thd_stack_alloc(xwsz_t stack_size);

static __xwup_code
xwer_t xwup_thd_stack_free(xwstk_t * stk);

static __xwup_code
void xwup_thd_activate(struct xwup_thd * thd,
                       const char * name,
                       xwup_thd_f mainfunc, void * arg,
                       xwstk_t * stack, xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr);

static __xwup_code
void xwup_thd_launch(struct xwup_thd * thd, xwup_thd_f mainfunc, void * arg);

#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
static __xwup_code
xwer_t xwup_thd_stop_unlock_cb(struct xwup_thd * thd);
#endif /* XWUPCFG_SKD_THD_EXIT */

/**
 * @brief 从线程对象缓存中申请一个对象
 * @return 线程对象的指针
 */
static __xwup_code
struct xwup_thd * xwup_thd_alloc(void)
{
        union {
                struct xwup_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_thd), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.thd;
}

/**
 * @brief 释放线程对象
 * @param thd: (I) 线程对象的指针
 */
static __xwup_code
void xwup_thd_free(struct xwup_thd * thd)
{
        xwmm_kma_free(thd);
}

/**
 * @brief 动态分配线程栈
 * @return 线程栈的首地址或指针类型的错误码或空指针
 */
static __xwup_code
xwstk_t * xwup_thd_stack_alloc(xwsz_t stack_size)
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
xwer_t xwup_thd_stack_free(xwstk_t * stk)
{
#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        return bdl_thd_stack_pool_free(stk);
#else
        return xwmm_kma_free(stk);
#endif
}

/**
 * @brief 激活线程对象
 * @param thd: (I) 线程对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈的首地址指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性，取值：@ref xwup_skdobj_attr_em中的一项
 */
static __xwup_code
void xwup_thd_activate(struct xwup_thd * thd,
                       const char * name,
                       xwup_thd_f mainfunc, void * arg,
                       xwstk_t * stack, xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        /* base info */
        thd->state = XWUP_SKDOBJ_DST_UNKNOWN;
        thd->attribute = attr;
        if (XWUP_SKDATTR_DETACHED & attr) {
                thd->state |= XWUP_SKDOBJ_DST_DETACHED;
        }

#if (1 == XWUPRULE_SKD_THD_FREEZE)
        /* frozen state info */
        xwlib_bclst_init_node(&thd->frznode);
#endif /* (1 == XWUPRULE_SKD_THD_FREEZE) */

        /* ready state info */
        xwlib_bclst_init_node(&thd->rqnode);

        /* sleeping state info */
        xwup_ttn_init(&thd->ttn, (xwptr_t)thd, XWUP_TTN_TYPE_THD);

        /* blocking state info */
        xwup_wqn_init(&thd->wqn, thd);

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
        /* mutex tree */
        xwup_mtxtree_init(&thd->mtxtree);
#endif /* XWUPCFG_LOCK_MTX */

        /* priority */
        if (priority >= XWUP_SKD_PRIORITY_RT_NUM) {
                priority = XWUP_SKD_PRIORITY_RT_MAX;
        } else if (priority <= XWUP_SKD_PRIORITY_INVALID) {
                priority = XWUP_SKD_PRIORITY_RT_MIN;
        }
        thd->prio.s = priority;
        thd->prio.d = priority;

        /* init stack */
        thd->stack.name = name;
        stack_size = (stack_size + XWMM_ALIGNMENT_MASK) & (~XWMM_ALIGNMENT_MASK);
        thd->stack.size = stack_size;
        thd->stack.base = stack;
#if defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK)
        thd->stack.sp = thd->stack.base + (stack_size / sizeof(xwstk_t));
#elif defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK)
        thd->stack.sp = thd->stack.base + (stack_size / sizeof(xwstk_t)) - 1;
#elif defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK)
        thd->stack.sp = thd->stack.base - 1;
#elif defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK)
        thd->stack.sp = thd->stack.base;
#else /* XWMMCFG_EA_STACK */
#error "Unknown stack type!"
#endif /* !XWMMCFG_EA_STACK */
        /* init completion */
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        xwup_cond_init(&thd->completion);
#endif /* XWUPCFG_SKD_THD_EXIT */

        /* init thd node */
        xwlib_bclst_init_node(&thd->thdnode);

        /* add to ready queue */
        if (mainfunc) {
                xwup_thd_launch(thd, mainfunc, arg);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_STANDBY);
        }
}

/**
 * @brief 使得线程对象无效
 * @param thd: (I) 线程对象的指针
 */
static __xwup_code
void xwup_thd_deactivate(struct xwup_thd * thd)
{
        XWOS_UNUSED(thd);
}

/**
 * @brief 加载线程
 * @param thd: (I) 线程对象的指针
 * @param mainfunc: (I) 线程主函数
 * @param arg: (I) 线程主函数的参数
 */
static __xwup_code
void xwup_thd_launch(struct xwup_thd * thd, xwup_thd_f mainfunc, void * arg)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        /* add to ready queue */
        xwskd = xwup_skd_get_lc();
        thd->stack.main = mainfunc;
        thd->stack.arg = arg;
        xwospl_skd_init_stack(&thd->stack, xwup_cthd_exit, thd->attribute);
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_STANDBY);
        xwup_thd_rq_add_tail(thd);
        xwskd->thd_num++;
        xwlib_bclst_add_tail(&xwskd->thdlist, &thd->thdnode);
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWUP API：静态初始化线程对象
 * @param thd: (I) 线程对象的指针
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
 * - 重入性：对于同一个 *thd* ，不可重入
 * @note
 * - 静态初始化线程需预先定义线程对象和线程栈数组，通常定义为全局变量；
 * - 栈数组的首地址与大小，必须要满足CPU的ABI规则，例如ARM，就需要8字节对齐，
 *   因此在定义栈数组时需要__xwcc_aligned(8)来修饰，且大小是8的倍数
 */
__xwup_api
xwer_t xwup_thd_init(struct xwup_thd * thd,
                     const char * name,
                     xwup_thd_f mainfunc, void * arg,
                     xwstk_t * stack, xwsz_t stack_size,
                     xwpr_t priority, xwsq_t attr)
{
        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stack), "nullptr", -EFAULT);

        xwup_thd_activate(thd, name,
                          mainfunc, arg,
                          stack, stack_size,
                          priority, attr);
        return XWOK;
}

/**
 * @brief XWUP API：销毁线程对象
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *thd* ，不可重入
 */
__xwup_api
xwer_t xwup_thd_destroy(struct xwup_thd * thd)
{
        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwup_thd_deactivate(thd);
        return XWOK;
}

/**
 * @brief XWUP API：动态创建线程
 * @param thdpbuf: (O) 指向缓冲区的指针，通过此缓冲区返回线程对象的指针
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
xwer_t xwup_thd_create(struct xwup_thd ** thdpbuf,
                       const char * name,
                       xwup_thd_f mainfunc, void * arg,
                       xwsz_t stack_size, xwpr_t priority,
                       xwsq_t attr)
{
        struct xwup_thd * thd;
        xwstk_t * stk;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thdpbuf), "nullptr", -EFAULT);

        stk = xwup_thd_stack_alloc(stack_size);
        if (__xwcc_unlikely(is_err(stk))) {
                rc = ptr_err(stk);
                goto err_stack_alloc;
        }

        thd = xwup_thd_alloc();
        if (is_err(thd)) {
                rc = ptr_err(thd);
                goto err_thd_alloc;
        }

        xwup_thd_activate(thd, name,
                          mainfunc, arg,
                          stk, stack_size,
                          priority, attr);
        *thdpbuf = thd;
        return XWOK;

err_thd_alloc:
        xwup_thd_stack_free(stk);
err_stack_alloc:
        *thdpbuf = NULL;
        return rc;
}

/**
 * @brief XWUP API：删除动态创建线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *thd* ，不可重入
 */
__xwup_api
xwer_t xwup_thd_delete(struct xwup_thd * thd)
{
        xwstk_t * base;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwup_thd_deactivate(thd);
        base = ((struct xwup_thd *)thd)->stack.base;
        xwup_thd_stack_free(base);
        xwup_thd_free(thd);
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
void xwup_cthd_exit(xwer_t rc)
{
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        xwospl_thd_exit_lc(cthd, rc);
#else /* XWUPCFG_SKD_THD_EXIT */
        xwtm_t time;

        XWOS_UNUSED(rc);
        time = XWTM_MAX;
        while (true) {
                xwup_cthd_sleep(&time);
        }
#endif /* !XWUPCFG_SKD_THD_EXIT */
}

/**
 * @brief 执行退出线程
 * @param thd: (I) 线程对象的指针
 * @param rc: (I) 线程的返回值
 */
__xwup_code
xwer_t xwup_thd_exit_lic(struct xwup_thd * thd, xwer_t rc)
{
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_EXITING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_STANDBY);
        thd->stack.arg = err_ptr(rc);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_cond_broadcast(&thd->completion);
        xwospl_cpuirq_disable_lc();
        xwlib_bclst_del_init(&thd->thdnode);
        xwskd->thd_num--;
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWUP_SKDATTR_DETACHED & thd->attribute) {
                        xwup_thd_delete(thd);
                }
                xwup_skd_notify_allfrz_lic();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWUP_SKDATTR_DETACHED & thd->attribute) {
                        xwup_thd_delete(thd);
                }
                xwup_skd_req_swcx();
        }
#else /* XWUPCFG_SKD_PM */
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_req_swcx();
#endif /* !XWUPCFG_SKD_PM */
#else /* XWUPCFG_SKD_THD_EXIT */
        XWOS_UNUSED(thd);
        XWOS_UNUSED(rc);
#endif /* !XWUPCFG_SKD_THD_EXIT */
        return XWOK;
}

#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
/**
 * @brief @ref xwup_thd_stop()中使用的回调锁的解锁函数
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
static __xwup_code
xwer_t xwup_thd_stop_unlock_cb(struct xwup_thd * thd)
{
        xwup_thd_intr(thd);
        return XWOK;
}

/**
 * @brief XWUP API：终止线程并等待它的返回值，最后回收线程资源
 * @param thd: (I) 线程对象的指针
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回被终止线程的返回值，
 *                 可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 线程在此之前已经退出
 * @note
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 线程不是Joinable的
 * @retval -EPERM: 线程不可stop自己
 * @note
 * - 此函数由父线程调用，用于终止Joinable的子线程，父线程会一直阻塞等待子线程退出，
 *   并获取子线程的返回值，最后释放子线程资源，此函数类似于POSIX线程库
 *   中的pthread_cancel() + pthread_join()组合；
 * - 子线程收到终止信号后，并不会立即退出，退出的时机由子线程自己控制；
 * - 不可对Detached态的线程使用此函数。
 */
__xwup_api
xwer_t xwup_thd_stop(struct xwup_thd * thd, xwer_t * trc)
{
        struct xwup_thd * cthd;
        struct xwos_cblk lockcb;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);
        cthd = xwup_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_stop_self;
        }

        lockcb.unlock = (xwer_t (*)(void *))xwup_thd_stop_unlock_cb;
        lockcb.lock = NULL;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_DETACHED & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINVAL;
        } else if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                rc = XWOK;
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_delete(thd);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_EXITING);
                rc = xwup_cond_wait(&thd->completion,
                                    &lockcb, XWOS_LK_CALLBACK, thd,
                                    &lkst);
                if (XWOK == rc) {
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                }
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWOK == rc) {
                        xwup_thd_delete(thd);
                }
        }

err_stop_self:
        return rc;
}

/**
 * @brief XWUP API：取消线程
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
 * - 此函数与xwup_thd_stop()不同，不会阻塞调用者，也不会回收子线程资源，因此
 *   可在中断中调用。
 */
__xwup_api
xwer_t xwup_thd_cancel(struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_EXITING);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_intr(thd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWUP API：等待线程结束并获取它的返回值，最后回收线程资源
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
__xwup_api
xwer_t xwup_thd_join(struct xwup_thd * thd, xwer_t * trc)
{
        struct xwup_thd * cthd;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        cthd = xwup_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_join_self;
        }
        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_DETACHED & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINVAL;
        } else if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                xwup_thd_delete(thd);
        } else {
                rc = xwup_cond_wait(&thd->completion,
                                    NULL, XWOS_LK_NONE, NULL,
                                    &lkst);
                if (XWOK == rc) {
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                } else {
                }
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWOK == rc) {
                        xwup_thd_delete(thd);
                }
        }

err_join_self:
        return rc;
}

/**
 * @brief XWUP API：分离线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数功能类似于pthread_detach()，处于分离态的线程退出后，系统能自动回收资源，
 *   不需要父线程join()或stop()它。
 */
__xwup_api
xwer_t xwup_thd_detach(struct xwup_thd * thd)
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_DETACHED);
        if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_delete(thd);
        } else {
                thd->attribute |= XWUP_SKDATTR_DETACHED;
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return XWOK;
}

#else /* XWUPCFG_SKD_THD_EXIT */

/**
 * @brief XWUP API：终止线程并等待它的返回值，最后回收线程资源
 * @param thd: (I) 线程对象的指针
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回被终止线程的返回值，
 *                 可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval -ENOSYS: 系统配置为不允许线程退出
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_thd_stop(struct xwup_thd * thd, xwer_t * trc)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}

/**
 * @brief XWUP API：取消线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval -ENOSYS: 系统配置为不允许线程退出
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_thd_cancel(struct xwup_thd * thd)
{
        XWOS_UNUSED(thd);
        return -ENOSYS;
}

/**
 * @brief XWUP API：等待线程结束并获取它的返回值，最后回收线程资源
 * @param thd: (I) 线程对象的指针
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回子线程的返回值，
 *                 可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval -ENOSYS: 系统配置为不允许线程退出
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_thd_join(struct xwup_thd * thd, xwer_t * trc)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}

/**
 * @brief XWUP API：分离线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @retval -ENOSYS: 系统配置为不允许线程退出
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
xwer_t xwup_thd_detach(struct xwup_thd * thd)
{
        XWOS_UNUSED(thd);
        return -ENOSYS;
}
#endif /* !XWUPCFG_SKD_THD_EXIT */

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
/**
 * @brief 改变线程的动态优先级一次
 * @param thd: (I) 线程对象的指针
 * @param dprio: (I) 动态优先级
 * @param pmtx: (O) 指向缓冲区的指针，通过此缓冲区返回互斥锁对象的指针
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
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_FROZEN) & thd->state) {
                thd->prio.d = dprio;
        } else if (XWUP_SKDOBJ_DST_READY & thd->state) {
                if (thd->prio.d != dprio) {
                        xwup_thd_rq_remove(thd);
                        thd->prio.d = dprio;
                        xwup_thd_rq_add_tail(thd);
                }
        } else if (XWUP_SKDOBJ_DST_BLOCKING & thd->state) {
                if (thd->prio.d == dprio) {
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
                } else if (XWUP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_baseof(thd->wqn.wq, struct xwup_mtx, rtwq);
                        xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
                        thd->prio.d = dprio;
                        xwup_rtwq_add(&mtx->rtwq, &thd->wqn, dprio);
                        *pmtx = mtx;
#endif /* XWUPCFG_LOCK_MTX */
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
                } else if (XWUP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_baseof(thd->wqn.wq, struct xwup_rtsem, rtwq);
                        xwup_rtwq_remove(&sem->rtwq, &thd->wqn);
                        thd->prio.d = dprio;
                        xwup_rtwq_add(&sem->rtwq, &thd->wqn, dprio);
                } else {
#endif /* XWUPCFG_SYNC_RTSEM */
                        thd->prio.d = dprio;
                }
        } else {
                XWOS_BUG_ON(XWUP_SKDOBJ_DST_STANDBY & thd->state);
                thd->prio.d = dprio;
        }
}

/**
 * @brief 重新设定线程的动态优先级
 * @param thd: (I) 线程对象的指针
 */
__xwup_code
void xwup_thd_chprio(struct xwup_thd * thd)
{
        struct xwup_mtx * unused;
        struct xwup_mtxtree * mt;
        xwpr_t dprio;

        mt = &thd->mtxtree;
        dprio = thd->prio.s > mt->maxprio ? thd->prio.s : mt->maxprio;
        xwup_thd_chprio_once(thd, dprio, &unused);
}
#endif /* XWUPCFG_LOCK_MTX */

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param thd: (I) 线程对象的指针
 * @param prio: (I) 动态优先级
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
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_READY) & thd->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_READY);
                xwup_rtrq_add_head(xwrtrq, thd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param thd: (I) 线程对象的指针
 * @param prio: (I) 动态优先级
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
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_READY) & thd->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_READY);
                xwup_rtrq_add_tail(xwrtrq, thd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程从就绪队列中删除
 * @param thd: (I) 线程对象的指针
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
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_READY);
}

/**
 * @brief XWUP API：中断线程的睡眠或阻塞状态
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
__xwup_api
xwer_t xwup_thd_intr(struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_UNINTERRUPTED & thd->state) {
                rc = -EPERM;
                xwospl_cpuirq_restore_lc(cpuirq);
        } else if (XWUP_SKDOBJ_DST_BLOCKING & thd->state) {
                xwup_wqn_f cb;

                if (XWUP_WQTYPE_NULL == thd->wqn.type) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        cb = thd->wqn.cb;
                        thd->wqn.cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(thd);
                        rc = XWOK;
#if defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
                } else if (XWUP_WQTYPE_PLSEM == thd->wqn.type) {
                        struct xwup_plsem * sem;

                        sem = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_plsem,
                                          plwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_plsem_intr(sem, &thd->wqn);
#endif /* XWUPCFG_SYNC_PLSEM */
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
                } else if (XWUP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_rtsem,
                                          rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_rtsem_intr(sem, &thd->wqn);
#endif /* XWUPCFG_SYNC_RTSEM */
#if defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND)
                } else if (XWUP_WQTYPE_COND == thd->wqn.type) {
                        struct xwup_cond * sem;

                        sem = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_cond,
                                          wq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_cond_intr(sem, &thd->wqn);
#endif /* XWUPCFG_SYNC_COND */
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
                } else if (XWUP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_mtx,
                                          rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_mtx_intr(mtx, thd);
#endif /* XWUPCFG_LOCK_MTX */
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -EBUG;
                        XWOS_BUG();
                }
        } else if (XWUP_SKDOBJ_DST_SLEEPING & thd->state) {
                struct xwup_skd * xwskd;
                struct xwup_tt * xwtt;

                xwskd = xwup_skd_get_lc();
                xwtt = &xwskd->tt;
                xwup_sqlk_wr_lock(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
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
 * @param entry: (I) 线程对象的地址
 */
__xwup_code
void xwup_thd_ttn_callback(void * entry)
{
        struct xwup_thd * thd;
        xwreg_t cpuirq;

        thd = entry;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
        xwup_thd_wakeup(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
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
__xwup_code
xwer_t xwup_thd_tt_add_locked(struct xwup_thd * thd, struct xwup_tt * xwtt,
                              xwtm_t expected, xwreg_t cpuirq)
{
        xwer_t rc;

        /* add to time tree */
        thd->ttn.wkup_xwtm = expected;
        thd->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
        thd->ttn.xwtt = xwtt;
        thd->ttn.cb = xwup_thd_ttn_callback;
        rc = xwup_tt_add_locked(xwtt, &thd->ttn, cpuirq);
        return rc;
}

/**
 * @brief 线程的等待队列节点回调函数
 * @param entry: (I) 线程对象的地址
 */
__xwup_code
void xwup_thd_wqn_callback(void * entry)
{
        struct xwup_thd * thd;
        xwreg_t cpuirq;

        thd = entry;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_DST_BLOCKING | XWUP_SKDOBJ_DST_UNINTERRUPTED);
        xwup_thd_wakeup(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_chkpmpt();
}

#if (1 == XWUPRULE_SKD_WQ_RT)
/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param thd: (I) 线程对象的指针
 * @param xwrtwq: (I) 实时（红黑树）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwup_code
void xwup_thd_eq_rtwq(struct xwup_thd * thd, struct xwup_rtwq * xwrtwq,
                      xwu16_t type)
{
        thd->wqn.wq = xwrtwq;
        thd->wqn.type = type;
        thd->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
        thd->wqn.cb = xwup_thd_wqn_callback;
        xwup_rtwq_add(xwrtwq, &thd->wqn, thd->prio.d);
}
#endif /* (1 == XWUPRULE_SKD_WQ_RT) */

#if (1 == XWUPRULE_SKD_WQ_PL)
/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param thd: (I) 线程对象的指针
 * @param xwplwq: (I) 管道（双循环链表）等待队列
 * @param type: (I) 等待队列的类型
 */
__xwup_code
void xwup_thd_eq_plwq(struct xwup_thd * thd, struct xwup_plwq * xwplwq,
                      xwu16_t type)
{
        thd->wqn.wq = xwplwq;
        thd->wqn.type = type;
        thd->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
        thd->wqn.cb = xwup_thd_wqn_callback;
        xwup_plwq_add_tail(xwplwq, &thd->wqn);
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
void xwup_cthd_yield(void)
{
        struct xwup_thd * cthd;
        xwreg_t cpuirq;

        cthd = xwup_skd_get_cthd_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwup_thd_rq_add_tail(cthd);
        xwospl_cpuirq_enable_lc();
        xwup_skd_req_swcx();
        xwospl_cpuirq_restore_lc(cpuirq);
}

#if (1 == XWUPRULE_SKD_THD_DO_UNLOCK)
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
xwer_t xwup_thd_do_unlock(void * lock, xwsq_t lktype, void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
        case XWOS_LK_MTX_UNINTR:
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) ||           \
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
        case XWOS_LK_NONE:
        default:
                break;
        }
        return rc;
}
#endif /* (1 == XWUPRULE_SKD_THD_DO_UNLOCK) */

#if (1 == XWUPRULE_SKD_THD_DO_LOCK)
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
xwer_t xwup_thd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm,
                        void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) ||           \
        (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX))
                if (xwtm) {
                        rc = xwup_mtx_timedlock(ulk.xwup.mtx, xwtm);
                } else {
                        rc = xwup_mtx_lock(ulk.xwup.mtx);
                }
#endif /* XWUPCFG_LOCK_MTX || XWUPCFG_LOCK_FAKEMTX */
                break;
        case XWOS_LK_MTX_UNINTR:
#if (defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) ||           \
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
        case XWOS_LK_NONE:
        default:
                XWOS_UNUSED(xwtm);
                break;
        }
        return rc;
}
#endif /* (1 == XWUPRULE_SKD_THD_DO_LOCK) */

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
xwer_t xwup_cthd_sleep(xwtm_t * xwtm)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_thd * cthd;
        xwtm_t expected, currtick;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);

        if (__xwcc_unlikely(xwtm_cmp(*xwtm, 0) <= 0)) {
                rc = -ETIMEDOUT;
        } else {
                cthd = xwup_skd_get_cthd_lc();
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
                                    & cthd->state);
                        /* set sleeping state */
                        xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_RUNNING);
                        xwbop_s1m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_SLEEPING);
                        /* add to time tree */
                        xwup_thd_tt_add_locked(cthd, xwtt, expected, cpuirq);
                        /* enable local CPU IRQ to enable schedule */
                        xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                        xwup_skd_wakelock_unlock();
#endif /* XWUPCFG_SKD_PM */
                        xwup_skd_req_swcx();
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_TTN_WKUPRS_TIMEDOUT == cthd->ttn.wkuprs) {
                                rc = XWOK;
                        } else if (XWUP_TTN_WKUPRS_INTR == cthd->ttn.wkuprs) {
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
 * @brief XWUP API：当前线程从一个时间起点睡眠到另一个时间点
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
xwer_t xwup_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_thd * cthd;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != origin), "nullptr", -EFAULT);

        cthd = xwup_skd_get_cthd_lc();
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
                            & cthd->state);
                /* set the sleeping state */
                xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_SLEEPING);
                /* add to time tree */
                xwup_thd_tt_add_locked(cthd, xwtt, expected, cpuirq);
                /* enable local CPU IRQ to enable schedule */
                xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                xwup_skd_wakelock_unlock();
#endif /* XWUPCFG_SKD_PM */
                xwup_skd_req_swcx();
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWUP_TTN_WKUPRS_TIMEDOUT == cthd->ttn.wkuprs) {
                        rc = XWOK;
                } else if (XWUP_TTN_WKUPRS_INTR == cthd->ttn.wkuprs) {
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
xwer_t xwup_cthd_freeze(void)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        struct xwup_thd * cthd;
        xwer_t rc;

        xwup_skd_dspmpt_lc();
        cthd = xwup_skd_get_cthd_lc();
        rc = xwospl_thd_freeze_lc(cthd);
        return rc;
#else /* (1 == XWUPRULE_SKD_THD_FREEZE) */
        return -ENOSYS;
#endif /* !(1 == XWUPRULE_SKD_THD_FREEZE) */
}

/**
 * @brief 冻结线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在中断上下文中调用并且这个中断只能由@ref xwup_cthd_freeze()触发。
 */
__xwup_code
xwer_t xwup_thd_freeze_lic(struct xwup_thd * thd)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        XWOS_BUG_ON(XWUP_SKDOBJ_DST_RUNNING != (thd->state & (XWUP_SKDOBJ_DST_MASK)));
        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_FROZEN);
        xwskd->pm.frz_thd_cnt++;
        xwlib_bclst_add_tail(&xwskd->pm.frzlist, &thd->frznode);
        xwup_skd_enpmpt_lc();
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_notify_allfrz_lic();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_req_swcx();
        }
        return XWOK;
#else /* (1 == XWUPRULE_SKD_THD_FREEZE) */
        XWOS_UNUSED(thd);
        return -ENOSYS;
#endif /* (1 != XWUPRULE_SKD_THD_FREEZE) */
}

/**
 * @brief 解冻线程
 * @param thd: (I) 线程对象的指针
 * @return 错误码
 */
__xwup_code
xwer_t xwup_thd_thaw_lic(struct xwup_thd * thd)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        XWOS_BUG_ON(!(XWUP_SKDOBJ_DST_FROZEN & thd->state));
        xwlib_bclst_del_init(&thd->frznode);
        xwskd->pm.frz_thd_cnt--;
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_FROZEN);
        rc = xwup_thd_rq_add_tail(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
#else /* (1 == XWUPRULE_SKD_THD_FREEZE) */
        XWOS_UNUSED(thd);
        return -ENOSYS;
#endif /* !(1 == XWUPRULE_SKD_THD_FREEZE) */
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
bool xwup_cthd_shld_frz(void)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        return !!(xwup_skd_get_pm_stage() < XWUP_PM_STAGE_RUNNING);
#else /* (1 == XWUPRULE_SKD_THD_FREEZE) */
        return false;
#endif /* !(1 == XWUPRULE_SKD_THD_FREEZE) */
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
 * - 此函数常用在线程主循环的循环条件中。例如：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwup_cthd_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 */
__xwup_api
bool xwup_cthd_shld_stop(void)
{
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return !!(XWUP_SKDOBJ_DST_EXITING & cthd->state);
#else /* XWUPCFG_SKD_THD_EXIT */
        return false;
#endif /* !XWUPCFG_SKD_THD_EXIT */
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
 * - 此函数在@ref xwup_cthd_shld_stop()的基础上增加了对冻结条件是否满足
 *   的判断，如果可以冻结，就在函数中将线程冻结。
 * - 通常线程冻结用于处理系统的低功耗事件或将线程在不同的CPU中进行迁移。
 * - 需要考虑线程的冻结，通常将线程的主循环写成：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwup_cthd_frz_shld_stop()) {
 *                 thread loop ...;
 *         }
 * }
 * ```
 * ```
 */
__xwup_api
bool xwup_cthd_frz_shld_stop(bool * frozen)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        bool frz;
        xwer_t rc;

        frz = false;
        if (xwup_cthd_shld_frz()) {
                rc = xwup_cthd_freeze();
                if (XWOK == rc) {
                        frz = true;
                }
                /* else {} */
        }/* else {} */
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }/* else {} */
#else /* (1 == XWUPRULE_SKD_THD_FREEZE) */
        if (!(is_err_or_null(frozen))) {
                *frozen = false;
        }/* else {} */
#endif /* !(1 == XWUPRULE_SKD_THD_FREEZE) */
        return xwup_cthd_shld_stop();
}

#if defined(XWUPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWUPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
/**
 * @brief XWUP API：设置线程的本地数据指针
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
__xwup_api
xwer_t xwup_thd_set_data(struct xwup_thd * thd, xwsq_t pos, void * data)
{
        xwer_t rc;

        XWOS_VALIDATE((thd), "nullptr", -EFAULT);

        if (pos < XWUPCFG_SKD_THD_LOCAL_DATA_NUM) {
                thd->data[pos] = data;
                rc = XWOK;
        } else {
                rc = -ECHRNG;
        }
        return rc;
}

/**
 * @brief XWUP API：获取线程的本地数据指针
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
__xwup_api
xwer_t xwup_thd_get_data(struct xwup_thd * thd, xwsq_t pos, void ** databuf)
{
        xwer_t rc;

        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        if (pos < XWUPCFG_SKD_THD_LOCAL_DATA_NUM) {
                *databuf = thd->data[pos];
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
xwer_t xwup_cthd_set_data(xwsq_t pos, void * data)
{
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return xwup_thd_set_data(cthd, pos, data);
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
xwer_t xwup_cthd_get_data(xwsq_t pos, void ** databuf)
{
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return xwup_thd_set_data(cthd, pos, databuf);

}
#endif /* XWUPCFG_SKD_THD_LOCAL_DATA_NUM */
