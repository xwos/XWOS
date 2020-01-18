/**
 * @file
 * @brief XuanWuOS的锁机制：虚假自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 单核(UP)系统中并不需要自旋锁，为了满足xwosal中API的风格，
 *   源码中实现的自旋锁是虚假的。
 */

#ifndef __xwos_up_lock_fakespinlock_h__
#define __xwos_up_lock_fakespinlock_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/up/lock/seqlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 虚假自旋锁
 */
struct xwlk_splk {
        struct xwlk_sqlk fake;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********  macro functions  ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_inline_api
void xwlk_splk_init(struct xwlk_splk * splk)
{
        xwlk_sqlk_init(&splk->fake);
}

static __xwos_inline_api
void xwlk_splk_lock(struct xwlk_splk * splk)
{
        xwlk_sqlk_wr_lock(&splk->fake);
}

static __xwos_inline_api
xwer_t xwlk_splk_trylock(struct xwlk_splk * splk)
{
        return xwlk_sqlk_wr_trylock(&splk->fake);
}

static __xwos_inline_api
void xwlk_splk_unlock(struct xwlk_splk * splk)
{
        xwlk_sqlk_wr_unlock(&splk->fake);
}

static __xwos_inline_api
void xwlk_splk_lock_cpuirq(struct xwlk_splk * splk)
{
        xwlk_sqlk_wr_lock_cpuirq(&splk->fake);
}

static __xwos_inline_api
xwer_t xwlk_splk_trylock_cpuirq(struct xwlk_splk * splk)
{
        return xwlk_sqlk_wr_trylock_cpuirq(&splk->fake);
}

static __xwos_inline_api
void xwlk_splk_unlock_cpuirq(struct xwlk_splk * splk)
{
        xwlk_sqlk_wr_unlock_cpuirq(&splk->fake);
}

static __xwos_inline_api
void xwlk_splk_lock_cpuirqsv(struct xwlk_splk * splk, xwreg_t * flag)
{
        xwlk_sqlk_wr_lock_cpuirqsv(&splk->fake, flag);
}

static __xwos_inline_api
xwer_t xwlk_splk_trylock_cpuirqsv(struct xwlk_splk * splk, xwreg_t * flag)
{
        return xwlk_sqlk_wr_trylock_cpuirqsv(&splk->fake, flag);
}

static __xwos_inline_api
void xwlk_splk_unlock_cpuirqrs(struct xwlk_splk * splk, xwreg_t flag)
{
        xwlk_sqlk_wr_unlock_cpuirqrs(&splk->fake, flag);
}

static __xwos_inline_api
void xwlk_splk_lock_irqs(struct xwlk_splk * splk,
                         const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqs(&splk->fake, irqs, num);
}

static __xwos_inline_api
xwer_t xwlk_splk_trylock_irqs(struct xwlk_splk * splk,
                              const struct xwos_irq_resource * irqs, xwsz_t num)
{
        return xwlk_sqlk_wr_trylock_irqs(&splk->fake, irqs, num);
}

static __xwos_inline_api
void xwlk_splk_unlock_irqs(struct xwlk_splk * splk,
                           const struct xwos_irq_resource * irqs, xwsz_t num)
{
        xwlk_sqlk_wr_unlock_irqs(&splk->fake, irqs, num);
}

static __xwos_inline_api
void xwlk_splk_lock_irqssv(struct xwlk_splk * splk,
                           const struct xwos_irq_resource * irqs,
                           xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_wr_lock_irqssv(&splk->fake, irqs, flags, num);
}

static __xwos_inline_api
xwer_t xwlk_splk_trylock_irqssv(struct xwlk_splk * splk,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        return xwlk_sqlk_wr_trylock_irqssv(&splk->fake, irqs, flags, num);
}

static __xwos_inline_api
void xwlk_splk_unlock_irqsrs(struct xwlk_splk * splk,
                             const struct xwos_irq_resource * irqs,
                             xwreg_t flags[], xwsz_t num)
{
        xwlk_sqlk_wr_unlock_irqsrs(&splk->fake, irqs, flags, num);
}

static __xwos_inline_api
void xwlk_splk_lock_bh(struct xwlk_splk * splk)
{
        xwlk_sqlk_wr_lock_bh(&splk->fake);
}

static __xwos_inline_api
xwer_t xwlk_splk_trylock_bh(struct xwlk_splk * splk)
{
        return xwlk_sqlk_wr_trylock_bh(&splk->fake);
}

static __xwos_inline_api
void xwlk_splk_unlock_bh(struct xwlk_splk * splk)
{
        xwlk_sqlk_wr_unlock_bh(&splk->fake);
}

#endif /* xwos/up/lock/fakespinlock.h */
