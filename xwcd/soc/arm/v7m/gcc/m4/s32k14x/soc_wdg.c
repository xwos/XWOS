/**
 * @file
 * @brief S32K14x Watchdog
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
#include <xwos/standard.h>
#include <soc_wdg.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SOC_WDG_CLK     (32000U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 初始化看门狗
 * @param deadline: 最后期限 (单位：毫秒)
 */
__xwbsp_code
void soc_wdg_init(xwu32_t deadline)
{
        WDOG->CNT = 0xD928C520;     /* Unlock watchdog */
        WDOG->TOVAL = (SOC_WDG_CLK / 1000)  * deadline;
        WDOG->CS = 0x00002120;
        /* WIN = 0: disable window mode */
        /* CMD32EN = 1: 32-bit refresh/unlock command write words */
        /* PRES = 0: disable 256 prescaler */
        /* CLK = 1: LPO_CLK as WDG source */
        /* EN = 0: disable watchdog */
        /* INT = 0: disable watchdog interrupt */
        /* UPDATE = 1: enable update */
        /* TST = 0: disable test */
        /* DBG = 0: stop watchdog in debug mode */
        /* WAIT = 0: stop watchdog in wait mode */
        /* STOP = 0: stop watchdog in stop mode */
        /* Disable watchdog */
}

/**
 * @brief 关闭看门狗
 */
__xwbsp_init_code
void soc_wdg_disable(void)
{
        WDOG->CNT = 0xD928C520;     /* Unlock watchdog */
        WDOG->TOVAL = 0x0000FFFF;   /* Maximum timeout value */
        WDOG->CS = 0x00002120;
        /* WIN = 0: disable window mode */
        /* CMD32EN = 1: 32-bit refresh/unlock command write words */
        /* PRES = 0: disable 256 prescaler */
        /* CLK = 1: LPO_CLK as WDG source */
        /* EN = 0: disable watchdog */
        /* INT = 0: disable watchdog interrupt */
        /* UPDATE = 1: enable update */
        /* TST = 0: disable test */
        /* DBG = 0: stop watchdog in debug mode */
        /* WAIT = 0: stop watchdog in wait mode */
        /* STOP = 0: stop watchdog in stop mode */
        /* Disable watchdog */
}

/**
 * @brief 启动看门狗
 */
__xwbsp_code
void soc_wdg_start(void)
{
        WDOG->CNT = 0xD928C520;     /* Unlock watchdog */
        WDOG->CS = 0x000021A0;
        /* WIN = 0: disable window mode */
        /* CMD32EN = 1: 32-bit refresh/unlock command write words */
        /* PRES = 0: disable 256 prescaler */
        /* CLK = 1: LPO_CLK as WDG source */
        /* EN = 1: enable watchdog */
        /* INT = 0: disable watchdog interrupt */
        /* UPDATE = 1: enable update */
        /* TST = 0: disable test */
        /* DBG = 0: stop watchdog in debug mode */
        /* WAIT = 0: stop watchdog in wait mode */
        /* STOP = 0: stop watchdog in stop mode */
        /* Disable watchdog */
}

/**
 * @brief 停止看门狗
 */
__xwbsp_code
void soc_wdg_stop(void)
{
        WDOG->CNT = 0xD928C520;     /* Unlock watchdog */
        WDOG->CS = 0x00002120;
        /* WIN = 0: disable window mode */
        /* CMD32EN = 1: 32-bit refresh/unlock command write words */
        /* PRES = 0: disable 256 prescaler */
        /* CLK = 1: LPO_CLK as WDG source */
        /* EN = 0: disable watchdog */
        /* INT = 0: disable watchdog interrupt */
        /* UPDATE = 1: enable update */
        /* TST = 0: disable test */
        /* DBG = 0: stop watchdog in debug mode */
        /* WAIT = 0: stop watchdog in wait mode */
        /* STOP = 0: stop watchdog in stop mode */
        /* Disable watchdog */
}

/**
 * @brief 喂看门狗
 */
__xwbsp_code
void soc_wdg_feed(void)
{
        WDOG->CNT = 0xB480A602;
}

/**
 * @brief 看门狗强制复位系统
 */
__xwbsp_code
void soc_wdg_force_reset_system(void)
{
        if (!(WDOG_CS_EN_MASK & WDOG->CS))
                soc_wdg_start();
        /* illegal value forces reset */
        WDOG->CNT = 0xBADF;
        WDOG->CNT = 0xBADF;
}
