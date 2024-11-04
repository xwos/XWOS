/**
 * @file
 * @brief xwos::autosar::cp::os::task
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_task_h__
#define __xwos_arcos_task_h__

#include <xwos/arcos/error.h>
#include <xwos/arcos/schedtbl.h>
#include <xwos/lib/setjmp.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>

/**
 * @defgroup arcos_task xwos::autosar::cp::os::task
 * @ingroup arcos
 *
 * ## ISO:17356-3 Compliances
 *
 * ### 3.2 Conformance classes
 *
 * + [ ] BCC1
 * + [ ] BCC2
 * + [ ] ECC1
 * + [x] ECC2
 *
 * ### 4 Task management
 *
 * + 4.1 Task concept
 *   + [x] 4.1.2 Basic tasks
 *     + Basic tasks only release the processor, if
 *       + they terminate,
 *       + the OSEK operating system switches to a higher-priority task, or
 *       + an interrupt occurs which causes the processor to switch to
 *         an interrupt service routine (ISR).
 *   + [ ] 4.1.3 Extended tasks
 *     + Extended tasks are allowed to use `WaitEvent()` , which may result in a
 *       waiting state
 * + 4.2 [x] Task state model
 * + 4.3 Activating a task
 *   + [x] After activation, the task is ready to execute from the first statement.
 *     + **注解**
 *       + 实现Task的线程必须为 **连接态(joinable)** 。
 *       + 基于信号量实现Activation。
 *   + [x] If the maximum number of multiple requests has not been reached,
 *         request is queued.
 *     + **注解** ：最大数量即为信号量的最大值。
 *   + [x] The requests of basic task activations are queued per priority
 *         in activation order.
 *     + **注解** ：信号量的值即为请求的次数。
 * + 4.4 Task switching mechanism
 *   + [x] The principle of multitasking allows the operating system to
 *         execute various tasks concurrently.
 *     + **注解** ：XWOS已经是多线程操作系统。
 *   + [x] A task can reserve the scheduler to avoid a task switch until
 *         it is released.
 *     + **注解** ：XWOS支持临时关闭/开启抢占的CAPI。
 * + 4.5 Task priority
 *   + [x] The value 0 is defined as the lowest priority of a task.
 *         Accordingly bigger numbers define higher priorities.
 *     + **注解** ：与XWOS的实现一致
 *   + [x] Tasks on the same priority level are started depending on their order of
 *         activation. A preempted task is considered to be the first (oldest) task
 *         in the ready list of its current priority (FIFO).
 *     + **注解** ：与XWOS的实现一致
 * + 4.6 Scheduling policy
 *   + [x] 4.6.1 Full preemptive scheduling
 *     + [x] Full preemptive scheduling will put the running task into
 *           the READY state.
 *     + [x] The preempted task can be continued at the location
 *           where it was preempted.
 *   + [ ] 4.6.2 Non preemptive scheduling
 *   + [ ] 4.6.3 Groups of tasks
 *   + [ ] 4.6.4 Mixed preemptive scheduling
 *   + **注解** ：目前XWOS只支持抢占式内核。
 * + [x] 4.7 Termination of tasks
 *
 * ### API (ISO:17356-3:13.2 Task management)
 *
 * #### 13.2.1 Data types
 *
 *   + [ ] `TaskType`
 *   + [ ] `TaskRefType`
 *   + [ ] `TaskStateType`
 *   + [ ] `TaskStateRefType`
 *
 * #### 13.2.2 Constructional elements
 *
 *   + [ ] 13.2.2.1 `DeclareTask()`
 *
 * #### 13.2.3 System services
 *
 * + [x] 13.2.3.1 `ActivateTask()`
 *   + **注解** ：XWOS仅提供后端实现 `xwarcos_task_activate()` 。
 *   + [x] The operating system ensures that the task code is being executed
 *         from the first statement.
 *   + [ ] When an extended task is transferred from suspended state into
 *         ready state all its events are cleared.
 *   + return value
 *     + [x] E_OS_OK: No error
 *     + [x] E_OS_ID: Task `TaskID` is invalid
 *     + [x] E_OS_LIMIT: Too many task activations of `TaskID`
 *
 * + [x] 13.2.3.2 `TerminateTask()`
 *   + **注解** ：XWOS仅提供后端实现 `xwarcos_task_terminate()` 。
 *   + [x] This service causes the termination of the calling task.
 *   + [x] The calling task is transferred from the running state into
 *         the suspended state.
 *   + return value
 *     + [x] E_OK: No error
 *     + [ ] E_OS_RESOURCE: Task still occupies resources
 *     + [x] E_OS_CALLEVEL: Call at interrupt level
 *
 * + [x] 13.2.3.3 `ChainTask()`
 *   + [x] If the succeeding task is identical with the current task, this does not
 *         result in multiple requests. The task is not transferred to the
 *         suspended state, but will immediately become ready again.
 *   + [ ] An internal resource assigned to the calling task is automatically
 *         released, even if the succeeding task is identical with the current task.
 *   + [ ] If a resource is still occupied in standard status the behaviour
 *         is undefined.
 *     + **注解** ：后端 `xwarcos_task_chain()` 实现为返回 `-E_XWARCOS_RESOURCE` 。
 *   + [x] If called successfully, `ChainTask()` does not return to the call level
 *         and the status can not be evaluated.
 *   + [x] If the service ChainTask is called successfully, this enforces
 *         a rescheduling.
 *   + [ ] Ending a task function without call to `TerminateTask()` or
 *         `ChainTask()` is strictly forbidden and may leave the system in an
 *         undefined state.
 *   + return value
 *     + [x] E_OK: No error
 *     + [x] E_OS_ID: Task `TaskID` is invalid
 *     + [ ] E_OS_RESOURCE: Task still occupies resources
 *     + [x] E_OS_CALLEVEL: Call at interrupt level
 *
 * + [ ] 13.2.3.4 `Schedule()`
 *
 * + [x] 13.2.3.5 `GetTaskID()`
 *   + **注解** ：XWOS仅提供后端实现 `xwarcos_task_get()` 。
 *   + [x] Allowed on task level, ISR level and in several hook routines.
 *   + [x] If `TaskID` can’t be evaluated (no task currently running), the
 *         service returns `INVALID_TASK` as TaskType.
 *     + **注解** ：`xwarcos_task_get()` 返回 `NULL` 表示 `INVALID_TASK` 。
 *   + return value
 *     + [ ] E_OK: No error
 *
 * + [x] 13.2.3.6 `GetTaskState()`
 *   + [x] When a call is made from a task in a full preemptive system, the
 *         result may already be incorrect at the time of evaluation.
 *   + return value
 *     + [x] E_OK: No error
 *     + [ ] E_OS_ID: Task `TaskID` is invalid
 *
 + #### 13.2.4 Constants
 *
 * + [x] RUNNING: XWARCOS_TASK_STATE_RUNNING
 * + [x] WAITING: XWARCOS_TASK_STATE_RUNNING
 * + [x] READY: XWARCOS_TASK_STATE_RUNNING
 * + [x] SUSPENDED: XWARCOS_TASK_STATE_RUNNING
 * + [x] INVALID_TASK: XWARCOS_TASK_INVALID
 *
 * ### Hook
 *
 * + [x] `PostTaskHook()` is called each time directly before the old task leaves
 *       the `RUNNING` state.
 *   + **注解** ：为了避免命名冲突，后端符号命名为 `xwarcos_task_posthook()` 。
 * + [x] `PreTaskHook()` is called each time directly after a new task enters
 *       the `RUNNING` state.
 *   + **注解** ：为了避免命名冲突，后端符号命名为 `xwarcos_task_prehook()` 。
 * + [x] `GetTaskID()` does not return `INVALID_TASK` in
 *       `PreTaskHook()` / `PostTaskHook()`.
 *
 *
 * ## AUTOSAR CP OS Compliances
 *
 * ### Multi-Core
 *
 * #### `GetTaskID()`
 *
 *   + [x] When called from an interrupt routine on Multi-Core systems it indicates
 *         that no Task is running on the core or returns the ID of the interrupted
 *         Task on the core.
 *     + **注解** ：后端实现 `xwarcos_task_get()` 返回被中断的任务。
 *
 * #### `ActivateTask()`
 *
 *   + [x] [SWS_Os_00596] It shall be possible to activate a Task that is part of
 *         an OS-Application located on another core, as long as
 *         the assigned access rights allow it. (SRS_Os_80001, SRS_Os_80015)
 *         + **注解** ：XWOS信号量支持跨核操作。
 *   + [x] [SWS_Os_00598] The call of `ActivateTask()` across cores shall behave
 *         synchronously, i.e. a call returns after the Task has been activated or
 *         an error has been detected. It shall not be possible to continue execution
 *         on the calling core before `ActivateTask()` is accomplished on
 *         the remote core. (SRS_Os_80015)
 *         + **注解** ：XWOS信号量的POST操作即为同步操作。
 *   + [ ] [SWS_Os_00599] In case of an error when calling `ActivateTask()`
 *         across cores, the error handler shall be called on the core on
 *         which `ActivateTask()` was originally called. (SRS_Os_80015)
 *   + [ ] [SWS_Os_00816] The operating system shall provide an asynchronous version
 *         of `ActivateTask()` which does not return errors to the caller,
 *         but only calls the (global) error hook (if configured).
 *         The function name shall be `ActivateTaskAsyn()` . (SRS_Os_80015)
 *
 * #### `ChainTask()`
 *
 *   + [ ] [SWS_Os_00600] It shall be possible to chain a Task that is part of
 *         an OS-Application located on another core, as long as the assigned
 *         access rights allow it. (SRS_Os_80001, SRS_Os_80015)
 *
 * #### `TerminateTask()`
 *
 *   + [ ] [SWS_Os_00612] In extended status `TerminateTask()` / `ChainTask()`
 *         shall return with an error ( `E_OS_SPINLOCK` ), which can be evaluated
 *         in the application. (SRS_Os_80021)
 *   + [ ] [SWS_Os_00613] Spinlocks occupied by Tasks that are terminated in
 *         response to a protection hook shall be automatically released.
 *         This applies also to the case in which an OS-Application is
 *         terminated. (SRS_Os_80021)
 *
 * ### API
 *
 * #### `ActivateTaskAsyn()`
 *   + [ ] [SWS_Os_91022] Definition of API function ActivateTaskAsyn. (SRS_Os_80015)
 *   + [ ] [SWS_Os_00818] Availability of `ActivateTaskAsyn()` : Available in systems
 *         which support OS-Applications. (SRS_Os_80015)
 *
 *
 * ### Protection
 *
 * + [x] [SWS_Os_00052] If a Task returns from its entry function without making a
 *       `TerminateTask()` or `ChainTask()` call, the Operating System module shall
 *       terminate the Task (and call the `OsPostTaskHook()`
 *       if configured). (SRS_Os_11009)
 * + [ ] [SWS_Os_00069] If a Task returns from its entry function without making a
 *       `TerminateTask()` or `ChainTask()` call AND the error hook is configured,
 *       the Operating System module shall call the `ErrorHook()` (this is done
 *       regardless of whether the Task causes other errors, e.g. `E_OS_RESOURCE`)
 *       with status `E_OS_MISSINGEND` before the Task leaves
 *       the `RUNNING` state. (SRS_Os_11009)
 * + [ ] [SWS_Os_00070] If a Task returns from the entry function without making a
 *       `TerminateTask()` or `ChainTask()` call and still holds OSEK Resources,
 *       the Operating System module shall release them. (SRS_Os_11009, SRS_Os_11013)
 * + [ ] [SWS_Os_00239] If a Task returns from the entry function without making a
 *       `TerminateTask()` or `ChainTask()` call and interrupts are still disabled,
 *       the Operating System module shall enable them. ()
 * + [ ] [SWS_Os_00071] If the `PostTaskHook()` is configured, the Operating System
 *       module shall not call the hook if `ShutdownOS()` is called. ()
 * + [ ] [SWS_Os_00108] If the Operating System module forcibly terminates a Task,
 *       it terminates the Task, releases all allocated OSEK resources and calls
 *       `EnableAllInterrupts()` / `ResumeOSInterrupts()` / `ResumeAllInterrupts()`
 *       if the Task called `DisableAllInterrupts()` / `SuspendOSInterrupts()` /
 *       `SuspendAllInterrupts()` before without the corresponding
 *       `EnableAllInterrupts() / `ResumeOSInterrupts()` / `ResumeAllInterrupts()`
 *       call. (SRS_Os_11014)
 *
 * ### Configuration
 *
 * + [ ] [ECUC_Os_00073] OsTask
 *   + [x] [ECUC_Os_00074] OsTaskActivation
 *   + [ ] [ECUC_Os_00404] OsTaskPeriod
 *   + [x] [ECUC_Os_00075] OsTaskPriority
 *   + [ ] [ECUC_Os_00076] OsTaskSchedule
 *   + [ ] [ECUC_Os_00402] OsMemoryMappingCodeLocationRef
 *   + [ ] [ECUC_Os_00077] OsTaskAccessingApplication
 *   + [ ] [ECUC_Os_00078] OsTaskEventRef
 *   + [ ] [ECUC_Os_00079] OsTaskResourceRef
 * + [ ] [ECUC_Os_00080] OsTaskAutostart
 *   + [ ] [ECUC_Os_00081] OsTaskAppModeRef
 * + [ ] [ECUC_Os_00082] OsTaskResourceLock
 *   + [ ] [ECUC_Os_00083] OsTaskResourceLockBudget
 *   + [ ] [ECUC_Os_00084] OsTaskResourceLockResourceRef
 * + [ ] [ECUC_Os_00325] OsTaskTimingProtection
 *   + [ ] [ECUC_Os_00085] OsTaskAllInterruptLockBudget
 *   + [ ] [ECUC_Os_00185] OsTaskExecutionBudget
 *   + [ ] [ECUC_Os_00086] OsTaskOsInterruptLockBudget
 *   + [ ] [ECUC_Os_00391] OsTaskTimeFrame
 * + [ECUC_Os_00035] OsHooks
 *   + [ ] [ECUC_Os_00037] OsPostTaskHook
 *   + [ ] [ECUC_Os_00038] OsPreTaskHook
 *
 * @{
 */

