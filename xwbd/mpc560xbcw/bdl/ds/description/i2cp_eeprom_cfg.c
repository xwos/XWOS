/**
 * @file
 * @brief 设备栈描述：I2C EEPROM
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/i2c/perpheral.h>
#include <xwcd/perpheral/ds/i2c/eeprom/common/driver.h>
#include <xwcd/perpheral/ds/i2c/eeprom/chip/at24sdd.h>
#include <xwcd/perpheral/ds/i2c/eeprom/chip/at24mdd.h>
#include <bdl/ds/description/mpc560xbdkp.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwds_resources at24cxx_resources = {
	.irqrsc_array = NULL,
	.irqrsc_num = 0,
	.regrsc_array = NULL,
	.regrsc_num = 0,
	.clkrsc_array = NULL,
	.clkrsc_num = 0,
	.gpiorsc_array = NULL,
	.gpiorsc_num = 0,
        .dmarsc_array = NULL,
        .dmarsc_num = 0,
};

const struct xwds_i2cp_eeprom_cfg at24cxx_cfg = {
        .page_size = 32,
        .total = 8192,
};

struct xwds_i2cp_eeprom at24cxx_cb = {
	.i2cp = {
	        .dev = {
                        .name = "at24cxx",
                        .id = 1,
                        .resources = &at24cxx_resources,
                        .drv = xwds_static_cast(struct xwds_driver *, &at24mdd_drv),
                        .data = NULL,
	        },
                .bus = &mpc560xb_i2cm_cb,
                .addr = 0xAE,
        },
        .cfg = &at24cxx_cfg,
};
