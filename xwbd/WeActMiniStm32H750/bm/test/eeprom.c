/**
 * @file
 * @brief 板级描述层：测试程序：eeprom
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/time.h>
#include <xwcd/peripheral/i2c/eeprom/driver.h>
#include "board/xwac/xwds/device.h"

#define LOGTAG "TST|EEPROM"

void tst_eeprom_init(void)
{
        xwu8_t rdbuf[64];
        xwsz_t rdsz;
        xwer_t rc;

        rdsz = sizeof(rdbuf);
        rc = xwds_eeprom_pgread(&stm32xwds_eeprom256k,
                                rdbuf, &rdsz, 0,
                                xwtm_ft(xwtm_s(1)));
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Failed to read ... <%ld>\n", rc);
        } else {
                rdbuf[63] = 0;
                xwlogf(INFO, LOGTAG, "Content:%s\n", rdbuf);
        }
}
