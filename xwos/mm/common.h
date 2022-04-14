/**
 * @file
 * @brief 玄武OS内存管理：通用定义
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_common_h__
#define __xwos_mm_common_h__

#include <xwos/standard.h>

/**
 * @defgroup xwmm_common XWOS内存管理通用定义
 * @ingroup xwmm
 * @{
 */

#define XWMM_ALIGNMENT          8U
#define XWMM_ALIGNMENT_MASK     (XWMM_ALIGNMENT - 1)

/**
 * @brief XWOS内存区域
 */
struct xwmm_zone {
        xwptr_t origin; /**< origin address */
        xwsz_t size; /**< unit: byte */
};

/**
 * @brief XWMM API：测试地址是否在内存区域内
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
static __xwcc_inline
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

/**
 * @} xwmm
 */

#endif /* xwos/mm/common.h */
