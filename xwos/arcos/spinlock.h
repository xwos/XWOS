/**
 * @file
 * @brief xwos::autosar::cp::os::spinlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_spinlock_h__
#define __xwos_arcos_spinlock_h__

#include <xwos/osal/lock/spinlock.h>
#include <xwos/arcos/error.h>

/**
 * @defgroup arcos_spinlock xwos::autosar::cp::os::spinlock
 * @ingroup arcos
 *
 * ## AUTOSAR CP OS Compliances
 *
 * ### Multi-Core
 *
 * + [x] [SWS_Os_00648] The AUTOSAR Operating System shall provide a spinlock
 *       mechanism that works across cores. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00649] The AUTOSAR Operating System shall provide a `GetSpinlock()`
 *       function which occupies a spinlock. If the spinlock is already occupied,
 *       `GetSpinlock()` shall keep on trying to occupy the spinlock until
 *       it succeeds. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00650] `GetSpinlock()` shall be callable from Task
 *       level. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00651] `GetSpinlock()` shall be callable from Category 2 ISR
 *       level. (SRS_Os_80021)
 *       The behavior of `GetSpinlock()` is undefined if called from a category 1 ISR.
 * + [x] [SWS_Os_00652] The AUTOSAR Operating System shall provide a
 *       `TryToGetSpinlock()` function which occupies a spinlock. If the spinlock is
 *       already occupied by a Task, `TryToGetSpinlock()` shall
 *       return. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00653] `TryToGetSpinlock()` shall be callable from Task
 *       level. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00654] `TryToGetSpinlock()` shall be callable from
 *       Category 2 ISR level. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00655] The AUTOSAR Operating System shall provide a
 *       `ReleaseSpinlock()` function which releases an occupied spinlock. If the
 *       spinlock is not occupied an error shall be
 *       returned. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00656] `ReleaseSpinlock()` shall be callable from
 *       Task level. (SRS_Os_80018, SRS_Os_80021)
 * + [x] [SWS_Os_00657] `ReleaseSpinlock() shall be callable from
 *       Category 2 ISR level. (SRS_Os_80018, SRS_Os_80021)
 * + [ ] [SWS_Os_00658] The AUTOSAR Operating System shall generate an error if a Task
 *       tries to occupy a spinlock that is assigned to a Task/Category 2 ISR on
 *       the same core (including itself). (SRS_Os_80018, SRS_Os_80021)
 * + [ ] [SWS_Os_00659] The AUTOSAR Operating System shall generate an error if an
 *       Category 2 ISR tries to occupy a spinlock that is assigned to a
 *       Task/Category 2 ISR on the same core. (SRS_Os_80018, SRS_Os_80021)
 * + [ ] [SWS_Os_00660] A unique order in which multiple spinlocks can be occupied by a
 *       Task/Category 2 ISR on one core should be configurable in the AUTOSAR
 *       Operating System. This might be realized by the configuration item
 *       (`OsSpinlockSuccessor` `{NEXT_SPINLOCK}`) where `NEXT_SPINLOCK` refers to the
 *       consecutive spinlock. (See `OsSpinlockSuccessor`) (SRS_Os_80018, SRS_Os_80021)
 * + [ ] [SWS_Os_00661] The AUTOSAR Operating System shall generate an error if a
 *       Task/Category 2 ISR on a core, where the same or a different Task/ISR already
 *       holds a spinlock, tries to seize another spinlock that has not been configured
 *       as a direct or indirect successor of the latest acquired spinlock (by means
 *       of the `OsSpinlockSuccessor` configuration parameter) or if no successor is
 *       configured. (SRS_Os_80018, SRS_Os_80021)
 *
 * ### API
 *
 * #### `GetSpinlock()`
 *
 * + [x] [SWS_Os_00686] Definition of API function `GetSpinlock()` . (SRS_Os_80021)
 * + [x] [SWS_Os_00687] The function `GetSpinlock()` shall occupy a spinlock.
 *       If the spinlock is already occupied the function shall busy wait
 *       until the spinlock becomes available. (SRS_Os_80021)
 * + [x] [SWS_Os_00688] The function `GetSpinlock()` shall return `E_OK` if no error
 *       was detected. The spinlock is now occupied by the calling Task/Category 2 ISR
 *       on the calling core. (SRS_Os_80021)
 *   + **注解** : XWOS的中断底半部机制类似于 **Category 2** ISR ，
 *     因此使用XWOS `_bh` 后缀的CAPI来实现AUTOSARCP OS自旋锁。
 * + [x] [SWS_Os_00689] The function `GetSpinlock()` shall return `E_OS_ID`
 *   if the parameter `SpinlockID` refers to
 *   a spinlock that does not exist. (SRS_Os_80021)
 * + [ ] [SWS_Os_00690] The function `GetSpinlock()` shall
 *   return `E_OS_INTERFERENCE_DEADLOCK` if the spinlock referred by the parameter
 *   `SpinlockID` is already occupied by a Task/Category 2 ISR
 *   on the same core. (SRS_Os_80021)
 * + [ ] [SWS_Os_00691] The function `GetSpinlock()` shall return
 *   `E_OS_NESTING_DEADLOCK` if the sequence by which multiple spinlocks are
 *   occupied at the same time on one core do not comply with
 *   the configured order. (SRS_Os_80021)
 * + [ ] [SWS_Os_00692] The function `GetSpinlock()` shall return `E_OS_ACCESS`
 *   if the accessing OS-Application was not listed in
 *   the configuration ( `OsSpinlock` ). (SRS_Os_80021)
 * + [x] [SWS_Os_00693] It shall be allowed to call the function `GetSpinlock()`
 *   while interrupts are disabled. (SRS_Os_80021)
 * + [x] [SWS_Os_00694] It shall be allowed to call the function `GetSpinlock()`
 *   while a Resource is occupied. (SRS_Os_80021)
 *
 * #### `ReleaseSpinlock()`
 *
 * + [x] [SWS_Os_00695] Definition of API function `ReleaseSpinlock()` . (SRS_Os_80021)
 * + [ ] [SWS_Os_00696] The function `ReleaseSpinlock()` shall release a spinlock that
 *   has been occupied by the same (calling) Task. If the related `GetSpinlock()`
 *   call used configured locks ( `OsSpinlockLockMethod` ) the function shall also
 *   perform the undo of the used lock. (SRS_Os_80021)
 * + [x] [SWS_Os_00697] The function `ReleaseSpinlock()` shall return `E_OK` if
 *   no error was detected. The spinlock is now free and can be occupied by
 *   the same or other Tasks. (SRS_Os_80021)
 * + [x] [SWS_Os_00698] The function `ReleaseSpinlock()` shall return `E_OS_ID` if
 *   the parameter `SpinlockID` refers to a spinlock that
 *   does not exist. (SRS_Os_80021)
 * + [ ] [SWS_Os_00699] The function `ReleaseSpinlock()` shall return `E_OS_STATE`
 *   if the parameter `SpinlockID` refers to a spinlock that is not occupied by
 *   the calling Task. (SRS_Os_80021)
 * + [ ] [SWS_Os_00700] The function `ReleaseSpinlock()` shall return `E_OS_ACCESS` if
 *   the Task has no access to the spinlock referred by
 *   the parameter `SpinlockID` . (SRS_Os_80021)
 * + [ ] [SWS_Os_00701] The function `ReleaseSpinlock()` shall return `E_OS_NOFUNC` if
 *   the Task tries to release a spinlock while another spinlock (or Resource) has to
 *   be released before. No functionality shall be performed. (SRS_Os_80021)
 *
 * #### `TryToGetSpinlock()`
 *
 * + [x] [SWS_Os_00703] Definition of API function `TryToGetSpinlock()` . (SRS_Os_80021)
 * + [x] [SWS_Os_00704] The function `TryToGetSpinlock()` shall atomically test
 *   the availability of the spinlock and if available occupy it. The result of
 *   success is returned. (SRS_Os_80021)
 * + [x] [SWS_Os_00705] The function `TryToGetSpinlock()` shall set the OUT parameter
 *   `Success` to `TRYTOGETSPINLOCK_SUCCESS` if the spinlock was successfully
 *   occupied, and `TRYTOGETSPINLOCK_NOSUCCESS` if not. In both cases `E_OK`
 *   shall be returned. (SRS_Os_80021)
 * + [x] [SWS_Os_00706] If the function `TryToGetSpinlock()` does not return `E_OK`,
 *   the OUT parameter `Success` shall be undefined. (SRS_Os_80021)
 * + [x] [SWS_Os_00707] The function `TryToGetSpinlock()` shall return `E_OS_ID`
 *   if the parameter `SpinlockID` refers to a spinlock that
 *   does not exist. (SRS_Os_80021)
 * + [ ] [SWS_Os_00708] The function `TryToGetSpinlock()` shall return
 *   `E_OS_INTERFERENCE_DEADLOCK` if the spinlock referred by the parameter `SpinlockID`
 *   is already occupied by a Task on the same core. (SRS_Os_80021)
 * + [ ] [SWS_Os_00709] The function `TryToGetSpinlock()` shall return
 *   `E_OS_NESTING_DEADLOCK` if a Task tries to occupy a spinlock while holding a
 *   different spinlock in a way that may cause a deadlock. (SRS_Os_80021)
 * + [ ] [SWS_Os_00710] The function `TryToGetSpinlock()` shall return `E_OS_ACCESS` if
 *   the Task has no access to the spinlock referred by
 *   the parameter `SpinlockID` . (SRS_Os_80021)
 * + [x] [SWS_Os_00711] It shall be allowed to call the function
 *   `TryToGetSpinlock()` while interrupts are disabled. (SRS_Os_80021)
 * + [x] [SWS_Os_00712] It shall be allowed to call the function
 *   `TryToGetSpinlock()` while a Resource is occupied. (SRS_Os_80021)
 *
 *
 * ### Configuration
 *
 * + [ ] [ECUC_Os_00258] OsSpinlock
 *   + [ ] [ECUC_Os_01038] OsSpinlockLockMethod
 *   + [ ] [ECUC_Os_01021] OsSpinlockAccessingApplication
 *   + [ ] [ECUC_Os_01022] OsSpinlockSuccessor
 *
 * @{
 */

