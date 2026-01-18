/**
 * @file
 * @brief 板级描述层：用户程序：xwssc
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
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mif.h>
#include <xwmd/isc/xwssc/hwif/uart.h>
#include "board/xwac/xwds/device.h"

struct xwssc board_xwssc;

XWSSC_DEF_MEMPOOL(board_xwssc_mem);

xwer_t board_xwssc_init(void)
{
        xwer_t rc;

        rc = xwssc_start(&board_xwssc, "board.xwssc",
                         &xwsscif_uart_ops, &stm32xwds_usart3,
                         board_xwssc_mem, sizeof(board_xwssc_mem));
        return rc;
}

xwer_t board_xwssc_fini(void)
{
        xwer_t rc;

        rc = xwssc_stop(&board_xwssc);
        return rc;
}

struct xwssc * board_xwssc_get(void)
{
        return &board_xwssc;
}

xwer_t board_xwssc_tx(const xwu8_t data[], xwsz_t * size, xwu8_t port, xwtm_t to)
{
        return xwssc_tx(&board_xwssc,
                        data, size,
                        XWSSC_MAX_PRI, port, XWSSC_MSG_QOS_3,
                        to);
}

xwer_t board_xwssc_rx(xwu8_t rxbuf[], xwsz_t * size, xwu8_t port, xwtm_t to)
{
        return xwssc_rx(&board_xwssc, port, rxbuf, size, NULL, to);
}

bool board_xwssc_tst_connected(void)
{
        return xwssc_tst_connected(&board_xwssc);
}
