/**
 * @file
 * @brief 中间件模块配置
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

#ifndef __cfg_xwmd_h__
#define __cfg_xwmd_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********    Base Config    ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_CHECK_PARAMETERS                                1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     Inter-System Communication      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** point to point communication protocol ******** ********/
#define XWMDCFG_isc_xwpcp                                       1
#define XWMDCFG_isc_xwpcp_LOG                                   0
#define XWMDCFG_isc_xwpcp_PORT_NUM                              (12U)
#define XWMDCFG_isc_xwpcp_PRI_NUM                               (4U)
#define XWMDCFG_isc_xwpcp_RETRY_PERIOD                          (1 * XWTM_S)
#define XWMDCFG_isc_xwpcp_RETRY_NUM                             (8U)
#define XWMDCFG_isc_xwpcp_MEMBLK_SIZE                           (64U)
#define XWMDCFG_isc_xwpcp_MEMBLK_ODR                            (6U)
#define XWMDCFG_isc_xwpcp_RXTHD_PRIORITY                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
#define XWMDCFG_isc_xwpcp_TXTHD_PRIORITY                        \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

/******** ******** simple communication protocol ******** ********/
#define XWMDCFG_isc_xwscp                                       1
#define XWMDCFG_isc_xwscp_LOG                                   0
#define XWMDCFG_isc_xwscp_PERIOD                                (1 * XWTM_S)
#define XWMDCFG_isc_xwscp_MEMBLK_SIZE                           (64U)
#define XWMDCFG_isc_xwscp_MEMBLK_ODR                            (6U)
#define XWMDCFG_isc_xwscp_THD_PRIORITY                          \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

/******** ******** Message Queue ******** ********/
#define XWMDCFG_isc_xwmq                                        1
#define XWMDCFG_isc_xwmq_LOG                                    0
#define XWMDCFG_isc_xwmq_MEMSLICE                               0
#define XWMDCFG_isc_xwmq_STDC_MM                                1

/******** ******** Circular Queue ******** ********/
#define XWMDCFG_isc_xwcq                                        0
#define XWMDCFG_isc_xwcq_LOG                                    0
#define XWMDCFG_isc_xwcq_STDC_MM                                1

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********            C/C++ Runtime            ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWMDCFG_libc_newlibac                                   0
#define XWMDCFG_cxx                                             0

#endif /* cfg/xwmd.h */
