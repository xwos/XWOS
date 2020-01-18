/**
 * @file
 * @brief XuanWuOS的锁机制：自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/smp/irq.h>
#include <xwos/smp/scheduler.h>
#include <arch_spinlock.h>
#include <xwos/smp/lock/spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   .sdata & .data  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwlk_rawly_lock(struct xwlk_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_rawly_trylock(struct xwlk_splk * spl)
{
        xwer_t rc;

        rc = OK;
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_rawly_unlock(struct xwlk_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
void xwlk_rawly_lock_cpuirq(struct xwlk_splk * spl)
{
        xwos_cpuirq_disable_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_rawly_trylock_cpuirq(struct xwlk_splk * spl)
{
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_disable_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_cpuirq_enable_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_rawly_unlock_cpuirq(struct xwlk_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_cpuirq_enable_lc();
}

__xwos_code
void xwlk_rawly_lock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq)
{
        xwos_cpuirq_save_lc(cpuirq);
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_rawly_trylock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_save_lc(cpuirq);
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_cpuirq_restore_lc(*cpuirq);
                return rc;
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_rawly_unlock_cpuirqrs(struct xwlk_splk * spl, xwreg_t cpuirq)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_cpuirq_restore_lc(cpuirq);
}

__xwos_code
void xwlk_rawly_lock_irqs(struct xwlk_splk * spl,
                          const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_rawly_trylock_irqs(struct xwlk_splk * spl,
                               const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = OK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwos_irq_enable(irqs[i].irqn);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_rawly_unlock_irqs(struct xwlk_splk * spl,
                            const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_enable(irqs[i].irqn);
        }
}

__xwos_code
void xwlk_rawly_lock_irqssv(struct xwlk_splk * spl,
                            const struct xwos_irq_resource * irqs,
                            xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_rawly_trylock_irqssv(struct xwlk_splk * spl,
                                 const struct xwos_irq_resource * irqs,
                                 xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = OK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwos_irq_restore(irqs[i].irqn, flags[i]);
                }
                return rc;
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_rawly_unlock_irqsrs(struct xwlk_splk * spl,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_restore(irqs[i].irqn, flags[i]);
        }
}

/**
 * @brief XWOS API：上锁自旋锁。若自旋锁已开启临界区，就自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
__xwos_api
void xwlk_splk_lock(struct xwlk_splk * spl)
{
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWOS API：尝试上锁自旋锁。若自旋锁已开启临界区。
 *                  若自旋锁已开启临界区，不会自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
__xwos_api
xwer_t xwlk_splk_trylock(struct xwlk_splk * spl)
{
        xwer_t rc;

        rc = OK;
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWOS API：解锁自旋锁。
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_splk_unlock(struct xwlk_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
}

/**
 * @brief XWOS API：上锁自旋锁，并关闭本地CPU的中断。
 *                  若自旋锁已开启临界区，就自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
__xwos_api
void xwlk_splk_lock_cpuirq(struct xwlk_splk * spl)
{
        xwos_cpuirq_disable_lc();
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWOS API：尝试上锁自旋锁，并关闭本地CPU的中断。
 *                  若自旋锁已开启临界区，不会自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
__xwos_api
xwer_t xwlk_splk_trylock_cpuirq(struct xwlk_splk * spl)
{
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_disable_lc();
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
                xwos_cpuirq_enable_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWOS API：解锁自旋锁，并开启本地CPU的中断。
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_splk_unlock_cpuirq(struct xwlk_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_enable_lc();
}

/**
 * @brief XWOS API：上锁自旋锁，保存本地CPU的中断标志并关闭。
 *                  若自旋锁已开启临界区，就自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
__xwos_api
void xwlk_splk_lock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq)
{
        xwos_cpuirq_save_lc(cpuirq);
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWOS API：尝试上锁自旋锁，保存本地CPU的中断标志并关闭。
 *                  若自旋锁已开启临界区，不会自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
__xwos_api
xwer_t xwlk_splk_trylock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_save_lc(cpuirq);
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
                xwos_cpuirq_restore_lc(*cpuirq);
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWOS API：解锁自旋锁，并恢复本地CPU的中断标志。
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_splk_unlock_cpuirqrs(struct xwlk_splk * spl, xwreg_t cpuirq)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWOS API：上锁自旋锁，并关闭部分外部中断。
 *                  若自旋锁已开启临界区，就自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。。
 */
