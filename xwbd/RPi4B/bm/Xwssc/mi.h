/**
 * @file
 * @brief bm::Xwssc::mi 模块集成接口
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

#ifndef Component_Xwssc_mi_h__
#define Component_Xwssc_mi_h__

#include "board/std.h"
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mi.h>

enum xwssc_ch0_port_em {
        XWSSC_CH0_PORT_POWER,
        XWSSC_CH0_PORT_VEHICLE,
        XWSSC_CH0_PORT_DOGW,
        XWSSC_CH0_PORT_DCM,
        XWSSC_CH0_PORT_RTC,
        XWSSC_CH0_PORT_WDG,
        XWSSC_CH0_PORT_FLASH,
        XWSSC_CH0_PORT_MFG,
        XWSSC_CH0_PORT_NUM,
};

extern struct xwssc xwssc_ch0;

/**
 * @brief 初始化并启动XWSSC通道0
 */
void xwssc_ch0_init(void);

#endif /* bm/Xwssc/mi.h */
