/**
 * @file
 * @brief 玄武OS UP内核锁机制：虚假自旋锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 * @note
 * - 单核(UP)系统中并不需要自旋锁，为了满足xwosal中API的风格，
 *   源码中实现的自旋锁是虚假的。
 */

#ifndef __xwos_up_lock_fakespinlock_h__
#define __xwos_up_lock_fakespinlock_h__

#include <xwos/standard.h>
#include <xwos/up/lock/seqlock.h>

/**
 * @brief 虚假自旋锁
 */
struct xwup_splk {
        struct xwup_sqlk fake;
};

static __xwup_inline_api
void xwup_splk_init(struct xwup_splk * splk)
{
        xwup_sqlk_init(&splk->fake);
}

static __xwup_inline_api
void xwup_splk_lock(struct xwup_splk * splk)
{
        xwup_sqlk_wr_lock(&splk->fake);
}

static __xwup_inline_api
xwer_t xwup_splk_trylock(struct xwup_splk * splk)
{
        return xwup_sqlk_wr_trylock(&splk->fake);
}

static __xwup_inline_api
void xwup_splk_unlock(struct xwup_splk * splk)
{
        xwup_sqlk_wr_unlock(&splk->fake);
}

static __xwup_inline_api
void xwup_splk_lock_cpuirq(struct xwup_splk * splk)
{
        xwup_sqlk_wr_lock_cpuirq(&splk->fake);
}

static __xwup_inline_api
xwer_t xwup_splk_trylock_cpuirq(struct xwup_splk * splk)
{
        return xwup_sqlk_wr_trylock_cpuirq(&splk->fake);
}

static __xwup_inline_api
void xwup_splk_unlock_cpuirq(struct xwup_splk * splk)
{
        xwup_sqlk_wr_unlock_cpuirq(&splk->fake);
}

static __xwup_inline_api
void xwup_splk_lock_cpuirqsv(struct xwup_splk * splk, xwreg_t * flag)
{
        xwup_sqlk_wr_lock_cpuirqsv(&splk->fake, flag);
}

static __xwup_inline_api
xwer_t xwup_splk_trylock_cpuirqsv(struct xwup_splk * splk, xwreg_t * flag)
{
        return xwup_sqlk_wr_trylock_cpuirqsv(&splk->fake, flag);
}

static __xwup_inline_api
void xwup_splk_unlock_cpuirqrs(struct xwup_splk * splk, xwreg_t flag)
{
        xwup_sqlk_wr_unlock_cpuirqrs(&splk->fake, flag);
}

static __xwup_inline_api
void xwup_splk_lock_irqs(struct xwup_splk * splk,
                         const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwup_sqlk_wr_lock_irqs(&splk->fake, irqs, num);
}

static __xwup_inline_api
xwer_t xwup_splk_trylock_irqs(struct xwup_splk * splk,
                              const struct xwos_irq_resource * irqs, xwsz_t num)
{
        return xwup_sqlk_wr_trylock_irqs(&splk->fake, irqs, num);
}

static __xwup_inline_api
void xwup_splk_unlock_irqs(struct xwup_splk * splk,
                           const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwup_sqlk_wr_unlock_irqs(&splk->fake, irqs, num);
}

static __xwup_inline_api
void xwup_splk_lock_irqssv(struct xwup_splk * splk,
                           const struct xwos_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_wr_lock_irqssv(&splk->fake, irqs, flags, num);
}

static __xwup_inline_api
xwer_t xwup_splk_trylock_irqssv(struct xwup_splk * splk,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        return xwup_sqlk_wr_trylock_irqssv(&splk->fake, irqs, flags, num);
}

static __xwup_inline_api
void xwup_splk_unlock_irqsrs(struct xwup_splk * splk,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_wr_unlock_irqsrs(&splk->fake, irqs, flags, num);
}

static __xwup_inline_api
void xwup_splk_lock_bh(struct xwup_splk * splk)
{
        xwup_sqlk_wr_lock_bh(&splk->fake);
}

static __xwup_inline_api
xwer_t xwup_splk_trylock_bh(struct xwup_splk * splk)
{
        return xwup_sqlk_wr_trylock_bh(&splk->fake);
}

static __xwup_inline_api
void xwup_splk_unlock_bh(struct xwup_splk * splk)
{
        xwup_sqlk_wr_unlock_bh(&splk->fake);
}

#endif /* xwos/up/lock/fakespinlock.h */
