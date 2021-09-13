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
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#include <bdl/xwac/thd_stack_mempool.h>

extern struct xwmm_bma * stkmempool_bma;

#if ((BRDCFG_MM_STKMEMPOOL_BLKSZ % XWMMCFG_STACK_ALIGNMENT) != 0)
  #error BRDCFG_MM_STKMEMPOOL_BLKSZ is not aligned to XWMMCFG_STACK_ALIGNMENT!
#endif

__xwos_code
xwer_t board_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf)
{
        union {
                xwstk_t * stk;
                void * anon;
        } mem;
        xwsz_t stknum;
        xwsq_t numodr;
        xwer_t rc;

        stknum = XWBOP_DIV_ROUND_UP(stack_size, BRDCFG_MM_STKMEMPOOL_BLKSZ);
        numodr = (xwsq_t)xwbop_fls(xwsz_t, stknum);
        if (stknum & (stknum - 1U)) {
                numodr++;
        }
        rc = xwmm_bma_alloc(stkmempool_bma, numodr, &mem.anon);
        if (rc < 0) {
                *membuf = NULL;
        } else {
                *membuf = mem.stk;
        }
        return rc;
}

__xwos_code
xwer_t board_thd_stack_pool_free(xwstk_t * stk)
{
        return xwmm_bma_free(stkmempool_bma, stk);
}
