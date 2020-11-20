/**
 * @file
 * @brief 玄武OS MP内核锁机制：顺序锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_lock_seqlock_h__
#define __xwos_mp_lock_seqlock_h__

#include <xwos/standard.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/irq.h>

/**
 * @brief 序锁顺序值的粒度
 */
#define XWMP_SQLK_GRANULARITY           (1U)

/**
 * @brief 顺序锁
 */
struct xwmp_sqlk {
        struct xwmp_splk splk; /**< 自旋锁 */
        xwsq_t seq; /**< 顺序值 */
};

/**
 * @brief XWMP API：初始化顺序锁
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_init(struct xwmp_sqlk * sql)
{
        sql->seq = 0;
        xwmp_splk_init(&sql->splk);
}

/**
 * @brief XWMP API：开启读临界区
 * @parem sql: (I) 顺序锁的指针
 * @return 当前顺序值
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 若顺序锁已处于写临界区，就自旋等待。
 */
static __xwmp_inline_api
xwsq_t xwmp_sqlk_rd_begin(struct xwmp_sqlk * sql)
{
        xwsq_t ret;

        do {
                ret = xwmb_access(xwsq_t, sql->seq);
#if (XWMP_SQLK_GRANULARITY > 1)
                ret &= ((~XWMP_SQLK_GRANULARITY) + 1U);
#endif
        } while (__xwcc_unlikely(ret & XWMP_SQLK_GRANULARITY));
        xwmb_mp_rmb();
        return ret;
}

/**
 * @brief XWMP API：结束非独占读访问
 * @parem sql: (I) 顺序锁的指针
 * @param start: (I) @ref xwmp_sqlk_rd_begin() 返回的顺序值，
 *                   用于测试顺序值是否发生改变（是否发生写操作）
 * @return 错误码
 * @retval true: 顺序值发生了改变，读到的值无效，需要重新开启非独占读访问
 * @retval false: 顺序值未发生改变
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwmp_inline_api
bool xwmp_sqlk_rd_retry(struct xwmp_sqlk * sql, xwsq_t start)
{
        xwmb_mp_rmb(); /* ensure all read accesses are finished */
#if (XWMP_SQLK_GRANULARITY > 1)
        return ((sql->seq & ((~XWMP_SQLK_GRANULARITY) + 1U)) != start);
#else
        return (sql->seq != start);
#endif
}

