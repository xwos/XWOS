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

#include "bm/autosarcp/os/CfgTask.h"
#include <xwos/lib/xwlog.h>

#define LOGTAG "OsCfgTask"

__xwcc_alignl1cache xwu8_t OsCycleTask_Bsw10ms_Stack[4096];
const struct xwarcos_task_config OsCycleTask_Bsw10ms_Config = {
        .priority = 1,
        .activations = 1,
        .preemptability = XWARCOS_TASK_PREEMPTABILITY_FULL,
        .stack = (xwstk_t *)OsCycleTask_Bsw10ms_Stack,
        .stack_size = sizeof(OsCycleTask_Bsw10ms_Stack),
};
struct xwarcos_task OsCycleTask_Bsw10ms_Object = {
        .cfg = &OsCycleTask_Bsw10ms_Config,
        .func = OsCycleTask_Bsw10ms_Func,
        .splk = XWOS_SPLK_INITIALIZER,
        .state = SUSPENDED,
};
TaskType OsCycleTask_Bsw10ms = &OsCycleTask_Bsw10ms_Object;

void Rte_Task_Init(void)
{
        xwer_t xwrc;

        xwrc = xwarcos_task_validate_and_init(OsCycleTask_Bsw10ms);
        if (xwrc < 0) {
                xwlogf(INFO, LOGTAG, "Init OsCycleTask_Bsw10ms ... %d\r\n", xwrc);
        }
}
