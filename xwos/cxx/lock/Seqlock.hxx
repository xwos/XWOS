/**
 * @file
 * @brief xwos::lock::Seqlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_Seqlock_hxx__
#define __xwos_cxx_lock_Seqlock_hxx__

#include <xwos/osal/lock/seqlock.hxx>
#include <xwos/cxx/sync/Cond.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_Seqlock 顺序锁
 * @ingroup xwos_cxx_lock
 *
 * 顺序锁的结构体非常小，因此不分别提供静态初始化与动态创建的方法。
 *
 * ## 共享读
 *
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     xwsq_t seq = gLock.readBegin();
 *     do {
 *         共享读临界区
 *     } while (gLock.readRetry(seq));
 * }
 * ```
 *
 *
 * ## 顺序锁的RAII机制守卫
 *
 * 顺序锁不同的锁模式具有不同的RAII机制守卫：
 *
 * ### Seqlock::RdexLkGrd : 独占读普通上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，然后上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时会自动对顺序锁上锁
 *         xwos::lock::Seqlock::RdexLkGrd grd(gSlock);
 *         进入临界区
 *     } // grd析构时会自动解锁顺序锁
 * }
 * ```
 *
 *
 * ### Seqlock::TryRdexLkGrd : 独占读普通尝试上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Seqlock::TryRdexLkGrd grd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::RdexLkThGrd : 独占读CPU中断上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区
 *         xwos::lock::Seqlock::RdexLkThGrd grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::TryRdexLkThGrd : 独占读CPU中断尝试上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断，并在 `mRc` 中返回 `-EAGAIN`
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Seqlock::TryRdexLkThGrd grd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::RdexLkBhGrd : 独占读中断底半部上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，然后上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区
 *         xwos::lock::Seqlock::RdexLkBhGrd grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::TryRdexLkBhGrd : 独占读中断底半部尝试上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，
 *   然后尝试上锁顺序锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，开启本地CPU的中断底半部，
 *   并在 `mRc` 中返回 `-EAGAIN` 。
 * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Seqlock::RdexLkBhGrd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::RdexLkIrqsGrd : 独占读部分中断上锁模式
 *
 * + 构造函数关闭并保存模板参数列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区，同时关闭编号为1, 3, 5, 8的四个中断。
 *         xwos::lock::Seqlock::RdexLkIrqsGrd<1, 3, 5, 8> grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区，并恢复编号为1, 3, 5, 8的四个中断。
 * }
 * ```
 *
 * ### Seqlock::TryRdexLkIrqsGrd : 独占读部分中断尝试上锁模式
 *
 * + 构造函数关闭并保存模板参数列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断，
 *   并在 `mRc` 中返回 `-EAGAIN` 。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区，同时关闭编号为1, 3, 5, 8的四个中断。
 *         xwos::lock::Seqlock::TryRdexLkIrqsGrd<1, 3, 5, 8> grd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区，并恢复编号为1, 3, 5, 8的四个中断。
 * }
 * ```
 *
 *
 * ### Seqlock::WrLkGrd : 独占写普通上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，然后上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时会自动对顺序锁上锁
 *         xwos::lock::Seqlock::WrLkGrd grd(gSlock);
 *         进入临界区
 *     } // grd析构时会自动解锁顺序锁
 * }
 * ```
 *
 *
 * ### Seqlock::TryWrLkGrd : 独占写普通尝试上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Seqlock::TryWrLkGrd grd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::WrLkThGrd : 独占写CPU中断上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区
 *         xwos::lock::Seqlock::WrLkThGrd grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::TryWrLkThGrd : 独占写CPU中断尝试上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断，并在 `mRc` 中返回 `-EAGAIN`
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Seqlock::TryWrLkThGrd grd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::WrLkBhGrd : 独占写中断底半部上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，然后上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区
 *         xwos::lock::Seqlock::WrLkBhGrd grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::TryWrLkBhGrd : 独占写中断底半部尝试上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，
 *   然后尝试上锁顺序锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，开启本地CPU的中断底半部，
 *   并在 `mRc` 中返回 `-EAGAIN` 。
 * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Seqlock::WrLkBhGrd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Seqlock::WrLkIrqsGrd : 独占写部分中断上锁模式
 *
 * + 构造函数关闭并保存模板参数列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区，同时关闭编号为1, 3, 5, 8的四个中断。
 *         xwos::lock::Seqlock::WrLkIrqsGrd<1, 3, 5, 8> grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区，并恢复编号为1, 3, 5, 8的四个中断。
 * }
 * ```
 *
 * ### Seqlock::TryWrLkIrqsGrd : 独占写部分中断尝试上锁模式
 *
 * + 构造函数关闭并保存模板参数列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断，
 *   并在 `mRc` 中返回 `-EAGAIN` 。
 * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Seqlock.hxx>
 *
 * xwos::lock::Seqlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区，同时关闭编号为1, 3, 5, 8的四个中断。
 *         xwos::lock::Seqlock::TryWrLkIrqsGrd<1, 3, 5, 8> grd(gLock);
 *         if (XWOK == grd.getRc()) {
 *             // 上锁成功，进入临界区
 *         } else {
 *             // 上锁失败
 *         }
 *     } // 如果上锁成功，析构 `grd` 时退出临界区，并恢复编号为1, 3, 5, 8的四个中断。
 * }
 * ```
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/lock/Seqlock.hxx
 *
 * @{
 */

