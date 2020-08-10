/**
 * @file
 * @brief 架构描述层：内存屏障
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

#ifndef __compiler_barrier_h__
#define __compiler_barrier_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>
#include <xwos/type.h>
#include <compiler/gcc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** barrier ******** ********/
#define xwccmb()                __asm__ volatile("": : :"memory")
#define eppc_isb(option)        __asm__ volatile("se_isync" : : : "memory")
#define eppc_dmb(option)        __asm__ volatile("msync" : : : "memory")

#define xwmb_smp_mb()           eppc_dmb()
#define xwmb_smp_rmb()          xwmb_smp_mb()
#define xwmb_smp_wmb()          __asm__ volatile("mbar      0" : : : "memory")

#define __raw_read8(a)          (*(volatile xwu8_t *)(a))
#define __raw_read16(a)         (*(volatile xwu16_t *)(a))
#define __raw_read32(a)         (*(volatile xwu32_t *)(a))
#define __raw_read64(a)         (*(volatile xwu64_t *)(a))

#define __raw_write8(a, v)      (*(volatile unsigned char *)(a) = (v))
#define __raw_write16(a, v)     (*(volatile unsigned short *)(a) = (v))
#define __raw_write32(a, v)     (*(volatile unsigned int *)(a) = (v))
#define __raw_write64(a, v)     (*(volatile unsigned long long *)(a) = (v))

#endif /* barrier.h */
