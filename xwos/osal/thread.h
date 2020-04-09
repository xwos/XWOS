/**
 * @file
 * @brief 操作系统抽象层：线程
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_thread_h__
#define __xwos_osal_thread_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层：线程控制块
 */
struct xwosal_tcb {
        struct xwosdl_tcb ostcb; /**< 操作系统的线程控制块 */
};

/**
 * @brief 操作系统抽象层：线程函数指针类型
 */
typedef xwosdl_thrd_f xwosal_thrd_f;

/**
 * @brief 操作系统抽象层：线程描述结构
 */
struct xwosal_thrd_desc {
        const char * name; /**< 线程的名字 */
        xwpr_t prio; /**< 线程的优先级 */
        xwstk_t * stack; /**< 线程栈的首地址指针：
                              - 静态创建线程时必须指定；
                              - 动态创建也可指定，若不指定，栈内存也采取
                                动态内存申请。*/
        xwsz_t stack_size; /**< 线程栈的大小，以字节(byte)为单位，
                                注意与CPU的ABI接口规定的内存边界对齐 */
        xwosal_thrd_f func; /**< 线程函数的指针 */
        void * arg; /**< 线程函数的参数 */
        xwsq_t attr; /**< 与具体操作系统相关的一些私有数据 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOSAL_THRD_STACK_DYNAMIC       (NULL) /**< 栈内存动态方式创建 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL API：静态方式初始化线程
 * @param tcb: (I) 线程控制块对象的指针
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack: (I) 线程栈的首地址指针
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 静态初始化线程需预先定义线程控制块对象和线程栈数组，通常定义为全局变量。
 * - 栈数组的首地址与大小，必须要满足CPU的ABI规则，
 *   例如ARM，就需要8字节对齐。因此在定义栈数组时需要使用__aligned(8)来修饰，
 *   且大小是8的倍数。
 * - attr参数的作用由OS而决定，在XWOS中，它的作用是设置线程的一些属性，取值为
 *   @ref xwos_sdobj_attr_em中的一项。
 *   attr参数的类型是xwsq_t，此类型定义成与unsigned long一样长，也就是
 *   和指针一样长，因此也可传递一个指针。
 */
static __xwos_inline_api
xwer_t xwosal_thrd_init(struct xwosal_tcb * tcb,
                        const char * name,
                        xwosal_thrd_f mainfunc, void * arg,
                        xwstk_t * stack, xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr)
{
        return xwosdl_thrd_init(&tcb->ostcb,
                                name,
                                (xwosdl_thrd_f)mainfunc, arg,
                                stack, stack_size,
                                priority, attr);
}

/**
 * @brief XWOSAL API：销毁静态方式初始化的线程
 * @param tcb: (I) 线程控制块的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_thrd_destroy(struct xwosal_tcb * tcb)
{
        return xwosdl_thrd_destroy(&tcb->ostcb);
}

/**
 * @brief XWOSAL API：动态方式创建并初始化线程
 * @param tidbuf: (O) 指向缓冲区的指针，通过此缓冲区返回线程ID
 * @param name: (I) 线程的名字
 * @param mainfunc: (I) 线程函数的指针
 * @param arg: (I) 线程函数的参数
 * @param stack_size: (I) 线程栈的大小，以字节(byte)为单位
 * @param priority: (I) 线程的优先级
 * @param attr: (I) 线程属性
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 动态创建线程采用的是操作系统提供的动态内存分配的接口创建线程控制块和线程
 *   的栈，当线程不使用时调用@ref xwosal_thrd_delete()回收资源，防止资源泄漏。
 * - attr参数的作用由OS而决定，在XWOS中，它的作用是设置线程的一些属性。
 *   此参数的类型是xwsq_t，此类型定义成与unsigned long一样长，也就是
 *   和指针类型一样长，因此也可传递一个指针。
 */
static __xwos_inline_api
xwer_t xwosal_thrd_create(xwid_t * tidbuf,
                          const char * name,
                          xwosal_thrd_f mainfunc, void * arg,
                          xwsz_t stack_size,
                          xwpr_t priority, xwsq_t attr)
{
        return xwosdl_thrd_create(tidbuf,
                                  name,
                                  (xwosdl_thrd_f)mainfunc, arg,
                                  stack_size,
                                  priority, attr);
}

/**
 * @brief XWOSAL API：删除动态方式创建的线程
 * @param tid: (I) 线程ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_thrd_delete(xwid_t tid)
{
        return xwosdl_thrd_delete(tid);
}

/**
 * @brief XWOSAL API：让出当前线程的CPU，调度器重新选择线程
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
void xwosal_cthrd_yield(void)
{
        xwosdl_cthrd_yield();
}

/**
 * @brief XWOSAL API：退出当前线程
 * @param rc: (I) 线程退出时的返回值
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数会使得调用的线程立即退出并抛出返回值，其功能类似于POSIX线程库中的
 *   pthread_exit()函数。
 */
static __xwos_inline_api
void xwosal_cthrd_exit(xwer_t rc)
{
        xwosdl_cthrd_exit(rc);
}

/**
 * @brief XWOSAL API：终止线程并等待它的返回值
 * @param tid: (I) 线程ID
 * @param trc: (O) 指向缓冲区的指针，通过此缓冲区返回被终止线程的返回值，
 *                 可为NULL，表示不需要返回
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入，调用时要确保线程没有退出
 * @note
 * - 此函数通常由父线程调用，用于终止一个子线程。父线程会一直等待子线程退出，
 *   并获取子线程的返回值。此函数类似于POSIX线程库中的pthread_join()函数。
 * - *注意* ，子线程不可在此函数调用之前就返回，这是因为不同OS的实现有细微差异。
 *   例如xwosal在Linux内核中，使用了kthread_stop()函数来实现此API，
 *   kthread_stop()在调用时，如果子线程已经退出，会造成Kernel Panic。
 * - XuanWuOS的线程可在此函数调用之前就返回，此时此函数的返回值是一个
 *   小于0的错误码，指针trc指向的缓冲区依然可以获取线程之前的返回值。
 *   但是，为了保证基于操作系统抽象层的代码的移植性，不建议子线程提前退出。
 */
static __xwos_inline_api
xwer_t xwosal_thrd_terminate(xwid_t tid, xwer_t * trc)
{
        return xwosdl_thrd_terminate(tid, trc);
}

/**
 * @brief XWOSAL API：判断当前线程是否可退出，
 *                           如果不是，当前线程就一直等待，直到它被终止
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数由子线程调用，用于等待父线程调用xwosal_thrd_terminate()发出退出信号。
 * - 在父线程需要获取子线程运行结果的应用场景，子线程不可在父线程
 *   还未调用@ref xwosal_thrd_terminate()时就提前退出。但有时父线程可能还没来得及
 *   调用@ref xwosal_thrd_terminate()，子线程就已经准备退出。此种情况，子线程可
 *   通过调用此函数等待父线程调用@ref xwosal_thrd_terminate()。
 */
static __xwos_inline_api
void xwosal_cthrd_wait_exit(void)
{
        xwosdl_cthrd_wait_exit();
}

/**
 * @brief XWOSAL API：判断当前线程是否可被冻结
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
bool xwosal_cthrd_shld_frz(void)
{
        return xwosdl_cthrd_shld_frz();
}

/**
 * @brief XWOSAL API：判断当前线程是否可以退出
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数类似于@ref xwosal_cthrd_wait_exit()，但不会阻塞，只会立即返回
 *   true或false，通常用在线程主循环的循环条件中。例如：
 *   ```C
 *   xwer_t thread_main(void * arg)
 *   {
 *           while (!xwosal_cthrd_shld_stop()) {
 *                   thread loop ...;
 *           }
 *   }
 *   ```
 */
static __xwos_inline_api
bool xwosal_cthrd_shld_stop(void)
{
        return xwosdl_cthrd_shld_stop();
}

/**
 * @brief XWOSAL API：判断当前线程是否可被冻结，如果是，就冻结线程，
 *                    之后再判断线程是否可以退出
 * @param frozen (O) 指向缓冲区的指针，通过此缓冲区返回线程是否被冻结过
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 此函数在@ref xwosal_cthrd_shld_stop()的基础上增加了对冻结条件是否满足
 *   的判断，如果可以冻结，就在函数中将线程冻结。例如：
 *   ```C
 *   xwer_t thread_main(void * arg)
 *   {
 *           while (!xwosal_cthrd_frz_shld_stop()) {
 *                   thread loop ...;
 *           }
 *   }
 *   ```
 */
static __xwos_inline_api
bool xwosal_cthrd_frz_shld_stop(bool * frozen)
{
        return xwosdl_cthrd_frz_shld_stop(frozen);
}

/**
 * @brief XWOSAL API：获取当前线程的线程ID
 * @return 线程ID
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwosal_cthrd_get_id(void)
{
        return xwosdl_cthrd_get_id();
}

/**
 * @brief XWOSAL API：通过线程控制块对象的指针获取线程ID
 * @param tcb: (I) 线程控制块对象的指针
 * @return 线程ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwosal_thrd_get_id(struct xwosal_tcb * tcb)
{
        return xwosdl_thrd_get_id(&tcb->ostcb);
}

/**
 * @brief XWOSAL API：继续已经暂停的线程
 * @param tid: (I) 线程ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EPERM: 线程未被暂停
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 中断或其他线程可以调用此函数使已暂停的线程继续运行。
 * - 线程通过调用@ref xwosal_cthrd_pause()或@ref xwosal_cthrd_timedpause()暂停自己。
 */
static __xwos_inline_api
xwer_t xwosal_thrd_continue(xwid_t tid)
{
        return xwosdl_thrd_continue(tid);
}

/**
 * @brief XWOSAL API：暂停当前线程
 * @param lock: (I) 锁
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval OK: OK
 * @retval -EINTR: 暂停过程被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 调用的线程会阻塞等待。在此过程中，线程还可以解锁已经取得的锁，
 *   注意整个过程是原子的（不可打断的）。此函数支持的“锁”的类型有很多种，
 *   因此，传递锁的对象ID ，具体的类型由参数lktype决定。有些“锁”可能还需要
 *   额外的参数，由lockdata指明首地址，datanum指明数据的数量或大小。
 *   此函数返回时，会在lkst中返回锁的状态。
 */
static __xwos_inline_api
xwer_t xwosal_cthrd_pause(union xwlk_ulock lock, xwsq_t lktype,
                          void * lockdata, xwsz_t datanum,
                          xwsq_t * lkst)
{
        return xwosdl_cthrd_pause(lock, lktype, lockdata, datanum, lkst);
}

/**
 * @brief XWOSAL API：限时暂停当前线程
 * @param lock: (I) 锁
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval OK: OK
 * @retval -EINTR: 暂停过程被中断
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 调用的线程限时阻塞等待。在此过程中，线程还可以解锁已经取得的锁，
 *   注意整个过程是原子的（不可打断的）。此函数支持的“锁”的类型有很多种，
 *   因此，传递锁的对象ID ，具体的类型由参数lktype决定。有些“锁”可能还需要
 *   额外的参数，由lockdata指明首地址，datanum指明数据的数量或大小。
 *   此函数返回时，会在lkst中返回锁的状态。
 * - 函数返回 *-ETIMEDOUT* 时， *xwtm* 指向的缓冲区内的值减为0。
 */
static __xwos_inline_api
xwer_t xwosal_cthrd_timedpause(union xwlk_ulock lock, xwsq_t lktype,
                               void * lockdata, xwsz_t datanum,
                               xwtm_t * xwtm, xwsq_t * lkst)
{
        return xwosdl_cthrd_timedpause(lock, lktype, lockdata, datanum, xwtm, lkst);
}

/**
 * @brief XWOSAL API：调用的线程睡眠一段时间
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
static __xwos_inline_api
xwer_t xwosal_cthrd_sleep(xwtm_t * xwtm)
{
        return xwosdl_cthrd_sleep(xwtm);
}

/**
 * @brief XWOSAL API：调用的线程睡眠到精准的系统时间
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
static __xwos_inline_api
xwer_t xwosal_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        return xwosdl_cthrd_sleep_from(origin, inc);
}

/**
 * @brief XWOSAL API：中断线程的睡眠或阻塞状态
 * @param tid: (I) 线程ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_thrd_intr(xwid_t tid)
{
        return xwosdl_thrd_intr(tid);
}

/**
 * @brief XWOSAL API：冻结当前线程
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_cthrd_freeze(void)
{
        return xwosdl_cthrd_freeze();
}

/**
 * @brief XWOSAL API：将线程迁移到其他CPU
 * @param tid: (I) 线程ID
 * @param dstcpu: (I) 目标CPU的ID
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个tid，不可重入
 */
static __xw_inline
xwer_t xwosal_thrd_migrate(xwid_t tid, xwid_t dstcpu)
{
        return xwosdl_thrd_migrate(tid, dstcpu);
}

#endif /* xwos/osal/thread.h */
