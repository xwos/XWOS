/**
 * @file
 * @brief 内核配置
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __cfg_xwos_h__
#define __cfg_xwos_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        Common Configurations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOSCFG_CHECK_PARAMETERS                        1
#define XWOSCFG_BUG                                     1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         xwlib Configurations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLIBCFG_LOG                                    1
#define XWLIBCFG_XWLOG_LEVEL                            (0U)
#define XWLIBCFG_XWLOG_BUFSIZE                          (256U)

#define XWLIBCFG_SC                                     0

#define XWLIBCFG_XWAOP8                                 1
#define XWLIBCFG_XWAOP16                                1
#define XWLIBCFG_XWAOP32                                1
#define XWLIBCFG_XWAOP64                                1
#define XWLIBCFG_XWBMPAOP                               1

#define XWLIBCFG_MAP                                    1

#define XWLIBCFG_CRC32                                  1
#define XWLIBCFG_CRC32_0X04C11DB7                       1
#define XWLIBCFG_CRC32_0XEDB88320                       1
#define XWLIBCFG_CRC8                                   1
#define XWLIBCFG_CRC8_0X07                              1
#define XWLIBCFG_CRC8_0X31                              1
#define XWLIBCFG_CRC8_0X9B                              1

#define XWLIBCFG_SETJMP                                 0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   Memory Management Configurations  ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** common ********/
#define XWMMCFG_ALIGNMENT                               (16U)
#define XWMMCFG_STACK_ALIGNMENT                         (16U)
#define XWMMCFG_STACK_SIZE_DEFAULT                      (8192U)
#define XWMMCFG_STACK_SIZE_MIN                          (1024U)
#define XWMMCFG_STACK_GUARD_SIZE_DEFAULT                (64U)
#define XWMMCFG_STACK_CHK_SWCX                          1
#define XWMMCFG_FD_STACK                                1
#define XWMMCFG_ED_STACK                                0
#define XWMMCFG_FA_STACK                                0
#define XWMMCFG_EA_STACK                                0

/******** memslice ********/
#define XWMMCFG_MEMSLICE                                1

/******** buddy algorithm memory allocater ********/
#define XWMMCFG_BMA                                     1

/******** mempool ********/
#define XWMMCFG_MEMPOOL                                 1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          OS Configurations          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** syshwt ******** ******** ********/
#define XWOSCFG_SYSHWT_PERIOD                           ((xwtm_t)1000000)

/******** ******** ******** scheduler ******** ******** ********/
#define XWOSCFG_SKD_PRIORITY_RT_NUM                     (8U)
#define XWOSCFG_SKD_IDLE_STACK_SIZE                     (8192U)
#define XWOSCFG_SKD_IDLE_TLS                            1
#define XWOSCFG_SKD_BH                                  0
#define XWOSCFG_SKD_BH_STACK_SIZE                       (8192U)
#define XWOSCFG_SKD_BH_TLS                              1
#define XWOSCFG_SKD_PM                                  1 /* UP Only */
#define XWOSCFG_SKD_THD_PRIVILEGED_DEFAULT              1
#define XWOSCFG_SKD_THD_MEMPOOL                         0
#define XWOSCFG_SKD_THD_MEMSLICE                        0
#define XWOSCFG_SKD_THD_SMA                             0
#define XWOSCFG_SKD_THD_STDC_MM                         1
#define XWOSCFG_SKD_THD_LOCAL_DATA_NUM                  (2U)
#define XWOSCFG_SKD_THD_EXIT                            1
#define XWOSCFG_SKD_THD_CXX                             1
#define XWOSCFG_SKD_SWT                                 1
#define XWOSCFG_SKD_SWT_MEMPOOL                         0
#define XWOSCFG_SKD_SWT_MEMSLICE                        0
#define XWOSCFG_SKD_SWT_SMA                             0
#define XWOSCFG_SKD_SWT_STDC_MM                         1
#define XWOSCFG_SKD_SWT_CXX                             1

/******** ******** ******** sync ******** ******** ********/
#define XWOSCFG_SYNC_PLSEM                              1
#define XWOSCFG_SYNC_RTSEM                              1
#define XWOSCFG_SYNC_SEM_MEMPOOL                        0
#define XWOSCFG_SYNC_SEM_MEMSLICE                       0
#define XWOSCFG_SYNC_SEM_SMA                            0
#define XWOSCFG_SYNC_SEM_STDC_MM                        1
#define XWOSCFG_SYNC_SEM_CXX                            1

#define XWOSCFG_SYNC_COND                               1
#define XWOSCFG_SYNC_COND_MEMPOOL                       0
#define XWOSCFG_SYNC_COND_MEMSLICE                      0
#define XWOSCFG_SYNC_COND_SMA                           0
#define XWOSCFG_SYNC_COND_STDC_MM                       1
#define XWOSCFG_SYNC_COND_CXX                           1

#define XWOSCFG_SYNC_EVT                                1
#define XWOSCFG_SYNC_EVT_MEMPOOL                        0
#define XWOSCFG_SYNC_EVT_MEMSLICE                       0
#define XWOSCFG_SYNC_EVT_SMA                            0
#define XWOSCFG_SYNC_EVT_STDC_MM                        1
#define XWOSCFG_SYNC_EVT_CXX                            1

/******** ******** ******** lock ******** ******** ********/
#define XWOSCFG_LOCK_MTX                                1
#define XWOSCFG_LOCK_MTX_MEMPOOL                        0
#define XWOSCFG_LOCK_MTX_MEMSLICE                       0
#define XWOSCFG_LOCK_MTX_SMA                            0
#define XWOSCFG_LOCK_MTX_STDC_MM                        1
#define XWOSCFG_LOCK_FAKEMTX                            0
#define XWOSCFG_LOCK_MTX_CXX                            1

#define XWOSCFG_LOCK_SPLK_CXX                           1
#define XWOSCFG_LOCK_SQLK_CXX                           1

#endif /* cfg/xwos.h */