/**
 * @brief AUTOSAR CP OS Spinlock
 * @note
 */
struct xwarcos_spinlock {
        struct xwos_splk splk;
};

#define XWARCOS_SPINLOCK_INITIALIZER { .splk = XWOS_SPLK_INITIALIZER, }

enum xwarcos_try_to_get_spinlock_status_em {
    XWARCOS_TRYTOGETSPINLOCK_SUCCESS,   /**< Spinlock successfully occupied. */
    XWARCOS_TRYTOGETSPINLOCK_NOSUCCESS  /**< Unable to occupy the spinlock. */
};

/**
 * @brief 获取自旋锁
 * @param[in] splk: 自旋锁的指针
 * @return 错误码
 * @retval E_XWARCOS_OK: 没有错误
 * @retval -E_XWARCOS_ID: splk是无效指针
 */
xwer_t xwarcos_spinlock_get(struct xwarcos_spinlock * splk);

/**
 * @brief 尝试获取自旋锁
 * @param[in] splk: 自旋锁的指针
 * @param[out] status: 指向缓冲区的指针，通过此缓冲区返回是否成功获取自旋锁的状态
 * @return 错误码
 * @retval E_XWARCOS_OK: 没有错误
 * @retval -E_XWARCOS_ID: splk是无效指针
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: status是无效指针
 */
xwer_t xwarcos_spinlock_try_to_get(struct xwarcos_spinlock * splk,
                                   enum xwarcos_try_to_get_spinlock_status_em * status);


/**
 * @brief 释放自旋锁
 * @param[in] splk: 自旋锁的指针
 * @return 错误码
 * @retval E_XWARCOS_OK: 没有错误
 * @retval -E_XWARCOS_ID: splk是无效指针
 */
xwer_t xwarcos_spinlock_release(struct xwarcos_spinlock * splk);

/**
 * @} arcos_spinlock
 */

#endif /* xwos/arcos/spinlock.h */
