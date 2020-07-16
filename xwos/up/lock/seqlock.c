/**
 * @file
 * @brief XuanWuOS的锁机制：顺序锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/up/irq.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/lock/seqlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ******** inline functions  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：初始化顺序锁
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_init(struct xwlk_sqlk * sql)
{
        sql->seq = 0;
}

/**
 * @brief XWOS API：开启读临界区
 * @parem sql: (I) 顺序锁的指针
 * @return 当前顺序值
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 若顺序锁已开启写临界区，就自旋等待。
 */
__xwos_api
xwsq_t xwlk_sqlk_rd_begin(struct xwlk_sqlk * sql)
{
        xwsq_t ret;

retry:
        xwmb_read(xwsq_t, ret, &sql->seq);
#if (XWLK_SQLK_GRANULARITY > 1)
        ret &= ((~XWLK_SQLK_GRANULARITY) + 1U);
#endif
        if (__unlikely(ret & XWLK_SQLK_GRANULARITY)) {
                goto retry;
        }
        xwmb_smp_rmb();
        return ret;
}

/**
 * @brief XWOS API：结束非独占读访问
 * @parem sql: (I) 顺序锁的指针
 * @param start: (I) @ref xwos_sqlk_rd_begin() 返回的顺序值，
 *                   用于测试顺序值是否发生改变（是否发生写操作）
 * @return 错误码
 * @retval true: 顺序值发生了改变，读到的值无效，需要重新开启非独占读访问
 * @retval false: 顺序值未发生改变
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
bool xwlk_sqlk_rd_retry(struct xwlk_sqlk * sql, xwsq_t start)
{
        xwmb_smp_rmb(); /* ensure all read accesses are finished */
#if (XWLK_SQLK_GRANULARITY > 1)
        return ((sql->seq & ((~XWLK_SQLK_GRANULARITY) + 1U)) != start);
#else
        return (sql->seq != start);
#endif
}

