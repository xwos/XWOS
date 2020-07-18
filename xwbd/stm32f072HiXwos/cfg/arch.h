/**
 * @file
 * @brief 架构描述层(ADL)配置
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

#ifndef __cfg_arch_h__
#define __cfg_arch_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      architecture specification     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCHCFG_LITTLE_ENDIAN                   1
#define ARCHCFG_BIG_ENDIAN                      0
#define ARCHCFG_16BIT                           0
#define ARCHCFG_32BIT                           1
#define ARCHCFG_64BIT                           0

/******** ******** ******** interrupt ******** ******** ********/
#define ARCHCFG_IRQ_NUM                         16

/******** ******** fpu ******** ********/
#define ARCHCFG_FPU                             0

/******** ******** architecture libs ******** ********/
/******** libc.a ********/
#define ARCHCFG_COMPILER_CLIB                   1
#define ARCHCFG_COMPILER_ERRNO                  1

/******** C++ ********/
#define ARCHCFG_CXX                             0

/******** xwbop ********/
#define ARCHCFG_LIB_XWBOP_S1M8                  0
#define ARCHCFG_LIB_XWBOP_C0M8                  0
#define ARCHCFG_LIB_XWBOP_X1M8                  0
#define ARCHCFG_LIB_XWBOP_FFS8                  0
#define ARCHCFG_LIB_XWBOP_FLS8                  0
#define ARCHCFG_LIB_XWBOP_RBIT8                 0

#define ARCHCFG_LIB_XWBOP_S1M16                 0
#define ARCHCFG_LIB_XWBOP_C0M16                 0
#define ARCHCFG_LIB_XWBOP_X1M16                 0
#define ARCHCFG_LIB_XWBOP_FFS16                 0
#define ARCHCFG_LIB_XWBOP_FLS16                 0
#define ARCHCFG_LIB_XWBOP_RBIT16                0
#define ARCHCFG_LIB_XWBOP_RE16                  1
#define ARCHCFG_LIB_XWBOP_RE16S32               1

#define ARCHCFG_LIB_XWBOP_S1M32                 0
#define ARCHCFG_LIB_XWBOP_C0M32                 0
#define ARCHCFG_LIB_XWBOP_X1M32                 0
#define ARCHCFG_LIB_XWBOP_FFS32                 0
#define ARCHCFG_LIB_XWBOP_FLS32                 0
#define ARCHCFG_LIB_XWBOP_RBIT32                0
#define ARCHCFG_LIB_XWBOP_RE32                  1
#define ARCHCFG_LIB_XWBOP_RE32S64               1

#define ARCHCFG_LIB_XWBOP_S1M64                 0
#define ARCHCFG_LIB_XWBOP_C0M64                 0
#define ARCHCFG_LIB_XWBOP_X1M64                 0
#define ARCHCFG_LIB_XWBOP_FFS64                 0
#define ARCHCFG_LIB_XWBOP_FLS64                 0
#define ARCHCFG_LIB_XWBOP_RBIT64                0
#define ARCHCFG_LIB_XWBOP_RE64                  1

#define ARCHCFG_LIB_XWBMPOP_FFS                 0
#define ARCHCFG_LIB_XWBMPOP_FFZ                 0
#define ARCHCFG_LIB_XWBMPOP_FLS                 0
#define ARCHCFG_LIB_XWBMPOP_FLZ                 0
#define ARCHCFG_LIB_XWBMPOP_S1I                 0
#define ARCHCFG_LIB_XWBMPOP_C0I                 0
#define ARCHCFG_LIB_XWBMPOP_X1I                 0
#define ARCHCFG_LIB_XWBMPOP_T1I                 0

/******** atomic operations ********/
#define ARCHCFG_LIB_XWAOP8                      1
#define ARCHCFG_LIB_XWAOP16                     1
#define ARCHCFG_LIB_XWAOP32                     1
#define ARCHCFG_LIB_XWAOP64                     1
#define ARCHCFG_LIB_XWBMPAOP                    0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     image flag    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCHCFG_IMAGE_FLAG_OFFSET               512U
#define ARCHCFG_IMAGE_TAILFLAG                  "XuanWuOS"

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         default configures          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if (((!defined(ARCHCFG_BIG_ENDIAN)) || (1 != ARCHCFG_BIG_ENDIAN)) && \
     ((!defined(ARCHCFG_LITTLE_ENDIAN)) || (1 != ARCHCFG_LITTLE_ENDIAN)))
  #error "Bit-endian is not defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 != ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_32BIT) && (1 != ARCHCFG_32BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 != ARCHCFG_16BIT)))
  #error "ARCH-bits is not defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 == ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_32BIT) && (1 == ARCHCFG_32BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 == ARCHCFG_16BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#if ((defined(ARCHCFG_32BIT) && (1 == ARCHCFG_32BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 == ARCHCFG_16BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 == ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_16BIT) && (1 == ARCHCFG_16BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#if ((defined(ARCHCFG_64BIT) && (1 == ARCHCFG_64BIT)) && \
     (defined(ARCHCFG_32BIT) && (1 == ARCHCFG_32BIT)))
  #error "ARCH-bits is multi-defined!"
#endif

#endif /* cfg/arch.h */
