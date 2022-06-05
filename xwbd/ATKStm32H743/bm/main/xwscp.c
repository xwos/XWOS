/**
 * @file
 * @brief 主模块：xwscp子模块
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

#include <xwos/standard.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/mif.h>
#include <xwmd/isc/xwscp/hwif/uart.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/xwscp.h>

struct xwscp bm_xwscp;

XWSCP_DEF_MEMPOOL(bm_xwscp_mem);

xwer_t bm_xwscp_start(void)
{
        xwer_t rc;

        rc = xwscp_start(&bm_xwscp, "bm.xwscp",
                         &xwscpif_uart_ops, &stm32usart2,
                         bm_xwscp_mem, sizeof(bm_xwscp_mem));
        return rc;
}

xwer_t bm_xwscp_stop(void)
{
        xwer_t rc;

        rc = xwscp_stop(&bm_xwscp);
        return rc;
}
