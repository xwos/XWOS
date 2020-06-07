/**
 * @file
 * @brief S32KSDK：初始化
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
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
#include <oem/s32ksdk/standard.h>
#include "Cpu.h"

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwbsp_init_code
void s32ksdk_llinit_close_wdog(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_init_code
void s32ksdk_lowlevel_init(void)
{
        s32ksdk_llinit_close_wdog();
}

__xwbsp_init_code
void s32ksdk_init(void)
{
        CLOCK_SYS_Init(g_clockManConfigsArr, CLOCK_MANAGER_CONFIG_CNT,
                       g_clockManCallbacksArr, CLOCK_MANAGER_CALLBACK_CNT);
        CLOCK_SYS_UpdateConfiguration(0, CLOCK_MANAGER_POLICY_FORCIBLE);

        POWER_SYS_Init(&powerConfigsArr, POWER_MANAGER_CONFIG_CNT,
                       &powerStaticCallbacksConfigsArr, POWER_MANAGER_CALLBACK_CNT);
        POWER_SYS_SetMode(1, POWER_MANAGER_POLICY_FORCIBLE);

        PINS_DRV_Init(NUM_OF_CONFIGURED_PINS, g_pin_mux_InitConfigArr);
}

static __xwbsp_init_code
void s32ksdk_llinit_close_wdog(void)
{
        WDOG->CNT = (uint32_t) FEATURE_WDOG_UNLOCK_VALUE;
        xwmb_access(uint32_t, WDOG->CNT);
        WDOG->CS  = (uint32_t ) ((1UL << WDOG_CS_CMD32EN_SHIFT)                 |
                            (FEATURE_WDOG_CLK_FROM_LPO << WDOG_CS_CLK_SHIFT)    |
                            (0U << WDOG_CS_EN_SHIFT)                            |
                            (1U << WDOG_CS_UPDATE_SHIFT));
        WDOG->TOVAL = (uint32_t )0xFFFF;
}
