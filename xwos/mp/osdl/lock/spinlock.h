/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：自旋锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_osdl_lock_spinlock_h__
#define __xwos_mp_osdl_lock_spinlock_h__

#include <xwos/mp/osdl/irq.h>
#include <xwos/mp/lock/spinlock.h>

typedef struct xwmp_splk xwosdl_splk_t;

#define XWOSDL_SPLK_INITIALIZER XWMP_SPLK_INITIALIZER

static __xwcc_inline
void xwosdl_splk_init(xwosdl_splk_t * ossl)
{
        xwmp_splk_init(ossl);
}

static __xwcc_inline
void xwosdl_splk_lock(xwosdl_splk_t * ossl)
{
        xwmp_splk_lock(ossl);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock(xwosdl_splk_t * ossl)
{
        return xwmp_splk_trylock(ossl);
}

static __xwcc_inline
void xwosdl_splk_unlock(xwosdl_splk_t * ossl)
{
        xwmp_splk_unlock(ossl);
}

static __xwcc_inline
void xwosdl_splk_lock_cpuirq(xwosdl_splk_t * ossl)
{
        xwmp_splk_lock_cpuirq(ossl);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_cpuirq(xwosdl_splk_t * ossl)
{
        return xwmp_splk_trylock_cpuirq(ossl);
}

static __xwcc_inline
void xwosdl_splk_unlock_cpuirq(xwosdl_splk_t * ossl)
{
        xwmp_splk_unlock_cpuirq(ossl);
}

static __xwcc_inline
void xwosdl_splk_lock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * cpuirq)
{
        xwmp_splk_lock_cpuirqsv(ossl, cpuirq);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_cpuirqsv(xwosdl_splk_t * ossl, xwreg_t * cpuirq)
{
        return xwmp_splk_trylock_cpuirqsv(ossl, cpuirq);
}

static __xwcc_inline
void xwosdl_splk_unlock_cpuirqrs(xwosdl_splk_t * ossl, xwreg_t cpuirq)
{
        xwmp_splk_unlock_cpuirqrs(ossl, cpuirq);
}

static __xwcc_inline
void xwosdl_splk_lock_irqs(xwosdl_splk_t * ossl,
                           const xwirq_t irqs[], xwsz_t num)
{
        xwmp_splk_lock_irqs(ossl, irqs, num);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_irqs(xwosdl_splk_t * ossl,
                                const xwirq_t irqs[], xwsz_t num)
{
        return xwmp_splk_trylock_irqs(ossl, irqs, num);
}

static __xwcc_inline
void xwosdl_splk_unlock_irqs(xwosdl_splk_t * ossl,
                             const xwirq_t irqs[], xwsz_t num)
{
        xwmp_splk_unlock_irqs(ossl, irqs, num);
}

static __xwcc_inline
void xwosdl_splk_lock_irqssv(xwosdl_splk_t * ossl,
                             const xwirq_t irqs[],
                             xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_lock_irqssv(ossl, irqs, flags, num);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_irqssv(xwosdl_splk_t * ossl,
                                  const xwirq_t irqs[],
                                  xwreg_t flags[], xwsz_t num)
{
        return xwmp_splk_trylock_irqssv(ossl, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_splk_unlock_irqsrs(xwosdl_splk_t * ossl,
                               const xwirq_t irqs[],
                               xwreg_t flags[], xwsz_t num)
{
        xwmp_splk_unlock_irqsrs(ossl, irqs, flags, num);
}

static __xwcc_inline
void xwosdl_splk_lock_bh(xwosdl_splk_t * ossl)
{
        xwmp_splk_lock_bh(ossl);
}

static __xwcc_inline
xwer_t xwosdl_splk_trylock_bh(xwosdl_splk_t * ossl)
{
        return xwmp_splk_trylock_bh(ossl);
}

static __xwcc_inline
void xwosdl_splk_unlock_bh(xwosdl_splk_t * ossl)
{
        xwmp_splk_unlock_bh(ossl);
}

#endif /* xwos/mp/osdl/lock/spinlock.h */
