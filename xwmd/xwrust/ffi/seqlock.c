/**
 * @file
 * @brief XWRUST FFI：顺序锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/lock/seqlock.h>

void xwrustffi_sqlk_init(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_init(sqlk);
}

xwsq_t xwrustffi_sqlk_rd_begin(struct xwos_sqlk * sqlk)
{
        return xwos_sqlk_rd_begin(sqlk);
}

bool xwrustffi_sqlk_rd_retry(struct xwos_sqlk * sqlk, xwsq_t start)
{
        return xwos_sqlk_rd_retry(sqlk, start);
}

void xwrustffi_sqlk_rdex_unlock(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_rdex_unlock(sqlk);
}

void xwrustffi_sqlk_rdex_lock(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_rdex_lock(sqlk);
}

xwer_t xwrustffi_sqlk_rdex_trylock(struct xwos_sqlk * sqlk)
{
        return xwos_sqlk_rdex_trylock(sqlk);
}

void xwrustffi_sqlk_rdex_unlock_cpuirq(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_rdex_unlock_cpuirq(sqlk);
}

void xwrustffi_sqlk_rdex_lock_cpuirq(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_rdex_lock_cpuirq(sqlk);
}

xwer_t xwrustffi_sqlk_rdex_trylock_cpuirq(struct xwos_sqlk * sqlk)
{
        return xwos_sqlk_rdex_trylock_cpuirq(sqlk);
}

void xwrustffi_sqlk_rdex_unlock_cpuirqrs(struct xwos_sqlk * sqlk, xwreg_t cpuirq)
{
        xwos_sqlk_rdex_unlock_cpuirqrs(sqlk, cpuirq);
}

void xwrustffi_sqlk_rdex_lock_cpuirqsv(struct xwos_sqlk * sqlk, xwreg_t * cpuirq)
{
        xwos_sqlk_rdex_lock_cpuirqsv(sqlk, cpuirq);
}

xwer_t xwrustffi_sqlk_rdex_trylock_cpuirqsv(struct xwos_sqlk * sqlk, xwreg_t * cpuirq)
{
        return xwos_sqlk_rdex_trylock_cpuirqsv(sqlk, cpuirq);
}

void xwrustffi_sqlk_rdex_unlock_bh(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_rdex_unlock_bh(sqlk);
}

void xwrustffi_sqlk_rdex_lock_bh(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_rdex_lock_bh(sqlk);
}

xwer_t xwrustffi_sqlk_rdex_trylock_bh(struct xwos_sqlk * sqlk)
{
        return xwos_sqlk_rdex_trylock_bh(sqlk);
}

void xwrustffi_sqlk_wr_unlock(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_wr_unlock(sqlk);
}

void xwrustffi_sqlk_wr_lock(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_wr_lock(sqlk);
}

xwer_t xwrustffi_sqlk_wr_trylock(struct xwos_sqlk * sqlk)
{
        return xwos_sqlk_wr_trylock(sqlk);
}

void xwrustffi_sqlk_wr_unlock_cpuirq(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_wr_unlock_cpuirq(sqlk);
}

void xwrustffi_sqlk_wr_lock_cpuirq(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_wr_lock_cpuirq(sqlk);
}

xwer_t xwrustffi_sqlk_wr_trylock_cpuirq(struct xwos_sqlk * sqlk)
{
        return xwos_sqlk_wr_trylock_cpuirq(sqlk);
}

void xwrustffi_sqlk_wr_unlock_cpuirqrs(struct xwos_sqlk * sqlk, xwreg_t cpuirq)
{
        xwos_sqlk_wr_unlock_cpuirqrs(sqlk, cpuirq);
}

void xwrustffi_sqlk_wr_lock_cpuirqsv(struct xwos_sqlk * sqlk, xwreg_t * cpuirq)
{
        xwos_sqlk_wr_lock_cpuirqsv(sqlk, cpuirq);
}

xwer_t xwrustffi_sqlk_wr_trylock_cpuirqsv(struct xwos_sqlk * sqlk, xwreg_t * cpuirq)
{
        return xwos_sqlk_wr_trylock_cpuirqsv(sqlk, cpuirq);
}

void xwrustffi_sqlk_wr_unlock_bh(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_wr_unlock_bh(sqlk);
}

void xwrustffi_sqlk_wr_lock_bh(struct xwos_sqlk * sqlk)
{
        xwos_sqlk_wr_lock_bh(sqlk);
}

xwer_t xwrustffi_sqlk_wr_trylock_bh(struct xwos_sqlk * sqlk)
{
        return xwos_sqlk_wr_trylock_bh(sqlk);
}
