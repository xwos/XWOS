/**
 * @file
 * @brief AUTOSAR::CP::Os::Cfg::ScheduleTable
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "bm/autosarcp/os/CfgScheduleTable.h"
#include "bm/autosarcp/os/CfgCounter.h"
#include "bm/autosarcp/os/CfgTask.h"
#include <xwos/lib/xwlog.h>

#define LOGTAG "OsCfgScheduleTable"

void Rte_ScheduleTable_Ep0_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
        ActivateTask(OsCycleTask_Bsw10ms);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep0_Actions[] = {
        [0] = Rte_ScheduleTable_Ep0_Action0,
};

void Rte_ScheduleTable_Ep1_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep1_Actions[] = {
        [0] = Rte_ScheduleTable_Ep1_Action0,
};

void Rte_ScheduleTable_Ep2_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep2_Actions[] = {
        [0] = Rte_ScheduleTable_Ep2_Action0,
};

void Rte_ScheduleTable_Ep3_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep3_Actions[] = {
        [0] = Rte_ScheduleTable_Ep3_Action0,
};

void Rte_ScheduleTable_Ep4_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep4_Actions[] = {
        [0] = Rte_ScheduleTable_Ep4_Action0,
};

void Rte_ScheduleTable_Ep5_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep5_Actions[] = {
        [0] = Rte_ScheduleTable_Ep5_Action0,
};

void Rte_ScheduleTable_Ep6_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep6_Actions[] = {
        [0] = Rte_ScheduleTable_Ep6_Action0,
};

void Rte_ScheduleTable_Ep7_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep7_Actions[] = {
        [0] = Rte_ScheduleTable_Ep7_Action0,
};

void Rte_ScheduleTable_Ep8_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep8_Actions[] = {
        [0] = Rte_ScheduleTable_Ep8_Action0,
};

void Rte_ScheduleTable_Ep9_Action0(struct xwarcos_schedtbl * schedtbl,
                                   const struct xwarcos_schedtbl_ep * ep)
{
        XWOS_UNUSED(schedtbl);
        XWOS_UNUSED(ep);
}

const xwarcos_schedtbl_action_f Rte_ScheduleTable_Ep9_Actions[] = {
        [0] = Rte_ScheduleTable_Ep9_Action0,
};

const struct xwarcos_schedtbl_ep Rte_ScheduleTable_Ep[10] = {
        [0] = {
                .offset = 0,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep0_Actions),
                .actions = Rte_ScheduleTable_Ep0_Actions,
        },
        [1] = {
                .offset = 1,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep1_Actions),
                .actions = Rte_ScheduleTable_Ep1_Actions,
        },
        [2] = {
                .offset = 2,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep2_Actions),
                .actions = Rte_ScheduleTable_Ep2_Actions,
        },
        [3] = {
                .offset = 3,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep3_Actions),
                .actions = Rte_ScheduleTable_Ep3_Actions,
        },
        [4] = {
                .offset = 4,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep4_Actions),
                .actions = Rte_ScheduleTable_Ep4_Actions,
        },
        [5] = {
                .offset = 5,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep5_Actions),
                .actions = Rte_ScheduleTable_Ep5_Actions,
        },
        [6] = {
                .offset = 6,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep6_Actions),
                .actions = Rte_ScheduleTable_Ep6_Actions,
        },
        [7] = {
                .offset = 7,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep7_Actions),
                .actions = Rte_ScheduleTable_Ep7_Actions,
        },
        [8] = {
                .offset = 8,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep8_Actions),
                .actions = Rte_ScheduleTable_Ep8_Actions,
        },
        [9] = {
                .offset = 9,
                .action_number = xw_array_size(Rte_ScheduleTable_Ep9_Actions),
                .actions = Rte_ScheduleTable_Ep9_Actions,
        },
};

const struct xwarcos_schedtbl_config Rte_ScheduleTable_cfg = {
        .counter = &Rte_TickCounter_Object,
        .repeating = true,
        .duration = 10,
        .ep_number = xw_array_size(Rte_ScheduleTable_Ep),
        .ep = Rte_ScheduleTable_Ep,
};

struct xwarcos_schedtbl Rte_ScheduleTable_Object = {
        .cfg = &Rte_ScheduleTable_cfg,
        .node = {
                .core_id = 0,
                .bcln = XWLIB_BCLST_NODE_INIT(Rte_ScheduleTable_Object.node.bcln),
                .operation = xwarcos_schedtbl_counter_operation,
                .counter = NULL,
        },
        .status = SCHEDULETABLE_STOPPED,
        .start_point = 0,
        .start_offset = 0,
        .tick = 0,
        .epidx = 0,
};

ScheduleTableType Rte_ScheduleTable = &Rte_ScheduleTable_Object;

void Rte_ScheduleTable_Init(void)
{
        xwer_t xwrc;

        xwrc = xwarcos_schedtbl_validate_and_init(Rte_ScheduleTable);
        if (xwrc < 0) {
                xwlogf(INFO, LOGTAG, "Init Rte_ScheduleTable ... xwrc:%d\r\n", xwrc);
        }
}
