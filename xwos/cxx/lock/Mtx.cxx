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

#include "xwos/cxx/lock/Mtx.hxx"

namespace xwos {
namespace lock {

/* Mtx::Grd Non-static Member */
Mtx::Grd::Grd(Mtx * mtx, enum LockMode mode, xwtm_t to)
    : mMtx(mtx)
    , mLockMode(mode)
{
    if (nullptr != mMtx) {
        switch (mode) {
            case Mtx::LockMode::MtxLock:
                mRc = xwos_mtx_lock(mMtx->mMtxPtr);
                break;
            case Mtx::LockMode::MtxLockTimed:
                mRc = xwos_mtx_lock_to(mMtx->mMtxPtr, to);
                break;
            case Mtx::LockMode::MtxLockUninterruptable:
                mRc = xwos_mtx_lock_unintr(mMtx->mMtxPtr);
                break;
            case Mtx::LockMode::MtxLockTry:
                mRc = xwos_mtx_trylock(mMtx->mMtxPtr);
                break;
            case Mtx::LockMode::MtxUnlock:
                mRc = XWOK;
                break;
            default:
                mLockMode = Mtx::LockMode::MtxUnlock;
                mRc = XWOK;
                break;
        }
        if (mRc < 0) {
            mLockMode = Mtx::LockMode::MtxUnlock;
        }
    }
}

Mtx::Grd::~Grd()
{
    if (nullptr != mMtx) {
        if (Mtx::LockMode::MtxUnlock != mLockMode) {
            mLockMode = Mtx::LockMode::MtxUnlock;
            xwos_mtx_unlock(mMtx->mMtxPtr);
        }
    }
}

void Mtx::Grd::unlock()
{
    if (nullptr != mMtx) {
        if (Mtx::LockMode::MtxUnlock != mLockMode) {
            mLockMode = Mtx::LockMode::MtxUnlock;
            xwos_mtx_unlock(mMtx->mMtxPtr);
        }
    }
}

xwer_t Mtx::Grd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mMtx) {
        if (Mtx::LockMode::MtxUnlock != mLockMode) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.mtx = mMtx->mMtxPtr;
            if (Mtx::LockMode::MtxLockUninterruptable == mLockMode) {
                rc = xwos_cond_wait_unintr(cond->getXwosObj(), lock, XWOS_LK_MTX,
                                           nullptr, &lkst);
            } else {
                rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr,
                                    &lkst);
            }
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    mLockMode = Mtx::LockMode::MtxUnlock;
                }
            }
        } else {
            rc = -ENOLCK;
        }
    } else {
        rc = -EFAULT;
    }
    return rc;
}

xwer_t Mtx::Grd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mMtx) {
        if (Mtx::LockMode::MtxUnlock != mLockMode) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.mtx = mMtx->mMtxPtr;
            if (Mtx::LockMode::MtxLockUninterruptable == mLockMode) {
                rc = xwos_cond_wait_unintr(cond->getXwosObj(), lock, XWOS_LK_MTX,
                                           nullptr, &lkst);
            } else {
                rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr,
                                       to, &lkst);
            }
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    mLockMode = Mtx::LockMode::MtxUnlock;
                }
            }
        } else {
            rc = -ENOLCK;
        }
    } else {
        rc = -EFAULT;
    }
    return rc;
}

} // namespace lock
} // namespace xwos
