/**
 * @file
 * @brief 操作系统抽象层：顺序锁
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_lock_seqlock_h__
#define __xwos_osal_lock_seqlock_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief [XWOSAL] 顺序锁
 */
struct xwosal_sqlk {
        xwosdl_sqlk_t ossql; /**< 操作系统的顺序锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 操作系统抽象层：顺序锁顺序值的粒度
 */
#define XWOSAL_SQLK_GRANULARITY         XWOSDL_SQLK_GRANULARITY

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL API：初始化顺序锁
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_init(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_init(&sql->ossql);
}

/**
 * @brief XWOSAL API：开启读临界区
 * @param sql: (I) 顺序锁的指针
 * @return 当前顺序值
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 若顺序锁已处于写临界区，就自旋等待。
 */
static __xwos_inline_api
xwsq_t xwosal_sqlk_rd_begin(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_rd_begin(&sql->ossql);
}

/**
 * @brief XWOSAL API：结束读临界区
 * @param sql: (I) 顺序锁的指针
 * @param start: (I) @ref xwosal_sqlk_rd_begin() 返回的顺序值，
 *                   用于测试顺序值是否发生改变
 * @retval true: 顺序值发生了改变，需要重试
 * @retval false: 顺序值未发生改变
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
bool xwosal_sqlk_rd_retry(struct xwosal_sqlk * sql, xwsq_t start)
{
        return xwosdl_sqlk_rd_retry(&sql->ossql, start);
}

/**
 * @brief XWOSAL API：获取顺序锁的顺序值
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwosal_sqlk_get_seq(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_get_seq(&sql->ossql);
}

/**
 * @brief XWOSAL API：开启独占读临界区
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_lock(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_rdex_lock(&sql->ossql);
}

/**
 * @brief XWOSAL API：尝试开启独占读临界区
 * @param sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_rdex_trylock(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_rdex_trylock(&sql->ossql);
}

/**
 * @brief XWOSAL API：关闭独占读临界区
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_unlock(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_rdex_unlock(&sql->ossql);
}

/**
 * @brief XWOSAL API：开启独占读临界区，并关闭本地CPU的中断
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_lock_cpuirq(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_rdex_lock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOSAL API：尝试开启独占读临界区，并关闭本地CPU的中断
 * @param sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_rdex_trylock_cpuirq(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_rdex_trylock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOSAL API：关闭独占读临界区，并开启本地CPU的中断
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_unlock_cpuirq(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_rdex_unlock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOSAL API：开启独占读临界区，保存本地CPU的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_lock_cpuirqsv(struct xwosal_sqlk * sql,
                                    xwreg_t * cpuirq)
{
        xwosdl_sqlk_rdex_lock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOSAL API：尝试开启独占读临界区，保存本地CPU的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_rdex_trylock_cpuirqsv(struct xwosal_sqlk * sql,
                                         xwreg_t * cpuirq)
{
        return xwosdl_sqlk_rdex_trylock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOSAL API：关闭独占读临界区，恢复本地CPU的中断标志
 * @param sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_unlock_cpuirqrs(struct xwosal_sqlk * sql,
                                      xwreg_t cpuirq)
{
        xwosdl_sqlk_rdex_unlock_cpuirqrs(&sql->ossql, cpuirq);
}

/**
 * @brief XWOSAL API：开启独占读临界区，并关闭部分外部中断
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_lock_irqs(struct xwosal_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwosdl_sqlk_rdex_lock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOSAL API：尝试开启独占读临界区，并关闭部分外部中断
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_rdex_trylock_irqs(struct xwosal_sqlk * sql,
                                     const struct xwos_irq_resource * irqs,
                                     xwsz_t num)
{
        return xwosdl_sqlk_rdex_trylock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOSAL API：关闭独占读临界区，并开启部分外部中断
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_unlock_irqs(struct xwosal_sqlk * sql,
                                  const struct xwos_irq_resource * irqs,
                                  xwsz_t num)
{
        xwosdl_sqlk_rdex_unlock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOSAL API：开启独占读临界区，保存部分外部中断的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_lock_irqssv(struct xwosal_sqlk * sql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_rdex_lock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOSAL API：尝试开启独占读临界区，保存部分外部中断的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_rdex_trylock_irqssv(struct xwosal_sqlk * sql,
                                       const struct xwos_irq_resource * irqs,
                                       xwreg_t flags[], xwsz_t num)
{
        return xwosdl_sqlk_rdex_trylock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOSAL API：关闭独占读临界区，恢复部分外部中断的中断标志
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_unlock_irqsrs(struct xwosal_sqlk * sql,
                                    const struct xwos_irq_resource * irqs,
                                    xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_rdex_unlock_irqsrs(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOSAL API：开启独占读临界区，关闭本地CPU的中断底半部
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_lock_bh(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_rdex_lock_bh(&sql->ossql);
}

/**
 * @brief XWOSAL API：尝试开启独占读临界区，关闭本地CPU的中断底半部
 * @param sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_rdex_trylock_bh(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_rdex_trylock_bh(&sql->ossql);
}

/**
 * @brief XWOSAL API：关闭独占读临界区，开启本地CPU的中断底半部
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_rdex_unlock_bh(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_rdex_unlock_bh(&sql->ossql);
}

/**
 * @brief XWOSAL API：开启写临界区
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 */
static __xwos_inline_api
void xwosal_sqlk_wr_lock(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_wr_lock(&sql->ossql);
}

/**
 * @brief XWOSAL API：尝试开启写临界区
 * @param sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_wr_trylock(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_wr_trylock(&sql->ossql);
}

/**
 * @brief XWOSAL API：关闭写临界区
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_wr_unlock(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_wr_unlock(&sql->ossql);
}

/**
 * @brief XWOSAL API：开启写临界区，并关闭本地CPU的中断
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_wr_lock_cpuirq(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_wr_lock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOSAL API：尝试开启写临界区，并关闭本地CPU的中断
 * @param sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_wr_trylock_cpuirq(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_wr_trylock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOSAL API：关闭写临界区，并开启本地CPU的中断
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_wr_unlock_cpuirq(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_wr_unlock_cpuirq(&sql->ossql);
}

/**
 * @brief XWOSAL API：开启写临界区，保存本地CPU的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_wr_lock_cpuirqsv(struct xwosal_sqlk * sql,
                                  xwreg_t * cpuirq)
{
        xwosdl_sqlk_wr_lock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOSAL API：尝试开启写临界区，保存本地CPU的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文都是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_wr_trylock_cpuirqsv(struct xwosal_sqlk * sql,
                                       xwreg_t * cpuirq)
{
        return xwosdl_sqlk_wr_trylock_cpuirqsv(&sql->ossql, cpuirq);
}

/**
 * @brief XWOSAL API：关闭写临界区，恢复本地CPU的中断标志
 * @param sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_wr_unlock_cpuirqrs(struct xwosal_sqlk * sql,
                                    xwreg_t cpuirq)
{
        xwosdl_sqlk_wr_unlock_cpuirqrs(&sql->ossql, cpuirq);
}

/**
 * @brief XWOSAL API：开启写临界区，并关闭部分外部中断
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文中是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_wr_lock_irqs(struct xwosal_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwosdl_sqlk_wr_lock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOSAL API：尝试开启写临界区，并关闭部分外部中断
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_wr_trylock_irqs(struct xwosal_sqlk * sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num)
{
        return xwosdl_sqlk_wr_trylock_irqs(&sql->ossql, irqs, num);
}

/**
 * @brief XWOSAL API：关闭写临界区，并开启部分外部中断
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_wr_unlock_irqs(struct xwosal_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwosdl_sqlk_wr_unlock_irqs(&sql->ossql, irqs, num);
}


/**
 * @brief XWOSAL API：开启写临界区，保存部分外部中断的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_wr_lock_irqssv(struct xwosal_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_wr_lock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOSAL API：尝试开启写临界区，保存部分外部中断的中断标志并关闭
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_wr_trylock_irqssv(struct xwosal_sqlk * sql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        return xwosdl_sqlk_wr_trylock_irqssv(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOSAL API：关闭写临界区，恢复部分外部中断的中断标志
 * @param sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwosal_sqlk_wr_unlock_irqsrs(struct xwosal_sqlk * sql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwosdl_sqlk_wr_unlock_irqsrs(&sql->ossql, irqs, flags, num);
}

/**
 * @brief XWOSAL API：开启写临界区，关闭本地CPU的中断底半部
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwos_inline_api
void xwosal_sqlk_wr_lock_bh(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_wr_lock_bh(&sql->ossql);
}

/**
 * @brief XWOSAL API：尝试开启写临界区，关闭本地CPU的中断底半部
 * @param sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwos_inline_api
xwer_t xwosal_sqlk_wr_trylock_bh(struct xwosal_sqlk * sql)
{
        return xwosdl_sqlk_wr_trylock_bh(&sql->ossql);
}

/**
 * @brief XWOSAL API：关闭写临界区，开启本地CPU的中断底半部
 * @param sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已处于写或独占读临界区，就自旋等待。
 */
static __xwos_inline_api
void xwosal_sqlk_wr_unlock_bh(struct xwosal_sqlk * sql)
{
        xwosdl_sqlk_wr_unlock_bh(&sql->ossql);
}

#endif /* xwos/osal/lock/seqlock.h */
