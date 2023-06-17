/**
 * @file
 * @brief XWOS内存管理：内核内存分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mm_kma_h__
#define __xwos_mm_kma_h__

#include <xwos/standard.h>

/**
 * @defgroup xwmm_kma 内核内存分配器
 * @ingroup xwmm
 * @{
 */

typedef xwer_t (* xwmm_kma_alloc_f)(xwsz_t, xwsz_t, void **);
typedef xwer_t (* xwmm_kma_free_f)(void *);

xwer_t xwmm_kma_init(void);
xwer_t xwmm_kma_alloc(xwsz_t size, xwsz_t aligned, void ** membuf);
xwer_t xwmm_kma_free(void * mem);

/**
 * @} xwmm_kma
 */

#endif /* xwos/mm/kma.h */
