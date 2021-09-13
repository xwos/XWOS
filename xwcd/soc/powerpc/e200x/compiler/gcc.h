/**
 * @file
 * @brief 架构描述层：编译器相关的宏定义
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

#ifndef __compiler_gcc_h__
#define __compiler_gcc_h__

#include <cfg/XuanWuOS.h>

#define __xw_i  volatile const /**< 'read only' structure member permissions */
#define __xw_o  volatile       /**< 'write only' structure member permissions */
#define __xw_io volatile       /**< 'read/write' structure member permissions */

#define GCC_VERSION (__GNUC__ * 10000 + __GNUC_MINOR__ * 100 + __GNUC_PATCHLEVEL__)

#define __xwcc_section(s)       __attribute__((__section__(s)))
#define __xwcc_aligned(x)       __attribute__((aligned(x)))
#define __xwcc_weak             __attribute__((weak))
#define __xwcc_weakref(x)       __attribute__((weakref(s)))
#define __xwcc_alias(s)         __attribute__((alias(s)))
#define __xwcc_weakalias(s)     __attribute__((weak, alias(s)))
#define __xwcc_inline           inline __attribute__((always_inline))
#define __xwcc_pure             __attribute__((pure))
#define __xwcc_packed           __attribute__((packed))
#define __xwcc_must_check       __attribute__((warn_unused_result))
#define __xwcc_naked            __attribute__((naked))
#define __xwcc_unused           __attribute__((unused))
#define __xwcc_used             __attribute__((used))
#define __xwcc_noreturn         __attribute__((noreturn))
#define __xwcc_hot              __attribute__((hot))
#define __xwcc_atomic           volatile
#define __xwcc_likely(x)        __builtin_expect(!!(x), 1)
#define __xwcc_unlikely(x)      __builtin_expect(!!(x), 0)
#if defined(CPUCFG_L1_CACHELINE_SIZE)
#  define __xwcc_alignl1cache   __xwcc_aligned(CPUCFG_L1_CACHELINE_SIZE)
#else
#  define __xwcc_alignl1cache   __xwcc_aligned(sizeof(void *))
#endif
#define __xwcc_alignptr         __xwcc_aligned(sizeof(void *))
#define xwcc_offsetof(type, member) __builtin_offsetof(type, member)
#define __xwcc_warning(message) __attribute__((warning(message)))
#define __xwcc_error(message)   __attribute__((error(message)))

/******** ******** section ******** ********/
/* 不要在init段使用全局变量，因为init段的代码在全局变量重定向之前执行。 */
#define __xwos_init_code        __xwcc_section(".xwos.init.text")
#define __xwos_init_rodata      __xwcc_section(".xwos.init.rodata")
#define __xwos_exit_code        __xwcc_section(".xwos.exit.text")
#define __xwos_exit_rodata      __xwcc_section(".xwos.exit.rodata")

#define __xwos_ivt              __xwcc_section(".xwos.ivt")
#define __xwos_isr              __xwcc_section(".xwos.isr.text")
#define __xwos_bh               __xwos_isr
#define __xwos_code             __xwcc_section(".xwos.text")
#define __xwos_api              __xwos_code
#define __xwos_rodata           __xwcc_section(".xwos.rodata")
#define __xwos_data             __xwcc_section(".xwos.data")

#define __bkup                  __xwcc_section(".bkup")
#define __image_header          __xwcc_section(".image_header")
#define __image_tail            __xwcc_section(".image_tail")
#define __flsopc                __xwcc_section(".flsopc")

/******** ******** barrier ******** ********/
#define xwccmb()                __asm__ volatile("": : :"memory")
#define eppc_isb(option)        __asm__ volatile("se_isync" : : : "memory")
#define eppc_dmb(option)        __asm__ volatile("msync" : : : "memory")

#define xwmb_mp_mb()            eppc_dmb()
#define xwmb_mp_rmb()           xwmb_mp_mb()
#define xwmb_mp_wmb()           __asm__ volatile("mbar      0" : : : "memory")
#define xwmb_mp_ddb()           xwccmb()

#define xwmb_mp_load_acquire_mb()       xwmb_mp_mb()
#define xwmb_mp_store_release_mb()      xwmb_mp_mb()

#endif /* gcc.h */