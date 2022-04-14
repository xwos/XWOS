/**
 * @file
 * @brief 玄武OS内存管理：简单内存分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_sma_h__
#define __xwos_mm_sma_h__

#include <xwos/standard.h>
#include <xwos/mm/common.h>

/**
 * @defgroup xwmm_sma 简单内存分配器
 * @ingroup xwmm
 * 此算法是在所有上下文（线程、中断、中断底半部）都是安全的。
 * @{
 */

/**
 * @brief 简单内存分配器
 */
struct xwmm_sma {
        struct xwmm_zone zone; /**< 内存区域 */
        const char * name; /**< 名字 */
        atomic_xwsq_t pos; /**< 当前位置指针 */
};

xwer_t xwmm_sma_init(struct xwmm_sma * sa,
                     xwptr_t origin, xwsz_t size, xwsq_t pos,
                     const char * name);
xwer_t xwmm_sma_alloc(struct xwmm_sma * sa, xwsz_t size, xwsz_t aligned,
                      void ** membuf);
xwer_t xwmm_sma_free(struct xwmm_sma * sa, void * mem);

/**
 * @} xwmm_sma
 */

#endif /* xwos/mm/sma.h */
