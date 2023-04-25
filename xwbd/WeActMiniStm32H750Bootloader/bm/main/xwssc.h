/**
 * @file
 * @brief 主模块：xwssc子模块
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

#ifndef __bm_main_xwssc_h__
#define __bm_main_xwssc_h__

#include <xwos/standard.h>
#include <xwmd/isc/xwssc/mif.h>

xwer_t bm_xwssc_start(void);
xwer_t bm_xwssc_stop(void);
xwer_t bm_xwssc_tx(const xwu8_t data[], xwsz_t * size, xwtm_t to);
xwer_t bm_xwssc_rx(xwu8_t rxbuf[], xwsz_t * size, xwtm_t to);

#endif /* bm/main/xwssc.h */
