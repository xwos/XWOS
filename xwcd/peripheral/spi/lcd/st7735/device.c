/**
 * @file
 * @brief SPI LCD Controller ST7735 Device
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

#include <xwcd/peripheral/spi/lcd/st7735/device.h>

__xwbsp_api
void xwds_st7735_construct(struct xwds_st7735 * st7735)
{
        xwds_spip_construct(&st7735->spip);
}

__xwbsp_api
void xwds_st7735_destruct(struct xwds_st7735 * st7735)
{
        xwds_spip_destruct(&st7735->spip);
}
