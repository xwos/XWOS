/**
 * @file
 * @brief 主模块：xwssc子模块
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
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mif.h>
#include <xwmd/isc/xwssc/hwif/uart.h>
#include <bm/stm32cube/mif.h>
#include <bm/main/xwssc.h>

#define BM_XWSSC_PORT_RAMCODE 0

XWSSC_DEF_MEMPOOL(bm_xwssc_mem);
struct xwssc bm_xwssc;

xwer_t bm_xwssc_start(void)
{
        xwer_t rc;

        rc = xwssc_start(&bm_xwssc, "bm.xwssc",
                         &xwsscif_uart_ops, &stm32usart3,
                         bm_xwssc_mem, sizeof(bm_xwssc_mem));
        return rc;
}

xwer_t bm_xwssc_stop(void)
{
        xwer_t rc;

        rc = xwssc_stop(&bm_xwssc);
        return rc;
}


xwer_t bm_xwssc_tx(const xwu8_t data[], xwsz_t * size, xwtm_t to)
{
        return xwssc_tx(&bm_xwssc,
                        data, size,
                        XWSSC_MAX_PRI, BM_XWSSC_PORT_RAMCODE, XWSSC_MSG_QOS_3,
                        to);
}

xwer_t bm_xwssc_rx(xwu8_t rxbuf[], xwsz_t * size, xwtm_t to)
{
        return xwssc_rx(&bm_xwssc, BM_XWSSC_PORT_RAMCODE,
                        rxbuf, size, NULL, to);
}
