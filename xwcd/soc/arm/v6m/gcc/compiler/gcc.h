/**
 * @file
 * @brief 编译器相关的宏定义
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>

#ifndef __GNUC__
  #error "Not gcc!"
#endif

#ifndef __XWOS_COMPILER__
  #define __XWOS_COMPILER__             gcc
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define __xw_i  volatile const /**< 'read only' structure member permissions */
#define __xw_o  volatile       /**< 'write only' structure member permissions */
#define __xw_io volatile       /**< 'read/write' structure member permissions */

#define GCC_VERSION             (__GNUC__ * 10000  +            \
                                 __GNUC_MINOR__ * 100 +         \
                                 __GNUC_PATCHLEVEL__)

#define __section(s)            __attribute__((__section__(s)))
#define __aligned(x)            __attribute__((aligned(x)))
#define __weak                  __attribute__((weak))
#define __weakref(x)            __attribute__((weakref(s)))
#define __alias(s)              __attribute__((alias(s)))
#define __weakalias(s)          __attribute__((weak, alias(s)))
#define __xw_inline             inline __attribute__((always_inline))
#define __pure                  __attribute__((pure))
#define __packed                __attribute__((packed))
#define __must_check            __attribute__((warn_unused_result))
#define __force
#define __naked                 __attribute__((naked))
#define __maybe_unused          __attribute__((unused))
#define __always_unused         __attribute__((unused))
#define __used                  __attribute__((used))
#define __noreturn              __attribute__((noreturn))
#define __hot                   __attribute__((hot))
#if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
  #define __atomic              _Atomic
#else
  #define __atomic              volatile
#endif
#define __likely(x)             __builtin_expect(!!(x), 1)
#define __unlikely(x)           __builtin_expect(!!(x), 0)
#if defined(CPUCFG_L1_CACHELINE_SIZE) && (1 == CPUCFG_L1_CACHELINE_SIZE)
  #define __aligned_l1cacheline \
          __aligned(CPUCFG_L1_CACHELINE_SIZE)
#else
  #define __aligned_l1cacheline __aligned(sizeof(long))
#endif

#define __alignptr              __aligned(sizeof(void *))

#define offsetof(type, member)  __builtin_offsetof(type, member)

#define __xwcc_warning(message) __attribute__((warning(message)))
#define __xwcc_error(message)   __attribute__((error(message)))

/******** ******** section ******** ********/
/* 不要在init段使用全局变量，因为init段的代码在全局变量重定向之前执行。 */
#define __xwos_init_code        __section(".xwos.init.text")
#define __xwos_init_rodata      __section(".xwos.init.rodata")
#define __xwos_exit_code        __section(".xwos.exit.text")
#define __xwos_exit_rodata      __section(".xwos.exit.rodata")

#define __xwos_vctbl            __section(".xwos.vctbl")
#define __xwos_isr              __section(".xwos.isr.text")
#define __xwos_bh               __xwos_isr
#define __xwos_code             __section(".xwos.text")
#define __xwos_api              __xwos_code
#define __xwos_rodata           __section(".xwos.rodata")
#define __xwos_data             __section(".xwos.data")

#define __xwmd_code             __section(".xwmd.text")
#define __xwmd_api              __xwmd_code
#define __xwmd_isr              __section(".xwmd.isr.text")
#define __xwmd_bh               __xwmd_isr
#define __xwmd_rodata           __section(".xwmd.rodata")
#define __xwmd_data             __section(".xwmd.data")

#define __image_description     __section(".image.description")
#define __image_tail            __section(".image.tail")

#endif /* gcc.h */
