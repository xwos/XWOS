/**
 * @file
 * @brief 玄武OS UP内核锁机制：顺序锁
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
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
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
 * @brief XWUP API：初始化顺序锁
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_init(struct xwup_sqlk * sql)
{
        sql->seq = 0;
}

/**
 * @brief XWUP API：开启读临界区
 * @parem sql: (I) 顺序锁的指针
 * @return 当前顺序值
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 若顺序锁已开启写临界区，就自旋等待。
 */
__xwup_api
xwsq_t xwup_sqlk_rd_begin(struct xwup_sqlk * sql)
{
        xwsq_t ret;

retry:
        xwmb_read(xwsq_t, ret, &sql->seq);
#if (XWUP_SQLK_GRANULARITY > 1)
        ret &= ((~XWUP_SQLK_GRANULARITY) + 1U);
#endif
        if (__xwcc_unlikely(ret & XWUP_SQLK_GRANULARITY)) {
                goto retry;
        }
        xwmb_mp_rmb();
        return ret;
}

/**
 * @brief XWUP API：结束非独占读访问
 * @parem sql: (I) 顺序锁的指针
 * @param start: (I) @ref xwup_sqlk_rd_begin() 返回的顺序值，
 *                   用于测试顺序值是否发生改变（是否发生写操作）
 * @return 错误码
 * @retval true: 顺序值发生了改变，读到的值无效，需要重新开启非独占读访问
 * @retval false: 顺序值未发生改变
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
bool xwup_sqlk_rd_retry(struct xwup_sqlk * sql, xwsq_t start)
{
        xwmb_mp_rmb(); /* ensure all read accesses are finished */
#if (XWUP_SQLK_GRANULARITY > 1)
        return ((sql->seq & ((~XWUP_SQLK_GRANULARITY) + 1U)) != start);
#else
        return (sql->seq != start);
#endif
}

