/**
 * @file
 * @brief XuanWuOS的锁机制：自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_lock_spinlock_h__
#define __xwos_smp_lock_spinlock_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <arch_spinlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS的自旋锁
 */
struct xwlk_splk {
        struct arch_splk aspl; /**< CPU架构的特色自旋锁结构体 */
};

struct xwos_irq_resource;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  macro functions  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwlk_rawly_lock(struct xwlk_splk * spl);

__xwos_code
xwer_t xwlk_rawly_trylock(struct xwlk_splk * spl);

__xwos_code
void xwlk_rawly_unlock(struct xwlk_splk * spl);

__xwos_code
void xwlk_rawly_lock_cpuirq(struct xwlk_splk * spl);

__xwos_code
xwer_t xwlk_rawly_trylock_cpuirq(struct xwlk_splk * spl);

__xwos_code
void xwlk_rawly_unlock_cpuirq(struct xwlk_splk * spl);

__xwos_code
void xwlk_rawly_lock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq);

__xwos_code
xwer_t xwlk_rawly_trylock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq);

__xwos_code
void xwlk_rawly_unlock_cpuirqrs(struct xwlk_splk * spl, xwreg_t cpuirq);

__xwos_code
void xwlk_rawly_lock_irqs(struct xwlk_splk * spl,
                          const struct xwos_irq_resource * irqs, xwsz_t num);

__xwos_code
xwer_t xwlk_rawly_trylock_irqs(struct xwlk_splk * spl,
                               const struct xwos_irq_resource * irqs, xwsz_t num);

__xwos_code
void xwlk_rawly_unlock_irqs(struct xwlk_splk * spl,
                            const struct xwos_irq_resource * irqs, xwsz_t num);

__xwos_code
void xwlk_rawly_lock_irqssv(struct xwlk_splk * spl,
                            const struct xwos_irq_resource * irqs,
                            xwreg_t flags[], xwsz_t num);

__xwos_code
xwer_t xwlk_rawly_trylock_irqssv(struct xwlk_splk * spl,
                                 const struct xwos_irq_resource * irqs,
                                 xwreg_t flags[], xwsz_t num);

__xwos_code
void xwlk_rawly_unlock_irqsrs(struct xwlk_splk * spl,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
void xwlk_splk_lock(struct xwlk_splk * spl);

__xwos_api
xwer_t xwlk_splk_trylock(struct xwlk_splk * spl);

__xwos_api
void xwlk_splk_unlock(struct xwlk_splk * spl);

__xwos_api
void xwlk_splk_lock_cpuirq(struct xwlk_splk * spl);

__xwos_api
xwer_t xwlk_splk_trylock_cpuirq(struct xwlk_splk * spl);

__xwos_api
void xwlk_splk_unlock_cpuirq(struct xwlk_splk * spl);

__xwos_api
void xwlk_splk_lock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq);

__xwos_api
xwer_t xwlk_splk_trylock_cpuirqsv(struct xwlk_splk * spl, xwreg_t * cpuirq);

__xwos_api
void xwlk_splk_unlock_cpuirqrs(struct xwlk_splk * spl, xwreg_t cpuirq);

__xwos_api
void xwlk_splk_lock_irqs(struct xwlk_splk * spl,
                         const struct xwos_irq_resource * irqs,
                         xwsz_t num);

__xwos_api
xwer_t xwlk_splk_trylock_irqs(struct xwlk_splk * spl,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num);

__xwos_api
void xwlk_splk_unlock_irqs(struct xwlk_splk * spl,
                           const struct xwos_irq_resource * irqs,
                           xwsz_t num);

__xwos_api
void xwlk_splk_lock_irqssv(struct xwlk_splk * spl,
                           const struct xwos_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num);

__xwos_api
xwer_t xwlk_splk_trylock_irqssv(struct xwlk_splk * spl,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_splk_unlock_irqsrs(struct xwlk_splk * spl,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_splk_lock_bh(struct xwlk_splk * spl);

__xwos_api
xwer_t xwlk_splk_trylock_bh(struct xwlk_splk * spl);

__xwos_api
void xwlk_splk_unlock_bh(struct xwlk_splk * spl);

__xwos_api
void xwlk_bitsplk_lock(xwptr_t * ptr);

__xwos_api
xwer_t xwlk_bitsplk_trylock(xwptr_t * ptr);

__xwos_api
void xwlk_bitsplk_unlock(xwptr_t * ptr);

__xwos_api
void xwlk_bitsplk_lock_cpuirq(xwptr_t * ptr);

__xwos_api
xwer_t xwlk_bitsplk_trylock_cpuirq(xwptr_t * ptr);

__xwos_api
void xwlk_bitsplk_unlock_cpuirq(xwptr_t * ptr);

__xwos_api
void xwlk_bitsplk_lock_cpuirqsv(xwptr_t * ptr, xwreg_t * cpuirq);

__xwos_api
xwer_t xwlk_bitsplk_trylock_cpuirqsv(xwptr_t * ptr, xwreg_t * cpuirq);

__xwos_api
void xwlk_bitsplk_unlock_cpuirqrs(xwptr_t * ptr, xwreg_t cpuirq);

__xwos_api
void xwlk_bitsplk_lock_irqs(xwptr_t * ptr, const struct xwos_irq_resource * irqs,
                            xwsz_t num);

__xwos_api
xwer_t xwlk_bitsplk_trylock_irqs(xwptr_t * ptr,
                                 const struct xwos_irq_resource * irqs,
                                 xwsz_t num);

__xwos_api
void xwlk_bitsplk_unlock_irqs(xwptr_t * ptr, const struct xwos_irq_resource * irqs,
                              xwsz_t num);

__xwos_api
void xwlk_bitsplk_lock_irqssv(xwptr_t * ptr,
                              const struct xwos_irq_resource * irqs,
                              xwreg_t flags[], xwsz_t num);

__xwos_api
xwer_t xwlk_bitsplk_trylock_irqssv(xwptr_t * ptr,
                                   const struct xwos_irq_resource * irqs,
                                   xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_bitsplk_unlock_irqsrs(xwptr_t * ptr,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num);

__xwos_api
void xwlk_bitsplk_lock_bh(xwptr_t * ptr);

__xwos_api
xwer_t xwlk_bitsplk_trylock_bh(xwptr_t * ptr);

__xwos_api
void xwlk_bitsplk_unlock_bh(xwptr_t * ptr);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：初始化自旋锁
 * @parem spl: (I) 自旋锁指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwlk_splk_init(struct xwlk_splk * spl)
{
        arch_splk_init(&spl->aspl);
}

/**
 * @brief XWOS API：初始化位自旋锁
 * @parem spl: (I) 自旋锁指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_inline_api
void xwlk_bitsplk_init(__maybe_unused xwptr_t * ptr)
{
        arch_bitsplk_init(ptr);
}

#endif /* xwos/smp/lock/spinlock.h */
