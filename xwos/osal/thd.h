/**
 * @file
 * @brief 操作系统抽象层：线程
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_thd_h__
#define __xwos_osal_thd_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/thd.h>
#include <xwos/osal/time.h>
#include <xwos/osal/skd.h>

/**
 * @defgroup xwos_thd 线程
 * @ingroup xwos
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
typedef xwer_t (* xwos_thd_f)(void *);

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
 * @brief XWOS API：线程属性
 */
struct xwos_thd_attr {
        const char * name; /**< 线程的名字 */
        xwstk_t * stack; /**< 线程栈的首地址指针：
                              + 静态初始化的线程，必须指定；
                              + 动态创建的线程，可指定也可不指定，
                                若不指定，栈内存采取动态申请的方式创建。*/
        xwsz_t stack_size; /**< 线程栈的大小，以字节(byte)为单位，
                                注意与CPU的ABI接口规定的内存边界对齐 */
        xwsz_t stack_guard_size; /**< 栈内存警戒线位置，
                                      当栈指针增长超过了警戒线就会触发stackoverflow警告 */
        xwpr_t priority; /**< 优先级 */
        bool detached; /**< 是否为分离态 */
        bool privileged; /**< 是否为特权线程 */
};

/**
 * @brief XWOS API：线程描述
 */
struct xwos_thd_desc {
        struct xwos_thd_attr attr; /**< 线程属性 */
        xwos_thd_f func; /**< 线程函数的指针 */
        void * arg; /**< 线程函数的参数 */
};


/**
 * @brief XWOS API：栈内存的默认大小
 */
#define XWOS_STACK_SIZE_DEFAULT         XWMMCFG_STACK_SIZE_DEFAULT

/**
 * @brief XWOS API：栈内存大小的最小值
 */
#define XWOS_STACK_SIZE_MIN             XWMMCFG_STACK_SIZE_MIN

/**
 * @brief XWOS API：默认的栈警戒线
 */
#define XWOS_STACK_GUARD_SIZE_DEFAULT   XWMMCFG_STACK_GUARD_SIZE_DEFAULT

/**
 * @brief XWOS API：线程本地数据指针的数量
 */
#define XWOS_THD_LOCAL_DATA_NUM         XWOSDL_THD_LOCAL_DATA_NUM


/**
 * @brief XWOS API：初始化线程属性结构体
 * @param[in] attr: 线程属性
 * @note
 * + 上下文：任意
 * + 重入性：可重入
 * @details
 * 线程属性结构体 `struct xwos_thd_attr` 是对 `pthread_attr_t` 的简化。
 *
 * 此函数功能类似于 `pthread_attr_init()` 。XWOS的线程属性结构体成员都是基本数据类型，
 * 初始化不会失败，因此此函数没有返回值。
 *
 * XWOS的线程属性结构体对用户透明，用户亦可在定义时直接对结构体成员赋值，
 * 因此XWOS不提供类似于 `pthread_attr_setXXX()` 与 `pthread_attr_getXXX()` 的函数组合。
 *
 * XWOS的线程属性结构体初始化不会额外动态申请内存，因此不需要对
 * 结构体进行销毁，也不存在与 `pthread_attr_destroy()` 类似的函数。
 */
static __xwos_inline_api
void xwos_thd_attr_init(struct xwos_thd_attr * attr)
{
        xwosdl_thd_attr_init((struct xwosdl_thd_attr *)attr);
}

