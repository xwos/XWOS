/**
 * @file
 * @brief 片上系统描述层(SDL)配置
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

#ifndef __cfg_soc_h__
#define __cfg_soc_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          soc specification          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** chip ******** ********/
#define SOCCFG_CHIP_STM32F100x4                 0
#define SOCCFG_CHIP_STM32F100x6                 0
#define SOCCFG_CHIP_STM32F100x8                 0
#define SOCCFG_CHIP_STM32F100xB                 0
#define SOCCFG_CHIP_STM32F100xC                 0
#define SOCCFG_CHIP_STM32F100xD                 0
#define SOCCFG_CHIP_STM32F100xE                 0

#define SOCCFG_CHIP_STM32F101x4                 0
#define SOCCFG_CHIP_STM32F101x6                 0
#define SOCCFG_CHIP_STM32F101x8                 0
#define SOCCFG_CHIP_STM32F101xB                 0
#define SOCCFG_CHIP_STM32F101xC                 0
#define SOCCFG_CHIP_STM32F101xD                 0
#define SOCCFG_CHIP_STM32F101xE                 0
#define SOCCFG_CHIP_STM32F101xF                 0
#define SOCCFG_CHIP_STM32F101xG                 0

#define SOCCFG_CHIP_STM32F102x4                 0
#define SOCCFG_CHIP_STM32F102x6                 0
#define SOCCFG_CHIP_STM32F102x8                 0
#define SOCCFG_CHIP_STM32F102xB                 0
#define SOCCFG_CHIP_STM32F102xC                 0
#define SOCCFG_CHIP_STM32F102xD                 0
#define SOCCFG_CHIP_STM32F102xE                 0
#define SOCCFG_CHIP_STM32F102xF                 0
#define SOCCFG_CHIP_STM32F102xG                 0

#define SOCCFG_CHIP_STM32F103x4                 0
#define SOCCFG_CHIP_STM32F103x6                 0
#define SOCCFG_CHIP_STM32F103x8                 0
#define SOCCFG_CHIP_STM32F103xB                 0
#define SOCCFG_CHIP_STM32F103xC                 1
#define SOCCFG_CHIP_STM32F103xD                 0
#define SOCCFG_CHIP_STM32F103xE                 0
#define SOCCFG_CHIP_STM32F103xF                 0
#define SOCCFG_CHIP_STM32F103xG                 0

#define SOCCFG_CHIP_STM32F105x                  0
#define SOCCFG_CHIP_STM32F107x                  0

/******** ******** ******** interrupt ******** ******** ********/
#define SOCCFG_IRQ_NUM                          112U
#define SOCCFG_RO_ISRTABLE                      1
#define SOCCFG_NVIC_PRIO_BITNUM                 4U
#define SOCCFG_NVIC_SUBPRIO_BITIDX              4U

#endif /* cfg/soc.h */
