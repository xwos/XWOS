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

extern struct xwarcos_schedtbl * const xwarcos_schedtbl_table[];
extern const xwu32_t xwarcos_schedtbl_num;

StatusType StartScheduleTableRel(ScheduleTableType ScheduleTableID, TickType Offset)
{
        xwer_t xwrc;
        struct xwarcos_schedtbl * schedtbl;

        if (ScheduleTableID >= xwarcos_schedtbl_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                schedtbl = xwarcos_schedtbl_table[ScheduleTableID];
                xwrc = xwarcos_schedtbl_start_rel(schedtbl, Offset);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType StartScheduleTableAbs(ScheduleTableType ScheduleTableID, TickType Start)
{
        xwer_t xwrc;
        struct xwarcos_schedtbl * schedtbl;

        if (ScheduleTableID >= xwarcos_schedtbl_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                schedtbl = xwarcos_schedtbl_table[ScheduleTableID];
                xwrc = xwarcos_schedtbl_start_abs(schedtbl, Start);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType StopScheduleTable(ScheduleTableType ScheduleTableID)
{
        xwer_t xwrc;
        struct xwarcos_schedtbl * schedtbl;

        if (ScheduleTableID >= xwarcos_schedtbl_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                schedtbl = xwarcos_schedtbl_table[ScheduleTableID];
                xwrc = xwarcos_schedtbl_stop(schedtbl);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetScheduleTableStatus(ScheduleTableType ScheduleTableID,
                                  ScheduleTableStatusRefType ScheduleStatus)
{
        xwer_t xwrc;
        struct xwarcos_schedtbl * schedtbl;

        if (ScheduleTableID >= xwarcos_schedtbl_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                schedtbl = xwarcos_schedtbl_table[ScheduleTableID];
                xwrc = xwarcos_schedtbl_get_status(schedtbl, ScheduleStatus);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
