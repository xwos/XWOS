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

#include <xwos/standard.h>
#include <bdl/axisram.h>

__xwos_code
xwer_t board_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf)
{
        union {
                xwstk_t * stk;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_mempool_malloc(axisram_mempool, stack_size, &mem.anon);
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
        return xwmm_mempool_free(axisram_mempool, stk);
}
