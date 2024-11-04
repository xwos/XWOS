/**
 * @file
 * @brief xwos::autosar::cp::os::schedtbl
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_schedtbl_h__
#define __xwos_arcos_schedtbl_h__

#include <xwos/arcos/error.h>
#include <xwos/arcos/counter.h>

/**
 * @defgroup arcos_schedtbl xwos::autosar::cp::os::schedtbl
 * @ingroup arcos
 *
 * ## AUTOSAR CP OS Compliances
 *
 * ### ScheduleTable
 *
 * + [ ] [SWS_Os_00401] A ScheduleTable shall have at least one expiry point. ()
 * + [ ] [SWS_Os_00402] An expiry point shall contain a (possibly empty) set of
 *       Tasks to activate. ()
 * + [ ] [SWS_Os_00403] An expiry point shall contain a (possibly empty) set of
 *       Events to set. ()
 * + [ ] [SWS_Os_00404] An expiry point shall contain an offset in ticks from
 *       the start of the ScheduleTable. ()
 * + [ ] [SWS_Os_00407] An expiry point shall activate at least one Task OR
 *       set at least one event. ()
 * + [ ] [SWS_Os_00442] Each expiry point on a given ScheduleTable shall have a unique
 *       offset. ()
 * + [ ] [SWS_Os_00443] The Initial Offset shall be zero OR in the range
 *       `OsCounterMinCycle .. OsCounterMaxAllowedValue` of the underlying Counter. ()
 * + [ ] [SWS_Os_00408] The delay between adjacent expiry points shall be in the range
 *       `OsCounterMinCycle .. OsCounterMaxAllowedValue` of the underlying Counter. ()
 * + [ ] [SWS_Os_00002] The Operating System module shall process each expiry point on
 *       a ScheduleTable from the Initial Expiry Point to the Final Expiry Point
 *       in order of increasing offset. (SRS_Os_00098)
 * + [ ] [SWS_Os_00007] The Operating System module shall permit multiple Schedule
 *       Tables to be processed concurrently. (SRS_Os_00098)
 * + [ ] [SWS_Os_00409] A ScheduleTable of the Operating System module shall be
 *       driven by exactly one Counter. ()
 * + [ ] [SWS_Os_00410] The Operating System module shall be able to process at least
 *       one ScheduleTable per Counter at any given time. ()
 * + [x] [SWS_Os_00411] The Operating System module shall make use of ticks so that one
 *       tick on the Counter corresponds to one tick on the ScheduleTable. ()
 * + [ ] [SWS_Os_00412] If an expiry point contains actions to activate a Task and
 *       to set one or several Event(s) of the same Task, then the Operating System
 *       module shall process this Task activation before the related Event(s) are set.
 *       No further assumptions about the order for the processing of expiry points
 *       can be made. ()
 * + [ ] [SWS_Os_00413] The ScheduleTable shall be configurable as either single-shot
 *       or repeating. ()
 * + [ ] [SWS_Os_00009] If the ScheduleTable is single-shot, the Operating System
 *       module shall stop the processing of the ScheduleTable Final Delay ticks
 *       after the Final Expiry Point is processed. ()
 * + [ ] [SWS_Os_00427] If the ScheduleTable is single-shot, the Operating System
 *       module shall allow a Final Delay between `0 .. OsCounterMaxAllowedValue`
 *       of the underlying Counter. ()
 * + [x] [SWS_Os_00444] For periodic ScheduleTables the value of Final Delay
 *       shall be in the range `OsCounterMinCycle .. OsCounterMaxAllowedValue`
 *       of the underlying Counter. ()
 * + [ ] [SWS_Os_00194] After processing the Final Expiry Point, and if
 *       the ScheduleTable is repeating, the Operating System shall process the
 *       next Initial Expiry Point, after Final Delay plus Initial Offset ticks
 *       have elapsed. ()
 * + [ ] [SWS_Os_00428] If ScheduleTable processing has been cancelled
 *       before reaching the Final Expiry Point and is subsequently
 *       restarted then [SWS_Os_00358]/[SWS_Os_00347] means that the restart occurs
 *       from the start of the ScheduleTable. ()
 * + [ ] [SWS_Os_00414] When a ScheduleTable switch is requested, the OS shall
 *       continue to process expiry points on the current ScheduleTable.
 *       After the Final Expiry Point there will be a delay equivalent to
 *       Final Delay ticks before processing the switched-to ScheduleTable.
 *       The initial expiry point will be processed after initial offset. ()
 * + [ ] [SWS_Os_00510] dThe Operating System module shall perform the autostart of
 *       ScheduleTables during startup after the autostart of Tasks and Alarms. ()
 *
 * ### Multi-Core
 *
 * + [ ] [SWS_Os_00641] A ScheduleTable shall be able to activate a Task bound on a
 *       core other than the one upon which the ScheduleTables resides. (SRS_Os_80018)
 * + [ ] [SWS_Os_00642] A ScheduleTable shall be able to set an Event on a core other
 *       than the one upon which the ScheduleTables resides. (SRS_Os_80018)
 * + [ ] [SWS_Os_00643] The AUTOSAR Operating System shall process a ScheduleTable
 *       on the core on which its corresponding OS-Application resides. (SRS_Os_80013)
 * + [ ] [SWS_Os_00644] The API call `StartScheduleTableAbs()` shall be able to start
 *       ScheduleTables of OS-Applications residing on other cores. (SRS_Os_80018)
 * + [ ] [SWS_Os_00645] The API call `StartScheduleTableRel()` shall be able to start
 *       ScheduleTables of OS-Applications residing on other cores. (SRS_Os_80013)
 * + [ ] [SWS_Os_00646] The API call `StopScheduleTable()` shall be able to stop
 *       ScheduleTables of OS-Applications residing on other cores. (SRS_Os_80013)
 * + [ ] [SWS_Os_00647] The API service `GetScheduleTableStatus()` shall be able to
 *       get the status of a ScheduleTable that is part of an OS-Application residing
 *       on a different core. (SRS_Os_80013)
 *
 * ### ScheduleTable Synchronization
 *
 * + [ ] [SWS_Os_00013] The Operating System module shall provide the ability to
 *       synchronize the processing of ScheduleTable to known Counter values. ()
 * + [ ] [SWS_Os_00429] A ScheduleTable of the Operating System module that is
 *       implicitly synchronized shall have a Duration equal to
 *       `OsCounterMaxAllowedValue + 1` of its associated OSEK OS Counter. ()
 * + [ ] [SWS_Os_00430] The Operating System module shall prevent a ScheduleTable
 *       that is implicitly synchronized from being started at a relative count value.
 * + [ ] [SWS_Os_00431] A ScheduleTable that is explicitly synchronized shall have a
 *       duration no greater than modulus of the drive Counter. ()
 * + [ ] [SWS_Os_00462] A ScheduleTable that is explicitly synchronized shall have a
 *       duration equal to the modulus of the synchronization Counter. ()
 * + [ ] [SWS_Os_00463] The synchronization Counter shall have the same resolution as
 *       the drive Counter associated with the ScheduleTable. This means that
 *       a tick on the ScheduleTable has the same duration as a tick on
 *       the synchronization Counter. ()
 * + [ ] [SWS_Os_00435] If an explicitly synchronized ScheduleTable was started
 *       synchronously, then the Operating System module shall guarantee that
 *       it has state "waiting" when the call of service `StartScheduleTableSynchron()`
 *       returns. ()
 * + [ ] [SWS_Os_00415] An expiry point shall permit the configuration of an
 *       `OsScheduleTableMaxShorten` that defines the maximum number of ticks that
 *       can be subtracted from expiry point offset. ()
 * + [ ] [SWS_Os_00416] An expiry point shall permit the configuration of an
 *       `OsScheduleTableMaxLengthen` that defines the maximum number of ticks that
 *       can be added to expiry point offset. ()
 * + [ ] [SWS_Os_00436] The value of `(Offset - OsScheduleTableMaxShorten)` of
 *       an expiry point shall be greater than `(Offset + OsCounterMinCycle)` of
 *       the pervious expiry point. ()
 * + [ ] [SWS_Os_00559] The value of `OsScheduleTableMaxLengthen` shall be smaller
 *       than the duration of the ScheduleTable. ()
 * + [ ] [SWS_Os_00437] The value of `(OsScheduleTableMaxLengthen +
 *       delay_from_ previous_EP)` of an expiry point shall be less than
 *       the `OsCounterMaxAllowedValue` of the underlying Counter. ()
 * + [ ] [SWS_Os_00438] A ScheduleTable shall define a precision bound with a value in
 *       the range 0 to duration. ()
 * + [ ] [SWS_Os_00206] When a new synchronization count is provided, the Operating
 *       System module shall calculate the current deviation between the explicitly
 *       synchronized scheduled table and the synchronization count. (SRS_Os_11002)
 * + [ ] [SWS_Os_00417] The Operating System module shall start to synchronize
 *       an explicitly synchronized ScheduleTable after a synchronization count
 *       is provided AND shall continue to adjust expiry points until synchronized. ()
 * + [ ] [SWS_Os_00418] The Operating System module shall set the state of
 *       an explicitly synchronized ScheduleTable to "running and synchronous"
 *       if the deviation is less than or equal to the configured
 *       `OsScheduleTblExplicitPrecision` threshold. ()
 * + [ ] [SWS_Os_00419] The Operating System module shall set the state of
 *       an explicitly synchronized ScheduleTable to "running" if the deviation
 *       is greater than the configured `OsScheduleTblExplicitPrecision` threshold. ()
 * + [ ] [SWS_Os_00420] IF the deviation is non-zero AND the next expiry point
 *       is adjustable AND the table is behind the sync Counter
 *       `(TableTicksAheadOfSyncCounter <= TableTicksBehindOfSyncCounter)` THEN
 *       the OS shall set the next EP to expire `delay - min(MaxShorten, Deviation)`
 *       ticks from the current expiry. ()
 * + [ ] [SWS_Os_00421] IF the deviation is non-zero AND the next expiry point
 *       is adjustable AND the table is ahead of the sync
 *       `Counter (TableTicksAheadOfSyncCounter > TableTicksBehindOfSyncCounter)` THEN
 *       the OS shall set the next EP to expire `delay + min(MaxLengthen, Deviation)`
 *       ticks from the current expiry. ()
 *
 * ### API
 *
 * #### `StartScheduleTableRel()`
 *
 * + [x] [SWS_Os_00347] Definition of API function `StartScheduleTableRel()` . ()
 * + [x] [SWS_Os_00275] If the ScheduleTable `ScheduleTableID` in a call of
 *       `StartScheduleTableRel()` is not valid, `StartScheduleTableRel()` shall
 *       return `E_OS_ID` . ()
 * + [ ] [SWS_Os_00452] If the ScheduleTable `ScheduleTableID` in a call of
 *       `StartScheduleTableRel()` is implicitely synchronized
 *       `(OsScheduleTblSync-Strategy = IMPLICIT)` , `StartScheduleTableRel()` shall
 *       return `E_OS_ID` . ()
 * + [x] [SWS_Os_00332] If `Offset` in a call of `StartScheduleTableRel()` is zero,
 *       `StartScheduleTableRel()` shall return `E_OS_VALUE` . ()
 * + [x] [SWS_Os_00276] If the offset `Offset` is greater than
 *       `OsCounterMaxAllowedValue` of the underlying Counter minus the Initial Offset,
 *       `StartScheduleTableRel` shall return `E_OS_VALUE` . ()
 * + [x] [SWS_Os_00277] If the ScheduleTable `ScheduleTableID` in a call of
 *       `StartScheduleTableRel()` is not in the state `SCHEDULETABLE_STOPPED` ,
 *       `StartScheduleTableRel()` shall return `E_OS_STATE` . ()
 * + [x] [SWS_Os_00278] If the input parameters of `StartScheduleTableRel()` are valid
 *       and the state of ScheduleTable `ScheduleTableID` is `SCHEDULETABLE_STOPPED` ,
 *       then `StartScheduleTableRel()` shall start the processing of a ScheduleTable
 *       `ScheduleTableID` . The Initial Expiry Point shall be processed after
 *       `Offset` + Initial Offset ticks have elapsed on the underlying Counter.
 *       The state of `ScheduleTableID` is set to `SCHEDULETABLE_RUNNING` before
 *       the service returns to the caller. ()
 * + [ ] [SWS_Os_00521] Availability of StartScheduleTableRel: Available in all
 *       Scalability Classes. ()
 *
 * #### `StartScheduleTableAbs()`
 *
 * + [x] [SWS_Os_00358] Definition of API function `StartScheduleTableAbs()` . ()
 * + [x] [SWS_Os_00348] If the ScheduleTable `ScheduleTableID` in a call of
 *       `StartScheduleTableAbs()` is not valid, `StartScheduleTableAbs()` shall
 *       return E_OS_ID. ()
 * + [x] [SWS_Os_00349] If the `Start` in a call of `StartScheduleTableAbs()`
 *       is greater than the `OsCounterMaxAllowedValue` of the underlying Counter,
 *       `StartScheduleTableAbs()` shall return `E_OS_VALUE` . ()
 * + [x] [SWS_Os_00350] If the ScheduleTable `ScheduleTableID` in a call of
 *       `StartScheduleTableAbs()` is not in the state `SCHEDULETABLE_STOPPED` ,
 *       `StartScheduleTableAbs()` shall return `E_OS_STATE`. ()
 * + [x] [SWS_Os_00351] If the input parameters of `StartScheduleTableAbs()` are valid
 *       and `ScheduleTableID` is in the state `SCHEDULETABLE_STOPPED` ,
 *       `StartScheduleTableAbs()` shall start the processing of
 *       ScheduleTable `ScheduleTableID` when the underlying Counter next equals
 *       `Start` and shall set the state of `ScheduleTableID` to
 *       - `SCHEDULETABLE_RUNNING` (for a non-synchronized / Explicitly synchronized
 *         ScheduleTable) OR
 *       - `SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS` (for implicitly synchronized
 *         ScheduleTable)
 *       before returning to the user. (The Initial Expiry Point will be processed
 *       when the underlying Counter next equals `Start` + Initial Offset). ()
 * + [ ] [SWS_Os_00522] Availability of `StartScheduleTableAbs()` : Available in
 *       all Scalability Classes. ()
 *
 * #### `StopScheduleTable()`
 *
 * + [x] [SWS_Os_00006] Definition of API function `StopScheduleTable()` . ()
 * + [x] [SWS_Os_00279] If the ScheduleTable identifier `ScheduleTableID` in a call of
 *       StopScheduleTable is not valid, StopScheduleTable shall return E_OS_ID. ()
 * + [x] [SWS_Os_00280] If the ScheduleTable with identifier `ScheduleTableID` is in
 *       state `SCHEDULETABLE_STOPPED` when calling `StopScheduleTable()` ,
 *       `StopScheduleTable()` shall return `E_OS_NOFUNC` . ()
 * + [x] [SWS_Os_00281] If the input parameters of `StopScheduleTable()` are valid,
 * +     `StopScheduleTable()` shall set the state of `ScheduleTableID` to
 *       `SCHEDULETABLE_STOPPED` and (stop the ScheduleTable `ScheduleTableID` from
 *       processing any further expiry points and) shall return `E_OK`. ()
 * + [ ] [SWS_Os_00523] Availability of `StopScheduleTable()` : Available in all
 *       Scalability Classes. ()
 *
 * #### `NextScheduleTable()`
 *
 * + [ ] [SWS_Os_00191] Definition of API
 *       function `NextScheduleTable()` . (SRS_Os_00099)
 * + [ ] [SWS_Os_00282] If the input parameter `ScheduleTableID_From` or
 *       `ScheduleTableID_To` in a call of `NextScheduleTable()` is not valid,
 *       `NextScheduleTable()` shall return `E_OS_ID`. ()
 * + [ ] [SWS_Os_00330] If in a call of `NextScheduleTable()` ScheduleTable
 *       `ScheduleTableID_To` is driven by different Counter than
 *       ScheduleTable `ScheduleTableID_From` then `NextScheduleTable()` shall return
 *       an error `E_OS_ID` . ()
 * + [ ] [SWS_Os_00283] If the ScheduleTable `ScheduleTableID_From` in a call of
 *       `NextScheduleTable()` is in state `SCHEDULETABLE_STOPPED` OR in state
 *       `SCHEDULETABLE_NEXT`, `NextScheduleTable()` shall leave the state of
 *       `ScheduleTable_From` and `ScheduleTable_To` unchanged and
 *       return `E_OS_NOFUNC` . ()
 * + [ ] [SWS_Os_00309] If the ScheduleTable `ScheduleTableID_To` in a call of
 *       `NextScheduleTable()` is not in state `SCHEDULETABLE_STOPPED`,
 *       `NextScheduleTable()` shall leave the state of `ScheduleTable_From` and
 *       `ScheduleTable_To` unchanged and return `E_OS_STATE` . ()
 * + [ ] [SWS_Os_00484] If `OsScheduleTblSyncStrategy` of `ScheduleTableID_To` in
 *       a call of `NextScheduleTable()` is not equal to the `OsScheduleTblSyncStrategy`
 *       of `ScheduleTableID_From` then `NextScheduleTable()` shall
 *       return `E_OS_ID` . ()
 * + [ ] [SWS_Os_00284] If the input parameters of `NextScheduleTable()` are valid then
 *       `NextScheduleTable()` shall start the processing of ScheduleTable
 *       `ScheduleTableID_To` after `ScheduleTableID_From->FinalDelay` ticks when
 *       the Final Expiry Point on `ScheduleTableID_From` is processed and shall
 *       return `E_OK` . `NextScheduleTable()` shall process the Initial Expiry Point
 *       on `ScheduleTableID_To` at
 *       `ScheduleTableID_From->FinalDelay + `ScheduleTable_To` .
 *       Initial Offset ticks after the Final Expiry Point
 *       on `ScheduleTableID_From` is processed . ()
 * + [ ] [SWS_Os_00324] If the input parameters of `NextScheduleTable()` are valid AND
 *       the `ScheduleTableID_From` already has a "next" ScheduleTable then
 *       `NextScheduleTable()` shall replace the previous "next" ScheduleTable with
 *       `ScheduleTableID_To` and shall change the old "next" ScheduleTable state to
 *       `SCHEDULETABLE_STOPPED`. ()
 * + [ ] [SWS_Os_00505] If `OsScheduleTblSyncStrategy` of the ScheduleTables
 *       `ScheduleTableID_From` and `ScheduleTableID_To` in a call of
 *       `NextScheduleTable()` is EXPLICIT and the Operating System module already
 *       synchronizes `ScheduleTableID_From` , `NextScheduleTable()` shall continue
 *       synchonization after the start of processing `ScheduleTableID_To` . ()
 * + [ ] [SWS_Os_00453] If the `ScheduleTableID_From` in a call of `NextScheduleTable()`
 *       is stopped, `NextScheduleTable()` shall not start the "next" ScheduleTable and
 *       change its state to `SCHEDULETABLE_STOPPED` . ()
 * + [ ] [SWS_Os_00524] Availability of `NextScheduleTable()` : Available in all
 *       Scalability Classes. ()
 *
 * #### `StartScheduleTableSynchron()`
 *
 * + [ ] [SWS_Os_00201] Definition of API
 *       function `StartScheduleTableSynchron()` . (SRS_Os_11002)
 * + [ ] [SWS_Os_00387] If in a call of `StartScheduleTableSynchron()` the Schedule
 *       Table `ScheduleTableID` is not valid OR the ScheduleTable `ScheduleTableID`
 *       is not explicitly synchronized `(OsScheduleTblSyncStrategy != EXPLICIT)`
 *       `StartScheduleTableSynchron()` shall return `E_OS_ID` . ()
 * + [ ] [SWS_Os_00388] If the ScheduleTable `ScheduleTableID` in a call of
 *       `StartScheduleTableSynchron()` is not in the state `SCHEDULETABLE_STOPPED` ,
 *       `StartScheduleTableSynchron()` shall return `E_OS_STATE` . ()
 * + [ ] [SWS_Os_00389] If `ScheduleTableID` in a call of `StartScheduleTableSynchron()`
 *       is valid, `StartScheduleTableSynchron()` shall set the state of
 *       `ScheduleTableID` to `SCHEDULETABLE_WAITING` and start the processing of
 *       ScheduleTable `ScheduleTableID` after the synchronization count of the
 *       ScheduleTable is set via `SyncScheduleTable()` . The Initial Expiry Point
 *       shall be processed when `(Duration - SyncValue) + InitialOffset` ticks have
 *       elapsed on the synchronization Counter where:
 *       - `Duration` is `ScheduleTableID` `OsScheduleTableDuration`
 *       - `SyncValue` is the `Value` parameter passed to the `SyncScheduleTable()`
 *       - `InitialOffset` is the shortest expiry point offset in `ScheduleTableID`
 *       ()
 * + [ ] [SWS_Os_00525] Availability of `StartScheduleTableSynchron()` : Available in
 *       Scalability Classes 2 and 4. ()
 *
 * #### `SyncScheduleTable()`
 *
 * + [ ] [SWS_Os_00199] Definition of API
 *       function `SyncScheduleTable()` . (SRS_Os_11002)
 * + [ ] [SWS_Os_00454] If the <ScheduleTableID> in a call of SyncScheduleTable is
 *       not valid OR ScheduleTable can not be explicitely synchronized
 *       ( `OsScheduleTblSyncStrategy` is not equal to `EXPLICIT` )
 *       `SyncScheduleTable()` shall return `E_OS_ID` . ()
 * + [ ] [SWS_Os_00455] If the `Value` in a call of `SyncScheduleTable()` is greater
 *       or equal than the `OsScheduleTableDuration` , `SyncScheduleTable()` shall
 *       return `E_OS_VALUE` . ()
 * + [ ] [SWS_Os_00456] If the state of the ScheduleTable `ScheduleTableID` in a
 *       call of `SyncScheduleTable()` is equal to `SCHEDULETABLE_STOPPED` or
 *       `SCHEDULETABLE_NEXT` , `SyncScheduleTable()` shall return `E_OS_STATE` . ()
 * + [ ] [SWS_Os_00457] If the parameters in a call of `SyncScheduleTable()` are valid,
 *       `SyncScheduleTable()` shall provide the Operating System module with
 *       the current synchronization count for the given ScheduleTable. (It is used to
 *       synchronize the processing of the ScheduleTable to the synchronization
 *       Counter.) ()
 * + [ ] [SWS_Os_00526] Availability of SyncScheduleTable:
 *       Classes 2 and 4. ()
 *
 * #### `SetScheduleTableAsync()`
 *
 * + [ ] [SWS_Os_00422] Definition of API function `SetScheduleTableAsync()` . ()
 * + [ ] [SWS_Os_00362] If `SetScheduleTableAsync()` is called for a running Schedule
 *       Table, the Operating System module shall stop further synchronization until
 *       a `SyncScheduleTable()` call is made. ()
 * + [ ] [SWS_Os_00323] If `SetScheduleTableAsync()` is called for a running Schedule
 *       Table the Operating System module shall continue to process expiry points
 *       on the ScheduleTable. ()
 * + [ ] [SWS_Os_00458] If `OsScheduleTblSyncStrategy` of `ScheduleTableID` in a
 *       call of `SetScheduleTableAsync()` is not equal to `EXPLICIT` OR if
 *       `ScheduleTableID` is invalid then `SetScheduleTableAsync()` shall return
 *       `E_OS_ID` . ()
 * + [ ] [SWS_Os_00483] If the current state of the `ScheduleTableID` in a call
 *       of `SetScheduleTableAsync()` equals to `SCHEDULETABLE_STOPPED` ,
 *       `SCHEDULETABLE_NEXT` or `SCHEDULETABLE_WAITING` then `SetScheduleTableAsync()`
 *       shall return `E_OS_STATE` . ()
 * + [ ] [SWS_Os_00300] If the current state of `ScheduleTableID` in a call of
 *       `SetScheduleTableAsync()` equals `SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS` (or
 *       `SCHEDULETABLE_RUNNING` ) then `SetScheduleTableAsync()` shall set (or keep in
 *       case of `SCHEDULETABLE_RUNNING` ) the status of `ScheduleTableID` to
 *       `SCHEDULETABLE_RUNNING` . ()
 * + [ ] [SWS_Os_00527] Availability of `SetScheduleTableAsync()` : Available in
 *       Scalability Classes 2 and 4. ()
 *
 *
 * #### `GetScheduleTableStatus()`
 *
 * + [x] [SWS_Os_00227] Definition of API
 *       function `GetScheduleTableStatus()` . (SRS_Os_11002)
 * + [x] [SWS_Os_00289] If the ScheduleTable `ScheduleTableID` in a call of
 *       `GetScheduleTableStatus()` is NOT started, `GetScheduleTableStatus()` shall
 *       pass back `SCHEDULETABLE_STOPPED` via the reference parameter `ScheduleStatus`
 *       AND shall return `E_OK` . ()
 * + [x] [SWS_Os_00353] If the ScheduleTable `ScheduleTableID` in a call of
 *       `GetScheduleTableStatus()` was used in a `NextScheduleTable()` call AND waits
 *       for the end of the current ScheduleTable, `GetScheduleTableStatus()` shall
 *       return `SCHEDULETABLE_NEXT` via the reference parameter `ScheduleStatus` AND
 *       shall return `E_OK` . ()
 * + [ ] [SWS_Os_00354] If the ScheduleTable `ScheduleTableID` in a call of
 *       `GetScheduleTableStatus()` is configured with explicit synchronization AND
 *       `ScheduleTableID` was started with `StartScheduleTableSynchron()` AND no
 *       synchronization count was provided to the Operating System,
 *       `GetScheduleTableStatus()` shall return `SCHEDULETABLE_WAITING` via
 *       the reference parameter `ScheduleStatus` AND shall return `E_OK` . ()
 * + [ ] [SWS_Os_00290] If the ScheduleTable `ScheduleTableID` in a call of
 *       `GetScheduleTableStatus()` is started AND synchronous,
 *       `GetScheduleTableStatus()` shall pass back
 *       `SCHEDULETABLE_RUNNING_AND_SYNCHRONOUS` via the
 *       reference parameter `ScheduleStatus` AND shall return `E_OK` . ()
 * + [ ] [SWS_Os_00291] If the ScheduleTable `ScheduleTableID` in a call of
 *       `GetScheduleTableStatus()` is started AND NOT synchronous (deviation is not
 *       within the precision interval OR the ScheduleTable has been set asynchronous),
 *       `GetScheduleTableStatus()` shall pass back `SCHEDULETABLE_RUNNING` via the
 *       reference parameter `ScheduleStatus` AND shall return `E_OK` . ()
 * + [x] [SWS_Os_00293] If the identifier `ScheduleTableID` in a call of
 *       `GetScheduleTableStatus()` is NOT valid, `GetScheduleTableStatus()` shall
 *       return E_OS_ID. ()
 * + [ ] [SWS_Os_00528] Availability of `GetScheduleTableStatus()` : Available in
 *       all Scalability Classes. ()
 *
 * ### Configuration Compliances
 *
 * + [x] [ECUC_Os_00141] OsScheduleTable
 *   + [x] [ECUC_Os_00053] OsScheduleTableDuration
 *   + [x] [ECUC_Os_00144] OsScheduleTableRepeating
 *   + [x] [ECUC_Os_00145] OsScheduleTableCounterRef
 *   + [ ] [ECUC_Os_00054] OsSchTblAccessingApplication
 * + [ ] [ECUC_Os_00335] OsScheduleTableAutostart
 *   + [ ] [ECUC_Os_00056] OsScheduleTableAutostartType
 *   + [ ] [ECUC_Os_00057] OsScheduleTableStartValue
 *   + [ ] [ECUC_Os_00058] OsScheduleTableAppModeRef
 * + [ ] [ECUC_Os_00059] OsScheduleTableEventSetting
 *   + [ ] [ECUC_Os_00060] OsScheduleTableSetEventRef
 *   + [ ] [ECUC_Os_00061] OsScheduleTableSetEventTaskRef
 * + [x] [ECUC_Os_00143] OsScheduleTableExpiryPoint
 *   + [x] [ECUC_Os_00062] OsScheduleTblExpPointOffset
 * + [ ] [ECUC_Os_00066] OsScheduleTableTaskActivation
 *   + [ ] [ECUC_Os_00067] OsScheduleTableActivateTaskRef
 * + [ ] [ECUC_Os_00068] OsScheduleTblAdjustableExpPoint
 *   + [ ] [ECUC_Os_00069] OsScheduleTableMaxLengthen
 *   + [ ] [ECUC_Os_00070] OsScheduleTableMaxShorten
 * + [ ] [ECUC_Os_00063] OsScheduleTableSync
 *   + [ ] [ECUC_Os_00064] OsScheduleTblExplicitPrecision
 *   + [ ] [ECUC_Os_00065] OsScheduleTblSyncStrategy
 * @{
 */

