/**
 * @file
 * @brief 板级描述层(BDL)配置
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

#ifndef __cfg_board_h__
#define __cfg_board_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     XWOS misc     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDCFG_XWSKD_IDLE_HOOK                  1
#define BRDCFG_XWSKD_PRE_SWCX_HOOK              0
#define BRDCFG_XWSKD_POST_SWCX_HOOK             0
#define BRDCFG_XWSKD_SYSHWT_HOOK                0
#define BRDCFG_XWSKD_THD_STACK_POOL             0
#define BRDCFG_LOG                              0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    board clock    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDCFG_ICS_OSC                          1
#define BRDCFG_ICS_OSC_IS_CRYSTAL               1
#define BRDCFG_ICS_OSC_HG                       1
#define BRDCFG_ICS_OSC_STEN                     0
#define BRDCFG_ICS_OSC_VALUE                    (8000000U)
#define BRDCFG_ICS_IRC                          1
#define BRDCFG_ICS_IRC_STEN                     0
#define BRDCFG_ICS_IRC_VALUE                    (37500U)
#define BRDCFG_ICS_MODE                         (SOC_CLKM_FEE)
#define BRDCFG_SYSCLK                           (40000000U)
#define BRDCFG_SYSHWT_SRCCLK                    (BRDCFG_SYSCLK / 16)

#endif /* cfg/board.h */
