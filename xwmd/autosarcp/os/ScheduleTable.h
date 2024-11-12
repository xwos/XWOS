/**
 * @file
 * @brief AUTOSAR::CP::Os::ScheduleTable
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef xwmd_autosarcp_os_ScheduleTable_h__
#define xwmd_autosarcp_os_ScheduleTable_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwmd/autosarcp/os/Counter.h>
#include <xwos/arcos/schedtbl.h>

typedef enum xwarcos_schedtbl_status_em ScheduleTableStatusType;
#define SCHEDULETABLE_STOPPED XWARCOS_SCHEDTBL_STATUS_STOPPED
#define SCHEDULETABLE_NEXT XWARCOS_SCHEDTBL_STATUS_NEXT
#define SCHEDULETABLE_RUNNING XWARCOS_SCHEDTBL_STATUS_RUNNING
typedef ScheduleTableStatusType * ScheduleTableStatusRefType;

typedef struct xwarcos_schedtbl * ScheduleTableType;
typedef ScheduleTableType * ScheduleTableRefType;
#define DeclareScheduleTable(x) extern ScheduleTableType x

StatusType StartScheduleTableRel(ScheduleTableType ScheduleTableID, TickType Offset);
StatusType StartScheduleTableAbs(ScheduleTableType ScheduleTableID, TickType Start);
StatusType StopScheduleTable(ScheduleTableType ScheduleTableID);
StatusType GetScheduleTableStatus(ScheduleTableType ScheduleTableID,
                                  ScheduleTableStatusRefType ScheduleStatus);

#endif /* xwmd/autosarcp/os/ScheduleTable.h */
