/**
 * @file
 * @brief 设备栈驱动：S32K14x SOC
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

#ifndef __driver_ds_soc_h__
#define __driver_ds_soc_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwmd/ds/device.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwds_rodata const struct xwds_soc_driver s32k14x_soc_drv;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwbsp_isr
void s32k14x_porta_isr(void);

__xwbsp_isr
void s32k14x_portb_isr(void);

__xwbsp_isr
void s32k14x_portc_isr(void);

__xwbsp_isr
void s32k14x_portd_isr(void);

__xwbsp_isr
void s32k14x_porte_isr(void);

__xwbsp_isr
void s32k14x_edma_cmberr_isr(void);

__xwbsp_isr
void s32k14x_edma_ch0_isr(void);

__xwbsp_isr
void s32k14x_edma_ch1_isr(void);

__xwbsp_isr
void s32k14x_edma_ch2_isr(void);

__xwbsp_isr
void s32k14x_edma_ch3_isr(void);

__xwbsp_isr
void s32k14x_edma_ch4_isr(void);

__xwbsp_isr
void s32k14x_edma_ch5_isr(void);

__xwbsp_isr
void s32k14x_edma_ch6_isr(void);

__xwbsp_isr
void s32k14x_edma_ch7_isr(void);

__xwbsp_isr
void s32k14x_edma_ch8_isr(void);

__xwbsp_isr
void s32k14x_edma_ch9_isr(void);

__xwbsp_isr
void s32k14x_edma_ch10_isr(void);

__xwbsp_isr
void s32k14x_edma_ch11_isr(void);

__xwbsp_isr
void s32k14x_edma_ch12_isr(void);

__xwbsp_isr
void s32k14x_edma_ch13_isr(void);

__xwbsp_isr
void s32k14x_edma_ch14_isr(void);

__xwbsp_isr
void s32k14x_edma_ch15_isr(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ******** inline  functions ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* soc.h */
