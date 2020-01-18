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
#define SOCCFG_CHIP_STM32F030x6                 0
#define SOCCFG_CHIP_STM32F030x8                 0
#define SOCCFG_CHIP_STM32F031x6                 0
#define SOCCFG_CHIP_STM32F038xx                 0
#define SOCCFG_CHIP_STM32F042x6                 0
#define SOCCFG_CHIP_STM32F048x6                 0
#define SOCCFG_CHIP_STM32F051x8                 0
#define SOCCFG_CHIP_STM32F058xx                 0
#define SOCCFG_CHIP_STM32F070x6                 0
#define SOCCFG_CHIP_STM32F071xB                 0
#define SOCCFG_CHIP_STM32F072xB                 1
#define SOCCFG_CHIP_STM32F078xx                 0
#define SOCCFG_CHIP_STM32F030xC                 0
#define SOCCFG_CHIP_STM32F091xC                 0
#define SOCCFG_CHIP_STM32F098xx                 0

/******** ******** ******** interrupt ******** ******** ********/
#define SOCCFG_IRQ_NUM                          28U
#define SOCCFG_RO_ISRTABLE                      1
#define SOCCFG_NVIC_PRIO_BITNUM                 2U

#endif /* cfg/soc.h */
