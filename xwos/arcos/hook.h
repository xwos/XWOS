/**
 * @file
 * @brief xwos::autosar::cp::os::hook
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_hook_h__
#define __xwos_arcos_hook_h__

#include <xwos/standard.h>

/**
 * @defgroup arcos_task xwos::autosar::cp::os::hook
 * @ingroup hook
 *
 * ## ISO:17356-3 Compliances
 *
 * ### API (ISO:17356-3:13.8 Hook routines)
 *
 * + 13.8 Hook routines
 *   + [x] 13.8.2.1 `ErrorHook()`
 *   + [x] 13.8.2.2 `PreTaskHook()`
 *   + [x] 13.8.2.3 `PostTaskHook()`
 *   + [ ] 13.8.2.4 `StartupHook()`
 *   + [ ] 13.8.2.5 `ShutdownHook()`
 *   + [ ] `OSServiceId_xx`
 *   + [ ] `OSErrorGetServiceId()`
 *   + [ ] `OSError_x1_x2()`
 *
 *
 * ## AUTOSAR CP OS Compliances
 *
 * + [ ] [SWS_Os_00439] The Operating System module shall provide the OSEK error macros
 *       `OSError...()` to all configured error hooks AND there shall be two (like
 *       in OIL) global configuration parameters to switch these macros on or off. ()
 *
 * + `StartupHook()`
 *   + [ ] [SWS_Os_00060] If an application-specific startup hook is configured for
 *         an OS-Application `App` , the Operating System module shall call
 *         `StartupHook_<App>` on startup of the Operating System module. ()
 *   + [ ] [SWS_Os_00226] The Operating System module shall execute an
 *         application-specific startup hook with the access rights of the associated
 *         OS-Application. ()
 *   + [ ] [SWS_Os_00236] If both a system-specific and one (or more) application specific
 *         startup hook(s) are configured, the Operating System module shall call
 *         the system-specific startup hook before the application-specific
 *         startup hook(s). ()
 *
 * + `ShutdownHook()`
 *   + [ ] [SWS_Os_00112] If an application-specific shutdown hook is configured for an
 *         OS-Application `App`, the Operating System module shall call
 *         `ShutdownHook_<App>` on shutdown of the OS. ()
 *   + [ ] [SWS_Os_00225] The Operating System module shall execute an
 *         application-specific shutdown hook with the access rights of
 *         the associated OS-Application. ()
 *   + [ ] [SWS_Os_00237] If both a system-specific and one (or more) application
 *         specific shutdown hook(s) are configured, the Operating System module shall
 *         call the system-specific shutdown hook after the application-specific
 *         shutdown hook(s). ()
 *
 * + `ErrorHook()`
 *   + [ ] [SWS_Os_00246] When an error occurs AND an application-specific error hook
 *         is configured for the faulty OS-Application `<App>` , the Operating System
 *         module shall call that application-specific error hook `ErrorHook_<App>`
 *         after the system specific error hook is
 *         called (if configured). (SRS_Os_11013)
 *   + [ ] [SWS_Os_00085] The Operating System module shall execute an
 *         application-specific error hook with the access rights of the associated
 *         OS-Application. ()
 *   + [ ] [SWS_Os_00367] Operating System module’s services which do not return
 *         a `StatusType` - except `ActivateTaskAsyn()` and `SetEventAsyn()` - shall
 *         not raise the error hook(s). ()
 *
 * + `ProtectionHook()`
 *   + [ ] [SWS_Os_00538] Definition of configurable interface `ProtectionHook()` . ()
 *   + [ ] [SWS_Os_00308] If ProtectionHook returns an invalid value,
 *         the Operating System module shall take the same action as if
 *         no protection hook is configured. ()
 *   + [ ] [SWS_Os_00542] Availability of ProtectionHook: Available in
 *         Scalability Classes 2, 3 and 4. ()
 *
 * + Application specific `StartupHook()`
 *   + [ ] [SWS_Os_00539] Definition of configurable interface
 *         `StartupHook_<App>()` . ()
 *   + [ ] [SWS_Os_00543] Availability of `StartupHook_<App>` : Classes 3 and 4. ()
 *
 * + Application specific `ShutdownHook()`
 *   + [ ] [SWS_Os_00541] Definition of configurable interface
 *         `ShutdownHook_<App>()` . ()
 *   + [ ] [SWS_Os_00545] Availability of `ShutdownHook_<App>()` : Available in
 *         Scalability Classes 3 and 4. ()
 *
 * + Application specific `ErrorHook()`
 *   + [ ] [SWS_Os_00540] Definition of configurable interface `ErrorHook_<App>()` . ()
 *   + [ ] [SWS_Os_00544] Availability of `ErrorHook_<App>()` : Available in
 *         Scalability Classes 3 and 4. ()
 *
 * ### Protection
 *
 * ###  Configuration
 *
 *
 * @{
 */

struct xwarcos_oshook {
        void (* startup_hook)(void);
        void (* shutdown_hook)(void);
        void (* pre_task_hook)(void);
        void (* post_task_hook)(void);
        void (* error_hook)(xwer_t);
};

extern struct xwarcos_oshook xwarcos_oshook;

/**
 * @} arcos_hook
 */

#endif /* xwos/arcos/hook.h */
