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

#ifndef xwmd_autosarcp_os_Control_h__
#define xwmd_autosarcp_os_Control_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwos/arcos/control.h>

typedef xwu8_t AppModeType;
#define OSDEFAULTAPPMODE ((AppModeType)1)
#define DONOTCARE ((AppModeType)0U)

typedef enum {
    OS_CORE_ID_MASTER = 0,
    OS_CORE_ID_0 = 0, /* CPU0 */
    OS_CORE_ID_1 = 1, /* CPU1 */
    OS_CORE_ID_2 = 2, /* CPU2 */
    OS_CORE_ID_3 = 3, /* CPU3 */
    OS_CORE_ID_4 = 4, /* CPU4 */
    OS_CORE_ID_5 = 5, /* CPU5 */
    OS_CORE_ID_6 = 6, /* CPU6 */
    OS_CORE_ID_7 = 7, /* CPU7 */
    OS_CORE_ID_8 = 8, /* CPU8 */
    OS_CORE_ID_9 = 9, /* CPU9 */
    OS_CORE_ID_10 = 10, /* CPU10 */
    OS_CORE_ID_11 = 11, /* CPU11 */
    OS_CORE_ID_12 = 12, /* CPU12 */
    OS_CORE_ID_13 = 13, /* CPU13 */
    OS_CORE_ID_14 = 14, /* CPU14 */
    OS_CORE_ID_15 = 15, /* CPU15 */
} CoreIdType;

AppModeType GetActiveApplicationMode(void);
CoreIdType GetCoreID(void);
void StartOS(AppModeType Mode);
void ShutdownOS(StatusType Error);

#endif /* xwmd/autosarcp/os/Control.h */
