/**
 * @file
 * @brief AUTOSAR::CP::Os::Integration
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "bm/autosarcp/os/OsCfg.h"

void AutosarCp_OsCfg_Init(void)
{
        Rte_TickCounter_Init();
        Rte_Task_Init();
        Rte_ScheduleTable_Init();
}
