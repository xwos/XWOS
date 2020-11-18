/**
 * @file
 * @brief SOC描述层：Mode Entry of MPC560xB
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

#ifndef __soc_me_h__
#define __soc_me_h__

#include <xwos/standard.h>
#include <soc.h>

#define MPC5_ME_MCTL_KEY        0x5AF0U
#define MPC5_ME_MCTL_KEY_INV    0xA50FU
#define MPC5_ME_MCTL_MODE_MASK  (0xFU << 28)
#define MPC5_ME_MCTL_MODE(n)    ((n) << 28)

#define MPC5_ME_MODE_RESET              0U
#define MPC5_ME_MODE_TEST               1U
#define MPC5_ME_MODE_SAFE               2U
#define MPC5_ME_MODE_DRUN               3U
#define MPC5_ME_MODE_RUN0               4U
#define MPC5_ME_MODE_RUN1               5U
#define MPC5_ME_MODE_RUN2               6U
#define MPC5_ME_MODE_RUN3               7U
#define MPC5_ME_MODE_HALT               8U
#define MPC5_ME_MODE_STOP               10U
#define MPC5_ME_MODE_STANDBY            13U

#define MPC5_ME_RUNMODE                 MPC5_ME_MODE_RUN0

#define MPC5_ME_RUNPC_FROZEN            0U
#define MPC5_ME_RUNPC_ACTIVE            1U
#define MPC5_ME_RUNPC_ACTIVE_ALL        2U

#define MPC5_ME_LPPC_FROZEN             0U
#define MPC5_ME_LPPC_ACTIVE_HALT        1U
#define MPC5_ME_LPPC_ACTIVE_STOP        2U
#define MPC5_ME_LPPC_ACTIVE_STANDBY     3U
#define MPC5_ME_LPPC_ACTIVE_HALT_STOP   4U
#define MPC5_ME_LPPC_ACTIVE_ALL_LPM     5U

#define MPC5_ME_GS_SYSCLK_IRC           0
#define MPC5_ME_GS_SYSCLK_IRCDIV        1
#define MPC5_ME_GS_SYSCLK_XOSC          2
#define MPC5_ME_GS_SYSCLK_XOSCDIV       3
#define MPC5_ME_GS_SYSCLK_FMPLL         4

void soc_me_enter_mode(xwu32_t mode);

#endif /* soc_me.h */