struct xwarcos_schedtbl;
struct xwarcos_schedtbl_ep;

/**
 * @brief AUTOSAR CP OS Schedule Ep Action
 */
typedef void (* xwarcos_schedtbl_action_f)(struct xwarcos_schedtbl *,
                                           const struct xwarcos_schedtbl_ep *);

/**
 * @brief AUTOSAR CP OS Schedule Table Expiry Point [ECUC_Os_00143]
 */
struct xwarcos_schedtbl_ep {
        xwtk_t offset; /**< [ECUC_Os_00062] */
        xwsz_t action_number;
        const xwarcos_schedtbl_action_f * actions;
};

/**
 * @brief AUTOSAR CP OS Schedule Table Status Enumeration
 */
enum xwarcos_schedtbl_status_em {
        XWARCOS_SCHEDTBL_STATUS_STOPPED,
        XWARCOS_SCHEDTBL_STATUS_NEXT,
        XWARCOS_SCHEDTBL_STATUS_RUNNING,
};

/**
 * @brief AUTOSAR CP OS Schedule Table Configurations
 */
struct xwarcos_schedtbl_config {
        struct xwarcos_counter * counter; /**< [ECUC_Os_00145] */
        bool repeating; /**< [ECUC_Os_00144] */
        xwtk_t duration; /**< [ECUC_Os_00053] */
        xwsz_t ep_number;
        const struct xwarcos_schedtbl_ep * ep;
};

