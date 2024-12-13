/**
 * @file
 * @brief xwos::lock::SMtx
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/lock/SMtx.hxx"

namespace xwos {
namespace lock {

/* SMtx::Grd Non-static Member */
SMtx::Grd::Grd(SMtx * mtx, enum LockMode mode, xwtm_t to)
    : mMtx(mtx)
{
    if (nullptr != mMtx) {
        switch (mode) {
            case SMtx::LockMode::SMtxLock:
                mRc = xwos_mtx_lock(&mMtx->mLock);
                break;
            case SMtx::LockMode::SMtxLockTimed:
                mRc = xwos_mtx_lock_to(&mMtx->mLock, to);
                break;
            case SMtx::LockMode::SMtxLockUninterruptable:
                mRc = xwos_mtx_lock_unintr(&mMtx->mLock);
                break;
            case SMtx::LockMode::SMtxLockTry:
                mRc = xwos_mtx_trylock(&mMtx->mLock);
                break;
        }
    }
}

SMtx::Grd::~Grd()
{
    if (nullptr != mMtx) {
        if (XWOK == mRc) {
            xwos_mtx_unlock(&mMtx->mLock);
        }
    }
}

/* SMtx Non-static Member */
SMtx::SMtx(xwpr_t pr)
{
    mCtorRc = xwos_mtx_init(&mLock, pr);
}

SMtx::~SMtx()
{
    xwos_mtx_fini(&mLock);
}

} // namespace xwos
} // namespace lock
