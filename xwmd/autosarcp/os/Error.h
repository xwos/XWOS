/**
 * @file
 * @brief AUTOSAR::CP::Os::Error
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef xwmd_autosarcp_os_Error_h__
#define xwmd_autosarcp_os_Error_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwos/arcos/error.h>

#define E_OS_ACCESS E_XWARCOS_ACCESS
#define E_OS_CALLEVEL E_XWARCOS_CALLEVEL
#define E_OS_ID E_XWARCOS_ID
#define E_OS_LIMIT E_XWARCOS_LIMIT
#define E_OS_NOFUNC E_XWARCOS_NOFUNC
#define E_OS_RESOURCE E_XWARCOS_RESOURCE
#define E_OS_STATE E_XWARCOS_STATE
#define E_OS_VALUE E_XWARCOS_VALUE
#define E_OS_ILLEGAL_ADDRESS E_XWARCOS_ILLEGAL_ADDRESS
#define E_OS_STACKFAULT E_XWARCOS_STACKFAULT
#define E_OS_CORE E_XWARCOS_CORE
#define E_OS_SERVICEID E_XWARCOS_SERVICEID
#define E_OS_MISSINGEND E_XWARCOS_MISSINGEND
#define E_OS_DISABLEDINT E_XWARCOS_DISABLEDINT
#define E_OS_SPINLOCK E_XWARCOS_SPINLOCK
#define E_OS_NESTING_DEADLOCK E_XWARCOS_NESTING_DEADLOCK
#define E_OS_INTERFERENCE_DEADLOCK E_XWARCOS_INTERFERENCE_DEADLOCK
#define E_OS_PROTECTION_MEMORY E_XWARCOS_PROTECTION_MEMORY
#define E_OS_PROTECTION_LOCKED E_XWARCOS_PROTECTION_LOCKED
#define E_OS_PROTECTION_EXCEPTION E_XWARCOS_PROTECTION_EXCEPTION
#define E_OS_PROTECTION_TIME E_XWARCOS_PROTECTION_TIME
#define E_OS_PROTECTION_ARRIVAL E_XWARCOS_PROTECTION_ARRIVAL
#define E_OS_SYS_NO_RESTART E_XWARCOS_SYS_NO_RESTART
#define E_OS_SYS_XWOS_THD_INIT E_XWARCOS_SYS_XWOS_THD_INIT

#define E_XWARCOS_TO_STATUSTYPE(e) ((StatusType)((0 - (e)) - (E_XWARCOS_BASE)))

#endif /* xwmd/autosarcp/os/Error.h */
