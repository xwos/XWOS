/**
 * @file
 * @brief xwos::autosar::cp::os::alarm
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_alarm_h__
#define __xwos_arcos_alarm_h__

/**
 * @defgroup arcos_alarm xwos::autosar::cp::os::alarm
 * @ingroup arcos
 *
 * ## Compliances
 *
 * + [ ] [SWS_Os_00242] The Operating System module shall only allow Alarm Callbacks
 *       in Scalability Class 1. ()
 * + [ ] [SWS_Os_00304] If in a call to `SetRelAlarm()` the parameter "increment" is
 *       set to zero, the service shall return `E_OS_VALUE` in standard and
 *       extended status. ()
 * + [ ] [SWS_Os_00476] The Operating System module shall allow to automatically start
 *       preconfigured absolute alarms during the start of the Operating System. ()
 *
 * ### ISO:17356-3
 *
 * + [ ] 9 Alarms
 *   + [ ] More than one alarm can be attached to a counter.
 *   + [ ] Alarms can be defined to be either single alarms or cyclic alarms.
 *   + [ ] Alarm-callback routines run with category 2 interrupts disabled.
 *   + [ ] ALARMCALLBACK(AlarmCallbackRoutineName);
 *   + [ ] Depending on configuration, this alarm-callback routine is called
 *         when the alarm expires.
 *   + [ ] Depending on configuration, this task is activated
 *         when the alarm expires.
 *   + [ ] Depending on configuration, an event is set for this task
 *         when the alarm expires.
 *   + [ ] System services allowed in alarm-callback routines are listed in Figure 12-1.
 *
 *
 * ### Multi-Core
 *
 * + [ ] [SWS_Os_00632] If an Alarm expires, it shall be allowed to activate a Task
 *       on a different core. (SRS_Os_80018)
 * + [ ] [SWS_Os_00633] If an Alarm expires, it shall be allowed to set an Event on a
 *       different core. (SRS_Os_80018)
 * + [ ] [SWS_Os_00634] The AUTOSAR Operating System shall process an Alarm on
 *       the core on which its corresponding OS-Application resides. (SRS_Os_80018)
 * + [ ] [SWS_Os_00635] Alarm callbacks shall be executed on the core to which
 *       the Alarm is bound. This is only applicable to SC1 systems, because
 *       otherwise Alarm Callback are not allowed ([SWS_Os_00242]). (SRS_Os_80013)
 * + [ ] [SWS_Os_00636] `SetRelAlarm()` shall also work on an Alarm that is bound to
 *       another core. (SRS_Os_80013)
 * + [ ] [SWS_Os_00637] `SetAbsAlarm()` shall also work on an Alarm that is bound to
 *       another core. (SRS_Os_80013)
 * + [ ] [SWS_Os_00638] `CancelAlarm()` shall also work on an Alarm that is bound to
 *       another core. (SRS_Os_80013)
 * + [ ] [SWS_Os_00639] `GetAlarmBase()` shall also work on an Alarm that is bound to
 *       another core. (SRS_Os_80013)
 * + [ ] [SWS_Os_00640] `GetAlarm()` shall also work on an Alarm that is bound to
 *       another core. (SRS_Os_80013)
 *
 *
 * ## API Compliances
 *
 * + [ ] ISO:17356-3: 13.6 Alarm management
 *   + [ ] 13.6.1 Data types
 *     + [ ] `TickType`
 *     + [ ] `TickRefType`
 *     + [ ] `AlarmBaseType`
 *     + [ ] `AlarmBaseRefType`
 *     + [ ] `AlarmType`
 *   + [ ] 13.6.2 Constructional elements
 *     + [ ] 13.6.2.1 `DeclareAlarm()`
 *   + [ ] 13.6.3 System services
 *     + [ ] 13.6.3.1 `GetAlarmBase()`
 *     + [ ] 13.6.3.2 `GetAlarm()`
 *     + [ ] 13.6.3.3 `SetRelAlarm()`
 *     + [ ] 13.6.3.4 `SetAbsAlarm()`
 *     + [ ] 13.6.3.5 `CancelAlarm()`
 *   + [ ] 13.6.5 Naming convention
 *
 *
 * ## Configuration Compliances
 *
 * + [ ] [ECUC_Os_00016] OsAlarmSetEvent
 *   + [ ] [ECUC_Os_00017] OsAlarmSetEventRef
 *   + [ ] [ECUC_Os_00018] OsAlarmSetEventTaskRef
 * + [ ] [ECUC_Os_00003] OsAlarm
 *   + [ ] [ECUC_Os_00004] OsAlarmAccessingApplication
 *   + [ ] [ECUC_Os_00005] OsAlarmCounterRef
 * + [ ] [ECUC_Os_00006] OsAlarmAction
 * + [ ] [ECUC_Os_00007] OsAlarmActivateTask
 *   + [ ] [ECUC_Os_00008] OsAlarmActivateTaskRef
 * + [ ] [ECUC_Os_00009] OsAlarmAutostart
 *   + [ ] [ECUC_Os_00010] OsAlarmAlarmTime
 *   + [ ] [ECUC_Os_00011] OsAlarmAutostartType
 *   + [ ] [ECUC_Os_00012] OsAlarmCycleTime
 *   + [ ] [ECUC_Os_00013] OsAlarmAppModeRef
 * + [ ] [ECUC_Os_00014] OsAlarmCallback
 *   + [ ] [ECUC_Os_00087] OsAlarmCallbackName
 *   + [ ] [ECUC_Os_00409] OsMemoryMappingCodeLocationRef
 * + [ ] [ECUC_Os_00302] OsAlarmIncrementCounter
 *   + [ ] [ECUC_Os_00015] OsAlarmIncrementCounterRef
 *
 * @{
 */

/**
 * @} arcos_alarm
 */

#endif /* xwos/arcos/alarm.h */
