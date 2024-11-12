/**
 * @file
 * @brief AUTOSAR::CP::Os::Cfg::Task
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef bm_autosarcp_os_CfgTask_h__
#define bm_autosarcp_os_CfgTask_h__

#include <xwmd/autosarcp/os/Task.h>

DeclareTask(OsCycleTask_Bsw10ms);

extern TASK(OsCycleTask_Bsw10ms);

extern void Rte_Task_Init(void);

#endif /* bm/autosarcp/os/CfgTask.h */
