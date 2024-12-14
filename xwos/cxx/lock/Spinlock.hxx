/**
 * @file
 * @brief xwos::lock::Spinlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_lock_Spinlock_hxx__
#define __xwos_cxx_lock_Spinlock_hxx__

#include <xwos/osal/lock/spinlock.hxx>

namespace xwos {
namespace lock {

/**
 * @defgroup xwos_cxx_lock_Spinlock 自旋锁
 * @ingroup xwos_cxx_lock
 *
 * 自旋锁的结构体非常小，因此不分别提供静态初始化与动态创建的方法。
 *
 *
 * ## 自旋锁的RAII机制守卫
 *
 * 自旋锁不同的锁模式具有不同的RAII机制守卫：
 *
 * ### Spinlock::LkGrd : 普通上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，然后上锁自旋锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时会自动对自旋锁上锁
 *         xwos::lock::Spinlock::LkGrd grd(gSlock);
 *         进入临界区
 *     } // grd析构时会自动解锁自旋锁
 * }
 * ```
 *
 *
 * ### Spinlock::TryLkGrd : 普通尝试上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Spinlock::TryLkGrd grd(gLock);
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
 * ### Spinlock::LkThGrd : CPU中断上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区
 *         xwos::lock::Spinlock::LkThGrd grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Spinlock::TryLkThGrd : CPU中断尝试上锁模式
 *
 * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断，并在 `mRc` 中返回 `-EAGAIN`
 * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复本地CPU的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Spinlock::TryLkThGrd grd(gLock);
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
 * ### Spinlock::LkBhGrd : 中断底半部上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，然后上锁自旋锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁自旋锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区
 *         xwos::lock::Spinlock::LkBhGrd grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区
 * }
 * ```
 *
 *
 * ### Spinlock::TryLkBhGrd : 中断底半部尝试上锁模式
 *
 * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，
 *   然后尝试上锁自旋锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，开启本地CPU的中断底半部，
 *   并在 `mRc` 中返回 `-EAGAIN` 。
 * + 析构函数解锁自旋锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区
 *         xwos::lock::Spinlock::LkBhGrd(gLock);
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
 * ### Spinlock::LkIrqsGrd : 部分中断上锁模式
 *
 * + 构造函数关闭并保存模板参数列表内的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
 *   上锁失败会自旋等待，直到上锁成功为止。
 * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时进入临界区，同时关闭编号为1, 3, 5, 8的四个中断。
 *         xwos::lock::Spinlock::LkIrqsGrd<1, 3, 5, 8> grd(gSlock);
 *         进入临界区
 *     } // 析构 `grd` 时退出临界区，并恢复编号为1, 3, 5, 8的四个中断。
 * }
 * ```
 *
 * ### Spinlock::TryLkIrqsGrd : 部分中断尝试上锁模式
 *
 * + 构造函数关闭并保存模板参数列表内的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
 *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断，
 *   并在 `mRc` 中返回 `-EAGAIN` 。
 * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
 *   用保存的中断标志恢复模板参数列表内的中断。
 * + 示例
 *
 * ```Cpp
 * #include <xwos/cxx/lock/Spinlock.hxx>
 *
 * xwos::lock::Spinlock gLock;
 *
 * void foo()
 * {
 *     { // 构造 `grd` 时尝试进入临界区，同时关闭编号为1, 3, 5, 8的四个中断。
 *         xwos::lock::Spinlock::TryLkIrqsGrd<1, 3, 5, 8> grd(gLock);
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
 * 头文件： @ref xwos/cxx/lock/Spinlock.hxx
 *
 * @{
 */

/**
 * @brief 自旋锁
 */
class Spinlock
{
  public:
    /**
     * @brief 普通上锁模式下的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，然后上锁自旋锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程
     */
    class LkGrd
    {
      protected:
        Spinlock * mSpinlock;

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        LkGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        LkGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~LkGrd();

      protected:
        LkGrd(): mSpinlock(nullptr) {}
    };

