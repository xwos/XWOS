/**
 * @file
 * @brief RPi4B::XWDS::SOC
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

#ifndef __board_xwac_xwds_soc_h__
#define __board_xwac_xwds_soc_h__

#include <board/std.h>

/**
 * @brief BCM2711 GPIO引脚配置
 */
struct rpi4bxwds_gpio_cfg {
        xwu32_t function; /**< 功能选择: SOC_GPIO_ALT_GPI / GPO / ALT0~5 */
        xwu32_t pud; /**< 上下拉: SOC_GPIO_PUPDC_NO / PU / PD */
};

#endif /* board/xwac/xwds/soc.h */