/**
 * @brief 顺序锁
 */
class Seqlock
{
  public:
    /**
     * @brief 锁状态
     */
    enum LockStatus : xwu32_t {
        SeqlockUnlocked = 0,
        SeqlockRdexLocked,
        SeqlockWrLocked,
    };

    /**
     * @brief 独占读普通上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，然后上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程
     */
    class RdexLkGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        RdexLkGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        RdexLkGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~RdexLkGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }
        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        RdexLkGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占读普通尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，然后尝试上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程
     */
    class TryRdexLkGrd
        : public RdexLkGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryRdexLkGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryRdexLkGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryRdexLkGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 独占读CPU中断上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断。
     * @note
     * + 上下文：任意
     */
    class RdexLkThGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;
        xwreg_t mCpuirq; /**< 保存当前CPU的中断标志 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        RdexLkThGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        RdexLkThGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~RdexLkThGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        RdexLkThGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占读CPU中断尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断，并在 `mRc` 中返回 `-EAGAIN`
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断。
     * @note
     * + 上下文：任意
     */
    class TryRdexLkThGrd
        : public RdexLkThGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryRdexLkThGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryRdexLkThGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryRdexLkThGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 独占读中断底半部上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，然后上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程、中断底半部
     */
    class RdexLkBhGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        RdexLkBhGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        RdexLkBhGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~RdexLkBhGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        RdexLkBhGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占读中断底半部尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，
     *   然后尝试上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，开启本地CPU的中断底半部，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程、中断底半部
     */
    class TryRdexLkBhGrd
        : public RdexLkBhGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryRdexLkBhGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryRdexLkBhGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryRdexLkBhGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 独占读部分中断上锁模式的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存 `TIrqList` 列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断。
     * @note
     * + 上下文：线程、`TIrqList` 列表中的中断
     */
    template<xwirq_t ... TIrqList>
    class RdexLkIrqsGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;
        xwirq_t mIrqs[sizeof...(TIrqList)];
        xwreg_t mIrqFlags[sizeof...(TIrqList)];

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        RdexLkIrqsGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        RdexLkIrqsGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~RdexLkIrqsGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        RdexLkIrqsGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占读部分中断尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存 `TIrqList` 列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断。
     * @note
     * + 上下文：线程、`TIrqList` 列表中的中断
     */
    template<xwirq_t ... TIrqList>
    class TryRdexLkIrqsGrd
        : public RdexLkIrqsGrd<TIrqList ...>
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryRdexLkIrqsGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryRdexLkIrqsGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryRdexLkIrqsGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 独占写普通上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，然后上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程
     */
    class WrLkGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        WrLkGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        WrLkGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~WrLkGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        WrLkGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占写普通尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，然后尝试上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程
     */
    class TryWrLkGrd
        : public WrLkGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryWrLkGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryWrLkGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryWrLkGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 独占写CPU中断上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断。
     * @note
     * + 上下文：任意
     */
    class WrLkThGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;
        xwreg_t mCpuirq; /**< 保存当前CPU的中断标志 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        WrLkThGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        WrLkThGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~WrLkThGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        WrLkThGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占写CPU中断尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断，并在 `mRc` 中返回 `-EAGAIN`
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断。
     * @note
     * + 上下文：任意
     */
    class TryWrLkThGrd
        : public WrLkThGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryWrLkThGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryWrLkThGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryWrLkThGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 独占写中断底半部上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，然后上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程、中断底半部
     */
    class WrLkBhGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        WrLkBhGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        WrLkBhGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~WrLkBhGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        WrLkBhGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占写中断底半部尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，
     *   然后尝试上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，开启本地CPU的中断底半部，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁顺序锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程、中断底半部
     */
    class TryWrLkBhGrd
        : public WrLkBhGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryWrLkBhGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryWrLkBhGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryWrLkBhGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 独占写部分中断上锁模式的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存 `TIrqList` 列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断。
     * @note
     * + 上下文：线程、`TIrqList` 列表中的中断
     */
    template<xwirq_t ... TIrqList>
    class WrLkIrqsGrd
    {
      protected:
        Seqlock * mSeqlock;
        enum LockStatus mStatus;
        xwirq_t mIrqs[sizeof...(TIrqList)];
        xwreg_t mIrqFlags[sizeof...(TIrqList)];

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        WrLkIrqsGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        WrLkIrqsGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~WrLkIrqsGrd();

        /**
         * @brief 解锁顺序锁
         * @details
         * + 主动解锁后，析构函数将不再解锁。
         */
        void unlock();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的指针
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码后，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond);

        /**
         * @brief 等待条件量
         * @param[in] cond: 条件量对象的引用
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond)` 。
         */
        xwer_t wait(sync::Cond & cond) { return wait(&cond); }

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的指针
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EFAULT: 无效的指针或空指针
         * @retval -EINVAL: 参数无效
         * @retval -ENOTTHDCTX: 不在线程上下文中
         * @retval -ENOLCK: 未上锁
         * @retval -EINTR: 等待被中断
         * @retval -ETIMEDOUT: 超时
         * @note
         * + 上下文：线程
         * @details
         * + 调用此C++API的线程会限时阻塞等待条件量，等待的同时会解锁顺序锁。
         * + 条件量被单播 `sync::Cond::unicast()` 或广播 `sync::Cond::broadcast()`
         *   时，会唤醒正在等待的线程。
         * + 线程被唤醒后，会重新用之前的上锁方式上锁顺序锁 。
         * + 重新上锁成功后将返回 `XWOK` 。
         * + 线程的等待被中断后，此C++API返回 `-EINTR` 。
         * + 线程的等待超时后，此C++API返回 `-ETIMEDOUT` 。
         * + 如果此C++API返回 **非** `XWOK` 的错误码，应该
         *   使用 `getStatus()` 确认是否上锁成功。
         */
        xwer_t wait(sync::Cond * cond, xwtm_t to);

        /**
         * @brief 限时等待条件量
         * @param[in] cond: 条件量对象的引用
         * @param[in] to: 期望唤醒的时间点
         * @return 错误码
         * @note
         * + 上下文：线程
         * @details
         * + 同 `wait(sync::Cond * cond, xwtm_t to)` 。
         */
        xwer_t wait(sync::Cond & cond, xwtm_t to) { return wait(&cond, to); }

      protected:
        WrLkIrqsGrd(): mSeqlock(nullptr), mStatus(SeqlockUnlocked) {}
    };

    /**
     * @brief 独占写部分中断尝试上锁模式下的顺序锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存 `TIrqList` 列表内的中断，关闭当前CPU调度器的抢占，上锁顺序锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁顺序锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断。
     * @note
     * + 上下文：线程、`TIrqList` 列表中的中断
     */
    template<xwirq_t ... TIrqList>
    class TryWrLkIrqsGrd
        : public WrLkIrqsGrd<TIrqList ...>
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象指针
         */
        TryWrLkIrqsGrd(Seqlock * seqlock);

        /**
         * @brief 构造顺序锁的RAII机制守卫
         * @param[in] seqlock: 顺序锁对象引用
         */
        TryWrLkIrqsGrd(Seqlock & seqlock);

        /**
         * @brief 析构顺序锁的RAII机制守卫
         */
        ~TryWrLkIrqsGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得顺序锁失败
         */
        xwer_t getRc() { return mRc; }
    };

  private:
    struct xwos_sqlk mLock; /**< 顺序锁结构体 */

  public:
    Seqlock(); /**< 构造函数 */
    ~Seqlock(); /**< 析构函数 */
    struct xwos_sqlk * getXwosObj() { return &mLock; } /**< 获取XWOS对象指针 */

    /**
     * @brief 开启共享读临界区
     * @return 当前顺序值
     * @note
     * + 上下文：任意
     * @details
     * + 若顺序锁已处于写临界区，调用的线程会自旋等待。
     */
    xwsq_t readBegin();

    /**
     * @brief 关闭共享读临界区，并检查是否需要重试
     * @param[in] start: @ref readBegin() 返回的顺序值，用于测试顺序值是否发生改变
     * @retval true: 顺序值发生了改变，需要重试
     * @retval false: 顺序值未发生改变
     * @note
     * + 上下文：任意
     */
    bool readRetry(xwsq_t start);

    /**
     * @brief 获取顺序锁的顺序值
     * @note
     * + 上下文：任意
     */
    xwsq_t getSeq();
};

/**
 * @} xwos_cxx_lock_Seqlock
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/Seqlock.hxx */
