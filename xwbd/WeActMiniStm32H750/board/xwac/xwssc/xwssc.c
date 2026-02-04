/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWSSC
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
#include <xwmd/isc/xwssc/mi.h>
#include <xwmd/isc/xwssc/hwif/uart.h>
#include <bm/Stm32Hal/xwds/device.h>
#include "board/xwac/xwssc/xwssc.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "XWSSC.UART3"

struct xwssc xwssc3;
XWSSC_DEF_MEMPOOL(xwssc3_mem);
xwstk_t xwssc3_rxthd_stack[(4096U / sizeof(xwstk_t))];
xwstk_t xwssc3_txthd_stack[(4096U / sizeof(xwstk_t))];

static
void xwssc3_notify(struct xwssc * xwssc, xwsq_t event);

void xwssc3_init(void)
{
        xwer_t rc;

        rc = xwssc_start(&xwssc3, "XWSSC.UART3",
                         &xwsscif_uart_ops, &stm32xwds_usart3,
                         xwssc3_mem, sizeof(xwssc3_mem),
                         xwssc3_rxthd_stack, sizeof(xwssc3_rxthd_stack),
                         xwssc3_txthd_stack, sizeof(xwssc3_txthd_stack));

        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Start XWSSC ... <rc:%d>", rc);
        } else {
                xwsscif_uart_set_notification(xwssc3_notify);
        }
}

void xwssc3_fini(void)
{
        xwssc_stop(&xwssc3);
}

static
void xwssc3_notify(struct xwssc * xwssc, xwsq_t event)
{
        XWOS_UNUSED(xwssc);
        switch (event) {
        case XWSSC_HWIFNTF_CONNECT:
                break;
        case XWSSC_HWIFNTF_NETUNREACH:
        default:
                break;
        }
}
