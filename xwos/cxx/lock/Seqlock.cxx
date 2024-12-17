/**
 * @file
 * @brief xwos::lock::Seqlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include "xwos/cxx/lock/Seqlock.hxx"

namespace xwos {
namespace lock {

/* Seqlock::RdexLkGrd Non-static Member */
Seqlock::RdexLkGrd::RdexLkGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

Seqlock::RdexLkGrd::RdexLkGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

Seqlock::RdexLkGrd::~RdexLkGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock(&mSeqlock->mLock);
        }
    }
}

void Seqlock::RdexLkGrd::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock(&mSeqlock->mLock);
        }
    }
}

xwer_t Seqlock::RdexLkGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

xwer_t Seqlock::RdexLkGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

/* Seqlock::TryRdexLkGrd Non-static Member */
Seqlock::TryRdexLkGrd::TryRdexLkGrd(Seqlock * seqlock)
    : RdexLkGrd()
{
    mSeqlock = seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

Seqlock::TryRdexLkGrd::TryRdexLkGrd(Seqlock & seqlock)
    : RdexLkGrd()
{
    mSeqlock = &seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

/* Seqlock::RdexLkThGrd Non-static Member */
Seqlock::RdexLkThGrd::RdexLkThGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

Seqlock::RdexLkThGrd::RdexLkThGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

Seqlock::RdexLkThGrd::~RdexLkThGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock_cpuirqrs(&mSeqlock->mLock, mCpuirq);
        }
    }
}

void Seqlock::RdexLkThGrd::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock_cpuirqrs(&mSeqlock->mLock, mCpuirq);
        }
    }
}

xwer_t Seqlock::RdexLkThGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

xwer_t Seqlock::RdexLkThGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

/* Seqlock::TryRdexLkThGrd Non-static Member */
Seqlock::TryRdexLkThGrd::TryRdexLkThGrd(Seqlock * seqlock)
    : RdexLkThGrd()
{
    mSeqlock = seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

Seqlock::TryRdexLkThGrd::TryRdexLkThGrd(Seqlock & seqlock)
    : RdexLkThGrd()
{
    mSeqlock = &seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

/* Seqlock::RdexLkBhGrd Non-static Member */
Seqlock::RdexLkBhGrd::RdexLkBhGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock_bh(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

Seqlock::RdexLkBhGrd::RdexLkBhGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock_bh(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

Seqlock::RdexLkBhGrd::~RdexLkBhGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock_bh(&mSeqlock->mLock);
        }
    }
}

void Seqlock::RdexLkBhGrd::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock_bh(&mSeqlock->mLock);
        }
    }
}

xwer_t Seqlock::RdexLkBhGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

xwer_t Seqlock::RdexLkBhGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

/* Seqlock::TryRdexLkBhGrd Non-static Member */
Seqlock::TryRdexLkBhGrd::TryRdexLkBhGrd(Seqlock * seqlock)
    : RdexLkBhGrd()
{
    mSeqlock = seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock_bh(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

Seqlock::TryRdexLkBhGrd::TryRdexLkBhGrd(Seqlock & seqlock)
    : RdexLkBhGrd()
{
    mSeqlock = &seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock_bh(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

/* Seqlock::RdexLkIrqsGrd Non-static Member */
template<xwirq_t ... TIrqList>
Seqlock::RdexLkIrqsGrd<TIrqList ...>::RdexLkIrqsGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock_irqssv(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                   sizeof...(TIrqList));
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

template<xwirq_t ... TIrqList>
Seqlock::RdexLkIrqsGrd<TIrqList ...>::RdexLkIrqsGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_rdex_lock_irqssv(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                   sizeof...(TIrqList));
        mStatus = Seqlock::LockStatus::SeqlockRdexLocked;
    }
}

template<xwirq_t ... TIrqList>
Seqlock::RdexLkIrqsGrd<TIrqList ...>::~RdexLkIrqsGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock_irqsrs(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                         sizeof...(TIrqList));
        }
    }
}

template<xwirq_t ... TIrqList>
void Seqlock::RdexLkIrqsGrd<TIrqList ...>::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_rdex_unlock_irqsrs(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                         sizeof...(TIrqList));
        }
    }
}

template<xwirq_t ... TIrqList>
xwer_t Seqlock::RdexLkIrqsGrd<TIrqList ...>::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

template<xwirq_t ... TIrqList>
xwer_t Seqlock::RdexLkIrqsGrd<TIrqList ...>::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockRdexLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_RDEX, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_rdex_lock(&mSeqlock->mLock);
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

