/**
 * @file
 * @brief xwmd设备栈：LIN总线ID校验表
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_lin_id_h__
#define __xwmd_ds_lin_id_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define LIN_ID_PARITY(id)       ((((~(GETBIT((id), 1U) ^ GETBIT((id), 3U) ^           \
                                     GETBIT((id), 4U) ^ GETBIT((id), 5U))) << 7U) |   \
                                 ((GETBIT((id), 0U) ^ GETBIT((id), 1U) ^              \
                                   GETBIT((id), 2U) ^ GETBIT((id), 4U)) << 6U)) &     \
                                 0xC0U)
#define LIN_ID_PARITY_FIELD(id) (LIN_ID_PARITY(id) | ((id) & 0xFFU))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwds_rodata const unsigned char xwos_linid_table[64];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********  inline functions implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/lin/id.h */