/**
 * @brief XWOS API：得到顺序锁的顺序值
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwsq_t xwlk_sqlk_get_seq(struct xwlk_sqlk * sql)
{
        xwsq_t ret;

        xwmb_read(xwsq_t, ret, &sql->seq);
        xwmb_smp_rmb();
#if (XWLK_SQLK_GRANULARITY > 1)
        return ret & ((~XWLK_SQLK_GRANULARITY) + 1U);
#else
        return ret;
#endif
}

/**
 * @brief XWOS API：开启独占读临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
__xwos_api
void xwlk_sqlk_rdex_lock(struct xwlk_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwos_scheduler_dspmpt_lc();
}

/**
 * @brief XWOS API：尝试开启独占读临界区
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
__xwos_api
xwer_t xwlk_sqlk_rdex_trylock(struct xwlk_sqlk * sql)
{
        xwlk_sqlk_rdex_lock(sql);
        return XWOK;
}

/**
 * @brief XWOS API：关闭独占读临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_rdex_unlock(struct xwlk_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwos_scheduler_enpmpt_lc();
}

/**
 * @brief XWOS API：开启独占读临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
__xwos_api
void xwlk_sqlk_rdex_lock_cpuirq(struct xwlk_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwos_cpuirq_disable_lc();
        xwos_scheduler_dspmpt_lc();
}

/**
 * @brief XWOS API：尝试开启独占读临界区，并关闭本地CPU的中断
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
__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_cpuirq(struct xwlk_sqlk * sql)
{
        xwlk_sqlk_rdex_lock_cpuirq(sql);
        return XWOK;
}

/**
 * @brief XWOS API：关闭独占读临界区，并开启本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_rdex_unlock_cpuirq(struct xwlk_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_enable_lc();
}

/**
 * @brief XWOS API：开启独占读临界区，保存本地CPU的中断标志并关闭
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
__xwos_api
void xwlk_sqlk_rdex_lock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag)
{
        XWOS_UNUSED(sql);
        xwos_cpuirq_save_lc(flag);
        xwos_scheduler_dspmpt_lc();
}

/**
 * @brief XWOS API：尝试开启独占读临界区，保存本地CPU的中断标志并关闭
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
__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag)
{
        xwlk_sqlk_rdex_lock_cpuirqsv(sql, flag);
        return XWOK;
}

/**
 * @brief XWOS API：关闭独占读临界区，恢复本地CPU的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_rdex_unlock_cpuirqrs(struct xwlk_sqlk * sql, xwreg_t flag)
{
        XWOS_UNUSED(sql);
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_restore_lc(flag);
}

/**
 * @brief XWOS API：开启独占读临界区，并关闭部分外部中断
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
__xwos_api
void xwlk_sqlk_rdex_lock_irqs(struct xwlk_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
        xwos_scheduler_dspmpt_lc();
}

/**
 * @brief XWOS API：尝试开启独占读临界区，并关闭部分外部中断
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
__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_irqs(struct xwlk_sqlk * sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num)
{
        xwlk_sqlk_rdex_lock_irqs(sql, irqs, num);
        return XWOK;
}

/**
 * @brief XWOS API：关闭独占读临界区，并开启部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_rdex_unlock_irqs(struct xwlk_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_enable(irqs[i].irqn);
        }
}

/**
 * @brief XWOS API：开启独占读临界区，保存部分外部中断的中断标志并关闭
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
__xwos_api
void xwlk_sqlk_rdex_lock_irqssv(struct xwlk_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwos_scheduler_dspmpt_lc();
}

/**
 * @brief XWOS API：尝试开启独占读临界区，保存部分外部中断的中断标志并关闭
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
__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_irqssv(struct xwlk_sqlk * sql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_rdex_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

/**
 * @brief XWOS API：关闭独占读临界区，恢复部分外部中断的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_rdex_unlock_irqsrs(struct xwlk_sqlk * sql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
/**
 * @brief XWOS API：开启独占读临界区，关闭本地CPU的中断底半部
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
__xwos_api
void xwlk_sqlk_rdex_lock_bh(struct xwlk_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwos_scheduler_dspmpt_lc();
        xwos_scheduler_dsbh_lc();
}

/**
 * @brief XWOS API：尝试开启独占读临界区，关闭本地CPU的中断底半部
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
__xwos_api
xwer_t xwlk_sqlk_rdex_trylock_bh(struct xwlk_sqlk * sql)
{
        xwlk_sqlk_rdex_lock_bh(sql);
        return XWOK;
}

/**
 * @brief XWOS API：关闭独占读临界区，开启本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_rdex_unlock_bh(struct xwlk_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwos_scheduler_enbh_lc();
        xwos_scheduler_enpmpt_lc();
}
#endif /* XWUPCFG_SD_BH */

/**
 * @brief XWOS API：开启写临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 */
__xwos_api
void xwlk_sqlk_wr_lock(struct xwlk_sqlk * sql)
{
        xwos_scheduler_dspmpt_lc();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwmb_smp_wmb();
}

