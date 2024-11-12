/**
 * @file
 * @brief AUTOSAR::CP::Os::Control
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Control.h>
#include <xwos/osal/skd.h>

AppModeType GetActiveApplicationMode(void)
{
        return OSDEFAULTAPPMODE;
}

CoreIdType GetCoreID(void)
{
    return (CoreIdType)xwos_skd_get_cpuid_lc();
}

void StartOS(AppModeType Mode)
{
        XWOS_UNUSED(Mode);
}

void ShutdownOS(StatusType Error)
{
        XWOS_UNUSED(Error);
}
