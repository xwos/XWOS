/**
 * @file
 * @brief 设备栈驱动：MPC560xB SOC
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

#ifndef __bdl_ds_driver_soc_h__
#define __bdl_ds_driver_soc_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwcd/ds/device.h>

extern __xwbsp_rodata const struct xwds_soc_driver mpc560xb_soc_drv;

void mpc560xb_wkup_irq0_7_isr(void);
void mpc560xb_wkup_irq8_15_isr(void);
void mpc560xb_wkup_irq16_23_isr(void);
void mpc560xb_wkup_irq24_28_isr(void);
void mpc560xb_eirq0_7_isr(void);
void mpc560xb_eirq8_15_isr(void);
void mpc560xb_eirq16_23_isr(void);
void mpc560xb_edma_cmberr_isr(void);
void mpc560xb_edma_ch0_isr(void);
void mpc560xb_edma_ch1_isr(void);
void mpc560xb_edma_ch2_isr(void);
void mpc560xb_edma_ch3_isr(void);
void mpc560xb_edma_ch4_isr(void);
void mpc560xb_edma_ch5_isr(void);
void mpc560xb_edma_ch6_isr(void);
void mpc560xb_edma_ch7_isr(void);
void mpc560xb_edma_ch8_isr(void);
void mpc560xb_edma_ch9_isr(void);
void mpc560xb_edma_ch10_isr(void);
void mpc560xb_edma_ch11_isr(void);
void mpc560xb_edma_ch12_isr(void);
void mpc560xb_edma_ch13_isr(void);
void mpc560xb_edma_ch14_isr(void);
void mpc560xb_edma_ch15_isr(void);

#endif /* bdl/ds/driver/soc.h */
