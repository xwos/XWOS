/**
 * @file
 * @brief 板级描述层：XWOS适配层：库：日志
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
#include <xwos/osal/skd.h>
#include <xwos/osal/thd.h>
#include <bm/Stm32Hal/mi.h>

xwer_t board_log_write(const char * s, xwsz_t * n)
{
        xwsq_t ctx;
        xwer_t rc;

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_BOOT == ctx) {
                rc = -EPERM;
        } else if (XWOS_SKD_CONTEXT_THD == ctx) {
                if (xwos_cthd_shld_frz()) {
                        rc = xwds_uartc_etx(&stm32xwds_usart1, (const xwu8_t *)s, n);
                } else {
                        rc = xwds_uartc_tx(&stm32xwds_usart1, (const xwu8_t *)s, n,
                                           XWTM_MAX);
                }
        } else {
                rc = xwds_uartc_etx(&stm32xwds_usart1, (const xwu8_t *)s, n);
        }
        return rc;
}
