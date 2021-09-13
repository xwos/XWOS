/**
 * @file
 * @brief 玄武设备栈：SOC外部RAM
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

#ifndef __xwcd_ds_soc_eram_h__
#define __xwcd_ds_soc_eram_h__

#include <xwcd/ds/standard.h>
#include <xwcd/ds/soc/chip.h>

xwer_t xwds_eram_test(struct xwds_soc * soc, xwptr_t * erraddr);

#endif /* xwcd/ds/soc/eram.h */