/**
 * @brief XWOS API：静态方式初始化线程
 * @param[in] thd: 指向构造线程对象内存的指针
 * @param[out] thdd: 指向缓冲区的指针，通过此缓冲区返回线程对象描述符
 * @param[in] attr: 线程属性
 * @param[in] mainfunc: 线程函数的指针
 * @param[in] arg: 线程函数的参数
 * @return 错误码
 * @note
 * + 上下文：任意
 * + 重入性：同一线程对象，不可重入
 * @details
 * 此函数用于不使用任何动态内存申请的方式创建线程，所有需要的内存（线程对象结构体，栈内存数组）
 * 都需要在调用前预先定义，并且用户需要保证其生命周期在线程运行期间始终有效（通常最简单的方法是
 * 定义为全局变量）。
 *
 * 这种方式创建线程，可在编译阶段就计算出准确的内存使用量，避免动态内存申请时返回 `-ENOMEM` 错误，
 * 在一些强调安全的规范中，这是对RTOS的硬性要求。
 *
 * 参数 `attr` 类似于 `pthread_attr_t` ，但静态方式初始化线程时不可为空，
 * 必须用来传递预先定义的线程栈内存的地址。
 * 栈内存的首地址与大小，必须要满足CPU的ABI规则，例如ARM，就需要8字节对齐，且大小是8的倍数。
 * 因此在定义栈数组时需要使用 `__xwcc_aligned(8)` 来修饰。
 */
static __xwos_inline_api
xwer_t xwos_thd_init(struct xwos_thd * thd, xwos_thd_d * thdd,
                     const struct xwos_thd_attr * attr,
                     xwos_thd_f mainfunc, void * arg)
{
        return xwosdl_thd_init(&thd->osthd, (xwosdl_thd_d *)thdd,
                               (const struct xwosdl_thd_attr *)attr,
                               (xwosdl_thd_f)mainfunc, arg);
}

/**
 * @brief XWOS API：增加线程对象的引用计数
 * @param[in] thdd: 线程对象描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 线程对象无效
 * @note
 * + 上下文：任意
 * + 重入性：可重入
 * @details
 * 此函数主要用于管理 **静态对象** 的引用计数。
 * 若用于 **动态对象** ，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_thd_grab(xwos_thd_d thdd)
{
        return xwosdl_thd_grab(&thdd.thd->osthd);
}

/**
 * @brief XWOS API：减少线程对象的引用计数
 * @param[in] thdd: 线程对象描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 线程对象无效
 * @note
 * + 上下文：任意
 * + 重入性：可重入
 * @details
 * 此函数主要用于管理 **静态对象** 的引用计数。
 * 若用于 **动态对象** ，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_thd_put(xwos_thd_d thdd)
{
        return xwosdl_thd_put(&thdd.thd->osthd);
}

/**
 * @brief XWOS API：使用动态申请内存方式创建线程并初始化
 * @param[out] thdd: 指向缓冲区的指针，通过此缓冲区返回线程对象描述符
 * @param[in] attr: 线程属性
 * @param[in] mainfunc: 线程函数的指针
 * @param[in] arg: 线程函数的参数
 * @return 错误码
 * @note
 * + 上下文：任意
 * + 重入性：可重入
 * @details
 * + 参数 `attr` 类似于 `pthread_attr_t` ，可为 `NULL` ，表示采用默认属性创建线程。
 * + 若通过 `attr->stack` 指定内存作为栈，栈内存的首地址与大小，必须要满足CPU的ABI规则，
 *   例如ARM，就需要8字节对齐，且大小是8的倍数。因此在定义栈数组时需要使用 `__xwcc_aligned(8)` 来修饰。
 * + 如果栈内存也动态申请，地址对齐问题由操作系统内核处理。
 *
 */
static __xwos_inline_api
xwer_t xwos_thd_create(xwos_thd_d * thdd,
                       const struct xwos_thd_attr * attr,
                       xwos_thd_f mainfunc, void * arg)
{
        return xwosdl_thd_create((xwosdl_thd_d *)thdd,
                                 (const struct xwosdl_thd_attr *)attr,
                                 (xwosdl_thd_f)mainfunc, arg);
}

