/**
 * @file
 * @brief 内核配置
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

#ifndef __cfg_xwos_h__
#define __cfg_xwos_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********        general Configuration        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWKNCFG_CHECK_PARAMETERS                        0
#define XWKNCFG_BUG                                     1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         xwlib Configuration         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLIBCFG_LOG                                    0
#define XWLIBCFG_XWLOG_LEVEL                            (0U)
#define XWLIBCFG_XWLOG_BUFSIZE                          (128U)

#define XWLIBCFG_XWBOP_FFS8                             1
#define XWLIBCFG_XWBOP_FLS8                             1
#define XWLIBCFG_XWBOP_RBIT8                            1
#define XWLIBCFG_XWBOP_FFS16                            1
#define XWLIBCFG_XWBOP_FLS16                            1
#define XWLIBCFG_XWBOP_RBIT16                           1
#define XWLIBCFG_XWBOP_RE16                             1
#define XWLIBCFG_XWBOP_RE16S32                          1
#define XWLIBCFG_XWBOP_FFS32                            1
#define XWLIBCFG_XWBOP_FLS32                            1
#define XWLIBCFG_XWBOP_RBIT32                           1
#define XWLIBCFG_XWBOP_RE32                             1
#define XWLIBCFG_XWBOP_RE32S64                          1
#define XWLIBCFG_XWBOP_FFS64                            1
#define XWLIBCFG_XWBOP_FLS64                            1
#define XWLIBCFG_XWBOP_RBIT64                           1
#define XWLIBCFG_XWBOP_RE64                             1

#define XWLIBCFG_XWAOP8                                 1
#define XWLIBCFG_XWAOP16                                1
#define XWLIBCFG_XWAOP32                                1
#define XWLIBCFG_XWAOP64                                1
#define XWLIBCFG_XWBMPAOP                               1

#define XWLIBCFG_MAP                                    1

#define XWLIBCFG_CRC32                                  1
#define XWLIBCFG_CRC32_0X04C11DB7                       1
#define XWLIBCFG_CRC32_0XEDB88320                       1
#define XWLIBCFG_CRC8                                   0
#define XWLIBCFG_CRC8_0X07                              1
#define XWLIBCFG_CRC8_0X31                              0
#define XWLIBCFG_CRC8_0X9B                              0

/******** ******** ******** ******** ******** ******** ******** ********
 ********        general memory management Configuration        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** common ********/
#define XWMMCFG_ALIGNMENT                               (8U)
#define XWMMCFG_STACK_ALIGNMENT                         (8U)
#define XWMMCFG_FD_STACK                                1
#define XWMMCFG_ED_STACK                                0
#define XWMMCFG_FA_STACK                                0
#define XWMMCFG_EA_STACK                                0

/******** memslice ********/
#define XWMMCFG_MEMSLICE                                1

/******** buddy algorithm memory allocater ********/
#define XWMMCFG_BMA                                     1

/******** mempool ********/
#define XWMMCFG_MEMPOOL                                 0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          SMP Configuration          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** irqc ******** ******** ********/

/******** ******** ******** syshwt ******** ******** ********/
#define XWSMPCFG_SYSHWT_PERIOD                          ((xwtm_t)1000000)

/******** ******** ******** memory management ******** ******** ********/

/******** ******** ******** scheduler ******** ******** ********/
#define XWSMPCFG_SD_PRIORITY_RT_NUM                     (8U)
#define XWSMPCFG_SD_IDLE_STACK_SIZE                     (1024U)
#define XWSMPCFG_SD_BH                                  0
#define XWSMPCFG_SD_BH_STACK_SIZE                       (2048U)
#define XWSMPCFG_SD_TCB_MEMSLICE                        0
#define XWSMPCFG_SD_SWT                                 1
#define XWSMPCFG_SD_SWT_MEMSLICE                        0

/******** ******** ******** sync ******** ******** ********/
#define XWSMPCFG_SYNC_PLSMR                             1
#define XWSMPCFG_SYNC_RTSMR                             1
#define XWSMPCFG_SYNC_SMR_MEMSLICE                      0

#define XWSMPCFG_SYNC_CDT_MEMSLICE                      0

#define XWSMPCFG_SYNC_EVT                               1
#define XWSMPCFG_SYNC_EVT_MAXNUM                        32
#define XWSMPCFG_SYNC_EVT_MEMSLICE                      0

/******** ******** ******** lock ******** ******** ********/
#define XWSMPCFG_LOCK_MTX_MEMSLICE                      0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          UP Configuration           ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** irqc ******** ******** ********/

/******** ******** ******** syshwt ******** ******** ********/
/* #define XWUPCFG_SYSHWT_PERIOD                           ((xwtm_t)1000000) */

/******** ******** ******** memory management ******** ******** ********/

/******** ******** ******** scheduler ******** ******** ********/
/* #define XWUPCFG_SD_PRIORITY_RT_NUM                      (4U) */
/* #define XWUPCFG_SD_IDLE_STACK_SIZE                      (512U) */
/* #define XWUPCFG_SD_BH                                   1 */
/* #define XWUPCFG_SD_BH_STACK_SIZE                        (1024U) */
/* #define XWUPCFG_SD_PM                                   1 */
/* #define XWUPCFG_SD_THRD_EXIT                            1 */
/* #define XWUPCFG_SD_SWT                                  1 */

/******** ******** ******** sync ******** ******** ********/
/* #define XWUPCFG_SYNC_PLSMR                              1 */
/* #define XWUPCFG_SYNC_RTSMR                              1 */

/* #define XWUPCFG_SYNC_CDT                                1 */

/* #define XWUPCFG_SYNC_EVT                                1 */
/* #define XWUPCFG_SYNC_EVT_MAXNUM                         32 */

/******** ******** ******** lock ******** ******** ********/
/* #define XWUPCFG_LOCK_MTX                                1 */
/* #define XWUPCFG_LOCK_FAKEMTX                            0 */

#endif /* cfg/xwos.h */
