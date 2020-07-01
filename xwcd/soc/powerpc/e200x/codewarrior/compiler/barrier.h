/**
 * @file
 * @brief barrier of cwcc
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
#include <compiler/codewarrior.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** barrier ******** ********/
#define xwccmb()                asm volatile("": : :"memory")
#define eppc_isb(option)        asm volatile("se_isync" : : : "memory")
#define eppc_dmb(option)        asm volatile("msync" : : : "memory")

#define xwmb_smp_mb()           eppc_dmb()
#define xwmb_smp_rmb()          xwmb_smp_mb()
#define xwmb_smp_wmb()          asm volatile("mbar      0" : : : "memory")

#endif /* barrier.h */
