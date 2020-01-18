/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存管理核心定义
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mm_common_h__
#define __xwos_mm_common_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMM_ALIGNMENT          XWMMCFG_ALIGNMENT
#define XWMM_ALIGNMENT_MASK     (XWMM_ALIGNMENT - 1)

#if (XWMM_ALIGNMENT & XWMM_ALIGNMENT_MASK)
  #error "XWMMCFG_ALIGNMENT must be the order of 2!"
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief memory allocating flag enumerations
 */
enum xwmm_flag_em {
        XWMM_FLAG_UNKNOEN = 0,
        XWMM_FLAG_ATOMIC = BIT(0),
        XWMM_FLAG_NORMAL = BIT(1),
        XWMM_FLAG_CRITICAL = BIT(2),
};
/**
 * @brief memory management zone
 */
struct xwmm_zone {
        xwptr_t origin; /**< origin address */
        xwsz_t size; /**< unit: byte */
};

#endif /* xwos/mm/common.h */
