/**
 * @file
 * @brief XWOS移植实现层：SOC：架构指令
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

#ifndef __xwcd_soc_arm_v8a_xwosimpl_soc_isa_h__
#define __xwcd_soc_arm_v8a_xwosimpl_soc_isa_h__

#ifndef __xwos_ospl_soc_isa_h__
#  error "This file should be included from <xwos/ospl/soc/isa.h>."
#endif

#include <xwos/ospl/soc/compiler.h>

/******** ******** break point ******** ********/
extern void soc_panic(const char * fmt, ...);
#define soc_bug() soc_panic("%s:%s():%d\n\r", __FILE__, __FUNCTION__, __LINE__)

/******** ******** barrier ******** ********/
#define armv8a_isb()            __asm__ volatile("isb" : : : "memory")
#define armv8a_dsb(opt)         __asm__ volatile("dsb " #opt : : : "memory")
#define armv8a_dmb(opt)         __asm__ volatile("dmb " #opt : : : "memory")

#define xwmb_compiler()         __asm__ volatile("": : :"memory")
#define xwmb_isb()              armv8a_isb()
#define xwmb_ddb()              xwmb_compiler()

#define xwmb_mb()               armv8a_dsb(sy)
#define xwmb_rmb()              armv8a_dsb(ld)
#define xwmb_wmb()              armv8a_dsb(st)

/* 参考文档《Learn the architecture - ARMv8 memory systems》，
 * + `簇，GPU` 这些属于Inner shareable domain，内存屏障使用 `ish` 即可；
 * + `DMA` 属于Outer shareable domain，内存屏障需要使用 `osh` 。
 */
#define xwmb_dma_mb()           armv8a_dmb(osh)
#define xwmb_dma_rmb()          armv8a_dmb(oshld)
#define xwmb_dma_wmb()          armv8a_dmb(oshst)

#define xwmb_mp_mb()            armv8a_dmb(ish)
#define xwmb_mp_rmb()           armv8a_dmb(ishld)
#define xwmb_mp_wmb()           armv8a_dmb(ishst)
#define xwmb_mp_acquire()       xwmb_mp_rmb()
#define xwmb_mp_release()       xwmb_mp_wmb()

#endif /* xwcd/soc/arm64/v8a/xwosimpl_soc_isa.h */
