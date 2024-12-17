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
    , mLockMode(mode)
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
            case SMtx::LockMode::SMtxUnlock:
                mRc = XWOK;
                break;
            default:
                mLockMode = SMtx::LockMode::SMtxUnlock;
                mRc = XWOK;
                break;
        }
    }
}

SMtx::Grd::~Grd()
{
    if (nullptr != mMtx) {
        if (SMtx::LockMode::SMtxUnlock != mLockMode) {
            xwos_mtx_unlock(&mMtx->mLock);
        }
    }
}

xwer_t SMtx::Grd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mMtx) {
        union xwos_ulock lock;
        xwsq_t lkst;
        lock.osal.mtx = &mMtx->mLock;
        if (SMtx::LockMode::SMtxLockUninterruptable == mLockMode) {
            rc = xwos_cond_wait_unintr(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, &lkst);
        } else {
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, &lkst);
        }
        if (rc < 0) {
            if (XWOS_LKST_UNLOCKED == lkst) {
                mLockMode = SMtx::LockMode::SMtxUnlock;
            }
        }
    } else {
        rc = -EFAULT;
    }
    return rc;
}

xwer_t SMtx::Grd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mMtx) {
        union xwos_ulock lock;
        xwsq_t lkst;

        lock.osal.mtx = &mMtx->mLock;
        if (SMtx::LockMode::SMtxLockUninterruptable == mLockMode) {
            rc = xwos_cond_wait_unintr(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, &lkst);
        } else {
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, to, &lkst);
        }
        if (rc < 0) {
            if (XWOS_LKST_UNLOCKED == lkst) {
                mLockMode = SMtx::LockMode::SMtxUnlock;
            }
        }
    } else {
        rc = -EFAULT;
    }
    return rc;
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
