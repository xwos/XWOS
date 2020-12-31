/**
 * @file
 * @brief 玄武OS内核适配代码：线程栈内存池
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

#ifndef __bdl_xwac_thd_stack_mempool_h__
#define __bdl_xwac_thd_stack_mempool_h__

#include <xwos/standard.h>

xwer_t bdl_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);
xwer_t bdl_thd_stack_pool_free(xwstk_t * stk);

#endif /* bdl/xwac/thd_stack_mempool.h */