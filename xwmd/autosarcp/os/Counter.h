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

#ifndef xwmd_autosarcp_os_Counter_h__
#define xwmd_autosarcp_os_Counter_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwos/arcos/counter.h>

typedef xwtk_t TickType;
typedef TickType * TickRefType;
typedef struct xwarcos_counter * CounterType;
#define DeclareCounter(x) extern CounterType x

StatusType IncrementCounter(CounterType CounterID);
StatusType GetCounterValue(CounterType CounterID, TickRefType Value);
StatusType GetElapsedValue(CounterType CounterID, TickRefType Value,
                           TickRefType ElapsedValue);

#endif /* xwmd/autosarcp/os/Counter.h */
