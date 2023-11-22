/**
 * @file
 * @brief 玄武设备栈：CAN收发器：通用型104x/105x系列收发器
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
#include <xwcd/ds/can/transceiver.h>
#include <xwcd/peripheral/can/transceiver/xxx10xx/device.h>

__xwbsp_api
void xwds_cantrcv_xxx10xx_construct(struct xwds_cantrcv_xxx10xx * xxx10xx)
{
        xwds_cantrcv_construct(&xxx10xx->cantrcv);
}

__xwbsp_api
void xwds_cantrcv_xxx10xx_destruct(struct xwds_cantrcv_xxx10xx * xxx10xx)
{
        xwds_cantrcv_destruct(&xxx10xx->cantrcv);
}
