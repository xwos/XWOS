/**
 * @file
 * @brief Stm32Hal::mi 模块集成接口
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

#include <board/std.h>
#include "bm/Stm32Hal/CubeMX/integration.h"
#include "bm/Stm32Hal/xwds/device.h"
#include "bm/Stm32Hal/mi.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "stm32hal"

void stm32hal_init_devices(void);
void stm32hal_fini_devices(void);

__xwbsp_init_code
void stm32hal_preinit(void)
{
        stm32cubemx_preinit();
}

__xwbsp_init_code
void stm32hal_init(void)
{
        stm32cubemx_init();
        stm32xwds_init();
        stm32xwds_soc_init();
}

__xwbsp_init_code
void stm32hal_postinit(void)
{
        stm32hal_init_devices();
}

__xwbsp_init_code
void stm32hal_init_devices(void)
{
        xwer_t rc;

        rc = stm32xwds_uart_init();
        XWOS_BUG_ON(rc < 0);
        /* Can print log from here */
        rc = stm32xwds_spi_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init SPI ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_w25q64jv_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init W25Q64JV ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_st7735_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init ST7735 ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_i2c_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init I2C ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_eeprom_init();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Init EEPROM ... <rc:%d>\n", rc);
        }
}

__xwbsp_init_code
void stm32hal_fini_devices(void)
{
        xwer_t rc;

        rc = stm32xwds_eeprom_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini EEPROM ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_i2c_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini I2C ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_st7735_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini ST7735 ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_w25q64jv_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini W25Q64JV ... <rc:%d>\n", rc);
        }
        rc = stm32xwds_spi_fini();
        if (rc < 0) {
                xwlogf(ERR, LOGTAG, "Fini SPI ... <rc:%d>\n", rc);
        }
        stm32xwds_uart_fini();
}
