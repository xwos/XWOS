/**
 * @file
 * @brief 板级描述层：内存池
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

#ifndef __board_mempool_h__
#define __board_mempool_h__

#include "board/std.h"
#include <xwos/mm/mempool/allocator.h>

/**
 * @brief mempool内存池
 * @note
 * + 头文件
 *   + board/mempool.h
 * + API
 *   + 申请：xwmm_mempool_malloc(&board_mempool, ...)
 *   + 释放：xwmm_mempool_free(&board_mempool, ...)
 *   + 重新申请：xwmm_mempool_realloc(&board_mempool, ...)
 */
extern struct xwmm_mempool board_mempool;

#endif /* board/mempool.h */
