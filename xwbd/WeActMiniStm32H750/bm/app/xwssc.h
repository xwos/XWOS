/**
 * @file
 * @brief 板级描述层：用户程序：xwssc
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

#ifndef __bm_app_xwssc_h__
#define __bm_app_xwssc_h__

#include "board/std.h"

xwer_t board_xwssc_start(void);
xwer_t board_xwssc_stop(void);
xwer_t board_xwssc_tx(const xwu8_t data[], xwsz_t * size, xwu8_t port, xwtm_t to);
xwer_t board_xwssc_rx(xwu8_t rxbuf[], xwsz_t * size, xwu8_t port, xwtm_t to);

#endif /* bm/app/xwssc.h */
