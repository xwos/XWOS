/**
 * @file
 * @brief SOC描述层：MPC560xB watchdog
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

#include <xwos/standard.h>
#include <soc_wdg.h>

#define SOC_WDG_CLK     (128000U)

/**
 * @brief Init software watchdog
 * @param[in] deadline: deadline (Unit: ms)
 */
__xwbsp_code
void soc_wdg_init(xwu32_t deadline)
{
        bool init;
        xwu32_t cnt;

        cnt = 100;
        if (1 != SWT.CR.B.HLK) {
                /* Disable Software lock */
                SWT.SR.B.WSC = 0xC520;
                SWT.SR.B.WSC = 0xD928;
                while (0 < cnt) {
                        if (1 != SWT.CR.B.SLK) {
                                /* Proceed with initialization */
                                init = true;
                                break;
                        } else {
                                init = false;
                        }
                        cnt--;
                }
        } else {
                init = false;
        }

        if (init) {
                SWT.CR.B.WEN = 0;
                SWT.TO.R = (SOC_WDG_CLK / 1000)  * deadline;
                SWT.WN.R = 0x0; /* SWT's Windowed time in clock cycles is 0 */
                SWT.CR.R = 0x8000011E;
                /* Master Access Control for Master 0: enabled */
                /* SWT Key service mode: disabled */
                /* SWT Reset on Invalid Access: enabled */
                /* SWT Window Mode: disabled */
                /* SWT Interrupt then Reset: disabled */
                /* SWT Hard Lock: disabled */
                /* SWT Soft Lock: enabled */
                /* SWT Clock Source Selected: IRC 128KHz */
                /* SWT Stop Mode Control: enabled */
                /* SWT Debug Mode Control: enabled */
                /* Software Watchdog Timer: disabled */
        }
}

/**
 * @brief Start watchdog
 */
__xwbsp_code
void soc_wdg_start(void)
{
        bool init;
        xwu32_t cnt;

        cnt = 100;
        if (1 != SWT.CR.B.HLK) {
                /* Disable Software lock */
                SWT.SR.B.WSC = 0xC520;
                SWT.SR.B.WSC = 0xD928;
                while (0 < cnt) {
                        if (1 != SWT.CR.B.SLK) {
                                /* Proceed with initialization */
                                init = true;
                                break;
                        } else {
                                init = false;
                        }
                        cnt--;
                }
        } else {
                init = false;
        }

        if (init) {
                SWT.CR.B.WEN = 1;
        }
}

/**
 * @brief Stop watchdog
 */
__xwbsp_code
void soc_wdg_stop(void)
{
        bool init;
        xwu32_t cnt;

        cnt = 100;
        if (1 != SWT.CR.B.HLK) {
                /* Disable Software lock */
                SWT.SR.B.WSC = 0xC520;
                SWT.SR.B.WSC = 0xD928;
                while (0 < cnt) {
                        if (1 != SWT.CR.B.SLK) {
                                /* Proceed with initialization */
                                init = true;
                                break;
                        } else {
                                init = false;
                        }
                        cnt--;
                }
        } else {
                init = false;
        }

        if (init) {
                SWT.CR.B.WEN = 0;
        }
}

/**
 * @brief Feed watchdog
 */
__xwbsp_code
void soc_wdg_feed(void)
{
        SWT.SR.B.WSC = 0xB480;
        SWT.SR.B.WSC = 0xA602;
}

/**
 * @brief watchdog force reset the system
 */
__xwbsp_code
void soc_wdg_force_reset_system(void)
{
        if (0 == SWT.CR.B.WEN) {
                soc_wdg_start();
        }
        /* illegal value forces reset */
        SWT.SR.B.WSC = 0xBADF;
        SWT.SR.B.WSC = 0xBADF;
}
