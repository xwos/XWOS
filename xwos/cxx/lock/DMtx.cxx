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
    , mLockMode(mode)
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
            case DMtx::LockMode::DMtxUnlock:
                mRc = XWOK;
                break;
            default:
                mLockMode = DMtx::LockMode::DMtxUnlock;
                mRc = XWOK;
                break;
        }
        if (mRc < 0) {
            mLockMode = DMtx::LockMode::DMtxUnlock;
        }
    }
}

DMtx::Grd::~Grd()
{
    if (nullptr != mMtx) {
        if (DMtx::LockMode::DMtxUnlock != mLockMode) {
            mLockMode = DMtx::LockMode::DMtxUnlock;
            xwos_mtx_unlock(mMtx->mMtxDesc.mtx);
        }
    }
}

void DMtx::Grd::unlock()
{
    if (nullptr != mMtx) {
        if (DMtx::LockMode::DMtxUnlock != mLockMode) {
            mLockMode = DMtx::LockMode::DMtxUnlock;
            xwos_mtx_unlock(mMtx->mMtxDesc.mtx);
        }
    }
}

xwer_t DMtx::Grd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mMtx) {
        if (DMtx::LockMode::DMtxUnlock != mLockMode) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.mtx = mMtx->mMtxDesc.mtx;
            if (DMtx::LockMode::DMtxLockUninterruptable == mLockMode) {
                rc = xwos_cond_wait_unintr(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, &lkst);
            } else {
                rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, &lkst);
            }
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    mLockMode = DMtx::LockMode::DMtxUnlock;
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

xwer_t DMtx::Grd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mMtx) {
        if (DMtx::LockMode::DMtxUnlock != mLockMode) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.mtx = mMtx->mMtxDesc.mtx;
            if (DMtx::LockMode::DMtxLockUninterruptable == mLockMode) {
                rc = xwos_cond_wait_unintr(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, &lkst);
            } else {
                rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_MTX, nullptr, to, &lkst);
            }
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    mLockMode = DMtx::LockMode::DMtxUnlock;
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
