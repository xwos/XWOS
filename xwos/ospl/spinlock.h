/**
 * @file
 * @brief XWOS移植层：Spinlock
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_spinlock_h__
#define __xwos_ospl_spinlock_h__

#include <xwos/standard.h>
#if defined(SOCCFG_LIB_SPINLOCK) && (1 == SOCCFG_LIB_SPINLOCK)
#  include <xwosimpl_soc_spinlock.h>
#elif defined(ARCHCFG_LIB_SPINLOCK) && (1 == ARCHCFG_LIB_SPINLOCK)
#  include <xwosimpl_arch_spinlock.h>
#else
#  error "Can't find LFQ configurations! SOCCFG_LIB_SPINLOCK or ARCHCFG_LIB_SPINLOCK."
#endif

struct xwospl_splk;

void xwospl_splk_init(struct xwospl_splk * socsplk);
void xwospl_splk_lock(struct xwospl_splk * socsplk);
xwer_t xwospl_splk_trylock(struct xwospl_splk * socsplk);
void xwospl_splk_unlock(struct xwospl_splk * socsplk);

#endif /* xwos/ospl/soc/spinlock.h */
