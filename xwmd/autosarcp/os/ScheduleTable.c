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

#include <xwmd/autosarcp/os/ScheduleTable.h>
#include <xwmd/autosarcp/os/Error.h>

StatusType StartScheduleTableRel(ScheduleTableType ScheduleTableID, TickType Offset)
{
        xwer_t xwrc;

        xwrc = xwarcos_schedtbl_start_rel(ScheduleTableID, Offset);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType StartScheduleTableAbs(ScheduleTableType ScheduleTableID, TickType Start)
{
        xwer_t xwrc;

        xwrc = xwarcos_schedtbl_start_abs(ScheduleTableID, Start);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType StopScheduleTable(ScheduleTableType ScheduleTableID)
{
        xwer_t xwrc;

        xwrc = xwarcos_schedtbl_stop(ScheduleTableID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetScheduleTableStatus(ScheduleTableType ScheduleTableID,
                                  ScheduleTableStatusRefType ScheduleStatus)
{
        xwer_t xwrc;

        xwrc = xwarcos_schedtbl_get_status(ScheduleTableID, ScheduleStatus);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
