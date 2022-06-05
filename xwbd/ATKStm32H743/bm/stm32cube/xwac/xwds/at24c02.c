/**
 * @file
 * @brief STM32CUBE设备栈：AT24C02
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

#include <xwcd/perpheral/i2c/eeprom/device.h>
#include <xwcd/perpheral/i2c/eeprom/driver.h>
#include <bm/stm32cube/xwac/xwds/device.h>

struct xwds_eeprom at24c02 = {
        /* attributes */
        .i2cp = {
                .dev = {
                        .name = "stm32cube.at24c02.1",
                        .id = 1,
                        .resources = NULL,
                        .drv = xwds_cast(struct xwds_driver *, &at24sd_drv),
                        .data = NULL,
                },
                .bus = &stm32i2c2m,
                .addr = 0xA0,
        },
        .parameter = {
                .page_size = 8,
                .total = 256,
        },
        .pwr_gpiorsc = NULL,
        .wp_gpiorsc = NULL,
};
