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
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock(&mSpinlock->mLock);
    }
}

Spinlock::LkGrd::LkGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock(&mSpinlock->mLock);
    }
}

Spinlock::LkGrd::~LkGrd()
{
    if (nullptr != mSpinlock) {
        xwos_splk_unlock(&mSpinlock->mLock);
    }
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
        }
    }
}

/* Spinlock::LkThGrd Non-static Member */
Spinlock::LkThGrd::LkThGrd(Spinlock * spinlock)
    : mSpinlock(spinlock)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_cpuirqsv(&mSpinlock->mLock, &mCpuirq);
    }
}

Spinlock::LkThGrd::LkThGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_cpuirqsv(&mSpinlock->mLock, &mCpuirq);
    }
}

Spinlock::LkThGrd::~LkThGrd()
{
    if (nullptr != mSpinlock) {
        xwos_splk_unlock_cpuirqrs(&mSpinlock->mLock, mCpuirq);
    }
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
        }
    }
}

/* Spinlock::LkBhGrd Non-static Member */
Spinlock::LkBhGrd::LkBhGrd(Spinlock * spinlock)
    : mSpinlock(spinlock)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_bh(&mSpinlock->mLock);
    }
}

Spinlock::LkBhGrd::LkBhGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_bh(&mSpinlock->mLock);
    }
}

Spinlock::LkBhGrd::~LkBhGrd()
{
    if (nullptr != mSpinlock) {
        xwos_splk_unlock_bh(&mSpinlock->mLock);
    }
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
        }
    }
}

/* Spinlock::LkIrqsGrd Non-static Member */
template<xwirq_t ... TIrqList>
Spinlock::LkIrqsGrd<TIrqList ...>::LkIrqsGrd(Spinlock * spinlock)
    : mSpinlock(spinlock)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_irqssv(&mSpinlock->mLock, mIrqs, mIrqFlags, sizeof...(TIrqList));
    }
}

template<xwirq_t ... TIrqList>
Spinlock::LkIrqsGrd<TIrqList ...>::LkIrqsGrd(Spinlock & spinlock)
    : mSpinlock(&spinlock)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSpinlock) {
        xwos_splk_lock_irqssv(&mSpinlock->mLock, mIrqs, mIrqFlags,
                              sizeof...(TIrqList));
    }
}

template<xwirq_t ... TIrqList>
Spinlock::LkIrqsGrd<TIrqList ...>::~LkIrqsGrd()
{
    if (nullptr != mSpinlock) {
        xwos_splk_unlock_irqsrs(&mSpinlock->mLock, mIrqs, mIrqFlags,
                                sizeof...(TIrqList));
    }
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
