/**
 * @file
 * @brief 玄武OS MP内核锁机制：自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#include <xwos/mp/skd.h>
#include <arch_spinlock.h>
#include <xwos/mp/lock/spinlock.h>

__xwmp_code
void xwmp_rawly_lock(struct xwmp_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwmp_code
xwer_t xwmp_rawly_trylock(struct xwmp_splk * spl)
{
        xwer_t rc;

        rc = XWOK;
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock(struct xwmp_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwmp_code
void xwmp_rawly_lock_cpuirq(struct xwmp_splk * spl)
{
        xwospl_cpuirq_disable_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwmp_code
xwer_t xwmp_rawly_trylock_cpuirq(struct xwmp_splk * spl)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_disable_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwospl_cpuirq_enable_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_cpuirq(struct xwmp_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwospl_cpuirq_enable_lc();
}

__xwmp_code
void xwmp_rawly_lock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwmp_code
xwer_t xwmp_rawly_trylock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(cpuirq);
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwospl_cpuirq_restore_lc(*cpuirq);
                return rc;
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_cpuirqrs(struct xwmp_splk * spl, xwreg_t cpuirq)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwmp_code
void xwmp_rawly_lock_irqs(struct xwmp_splk * spl,
                          const struct xwmp_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwmp_code
xwer_t xwmp_rawly_trylock_irqs(struct xwmp_splk * spl,
                               const struct xwmp_irq_resource * irqs, xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_enable(irqs[i].irqn);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_irqs(struct xwmp_splk * spl,
                            const struct xwmp_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_enable(irqs[i].irqn);
        }
}

__xwmp_code
void xwmp_rawly_lock_irqssv(struct xwmp_splk * spl,
                            const struct xwmp_irq_resource * irqs,
                            xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

__xwmp_code
xwer_t xwmp_rawly_trylock_irqssv(struct xwmp_splk * spl,
                                 const struct xwmp_irq_resource * irqs,
                                 xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_restore(irqs[i].irqn, flags[i]);
                }
                return rc;
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

__xwmp_code
void xwmp_rawly_unlock_irqsrs(struct xwmp_splk * spl,
                              const struct xwmp_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_restore(irqs[i].irqn, flags[i]);
        }
}

/**
 * @brief XWMP API：初始化自旋锁
 * @parem spl: (I) 自旋锁指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_inline_api
void xwmp_splk_init(struct xwmp_splk * spl)
{
        arch_splk_init(&spl->aspl);
}

/**
 * @brief XWMP API：上锁自旋锁
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占，因此只能保证其临界区在线程中是安全的。
 */
