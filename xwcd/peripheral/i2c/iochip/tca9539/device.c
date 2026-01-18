/**
 * @file
 * @brief 玄武设备栈：I2C IO扩展芯片：TCA9539
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

#include <xwos/standard.h>
#include <xwcd/ds/i2c/peripheral.h>
#include <xwcd/peripheral/i2c/iochip/tca9539/device.h>

__xwbsp_api
void xwds_tca9539_construct(struct xwds_tca9539 * tca9539)
{
        xwds_i2cp_construct(&tca9539->iochip.bc.i2cp);
}

__xwbsp_api
void xwds_tca9539_destruct(struct xwds_tca9539 * tca9539)
{
        xwds_i2cp_destruct(&tca9539->iochip.bc.i2cp);
}
