/**
 * @file
 * @brief xwos::autosar::cp::os::irq
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_irq_h__
#define __xwos_arcos_irq_h__

#include <xwos/standard.h>

/**
 * @defgroup arcos_task xwos::autosar::cp::os::irq
 * @ingroup irq
 *
 * ## ISO:17356-3
 *
 * ### API (ISO:17356-3:13.3 Interrupt handling)
 *
 * + [x] 13.3.2.1 `EnableAllInterrupts()`
 * + [x] 13.3.2.2 `DisableAllInterrupts()`
 * + [x] 13.3.2.3 `ResumeAllInterrupts()`
 * + [x] 13.3.2.4 `SuspendAllInterrupts()`
 * + [ ] 13.3.2.5 `ResumeOSInterrupts()`
 * + [ ] 13.3.2.6 `SuspendOSInterrupts()`
 *
 *
 * ## AUTOSAR CP OS Compliances
 *
 * + [ ] [SWS_Os_00299] The Operating System module shall provide the services
 *       `DisableAllInterrupts()` , `EnableAllInterrupts()` ,
 *       `SuspendAllInterrupts()` , `ResumeAllInterrupts()` prior to calling
 *       `StartOS()` and after calling `ShutdownOS()` . (SRS_Os_11018)
 * + [ ] [SWS_Os_00808] The Operating System shall provide for each category 2
 *       interrupt source `(OsIsrCategory == CATEGORY_2)` the APIs
 *       `DisableInterruptSource()` , `EnableInterruptSource()` and
 *       `ClearPendingInterrupt()` . (SRS_Os_11011)
 * + [ ] [SWS_Os_00809] `DisableInterruptSource()` / `EnableInterruptSource()` does not
 *       support nested calls. Nested calls of interrupt source control API
 *       The Operating System shall return `E_OS_NOFUNC` (in EXTENDED status)
 *       in case `DisableInterruptSource()` is called for an interrupt source which
 *       is already disabled or `EnableInterruptSource()` is called for an interrupt
 *       source which is already enabled. (SRS_Os_11011)
 * + [ ] [SWS_Os_00810] Error handling of interrupt source control API
 *       If the Operating System detects an error while executing a
 *       `DisableInterruptSource()` , `EnableInterruptSource()` and
 *       `ClearPendingInterrupt()` the OS shall return the appropriate `StatusType`
 *       and call the `ErrorHook()` . Otherwise `E_OK` shall be returned. (SRS_Os_11011)
 * + [ ] [SWS_Os_00811] A call of `EnableInterruptSource()` shall enable the requested
 *       interrupt source by modifying the interrupt controller registers.
 *       Additionally it shall clear the interrupt pending flag. (SRS_Os_11011)
 * + [ ] [SWS_Os_00812] A call of `DisableInterruptSource()` shall disable the
 *       requested interrupt source by modifying the interrupt controller registers.
 *       (SRS_Os_11011)
 * + [ ] [SWS_Os_00813] A call of `ClearPendingInterrupt()` shall clear the interrupt
 *       pending flag by modifying the respective interrupt controller registers.
 *       (SRS_Os_11011)
 * + [ ] [SWS_Os_00814] Clearing of pending interrupts shall be restricted to
 *       clearing the pending flag in the interrupt controller. (SRS_Os_11011)
 *
 * ### Multi-Core
 *
 * + [x] [SWS_Os_00590] The OS service `DisableAllInterrupts()` shall only affect the
 *       core on which it is called. (SRS_Os_80013)
 * + [x] [SWS_Os_00591] The OS service `EnableAllInterrupts()` shall only affect the
 *       core on which it is called. (SRS_Os_80013)
 * + [x] [SWS_Os_00592] The OS service `SuspendAllInterrupts()` shall only affect the
 *       core on which it is called. (SRS_Os_80013)
 * + [x] [SWS_Os_00593]  The OS service `ResumeAllInterrupts()` shall only affect the
 *       core on which it is called. (SRS_Os_80013)
 * + [ ] [SWS_Os_00594]  The OS service `SuspendOSInterrupts()` shall only affect the
 *       core on which it is called. (SRS_Os_80013)
 * + [ ] [SWS_Os_00595] The OS service `ResumeOSInterrupts()` shall only affect the
 *       core on which it is called. (SRS_Os_80013)
 *
 * ### API
 *
 * + [ ] `GetISRID()`
 *   + [ ] [SWS_Os_00511] Definition of API function `GetISRID()` . ()
 *   + [ ] [SWS_Os_00263] If called from category 2 ISR (or Hook routines called
 *         inside a category 2 ISR), `GetISRID()` shall return the identifier of
 *         the currently executing ISR. ()
 *   + [ ] [SWS_Os_00264] If its caller is not a category 2 ISR (or Hook routines
 *         called inside a category 2 ISR), `GetISRID()` shall return `INVALID_ISR`. ()
 *   + [ ] [SWS_Os_00515] Availability of `GetISRID()` : Available in all
 *         Scalability Classes. ()
 *
 * + [ ] `EnableInterruptSource()`
 *   + [ ] [SWS_Os_91020] Definition of API function `EnableInterruptSource()` .
 *         (SRS_Os_11011)
 *
 * + [ ] `DisableInterruptSource()`
 *   + [ ] [SWS_Os_91019] Definition of API function `DisableInterruptSource()` .
 *         (SRS_Os_11011)
 *
 * + [ ] `ClearPendingInterrupt()`
 *   + [ ] [SWS_Os_91021] Definition of API function `ClearPendingInterrupt()` .
 *         (SRS_Os_11011)
 *
 * ### Protection
 *
 * + [ ] [SWS_Os_00368] If a Category 2 ISR calls `DisableAllInterrupts()` /
 *       `SuspendAllInterrupts()` / `SuspendOSInterrupts()` and ends (returns) without
 *       calling the corresponding `EnableAllInterrupts()` / `ResumeAllInterrupts()` /
 *       `ResumeOSInterrupts()` , the Operating System module shall perform the missing
 *       service and shall call the `ErrorHook()` (if configured) with the status
 *       `E_OS_DISABLEDINT` . ()
 * + [ ] [SWS_Os_00369] If a Category 2 ISR calls `GetResource()` and ends (returns)
 *       without calling the corresponding `ReleaseResource()` , the Operating System
 *       module shall perform the `ReleaseResource()` call and shall call
 *       the `ErrorHook()` (if configured) with the
 *       status `E_OS_RESOURCE` (see [12], section 13.1). ()
 * + [ ] [SWS_Os_00092] If `EnableAllInterrupts()` / `ResumeAllInterrupts()` /
 *       `ResumeOSInterrupts()` are called and no corresponding
 *       `DisableAllInterrupts()` / `SuspendAllInterrupts()` / `SuspendOSInterrupts()`
 *       was done before, the Operating System module shall not perform this
 *       Operating System service. (SRS_Os_11009)
 * + [ ] [SWS_Os_00093] If interrupts are disabled/suspended by a Task/ISR/Hook and
 *       the Task/ISR/Hook calls any Operating System service (excluding
 *       the interrupt services) then the Operating System module shall ignore
 *       the service AND shall return `E_OS_DISABLEDINT` if the service returns a
 *       `StatusType` value. (SRS_Os_11009, SRS_Os_11013)
 * + [ ] [SWS_Os_00109] If the Operating System module forcibly terminates an interrupt
 *       service routine, it clears the interrupt request, aborts the interrupt service
 *       routine (The interrupt source stays in the current state.) and releases all
 *       OSEK resources the interrupt service routine has allocated and calls
 *       `EnableAllInterrupts()` / `ResumeOSInterrupts()` / `ResumeAllInterrupts()`
 *       if the interrupt called `DisableAllInterrupts()` / `SuspendOSInterrupts()` /
 *       `SuspendAllInterrupts()` before without the corresponding
 *       `EnableAllInterrupts()` / `ResumeOSInterrupts() / `ResumeAllInterrupts()`
 *       call. (SRS_Os_11014)
 *
 * ##@ Configuration
 *
 * + [ ] [ECUC_Os_00041] OsIsr
 *   + [ ] [ECUC_Os_00042] OsIsrCategory
 *   + [ ] [ECUC_Os_00403] OsIsrPeriod
 *   + [ ] [ECUC_Os_00043] OsIsrResourceRef
 *   + [ ] [ECUC_Os_00402] OsMemoryMappingCodeLocationRef
 * + [ ] [ECUC_Os_00388] OsIsrResourceLock
 *   + [ ] [ECUC_Os_00389] OsIsrResourceLockBudget
 *   + [ ] [ECUC_Os_00390] OsIsrResourceLockResourceRef
 * + [ ] [ECUC_Os_00326] OsIsrTimingProtection
 *   + [ ] [ECUC_Os_00229] OsIsrAllInterruptLockBudget
 *   + [ ] [ECUC_Os_00222] OsIsrExecutionBudget
 *   + [ ] [ECUC_Os_00387] OsIsrOsInterruptLockBudget
 *   + [ ] [ECUC_Os_00223] OsIsrTimeFrame
 *
 * @{
 */

