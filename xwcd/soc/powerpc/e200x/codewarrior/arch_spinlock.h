/**
 * @file
 * @brief 架构描述层：自旋锁
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

#ifndef __arch_spinlock_h__
#define __arch_spinlock_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct arch_splk {
        xwu32_t lockval;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (CPUCFG_CPU_NUM > 1)
__xwbsp_code
void arch_bitsplk_lock(void * addr);

__xwbsp_code
xwer_t arch_bitsplk_trylock(void * addr);

__xwbsp_code
void arch_bitsplk_unlock(void * addr)
#endif /*(CPUCFG_CPU_NUM > 1)*/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Initialize a bit spin lock
 */
static __xwbsp_inline
void arch_bitsplk_init(void * addr)
{
        *((xwsq_t *)addr) |= 1U;
}

/**
 * @brief Initialize a spinlock
 * @parem asl: arch spin lock
 */
static __xwbsp_inline
void arch_splk_init(struct arch_splk * asl)
{
        asl->lockval = 1;
}

#if (CPUCFG_CPU_NUM > 1)
/**
 * @brief Lock a spinlock
 * @parem asl: arch spin lock
 */
static __xwbsp_inline
void arch_splk_lock(struct arch_splk * asl)
{
        arch_bitsplk_lock(asl);
}

/**
 * @brief Try to lock a spinlock
 * @parem asl: arch spin lock
 * @retval 0: OK
 * @retval -EAGAIN: Can't acquire the lock and try again
 */
static __xwbsp_inline
xwer_t arch_splk_trylock(struct arch_splk * asl)
{
        return arch_bitsplk_trylock(asl);
}

/**
 * @brief Unlock a spinlock
 * @parem asl: arch spin lock
 */
static __xwbsp_inline
void arch_splk_unlock(__maybe_unused struct arch_splk * asl)
{
        arch_bitsplk_unlock(asl);
}
#endif /* (CPUCFG_CPU_NUM > 1) */

#endif /* arch_spinlock.h */
