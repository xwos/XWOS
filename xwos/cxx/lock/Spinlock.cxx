/**
 * @file
 * @brief xwos::lock::Spinlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/lock/Spinlock.hxx"

namespace xwos {
namespace lock {

/* Spinlock::LkGrd Non-static Member */
Spinlock::LkGrd::LkGrd(Spinlock * spinlock)
    : mSpinlock(spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock(&mSpinlock->mLock);
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

Spinlock::LkGrd::LkGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock(&mSpinlock->mLock);
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

Spinlock::LkGrd::~LkGrd()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock(&mSpinlock->mLock);
        }
    }
}

void Spinlock::LkGrd::unlock()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock(&mSpinlock->mLock);
        }
    }
}

xwer_t Spinlock::LkGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

xwer_t Spinlock::LkGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

/* Spinlock::TryLkGrd Non-static Member */
Spinlock::TryLkGrd::TryLkGrd(Spinlock * spinlock)
    : LkGrd()
{
    mSpinlock = spinlock;
    if (nullptr != mSpinlock) {
        mRc = xwos_splk_trylock(&mSpinlock->mLock);
        if (XWOK != mRc) {
            mSpinlock = nullptr;
        } else {
            mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

Spinlock::TryLkGrd::TryLkGrd(Spinlock & spinlock)
    : LkGrd()
{
    mSpinlock = &spinlock;
    if (nullptr != mSpinlock) {
        mRc = xwos_splk_trylock(&mSpinlock->mLock);
        if (XWOK != mRc) {
            mSpinlock = nullptr;
        } else {
            mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

/* Spinlock::LkThGrd Non-static Member */
Spinlock::LkThGrd::LkThGrd(Spinlock * spinlock)
    : mSpinlock(spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_cpuirqsv(&mSpinlock->mLock, &mCpuirq);
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

Spinlock::LkThGrd::LkThGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_cpuirqsv(&mSpinlock->mLock, &mCpuirq);
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

Spinlock::LkThGrd::~LkThGrd()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock_cpuirqrs(&mSpinlock->mLock, mCpuirq);
        }
    }
}

void Spinlock::LkThGrd::unlock()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock_cpuirqrs(&mSpinlock->mLock, mCpuirq);
        }
    }
}

xwer_t Spinlock::LkThGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

xwer_t Spinlock::LkThGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

/* Spinlock::TryLkThGrd Non-static Member */
Spinlock::TryLkThGrd::TryLkThGrd(Spinlock * spinlock)
    : LkThGrd()
{
    mSpinlock = spinlock;
    if (nullptr != mSpinlock) {
        mRc = xwos_splk_trylock_cpuirqsv(&mSpinlock->mLock, &mCpuirq);
        if (XWOK != mRc) {
            mSpinlock = nullptr;
        } else {
            mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

Spinlock::TryLkThGrd::TryLkThGrd(Spinlock & spinlock)
    : LkThGrd()
{
    mSpinlock = &spinlock;
    if (nullptr != mSpinlock) {
        mRc = xwos_splk_trylock_cpuirqsv(&mSpinlock->mLock, &mCpuirq);
        if (XWOK != mRc) {
            mSpinlock = nullptr;
        } else {
            mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

/* Spinlock::LkBhGrd Non-static Member */
Spinlock::LkBhGrd::LkBhGrd(Spinlock * spinlock)
    : mSpinlock(spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_bh(&mSpinlock->mLock);
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

Spinlock::LkBhGrd::LkBhGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_bh(&mSpinlock->mLock);
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

Spinlock::LkBhGrd::~LkBhGrd()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock_bh(&mSpinlock->mLock);
        }
    }
}

void Spinlock::LkBhGrd::unlock()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock_bh(&mSpinlock->mLock);
        }
    }
}

xwer_t Spinlock::LkBhGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

xwer_t Spinlock::LkBhGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

/* Spinlock::TryLkBhGrd Non-static Member */
Spinlock::TryLkBhGrd::TryLkBhGrd(Spinlock * spinlock)
    : LkBhGrd()
{
    mSpinlock = spinlock;
    if (nullptr != mSpinlock) {
        mRc = xwos_splk_trylock_bh(&mSpinlock->mLock);
        if (XWOK != mRc) {
            mSpinlock = nullptr;
        } else {
            mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

Spinlock::TryLkBhGrd::TryLkBhGrd(Spinlock & spinlock)
    : LkBhGrd()
{
    mSpinlock = &spinlock;
    if (nullptr != mSpinlock) {
        mRc = xwos_splk_trylock_bh(&mSpinlock->mLock);
        if (XWOK != mRc) {
            mSpinlock = nullptr;
        } else {
            mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

/* Spinlock::LkIrqsGrd Non-static Member */
template<xwirq_t ... TIrqList>
Spinlock::LkIrqsGrd<TIrqList ...>::LkIrqsGrd(Spinlock * spinlock)
    : mSpinlock(spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_irqssv(&mSpinlock->mLock, mIrqs, mIrqFlags, sizeof...(TIrqList));
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

template<xwirq_t ... TIrqList>
Spinlock::LkIrqsGrd<TIrqList ...>::LkIrqsGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
    , mStatus(Spinlock::LockStatus::SpinlockUnlocked)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_irqssv(&mSpinlock->mLock, mIrqs, mIrqFlags, sizeof...(TIrqList));
        mStatus = Spinlock::LockStatus::SpinlockLocked;
    }
}

template<xwirq_t ... TIrqList>
Spinlock::LkIrqsGrd<TIrqList ...>::~LkIrqsGrd()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock_irqsrs(&mSpinlock->mLock, mIrqs, mIrqFlags,
                                    sizeof...(TIrqList));
        }
    }
}

template<xwirq_t ... TIrqList>
void Spinlock::LkIrqsGrd<TIrqList ...>::unlock()
{
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            mStatus = Spinlock::LockStatus::SpinlockUnlocked;
            xwos_splk_unlock_irqsrs(&mSpinlock->mLock, mIrqs, mIrqFlags,
                                    sizeof...(TIrqList));
        }
    }
}

template <xwirq_t ... TIrqList>
xwer_t Spinlock::LkIrqsGrd<TIrqList ...>::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

template <xwirq_t ... TIrqList>
xwer_t Spinlock::LkIrqsGrd<TIrqList ...>::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSpinlock) {
        if (Spinlock::LockStatus::SpinlockLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.splk = &mSpinlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SPLK, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_splk_lock(&mSpinlock->mLock);
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

/* Spinlock::LkIrqsGrd Non-static Member */
template<xwirq_t ... TIrqList>
Spinlock::TryLkIrqsGrd<TIrqList ...>::TryLkIrqsGrd(Spinlock * spinlock)
    : LkIrqsGrd<TIrqList ...>()
{
    LkIrqsGrd<TIrqList ...>::mSpinlock = spinlock;
    if (nullptr != LkIrqsGrd<TIrqList ...>::mSpinlock) {
        mRc = xwos_splk_trylock_irqssv(&LkIrqsGrd<TIrqList ...>::mSpinlock->mLock,
                                       LkIrqsGrd<TIrqList ...>::mIrqs,
                                       LkIrqsGrd<TIrqList ...>::mIrqFlags,
                                       sizeof...(TIrqList));
        if (XWOK != mRc) {
            LkIrqsGrd<TIrqList ...>::mSpinlock = nullptr;
        } else {
            LkIrqsGrd<TIrqList ...>::mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

template<xwirq_t ... TIrqList>
Spinlock::TryLkIrqsGrd<TIrqList ...>::TryLkIrqsGrd(Spinlock & spinlock)
    : LkIrqsGrd<TIrqList ...>()
{
    LkIrqsGrd<TIrqList ...>::mSpinlock = spinlock;
    if (nullptr != LkIrqsGrd<TIrqList ...>::mSpinlock) {
        mRc = xwos_splk_trylock_irqssv(&LkIrqsGrd<TIrqList ...>::mSpinlock->mLock,
                                       LkIrqsGrd<TIrqList ...>::mIrqs,
                                       LkIrqsGrd<TIrqList ...>::mIrqFlags,
                                       sizeof...(TIrqList));
        if (XWOK != mRc) {
            LkIrqsGrd<TIrqList ...>::mSpinlock = nullptr;
        } else {
            LkIrqsGrd<TIrqList ...>::mStatus = Spinlock::LockStatus::SpinlockLocked;
        }
    }
}

/* Spinlock Non-static Member */
Spinlock::Spinlock()
{
    xwos_splk_init(&mLock);
}

Spinlock::~Spinlock()
{
}

} // namespace xwos
} // namespace lock
