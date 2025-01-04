/**
 * @file
 * @brief xwos::sync::Sem
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_sync_Sem_hxx__
#define __xwos_cxx_sync_Sem_hxx__

#include <xwos/osal/sync/sem.hxx>

namespace xwos {
namespace sync {

/**
 * @defgroup xwos_cxx_sync_Sem 信号量基类
 * @ingroup xwos_cxx_sync
 *
 */

/**
 * @brief 信号量基类
 */
class Sem
{
  protected:
    struct xwos_sem * mSemPtr;
  protected:
    explicit Sem(xwos_sem * sem) : mSemPtr(sem) {}
    ~Sem() { mSemPtr = nullptr; }

    xwer_t post() { return xwos_sem_post(mSemPtr); }
    xwer_t wait() { return xwos_sem_wait(mSemPtr); }
    xwer_t waitTo(xwtm_t to) { return xwos_sem_wait_to(mSemPtr, to); }
    xwer_t tryWait() { return xwos_sem_trywait(mSemPtr); }
    xwer_t waitUnintr() { return xwos_sem_wait_unintr(mSemPtr); }
    xwer_t freeze() { return xwos_sem_freeze(mSemPtr); }
    xwer_t thaw() { return xwos_sem_thaw(mSemPtr); }
    xwssq_t max() {
        xwssq_t max;
        xwos_sem_get_max(mSemPtr, &max);
        return max;
    }
    xwssq_t value() {
        xwssq_t value;
        xwos_sem_get_value(mSemPtr, &value);
        return value;
    }

  public:
    struct xwos_sem * getXwosObj() { return mSemPtr; }
};

/**
 * @} xwos_cxx_sync_Sem
 */

} // namespace sync
} // namespace xwos

#endif /* xwos/cxx/sync/Sem.hxx */
