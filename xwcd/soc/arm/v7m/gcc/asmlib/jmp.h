/**
 * @file
 * @brief 汇编库：setjmp/longjmp
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

#ifndef __asmlib_jmp_h__
#define __asmlib_jmp_h__

#include <xwos/standard.h>

#if defined(ARCHCFG_FPU) && (1 == ARCHCFG_FPU)
  #define ARCH_XWJMPBUF_DEPTH 32 /* 16 FP Frame + 10 Basic Frame + 6 reserved */
#else
  #define ARCH_XWJMPBUF_DEPTH 16 /* 10 Basic Frame + 6 reserved */
#endif

typedef xwstk_t arch_xwjmpbuf_b[ARCH_XWJMPBUF_DEPTH];

#endif /* asmlib/jmp.h */
