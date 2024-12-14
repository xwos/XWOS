/**
 * @file
 * @brief 操作系统抽象层：顺序锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_lock_seqlock_h__
#define __xwos_osal_lock_seqlock_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/lock/seqlock.h>
#include <xwos/osal/irq.h>

/**
 * @defgroup xwos_lock_seqlock 顺序锁
 * @ingroup xwos_lock
 * 技术参考手册： [顺序锁](../Docs/TechRefManual/Lock/Seqlock)
 *
 *
 * ## 顺序锁的初始化
 *
 * + `xwos_sqlk_init()` ：静态初始化
 *
 *
 * ## 非独占读
 *
 * + `xwos_sqlk_rd_begin()` ：开启共享读临界区
 * + `xwos_sqlk_rd_retry()` ：关闭共享读临界区，并检查是否需要重试
 * + `xwos_sqlk_get_seq()` ：获取顺序锁的顺序值
 *
 *
 * ## 独占读
 *
 * + 保护 **线程** 上下文之间的独占读临界区
 *   + `xwos_sqlk_rdex_lock()` ：上锁，关闭抢占
 *   + `xwos_sqlk_rdex_trylock()` ：尝试上锁，关闭抢占
 *   + `xwos_sqlk_rdex_unlock()` ：解锁，开启抢占
 *
 * + 保护 **任意** 上下文之间的独占读临界区
 *   + `xwos_sqlk_rdex_lock_cpuirq()` ：上锁，关闭抢占、CPU中断
 *   + `xwos_sqlk_rdex_trylock_cpuirq()` ：尝试上锁，关闭抢占、CPU中断
 *   + `xwos_sqlk_rdex_unlock_cpuirq()` ：解锁，开启抢占、CPU中断
 *
 * + 保护 **任意** 上下文之间的 **嵌套** 独占读临界区
 *   + `xwos_sqlk_rdex_lock_cpuirqsv()` ：上锁，关闭抢占，保存CPU中断标志并关闭
 *   + `xwos_sqlk_rdex_trylock_cpuirqsv()` ：尝试上锁，关闭抢占，保存CPU中断标志并关闭
 *   + `xwos_sqlk_rdex_unlock_cpuirqrs()` ：解锁，开启抢占，恢复CPU中断
 *
 * + 保护 **线程** 和 **指定中断** 上下文之间的独占读临界区
 *   + `xwos_sqlk_rdex_lock_irqs()` ：上锁，关闭抢占，关闭部分中断
 *   + `xwos_sqlk_rdex_trylock_irqs()` ：尝试上锁，关闭抢占，关闭部分中断
 *   + `xwos_sqlk_rdex_unlock_irqs()` ：解锁，开启抢占、开启部分中断
 *
 * + 保护 **线程** 和 **指定中断** 上下文之间的 **嵌套** 独占读临界区
 *   + `xwos_sqlk_rdex_lock_irqssv()` ：上锁，关闭抢占，保存部分中断标志并关闭
 *   + `xwos_sqlk_rdex_trylock_irqssv()` ：尝试上锁，关闭抢占，保存部分中断标志并关闭
 *   + `xwos_sqlk_rdex_unlock_irqsrs()` ：解锁，开启抢占、恢复部分中断
 *
 * + 保护 **线程** 和 **中断底半部** 上下文之间的独占读临界区
 *   + `xwos_sqlk_rdex_lock_bh()` ：上锁，关闭抢占、中断底半部
 *   + `xwos_sqlk_rdex_trylock_bh()` ：尝试上锁，关闭抢占、中断底半部
 *   + `xwos_sqlk_rdex_unlock_bh()` ：解锁，开启抢占、中断底半部
 *
 *
 * ## 写
 *
 * + 保护 **线程** 上下文之间的读写临界区
 *   + `xwos_sqlk_wr_lock()` ：上锁，关闭抢占
 *   + `xwos_sqlk_wr_trylock()` ：尝试上锁，关闭抢占
 *   + `xwos_sqlk_wr_unlock()` ：解锁，开启抢占
 *
 * + 保护 **任意** 上下文之间的读写临界区
 *   + `xwos_sqlk_wr_lock_cpuirq()` ：上锁，关闭抢占、CPU中断
 *   + `xwos_sqlk_wr_trylock_cpuirq()` ：尝试上锁，关闭抢占、CPU中断
 *   + `xwos_sqlk_wr_unlock_cpuirq()` ：解锁，开启抢占、CPU中断
 *
 * + 保护 **任意** 上下文之间的 **嵌套** 读写临界区
 *   + `xwos_sqlk_wr_lock_cpuirqsv()` ：上锁，关闭抢占，保存CPU中断标志并关闭
 *   + `xwos_sqlk_wr_trylock_cpuirqsv()` ：尝试上锁，关闭抢占，保存CPU中断标志并关闭
 *   + `xwos_sqlk_wr_unlock_cpuirqrs()` ：解锁，开启抢占，恢复CPU中断
 *
 * + 保护 **线程** 和 **指定中断** 上下文之间的读写临界区
 *   + `xwos_sqlk_wr_lock_irqs()` ：上锁，关闭抢占，关闭部分中断
 *   + `xwos_sqlk_wr_trylock_irqs()` ：尝试上锁，关闭抢占，关闭部分中断
 *   + `xwos_sqlk_wr_unlock_irqs()` ：解锁，开启抢占、开启部分中断
 *
 * + 保护 **线程** 和 **指定中断** 上下文之间的 **嵌套** 读写临界区
 *   + `xwos_sqlk_wr_lock_irqssv()` ：上锁，关闭抢占，保存部分中断标志并关闭
 *   + `xwos_sqlk_wr_trylock_irqssv()` ：尝试上锁，关闭抢占，保存部分中断标志并关闭
 *   + `xwos_sqlk_wr_unlock_irqsrs()` ：解锁，开启抢占、恢复部分中断
 *
 * + 保护 **线程** 和 **中断底半部** 上下文之间的读写临界区
 *   + `xwos_sqlk_wr_lock_bh()` ：上锁，关闭抢占、中断底半部
 *   + `xwos_sqlk_wr_trylock_bh()` ：尝试上锁，关闭抢占、中断底半部
 *   + `xwos_sqlk_wr_unlock_bh()` ：解锁，开启抢占、中断底半部
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/lock/seqlock.hxx
 * @{
 */

