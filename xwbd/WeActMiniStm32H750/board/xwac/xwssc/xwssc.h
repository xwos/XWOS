/**
 * @file
 * @brief 板级描述层：XWOS适配层：XWSSC
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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

#ifndef board_xwac_xwssc_xwssc_h__
#define board_xwac_xwssc_xwssc_h__

#include "board/std.h"
#include <xwmd/isc/xwssc/mi.h>

extern struct xwssc xwssc3;

void xwssc3_init(void);
void xwssc3_fini(void);

#endif /* board/xwac/xwssc/xwssc.h */
