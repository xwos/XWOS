/**
 * @file
 * @brief STM32CUBE XWDS 设备：SOC
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

#ifndef __bm_stm32cube_xwds_soc_h__
#define __bm_stm32cube_xwds_soc_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <bm/stm32cube/standard.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
enum stm32cube_soc_dma_rc {
        STM32CUBE_SOC_DMA_RC_FE = BIT(0), /**< FIFO error */
        STM32CUBE_SOC_DMA_RC_DME = BIT(2), /**< Direct Mode error */
        STM32CUBE_SOC_DMA_RC_TE = BIT(3), /**< Transfer error */
        STM32CUBE_SOC_DMA_RC_HT = BIT(4), /**< Half transfer */
        STM32CUBE_SOC_DMA_RC_TC = BIT(5), /**< Transfer complete */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void stm32cube_exti_isr(void);

void stm32cube_dma1_stream0_isr(void);

void stm32cube_dma1_stream1_isr(void);

void stm32cube_dma1_stream2_isr(void);

void stm32cube_dma1_stream3_isr(void);

void stm32cube_dma1_stream4_isr(void);

void stm32cube_dma1_stream5_isr(void);

void stm32cube_dma1_stream6_isr(void);

void stm32cube_dma1_stream7_isr(void);

void stm32cube_dma2_stream0_isr(void);

void stm32cube_dma2_stream1_isr(void);

void stm32cube_dma2_stream2_isr(void);

void stm32cube_dma2_stream3_isr(void);

void stm32cube_dma2_stream4_isr(void);

void stm32cube_dma2_stream5_isr(void);

void stm32cube_dma2_stream6_isr(void);

void stm32cube_dma2_stream7_isr(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* bm/stm32cube/xwds/soc.h */
