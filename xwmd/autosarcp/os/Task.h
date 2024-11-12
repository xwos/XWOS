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

#ifndef xwmd_autosarcp_os_Task_h__
#define xwmd_autosarcp_os_Task_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwos/arcos/task.h>

typedef enum xwarcos_task_state_em TaskStateType;
#define SUSPENDED XWARCOS_TASK_STATE_SUSPENDED
#define READY XWARCOS_TASK_STATE_READY
#define WAITING XWARCOS_TASK_STATE_WAITING
#define RUNNING XWARCOS_TASK_STATE_RUNNING
typedef TaskStateType * TaskStateRefType;

typedef struct xwarcos_task * TaskType;
typedef TaskType * TaskRefType;
#define DeclareTask(x) extern TaskType x
#define TASK(x) void x##_Func(void)

StatusType ActivateTask(TaskType TaskID);
StatusType TerminateTask(void);
StatusType ChainTask(TaskType TaskID);
StatusType GetTaskID(TaskRefType TaskID);
StatusType GetTaskState(TaskType TaskID, TaskStateRefType State);

#endif /* xwmd/autosarcp/os/Task.h */
