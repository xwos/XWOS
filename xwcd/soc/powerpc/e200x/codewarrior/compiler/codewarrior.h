/**
 * @file
 * @brief 架构描述层：mwcceppc的宏定义
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

#ifndef __compiler_codewarrior_h__
#define __compiler_codewarrior_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <cfg/XuanWuOS.h>

#ifndef __CWCC__
  #error "Not CodeWarrior!"
#endif

#ifndef __XWOS_COMPILER__
  #define __XWOS_COMPILER__  codewarrior
#endif

#pragma c99 on
#pragma gcc_extensions on
#pragma bool off
#pragma always_inline on

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define __xw_i  volatile const /**< 'read only' structure member permissions */
#define __xw_o  volatile       /**< 'write only' structure member permissions */
#define __xw_io volatile       /**< 'read/write' structure member permissions */

#define CWCC_VERSION            __VERSION__

#define __section(s)            __declspec(section #s)
#define __aligned(x)            __attribute__((aligned(x)))
#define __weak                  __declspec((weak)
#define __alias(s)              __attribute__((alias(#s)))
#define __xw_inline             inline
#define __never_inline          __attribute__((never_inline))
#define __packed                __attribute__((packed))
#define __must_check
#define __force
#define __maybe_unused          __attribute__((unused))
#define __always_unused         __attribute__((unused))
#define __used                  __attribute__((used))
#define __maybe_unused          __attribute__((unused))
#define __noreturn              __attribute__((noreturn))
#define __hot
#define __atomic                volatile
#define __likely(x)             __builtin_expect(!!(x), 1)
#define __unlikely(x)           __builtin_expect(!!(x), 0)
#if defined(CPUCFG_L1_CACHELINE_SIZE) && (1 == CPUCFG_L1_CACHELINE_SIZE)
  #define __aligned_l1cacheline \
          __aligned(CPUCFG_L1_CACHELINE_SIZE)
#else
  #define __aligned_l1cacheline         __aligned(sizeof(long))
#endif

#define __alignptr                      __aligned(sizeof(void *))

#define __xwcc_warning(message) __attribute__((warning(message)))
#define __xwcc_error(message) __attribute__((error(message)))

/******** ******** section ******** ********/
#pragma section RX ".xwos.init.text" code_mode=pc_rel
#pragma section R ".xwos.init.rodata" ".xwos.init.rodata" data_mode=far_abs
#pragma section RX ".xwos.exit.text" code_mode=pc_rel
#pragma section R ".xwos.exit.rodata" ".xwos.exit.rodata" data_mode=far_abs

#pragma section RX ".xwos.isr.text" code_mode=pc_rel
#pragma section R ".xwos.vctbl" ".xwos.vctbl" data_mode=far_abs

#pragma section RX ".xwos.text" code_mode=pc_rel
#pragma section R ".xwos.rodata" ".xwos.rodata" data_mode=far_abs
#pragma section RW ".xwos.data" ".xwos.data" data_mode=far_abs

#pragma section RX ".xwmd.text" code_mode=pc_rel
#pragma section RX ".xwmd.isr.text" code_mode=pc_rel
#pragma section R ".xwmd.rodata" ".xwmd.rodata" data_mode=far_abs
#pragma section RW ".xwmd.data" ".xwmd.data" data_mode=far_abs

#pragma section RW ".bkup" ".bkup" data_mode=far_abs
#pragma section R ".image_header" ".image_header" data_mode=far_abs
#pragma section R ".image_tail" ".image_tail" data_mode=far_abs

#pragma section RXW ".flsopc" data_mode=far_abs code_mode=far_abs

#define __xwos_init_code        __section(.xwos.init.text)
#define __xwos_init_rodata      __section(.xwos.init.rodata)
#define __xwos_exit_code        __section(.xwos.exit.text)
#define __xwos_exit_rodata      __section(.xwos.exit.rodata)

#define __xwos_vctbl            __section(.xwos.vctbl)
#define __xwos_isr              __section(.xwos.isr.text)
#define __xwos_bh               __xwos_isr
#define __xwos_code             __section(.xwos.text)
#define __xwos_api              __xwos_code
#define __xwos_rodata           __section(.xwos.rodata)
#define __xwos_data             __section(.xwos.data)

#define __xwmd_isr              __section(.xwmd.isr.text)
#define __xwmd_bh               __xwmd_isr
#define __xwmd_code             __section(.xwmd.text)
#define __xwmd_api              __xwmd_code
#define __xwmd_rodata           __section(.xwmd.rodata)
#define __xwmd_data             __section(.xwmd.data)

#define __bkup                  __section(.bkup)
#define __image_header          __section(.image_header)
#define __image_tail            __section(.image_tail)
#define __flsopc                __section(.flsopc)

#endif /* codewarrior.h */
