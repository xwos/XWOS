/**
 * @file
 * @brief 玄武设备栈：LIN：ID校验表
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

#ifndef __xwcd_ds_lin_id_h__
#define __xwcd_ds_lin_id_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>

/**
 * @defgroup xwcd_ds_lin LIN控制器
 * @ingroup xwcd_ds
 * @{
 */

#define LIN_ID_PARITY(id) ((((~(XWBOP_TBIT((id), 1U) ^ XWBOP_TBIT((id), 3U) ^ \
                                XWBOP_TBIT((id), 4U) ^ XWBOP_TBIT((id), 5U))) << 7U) | \
                            ((XWBOP_TBIT((id), 0U) ^ XWBOP_TBIT((id), 1U) ^ \
                              XWBOP_TBIT((id), 2U) ^ XWBOP_TBIT((id), 4U)) << 6U)) & \
                           0xC0U)
#define LIN_ID_PARITY_FIELD(id) (LIN_ID_PARITY(id) | ((id) & 0xFFU))

/**
 * @brief LIN总线ID校验表
 */
extern __xwds_rodata const unsigned char xwos_linid_table[64];

/**
 * @} xwcd_ds_lin
 */

#endif /* xwcd/ds/lin/id.h */