__xwos_api
void xwlk_splk_lock_irqs(struct xwlk_splk * spl,
                         const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWOS API：尝试上锁自旋锁，并关闭部分外部中断。
 *                  若自旋锁已开启临界区，不会自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
__xwos_api
xwer_t xwlk_splk_trylock_irqs(struct xwlk_splk * spl,
                              const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = OK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwos_irq_enable(irqs[i].irqn);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWOS API：解锁自旋锁，并开启部分外部中断。
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_splk_unlock_irqs(struct xwlk_splk * spl,
                           const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_enable(irqs[i].irqn);
        }
}

/**
 * @brief XWOS API：上锁自旋锁，保存部分外部中断的中断标志并关闭。
 *                  若自旋锁已开启临界区，就自旋等待。
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
__xwos_api
void xwlk_splk_lock_irqssv(struct xwlk_splk * spl,
                           const struct xwos_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWOS API：尝试上锁自旋锁，保存部分外部中断的中断标志并关闭。
 *                  若自旋锁已开启临界区，不会自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
__xwos_api
xwer_t xwlk_splk_trylock_irqssv(struct xwlk_splk * spl,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = OK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwos_irq_restore(irqs[i].irqn, flags[i]);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWOS API：解锁自旋锁，并恢复部分外部中断的中断标志。
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_splk_unlock_irqsrs(struct xwlk_splk * spl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
/**
 * @brief XWOS API：上锁自旋锁，关闭本地CPU的中断底半部。
 *                  若自旋锁已开启临界区，就自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程上下文与
 *   中断底半部中是安全的。
 */
__xwos_api
void xwlk_splk_lock_bh(struct xwlk_splk * spl)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_dspmpt_lc();
        xwos_scheduler_dsbh(xwsd);
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWOS API：尝试上锁自旋锁，关闭本地CPU的中断底半部。
 *                  若自旋锁已开启临界区，不会自旋等待。
 * @param spl: (I) 自旋锁的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EAGAIN: 获得自旋锁失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程上下文与
 *   中断底半部中是安全的。
 */
__xwos_api
xwer_t xwlk_splk_trylock_bh(struct xwlk_splk * spl)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        rc = OK;
        xwsd = xwos_scheduler_dspmpt_lc();
        xwos_scheduler_dsbh(xwsd);
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwos_scheduler_enbh(xwsd);
                xwos_scheduler_enpmpt_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWOS API：解锁自旋锁，开启本地CPU的中断底半部。
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
void xwlk_splk_unlock_bh(struct xwlk_splk * spl)
{
        struct xwos_scheduler * xwsd;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwsd = xwos_scheduler_enbh_lc();
        xwos_scheduler_enpmpt(xwsd);
}
#endif /* XWSMPCFG_SD_BH */

__xwos_code
void xwlk_bitsplk_lock(xwptr_t * ptr)
{
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_lock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_bitsplk_trylock(xwptr_t * ptr)
{
        xwer_t rc;

        rc = OK;
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_bitsplk_trylock(ptr);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_bitsplk_unlock(xwptr_t * ptr)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_unlock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
}

__xwos_code
void xwlk_bitsplk_lock_cpuirq(xwptr_t * ptr)
{
        xwos_cpuirq_disable_lc();
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_lock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_bitsplk_trylock_cpuirq(xwptr_t * ptr)
{
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_disable_lc();
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_bitsplk_trylock(ptr);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
                xwos_cpuirq_enable_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_bitsplk_unlock_cpuirq(xwptr_t * ptr)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_unlock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_enable_lc();
}

__xwos_code
void xwlk_bitsplk_lock_cpuirqsv(xwptr_t * ptr, xwreg_t * cpuirq)
{
        xwos_cpuirq_save_lc(cpuirq);
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_lock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_bitsplk_trylock_cpuirqsv(xwptr_t * ptr, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = OK;
        xwos_cpuirq_save_lc(cpuirq);
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_bitsplk_trylock(ptr);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
                xwos_cpuirq_restore_lc(*cpuirq);
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_bitsplk_unlock_cpuirqrs(xwptr_t * ptr, xwreg_t cpuirq)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_unlock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        xwos_cpuirq_restore_lc(cpuirq);
}

__xwos_code
void xwlk_bitsplk_lock_irqs(xwptr_t * ptr,
                            const struct xwos_irq_resource * irqs,
                            xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_lock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_bitsplk_trylock_irqs(xwptr_t * ptr,
                                 const struct xwos_irq_resource * irqs,
                                 xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = OK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_disable(irqs[i].irqn);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_bitsplk_trylock(ptr);
        if (rc < 0) {
                xwos_scheduler_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwos_irq_enable(irqs[i].irqn);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_bitsplk_unlock_irqs(xwptr_t * ptr,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_unlock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_enable(irqs[i].irqn);
        }
}

__xwos_code
void xwlk_bitsplk_lock_irqssv(xwptr_t * ptr,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_lock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_bitsplk_trylock_irqssv(xwptr_t * ptr,
                                   const struct xwos_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = OK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwos_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwos_scheduler_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_bitsplk_trylock(ptr);
        if (rc) {
                xwos_scheduler_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwos_irq_restore(irqs[i].irqn, flags[i]);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_bitsplk_unlock_irqsrs(xwptr_t * ptr,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_unlock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwos_scheduler_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwos_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if (defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH))
__xwos_code
void xwlk_bitsplk_lock_bh(xwptr_t * ptr)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_dspmpt_lc();
        xwos_scheduler_dsbh(xwsd);
#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_lock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwos_code
xwer_t xwlk_bitsplk_trylock_bh(xwptr_t * ptr)
{
        struct xwos_scheduler * xwsd;
        xwer_t rc;

        rc = OK;
        xwsd = xwos_scheduler_dspmpt_lc();
        xwos_scheduler_dsbh(xwsd);
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_bitsplk_trylock(ptr);
        if (rc < 0) {
                xwos_scheduler_enbh(xwsd);
                xwos_scheduler_enpmpt_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwos_code
void xwlk_bitsplk_unlock_bh(xwptr_t * ptr)
{
        struct xwos_scheduler * xwsd;

#if (CPUCFG_CPU_NUM > 1)
        arch_bitsplk_unlock(ptr);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(ptr);
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwsd = xwos_scheduler_enbh_lc();
        xwos_scheduler_enpmpt(xwsd);
}
#endif /* XWSMPCFG_SD_BH */