/* Seqlock::TryRdexLkIrqsGrd Non-static Member */
template<xwirq_t ... TIrqList>
Seqlock::TryRdexLkIrqsGrd<TIrqList ...>::TryRdexLkIrqsGrd(Seqlock * seqlock)
    : RdexLkIrqsGrd<TIrqList ...>()
{
    RdexLkIrqsGrd<TIrqList ...>::mSeqlock = seqlock;
    if (nullptr != RdexLkIrqsGrd<TIrqList ...>::mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock_irqssv(&RdexLkIrqsGrd<TIrqList ...>::mSeqlock->mLock,
                                            RdexLkIrqsGrd<TIrqList ...>::mIrqs,
                                            RdexLkIrqsGrd<TIrqList ...>::mIrqFlags,
                                            sizeof...(TIrqList));
        if (XWOK != mRc) {
            RdexLkIrqsGrd<TIrqList ...>::mSeqlock = nullptr;
        }
    }
}

template<xwirq_t ... TIrqList>
Seqlock::TryRdexLkIrqsGrd<TIrqList ...>::TryRdexLkIrqsGrd(Seqlock & seqlock)
    : RdexLkIrqsGrd<TIrqList ...>()
{
    RdexLkIrqsGrd<TIrqList ...>::mSeqlock = seqlock;
    if (nullptr != RdexLkIrqsGrd<TIrqList ...>::mSeqlock) {
        mRc = xwos_sqlk_rdex_trylock_irqssv(&RdexLkIrqsGrd<TIrqList ...>::mSeqlock->mLock,
                                            RdexLkIrqsGrd<TIrqList ...>::mIrqs,
                                            RdexLkIrqsGrd<TIrqList ...>::mIrqFlags,
                                            sizeof...(TIrqList));
        if (XWOK != mRc) {
            RdexLkIrqsGrd<TIrqList ...>::mSeqlock = nullptr;
        }
    }
}

/* Seqlock::WrLkGrd Non-static Member */
Seqlock::WrLkGrd::WrLkGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

Seqlock::WrLkGrd::WrLkGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

Seqlock::WrLkGrd::~WrLkGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock(&mSeqlock->mLock);
        }
    }
}

void Seqlock::WrLkGrd::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock(&mSeqlock->mLock);
        }
    }
}

xwer_t Seqlock::WrLkGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

xwer_t Seqlock::WrLkGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

/* Seqlock::TryWrLkGrd Non-static Member */
Seqlock::TryWrLkGrd::TryWrLkGrd(Seqlock * seqlock)
    : WrLkGrd()
{
    mSeqlock = seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_wr_trylock(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

Seqlock::TryWrLkGrd::TryWrLkGrd(Seqlock & seqlock)
    : WrLkGrd()
{
    mSeqlock = &seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_wr_trylock(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

/* Seqlock::WrLkThGrd Non-static Member */
Seqlock::WrLkThGrd::WrLkThGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

Seqlock::WrLkThGrd::WrLkThGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

Seqlock::WrLkThGrd::~WrLkThGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock_cpuirqrs(&mSeqlock->mLock, mCpuirq);
        }
    }
}

void Seqlock::WrLkThGrd::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock_cpuirqrs(&mSeqlock->mLock, mCpuirq);
        }
    }
}

xwer_t Seqlock::WrLkThGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

xwer_t Seqlock::WrLkThGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

/* Seqlock::TryWrLkThGrd Non-static Member */
Seqlock::TryWrLkThGrd::TryWrLkThGrd(Seqlock * seqlock)
    : WrLkThGrd()
{
    mSeqlock = seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_wr_trylock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

Seqlock::TryWrLkThGrd::TryWrLkThGrd(Seqlock & seqlock)
    : WrLkThGrd()
{
    mSeqlock = &seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_wr_trylock_cpuirqsv(&mSeqlock->mLock, &mCpuirq);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

/* Seqlock::WrLkBhGrd Non-static Member */
Seqlock::WrLkBhGrd::WrLkBhGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock_bh(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

Seqlock::WrLkBhGrd::WrLkBhGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock_bh(&mSeqlock->mLock);
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

Seqlock::WrLkBhGrd::~WrLkBhGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock_bh(&mSeqlock->mLock);
        }
    }
}

void Seqlock::WrLkBhGrd::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock_bh(&mSeqlock->mLock);
        }
    }
}

xwer_t Seqlock::WrLkBhGrd::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

xwer_t Seqlock::WrLkBhGrd::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

