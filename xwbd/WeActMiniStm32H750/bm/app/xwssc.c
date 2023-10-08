/**
 * @file
 * @brief 板级描述层：用户程序：xwssc
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
#include <stdio.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mif.h>
#include <xwmd/isc/xwssc/hwif/uart.h>
#include "bm/xwac/xwds/device.h"

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

xwer_t board_xwssc_tst_connected(void)
{
        return xwssc_tst_connected(&board_xwssc);
}

xwer_t board_xwssc_txtsttask(void * arg)
{
        xwer_t rc;
        char msg[64];
        xwsq_t count;
        xwsz_t txsz;
        int fmtsz;
        XWOS_UNUSED(arg);
        rc = XWOK;
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                if (board_xwssc_tst_connected()) {
                        break;
                } else {
                        xwos_cthd_sleep(xwtm_s(2));
                }
        }
        xwos_cthd_sleep(xwtm_s(2));
        count = 0;
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                if (board_xwssc_tst_connected()) {
                        fmtsz = snprintf(msg, sizeof(msg),
                                         "[WeActMiniStm32H750/XWOS-3.0/XWSSC-1.0.0] "
                                         "Count:%ld",
                                         count);
                        txsz = (xwsz_t)fmtsz;
                        rc = board_xwssc_tx((xwu8_t *)msg, &txsz, 1, XWTM_MAX);
                        if (XWOK == rc) {
                                count++;
                        } else {
                                xwlogf(ERR, "board", "Failed to TX ... [%ld]\n", rc);
                        }
                }
        }
        return rc;
}

#define BOARD_XWSSC_TXTSTTHD_PRIORITY \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
struct xwos_thd board_xwssc_txtstthd;
xwos_thd_d board_xwssc_txtstthdd;
__xwcc_alignl1cache xwu8_t board_xwssc_txtstthd_stack[4096] = {0};

void board_xwssc_txtstthd_start(void)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        xwos_thd_attr_init(&attr);
        attr.name = "board.xwssc.txtstthd";
        attr.stack = (xwstk_t *)board_xwssc_txtstthd_stack;
        attr.stack_size = sizeof(board_xwssc_txtstthd_stack);
        attr.priority = BOARD_XWSSC_TXTSTTHD_PRIORITY;
        attr.detached = true;
        attr.privileged = true;
        rc = xwos_thd_init(&board_xwssc_txtstthd,
                           &board_xwssc_txtstthdd,
                           &attr,
                           board_xwssc_txtsttask,
                           NULL);
        if (rc < 0) {
        }
}

xwer_t board_xwssc_rxtsttask(void * arg)
{
        xwer_t rc;
        char msg[64];
        xwsz_t rxsz;
        XWOS_UNUSED(arg);
        rc = XWOK;
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rxsz = sizeof(msg);
                rc = board_xwssc_rx((xwu8_t *)msg, &rxsz, 1, XWTM_MAX);
                if (XWOK == rc) {
                        xwlogf(INFO, "board", "RX(%ld bytes):[%s]\n", rxsz, msg);
                }
        }
        return rc;
}

#define BOARD_XWSSC_RXTSTTHD_PRIORITY \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
struct xwos_thd board_xwssc_rxtstthd;
xwos_thd_d board_xwssc_rxtstthdd;
__xwcc_alignl1cache xwu8_t board_xwssc_rxtstthd_stack[4096] = {0};

void board_xwssc_rxtstthd_start(void)
{
        xwer_t rc;
        struct xwos_thd_attr attr;

        xwos_thd_attr_init(&attr);
        attr.name = "board.xwssc.rxtstthd";
        attr.stack = (xwstk_t *)board_xwssc_rxtstthd_stack;
        attr.stack_size = sizeof(board_xwssc_rxtstthd_stack);
        attr.priority = BOARD_XWSSC_RXTSTTHD_PRIORITY;
        attr.detached = true;
        attr.privileged = true;
        rc = xwos_thd_init(&board_xwssc_rxtstthd,
                           &board_xwssc_rxtstthdd,
                           &attr,
                           board_xwssc_rxtsttask,
                           NULL);
        if (rc < 0) {
        }
}