/**
 * @brief XWOS API：检查线程对象的标签并增加引用计数
 * @param[in] thdd: 线程对象描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 线程对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 * + 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_acquire(xwos_thd_d thdd)
{
        return xwosdl_thd_acquire(&thdd.thd->osthd, thdd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] thdd: 线程对象描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 线程对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 * + 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_thd_release(xwos_thd_d thdd)
{
        return xwosdl_thd_release(&thdd.thd->osthd, thdd.tik);
}

/**
 * @brief XWOS API：通知线程退出
 * @param[in] thdd: 线程对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 线程对象无效
 * @note
 * + 上下文：任意
 * + 重入性：可重入
 * @details
 * 此函数用于父线程A向另一个子线程B设置 **退出状态** 。
 *
 * + 调用此函数的父线程A不会等待子线程B退出。
 * + 子线程B可以是Joinable的，也可以是Detached的。
 * + 此函数可被重复调用，子线程B **退出状态** 一旦被设置，不可被清除。
 * + 此函数会中断子线程B的 **阻塞状态** 和 **睡眠状态** ，阻塞和睡眠的函数将以错误码 `-EINTR` 退出。
 * + 如果子线程B的 **阻塞状态** 是不可被中断的，中断将不会发生。
 * + 子线程B可以通过 `xwos_cthd_shld_stop()` 检测 **退出状态** ，并以此作为线程循环的退出条件。
 * + 此函数还可在其他上下文调用。
 *
 *
 * 在XWOS-V2.0以前，此函数被命名为 `xwos_thd_cancel()` ，名称类似于 `pthread_cacnel()` ，
 * 但功能差异较大。为避免迷惑，将其改名为 `xwos_thd_quit()` 。
 */
static __xwos_inline_api
xwer_t xwos_thd_quit(xwos_thd_d thdd)
{
        return xwosdl_thd_quit(&thdd.thd->osthd, thdd.tik);
}

/**
 * @brief XWOS API：等待线程结束，回收线程内存资源，抛出它的返回值
 * @param[in] thdd: 线程对象描述符
 * @param[out] trc: 指向缓冲区的指针，通过此缓冲区返回线程的返回值，可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 线程对象无效
 * @retval -EINVAL: 线程不是Joinable的
 * @retval -EPERM; 不允许线程join自己
 * @retval -EALREADY: 线程已被连接
 * @note
 * + 上下文：线程
 * + 重入性：不可重入
 * @details
 * 此函数类似于POSIX线程库的 `pthread_join()` 函数，用于等待另一个线程退出。
 *
 * 父线程A调用此函数时，会阻塞等待子线程B退出。子线程B退出后，此函数会释放子线程B的内存资源，并通过 `trc` 将子线程B的返回值返回给父线程A。
 *
 * + 父线程A的阻塞状态可被中断。
 * + 如果子线程B已经提前运行至退出，此函数可立即返回子线程B的返回值。
 * + 此函数只能对Joinable的子线程B使用。对Detached的子线程B调用此函数会得到错误码 `-EINVAL` 。
 * + 多个线程对同一个线程进行 `xwos_thd_join()` ，会返回错误码 `-EALREADY` 。
 * + 父线程A对自己调用此函数，会返回错误码 `-EPERM` 。
 *
 */
static __xwos_inline_api
xwer_t xwos_thd_join(xwos_thd_d thdd, xwer_t * trc)
{
        return xwosdl_thd_join(&thdd.thd->osthd, thdd.tik, trc);
}

/**
 * @brief XWOS API：终止线程并等待它退出，回收线程内存资源，并抛出线程的返回值
 * @param[in] thdd: 线程对象描述符
 * @param[out] trc: 指向缓冲区的指针，通过此缓冲区返回线程的返回值，可为NULL，表示不需要获取返回值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 线程对象无效
 * @retval -EINVAL: 线程不是Joinable的
 * @retval -EPERM; 不允许线程stop自己
 * @retval -EALREADY: 线程已连接
 * @note
 * + 上下文：线程
 * + 重入性：不可重入
 * @details
 * 此函数等价于 `xwos_thd_quit()` + `xwos_thd_join()` 。
 */
static __xwos_inline_api
xwer_t xwos_thd_stop(xwos_thd_d thdd, xwer_t * trc)
{
        return xwosdl_thd_stop(&thdd.thd->osthd, thdd.tik, trc);
}

/**
 * @brief XWMP API：分离线程
 * @param[in] thdd: 线程对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 线程已经被连接
 * @retval -EOBJDEAD: 线程对象无效
 * @note
 * + 上下文：任意
 * + 重入性：同一个线程对象，不可重复调用
 * @details
 * 此函数功能类似于 `pthread_detach()` ，将线程设置为 **分离状态** 。
 * 处于 **分离状态** 的线程退出后，系统会自动回收其内存资源，
 * 不需要另一个线程调用 `xwos_thd_join()` 来回收其内存资源。
 */
