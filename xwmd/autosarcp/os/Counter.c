/**
 * @file
 * @brief AUTOSAR::CP::Os::Counter
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Counter.h>
#include <xwmd/autosarcp/os/Error.h>

extern struct xwarcos_counter * const xwarcos_counter_table[];
extern const xwu32_t xwarcos_counter_num;

StatusType IncrementCounter(CounterType CounterID)
{
        xwer_t xwrc;
        struct xwarcos_counter * counter;

        if (CounterID >= xwarcos_counter_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                counter = xwarcos_counter_table[CounterID];
                xwrc = xwarcos_counter_increment(counter);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetCounterValue(CounterType CounterID, TickRefType Value)
{
        xwer_t xwrc;
        struct xwarcos_counter * counter;

        if (CounterID >= xwarcos_counter_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                counter = xwarcos_counter_table[CounterID];
                xwrc = xwarcos_counter_get_value(counter, Value);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType GetElapsedValue(CounterType CounterID, TickRefType Value,
                           TickRefType ElapsedValue)
{
        xwer_t xwrc;
        struct xwarcos_counter * counter;

        if (CounterID >= xwarcos_counter_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                counter = xwarcos_counter_table[CounterID];
                xwrc = xwarcos_counter_get_elapsed_value(counter, Value, ElapsedValue);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
