/**
 * @file
 * @brief AUTOSAR::CP::Os::Spinlock
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwmd/autosarcp/os/Spinlock.h>
#include <xwmd/autosarcp/os/Error.h>

extern struct xwarcos_spinlock * const xwarcos_spinlock_table[];
extern const xwu32_t xwarcos_spinlock_num;

StatusType GetSpinlock(SpinlockIdType SpinlockID)
{
        xwer_t xwrc;
        struct xwarcos_spinlock * spinlock;

        if (SpinlockID >= xwarcos_spinlock_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                spinlock = xwarcos_spinlock_table[SpinlockID];
                xwrc = xwarcos_spinlock_get(spinlock);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType TryToGetSpinlock(SpinlockIdType SpinlockID, TryToGetSpinlockType * Success)
{
        xwer_t xwrc;
        struct xwarcos_spinlock * spinlock;

        if (SpinlockID >= xwarcos_spinlock_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                spinlock = xwarcos_spinlock_table[SpinlockID];
                xwrc = xwarcos_spinlock_try_to_get(spinlock, Success);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}

StatusType ReleaseSpinlock(SpinlockIdType SpinlockID)
{
        xwer_t xwrc;
        struct xwarcos_spinlock * spinlock;

        if (SpinlockID >= xwarcos_spinlock_num) {
                xwrc = E_XWARCOS_ID;
        } else {
                spinlock = xwarcos_spinlock_table[SpinlockID];
                xwrc = xwarcos_spinlock_release(spinlock);
        }
        return E_XWARCOS_TO_STATUSTYPE(xwrc);
}
