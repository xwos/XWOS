/**
 * @file
 * @brief 内存屏障
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#define armv7m_isb()            __asm__ volatile("isb" : : : "memory")
#define armv7m_dsb()            __asm__ volatile("dsb" : : : "memory")
#define armv7m_dmb()            __asm__ volatile("dmb" : : : "memory")

#define xwmb_smp_isb()          armv7m_isb()
#define xwmb_smp_mb()           armv7m_dmb()
#define xwmb_smp_rmb()          xwmb_smp_mb()
#define xwmb_smp_wmb()          armv7m_dmb()
#define xwmb_smp_ddb()          xwccmb()

#define xwmb_read8(a)           (*(volatile xwu8_t *)(a))
#define xwmb_read16(a)          (*(volatile xwu16_t *)(a))
#define xwmb_read32(a)          (*(volatile xwu32_t *)(a))
#define xwmb_read64(a)          (*(volatile xwu64_t *)(a))

#define xwmb_write8(a, v)       (*(volatile unsigned char *)(a) = (v))
#define xwmb_write16(a, v)      (*(volatile unsigned short *)(a) = (v))
#define xwmb_write32(a, v)      (*(volatile unsigned int *)(a) = (v))
#define xwmb_write64(a, v)      (*(volatile unsigned long long *)(a) = (v))

#endif /* barrier.h */