/* Seqlock::TryWrLkBhGrd Non-static Member */
Seqlock::TryWrLkBhGrd::TryWrLkBhGrd(Seqlock * seqlock)
    : WrLkBhGrd()
{
    mSeqlock = seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_wr_trylock_bh(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

Seqlock::TryWrLkBhGrd::TryWrLkBhGrd(Seqlock & seqlock)
    : WrLkBhGrd()
{
    mSeqlock = &seqlock;
    if (nullptr != mSeqlock) {
        mRc = xwos_sqlk_wr_trylock_bh(&mSeqlock->mLock);
        if (XWOK != mRc) {
            mSeqlock = nullptr;
        }
    }
}

/* Seqlock::WrLkIrqsGrd Non-static Member */
template<xwirq_t ... TIrqList>
Seqlock::WrLkIrqsGrd<TIrqList ...>::WrLkIrqsGrd(Seqlock * seqlock)
    : mSeqlock(seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock_irqssv(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                 sizeof...(TIrqList));
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

template<xwirq_t ... TIrqList>
Seqlock::WrLkIrqsGrd<TIrqList ...>::WrLkIrqsGrd(Seqlock & seqlock)
    : mSeqlock(&seqlock)
    , mStatus(Seqlock::LockStatus::SeqlockUnlocked)
    , mIrqs{TIrqList ...}
{
    if (nullptr != mSeqlock) {
        xwos_sqlk_wr_lock_irqssv(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                 sizeof...(TIrqList));
        mStatus = Seqlock::LockStatus::SeqlockWrLocked;
    }
}

template<xwirq_t ... TIrqList>
Seqlock::WrLkIrqsGrd<TIrqList ...>::~WrLkIrqsGrd()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock_irqsrs(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                       sizeof...(TIrqList));
        }
    }
}

template<xwirq_t ... TIrqList>
void Seqlock::WrLkIrqsGrd<TIrqList ...>::unlock()
{
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            mStatus = Seqlock::LockStatus::SeqlockUnlocked;
            xwos_sqlk_wr_unlock_irqsrs(&mSeqlock->mLock, mIrqs, mIrqFlags,
                                       sizeof...(TIrqList));
        }
    }
}

template<xwirq_t ... TIrqList>
xwer_t Seqlock::WrLkIrqsGrd<TIrqList ...>::wait(sync::Cond * cond)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

template<xwirq_t ... TIrqList>
xwer_t Seqlock::WrLkIrqsGrd<TIrqList ...>::wait(sync::Cond * cond, xwtm_t to)
{
    xwer_t rc;
    if (nullptr != mSeqlock) {
        if (Seqlock::LockStatus::SeqlockWrLocked == mStatus) {
            union xwos_ulock lock;
            xwsq_t lkst;
            lock.osal.sqlk = &mSeqlock->mLock;
            rc = xwos_cond_wait_to(cond->getXwosObj(), lock, XWOS_LK_SQLK_WR, nullptr,
                                   to, &lkst);
            if (rc < 0) {
                if (XWOS_LKST_UNLOCKED == lkst) {
                    xwos_sqlk_wr_lock(&mSeqlock->mLock);
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

/* Seqlock::TryWrLkIrqsGrd Non-static Member */
template<xwirq_t ... TIrqList>
Seqlock::TryWrLkIrqsGrd<TIrqList ...>::TryWrLkIrqsGrd(Seqlock * seqlock)
    : WrLkIrqsGrd<TIrqList ...>()
{
    WrLkIrqsGrd<TIrqList ...>::mSeqlock = seqlock;
    if (nullptr != WrLkIrqsGrd<TIrqList ...>::mSeqlock) {
        mRc = xwos_sqlk_wr_trylock_irqssv(&WrLkIrqsGrd<TIrqList ...>::mSeqlock->mLock,
                                          WrLkIrqsGrd<TIrqList ...>::mIrqs,
                                          WrLkIrqsGrd<TIrqList ...>::mIrqFlags,
                                          sizeof...(TIrqList));
        if (XWOK != mRc) {
            WrLkIrqsGrd<TIrqList ...>::mSeqlock = nullptr;
        }
    }
}

template<xwirq_t ... TIrqList>
Seqlock::TryWrLkIrqsGrd<TIrqList ...>::TryWrLkIrqsGrd(Seqlock & seqlock)
    : WrLkIrqsGrd<TIrqList ...>()
{
    WrLkIrqsGrd<TIrqList ...>::mSeqlock = seqlock;
    if (nullptr != WrLkIrqsGrd<TIrqList ...>::mSeqlock) {
        mRc = xwos_sqlk_wr_trylock_irqssv(&WrLkIrqsGrd<TIrqList ...>::mSeqlock->mLock,
                                          WrLkIrqsGrd<TIrqList ...>::mIrqs,
                                          WrLkIrqsGrd<TIrqList ...>::mIrqFlags,
                                          sizeof...(TIrqList));
        if (XWOK != mRc) {
            WrLkIrqsGrd<TIrqList ...>::mSeqlock = nullptr;
        }
    }
}

/* Seqlock Non-static Member */
Seqlock::Seqlock()
{
    xwos_sqlk_init(&mLock);
}

Seqlock::~Seqlock()
{
}

xwsq_t Seqlock::readBegin()
{
    return xwos_sqlk_rd_begin(&mLock);
}

bool Seqlock::readRetry(xwsq_t start)
{
    return xwos_sqlk_rd_retry(&mLock, start);
}

xwsq_t Seqlock::getSeq()
{
    return xwos_sqlk_get_seq(&mLock);
}

} // namespace xwos
} // namespace lock
