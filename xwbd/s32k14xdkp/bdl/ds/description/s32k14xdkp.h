/**
 * @file
 * @brief 设备栈描述
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

#ifndef __bdl_ds_description_s32k14xdkp_h__
#define __bdl_ds_description_s32k14xdkp_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macro       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BDL_IRQ_PRIO_LOW        (ARCH_IRQ_PRIO_1 | ARCH_IRQ_SUBPRIO_HIGH)
#define BDL_IRQ_PRIO_MIDDLE     (ARCH_IRQ_PRIO_2 | ARCH_IRQ_SUBPRIO_HIGH)
#define BDL_IRQ_PRIO_HIGH       (ARCH_IRQ_PRIO_3 | ARCH_IRQ_SUBPRIO_HIGH)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** irqc data ******** ********/
extern __soc_isr_table_qualifier struct soc_irq_data_table soc_irq_data_table;

/******** ******** device stack ******** ********/
extern struct xwds s32k14x_ds;

/******** ******** SOC ******** ********/
extern struct xwds_soc s32k14x_soc_cb;

/******** ******** UART ******** ********/

/******** ******** I2C master ******** ********/

/******** ******** I2C perpheral ******** ********/

/******** ******** misc device ******** ********/

/******** ******** RTC ******** ********/

#endif /* bdl/ds/description/s32k14xdkp.h */
