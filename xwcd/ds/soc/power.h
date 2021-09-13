/**
 * @file
 * @brief 玄武设备栈：电源
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

#ifndef __xwcd_ds_soc_power_h__
#define __xwcd_ds_soc_power_h__

#include <xwcd/ds/standard.h>
#include <xwcd/ds/soc/chip.h>

xwer_t xwds_pwr_req(struct xwds_soc * soc, xwid_t id);
xwer_t xwds_pwr_rls(struct xwds_soc * soc, xwid_t id);
xwer_t xwds_pwr_getvltg(struct xwds_soc * soc, xwid_t id,
                        xwu32_t * buf, xwsz_t * num);

#endif /* xwcd/ds/soc/power.h */
