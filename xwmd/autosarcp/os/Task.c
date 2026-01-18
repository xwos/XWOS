/**
 * @file
 * @brief AUTOSAR::CP::Os::Task
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Task.h>
#include <xwmd/autosarcp/os/Error.h>

extern struct xwarcos_task * const xwarcos_task_table[];
extern const xwu32_t xwarcos_task_num;

StatusType ActivateTask(TaskType TaskID)
{
        xwer_t xwrc;
        struct xwarcos_task * task;

        if (TaskID >= xwarcos_task_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                task = xwarcos_task_table[TaskID];
                xwrc = xwarcos_task_activate(task);
        }
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
        struct xwarcos_task * task;

        if (TaskID >= xwarcos_task_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                task = xwarcos_task_table[TaskID];
                xwrc = xwarcos_task_chain(task);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetTaskID(TaskRefType TaskID)
{
        xwer_t xwrc;
        struct xwarcos_task * task;

        if (NULL != TaskID) {
                xwrc = xwarcos_task_get(&task);
                if (XWOK == xwrc) {
                        if (NULL != task) {
                                *TaskID = task->id;
                        } else {
                                xwrc = E_XWARCOS_ACCESS;
                        }
                }
        } else {
                xwrc = E_XWARCOS_ILLEGAL_ADDRESS;
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetTaskState(TaskType TaskID, TaskStateRefType State)
{

        xwer_t xwrc;
        struct xwarcos_task * task;

        if (TaskID >= xwarcos_task_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                task = xwarcos_task_table[TaskID];
                xwrc = xwarcos_task_get_state(task, State);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
