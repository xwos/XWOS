/**
 * @file
 * @brief 板级描述层：测试程序：W25QXX
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
#include <xwos/lib/xwlog.h>
#include <xwcd/peripheral/spi/flash/w25qxx/driver.h>
#include "board/xwac/xwds/device.h"
#include <xwam/application/w25qpt/mif.h>
#include <xwam/application/w25qpt/hwif/uart.h>

void tst_w25qxx_read_id(void)
{
        xwu64_t uid;
        xwu16_t mid;
        xwu32_t jid;
        xwer_t rc;

        rc = xwds_w25qxx_read_uid(&stm32xwds_w25q64jv, &uid, xwtm_ft(xwtm_s(2)));
        if (rc < 0) {
                xwlogf(E, "T.W25QXX", "Failed to read UID ... %d\r\n", rc);
        }
        rc = xwds_w25qxx_read_mid(&stm32xwds_w25q64jv, &mid, xwtm_ft(xwtm_s(2)));
        if (rc < 0) {
                xwlogf(E, "T.W25QXX", "Failed to read MID ... %d\r\n", rc);
        }
        rc = xwds_w25qxx_read_jid(&stm32xwds_w25q64jv, &jid, xwtm_ft(xwtm_s(2)));
        if (rc < 0) {
                xwlogf(E, "T.W25QXX", "Failed to read JID ... %d\r\n", rc);
        }
        xwlogf(I, "T.W25QXX", "UID:0x%llX, MID:0x%X, JID:0x%X\r\n", uid, mid, jid);
}

struct w25qpt tst_w25qpt;

xwer_t tst_w25qxx_init(void)
{
        xwer_t rc;

        tst_w25qxx_read_id();
        rc = w25qpt_start(&tst_w25qpt, "SatelliteBox.w25qpt",
                           &stm32xwds_w25q64jv,
                           &w25qptif_uart_ops, &stm32xwds_usart1);
        if (rc < 0) {
                xwlogf(E, "T.W25QXX", "Failed to start programer tool ... %d\r\n", rc);
        }
        return rc;
}
