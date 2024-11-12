/**
 * @file
 * @brief AUTOSAR::CP::Os::Spinlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tec>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef xwmd_autosarcp_os_Spinlock_h__
#define xwmd_autosarcp_os_Spinlock_h__

#include <xwmd/autosarcp/os/StdTypesWrapper.h>
#include <xwos/arcos/spinlock.h>

typedef struct xwarcos_spinlock * SpinlockIdType;

typedef enum xwarcos_try_to_get_spinlock_status_em TryToGetSpinlockType;
#define TRYTOGETSPINLOCK_SUCCESS XWARCOS_TRYTOGETSPINLOCK_SUCCESS
#define TRYTOGETSPINLOCK_NOSUCCESS XWARCOS_TRYTOGETSPINLOCK_NOSUCCESS

StatusType GetSpinlock(SpinlockIdType SpinlockID);
StatusType TryToGetSpinlock(SpinlockIdType SpinlockID, TryToGetSpinlockType * Success);
StatusType ReleaseSpinlock(SpinlockIdType SpinlockID);

#endif /* xwmd/autosarcp/os/Spinlock.h */
