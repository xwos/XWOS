/**
 * @file
 * @brief XWRUST FFI：自旋锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>

void xwrustffi_splk_init(struct xwos_splk * splk)
{
        xwos_splk_init(splk);
}

void xwrustffi_splk_unlock(struct xwos_splk * splk)
{
        xwos_splk_unlock(splk);
}

void xwrustffi_splk_lock(struct xwos_splk * splk)
{
        xwos_splk_lock(splk);
}

xwer_t xwrustffi_splk_trylock(struct xwos_splk * splk)
{
        return xwos_splk_trylock(splk);
}

void xwrustffi_splk_unlock_cpuirq(struct xwos_splk * splk)
{
        xwos_splk_unlock_cpuirq(splk);
}

void xwrustffi_splk_lock_cpuirq(struct xwos_splk * splk)
{
        xwos_splk_lock_cpuirq(splk);
}

xwer_t xwrustffi_splk_trylock_cpuirq(struct xwos_splk * splk)
{
        return xwos_splk_trylock_cpuirq(splk);
}

void xwrustffi_splk_unlock_cpuirqrs(struct xwos_splk * splk, xwreg_t cpuirq)
{
        xwos_splk_unlock_cpuirqrs(splk, cpuirq);
}

void xwrustffi_splk_lock_cpuirqsv(struct xwos_splk * splk, xwreg_t * cpuirq)
{
        xwos_splk_lock_cpuirqsv(splk, cpuirq);
}

xwer_t xwrustffi_splk_trylock_cpuirqsv(struct xwos_splk * splk, xwreg_t * cpuirq)
{
        return xwos_splk_trylock_cpuirqsv(splk, cpuirq);
}

void xwrustffi_splk_unlock_bh(struct xwos_splk * splk)
{
        xwos_splk_unlock_bh(splk);
}

void xwrustffi_splk_lock_bh(struct xwos_splk * splk)
{
        xwos_splk_lock_bh(splk);
}

xwer_t xwrustffi_splk_trylock_bh(struct xwos_splk * splk)
{
        return xwos_splk_trylock_bh(splk);
}