    /**
     * @brief 普通尝试上锁模式下的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，然后尝试上锁自旋锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程
     */
    class TryLkGrd
        : public LkGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        TryLkGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        TryLkGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~TryLkGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得自旋锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief CPU中断上锁模式下的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断。
     * @note
     * + 上下文：任意
     */
    class LkThGrd
    {
      protected:
        Spinlock * mSpinlock;
        xwreg_t mCpuirq; /**< 保存当前CPU的中断标志 */

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        LkThGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        LkThGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~LkThGrd();

      protected:
        LkThGrd(): mSpinlock(nullptr) {}
    };

    /**
     * @brief CPU中断尝试上锁模式下的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存本地CPU的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断，并在 `mRc` 中返回 `-EAGAIN`
     * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复本地CPU的中断。
     * @note
     * + 上下文：任意
     */
    class TryLkThGrd
        : public LkThGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        TryLkThGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        TryLkThGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~TryLkThGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得自旋锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 中断底半部上锁模式下的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，然后上锁自旋锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁自旋锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程、中断底半部
     */
    class LkBhGrd
    {
      protected:
        Spinlock * mSpinlock;

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        LkBhGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        LkBhGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~LkBhGrd();

      protected:
        LkBhGrd(): mSpinlock(nullptr) {}
    };

    /**
     * @brief 中断底半部尝试上锁模式下的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭当前CPU调度器的抢占，关闭本地CPU的中断底半部，
     *   然后尝试上锁自旋锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，开启本地CPU的中断底半部，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁自旋锁，开启本地CPU的中断底半部，开启当前CPU调度器的抢占。
     * @note
     * + 上下文：线程、中断底半部
     */
    class TryLkBhGrd
        : public LkBhGrd
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        TryLkBhGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        TryLkBhGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~TryLkBhGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得自旋锁失败
         */
        xwer_t getRc() { return mRc; }
    };

    /**
     * @brief 部分中断上锁模式的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存 `TIrqList` 列表内的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
     *   上锁失败会自旋等待，直到上锁成功为止。
     * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断。
     * @note
     * + 上下文：线程、`TIrqList` 列表中的中断
     */
    template<xwirq_t ... TIrqList>
    class LkIrqsGrd
    {
      protected:
        Spinlock * mSpinlock;
        xwirq_t mIrqs[sizeof...(TIrqList)];
        xwreg_t mIrqFlags[sizeof...(TIrqList)];

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        LkIrqsGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        LkIrqsGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~LkIrqsGrd();

      protected:
        LkIrqsGrd(): mSpinlock(nullptr) {}
    };

    /**
     * @brief 部分中断尝试上锁模式下的自旋锁RAII机制守卫
     * @details
     * + 构造函数关闭并保存 `TIrqList` 列表内的中断，关闭当前CPU调度器的抢占，上锁自旋锁。
     *   上锁失败不会自旋等待，会开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断，
     *   并在 `mRc` 中返回 `-EAGAIN` 。
     * + 析构函数解锁自旋锁，开启当前CPU调度器的抢占，
     *   用保存的中断标志恢复 `TIrqList` 列表内的中断。
     * @note
     * + 上下文：线程、`TIrqList` 列表中的中断
     */
    template<xwirq_t ... TIrqList>
    class TryLkIrqsGrd
        : public LkIrqsGrd<TIrqList ...>
    {
      private:
        xwer_t mRc; /**< 尝试上锁的结果 */

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        TryLkIrqsGrd(Spinlock * spinlock);

        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象引用
         */
        TryLkIrqsGrd(Spinlock & spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~TryLkIrqsGrd() {}

        /**
         * @brief 获取上锁结果错误码
         * @return 错误码
         * @retval XWOK: 没有错误
         * @retval -EAGAIN: 获得自旋锁失败
         */
        xwer_t getRc() { return mRc; }
    };

  private:
    struct xwos_splk mLock; /**< 自旋锁结构体 */

  public:
    Spinlock(); /**< 构造函数 */
    ~Spinlock(); /**< 析构函数 */
};

/**
 * @} xwos_cxx_lock_Spinlock
 */

} // namespace lock
} // namespace xwos

#endif /* xwos/cxx/lock/Spinlock.hxx */
