/**
 * @file
 * @brief 操作系统抽象层：自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_lock_spinlock_h__
#define __xwos_osal_lock_spinlock_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief [XWOSAL] 自旋锁
 */
struct xwosal_splk {
        xwosdl_splk_t osspl; /**< 操作系统的自旋锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL API：初始化自旋锁
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_splk_init(struct xwosal_splk * spl)
{
        xwosdl_splk_init(&spl->osspl);
}

/**
 * @brief XWOSAL API：上锁自旋锁，若自旋锁已开启临界区，就自旋等待
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwos_inline_api
void xwosal_splk_lock(struct xwosal_splk * spl)
{
        xwosdl_splk_lock(&spl->osspl);
}

/**
 * @brief XWOSAL API：尝试上锁自旋锁
 * @param spl: (I) 自旋锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，立即返回，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_splk_trylock(struct xwosal_splk * spl)
{
        return xwosdl_splk_trylock(&spl->osspl);
}

/**
 * @brief XWOSAL API：解锁自旋锁
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_splk_unlock(struct xwosal_splk * spl)
{
        xwosdl_splk_unlock(&spl->osspl);
}

/**
 * @brief XWOSAL API：上锁自旋锁，并关闭本地CPU的中断
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
static __xwos_inline_api
void xwosal_splk_lock_cpuirq(struct xwosal_splk * spl)
{
        xwosdl_splk_lock_cpuirq(&spl->osspl);
}

/**
 * @brief XWOSAL API：尝试上锁自旋锁，并关闭本地CPU的中断
 * @param spl: (I) 自旋锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，立即返回，不会自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_splk_trylock_cpuirq(struct xwosal_splk * spl)
{
        return xwosdl_splk_trylock_cpuirq(&spl->osspl);
}

/**
 * @brief XWOSAL API：解锁自旋锁，并开启本地CPU的中断
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_splk_unlock_cpuirq(struct xwosal_splk * spl)
{
        xwosdl_splk_unlock_cpuirq(&spl->osspl);
}

/**
 * @brief XWOSAL API：上锁自旋锁，保存本地CPU的中断标志并关闭
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
static __xwos_inline_api
void xwosal_splk_lock_cpuirqsv(struct xwosal_splk * spl, xwreg_t * cpuirq)
{
        xwosdl_splk_lock_cpuirqsv(&spl->osspl, cpuirq);
}

/**
 * @brief XWOSAL API：尝试上锁自旋锁，保存本地CPU的中断标志并关闭
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，立即返回，不会自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_splk_trylock_cpuirqsv(struct xwosal_splk * spl, xwreg_t * cpuirq)
{
        return xwosdl_splk_trylock_cpuirqsv(&spl->osspl, cpuirq);
}

/**
 * @brief XWOSAL API：解锁自旋锁，并恢复本地CPU的中断标志
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_splk_unlock_cpuirqrs(struct xwosal_splk * spl, xwreg_t cpuirq)
{
        xwosdl_splk_unlock_cpuirqrs(&spl->osspl, cpuirq);
}

/**
 * @brief XWOSAL API：上锁自旋锁，并关闭部分外部中断
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
void xwosal_splk_lock_irqs(struct xwosal_splk * spl,
                           const struct xwos_irq_resource * irqs,
                           xwsz_t num)
{
        xwosdl_splk_lock_irqs(&spl->osspl, irqs, num);
}

/**
 * @brief XWOSAL API：尝试上锁自旋锁，并关闭部分外部中断
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，立即返回，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_splk_trylock_irqs(struct xwosal_splk * spl,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        return xwosdl_splk_trylock_irqs(&spl->osspl, irqs, num);
}

/**
 * @brief XWOSAL API：解锁自旋锁，并开启部分外部中断
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_splk_unlock_irqs(struct xwosal_splk * spl,
                             const struct xwos_irq_resource * irqs,
                             xwsz_t num)
{
        xwosdl_splk_unlock_irqs(&spl->osspl, irqs, num);
}

/**
 * @brief XWOSAL API：上锁自旋锁，保存部分外部中断的中断标志并关闭
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
void xwosal_splk_lock_irqssv(struct xwosal_splk * spl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwosdl_splk_lock_irqssv(&spl->osspl, irqs, flags, num);
}

/**
 * @brief XWOSAL API：尝试上锁自旋锁，保存部分外部中断的中断标志并关闭
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，立即返回，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_splk_trylock_irqssv(struct xwosal_splk * spl,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        return xwosdl_splk_trylock_irqssv(&spl->osspl, irqs, flags, num);
}

/**
 * @brief XWOSAL API：解锁自旋锁，并恢复部分外部中断的中断标志
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_splk_unlock_irqsrs(struct xwosal_splk * spl,
                               const struct xwos_irq_resource * irqs,
                               xwreg_t flags[], xwsz_t num)
{
        xwosdl_splk_unlock_irqsrs(&spl->osspl, irqs, flags, num);
}

/**
 * @brief XWOSAL API：上锁自旋锁，关闭本地CPU的中断底半部
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程上下文与
 *   中断底半部中是安全的。
 */
static __xwos_inline_api
void xwosal_splk_lock_bh(struct xwosal_splk * spl)
{
        xwosdl_splk_lock_bh(&spl->osspl);
}

/**
 * @brief XWOSAL API：尝试上锁自旋锁，关闭本地CPU的中断底半部
 * @param spl: (I) 自旋锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，立即返回，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程上下文与
 *   中断底半部中是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_splk_trylock_bh(struct xwosal_splk * spl)
{
        return xwosdl_splk_trylock_bh(&spl->osspl);
}

/**
 * @brief XWOSAL API：解锁自旋锁，开启本地CPU的中断底半部
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_splk_unlock_bh(struct xwosal_splk * spl)
{
        xwosdl_splk_unlock_bh(&spl->osspl);
}

#endif /* xwos/osal/lock/spinlock.h */
