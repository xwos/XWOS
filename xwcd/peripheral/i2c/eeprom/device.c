/**
 * @file
 * @brief I2C EEPROM 设备
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

#include <xwos/standard.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/peripheral/i2c/eeprom/device.h>

void xwds_eeprom_construct(struct xwds_eeprom * eeprom)
{
        xwds_i2cp_construct(&eeprom->i2cp);
}

void xwds_eeprom_destruct(struct xwds_eeprom * eeprom)
{
        xwds_i2cp_destruct(&eeprom->i2cp);
}
