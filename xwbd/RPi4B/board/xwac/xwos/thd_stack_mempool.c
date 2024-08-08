/**
 * @file
 * @brief 板级描述层：XWOS适配层：内核：线程栈内存池
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

#include "board/std.h"
#include "board/mempool.h"

__xwos_code
xwer_t board_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf)
{
        union {
                xwstk_t * stk;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_malloc(&board_mempool, stack_size, &mem.anon);
        if (XWOK == rc) {
                *membuf = mem.stk;
        } else {
                *membuf = NULL;
        }
        return rc;
}

__xwos_code
xwer_t board_thd_stack_pool_free(xwstk_t * stk)
{
        return xwmm_mempool_free(&board_mempool, stk);
}
