/**
 * @file
 * @brief AUTOSAR::CP::Os
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef xwmd_autosarcp_os_Os_h__
#define xwmd_autosarcp_os_Os_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>

#define OS_MODULE_ID (1U)
#define OS_VENDOR_ID (0x5857U)
#define OS_AR_MAJOR_VERSION (4U)
#define OS_AR_MINOR_VERSION (6U)
#define OS_AR_PATCH_VERSION (0U)

#include <xwmd/autosarcp/os/Error.h>
#include <xwmd/autosarcp/os/Control.h>
#include <xwmd/autosarcp/os/Irq.h>
#include <xwmd/autosarcp/os/Resource.h>
#include <xwmd/autosarcp/os/Counter.h>
#include <xwmd/autosarcp/os/Task.h>
#include <xwmd/autosarcp/os/ScheduleTable.h>
#include <xwmd/autosarcp/os/Event.h>
#include <xwmd/autosarcp/os/Spinlock.h>
#include <xwmd/autosarcp/os/Application.h>

#endif /* xwmd/autosarcp/os/Os.h */
