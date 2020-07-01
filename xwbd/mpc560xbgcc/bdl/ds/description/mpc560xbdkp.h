/**
 * @file
 * @brief 设备栈描述
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

#ifndef __bdl_ds_description_mpc560xbdkp_h__
#define __bdl_ds_description_mpc560xbdkp_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>
#include <xwmd/ds/uart/dma.h>
#include <xwmd/ds/i2c/master.h>
#include <xwmd/ds/misc/chip.h>
#include <soc_irqc.h>
#include <soc_clk.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BDL_IRQ_PRIO_LOW        (SOC_IRQC_MIN_PRIO)
#define BDL_IRQ_PRIO_MIDDLE     (SOC_IRQC_MIN_PRIO + 1)
#define BDL_IRQ_PRIO_HIGH       (SOC_IRQC_MIN_PRIO + 2)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** irqc data ******** ********/
extern const struct soc_irqc_data mpc560xbdkp_isr_match_table;

/******** ******** ds ******** ********/
extern struct xwds mpc560xb_ds;

/******** ******** SOC ******** ********/
extern struct xwds_soc mpc560xb_soc_cb;

/******** ******** UART ******** ********/
extern struct xwds_dmauartc mpc560xb_uart0_cb;

/******** ******** I2C master ******** ********/
extern struct xwds_i2cm mpc560xb_i2cm_cb;

/******** ******** I2C perpheral ******** ********/
extern struct xwds_i2cp_eeprom at24cxx_cb;

/******** ******** misc device ******** ********/
extern struct xwds_misc mpc560xb_misc_cb;

/******** ******** RTC ******** ********/
extern struct xwds_misc mpc560xb_rtc_cb;

#endif /* bdl/ds/description/mpc560xbdkp.h */
