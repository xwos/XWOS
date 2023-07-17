/**
 * @file
 * @brief STM32CUBE模块：Interrupt Service Routines
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

#ifndef __stm32cube_cubemx_IVT_isr_h__
#define __stm32cube_cubemx_IVT_isr_h__

#include "standard.h"
#include "cubemx/Core/Inc/main.h"

struct soc_ivt {
        xwisr_f exc[SOCCFG_EXC_NUM];
        xwisr_f irq[SOCCFG_IRQ_NUM];
};

extern struct soc_ivt stm32_ivt;

[T:ISR]

#endif /* stm32cube/cubemx/IVT/isr.h */
