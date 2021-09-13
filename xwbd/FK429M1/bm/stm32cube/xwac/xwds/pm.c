/**
 * @file
 * @brief STM32CUBE设备栈：电源管理
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

#include <bm/stm32cube/standard.h>
#include <xwos/osal/pm.h>
#include <xwcd/ds/xwds.h>
#include <stm32f4xx_ll_cortex.h>
#include <stm32f4xx_ll_pwr.h>
#include <bm/stm32cube/cubemx/Core/Inc/main.h>
#include <bm/stm32cube/xwac/xwds/device.h>
#include <bm/stm32cube/xwac/xwds/pm.h>

extern
void SystemClock_Config(void);

void stm32cube_pm_resume(void * arg)
{
        __xwcc_unused xwer_t rc;
        xwirq_t irq;

        XWOS_UNUSED(arg);
        rc = xwos_irq_get_id(&irq);
        LL_PWR_SetPowerMode(LL_PWR_MODE_STOP_MAINREGU);
        xwds_pm_resume(&stm32cube_ds);
}

void stm32cube_pm_suspend(void * arg)
{
        __xwcc_unused xwer_t rc;
        xwirq_t irq;

        XWOS_UNUSED(arg);
        rc = xwos_irq_get_id(&irq);
        xwds_pm_suspend(&stm32cube_ds);
        LL_PWR_SetPowerMode(LL_PWR_MODE_STOP_LPREGU);
        LL_LPM_EnableDeepSleep();
}

void stm32cube_pm_wakeup(void * arg)
{
        XWOS_UNUSED(arg);
        LL_LPM_EnableSleep();
        /* 从STOP模式恢复后，需要重新配置时钟 */
        SystemClock_Config();
}

void stm32cube_pm_sleep(void * arg)
{
        XWOS_UNUSED(arg);
        cm_wfi();
}
