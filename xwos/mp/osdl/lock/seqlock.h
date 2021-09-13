/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：顺序锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_lock_seqlock_h__
#define __xwos_mp_osdl_lock_seqlock_h__

#include <xwos/mp/osdl/irq.h>
#include <xwos/mp/lock/seqlock.h>

#define XWOSDL_SQLK_GRANULARITY          XWMP_SQLK_GRANULARITY

typedef struct xwmp_sqlk xwosdl_sqlk_t;

static __xwcc_inline
void xwosdl_sqlk_init(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_init(ossql);
}

static __xwcc_inline
xwsq_t xwosdl_sqlk_rd_begin(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_rd_begin(ossql);
}

static __xwcc_inline
bool xwosdl_sqlk_rd_retry(xwosdl_sqlk_t * ossql, xwsq_t start)
{
        return xwmp_sqlk_rd_retry(ossql, start);
}

static __xwcc_inline
xwsq_t xwosdl_sqlk_get_seq(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_get_seq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_rdex_lock(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_rdex_trylock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_rdex_unlock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_rdex_lock_cpuirq(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_rdex_trylock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_rdex_unlock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        xwmp_sqlk_rdex_lock_cpuirqsv(ossql, cpuirq);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        return xwmp_sqlk_rdex_trylock_cpuirqsv(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t cpuirq)
{
        xwmp_sqlk_rdex_unlock_cpuirqrs(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_irqs(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwmp_sqlk_rdex_lock_irqs(ossql, irqs, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqs(xwosdl_sqlk_t * ossql,
                                     const struct xwos_irq_resource * irqs,
                                     xwsz_t num)
{
        return xwmp_sqlk_rdex_trylock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_irqs(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwsz_t num)
{
        xwmp_sqlk_rdex_unlock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_irqssv(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwmp_sqlk_rdex_lock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                       const struct xwos_irq_resource * irqs,
                                       xwreg_t flags[], xwsz_t num)
{
        return xwmp_sqlk_rdex_trylock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                    const struct xwos_irq_resource * irqs,
                                    xwreg_t flags[], xwsz_t num)
{
        xwmp_sqlk_rdex_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_rdex_lock_bh(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_rdex_trylock_bh(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_rdex_unlock_bh(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_wr_lock(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_wr_trylock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_wr_unlock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_wr_lock_cpuirq(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_wr_trylock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_wr_unlock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        xwmp_sqlk_wr_lock_cpuirqsv(ossql, cpuirq);
}


static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        return xwmp_sqlk_wr_trylock_cpuirqsv(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t cpuirq)
{
        xwmp_sqlk_wr_unlock_cpuirqrs(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_irqs(xwosdl_sqlk_t * ossql,
                              const struct xwos_irq_resource * irqs,
                              xwsz_t num)
{
        xwmp_sqlk_wr_lock_irqs(ossql, irqs, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_irqs(xwosdl_sqlk_t * ossql,
                                   const struct xwos_irq_resource * irqs,
                                   xwsz_t num)
{
        return xwmp_sqlk_wr_trylock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_irqs(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwsz_t num)
{
        xwmp_sqlk_wr_unlock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_irqssv(xwosdl_sqlk_t * ossql,
                                const struct xwos_irq_resource * irqs,
                                xwreg_t flags[], xwsz_t num)
{
        xwmp_sqlk_wr_lock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                     const struct xwos_irq_resource * irqs,
                                     xwreg_t flags[], xwsz_t num)
{
        return xwmp_sqlk_wr_trylock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                  const struct xwos_irq_resource * irqs,
                                  xwreg_t flags[], xwsz_t num)
{
        xwmp_sqlk_wr_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_wr_lock_bh(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwmp_sqlk_wr_trylock_bh(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwmp_sqlk_wr_unlock_bh(ossql);
}

#endif /* xwos/mp/osdl/lock/seqlock.h */
