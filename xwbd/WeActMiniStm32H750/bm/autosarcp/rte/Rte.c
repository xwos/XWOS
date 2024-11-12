/**
 * @file
 * @brief AUTOSAR::CP::Rte
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "bm/autosarcp/rte/Rte.h"
#include "bm/autosarcp/os/OsCfg.h"
#include <xwos/lib/xwlog.h>

void Rte_Init(void)
{
        AutosarCp_OsCfg_Init();
}

void Rte_Start(void)
{
        StatusType st;

        st = StartScheduleTableAbs(Rte_ScheduleTable, 0);
        if (E_OK != st) {
                xwlogf(E, "Rte",
                       "StartScheduleTableAbs(Rte_ScheduleTable) ... st:%d\r\n", st);
        }
}

xwu32_t count = 0;

TASK(OsCycleTask_Bsw10ms)
{
        count++;
        if (0 == (count % 100)) {
                xwlogf(I, "OsCycleTask_Bsw10ms", "count = %d\r\n", count);
        }
        TerminateTask();
}
