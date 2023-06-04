/**
 * @file
 * @brief 板级描述层：RAMCODE下载器：xwssc
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
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mif.h>
#include <xwmd/isc/xwssc/hwif/uart.h>
#include "bm/xwac/xwds/device.h"

#define BOARD_XWSSC_PORT_RAMCODE 0

struct xwssc board_xwssc;

XWSSC_DEF_MEMPOOL(board_xwssc_mem);

xwer_t board_xwssc_start(void)
{
        xwer_t rc;

        rc = xwssc_start(&board_xwssc, "board.xwssc",
                         &xwsscif_uart_ops, &stm32usart3,
                         board_xwssc_mem, sizeof(board_xwssc_mem));
        return rc;
}

xwer_t board_xwssc_stop(void)
{
        xwer_t rc;

        rc = xwssc_stop(&board_xwssc);
        return rc;
}


xwer_t board_xwssc_tx(const xwu8_t data[], xwsz_t * size, xwtm_t to)
{
        return xwssc_tx(&board_xwssc,
                        data, size,
                        XWSSC_MAX_PRI, BOARD_XWSSC_PORT_RAMCODE, XWSSC_MSG_QOS_3,
                        to);
}

xwer_t board_xwssc_rx(xwu8_t rxbuf[], xwsz_t * size, xwtm_t to)
{
        return xwssc_rx(&board_xwssc, BOARD_XWSSC_PORT_RAMCODE,
                        rxbuf, size, NULL, to);
}
