/**
 * @file
 * @brief xwos::autosar::cp::os::event
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_event_h__
#define __xwos_arcos_event_h__

#include <xwos/arcos/error.h>
#include <xwos/arcos/task.h>
#include <xwos/osal/sync/flg.h>

/**
 * @defgroup arcos_task xwos::autosar::cp::os::event
 * @ingroup event
 *
 * ## ISO:17356-3 Compliances
 *
 * ### 7 Event mechanism
 *
 * + [ ] Events are not independent objects and assigned to extended tasks.
 * + [ ] When activating an extended task, these events are cleared.
 * + [ ] Any task or ISR of category 2 can set an event for
 *       a not suspended extended task.
 * + [ ] Only the owner is able to clear its events and to wait for
 *       the reception of its events.
 * + [ ] basic tasks are not allowed to clear events.
 * + [ ] An extended task in the waiting state is released to the ready state
 *       if at least one event for which the task is waiting has occurred.
 * + [ ] If a running extended task tries to wait for an event and
 *       this event has already occurred, the task remains in the running state.
 *
 * ### API (ISO:17356-3: 13.5 Event control)
 *
 * #### 13.5.1 Data types
 *
 * + [ ] `EventMaskType`
 * + [ ] `EventMaskRefType`
 *
 * #### 13.5.2 Constructional elements
 *
 * + [ ] 13.5.2.1 `DeclareEvent()`
 *
 * #### 13.5.3 System services
 *
 * + [ ] 13.5.3.1 `SetEvent()`
 *   + [ ] The service may be called from an interrupt service routine and
 *         from the task level, but not from hook routines.
 *   + [ ] The events of task `TaskID` are set according to the event
 *         mask `Mask` . Calling `SetEvent()` causes the task `TaskID` to
 *         be transferred to the ready state, if it was waiting for at least
 *         one of the events specified in `Mask` .
 *   + [ ] Any events not set in the event mask remain unchanged.
 *   + return value
 *     + [x] E_OS_OK: No error
 *     + [x] E_OS_ID: Task `TaskID` is invalid
 *     + [x] E_OS_ACCESS: Referenced task is no extended task
 *     + [x] E_OS_STATE: Events can not be set as the referenced task is in the
 *           suspended state.
 *
 * + [ ] 13.5.3.2 `ClearEvent()`
 *   + [ ] The system service `ClearEvent()` is restricted to extended tasks
 *         which own the event.
 *   + return value
 *     + [ ] E_OS_OK: No error
 *     + [ ] E_OS_ACCESS: Referenced task is no extended task
 *     + [ ] E_OS_CALLEVEL: Call at interrupt level
 *
 * + [ ] 13.5.3.3 `GetEvent()`
 *   + [ ] This service returns the current state of all event bits of the task
 *         `TaskID` , not the events that the task is waiting for.
 *   + [ ] The service may be called from interrupt service routines, task
 *         level and some hook routines (see Figure 12-1).
 *   + [ ] The current status of the event mask of task `TaskID` is copied to `Event` .
 *   + return value
 *     + [ ] E_OS_OK: No error
 *     + [ ] E_OS_ID: Task `TaskID` is invalid
 *     + [ ] E_OS_ACCESS: Referenced task is no extended task
 *     + [ ] E_OS_STATE: Referenced task `TaskID` is in the suspended state
 *
 * + [ ] 13.5.3.4 `WaitEvent()`
 *   + [ ] The state of the calling task is set to waiting, unless at least one
 *         of the events specified in `Mask` has already been set.
 *   + [ ] This call enforces rescheduling, if the wait condition occurs.
 *   + [ ] If rescheduling takes place, the internal resource of the task is
 *         released while the task is in the waiting state.
 *   + [ ] This service shall only be called from the extended task owning the event.
 *   + return value
 *     + [ ] E_OS_OK: No error
 *     + [ ] E_OS_ACCESS: Referenced task is no extended task
 *     + [ ] E_OS_RESOURCE: Calling task occupies resources
 *     + [ ] E_OS_CALLEVEL: Call at interrupt level
 *
 *
 *
 * ## AUTOSAR CP OS Compliances
 *
 * ### Multi-Core
 *
 * + Event setting
 *   + [ ] [SWS_Os_00602] It shall be possible to set an Event that is part of
 *         an OS-Application located on another core, as long as the assigned
 *         access rights allow it. (SRS_Os_80016)
 *   + [ ] [SWS_Os_00604] The call of `SetEvent()` across cores shall behave
 *         synchronously, i.e. a call returns after the Event has been set or
 *         an error has been detected. It shall not be possible to continue
 *         execution on the calling core before `SetEvent()` is accomplished on
 *         the remote core. (SRS_Os_80016)
 *   + [ ] [SWS_Os_00605] In case of an error when calling `SetEvent()` across cores,
 *         the error handler shall be called on the core on which `SetEvent()` was
 *         originally called. (SRS_Os_80016)
 *   + [ ] [SWS_Os_00817] The operating system shall provide an asynchronous version of
 *         `SetEvent()` which does not return errors to the caller, but only calls
 *         the (global) error hook (if configured). The function name shall be
 *         `SetEventAsyn()` . (SRS_Os_80016)
 * + Waiting for Events
 *   + [ ] [SWS_Os_00622] The AUTOSAR Operating System `WaitEvent()` API service shall
 *         check if it has been called while the calling Task has occupied a spinlock.
 *         In extended status an error `E_OS_SPINLOCK` shall be returned and
 *         the Task shall not enter the wait state. (SRS_Os_80021)
 *
 * ### Configuration Compliances
 *
 * + [ ] [ECUC_Os_00033] OsEvent
 *   + [ ] [ECUC_Os_00034] OsEventMask
 *
 *
 * @{
 */


/**
 * @} arcos_event
 */

#endif /* xwos/arcos/event.h */
