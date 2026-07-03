/**
 * @file
 * @brief bm::Xwssc::ch0
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
#include <xwmd/isc/xwssc/hwif/uart.h>
#include "board/xwac/xwds/device.h"
#include "bm/Xwssc/mi.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "Xwssc.Ch0"

struct xwssc xwssc_ch0;
XWSSC_DEF_MEMPOOL(xwssc_ch0_mem);
__xwos_thd_stack xwstk_t xwssc_ch0_rxthd_stack[1024U];
__xwos_thd_stack xwstk_t xwssc_ch0_txthd_stack[1024U];

void xwssc_ch0_notify(struct xwssc * xwssc, xwsq_t event)
{
        XWOS_UNUSED(xwssc);
        switch (event) {
        case XWSSC_HWIFNTF_CONNECT:
                xwlogf(I, LOGTAG, "Connected\r\n");
                break;
        case XWSSC_HWIFNTF_NETUNREACH:
                xwlogf(I, LOGTAG, "NetUnreach\r\n");
        default:
                break;
        }
}

void xwssc_ch0_init(void)
{
        xwer_t rc;

        rc = xwssc_start(&xwssc_ch0, "xwssc.ch0", &xwsscif_uart_ops, &rpi4bxwds_uart5,
                         xwssc_ch0_mem, sizeof(xwssc_ch0_mem),
                         xwssc_ch0_rxthd_stack, sizeof(xwssc_ch0_rxthd_stack),
                         xwssc_ch0_txthd_stack, sizeof(xwssc_ch0_txthd_stack));
        if (rc < 0) {
                xwlogf(I, LOGTAG, "xwssc_start(&xwssc_ch0) ... %d\r\n", rc);
        } else {
                xwsscif_uart_set_notification(xwssc_ch0_notify);
        }
}
