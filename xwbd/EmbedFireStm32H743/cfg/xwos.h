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
 ******** ********        Common Configurations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWKNCFG_CHECK_PARAMETERS                        1
#define XWKNCFG_BUG                                     1
#define XWKNCFG_RELOCATE_DATA                           1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         xwlib Configurations        ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWLIBCFG_LOG                                    1
#define XWLIBCFG_XWLOG_LEVEL                            (0U)
#define XWLIBCFG_XWLOG_BUFSIZE                          (256U)

#define XWLIBCFG_SC                                     1

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

#define XWLIBCFG_SETJMP                                 1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   Memory Management Configurations  ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** common ********/
#define XWMMCFG_STACK_ALIGNMENT                         (8U)
#define XWMMCFG_STACK_WATERMARK                         (8U)
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
 ******** ********          MP Configurations          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** irqc ******** ******** ********/

/******** ******** ******** syshwt ******** ******** ********/
#define XWMPCFG_SYSHWT_PERIOD                           ((xwtm_t)1000000)

/******** ******** ******** memory management ******** ******** ********/

/******** ******** ******** scheduler ******** ******** ********/
#define XWMPCFG_SKD_PRIORITY_RT_NUM                     (8U)
#define XWMPCFG_SKD_IDLE_STACK_SIZE                     (2048U)
#define XWMPCFG_SKD_BH                                  1
#define XWMPCFG_SKD_BH_STACK_SIZE                       (2048U)
#define XWMPCFG_SKD_THD_MEMSLICE                        1
#define XWMPCFG_SKD_THD_STDC_MM                         0
#define XWMPCFG_SKD_THD_LOCAL_DATA_NUM                  (2U)
#define XWMPCFG_SKD_SWT                                 1
#define XWMPCFG_SKD_SWT_MEMSLICE                        1
#define XWMPCFG_SKD_SWT_STDC_MM                         0

/******** ******** ******** sync ******** ******** ********/
#define XWMPCFG_SYNC_PLSEM                              0
#define XWMPCFG_SYNC_RTSEM                              1
#define XWMPCFG_SYNC_SEM_MEMSLICE                       1
#define XWMPCFG_SYNC_SEM_STDC_MM                        0

#define XWMPCFG_SYNC_COND_MEMSLICE                      1
#define XWMPCFG_SYNC_COND_STDC_MM                       0

#define XWMPCFG_SYNC_EVT                                1
#define XWMPCFG_SYNC_EVT_STDC_MM                        0

/******** ******** ******** lock ******** ******** ********/
#define XWMPCFG_LOCK_MTX_MEMSLICE                       1
#define XWMPCFG_LOCK_MTX_STDC_MM                        0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********          UP Configurations          ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** irqc ******** ******** ********/

/******** ******** ******** syshwt ******** ******** ********/
/* #define XWUPCFG_SYSHWT_PERIOD                        ((xwtm_t)1000000) */

/******** ******** ******** memory management ******** ******** ********/

/******** ******** ******** scheduler ******** ******** ********/
/* #define XWUPCFG_SKD_PRIORITY_RT_NUM                  (4U) */
/* #define XWUPCFG_SKD_IDLE_STACK_SIZE                  (2048U) */
/* #define XWUPCFG_SKD_BH                               1 */
/* #define XWUPCFG_SKD_BH_STACK_SIZE                    (2048U) */
/* #define XWUPCFG_SKD_PM                               1 */
/* #define XWUPCFG_SKD_THD_STDC_MM                      0 */
/* #define XWUPCFG_SKD_THD_LOCAL_DATA_NUM               (2U) */
/* #define XWUPCFG_SKD_THD_EXIT                         1 */
/* #define XWUPCFG_SKD_SWT                              1 */
/* #define XWUPCFG_SKD_SWT_STDC_MM                      0 */

/******** ******** ******** sync ******** ******** ********/
/* #define XWUPCFG_SYNC_PLSEM                           1 */
/* #define XWUPCFG_SYNC_PLSEM_STDC_MM                   0 */
/* #define XWUPCFG_SYNC_RTSEM                           1 */
/* #define XWUPCFG_SYNC_RTSEM_STDC_MM                   0 */

/* #define XWUPCFG_SYNC_COND                            1 */
/* #define XWUPCFG_SYNC_COND_STDC_MM                    0 */

/* #define XWUPCFG_SYNC_EVT                             1 */
/* #define XWUPCFG_SYNC_EVT_STDC_MM                     0 */

/******** ******** ******** lock ******** ******** ********/
/* #define XWUPCFG_LOCK_MTX                             1 */
/* #define XWUPCFG_LOCK_MTX_STDC_MM                     0 */
/* #define XWUPCFG_LOCK_FAKEMTX                         0 */

#endif /* cfg/xwos.h */
