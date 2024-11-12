/**
 * @file
 * @brief AUTOSAR::CP::Os::Irq
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef xwmd_autosarcp_os_Irq_h__
#define xwmd_autosarcp_os_Irq_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwos/arcos/irq.h>

void DisableAllInterrupts(void);
void EnableAllInterrupts(void);
void SuspendAllInterrupts(void);
void ResumeAllInterrupts(void);
void SuspendOSInterrupts(void);
void ResumeOSInterrupts(void);

#endif /* xwmd/autosarcp/os/Irq.h */