__xwmp_api
void xwmp_splk_lock(struct xwmp_splk * spl)
{
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWMP API：尝试上锁自旋锁
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
__xwmp_api
xwer_t xwmp_splk_trylock(struct xwmp_splk * spl)
{
        xwer_t rc;

        rc = XWOK;
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWMP API：解锁自旋锁
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：不可重入
 */
__xwmp_api
void xwmp_splk_unlock(struct xwmp_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwmp_skd_enpmpt_lc();
}

/**
 * @brief XWMP API：上锁自旋锁，并关闭本地CPU的中断
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
__xwmp_api
void xwmp_splk_lock_cpuirq(struct xwmp_splk * spl)
{
        xwospl_cpuirq_disable_lc();
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWMP API：尝试上锁自旋锁，并关闭本地CPU的中断
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
__xwmp_api
xwer_t xwmp_splk_trylock_cpuirq(struct xwmp_splk * spl)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_disable_lc();
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                xwospl_cpuirq_enable_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWMP API：解锁自旋锁，并开启本地CPU的中断
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
void xwmp_splk_unlock_cpuirq(struct xwmp_splk * spl)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwmp_skd_enpmpt_lc();
        xwospl_cpuirq_enable_lc();
}

/**
 * @brief XWMP API：上锁自旋锁，保存本地CPU的中断标志并关闭
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (O) 缓冲区指针，用于返回本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，就自旋等待。
 * - 此函数会关闭本地CPU的抢占与中断，因此可保证其临界区在所有上下文中都是安全的。
 */
__xwmp_api
void xwmp_splk_lock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWMP API：尝试上锁自旋锁，保存本地CPU的中断标志并关闭
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
__xwmp_api
xwer_t xwmp_splk_trylock_cpuirqsv(struct xwmp_splk * spl, xwreg_t * cpuirq)
{
        xwer_t rc;

        rc = XWOK;
        xwospl_cpuirq_save_lc(cpuirq);
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                xwospl_cpuirq_restore_lc(*cpuirq);
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWMP API：解锁自旋锁，并恢复本地CPU的中断标志
 * @param spl: (I) 自旋锁的指针
 * @param cpuirq: (I) 本地CPU的中断标志
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
void xwmp_splk_unlock_cpuirqrs(struct xwmp_splk * spl, xwreg_t cpuirq)
{
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwmp_skd_enpmpt_lc();
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWMP API：上锁自旋锁，并关闭部分外部中断
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
__xwmp_api
void xwmp_splk_lock_irqs(struct xwmp_splk * spl,
                         const struct xwmp_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWMP API：尝试上锁自旋锁，并关闭部分外部中断
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
__xwmp_api
xwer_t xwmp_splk_trylock_irqs(struct xwmp_splk * spl,
                              const struct xwmp_irq_resource * irqs, xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_disable(irqs[i].irqn);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_enable(irqs[i].irqn);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWMP API：解锁自旋锁，并开启部分外部中断
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwmp_api
void xwmp_splk_unlock_irqs(struct xwmp_splk * spl,
                           const struct xwmp_irq_resource * irqs, xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwmp_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_enable(irqs[i].irqn);
        }
}

/**
 * @brief XWMP API：上锁自旋锁，保存部分外部中断的中断标志并关闭
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (O) 缓冲区指针，用于返回部分外部中断的中断标志
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和部分外部中断，因此只能保证其临界区在这些中断
 *   与线程上下文是安全的。
 */
__xwmp_api
void xwmp_splk_lock_irqssv(struct xwmp_splk * spl,
                           const struct xwmp_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWMP API：尝试上锁自旋锁，保存部分外部中断的中断标志并关闭
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
__xwmp_api
xwer_t xwmp_splk_trylock_irqssv(struct xwmp_splk * spl,
                                const struct xwmp_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwer_t rc;
        xwssz_t i;

        rc = XWOK;
        for (i = 0; i < (xwssz_t)num; i++) {
                xwmp_irq_save(irqs[i].irqn, &flags[i]);
        }
        xwmp_skd_dspmpt_lc();
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwmp_skd_enpmpt_lc();
                for (i = (xwssz_t)num - 1; i >= 0; i--) {
                        xwmp_irq_restore(irqs[i].irqn, flags[i]);
                }
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWMP API：解锁自旋锁，并恢复部分外部中断的中断标志
 * @param spl: (I) 自旋锁的指针
 * @param irqs: (I) 外部中断资源数组指针
 * @param flags: (I) 部分外部中断的中断标志数组
 * @param num: (I) 数组中元素数量
 * @note
 * - 同步/异步：同步
 * - 上下文：中断资源数组中描述的中断、线程
 * - 重入性：不可重入
 */
__xwmp_api
void xwmp_splk_unlock_irqsrs(struct xwmp_splk * spl,
                             const struct xwmp_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwssz_t i;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwmp_skd_enpmpt_lc();
        for (i = (xwssz_t)num - 1; i >= 0; i--) {
                xwmp_irq_restore(irqs[i].irqn, flags[i]);
        }
}

#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
/**
 * @brief XWMP API：上锁自旋锁，关闭本地CPU的中断底半部
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 * @note
 * - 若自旋锁已开启临界区，就自旋等待。
 * - 此函数只会关闭本地CPU的抢占和中断底半部，因此只能保证其临界区在线程上下文与
 *   中断底半部中是安全的。
 */
__xwmp_api
void xwmp_splk_lock_bh(struct xwmp_splk * spl)
{
        struct xwmp_skd * xwskd;

        xwskd = xwmp_skd_dspmpt_lc();
        xwmp_skd_dsbh(xwskd);
#if (CPUCFG_CPU_NUM > 1)
        arch_splk_lock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
}

/**
 * @brief XWMP API：尝试上锁自旋锁，关闭本地CPU的中断底半部
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
__xwmp_api
xwer_t xwmp_splk_trylock_bh(struct xwmp_splk * spl)
{
        struct xwmp_skd * xwskd;
        xwer_t rc;

        rc = XWOK;
        xwskd = xwmp_skd_dspmpt_lc();
        xwmp_skd_dsbh(xwskd);
#if (CPUCFG_CPU_NUM > 1)
        rc = arch_splk_trylock(&spl->aspl);
        if (rc < 0) {
                xwmp_skd_enbh(xwskd);
                xwmp_skd_enpmpt_lc();
        }
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        return rc;
}

/**
 * @brief XWMP API：解锁自旋锁，开启本地CPU的中断底半部
 * @param spl: (I) 自旋锁的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
void xwmp_splk_unlock_bh(struct xwmp_splk * spl)
{
        struct xwmp_skd * xwskd;

#if (CPUCFG_CPU_NUM > 1)
        arch_splk_unlock(&spl->aspl);
#else /* (CPUCFG_CPU_NUM > 1) */
        XWOS_UNUSED(spl);
        xwmb_mp_mb();
#endif /* !(CPUCFG_CPU_NUM > 1) */
        xwskd = xwmp_skd_enbh_lc();
        xwmp_skd_enpmpt(xwskd);
}
#endif /* XWMPCFG_SKD_BH */