/**
 * @brief XWUP API：得到顺序锁的顺序值
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwsq_t xwup_sqlk_get_seq(struct xwup_sqlk * sql)
{
        xwsq_t ret;

        xwmb_read(xwsq_t, ret, &sql->seq);
        xwmb_mp_rmb();
#if (XWUP_SQLK_GRANULARITY > 1)
        return ret & ((~XWUP_SQLK_GRANULARITY) + 1U);
#else
        return ret;
#endif
}

/**
 * @brief XWUP API：开启独占读临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
__xwup_api
void xwup_sqlk_rdex_lock(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_dspmpt_lc();
}

/**
 * @brief XWUP API：尝试开启独占读临界区
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
__xwup_api
xwer_t xwup_sqlk_rdex_trylock(struct xwup_sqlk * sql)
{
        xwup_sqlk_rdex_lock(sql);
        return XWOK;
}

/**
 * @brief XWUP API：关闭独占读临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_rdex_unlock(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
}

/**
 * @brief XWUP API：开启独占读临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
__xwup_api
void xwup_sqlk_rdex_lock_cpuirq(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwospl_cpuirq_disable_lc();
        xwup_skd_dspmpt_lc();
}

/**
 * @brief XWUP API：尝试开启独占读临界区，并关闭本地CPU的中断
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
__xwup_api
xwer_t xwup_sqlk_rdex_trylock_cpuirq(struct xwup_sqlk * sql)
{
        xwup_sqlk_rdex_lock_cpuirq(sql);
        return XWOK;
}

/**
 * @brief XWUP API：关闭独占读临界区，并开启本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_rdex_unlock_cpuirq(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_enable_lc();
}

/**
 * @brief XWUP API：开启独占读临界区，保存本地CPU的中断标志并关闭
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
__xwup_api
void xwup_sqlk_rdex_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        XWOS_UNUSED(sql);
        xwospl_cpuirq_save_lc(flag);
        xwup_skd_dspmpt_lc();
}

/**
 * @brief XWUP API：尝试开启独占读临界区，保存本地CPU的中断标志并关闭
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
__xwup_api
xwer_t xwup_sqlk_rdex_trylock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        xwup_sqlk_rdex_lock_cpuirqsv(sql, flag);
        return XWOK;
}

/**
 * @brief XWUP API：关闭独占读临界区，恢复本地CPU的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_rdex_unlock_cpuirqrs(struct xwup_sqlk * sql, xwreg_t flag)
{
        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_restore_lc(flag);
}

/**
 * @brief XWUP API：开启独占读临界区，并关闭部分外部中断
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
__xwup_api
void xwup_sqlk_rdex_lock_irqs(struct xwup_sqlk * sql,
                              const struct xwup_irq_resource * irqs,
                              xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_disable(irqs[i].irqn);
        }
        xwup_skd_dspmpt_lc();
}

/**
 * @brief XWUP API：尝试开启独占读临界区，并关闭部分外部中断
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
__xwup_api
xwer_t xwup_sqlk_rdex_trylock_irqs(struct xwup_sqlk * sql,
                                   const struct xwup_irq_resource * irqs,
                                   xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqs(sql, irqs, num);
        return XWOK;
}

/**
 * @brief XWUP API：关闭独占读临界区，并开启部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_rdex_unlock_irqs(struct xwup_sqlk * sql,
                                const struct xwup_irq_resource * irqs,
                                xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_enable(irqs[i].irqn);
        }
}

/**
 * @brief XWUP API：开启独占读临界区，保存部分外部中断的中断标志并关闭
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
__xwup_api
void xwup_sqlk_rdex_lock_irqssv(struct xwup_sqlk * sql,
                                const struct xwup_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwup_skd_dspmpt_lc();
}

/**
 * @brief XWUP API：尝试开启独占读临界区，保存部分外部中断的中断标志并关闭
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
__xwup_api
xwer_t xwup_sqlk_rdex_trylock_irqssv(struct xwup_sqlk * sql,
                                     const struct xwup_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

/**
 * @brief XWUP API：关闭独占读临界区，恢复部分外部中断的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_rdex_unlock_irqsrs(struct xwup_sqlk * sql,
                                  const struct xwup_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        XWOS_UNUSED(sql);
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
/**
 * @brief XWUP API：开启独占读临界区，关闭本地CPU的中断底半部
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
__xwup_api
void xwup_sqlk_rdex_lock_bh(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_dspmpt_lc();
        xwup_skd_dsbh_lc();
}

/**
 * @brief XWUP API：尝试开启独占读临界区，关闭本地CPU的中断底半部
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
__xwup_api
xwer_t xwup_sqlk_rdex_trylock_bh(struct xwup_sqlk * sql)
{
        xwup_sqlk_rdex_lock_bh(sql);
        return XWOK;
}

/**
 * @brief XWUP API：关闭独占读临界区，开启本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_rdex_unlock_bh(struct xwup_sqlk * sql)
{
        XWOS_UNUSED(sql);
        xwup_skd_enbh_lc();
        xwup_skd_enpmpt_lc();
}
#endif /* XWUPCFG_SKD_BH */

/**
 * @brief XWUP API：开启写临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 */
