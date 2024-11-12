/**
 * @file
 * @brief AUTOSAR::CP::Os::Cfg::ScheduleTable
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef bm_autosarcp_os_CfgScheduleTable_h__
#define bm_autosarcp_os_CfgScheduleTable_h__

#include <xwmd/autosarcp/os/ScheduleTable.h>

DeclareScheduleTable(Rte_ScheduleTable);

extern void Rte_ScheduleTable_Init(void);

#endif /* bm/autosarcp/os/CfgScheduleTable.h */
