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
#define SOCCFG_CHIP_STM32F405xx                 0
#define SOCCFG_CHIP_STM32F415xx                 0
#define SOCCFG_CHIP_STM32F407xx                 0
#define SOCCFG_CHIP_STM32F417xx                 0
#define SOCCFG_CHIP_STM32F427xx                 0
#define SOCCFG_CHIP_STM32F437xx                 0
#define SOCCFG_CHIP_STM32F429xx                 1
#define SOCCFG_CHIP_STM32F439xx                 0
#define SOCCFG_CHIP_STM32F401xC                 0
#define SOCCFG_CHIP_STM32F401xE                 0
#define SOCCFG_CHIP_STM32F410Tx                 0
#define SOCCFG_CHIP_STM32F410Cx                 0
#define SOCCFG_CHIP_STM32F410Rx                 0
#define SOCCFG_CHIP_STM32F411xE                 0
#define SOCCFG_CHIP_STM32F446xx                 0
#define SOCCFG_CHIP_STM32F469xx                 0
#define SOCCFG_CHIP_STM32F479xx                 0
#define SOCCFG_CHIP_STM32F412Cx                 0
#define SOCCFG_CHIP_STM32F412Zx                 0
#define SOCCFG_CHIP_STM32F412Vx                 0
#define SOCCFG_CHIP_STM32F412Rx                 0
#define SOCCFG_CHIP_STM32F413xx                 0
#define SOCCFG_CHIP_STM32F423xx                 0

/******** ******** interrupt ******** ********/
#define SOCCFG_IRQ_NUM                          112U
#define SOCCFG_RO_ISRTABLE                      1
#define SOCCFG_NVIC_PRIO_BITNUM                 4U
#define SOCCFG_NVIC_SUBPRIO_BITIDX              4U

#endif /* cfg/soc.h */
