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

#define xwccmb()                __asm__ volatile("": : :"memory")
#define xwmb_mp_isb()           __asm__ volatile("fence.i" : : : "memory")
#define xwmb_mp_mb()            __asm__ volatile("fence iorw, iorw" : : : "memory")
#define xwmb_mp_rmb()           __asm__ volatile("fence ir, ir" : : : "memory")
#define xwmb_mp_wmb()           __asm__ volatile("fence ow, ow" : : : "memory")
#define xwmb_mp_ddb()           xwccmb()

#define xwmb_mp_load_acquire_mb()  __asm__ volatile("fence r, rw" : : : "memory")
#define xwmb_mp_store_release_mb() __asm__ volatile("fence rw, w" : : : "memory")

#endif /* barrier.h */
