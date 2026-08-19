/**
 * @file
 * @brief Intel8080 LCD Controller Device
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

#include <xwcd/peripheral/i80/lcd/device.h>

__xwbsp_api
void xwds_i80lcd_construct(struct xwds_i80lcd * i80lcd)
{
        xwds_device_construct(&i80lcd->dev);
        xwos_sem_init(&i80lcd->frmsem, 0, 1);
}

__xwbsp_api
void xwds_i80lcd_destruct(struct xwds_i80lcd * i80lcd)
{
        xwos_sem_fini(&i80lcd->frmsem);
        xwds_device_destruct(&i80lcd->dev);
}
