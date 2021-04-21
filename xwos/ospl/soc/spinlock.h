/**
 * @file
 * @brief 玄武OS移植层：SOC SPINLOCK
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_soc_spinlock_h__
#define __xwos_ospl_soc_spinlock_h__

#include <xwos/standard.h>

struct soc_splk;

static __xwbsp_inline
void soc_splk_init(struct soc_splk * socsplk);

void soc_splk_lock(struct soc_splk * socsplk);
xwer_t soc_splk_trylock(struct soc_splk * socsplk);
void soc_splk_unlock(struct soc_splk * socsplk);

#include <xwosimpl_soc_spinlock.h>

#endif /* xwos/ospl/soc/spinlock.h */
