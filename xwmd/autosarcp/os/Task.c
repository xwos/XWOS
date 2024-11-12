/**
 * @file
 * @brief AUTOSAR::CP::Os::Task
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Task.h>
#include <xwmd/autosarcp/os/Error.h>

StatusType ActivateTask(TaskType TaskID)
{
        xwer_t xwrc;

        xwrc = xwarcos_task_activate(TaskID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType TerminateTask(void)
{
        xwer_t xwrc;

        xwrc = xwarcos_task_terminate();
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType ChainTask(TaskType TaskID)
{
        xwer_t xwrc;

        xwrc = xwarcos_task_chain(TaskID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetTaskID(TaskRefType TaskID)
{
        xwer_t xwrc;

        xwrc = xwarcos_task_get(TaskID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetTaskState(TaskType TaskID, TaskStateRefType State)
{

        xwer_t xwrc;

        xwrc = xwarcos_task_get_state(TaskID, State);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
