/**
 * @file
 * @brief XuanWuOS的内存管理机制：内存管理核心定义
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
 * @brief memory management zone
 */
struct xwmm_zone {
        xwptr_t origin; /**< origin address */
        xwsz_t size; /**< unit: byte */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWMM API：测试地址是否在内存区域内
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
static __xw_inline
bool xwmm_in_zone(void * mem, xwptr_t origin, xwsz_t size)
{
        xwptr_t memptr = (xwptr_t)mem;
        bool ret;

        if (memptr < origin) {
                ret = false;
        } else if (memptr > (origin + size)) {
                ret = false;
        } else {
                ret = true;
        }
        return ret;
}

#endif /* xwos/mm/common.h */
