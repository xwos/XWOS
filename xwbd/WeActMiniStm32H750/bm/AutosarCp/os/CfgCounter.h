/**
 * @file
 * @brief AUTOSAR::CP::Os::Cfg::Counter
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef bm_AutosarCp_os_CfgCounter_h__
#define bm_AutosarCp_os_CfgCounter_h__

#include <xwmd/autosarcp/os/Counter.h>

#define Rte_TickCounter 0

extern struct xwarcos_counter Rte_TickCounter_Object;
void Rte_TickCounter_Drive(void);
void Rte_TickCounter_Init(void);

#endif /* bm/AutosarCp/os/CfgCounter.h */