/**
 * @brief AUTOSAR CP OS Task Preemptability Enumeration
 */
enum xwarcos_task_preemptability_em {
        XWARCOS_TASK_PREEMPTABILITY_NON,
        XWARCOS_TASK_PREEMPTABILITY_FULL,
};

/**
 * @brief AUTOSAR CP OS Task Configurations
 */
struct xwarcos_task_config {
        xwpr_t priority; /**< [ECUC_Os_00404] */
        xwu32_t activations; /**< [ECUC_Os_00074] */
        enum xwarcos_task_preemptability_em preemptability; /**< [ECUC_Os_00076] */
        xwstk_t * stack;
        xwsz_t stack_size;
};

/**
 * @brief AUTOSAR CP OS Task State Enumeration
 */
enum xwarcos_task_state_em {
        XWARCOS_TASK_STATE_SUSPENDED,
        XWARCOS_TASK_STATE_READY,
        XWARCOS_TASK_STATE_WAITING,
        XWARCOS_TASK_STATE_RUNNING,
};

/**
 * @brief AUTOSAR CP OS Task Function
 */
typedef void (* xwarcos_task_f)(void);

/**
 * @brief AUTOSAR CP OS Task
 */
struct xwarcos_task {
        const struct xwarcos_task_config * cfg;
        struct xwos_thd thd;
        xwos_thd_d thdd;
        xwarcos_task_f func;
        struct xwos_splk splk;
        enum xwarcos_task_state_em state;
        struct xwos_sem sem;
        xwjmpbuf_b termination;
};

