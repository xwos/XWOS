/**
 * @file
 * @brief 主模块：xwpcp子模块
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
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwpcp/mif.h>
#include <xwmd/isc/xwpcp/hwif/uart.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/xwpcp.h>

struct xwpcp bm_xwpcp;

XWPCP_DEF_MEMPOOL(bm_xwpcp_mem);

xwer_t bm_xwpcp_start(void)
{
        xwer_t rc;

        rc = xwpcp_start(&bm_xwpcp, "bm.xwpcp",
                         &xwpcpif_uart_ops, &stm32cube_usart2_cb,
                         bm_xwpcp_mem, sizeof(bm_xwpcp_mem));
        return rc;
}

xwer_t bm_xwpcp_stop(void)
{
        xwer_t rc;

        rc = xwpcp_stop(&bm_xwpcp);
        return rc;
}