static __xwos_inline_api
xwer_t xwos_thd_detach(xwos_thd_d thdd)
{
        return xwosdl_thd_detach(&thdd.thd->osthd, thdd.tik);
}

/**
 * @brief XWOS API：将线程迁移到目标CPU
 * @param[in] thdd: 线程对象描述符
 * @param[in] dstcpu: 目标CPU的ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENODEV: CPU序号不存在
 * @retval -EOBJDEAD: 线程对象无效
 * @note
 * + 上下文：任意
 * + 重入性：对于同一个线程对象，不可重复调用
 * @details
 * 此函数用于将线程 `thdd.thd` 迁移到另一个CPU `dstcpu` 上。
 * 此函数只会发起一个申请就返回，不会等待迁移过程完成。线程的迁移由内核在恰当的时机完成。
 */
static __xwos_inline_api
xwer_t xwos_thd_migrate(xwos_thd_d thdd, xwid_t dstcpu)
{
        return xwosdl_thd_migrate(&thdd.thd->osthd, thdd.tik, dstcpu);
}

/**
 * @brief XWOS API：获取当前线程的对象描述符
 * @return 线程对象描述符
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwos_thd_d xwos_cthd_self(void)
{
        xwosdl_thd_d thdd;

        thdd = xwosdl_cthd_self();
        return (xwos_thd_d){(struct xwos_thd *)thdd.thd, thdd.tik};
}

/**
 * @brief XWOS API：通知调度器重新调度线程
 * @note
 * + 上下文：线程
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
 * + 上下文：线程
 * @details
 * 此函数类似于POSIX线程库中的 `pthread_exit()` ，会立即终止调用线程，并抛出返回值。
 * 线程调用此函数后就结束运行，不再返回。
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
 * + 上下文：线程
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
 * + 上下文：线程
 * @details
 * 退出的条件由 `xwos_thd_quit()` 或 `xwos_thd_stop()` 设置。
 * 此函数常用在线程主循环的循环条件中。例如：
 *
 * ```C
 * xwer_t thd_main(void * arg)
 * {
 *         while (!xwos_cthd_shld_stop()) {
 *                 // thread loop ...
 *         }
 * }
 * ```
 */
static __xwos_inline_api
bool xwos_cthd_shld_stop(void)
{
        return xwosdl_cthd_shld_stop();
}

/**
 * @brief XWOS API：判断当前线程是否可被冻结，如果是，就冻结线程，之后再判断线程是否可以退出
 * @param[out] frozen: 指向缓冲区的指针，通过此缓冲区返回线程是否被冻结过
 * @return 布尔值
 * @retval true: 可以退出
 * @retval false: 不能退出
 * @note
 * + 上下文：线程
 * @details
 * 此函数是在 @ref xwos_cthd_shld_stop() 的基础上增加了是否需要冻结的判断。
 * 如果可以冻结，就在函数内部将线程冻结。
 *
 * 参数```frozen```用于返回线程是否被冻结过，可以填```NULL```表示不需要关注这个状态。
 *
 * 例如：
 * ```C
 * xwer_t thread_main(void * arg)
 * {
 *         while (!xwos_cthd_frz_shld_stop(NULL)) { // 线程可能在此处发生冻结
 *                 // thread loop ...
 *         }
 * }
 * ```
 */
static __xwos_inline_api
bool xwos_cthd_frz_shld_stop(bool * frozen)
{
        return xwosdl_cthd_frz_shld_stop(frozen);
}

/**
 * @brief XWOS API：线程睡眠一段时间
 * @param[in] dur: 期望睡眠的时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 睡眠过程被中断
 * @retval -ETIMEDOUT: 输入的时间为负数
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 调用此函数的线程会睡眠 `dur` ，也即是线程会在 **当前时间点** + `dur` 时被唤醒。
 */
static __xwos_inline_api
xwer_t xwos_cthd_sleep(xwtm_t dur)
{
        return xwosdl_cthd_sleep_to(xwtm_ft(dur));
}

