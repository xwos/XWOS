/**
 * @file
 * @brief 板级描述层：初始化
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <xwcd/soc/arm64/v8a/a72/bcm2711/soc_init.h>
#include <xwos/ospl/skd.h>
#include <xwos/mm/mempool/allocator.h>
#include <xwos/lib/xwaop.h>
#include "board/xwac/xwds/device.h"

extern struct xwmm_mempool mempool_allocator;
extern xwu64_t * ram_mr_origin[];
extern xwu64_t * ram_mr_size[];

/**
 * @brief 初始化内存管理
 */
__xwbsp_init_code
void board_mm_init(void)
{
        xwer_t rc;
        xwssq_t odr;

        odr = xwbop_fls(xwsz_t, (xwu64_t)ram_mr_size / XWMM_MEMPOOL_PAGE_SIZE);
        rc = xwmm_mempool_init(&mempool_allocator, "mempool",
                               (xwptr_t)ram_mr_origin,
                               (xwsz_t)ram_mr_size,
                               (xwsz_t)odr,
                               (xwsz_t)0x40000000, NULL);
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
        xwid_t cpuid;

        cpuid = xwospl_skd_get_cpuid_lc();
        switch (cpuid) {
        case 0U:
                board_mm_init();
                rpi4bxwds_init();
                rpi4bxwds_miniuart_init();
                rpi4bxwds_uart_init();
                break;
        case 1U:
                break;
        case 2U:
                break;
        case 3U:
                break;
        default:
                break;
        }
}
