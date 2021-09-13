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
#define BRDCFG_XWSKD_SYSHWT_HOOK                1
#define BRDCFG_XWSKD_THD_STACK_POOL             1
#define BRDCFG_LOG                              1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    board clock    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDCFG_SYSCLK                           (480000000U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          memory management          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BRDCFG_MM_DTCMHEAP_BLKSZ                (512U)
#define BRDCFG_MM_DTCMHEAP_BLKODR               (8U) /* 256 */
#define BRDCFG_MM_AXISRAM_BLKSZ                 (1024U)
#define BRDCFG_MM_AXISRAM_BLKODR                (9U) /* 512 */
#define BRDCFG_XWOS_THD_CACHE_ODR               3
#define BRDCFG_XWOS_SWT_CACHE_ODR               0
#define BRDCFG_XWOS_SEM_CACHE_ODR               0
#define BRDCFG_XWOS_COND_CACHE_ODR              0
#define BRDCFG_XWOS_MTX_CACHE_ODR               0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********   board modules   ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define BMCFG_main                              1
#define BMCFG_stm32cube                         1
#define BMCFG_button                            1
#define BMCFG_luamod                            1

#endif /* cfg/board.h */
