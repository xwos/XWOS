/**
 * @file
 * @brief xwos::autosar::cp::os::resource
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_resource_h__
#define __xwos_arcos_resource_h__

#include <xwos/osal/lock/spinlock.h>
#include <xwos/arcos/error.h>

/**
 * @defgroup arcos_resource xwos::autosar::cp::os::resource
 * @ingroup arcos
 *
 * ## ISO:17356-3 Compliances
 *
 * ### Resource management
 *
 *   + [ ] 8.1 Behaviour during access to occupied resources
 *   + [ ] 8.2 Restrictions when using resources
 *   + [ ] 8.3 Scheduler as a resource
 *     + [ ] If a task shall protect itself against preemptions by other tasks,
 *           it can lock the scheduler.
 *     + [ ] predefined resource `RES_SCHEDULER`
 *     + [ ] Interrupts are received and processed independently of the state of the
 *           resource `RES_SCHEDULER`. However, it prevents the rescheduling of tasks.
 *   + [ ] 8.5 OSEK Priority Ceiling Protocol
 *   + [ ] 8.6 OSEK Priority Ceiling Protocol with extensions for interrupt levels
 *   + [ ] 8.7 Internal Resources
 *
 * ### API (ISO:17356-3: 13.4 Resource management)
 *
 * #### 13.4.1 Data types
 *
 * + [ ] `ResourceType`
 *
 * #### 13.4.2 Constructional elements
 *
 * + [ ] 13.4.2.1 `DeclareResource()`
 *
 * #### 13.4.3 System services
 *
 * + [ ] 13.4.3.1 `GetResource()`
 *   + [ ] Nested occupation of one and the same resource is also forbidden.
 *   + [ ] It is not allowed to use services which are points of rescheduling
 *         for non preemptable tasks in critical sections.
 *   + [ ] The service may be called from an ISR and from task level (see Figure 12-1).
 *   + return value
 *     + [ ] E_OK: No error
 *     + [ ] E_OS_ID: Resource `ResID` is invalid
 *     + [ ] E_OS_ACCESS: Attempt to get a resource which is already occupied by
 *           any task or ISR, or the statically assigned priority of the calling
 *           task or interrupt routine is higher than the calculated ceiling priority.
 *
 * + [ ] 13.4.3.2 `ReleaseResource()`
 *   + return value
 *     + [ ] E_OK: No error
 *     + [ ] E_OS_ID: Resource `ResID` is invalid
 *     + [ ] E_OS_NOFUNC: Attempt to release a resource which is not occupied by
 *           any task or ISR, or another resource shall be released before
 *     + [ ] E_OS_ACCESS: Attempt to release a resource which has a lower
 *           ceiling priority than the statically assigned priority of the calling
 *           task or interrupt routine
 *
 * #### 13.4.4 Constants
 *
 * + [ ] RES_SCHEDULER
 *
 *
 * ## AUTOSAR CP OS Compliances
 *
 * ### Multi-Core
 *
 * + [ ] [SWS_Os_00801] If Spinlocks and Resources are locked by a Task/ISR they have
 *       to be unlocked in strict LIFO order. `ReleaseResource()` shall return
 *       `E_OS_NOFUNC` if the unlock order is violated.
 *       No other functionality shall be performed. (SRS_Os_80021)
 * + [ ] [SWS_Os_00851] If `OsUseResScheduler` is `TRUE` , the OS generation tool shall
 *       create a virtual instance of `RES_SCHEDULER` for each configured core.
 * + [ ] [SWS_Os_00852] It shall be possible for tasks running on different cores to
 *       occupy their own instance of `RES_SCHEDULER` at the same time.
 * + [ ] [SWS_Os_00853] The ceiling priority of each instance of `RES_SCHEDULER` shall
 *       prevent the execution of any other task on the core on which it is occupied
 *       but shall have no effect on the scheduling on any other core.
 * + [ ] [SWS_Os_00854] If `OsUseResScheduler` is `FALSE` AND the configuration
 *       contains a resource called `RES_SCHEDULER` , the configured resource shall
 *       behave the same as any other configured resource.
 * + [ ] [SWS_Os_00855] It shall be possible to configure a LINKED resource that
 *       links to `RES_SCHEDULER` . In a multi-core configuration with
 *       `OsUseResScheduler = TRUE` , the linkage shall be to the instance of
 *       `RES_SCHEDULER` on the core to which the LINKED resource is assigned.
 *
 *
 * ### Protection Compliances
 *
 *
 * ### Configuration Compliances
 *
 * + [ ] [ECUC_Os_00252] OsResource
 *   + [ ] [ECUC_Os_00050] OsResourceProperty
 *   + [ ] [ECUC_Os_00051] OsResourceAccessingApplication
 *   + [ ] [ECUC_Os_00052] OsResourceLinkedResourceRef
 *
 * @{
 */

/**
 * @brief AUTOSAR CP OS Resource
 * @note
 * FIXME: 暂时将Resource实现为自旋锁。
 */
struct xwarcos_resource {
        struct xwos_splk splk;
};

#define XWARCOS_RESOURCE_INITIALIZER { .splk = XWOS_SPLK_INITIALIZER, }

extern struct xwarcos_resource xwarcos_res_scheduler;
#define XWARCOS_RES_SCHEDULER (&xwarcos_res_scheduler)

xwer_t xwarcos_resource_get(struct xwarcos_resource * res);
xwer_t xwarcos_resource_release(struct xwarcos_resource * res);


/**
 * @} arcos_resource
 */

#endif /* xwos/arcos/resource.h */
