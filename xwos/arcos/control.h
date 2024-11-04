/**
 * @file
 * @brief xwos::autosar::cp::os::control
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_control_h__
#define __xwos_arcos_control_h__

#include <xwos/standard.h>

/**
 * @defgroup arcos_control xwos::autosar::cp::os::control
 * @ingroup arcos
 *
 * ## ISO:17356-3
 *
 * ### API Compliances
 *
 * + ISO:17356-3: 13.7 Operating system execution control
 *   + [ ] AppModeType
 *   + [ ] `GetActiveApplicationMode()`
 *   + [ ] `StartOS()`
 *   + [ ] `ShutdownOS()`
 *   + [ ] `OSDEFAULTAPPMODE`
 *
 *
 * ## AUTOSAR CP OS Compliances
 *
 * ### Core OS
 *
 * + [ ] [SWS_Os_00001] The Operating System module shall provide an API that is
 *       backward compatible with the OSEK OS API [2].
 *       (SRS_Os_00097, SRS_BSW_00336, SRS_BSW_00345)
 * + [ ] [SWS_Os_00424] The first call to `StartOS()` (for starting
 *       the Operating System) shall not return. ()
 * + [ ] [SWS_Os_00425] If ShutdownOS is called and ShutdownHook returns then
 *       the Operating System module shall disable all interrupts and
 *       enter an endless loop. ()
 * + [ ] [SWS_Os_00566] The Operating System API shall check in extended mode all
 *       pointer arguments for a `NULL_PTR` and return `E_OS_ILLEGAL_ADDRESS`
 *       in this caseunless `NULL_PTR` is explicitly allowed as a valid pointer
 *       address value in the API parameter specification. ()
 *
 * ### # Multi-Core
 *
 * + [ ] [SWS_Os_00606] The AUTOSAR specification does not support the activation of
 *       AUTOSAR cores after calling `StartOS()` on that core. If `StartCore() is
 *       called after `StartOS()` it shall return with `E_OS_ACCESS` in
 *       extended status. (SRS_Os_80001)
 * + [ ] [SWS_Os_00607] `StartOS()` shall start the OS on the core
 *       on which it is called. (SRS_Os_80006, SRS_Os_80013)
 * + [ ] [SWS_Os_00608] If more than one core calls `StartOS()` with an AppMode
 *       other than `DONOTCARE` , the AppModes shall be the same. `StartOS()` shall
 *       check this at the first synchronization point. In case of violation,
 *       `StartOS()` shall not start the scheduling, shall not call
 *       any `StartupHooks()`, and shall enter an endless loop on
 *       every core. (SRS_Os_80006)
 * + [ ] [SWS_Os_00609] If `StartOS()` is called with the AppMode `DONOTCARE` ,
 *       the application mode of the other core(s) (differing from `DONOTCARE` )
 *       shall be used. (SRS_Os_80006)
 * + [ ] [SWS_Os_00610] At least one core shall define an AppMode other than
 *       `DONOTCARE` . (SRS_Os_80006)
 * + [ ] [SWS_Os_00611] If the IOC is configured, `StartOS()` shall initialize
 *       the data structures of the IOC. (SRS_Os_80020)
 * + [ ] [SWS_Os_00830] [ **DRAFT** ] If the IOC is configured and the OS Generator
 *       is invoked in "Default mode", `StartOS()` shall invoke the `IocInit()`
 *       (See [SWS_Os_00835]) to initialize the data structures of
 *       the IOC. (SRS_Os_80020)
 *
 * ### Protection
 *
 * + [ ] [SWS_Os_00088] If an OS-Application makes a service call from the wrong
 *       context AND is currently not inside a Category 1 ISR the Operating System
 *       module shall not perform the requested action (the service call shall have
 *       no effect) and return `E_OS_CALLEVEL` or the "invalid value" of the
 *       service. (SRS_Os_11009, SRS_Os_11013)
 *
 * @{
 */

/**
 * @} arcos_control
 */

#endif /* xwos/arcos/control.h */
