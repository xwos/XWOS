/**
 * @file
 * @brief 中间件模块配置
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

#ifndef __cfg_xwmd_h__
#define __cfg_xwmd_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    Base Config    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_CHECK_PARAMETERS                                1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     Inter-System Communication      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** SOC to SOC Communication ******** ********/
#define XWMDCFG_isc_xwssc                                       1
#define XWMDCFG_isc_xwssc_LOG                                   0
#define XWMDCFG_isc_xwssc_PORT_NUM                              (12U)
#define XWMDCFG_isc_xwssc_PRI_NUM                               (4U)
#define XWMDCFG_isc_xwssc_RETRY_PERIOD                          (XWTM_MS(1000))
#define XWMDCFG_isc_xwssc_RETRY_NUM                             (8U)
#define XWMDCFG_isc_xwssc_MEMBLK_SIZE                           (64U)
#define XWMDCFG_isc_xwssc_MEMBLK_ODR                            (6U)
#define XWMDCFG_isc_xwssc_RXTHD_PRIORITY                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
#define XWMDCFG_isc_xwssc_TXTHD_PRIORITY                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

/******** ******** Message Queue ******** ********/
#define XWMDCFG_isc_xwmq                                        1

/******** ******** Circular Queue ******** ********/
#define XWMDCFG_isc_xwcq                                        1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      AUTOSAR classical Platform     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** OS ******** ********/
#define XWMDCFG_autosarcp_os                                    1
#define XWMDCFG_autosarcp_os_USING_STDTYPES_H                   0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********            C/C++ Runtime            ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_libc                                            1
#define XWMDCFG_libc_newlibac_SETJMP                            0
#define XWMDCFG_libc_newlibac_MEM                               1
#define XWMDCFG_libc_newlibac_SPRINTF                           0
#define XWMDCFG_libc_newlibac_FOPS                              1
#define XWMDCFG_libc_newlibac_FATFS                             0
#define XWMDCFG_libc_newlibac_TIME                              0
#define XWMDCFG_libc_picolibcac_SETJMP                          0
#define XWMDCFG_libc_picolibcac_MEM                             1
#define XWMDCFG_libc_picolibcac_SPRINTF                         1
#define XWMDCFG_libc_picolibcac_FOPS                            1
#define XWMDCFG_libc_picolibcac_FATFS                           0
#define XWMDCFG_libc_picolibcac_TIME                            0

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********            XWRust Runtime            ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_xwrust_ffi                                      1

#endif /* cfg/xwmd.h */
