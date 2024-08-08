/**
 * @file
 * @brief 板级描述层：初始化
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
#include <xwcd/soc/arm64/v8a/a76_a55/uis7870/soc_init.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwos/lib/xwaop.h>
#include "board/xwac/xwds/device.h"

extern void * mempool_mr_origin[];

#define BOARD_MEMPOOL_MR_SIZE (8U * 1024U * 1024U)
#define BOARD_MEMPOOL_MR_ORDER (11U)

__xwbsp_data
XWMM_MEMPOOL_DEF(board_mempool, BOARD_MEMPOOL_MR_ORDER);

/**
 * @brief 初始化内存管理
 */
__xwbsp_init_code
void board_mm_init(void)
{
        xwer_t rc;

        rc = xwmm_mempool_init((struct xwmm_mempool *)&board_mempool, "mempool",
                               (xwptr_t)mempool_mr_origin,
                               (xwsz_t)BOARD_MEMPOOL_MR_SIZE,
                               (xwsz_t)BOARD_MEMPOOL_MR_ORDER,
                               (xwsz_t)0, NULL);
        BOARD_BUG_ON(rc < 0);
}

/**
 * @brief XWOS预初始化
 */
__xwbsp_init_code
void xwos_preinit(void)
{
        soc_init();
}

/**
 * @brief XWOS后初始化
 */
__xwbsp_init_code
void xwos_postinit(void)
{
        board_mm_init();
        rpi4bxwds_init();
        rpi4bxwds_uart_init();
}
