/**
 * @file
 * @brief XWOS UP内核操作系统接口描述层：顺序锁
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_lock_seqlock_h__
#define __xwos_up_osdl_lock_seqlock_h__

#include <xwos/up/osdl/irq.h>
#include <xwos/up/lock/seqlock.h>

#define XWOSDL_SQLK_GRANULARITY XWUP_SQLK_GRANULARITY

typedef struct xwup_sqlk xwosdl_sqlk_t;

#define XWOSDL_SQLK_INITIALIZER XWUP_SQLK_INITIALIZER

static __xwcc_inline
void xwosdl_sqlk_init(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_init(ossql);
}

static __xwcc_inline
xwsq_t xwosdl_sqlk_rd_begin(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_rd_begin(ossql);
}

static __xwcc_inline
bool xwosdl_sqlk_rd_retry(xwosdl_sqlk_t * ossql, xwsq_t start)
{
        return xwup_sqlk_rd_retry(ossql, start);
}

static __xwcc_inline
xwsq_t xwosdl_sqlk_get_seq(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_get_seq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_rdex_lock(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_rdex_trylock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_rdex_unlock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_rdex_lock_cpuirq(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_rdex_trylock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_rdex_unlock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        xwup_sqlk_rdex_lock_cpuirqsv(ossql, cpuirq);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        return xwup_sqlk_rdex_trylock_cpuirqsv(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t cpuirq)
{
        xwup_sqlk_rdex_unlock_cpuirqrs(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_irqs(xwosdl_sqlk_t * ossql,
                                const xwirq_t irqs[],
                                xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqs(ossql, irqs, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqs(xwosdl_sqlk_t * ossql,
                                     const xwirq_t irqs[],
                                     xwsz_t num)
{
        return xwup_sqlk_rdex_trylock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_irqs(xwosdl_sqlk_t * ossql,
                                  const xwirq_t irqs[],
                                  xwsz_t num)
{
        xwup_sqlk_rdex_unlock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_irqssv(xwosdl_sqlk_t * ossql,
                                  const xwirq_t irqs[],
                                  xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_rdex_lock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                       const xwirq_t irqs[],
                                       xwreg_t flags[], xwsz_t num)
{
        return xwup_sqlk_rdex_trylock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                    const xwirq_t irqs[],
                                    xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_rdex_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_rdex_lock_bh(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_rdex_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_rdex_trylock_bh(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_rdex_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_rdex_unlock_bh(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_wr_lock(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_wr_trylock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_wr_unlock(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_wr_lock_cpuirq(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirq(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_wr_trylock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_cpuirq(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_wr_unlock_cpuirq(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        xwup_sqlk_wr_lock_cpuirqsv(ossql, cpuirq);
}


static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_cpuirqsv(xwosdl_sqlk_t * ossql, xwreg_t * cpuirq)
{
        return xwup_sqlk_wr_trylock_cpuirqsv(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_cpuirqrs(xwosdl_sqlk_t * ossql, xwreg_t cpuirq)
{
        xwup_sqlk_wr_unlock_cpuirqrs(ossql, cpuirq);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_irqs(xwosdl_sqlk_t * ossql,
                              const xwirq_t irqs[],
                              xwsz_t num)
{
        xwup_sqlk_wr_lock_irqs(ossql, irqs, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_irqs(xwosdl_sqlk_t * ossql,
                                   const xwirq_t irqs[],
                                   xwsz_t num)
{
        return xwup_sqlk_wr_trylock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_irqs(xwosdl_sqlk_t * ossql,
                                const xwirq_t irqs[],
                                xwsz_t num)
{
        xwup_sqlk_wr_unlock_irqs(ossql, irqs, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_irqssv(xwosdl_sqlk_t * ossql,
                                const xwirq_t irqs[],
                                xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_wr_lock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_irqssv(xwosdl_sqlk_t * ossql,
                                     const xwirq_t irqs[],
                                     xwreg_t flags[], xwsz_t num)
{
        return xwup_sqlk_wr_trylock_irqssv(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_irqsrs(xwosdl_sqlk_t * ossql,
                                  const xwirq_t irqs[],
                                  xwreg_t flags[], xwsz_t num)
{
        xwup_sqlk_wr_unlock_irqsrs(ossql, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_sqlk_wr_lock_bh(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_wr_lock_bh(ossql);
}

static __xwcc_inline
xwer_t xwosdl_sqlk_wr_trylock_bh(xwosdl_sqlk_t * ossql)
{
        return xwup_sqlk_wr_trylock_bh(ossql);
}

static __xwcc_inline
void xwosdl_sqlk_wr_unlock_bh(xwosdl_sqlk_t * ossql)
{
        xwup_sqlk_wr_unlock_bh(ossql);
}

#endif /* xwos/up/osdl/lock/seqlock.h */