/**
 * @brief XWOS API：尝试开启写临界区
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
__xwos_api
xwer_t xwlk_sqlk_wr_trylock(struct xwlk_sqlk * sql)
{
        xwlk_sqlk_wr_lock(sql);
        return XWOK;
}

/**
 * @brief XWOS API：关闭写临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_wr_unlock(struct xwlk_sqlk * sql)
{
        xwmb_smp_wmb();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwos_scheduler_enpmpt_lc();
}

/**
 * @brief XWOS API：开启写临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
__xwos_api
void xwlk_sqlk_wr_lock_cpuirq(struct xwlk_sqlk * sql)
{
        xwos_cpuirq_disable_lc();
        xwos_scheduler_dspmpt_lc();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwmb_smp_wmb();
}

/**
 * @brief XWOS API：尝试开启写临界区，并关闭本地CPU的中断
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
__xwos_api
xwer_t xwlk_sqlk_wr_trylock_cpuirq(struct xwlk_sqlk * sql)
{
        xwlk_sqlk_wr_lock_cpuirq(sql);
        return XWOK;
}

/**
 * @brief XWOS API：关闭写临界区，并开启本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_wr_unlock_cpuirq(struct xwlk_sqlk * sql)
{
        xwmb_smp_wmb();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_enable_lc();
}

/**
 * @brief XWOS API：开启写临界区，保存本地CPU的中断标志并关闭
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
__xwos_api
void xwlk_sqlk_wr_lock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag)
{
        xwos_cpuirq_save_lc(flag);
        xwos_scheduler_dspmpt_lc();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwmb_smp_wmb();
}

/**
 * @brief XWOS API：尝试开启写临界区，保存本地CPU的中断标志并关闭
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
__xwos_api
xwer_t xwlk_sqlk_wr_trylock_cpuirqsv(struct xwlk_sqlk * sql, xwreg_t * flag)
{
        xwlk_sqlk_wr_lock_cpuirqsv(sql, flag);
        return XWOK;
}

/**
 * @brief XWOS API：关闭写临界区，恢复本地CPU的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_wr_unlock_cpuirqrs(struct xwlk_sqlk * sql, xwreg_t flag)
{
        xwmb_smp_wmb();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_restore_lc(flag);
}

/**
 * @brief XWOS API：开启写临界区，并关闭部分外部中断
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
__xwos_api
void xwlk_sqlk_wr_lock_irqs(struct xwlk_sqlk * sql,
                            const struct xwos_irq_resource * irqs,
                            xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
        xwos_scheduler_dspmpt_lc();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwmb_smp_wmb();
}

/**
 * @brief XWOS API：尝试开启写临界区，并关闭部分外部中断
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
__xwos_api
xwer_t xwlk_sqlk_wr_trylock_irqs(struct xwlk_sqlk * sql,
                                 const struct xwos_irq_resource * irqs,
                                 xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqs(sql, irqs, num);
        return XWOK;
}

/**
 * @brief XWOS API：关闭写临界区，并开启部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_wr_unlock_irqs(struct xwlk_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwssz_t i;

        xwmb_smp_wmb();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_enable(irqs[i].irqn);
        }
}


/**
 * @brief XWOS API：开启写临界区，保存部分外部中断的中断标志并关闭
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
 *   与线程中是安全的
 */
__xwos_api
void xwlk_sqlk_wr_lock_irqssv(struct xwlk_sqlk * sql,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwos_scheduler_dspmpt_lc();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwmb_smp_wmb();
}

/**
 * @brief XWOS API：尝试开启写临界区，保存部分外部中断的中断标志并关闭
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
__xwos_api
xwer_t xwlk_sqlk_wr_trylock_irqssv(struct xwlk_sqlk * sql,
                                   const struct xwos_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

/**
 * @brief XWOS API：关闭写临界区，恢复部分外部中断的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_wr_unlock_irqsrs(struct xwlk_sqlk * sql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        xwmb_smp_wmb();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
/**
 * @brief XWOS API：开启写临界区，关闭本地CPU的中断底半部
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
__xwos_api
void xwlk_sqlk_wr_lock_bh(struct xwlk_sqlk * sql)
{
        xwos_scheduler_dspmpt_lc();
        xwos_scheduler_dsbh_lc();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwmb_smp_wmb();
}

/**
 * @brief XWOS API：尝试开启写临界区，关闭本地CPU的中断底半部
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
 *   中断底半部中是安全的
 */
__xwos_api
xwer_t xwlk_sqlk_wr_trylock_bh(struct xwlk_sqlk * sql)
{
        xwlk_sqlk_wr_lock_bh(sql);
        return XWOK;
}

/**
 * @brief XWOS API：关闭写临界区，开启本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_sqlk_wr_unlock_bh(struct xwlk_sqlk * sql)
{
        xwmb_smp_wmb();
        sql->seq += XWLK_SQLK_GRANULARITY;
        xwos_scheduler_enbh_lc();
        xwos_scheduler_enpmpt_lc();
}
#endif /* XWUPCFG_SD_BH */
