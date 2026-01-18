/**
 * @file
 * @brief XWOS内存管理：内存池：分配器接口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_mempool_i_allocator_h__
#define __xwos_mm_mempool_i_allocator_h__

#include <xwos/standard.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_mempool 内存池分配器
 * @{
 */

/**
 * @brief interface：分配器
 */
interface xwmm_mempool_i_allocator {
        // cppcheck-suppress [misra-c2012-5.8]
        xwer_t (* malloc)(void *, xwsz_t, void **);
        // cppcheck-suppress [misra-c2012-5.8]
        xwer_t (* free)(void *, void *);
};

/**
 * @} xwmm_mempool
 */

#endif /* xwos/mm/mempool/i_allocator.h */
