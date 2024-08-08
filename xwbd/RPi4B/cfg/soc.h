/**
 * @file
 * @brief 片上系统描述层(SDL)配置
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

#ifndef __cfg_soc_h__
#define __cfg_soc_h__

/******** ******** SOC ******** ********/
#define SOCCFG_PERI_HIGH                        1
#define SOCCFG_EXC_NUM                          64U
#define SOCCFG_IRQ_NUM                          512U

/******** ******** Debug ******** ********/
#define SOCCFG_DEBUG_LV                         3U
#define SOCCFG_BUG                              1
#define SOCCFG_DBGLOGBUF_SIZE                   512U

#endif /* cfg/soc.h */