/**
 * @brief XWOS API：顺序锁
 */
struct xwos_sqlk {
        xwosdl_sqlk_t ossql;
};

/**
 * @brief XWOS API：顺序锁的初始值
 */
#define XWOS_SQLK_INITIALIZER { .ossql = XWOSDL_SQLK_INITIALIZER, }

/**
 * @brief 操作系统抽象层：顺序锁顺序值的粒度
 */
#define XWOS_SQLK_GRANULARITY XWOSDL_SQLK_GRANULARITY

/**
 * @brief XWOS API：初始化顺序锁
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_sqlk_init(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_init(&sql->ossql);
}

/**
 * @brief XWOS API：开启共享读临界区
 * @param[in] sql: 顺序锁的指针
 * @return 当前顺序值
 * @note
 * + 上下文：任意
 * @details
 * + 若顺序锁已处于写临界区，调用的线程会自旋等待。
 */
static __xwos_inline_api
xwsq_t xwos_sqlk_rd_begin(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_rd_begin(&sql->ossql);
}

/**
 * @brief XWOS API：关闭共享读临界区，并检查是否需要重试
 * @param[in] sql: 顺序锁的指针
 * @param[in] start: @ref xwos_sqlk_rd_begin() 返回的顺序值，用于测试顺序值是否发生改变
 * @retval true: 顺序值发生了改变，需要重试
 * @retval false: 顺序值未发生改变
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
bool xwos_sqlk_rd_retry(struct xwos_sqlk * sql, xwsq_t start)
{
        return xwosdl_sqlk_rd_retry(&sql->ossql, start);
}

/**
 * @brief XWOS API：获取顺序锁的顺序值
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwsq_t xwos_sqlk_get_seq(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_get_seq(&sql->ossql);
}

/**
 * @brief XWOS API：开启独占读临界区
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程
 * @details
 * + 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_rdex_lock(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_rdex_lock(&sql->ossql);
}

/**
 * @brief XWOS API：尝试开启独占读临界区
 * @param[in] sql: 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程
 * @details
 * + 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_rdex_trylock(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_rdex_trylock(&sql->ossql);
}

/**
 * @brief XWOS API：关闭独占读临界区
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
void xwos_sqlk_rdex_unlock(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_rdex_unlock(&sql->ossql);
}

/**
 * @brief XWOS API：开启独占读临界区，并关闭本地CPU的中断
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：任意
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_rdex_lock_cpuirq(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_rdex_lock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOS API：尝试开启独占读临界区，并关闭本地CPU的中断
 * @param[in] sql: 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：任意
 * @details
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_rdex_trylock_cpuirq(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_rdex_trylock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOS API：关闭独占读临界区，并开启本地CPU的中断
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_sqlk_rdex_unlock_cpuirq(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_rdex_unlock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOS API：开启独占读临界区，保存本地CPU的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[out] cpuirq: 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * + 上下文：任意
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_rdex_lock_cpuirqsv(struct xwos_sqlk * sql,
                                  xwreg_t * cpuirq)
{
        xwosdl_sqlk_rdex_lock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOS API：尝试开启独占读临界区，保存本地CPU的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[out] cpuirq: 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：任意
 * @details
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_rdex_trylock_cpuirqsv(struct xwos_sqlk * sql,
                                       xwreg_t * cpuirq)
{
        return xwosdl_sqlk_rdex_trylock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOS API：关闭独占读临界区，恢复本地CPU的中断标志
 * @param[in] sql: 顺序锁的指针
 * @param[in] cpuirq: 本地CPU的中断标志
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_sqlk_rdex_unlock_cpuirqrs(struct xwos_sqlk * sql,
                                    xwreg_t cpuirq)
{
        xwosdl_sqlk_rdex_unlock_cpuirqrs(&sql->ossql, cpuirq);
}

/**
 * @brief XWOS API：开启独占读临界区，并关闭部分中断
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_rdex_lock_irqs(struct xwos_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num)
{
        xwosdl_sqlk_rdex_lock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOS API：尝试开启独占读临界区，并关闭部分中断
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] num: 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_rdex_trylock_irqs(struct xwos_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwsz_t num)
{
        return xwosdl_sqlk_rdex_trylock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOS API：关闭独占读临界区，并开启部分中断
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 */
static __xwos_inline_api
void xwos_sqlk_rdex_unlock_irqs(struct xwos_sqlk * sql,
                                const xwirq_t irqs[],
                                xwsz_t num)
{
        xwosdl_sqlk_rdex_unlock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOS API：开启独占读临界区，保存部分中断的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[out] flags: 缓冲区指针，用于返回部分中断的中断标志
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_rdex_lock_irqssv(struct xwos_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_rdex_lock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOS API：尝试开启独占读临界区，保存部分中断的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[out] flags: 缓冲区指针，用于返回部分中断的中断标志
 * @param[in] num: 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_rdex_trylock_irqssv(struct xwos_sqlk * sql,
                                     const xwirq_t irqs[],
                                     xwreg_t flags[], xwsz_t num)
{
        return xwosdl_sqlk_rdex_trylock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOS API：关闭独占读临界区，恢复部分中断的中断标志
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] flags: 部分中断的中断标志数组
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 */
static __xwos_inline_api
void xwos_sqlk_rdex_unlock_irqsrs(struct xwos_sqlk * sql,
                                  const xwirq_t irqs[],
                                  xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_rdex_unlock_irqsrs(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOS API：开启独占读临界区，关闭本地CPU的中断底半部
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程、中断底半部
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数只会关闭本地CPU的抢占和中断底半部，
 *   因此只能保证其临界区在线程与中断底半部中是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_rdex_lock_bh(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_rdex_lock_bh(&sql->ossql);
}

/**
 * @brief XWOS API：尝试开启独占读临界区，关闭本地CPU的中断底半部
 * @param[in] sql: 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程、中断底半部
 * @details
 * + 此函数只会关闭本地CPU的抢占和中断底半部，
 *   因此只能保证其临界区在线程与中断底半部中是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_rdex_trylock_bh(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_rdex_trylock_bh(&sql->ossql);
}

/**
 * @brief XWOS API：关闭独占读临界区，开启本地CPU的中断底半部
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程、中断底半部
 */
static __xwos_inline_api
void xwos_sqlk_rdex_unlock_bh(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_rdex_unlock_bh(&sql->ossql);
}

/**
 * @brief XWOS API：开启写临界区
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 */
static __xwos_inline_api
void xwos_sqlk_wr_lock(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_wr_lock(&sql->ossql);
}

/**
 * @brief XWOS API：尝试开启写临界区
 * @param[in] sql: 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_sqlk_wr_trylock(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_wr_trylock(&sql->ossql);
}

/**
 * @brief XWOS API：关闭写临界区
 * @param[in] sql: 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwos_sqlk_wr_unlock(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_wr_unlock(&sql->ossql);
}

/**
 * @brief XWOS API：开启写临界区，并关闭本地CPU的中断
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_wr_lock_cpuirq(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_wr_lock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOS API：尝试开启写临界区，并关闭本地CPU的中断
 * @param[in] sql: 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：任意
 * @details
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_wr_trylock_cpuirq(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_wr_trylock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOS API：关闭写临界区，并开启本地CPU的中断
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_sqlk_wr_unlock_cpuirq(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_wr_unlock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOS API：开启写临界区，保存本地CPU的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[out] cpuirq: 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * + 上下文：任意
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_wr_lock_cpuirqsv(struct xwos_sqlk * sql,
                                xwreg_t * cpuirq)
{
        xwosdl_sqlk_wr_lock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOS API：尝试开启写临界区，保存本地CPU的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[out] cpuirq: 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：任意
 * @details
 * + 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_wr_trylock_cpuirqsv(struct xwos_sqlk * sql,
                                     xwreg_t * cpuirq)
{
        return xwosdl_sqlk_wr_trylock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOS API：关闭写临界区，恢复本地CPU的中断标志
 * @param[in] sql: 顺序锁的指针
 * @param[in] cpuirq: 本地CPU的中断标志
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
void xwos_sqlk_wr_unlock_cpuirqrs(struct xwos_sqlk * sql,
                                  xwreg_t cpuirq)
{
        xwosdl_sqlk_wr_unlock_cpuirqrs(&sql->ossql, cpuirq);
}

/**
 * @brief XWOS API：开启写临界区，并关闭部分中断
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文中是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_wr_lock_irqs(struct xwos_sqlk * sql,
                            const xwirq_t irqs[],
                            xwsz_t num)
{
        xwosdl_sqlk_wr_lock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOS API：尝试开启写临界区，并关闭部分中断
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] num: 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_wr_trylock_irqs(struct xwos_sqlk * sql,
                                 const xwirq_t irqs[],
                                 xwsz_t num)
{
        return xwosdl_sqlk_wr_trylock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOS API：关闭写临界区，并开启部分中断
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 */
static __xwos_inline_api
void xwos_sqlk_wr_unlock_irqs(struct xwos_sqlk * sql,
                              const xwirq_t irqs[],
                              xwsz_t num)
{
        xwosdl_sqlk_wr_unlock_irqs(&sql->ossql, irqs, num);
}


/**
 * @brief XWOS API：开启写临界区，保存部分中断的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[out] flags: 缓冲区指针，用于返回部分中断的中断标志
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_wr_lock_irqssv(struct xwos_sqlk * sql,
                              const xwirq_t irqs[],
                              xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_wr_lock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOS API：尝试开启写临界区，保存部分中断的中断标志并关闭
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[out] flags: 缓冲区指针，用于返回部分中断的中断标志
 * @param[in] num: 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 * @details
 * + 此函数只会关闭本地CPU的抢占和部分中断，
 *   因此只能保证其临界区在这些中断与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_wr_trylock_irqssv(struct xwos_sqlk * sql,
                                   const xwirq_t irqs[],
                                   xwreg_t flags[], xwsz_t num)
{
        return xwosdl_sqlk_wr_trylock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOS API：关闭写临界区，恢复部分中断的中断标志
 * @param[in] sql: 顺序锁的指针
 * @param[in] irqs: 中断资源数组指针
 * @param[in] flags: 部分中断的中断标志数组
 * @param[in] num: 数组中元素数量
 * @note
 * + 上下文：线程、中断资源数组中描述的中断
 */
static __xwos_inline_api
void xwos_sqlk_wr_unlock_irqsrs(struct xwos_sqlk * sql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_wr_unlock_irqsrs(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOS API：开启写临界区，关闭本地CPU的中断底半部
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程、中断底半部
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 * + 此函数只会关闭本地CPU的抢占和中断底半部，
 *   因此只能保证其临界区在线程与中断底半部中是安全的。
 */
static __xwos_inline_api
void xwos_sqlk_wr_lock_bh(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_wr_lock_bh(&sql->ossql);
}

/**
 * @brief XWOS API：尝试开启写临界区，关闭本地CPU的中断底半部
 * @param[in] sql: 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * + 上下文：线程、中断底半部
 * @details
 * + 此函数只会关闭本地CPU的抢占和中断底半部，
 *   因此只能保证其临界区在线程与中断底半部中是安全的。
 */
static __xwos_inline_api
xwer_t xwos_sqlk_wr_trylock_bh(struct xwos_sqlk * sql)
{
        return xwosdl_sqlk_wr_trylock_bh(&sql->ossql);
}

/**
 * @brief XWOS API：关闭写临界区，开启本地CPU的中断底半部
 * @param[in] sql: 顺序锁的指针
 * @note
 * + 上下文：线程、中断底半部
 * @details
 * + 若顺序锁已处于写或独占读临界区，就自旋等待。
 */
static __xwos_inline_api
void xwos_sqlk_wr_unlock_bh(struct xwos_sqlk * sql)
{
        xwosdl_sqlk_wr_unlock_bh(&sql->ossql);
}

/**
 * @} xwos_lock_seqlock
 */

#endif /* xwos/osal/lock/seqlock.h */
