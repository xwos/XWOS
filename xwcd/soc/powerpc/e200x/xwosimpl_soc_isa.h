/**
 * @file
 * @brief XWOS移植实现层：SOC的架构指令
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __xwcd_soc_powerpc_e200x_xwosimpl_soc_isa_h__
#define __xwcd_soc_powerpc_e200x_xwosimpl_soc_isa_h__

#ifndef __xwos_ospl_soc_isa_h__
#  error "This file should be included from <xwos/ospl/soc/isa.h>."
#endif

#include <xwos/ospl/soc/compiler.h>

/******** ******** Bug report ******** ********/

/******** ******** barrier ******** ********/
#define eppc_isb()              __asm__ volatile("se_isync" : : : "memory")
#define eppc_dmb()              __asm__ volatile("msync" : : : "memory")

#define xwmb_compiler()         __asm__ volatile("": : :"memory")
#define xwmb_isb()              eppc_isb()
#define xwmb_ddb()              xwmb_compiler()

#define xwmb_mb()               eppc_dmb()
#define xwmb_rmb()              eppc_dmb()
#define xwmb_wmb()              eppc_dmb()

#define xwmb_dma_mb()           eppc_dmb()
#define xwmb_dma_rmb()          eppc_dmb()
#define xwmb_dma_wmb()          eppc_dmb()

#define xwmb_mp_mb()            eppc_dmb()
#define xwmb_mp_rmb()           eppc_dmb()
#define xwmb_mp_wmb()           __asm__ volatile("mbar  0" : : : "memory")
#define xwmb_mp_acquire()       xwmb_mp_rmb()
#define xwmb_mp_release()       xwmb_mp_wmb()

#endif /* xwcd/soc/powerpc/e200x/xwosimpl_soc_isa.h */
