/**
 * @file
 * @brief AUTOSAR::CP::Os::Spinlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Spinlock.h>
#include <xwmd/autosarcp/os/Error.h>

StatusType GetSpinlock(SpinlockIdType SpinlockID)
{
        xwer_t xwrc;

        xwrc = xwarcos_spinlock_get(SpinlockID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType TryToGetSpinlock(SpinlockIdType SpinlockID, TryToGetSpinlockType * Success)
{
        xwer_t xwrc;

        xwrc = xwarcos_spinlock_try_to_get(SpinlockID, Success);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType ReleaseSpinlock(SpinlockIdType SpinlockID)
{
        xwer_t xwrc;

        xwrc = xwarcos_spinlock_release(SpinlockID);
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