/**
 * @brief XWOS API：线程睡眠到一个时间点
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 睡眠过程被中断
 * @retval -ETIMEDOUT: 输入的时间为负数
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 调用此函数的线程，睡眠到未来的某个时间点 ```to``` 被唤醒。
 * 如果 ```to``` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_cthd_sleep_to(xwtm_t to)
{
        return xwosdl_cthd_sleep_to(to);
}

/**
 * @brief XWOS API：线程从一个时间起点睡眠到另一个时间点
 * @param[in,out] from: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，作为时间起点
 * + (O) 作为输出时，返回线程被唤醒的时间（可作为下一次时间起点，形成精确的周期）
 * @param[in] dur: 期望被唤醒的时间增量（相对于时间原点）
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 睡眠过程被中断
 * @retval -ETIMEDOUT: 时间点是过去
 * @retval -EDSPMPT: 抢占被关闭
 * @retval -EDSBH: 中断底半部被关闭
 * @note
 * + 上下文：线程
 * @details
 * 调用此函数前，需要先确定一个时间起点，可以通过 `xwtm_now()` 获取当前的系统时间作为起点。
 * 通过指针 `from` 将时间起点的地址传递给函数， `dur` 表示唤醒时间为 `*from` + `dur` 。
 *
 * 当线程被唤醒时，`*from` + `dur` 会覆盖到指针 `from` 指向的地址中，此时 `*from` 又可作为下次调用此函数的时间起点。
 * 由此，循环调用，可以形成周期为 `dur` ，更为精确的周期性睡眠唤醒。
 */
static __xwos_inline_api
xwer_t xwos_cthd_sleep_from(xwtm_t * from, xwtm_t dur)
{
        return xwosdl_cthd_sleep_from(from, dur);
}

/**
 * @brief XWOS API：冻结当前线程
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EPERM: 当前不需要冻结线程
 * @note
 * + 上下文：线程
 * @details
 * 线程的冻结功能，主要用于两个目的：
 *
 * + 系统进入低功耗前的准备工作；
 * + 线程从一个CPU迁移到另一个CPU之前的准备工作。
 *
 * 此函数由线程自己调用，冻结自身。但线程并不能随时冻结，必须满足下列条件之一：
 *
 * + 系统已经开始准备进入低功耗（调用了函数 `xwos_pm_suspend()` ）;
 * + 线程正准备开始迁移（调用了函数 `xwos_thd_migrate()` ）。
 *
 * 可以通过 `xwos_cthd_shld_frz()` 来判断是否可以冻结。
 * API `xwos_cthd_frz_shld_stop()` 的内部等价于 `xwos_cthd_shld_frz()` + `xwos_cthd_freeze()` 。
 */
static __xwos_inline_api
xwer_t xwos_cthd_freeze(void)
{
        return xwosdl_cthd_freeze();
}

#if (XWOS_THD_LOCAL_DATA_NUM > 0U)
/**
 * @brief XWOS API：设置线程的本地数据指针
 * @param[in] thdd: 线程对象描述符
 * @param[in] pos: 数据存放位置的索引
 * @param[in] data: 数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_thd_set_data(xwos_thd_d thdd, xwsq_t pos, void * data)
{
        return xwosdl_thd_set_data(&thdd.thd->osthd, thdd.tik, pos, data);
}

/**
 * @brief XWOS API：获取线程的本地数据指针
 * @param[in] thdd: 线程对象描述符
 * @param[in] pos: 数据存放位置的索引
 * @param[out] databuf: 指向缓冲区的指针，通过此缓冲区返回数据指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_thd_get_data(xwos_thd_d thdd, xwsq_t pos, void ** databuf)
{
        return xwosdl_thd_get_data(&thdd.thd->osthd, thdd.tik, pos, databuf);
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
 * + 上下文：线程
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
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_cthd_get_data(xwsq_t pos, void ** databuf)
{
        return xwosdl_cthd_get_data(pos, databuf);

}
#endif

/**
 * @} xwos_thd
 */

#endif /* xwos/osal/thd.h */
