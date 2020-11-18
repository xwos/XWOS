/**
 * @file
 * @brief 玄武设备栈：LIN总线ID校验表
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

#ifndef __xwcd_ds_lin_id_h__
#define __xwcd_ds_lin_id_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>

#define LIN_ID_PARITY(id)       ((((~(GETBIT((id), 1U) ^ GETBIT((id), 3U) ^           \
                                     GETBIT((id), 4U) ^ GETBIT((id), 5U))) << 7U) |   \
                                 ((GETBIT((id), 0U) ^ GETBIT((id), 1U) ^              \
                                   GETBIT((id), 2U) ^ GETBIT((id), 4U)) << 6U)) &     \
                                 0xC0U)
#define LIN_ID_PARITY_FIELD(id) (LIN_ID_PARITY(id) | ((id) & 0xFFU))

extern __xwds_rodata const unsigned char xwos_linid_table[64];

#endif /* xwcd/ds/lin/id.h */
