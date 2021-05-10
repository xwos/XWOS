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

#include <cfg/XuanWuOS.h>
#include <compiler/gcc.h>

/******** ******** barrier ******** ********/
#define xwccmb()                __asm__ volatile("": : :"memory")
#define armv6m_isb()            __asm__ volatile("isb" : : : "memory")
#define armv6m_dsb()            __asm__ volatile("dsb" : : : "memory")
#define armv6m_dmb()            __asm__ volatile("dmb" : : : "memory")

#define xwmb_mp_isb()           armv6m_isb()
#define xwmb_mp_mb()            armv6m_dmb()
#define xwmb_mp_rmb()           xwmb_mp_mb()
#define xwmb_mp_wmb()           armv6m_dmb()
#define xwmb_mp_ddb()           xwccmb()

#define xwmb_mp_load_acquire_mb()       xwmb_mp_mb()
#define xwmb_mp_store_release_mb()      xwmb_mp_mb()

#endif /* compiler/barrier.h */