/**
 * @brief XWMP API：得到顺序锁的顺序值
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwmp_inline_api
xwsq_t xwmp_sqlk_get_seq(struct xwmp_sqlk * sql)
{
        xwsq_t ret;

        ret = xwmb_access(xwsq_t, sql->seq);
        xwmb_mp_rmb();
#if (XWMP_SQLK_GRANULARITY > 1)
        return ret & ((~XWMP_SQLK_GRANULARITY) + 1U);
#else
        return ret;
#endif
}

/**
 * @brief XWMP API：开启独占读临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_lock(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock(&sql->splk);
}

/**
 * @brief XWMP API：尝试开启独占读临界区
 * @parem sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock(struct xwmp_sqlk * sql)
{
        return xwmp_splk_trylock(&sql->splk);
}

/**
 * @brief XWMP API：关闭独占读临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock(struct xwmp_sqlk * sql)
{
        xwmp_splk_unlock(&sql->splk);
}

/**
 * @brief XWMP API：开启独占读临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_cpuirq(&sql->splk);
}

/**
 * @brief XWMP API：尝试开启独占读临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_cpuirq(struct xwmp_sqlk * sql)
{
        return xwmp_splk_trylock_cpuirq(&sql->splk);
}

/**
 * @brief XWMP API：关闭独占读临界区，并开启本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmp_splk_unlock_cpuirq(&sql->splk);
}

/**
 * @brief XWMP API：开启独占读临界区，保存本地CPU的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(&sql->splk, cpuirq);
}

/**
 * @brief XWMP API：尝试开启独占读临界区，保存本地CPU的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        return xwmp_splk_trylock_cpuirqsv(&sql->splk, cpuirq);
}

/**
 * @brief XWMP API：关闭独占读临界区，恢复本地CPU的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_cpuirqrs(struct xwmp_sqlk * sql, xwreg_t cpuirq)
{
        xwmp_splk_unlock_cpuirqrs(&sql->splk, cpuirq);
}

/**
 * @brief XWMP API：开启独占读临界区，并关闭部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_irqs(struct xwmp_sqlk * sql,
                              const struct xwmp_irq_resource * irqs,
                              xwsz_t num)
{
        xwmp_splk_lock_irqs(&sql->splk, irqs, num);
}

/**
 * @brief XWMP API：尝试开启独占读临界区，并关闭部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_irqs(struct xwmp_sqlk * sql,
                                   const struct xwmp_irq_resource * irqs,
                                   xwsz_t num)
{
        return xwmp_splk_trylock_irqs(&sql->splk, irqs, num);
}

/**
 * @brief XWMP API：关闭独占读临界区，并开启部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_irqs(struct xwmp_sqlk * sql,
                                const struct xwmp_irq_resource * irqs,
                                xwsz_t num)
{
        xwmp_splk_unlock_irqs(&sql->splk, irqs, num);
}

/**
 * @brief XWMP API：开启独占读临界区，保存部分外部中断的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_irqssv(struct xwmp_sqlk * sql,
                                const struct xwmp_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_lock_irqssv(&sql->splk, irqs, flags, num);
}

/**
 * @brief XWMP API：尝试开启独占读临界区，保存部分外部中断的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_irqssv(struct xwmp_sqlk * sql,
                                     const struct xwmp_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        return xwmp_splk_trylock_irqssv(&sql->splk, irqs, flags, num);
}

/**
 * @brief XWMP API：关闭独占读临界区，恢复部分外部中断的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_irqsrs(struct xwmp_sqlk * sql,
                                  const struct xwmp_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_unlock_irqsrs(&sql->splk, irqs, flags, num);
}

/**
 * @brief XWMP API：开启独占读临界区，关闭本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_lock_bh(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_bh(&sql->splk);
}

/**
 * @brief XWMP API：尝试开启独占读临界区，关闭本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_rdex_trylock_bh(struct xwmp_sqlk * sql)
{
        return xwmp_splk_trylock_bh(&sql->splk);
}

/**
 * @brief XWMP API：关闭独占读临界区，开启本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_rdex_unlock_bh(struct xwmp_sqlk * sql)
{
        xwmp_splk_unlock_bh(&sql->splk);
}

/**
 * @brief XWMP API：开启写临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_lock(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock(&sql->splk);
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWMP API：尝试开启写临界区
 * @parem sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock(struct xwmp_sqlk * sql)
{
        xwer_t rc;

        rc = xwmp_splk_trylock(&sql->splk);
        if (XWOK == rc) {
                sql->seq += XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

/**
 * @brief XWMP API：关闭写临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_unlock(struct xwmp_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock(&sql->splk);
}

/**
 * @brief XWMP API：开启写临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_lock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_cpuirq(&sql->splk);
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWMP API：尝试开启写临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_cpuirq(struct xwmp_sqlk * sql)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_cpuirq(&sql->splk);
        if (XWOK == rc) {
                sql->seq += XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

/**
 * @brief XWMP API：关闭写临界区，并开启本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_cpuirq(struct xwmp_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_cpuirq(&sql->splk);
}

/**
 * @brief XWMP API：开启写临界区，保存本地CPU的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_lock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(&sql->splk, cpuirq);
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWMP API：尝试开启写临界区，保存本地CPU的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_cpuirqsv(struct xwmp_sqlk * sql, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_cpuirqsv(&sql->splk, cpuirq);
        if (XWOK == rc) {
                sql->seq += XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

/**
 * @brief XWMP API：关闭写临界区，恢复本地CPU的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_cpuirqrs(struct xwmp_sqlk * sql, xwreg_t cpuirq)
{
        xwmb_mp_wmb();
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_cpuirqrs(&sql->splk, cpuirq);
}

/**
 * @brief XWMP API：开启写临界区，并关闭部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_lock_irqs(struct xwmp_sqlk * sql,
                            const struct xwmp_irq_resource * irqs,
                            xwsz_t num)
{
        xwmp_splk_lock_irqs(&sql->splk, irqs, num);
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWMP API：尝试开启写临界区，并关闭部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_irqs(struct xwmp_sqlk * sql,
                                 const struct xwmp_irq_resource * irqs,
                                 xwsz_t num)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_irqs(&sql->splk, irqs, num);
        if (XWOK == rc) {
                sql->seq += XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

/**
 * @brief XWMP API：关闭写临界区，并开启部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_irqs(struct xwmp_sqlk * sql,
                              const struct xwmp_irq_resource * irqs,
                              xwsz_t num)
{
        xwmb_mp_wmb();
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_irqs(&sql->splk, irqs, num);
}

/**
 * @brief XWMP API：开启写临界区，保存部分外部中断的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_lock_irqssv(struct xwmp_sqlk * sql,
                              const struct xwmp_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_lock_irqssv(&sql->splk, irqs, flags, num);
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWMP API：尝试开启写临界区，保存部分外部中断的中断标志并关闭
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_irqssv(struct xwmp_sqlk * sql,
                                   const struct xwmp_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_irqssv(&sql->splk, irqs, flags, num);
        if (XWOK == rc) {
                sql->seq += XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

/**
 * @brief XWMP API：关闭写临界区，恢复部分外部中断的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_irqsrs(struct xwmp_sqlk * sql,
                                const struct xwmp_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwmb_mp_wmb();
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_irqsrs(&sql->splk, irqs, flags, num);
}

/**
 * @brief XWMP API：开启写临界区，关闭本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_lock_bh(struct xwmp_sqlk * sql)
{
        xwmp_splk_lock_bh(&sql->splk);
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWMP API：尝试开启写临界区，关闭本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EAGAIN: 获得锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，不会自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程与
 *   中断底半部中是安全的。
 */
static __xwmp_inline_api
xwer_t xwmp_sqlk_wr_trylock_bh(struct xwmp_sqlk * sql)
{
        xwer_t rc;

        rc = xwmp_splk_trylock_bh(&sql->splk);
        if (XWOK == rc) {
                sql->seq += XWMP_SQLK_GRANULARITY;
                xwmb_mp_wmb();
        }
        return rc;
}

/**
 * @brief XWMP API：关闭写临界区，开启本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmp_inline_api
void xwmp_sqlk_wr_unlock_bh(struct xwmp_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWMP_SQLK_GRANULARITY;
        xwmp_splk_unlock_bh(&sql->splk);
}

#endif /* xwos/mp/lock/seqlock.h */