__xwup_api
void xwup_sqlk_wr_lock(struct xwup_sqlk * sql)
{
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWUP API：尝试开启写临界区
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
__xwup_api
xwer_t xwup_sqlk_wr_trylock(struct xwup_sqlk * sql)
{
        xwup_sqlk_wr_lock(sql);
        return XWOK;
}

/**
 * @brief XWUP API：关闭写临界区
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_wr_unlock(struct xwup_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
}

/**
 * @brief XWUP API：开启写临界区，并关闭本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若顺序锁已开启写或独占读临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在线程与中断中是安全的。
 */
__xwup_api
void xwup_sqlk_wr_lock_cpuirq(struct xwup_sqlk * sql)
{
        xwospl_cpuirq_disable_lc();
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWUP API：尝试开启写临界区，并关闭本地CPU的中断
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
__xwup_api
xwer_t xwup_sqlk_wr_trylock_cpuirq(struct xwup_sqlk * sql)
{
        xwup_sqlk_wr_lock_cpuirq(sql);
        return XWOK;
}

/**
 * @brief XWUP API：关闭写临界区，并开启本地CPU的中断
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_wr_unlock_cpuirq(struct xwup_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_enable_lc();
}

/**
 * @brief XWUP API：开启写临界区，保存本地CPU的中断标志并关闭
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
__xwup_api
void xwup_sqlk_wr_lock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        xwospl_cpuirq_save_lc(flag);
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWUP API：尝试开启写临界区，保存本地CPU的中断标志并关闭
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
__xwup_api
xwer_t xwup_sqlk_wr_trylock_cpuirqsv(struct xwup_sqlk * sql, xwreg_t * flag)
{
        xwup_sqlk_wr_lock_cpuirqsv(sql, flag);
        return XWOK;
}

/**
 * @brief XWUP API：关闭写临界区，恢复本地CPU的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_wr_unlock_cpuirqrs(struct xwup_sqlk * sql, xwreg_t flag)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        xwospl_cpuirq_restore_lc(flag);
}

/**
 * @brief XWUP API：开启写临界区，并关闭部分外部中断
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
__xwup_api
void xwup_sqlk_wr_lock_irqs(struct xwup_sqlk * sql,
                            const struct xwup_irq_resource * irqs,
                            xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_disable(irqs[i].irqn);
        }
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWUP API：尝试开启写临界区，并关闭部分外部中断
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
__xwup_api
xwer_t xwup_sqlk_wr_trylock_irqs(struct xwup_sqlk * sql,
                                 const struct xwup_irq_resource * irqs,
                                 xwsz_t num)
{
        xwup_sqlk_wr_lock_irqs(sql, irqs, num);
        return XWOK;
}

/**
 * @brief XWUP API：关闭写临界区，并开启部分外部中断
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_wr_unlock_irqs(struct xwup_sqlk * sql,
                              const struct xwup_irq_resource * irqs,
                              xwsz_t num)
{
        xwssz_t i;

        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_enable(irqs[i].irqn);
        }
}


/**
 * @brief XWUP API：开启写临界区，保存部分外部中断的中断标志并关闭
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
__xwup_api
void xwup_sqlk_wr_lock_irqssv(struct xwup_sqlk * sql,
                              const struct xwup_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwup_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwup_skd_dspmpt_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWUP API：尝试开启写临界区，保存部分外部中断的中断标志并关闭
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
__xwup_api
xwer_t xwup_sqlk_wr_trylock_irqssv(struct xwup_sqlk * sql,
                                   const struct xwup_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_wr_lock_irqssv(sql, irqs, flags, num);
        return XWOK;
}

/**
 * @brief XWUP API：关闭写临界区，恢复部分外部中断的中断标志
 * @parem sql: (I) 顺序锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_wr_unlock_irqsrs(struct xwup_sqlk * sql,
                                const struct xwup_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwup_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
/**
 * @brief XWUP API：开启写临界区，关闭本地CPU的中断底半部
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
__xwup_api
void xwup_sqlk_wr_lock_bh(struct xwup_sqlk * sql)
{
        xwup_skd_dspmpt_lc();
        xwup_skd_dsbh_lc();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwmb_mp_wmb();
}

/**
 * @brief XWUP API：尝试开启写临界区，关闭本地CPU的中断底半部
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
__xwup_api
xwer_t xwup_sqlk_wr_trylock_bh(struct xwup_sqlk * sql)
{
        xwup_sqlk_wr_lock_bh(sql);
        return XWOK;
}

/**
 * @brief XWUP API：关闭写临界区，开启本地CPU的中断底半部
 * @parem sql: (I) 顺序锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
__xwup_api
void xwup_sqlk_wr_unlock_bh(struct xwup_sqlk * sql)
{
        xwmb_mp_wmb();
        sql->seq += XWUP_SQLK_GRANULARITY;
        xwup_skd_enbh_lc();
        xwup_skd_enpmpt_lc();
}
#endif /* XWUPCFG_SKD_BH */
