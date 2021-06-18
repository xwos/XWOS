/**
 * @file
 * @brief STM32CUBE模块：Log
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
#include <bm/stm32cube/xwac/xwds/cmif.h>

xwer_t board_log_write(const char * s, xwsz_t * n)
{
        xwtm_t desired;
        xwer_t rc;

        desired = XWTM_MAX;
        rc = xwds_dmauartc_tx(&stm32cube_usart1_cb,
                              (const xwu8_t *)s, n,
                              &desired);
        return rc;
}
