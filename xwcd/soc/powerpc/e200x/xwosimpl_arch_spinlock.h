/**
 * @file
 * @brief XWOS移植实现层：ARCH自旋锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#ifndef __xwosimpl_arch_spinlock_h__
#define __xwosimpl_arch_spinlock_h__

#ifndef __xwos_ospl_spinlock_h__
#  error "This file should be included from <xwos/ospl/spinlock.h>."
#endif

struct xwospl_splk {
        xwu32_t lockval;
};

#define XWOSPL_SPLK_INITIALIZER { .lockval = 0U, }

static __xwbsp_inline
void xwospl_splk_init(struct xwospl_splk * osplsplk)
{
        osplsplk->lockval = 0U;
}

void xwospl_splk_lock(struct xwospl_splk * osplsplk);
xwer_t xwospl_splk_trylock(struct xwospl_splk * osplsplk);
void xwospl_splk_unlock(struct xwospl_splk * osplsplk);

#endif /* xwcd/soc/powerpc/e200x/xwosimpl_arch_spinlock.h */
