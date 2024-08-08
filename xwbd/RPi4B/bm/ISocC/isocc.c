/**
 * @file
 * @brief Board Module：ISocC
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
#include <xwos/osal/time.h>
#include <xwos/osal/thd.h>
#include <stdio.h>
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mif.h>
#include <xwmd/isc/xwssc/hwif/uart.h>
#include "board/xwac/xwds/device.h"

struct xwssc isocc_xwssc;
XWSSC_DEF_MEMPOOL(isocc_xwssc_mem);

xwer_t isocc_init(void)
{
        xwer_t rc;

        rc = xwssc_start(&isocc_xwssc, "ISocC", &xwsscif_uart_ops,
                         &rpi4bxwds_uart5, isocc_xwssc_mem,
                         sizeof(isocc_xwssc_mem));
        return rc;
}

xwer_t isocc_fini(void)
{
        xwer_t rc;

        rc = xwssc_stop(&isocc_xwssc);
        return rc;
}

xwer_t isocc_tx(const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port,
                xwtm_t to)
{
        return xwssc_tx(&isocc_xwssc, data, size, pri, port,
                        XWSSC_MSG_QOS_3, to);
}

xwer_t isocc_rx(xwu8_t rxbuf[], xwsz_t * size, xwu8_t port, xwtm_t to)
{
        return xwssc_rx(&isocc_xwssc, port, rxbuf, size, NULL, to);
}

xwer_t isocc_try_rx(xwu8_t rxbuf[], xwsz_t * size, xwu8_t port)
{
        return xwssc_try_rx(&isocc_xwssc, port, rxbuf, size, NULL);
}

bool isocc_tst_connected(void)
{
        return xwssc_tst_connected(&isocc_xwssc);
}