/**
 * @brief Invalid task
 */
#define XWARCOS_TASK_INVALID NULL

/**
 * @brief Init a task
 * @param[in] task: Task reference
 * @return Error code
 * @retval E_XWARC_OK: No error, convert to E_OK
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: Invalid pointer
 * @retval -E_XWARCOS_VALUE: Activations in config is error
 * @retval -E_XWARCOS_SYS_XWOS_THD_INIT: Os internal error when call `xwos_thd_init()`
 */
xwer_t xwarcos_task_validate_and_init(struct xwarcos_task * task);

/**
 * @brief Activate a task
 * @param[in] task: Task reference
 * @return Error code
 * @retval E_XWARC_OK: No error, convert to E_OK
 * @retval -E_XWARCOS_LIMIT: Too many task activations
 * @retval -E_XWARCOS_ID: Task is invalid
 */
xwer_t xwarcos_task_activate(struct xwarcos_task * task);

/**
 * @brief Terminate a task
 * @param[in] task: Task reference
 * @return No return to call level or Error code
 * @retval -E_XWARCOS_RESOURCE: Task still occupies resources
 * @retval -E_XWARCOS_CALLEVEL: Call at interrupt level
 */
xwer_t xwarcos_task_terminate(void);

/**
 * @brief Activate a task after the termination of the running task
 * @param[in] task: Task to be activated
 * @return No return to call level or Error code
 * @retval -E_XWARCOS_LIMIT: Too many task activations
 * @retval -E_XWARCOS_ID: Task is invalid
 * @retval -E_XWARCOS_RESOURCE: Task still occupies resources
 * @retval -E_XWARCOS_CALLEVEL: Call at interrupt level
 */
xwer_t xwarcos_task_chain(struct xwarcos_task * task);

/**
 * @brief Return the information about the task which is currently running
 * @param[out] task: Reference to the task which is currently running
 * @return Error code
 * @retval E_XWARCOS_OK: No error
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: Invalid pointer
 */
xwer_t xwarcos_task_get(struct xwarcos_task ** task);

/**
 * @brief Returns the state of a task
 * @param[in] task: task reference
 * @param[out] state: reference to the task state
 * @return Error code
 * @retval E_XWARCOS_OK: No error
 * @retval -E_XWARCOS_ID: Invalid task pointer
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: Invalid state pointer
 */
xwer_t xwarcos_task_get_state(struct xwarcos_task * task,
                              enum xwarcos_task_state_em * state);

/**
 * @} arcos_task
 */

#endif /* xwos/arcos/task.h */
