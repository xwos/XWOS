/**
 * @file
 * @brief 操作系统抽象层：调度
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_skd_h__
#define __xwos_osal_skd_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/skd.h>

/**
 * @defgroup xwos_skd 调度
 * @{
 */

/**
 * @brief XWOS API：线程对象
 */
struct xwos_thd {
        struct xwosdl_thd osthd;
};

/**
 * @brief XWOS API：线程函数指针类型
 */
typedef xwosdl_thd_f xwos_thd_f;

/**
 * @brief XWOS API：线程对象描述符
 */
typedef struct {
        struct xwos_thd * thd; /**< 线程对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_thd_d;

/**
 * @brief XWOS API：空的线程对象描述符
 */
#define XWOS_THD_NILD ((xwos_thd_d){NULL, 0,})

/**
 * @defgroup xwos_skdattr_em 线程属性
 * @{
 */
/**
 * @brief XWOS API：线程属性，超级权限
 */
#define XWOS_SKDATTR_PRIVILEGED XWOSDL_SKDATTR_PRIVILEGED

/**
 * @brief XWOS API：线程属性，DETACHED态
 */
#define XWOS_SKDATTR_DETACHED XWOSDL_SKDATTR_DETACHED

/**
 * @brief XWOS API：线程属性，JOINABLE态
 */
#define XWOS_SKDATTR_JOINABLE XWOSDL_SKDATTR_JOINABLE
/**
 * @} xwos_skdattr_em
 */

/**
 * @brief XWOS API：线程描述表
 */
struct xwos_thd_desc {
        const char * name; /**< 线程的名字 */
        xwpr_t prio; /**< 线程的优先级 */
        xwstk_t * stack; /**< 线程栈的首地址指针：
                              - 静态创建线程时必须指定；
                              - 动态创建也可指定，若不指定，栈内存也采取
                                动态内存申请。*/
        xwsz_t stack_size; /**< 线程栈的大小，以字节(byte)为单位，
                                注意与CPU的ABI接口规定的内存边界对齐 */
        xwos_thd_f func; /**< 线程函数的指针 */
        void * arg; /**< 线程函数的参数 */
        xwsq_t attr; /**< 与具体操作系统相关的一些数据 */
};

/**
 * @brief XWOS API：栈内存动态方式创建
 */
#define XWOS_THD_STACK_DYNAMIC (NULL)

/**
 * @brief XWOS API：最小实时优先级
 */
#define XWOS_SKD_PRIORITY_RT_MIN        XWOSDL_SKD_PRIORITY_RT_MIN

/**
 * @brief XWOS API：最大实时优先级
 */
#define XWOS_SKD_PRIORITY_RT_MAX        XWOSDL_SKD_PRIORITY_RT_MAX

/**
 * @brief XWOS API：无效优先级
 */
#define XWOS_SKD_PRIORITY_INVALID       XWOSDL_SKD_PRIORITY_INVALID

/**
 * @brief XWOS API：优先级在base基础上提高inc
 */
#define XWOS_SKD_PRIORITY_RAISE(base, inc)      XWOSDL_SKD_PRIORITY_RAISE(base, inc)

/**
 * @brief XWOS API：优先级在base基础上降低dec
 */
#define XWOS_SKD_PRIORITY_DROP(base, dec)       XWOSDL_SKD_PRIORITY_DROP(base, dec)

/**
 * @defgroup xwos_skd_context_em 上下文枚举
 * @{
 */
/**
 * @brief XWOS API：上下文：初始化与反初始化
 */
#define XWOS_SKD_CONTEXT_INIT_EXIT      XWOSDL_SKD_CONTEXT_INIT_EXIT

/**
 * @brief XWOS API：上下文：线程
 */
#define XWOS_SKD_CONTEXT_THD            XWOSDL_SKD_CONTEXT_THD

/**
 * @brief XWOS API：上下文：中断
 */
#define XWOS_SKD_CONTEXT_ISR            XWOSDL_SKD_CONTEXT_ISR

/**
 * @brief XWOS API：上下文：中断底半部
 */
#define XWOS_SKD_CONTEXT_BH             XWOSDL_SKD_CONTEXT_BH

/**
 * @brief XWOS API：上下文：空闲任务
 */
#define XWOS_SKD_CONTEXT_IDLE           XWOSDL_SKD_CONTEXT_IDLE
/**
 * @} xwos_skd_context_em
 */

/**
 * @brief XWOS API：线程本地数据指针的数量
 */
#define XWOS_THD_LOCAL_DATA_NUM         XWOSDL_THD_LOCAL_DATA_NUM

/**
 * @brief XWOS API：检查优先级是否有效
 * @param[in] prio: 优先级
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
bool xwos_skd_prio_tst_valid(xwpr_t prio)
{
        return xwosdl_skd_prio_tst_valid(prio);
}

/**
 * @brief XWOS API：启动当前CPU的调度器
 * @return 此函数不会返回
 * @note
 * - 同步/异步：同步
 * - 上下文：只可在系统复位后初始化流程中调用
 * - 重入性：只可调用一次
 */
static __xwos_inline_api
xwer_t xwos_skd_start_lc(void)
{
        return xwosdl_skd_start_lc();
}

/**
 * @brief XWOS API：获取当前CPU的ID
 * @return 当前CPU的ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwos_skd_id_lc(void)
{
        return xwosdl_skd_id_lc();
}

/**
 * @brief XWOS API：获取当前上下文
 * @param[out] ctxbuf: 指向缓冲区的指针，通过此缓冲区返回当前上下文，
 *  返回值@ref xwos_skd_context_em
 * @param[out] irqnbuf: 指向缓冲区的指针，通过此缓冲区返回中断号
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
void xwos_skd_get_context_lc(xwsq_t * ctxbuf, xwirq_t * irqnbuf)
{
        xwosdl_skd_get_context_lc(ctxbuf, irqnbuf);
}

/**
 * @brief XWOS API：获取当前CPU调度器的系统滴答时间
 * @return 系统时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwtm_t xwos_skd_get_timetick_lc(void)
{
        return xwosdl_skd_get_timetick_lc();
}

/**
 * @brief XWOS API：获取当前CPU调度器的系统滴答计数
 * @return 滴答计数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwu64_t xwos_skd_get_tickcount_lc(void)
{
        return xwosdl_skd_get_tickcount_lc();
}

/**
 * @brief XWOS API：获取当前CPU调度器的系统时间戳
 * @return 系统时间
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwtm_t xwos_skd_get_timestamp_lc(void)
{
        return xwosdl_skd_get_timestamp_lc();
}

/**
 * @brief XWOS API：关闭本地CPU调度器的抢占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
void xwos_skd_dspmpt_lc(void)
{
        xwosdl_skd_dspmpt_lc();
}

/**
 * @brief XWOS API：开启本地CPU调度器的抢占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
void xwos_skd_enpmpt_lc(void)
{
        xwosdl_skd_enpmpt_lc();
}

/**
 * @brief XWOS API：静态方式初始化线程
 * @param[in] thd: 线程对象的指针
 * @param[in] name: 线程的名字
 * @param[in] mainfunc: 线程函数的指针
 * @param[in] arg: 线程函数的参数
 * @param[in] stack: 线程栈的首地址指针
 * @param[in] stack_size: 线程栈的大小，以字节(byte)为单位
 * @param[in] priority: 线程的优先级
 * @param[in] attr: 线程属性
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 静态初始化线程需预先定义线程对象和线程栈数组，通常定义为全局变量。
 * - 栈数组的首地址与大小，必须要满足CPU的ABI规则，
 *   例如ARM，就需要8字节对齐。因此在定义栈数组时需要使用__xwcc_aligned(8)来修饰，
 *   且大小是8的倍数。
 * - attr参数的作用由OS而决定，在XWOS中，它的作用是设置线程的一些属性，取值为
 *   @ref xwos_skdattr_em中的一项。
 *   attr参数的类型是xwsq_t，此类型定义成与指针一样长，因此也可传递一个指针。
 */
static __xwos_inline_api
xwer_t xwos_thd_init(struct xwos_thd * thd,
                     const char * name,
                     xwos_thd_f mainfunc, void * arg,
                     xwstk_t * stack, xwsz_t stack_size,
                     xwpr_t priority, xwsq_t attr)
{
        return xwosdl_thd_init(&thd->osthd,
                               name,
                               (xwosdl_thd_f)mainfunc, arg,
                               stack, stack_size,
                               priority, attr);
}

/**
 * @brief XWOS API：动态方式创建线程并初始化
 * @param[out] thd: 指向缓冲区的指针，通过此缓冲区返回线程对象的指针
 * @param[in] name: 线程的名字
 * @param[in] mainfunc: 线程函数的指针
 * @param[in] arg: 线程函数的参数
 * @param[in] stack_size: 线程栈的大小，以字节(byte)为单位
 * @param[in] priority: 线程的优先级
 * @param[in] attr: 线程属性
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 动态创建线程采用的是操作系统提供的动态内存分配的接口创建线程对象和线程
 *   的栈；
 * - attr参数的作用由OS而决定，在XWOS中，它的作用是设置线程的一些属性。
 *   此参数的类型是xwsq_t，此类型定义成与unsigned long一样长，也就是
 *   和指针类型一样长，因此也可传递一个指针。
 */
static __xwos_inline_api
xwer_t xwos_thd_create(struct xwos_thd ** thdbuf,
                       const char * name,
                       xwos_thd_f mainfunc, void * arg,
                       xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        return xwosdl_thd_create((struct xwosdl_thd **)thdbuf,
                                 name,
                                 (xwosdl_thd_f)mainfunc, arg,
                                 stack_size,
                                 priority, attr);
}

/**
 * @brief XWOS API：获取线程对象的标签
 * @param[in] thd: 线程对象的指针
 * @return 线程对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_thd_gettik(struct xwos_thd * thd)
{
        return xwosdl_thd_gettik(&thd->osthd);
}

/**
 * @brief XWOS API：获取线程对象的描述符
 * @param[in] thd: 线程对象的指针
 * @return 线程对象的描述符
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwos_thd_d xwos_thd_getd(struct xwos_thd * thd)
{
        xwos_thd_d thdd;

        thdd.thd = thd;
        thdd.tik = xwosdl_thd_gettik(&thd->osthd);
        return thdd;
}

/**
 * @brief XWOS API：检查线程对象的标签并增加引用计数
 * @param[in] thdd: 线程对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_acquire(xwos_thd_d thdd)
{
        return xwosdl_thd_acquire(&thdd.thd->osthd, thdd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] thdd: 线程对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_release(xwos_thd_d thdd)
{
        return xwosdl_thd_release(&thdd.thd->osthd, thdd.tik);
}

/**
 * @brief XWOS API：增加线程对象的引用计数
 * @param[in] thd: 线程对象指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_grab(struct xwos_thd * thd)
{
        return xwosdl_thd_grab(&thd->osthd);
}

/**
 * @brief XWOS API：减少线程对象的引用计数
 * @param[in] thd: 线程对象指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_put(struct xwos_thd * thd)
{
        return xwosdl_thd_put(&thd->osthd);
}

/**
 * @brief XWOS API：终止线程并等待它的返回值，最后回收线程资源
 * @param[in] thd: 线程对象的指针
 * @param[out] trc: 指向缓冲区的指针，通过此缓冲区返回子线程的返回值，
 *  可为NULL，表示不需要获取返回值
 * @return 错误码
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
static __xwos_inline_api
xwer_t xwos_thd_stop(struct xwos_thd * thd, xwer_t * trc)
{
        return xwosdl_thd_stop(&thd->osthd, trc);
}

/**
 * @brief XWOS API：取消线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数功能类似于pthread_cancel()，通知子线程退出；
 * - 此函数可中断子线程的阻塞态与睡眠态；
 * - 子线程收到终止信号后，并不会立即退出，退出的时机由子线程自己控制；
 * - 此函数与xwos_thd_stop()不同，不会阻塞调用者，也不会回收子线程资源，因此
 *   可在中断中调用。
 */
static __xwos_inline_api
xwer_t xwos_thd_cancel(struct xwos_thd * thd)
{
        return xwosdl_thd_cancel(&thd->osthd);
}

/**
 * @brief XWOS API：等待线程结束并获取它的返回值，最后回收线程资源
 * @param[in] thd: 线程对象的指针
 * @param[out] trc: 指向缓冲区的指针，通过此缓冲区返回子线程的返回值，
 *  可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 线程不是Joinable的
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
static __xwos_inline_api
xwer_t xwos_thd_join(struct xwos_thd * thd, xwer_t * trc)
{
        return xwosdl_thd_join(&thd->osthd, trc);
}

/**
 * @brief XWMP API：分离线程
 * @param[in] thd: 线程对象的指针
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
static __xwos_inline_api
xwer_t xwos_thd_detach(struct xwos_thd * thd)
{
        return xwosdl_thd_detach(&thd->osthd);
}

/**
 * @brief XWOS API：中断线程的睡眠或阻塞状态
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_intr(struct xwos_thd * thd)
{
        return xwosdl_thd_intr(&thd->osthd);
}

/**
 * @brief XWOS API：将线程迁移到目标CPU
 * @param[in] thd: 线程对象的指针
 * @param[in] dstcpu: 目标CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个thd，不可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_migrate(struct xwos_thd * thd, xwid_t dstcpu)
{
        return xwosdl_thd_migrate(&thd->osthd, dstcpu);
}

/**
 * @brief XWOS API：获取当前线程的对象指针
 * @return 线程对象指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
struct xwos_thd * xwos_cthd_self(void)
{
        return (struct xwos_thd *)xwosdl_cthd_self();
}

/**
 * @brief XWOS API：通知调度器重新选择线程
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
void xwos_cthd_yield(void)
{
        xwosdl_cthd_yield();
}

/**
 * @brief XWOS API：退出当前线程
 * @param[in] rc: 线程退出时的返回值
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数会使得调用的线程立即退出并抛出返回值，其功能类似于POSIX线程库中的
 *   pthread_exit()函数。
 */
static __xwos_inline_api
void xwos_cthd_exit(xwer_t rc)
{
        xwosdl_cthd_exit(rc);
}

/**
 * @brief XWOS API：判断当前线程是否可被冻结
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
bool xwos_cthd_shld_frz(void)
{
        return xwosdl_cthd_shld_frz();
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
 * - 此函数常用在线程主循环的循环条件中。例如：
 *   ```C
 *   xwer_t thread_main(void * arg)
 *   {
 *           while (!xwos_cthd_shld_stop()) {
 *                   thread loop ...;
 *           }
 *   }
 *   ```
 */
static __xwos_inline_api
bool xwos_cthd_shld_stop(void)
{
        return xwosdl_cthd_shld_stop();
}

/**
 * @brief XWOS API：判断当前线程是否可被冻结，如果是，就冻结线程，
 *                  之后再判断线程是否可以退出
 * @param[out] frozen: 指向缓冲区的指针，通过此缓冲区返回线程是否被冻结过
 * @return 布尔值
 * @retval true: 可以退出
 * @retval false: 不能退出
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数在@ref xwos_cthd_shld_stop()的基础上增加了对冻结条件是否满足
 *   的判断，如果可以冻结，就在函数中将线程冻结。例如：
 *   ```C
 *   xwer_t thread_main(void * arg)
 *   {
 *           while (!xwos_cthd_frz_shld_stop()) {
 *                   thread loop ...;
 *           }
 *   }
 *   ```
 */
static __xwos_inline_api
bool xwos_cthd_frz_shld_stop(bool * frozen)
{
        return xwosdl_cthd_frz_shld_stop(frozen);
}

/**
 * @brief XWOS API：当前线程睡眠一段时间
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的睡眠时间
 * + (O) 作为输出时，返回剩余的睡眠时间
 * @return 错误码
 * @retval XWOK: 没有错误，指针xwtm指向的缓冲区内的睡眠时间减为0
 * @retval -EINTR: 睡眠过程被中断，指针xwtm指向的缓冲区返回剩余的时间
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cthd_sleep(xwtm_t * xwtm)
{
        return xwosdl_cthd_sleep(xwtm);
}

/**
 * @brief XWOS API：当前线程从一个时间起点睡眠到另一个时间点
 * @param[in,out] origin: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，作为时间起点
 * + (O) 作为输出时，返回线程被唤醒的时间（可作为下一次时间起点，形成精确的周期）
 * @param[in] inc: 期望被唤醒的时间增量（相对于时间原点）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 睡眠过程被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwosdl_cthd_sleep_from(origin, inc);
}

/**
 * @brief XWOS API：冻结当前线程
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cthd_freeze(void)
{
        return xwosdl_cthd_freeze();
}

#if (XWOS_THD_LOCAL_DATA_NUM > 0U)
/**
 * @brief XWOS API：设置线程的本地数据指针
 * @param[in] thd: 线程对象的指针
 * @param[in] pos: 数据存放位置的索引
 * @param[in] data: 数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_set_data(struct xwos_thd * thd, xwsq_t pos, void * data)
{
        return xwosdl_thd_set_data(&thd->osthd, pos, data);
}

/**
 * @brief XWOS API：获取线程的本地数据指针
 * @param[in] thd: 线程对象的指针
 * @param[in] pos: 数据存放位置的索引
 * @param[out] databuf: 指向缓冲区的指针，通过此缓冲区返回数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_get_data(struct xwos_thd * thd, xwsq_t pos, void ** databuf)
{
        return xwosdl_thd_get_data(&thd->osthd, pos, databuf);
}

/**
 * @brief XWOS API：设置当前线程的本地数据指针
 * @param[in] pos: 数据存放位置的索引
 * @param[in] data: 数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cthd_set_data(xwsq_t pos, void * data)
{
        return xwosdl_cthd_set_data(pos, data);
}

/**
 * @brief XWOS API：获取当前线程的本地数据指针
 * @param[in] pos: 数据存放位置的索引
 * @param[out] databuf: 指向缓冲区的指针，通过此缓冲区返回数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cthd_get_data(xwsq_t pos, void ** databuf)
{
        return xwosdl_cthd_get_data(pos, databuf);

}
#endif

/**
 * @} xwos_skd
 */

#endif /* xwos/osal/skd.h */
