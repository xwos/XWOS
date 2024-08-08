/**
 * @file
 * @brief 架构描述层(ADL)配置
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

#ifndef __cfg_arch_h__
#define __cfg_arch_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      architecture specification     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define ARCHCFG_LITTLE_ENDIAN                   1
#define ARCHCFG_BIG_ENDIAN                      0
#define ARCHCFG_16BIT                           0
#define ARCHCFG_32BIT                           0
#define ARCHCFG_64BIT                           1

/******** ******** GIC ******** ********/
#define ARCHCFG_GICV2                           1
#define ARCHCFG_GICV3                           0

/******** ******** architecture libs ******** ********/
/******** libc.a ********/
#define ARCHCFG_COMPILER_ERRNO                  1

/******** xwbop ********/
#define ARCHCFG_LIB_XWBOP_FFS8                  1
#define ARCHCFG_LIB_XWBOP_FLS8                  1
#define ARCHCFG_LIB_XWBOP_RBIT8                 1
#define ARCHCFG_LIB_XWBOP_WEIGHT8               0

#define ARCHCFG_LIB_XWBOP_FFS16                 1
#define ARCHCFG_LIB_XWBOP_FLS16                 1
#define ARCHCFG_LIB_XWBOP_RBIT16                1
#define ARCHCFG_LIB_XWBOP_RE16                  1
#define ARCHCFG_LIB_XWBOP_WEIGHT16              0

#define ARCHCFG_LIB_XWBOP_FFS32                 1
#define ARCHCFG_LIB_XWBOP_FLS32                 1
#define ARCHCFG_LIB_XWBOP_RBIT32                1
#define ARCHCFG_LIB_XWBOP_RE32                  1
#define ARCHCFG_LIB_XWBOP_WEIGHT32              0

#define ARCHCFG_LIB_XWBOP_FFS64                 1
#define ARCHCFG_LIB_XWBOP_FLS64                 1
#define ARCHCFG_LIB_XWBOP_RBIT64                1
#define ARCHCFG_LIB_XWBOP_RE64                  1
#define ARCHCFG_LIB_XWBOP_WEIGHT64              0

#define ARCHCFG_LIB_XWBMPOP_FFS                 1
#define ARCHCFG_LIB_XWBMPOP_FFZ                 1
#define ARCHCFG_LIB_XWBMPOP_FLS                 1
#define ARCHCFG_LIB_XWBMPOP_FLZ                 1
#define ARCHCFG_LIB_XWBMPOP_S1I                 0
#define ARCHCFG_LIB_XWBMPOP_C0I                 0
#define ARCHCFG_LIB_XWBMPOP_X1I                 0
#define ARCHCFG_LIB_XWBMPOP_T1I                 0

/******** xwaop ********/
#define ARCHCFG_LIB_XWAOP8                      1
#define ARCHCFG_LIB_XWAOP16                     1
#define ARCHCFG_LIB_XWAOP32                     1
#define ARCHCFG_LIB_XWAOP64                     1
#define ARCHCFG_LIB_XWBMPAOP                    1

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
