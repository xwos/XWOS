/**
 * @file
 * @brief xwos::autosar::cp::os::error
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_error_h__
#define __xwos_arcos_error_h__

#include <xwos/standard.h>

/**
 * @defgroup arcos_task xwos::autosar::cp::os::error
 * @ingroup error
 *
 * ## ISO:17356-3 Compliances
 *
 * ### All errors of API services
 *
 * + [x] E_OS_ACCESS = 1
 * + [x] E_OS_CALLEVEL = 2
 * + [x] E_OS_ID = 3
 * + [x] E_OS_LIMIT = 4
 * + [x] E_OS_NOFUNC = 5
 * + [x] E_OS_RESOURCE = 6
 * + [x] E_OS_STATE = 7
 * + [x] E_OS_VALUE = 8
 *
 *
 * ## AUTOSAR CP OS Compliances
 *
 * + [x] [SWS_Os_91025] Definiton of development errors in module Os. (SRS_BSW_00480)
 *   + [x] E_OS_ILLEGAL_ADDRESS
 *   + [x] E_OS_PROTECTION_MEMORY
 *   + [x] E_OS_STACKFAULT
 *   + [x] E_OS_CORE
 *   + [x] E_OS_NESTING_DEADLOCK
 *   + [x] E_OS_MISSINGEND
 *   + [x] E_OS_PROTECTION_LOCKED
 *   + [x] E_OS_SPINLOCK
 *   + [x] E_OS_SERVICEID
 *   + [x] E_OS_PROTECTION_EXCEPTION
 *   + [x] E_OS_INTERFERENCE_DEADLOCK
 *   + [x] E_OS_PROTECTION_TIME
 *   + [x] E_OS_DISABLEDINT
 *   + [x] E_OS_PROTECTION_ARRIVAL
 *   + [x] E_OS_SYS_XWOS_THD_INIT
 *
 * ### Protection
 *
 * ### Configuration
 *
 * @{
 */

#define E_XWARCOS_BASE                          0
#define E_XWARCOS_OK                            (E_XWARCOS_BASE + 0)
#define E_XWARCOS_ACCESS                        (E_XWARCOS_BASE + 1) /**< Wrong call context */
#define E_XWARCOS_CALLEVEL                      (E_XWARCOS_BASE + 2) /**< Permission denied */
#define E_XWARCOS_ID                            (E_XWARCOS_BASE + 3) /**< Invalid AUTOSAR CP OS Object ID */
#define E_XWARCOS_LIMIT                         (E_XWARCOS_BASE + 4) /**< Too many task activations */
#define E_XWARCOS_NOFUNC                        (E_XWARCOS_BASE + 5)
#define E_XWARCOS_RESOURCE                      (E_XWARCOS_BASE + 6) /**< Still occupies resources */
#define E_XWARCOS_STATE                         (E_XWARCOS_BASE + 7)
#define E_XWARCOS_VALUE                         (E_XWARCOS_BASE + 8)
#define E_XWARCOS_ILLEGAL_ADDRESS               (E_XWARCOS_BASE + 9) /**< An invalid address is given as a parameter to a service. */
#define E_XWARCOS_STACKFAULT                    (E_XWARCOS_BASE + 10) /**< A stack fault detected via stack monitoring by the OS */
#define E_XWARCOS_CORE                          (E_XWARCOS_BASE + 11) /**< Core is not match */
#define E_XWARCOS_SERVICEID                     (E_XWARCOS_BASE + 12) /**< Service cannot be called */
#define E_XWARCOS_MISSINGEND                    (E_XWARCOS_BASE + 13) /**< Tasks terminates without a TerminateTask() or ChainTask() call. */
#define E_XWARCOS_DISABLEDINT                   (E_XWARCOS_BASE + 14) /**< A service of the OS is called inside an interrupt disable/enable pair */
#define E_XWARCOS_SPINLOCK                      (E_XWARCOS_BASE + 15) /**< De-scheduling with occupied spinlock */
#define E_XWARCOS_NESTING_DEADLOCK              (E_XWARCOS_BASE + 16) /**< Potential deadlock due to wrong nesting */
#define E_XWARCOS_INTERFERENCE_DEADLOCK         (E_XWARCOS_BASE + 17) /**< Deadlock situation due to interference */
#define E_XWARCOS_PROTECTION_MEMORY             (E_XWARCOS_BASE + 18) /**< A memory access violation occurred */
#define E_XWARCOS_PROTECTION_LOCKED             (E_XWARCOS_BASE + 19) /**< A Task/Category 2 ISR blocks for too long */
#define E_XWARCOS_PROTECTION_EXCEPTION          (E_XWARCOS_BASE + 20) /**< A trap occurred */
#define E_XWARCOS_PROTECTION_TIME               (E_XWARCOS_BASE + 21) /**< A Task or Category 2 ISR exceeds its execution time budget */
#define E_XWARCOS_PROTECTION_ARRIVAL            (E_XWARCOS_BASE + 22) /**< A Task/Category 2 ISR arrives before its timeframe has expired */
#define E_XWARCOS_SYS_NO_RESTART                (E_XWARCOS_BASE + 23)
#define E_XWARCOS_SYS_XWOS_THD_INIT             (E_XWARCOS_BASE + 80) /**< `xwos_thd_init() returns error` */

/**
 * @} arcos_error
 */

#endif /* xwos/arcos/error.h */