/**
 * @brief Disable all interrupts of local CPU
 * @note
 * + This service does not support nesting.
 */
void xwacos_irq_disable_all_interrupts(void);

/**
 * @brief Enable all interrupts of local CPU
 * @note
 * + This service does not support nesting.
 */
void xwacos_irq_enable_all_interrupts(void);

/**
 * @brief Save the recognition status of all interrupts and disable all interrupts
 *        for local CPU.
 * @note
 * + This service supports nesting.
 */
void xwacos_irq_suspend_all_interrupts(void);

/**
 * @brief Restores the recognition status of all interrupts saved by
 *        the `xwacos_irq_suspend_all_interrupts()` service
 * @note
 * + This service supports nesting.
 */
void xwacos_irq_resume_all_interrupts(void);

/**
 * @brief Save the recognition status of interrupts of category 2 and
 *        disable the recognition of these interrupts
 * @note
 * + This service supports nesting.
 */
void xwacos_irq_suspend_os_interrupts(void);

/**
 * @brief Restore the recognition status of interrupts saved by
 *        the `xwacos_irq_suspend_os_interrupts()` service
 * @note
 * + This service supports nesting.
 */
void xwacos_irq_resume_os_interrupts(void);

/**
 * @} arcos_irq
 */

#endif /* xwos/arcos/irq.h */
