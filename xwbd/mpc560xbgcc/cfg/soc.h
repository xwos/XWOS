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

/******** ******** ******** chip ******** ******** ********/
#define SOCCFG_CHIP_MPC5604B                    0
#define SOCCFG_CHIP_MPC5607B                    1

/******** ******** ******** interrupt ******** ******** ********/
#define SOCCFG_IRQ_NUM                          256U
#define SOCCFG_RO_ISRTABLE                      1

/******** ******** ******** syshwt ******** ******** ********/
#define SOCCFG_SYSHWT_SRCCLK                    (64000000UL)
#define SOCCFG_SYSHWT_CHANNEL                   0U

/******** ******** ******** clock ******** ******** ********/
#define SOCCFG_CLK_FXOSC                        1
#define SOCCFG_CLK_FXOSC_VALUE                  (8000000U)
#define SOCCFG_CLK_FXOSC_DIV                    (1U)
#define SOCCFG_CLK_FIRC                         0
#define SOCCFG_CLK_FIRC_VALUE                   (16000000U)
#define SOCCFG_CLK_FIRC_DIV                     (1U)
#define SOCCFG_CLK_SCOSC                        0
#define SOCCFG_CLK_SCOSC_VALUE                  (32768U)
#define SOCCFG_CLK_SCOSC_DIV                    (1U)
#define SOCCFG_CLK_SIRC_VALUE                   (128000U)
#define SOCCFG_CLK_SIRC_DIV                     (25U)
#define SOCCFG_CLK_FMPLL_VALUE                  (64000000U)

/******** ******** CRC32 ******** ********/
#define SOCCFG_CRC32                            0

/******** ******** Flash ******** ********/
#define SOCCFG_FLASH_OPCODE                     0

#endif /* cfg/soc.h */
