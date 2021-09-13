/**
 * @file
 * @brief 玄武OS移植实现层：SOC自旋锁
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

#ifndef __xwosimpl_soc_spinlock_h__
#define __xwosimpl_soc_spinlock_h__

#ifndef __xwos_ospl_soc_spinlock_h__
  #error "This file should be included from <xwos/ospl/soc/spinlock.h>."
#endif

struct soc_splk {
        xwu32_t lockval;
};

static __xwbsp_inline
void soc_splk_init(struct soc_splk * socsplk)
{
        socsplk->lockval = 1;
}

void soc_splk_lock(struct soc_splk * socsplk);
xwer_t soc_splk_trylock(struct soc_splk * socsplk);
void soc_splk_unlock(struct soc_splk * socsplk);

#endif /* xwosimpl_soc_spinlock.h */
