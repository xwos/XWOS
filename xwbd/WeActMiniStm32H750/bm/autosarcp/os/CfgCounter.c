/**
 * @file
 * @brief AUTOSAR::CP::Os::Cfg::Counter
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "bm/autosarcp/os/CfgCounter.h"
#include <xwos/lib/xwlog.h>

#define LOGTAG "OsCfgCounter"

const struct xwarcos_counter_config Rte_TickCounter_Config = {
        .core_id = 0,
        .ticks_per_base = 1,
        .max_allowed_value = 9,
        .min_cycle = 0,
        .nanoseconds_per_tick = 1000000,
        .type = XWARCOS_COUNTER_TYPE_SOFTWARE,
};

struct xwarcos_counter Rte_TickCounter_Object = {
        .cfg = &Rte_TickCounter_Config,
        .value = 0,
        .head = XWLIB_BCLST_HEAD_INIT(Rte_TickCounter_Object.head),
        .lock = XWOS_SQLK_INITIALIZER,
};
CounterType Rte_TickCounter = &Rte_TickCounter_Object;

void Rte_TickCounter_Drive(void)
{
        IncrementCounter(Rte_TickCounter);
}

void Rte_TickCounter_Init(void)
{
        xwer_t xwrc;

        xwrc = xwarcos_counter_validate_and_init(&Rte_TickCounter_Object);
        if (xwrc < 0) {
                xwlogf(INFO, LOGTAG, "Init Rte_TickCounter ... %d\r\n", xwrc);
        }
}
