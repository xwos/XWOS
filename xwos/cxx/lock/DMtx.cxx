/**
 * @file
 * @brief xwos::lock::DMtx
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/lock/DMtx.hxx"

namespace xwos {
namespace lock {

/* DMtx::Grd Non-static Member */
DMtx::Grd::Grd(DMtx * mtx, enum LockMode mode, xwtm_t to)
    : mMtx(mtx)
{
    if (nullptr != mMtx) {
        switch (mode) {
            case DMtx::LockMode::DMtxLock:
                mRc = xwos_mtx_lock(mMtx->mMtxDesc.mtx);
                break;
            case DMtx::LockMode::DMtxLockTimed:
                mRc = xwos_mtx_lock_to(mMtx->mMtxDesc.mtx, to);
                break;
            case DMtx::LockMode::DMtxLockUninterruptable:
                mRc = xwos_mtx_lock_unintr(mMtx->mMtxDesc.mtx);
                break;
            case DMtx::LockMode::DMtxLockTry:
                mRc = xwos_mtx_trylock(mMtx->mMtxDesc.mtx);
                break;
        }
    }
}

DMtx::Grd::~Grd()
{
    if (nullptr != mMtx) {
        if (XWOK == mRc) {
            xwos_mtx_unlock(mMtx->mMtxDesc.mtx);
        }
    }
}

/* DMtx Non-static Member */
DMtx::DMtx(xwpr_t pr)
{
    mCtorRc = xwos_mtx_create(&mMtxDesc, pr);
}

DMtx::~DMtx()
{
    xwos_mtx_delete(mMtxDesc);
}

} // namespace xwos
} // namespace lock