/**
 * @brief AUTOSAR CP OS 调度表
 */
struct xwarcos_schedtbl {
        const struct xwarcos_schedtbl_config * cfg;
        struct xwarcos_counter_node node;
        enum xwarcos_schedtbl_status_em status;
        xwtk_t start_point;
        xwtk_t start_offset;
        xwtk_t tick;
        xwsq_t epidx;
};

xwer_t xwarcos_schedtbl_validate_and_init(struct xwarcos_schedtbl * schedtbl);
bool xwarcos_schedtbl_counter_operation(struct xwarcos_counter * counter,
                                        struct xwarcos_counter_node * node);
xwer_t xwarcos_schedtbl_start_rel(struct xwarcos_schedtbl * schedtbl, xwtk_t offset);
xwer_t xwarcos_schedtbl_start_abs(struct xwarcos_schedtbl * schedtbl, xwtk_t start);
xwer_t xwarcos_schedtbl_stop(struct xwarcos_schedtbl * schedtbl);
xwer_t xwarcos_schedtbl_get_status(struct xwarcos_schedtbl * schedtbl,
                                   enum xwarcos_schedtbl_status_em * status);
xwer_t xwarcos_schedtbl_next(struct xwarcos_schedtbl * from,
                             struct xwarcos_schedtbl * to);

/**
 * @} arcos_schedtbl
 */

#endif /* xwos/arcos/schedtbl.h */
