/**
 * @file
 * @brief SOC描述层：初始化
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
#include <xwcd/soc/arm/v7m/m4/s32k14x/soc_init.h>

struct soc_flash_cfg {
        xwu8_t backdoor_key[8];
        xwu8_t fprot[4];
        xwu8_t fsec;
        xwu8_t fopt;
        xwu8_t feprot;
        xwu8_t fdprot;
};

/**
 * @brief Flash OTP区域配置
 */
__xwcc_section(".flash.cfg") struct soc_flash_cfg soc_flash_cfg = {
        .backdoor_key = {
                0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        },
        .fprot = {
                0xFF, 0xFF, 0xFF, 0xFF,
        },
        .fsec = 0xFE,
        .fopt = 0x7F,
        .feprot = 0xFF,
        .fdprot = 0xFF,
};

#define WDOG_CS (*((__xw_io xwreg_t *)0x40052000))
#define WDOG_CS_DISABLE (0x00002520)
#define WDOG_TOVAL (*((__xw_io xwreg_t *)0x40052008))

__xwbsp_init_code
void soc_disable_wdog(void)
{
        WDOG_CS = WDOG_CS_DISABLE;
        WDOG_TOVAL= 0xFFFF;
}

/**
 * @brief SOC的初始化
 */
__xwbsp_init_code
void soc_init(void)
{
        soc_disable_wdog();
}
