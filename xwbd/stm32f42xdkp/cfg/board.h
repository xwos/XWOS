/**
 * @file
 * @brief 板级描述层(BDL)配置
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

#ifndef __cfg_board_h__
#define __cfg_board_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********     XWOS misc     ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDCFG_XWSD_IDLE_HOOK                   1
#define BRDCFG_XWSD_THRD_STACK_POOL             1
#define BRDCFG_LOG                              0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    board clock    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDCFG_SYSCLK                           (168000000U)
#define BRDCFG_SYSHWT_SRCCLK                    (BRDCFG_SYSCLK / 8)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          memory management          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDCFG_MM_CCMHEAP_BLKSZ                 (512U)
#define BRDCFG_MM_STKMEMPOOL_BLKSZ              (1024U)
#define BRDCFG_MM_ERAM_BLKSZ                    (4096U)
#define BRDCFG_XWOS_THRD_CACHE_ODR              3
#define BRDCFG_XWOS_SMR_CACHE_ODR               0
#define BRDCFG_XWOS_CDT_CACHE_ODR               0
#define BRDCFG_XWOS_EVT_CACHE_ODR               0
#define BRDCFG_XWOS_MTX_CACHE_ODR               0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   board modules   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BMCFG_stm32cube                         1
#define BMCFG_lua                               1
#define BMCFG_cxx                               1
#define BMCFG_xwpcp                             1
#define BMCFG_xwtst_sync_flag                   1
#define BMCFG_xwtst_sync_selector               1
#define BMCFG_xwtst_sync_barrier                1

#endif /* cfg/board.h */
