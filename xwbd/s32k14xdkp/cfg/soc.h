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
#define SOCCFG_CHIP_S32K144                     1

/******** ******** ******** clock ******** ******** ********/
#define SOCCFG_CLK_SOSC_VALUE                   (8000000U)
#define SOCCFG_CLK_RTCOSC_VALUE                 (0U)
#define SOCCFG_CLK_SIRC_VALUE                   (8000000U)
#define SOCCFG_CLK_FIRC_VALUE                   (48000000U)
#define SOCCFG_CLK_LPO128K_VALUE                (128000U)
#define SOCCFG_CLK_LPO32K_VALUE                 (32000U)
#define SOCCFG_CLK_LPO1K_VALUE                  (1000U)
#define SOCCFG_CLK_LPOCLK_VALUE                 (SOCCFG_CLK_LPO32K_VALUE)
#define SOCCFG_CLK_RCTCLK_VALUE                 (SOCCFG_CLK_LPO32K_VALUE)

/******** ******** ******** interrupt ******** ******** ********/
#define SOCCFG_IRQ_NUM                          240U
#define SOCCFG_RO_ISRTABLE                      1
#define SOCCFG_NVIC_PRIO_BITNUM                 4U
#define SOCCFG_NVIC_SUBPRIO_BITIDX              4U

/******** ******** ******** SOC perpheral ******** ******** ********/
#define SOCCFG_CRC32                            0

/******** ******** SOC xwds driver ******** ********/
#define SOCCFG_ds_SOC                           1

#endif /* cfg/soc.h */
