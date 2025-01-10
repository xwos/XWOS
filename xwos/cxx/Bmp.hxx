/**
 * @file
 * @brief xwos::Bmp
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_Bmp_hxx__
#define __xwos_cxx_Bmp_hxx__

extern "C" {
#include <xwos/lib/xwbop.h>
}

namespace xwos {

/**
 * @defgroup xwos_cxx_Bmp 位图库
 * @ingroup xwos_cxx
 *
 * XWOS位图库
 *
 * ## 头文件
 *
 * @ref xwos/cxx/Bmp.hxx
 *
 * @{
 */

template<xwsz_t TNum>
class Bmp
{
  public:
    class Bit
    {
      private:
        Bmp * mBmp;
        xwsq_t mBit;

      public:
        explicit Bit(Bmp & bmp, xwsq_t index)
            : mBmp(&bmp)
            , mBit(index)
        {
        }
        explicit Bit(Bmp * bmp, xwsq_t index)
            : mBmp(bmp)
            , mBit(index)
        {
        }
        explicit Bit(const Bit & other)
            : mBit(other.mBmp, other.index)
        {
        }
        explicit Bit(const Bit && other)
            : mBit(other.mBmp, other.index)
        {
        }
        ~Bit()
        {
        }
        Bit & operator=(bool x)
        {
            if (x) {
                mBmp->set(mBit);
            } else {
                mBmp->clear(mBit);
            }
            return *this;
        }
        Bit & operator=(const Bit & other)
        {
            if (this != &other) {
                mBmp = other.mBmp;
                mBit = other.mBit;
            }
            return *this;
        }
        Bit & operator=(const Bit && other)
        {
            if (this != &other) {
                mBmp = other.mBmp;
                mBit = other.mBit;
            }
            return *this;
        }
        Bit & operator~()
        {
            mBmp->flip(mBit);
            return *this;
        }
        explicit operator bool()
        {
            bool ret;
            if ((xwsq_t)mBit < TNum) {
                ret = xwbmpop_t1i(mBmp->mData, mBit);
            } else {
                ret = false;
            }
            return ret;
        }
        void flip() { mBmp->flip(mBit); }
    };

  public:
    xwbmp_t mData[BITS_TO_XWBMP_T(TNum)];

  public:
    Bmp(const Bmp & other)
    {
        xwbmpop_assign(mData, other.mData, TNum);
    }
    Bmp(const Bmp && other)
    {
        xwbmpop_assign(mData, other.mData, TNum);
    }
    Bmp()
    {
        xwbmpop_c0all(mData, TNum);
    }
    ~Bmp()
    {
    }
    void set()
    {
        xwbmpop_s1all(mData, TNum);
    }
    void set(int pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_s1i(mData, (xwsq_t)pos);
        }
    }
    void set(unsigned int pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_s1i(mData, (xwsq_t)pos);
        }
    }
    void set(long pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_s1i(mData, (xwsq_t)pos);
        }
    }
    void set(unsigned long pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_s1i(mData, (xwsq_t)pos);
        }
    }
    void set(Bmp<TNum> * msk)
    {
        xwbmpop_s1m(mData, msk->mData, TNum);
    }
    void set(Bmp<TNum> & msk)
    {
        xwbmpop_s1m(mData, msk.mData, TNum);
    }
    void clear()
    {
        xwbmpop_c0all(mData, TNum);
    }
    void clear(int pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_c0i(mData, (xwsq_t)pos);
        }
    }
    void clear(unsigned int pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_c0i(mData, (xwsq_t)pos);
        }
    }
    void clear(long pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_c0i(mData, (xwsq_t)pos);
        }
    }
    void clear(unsigned long pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_c0i(mData, (xwsq_t)pos);
        }
    }
    void clear(Bmp<TNum> * msk)
    {
        xwbmpop_c0m(mData, msk->mData, TNum);
    }
    void clear(Bmp<TNum> & msk)
    {
        xwbmpop_c0m(mData, msk.mData, TNum);
    }
    void flip()
    {
        xwbmpop_not(mData, TNum);
    }
    void flip(int pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_x1i(mData, (xwsq_t)pos);
        }
    }
    void flip(unsigned int pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_x1i(mData, (xwsq_t)pos);
        }
    }
    void flip(long pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_x1i(mData, (xwsq_t)pos);
        }
    }
    void flip(unsigned long pos)
    {
        if ((xwsq_t)pos < TNum) {
            xwbmpop_x1i(mData, (xwsq_t)pos);
        }
    }
    void flip(Bmp<TNum> * msk)
    {
        xwbmpop_x1m(mData, msk->mData, TNum);
    }
    void flip(Bmp<TNum> & msk)
    {
        xwbmpop_x1m(mData, msk.mData, TNum);
    }
    xwssq_t ffs()
    {
        return xwbmpop_ffs(mData, TNum);
    }
    xwssq_t fls()
    {
        return xwbmpop_fls(mData, TNum);
    }
    xwssq_t ffz()
    {
        return xwbmpop_ffz(mData, TNum);
    }
    xwssq_t flz()
    {
        return xwbmpop_flz(mData, TNum);
    }
    xwsz_t weight()
    {
        return xwbmpop_weight(mData, TNum);
    }

    bool all()
    {
        return xwbmpop_t1ma(mData);
    }
    bool any()
    {
        return xwbmpop_t1mo(mData);
    }
    bool none()
    {
        return xwbmpop_t0ma(mData);
    }

    Bmp<TNum> & operator=(const Bmp<TNum> & other)
    {
        if (this != &other) {
            xwbmpop_assign(mData, other.mData, TNum);
        }
        return *this;
    }
    Bmp<TNum> & operator=(const Bmp<TNum> && other)
    {
        if (this != &other) {
            xwbmpop_assign(mData, other.mData, TNum);
        }
        return *this;
    }
    Bmp<TNum> operator&(const Bmp<TNum> & other)
    {
        Bmp tmp(*this);
        xwbmpop_and(tmp.mData, other.mData, TNum);
        return tmp;
    }
    Bmp<TNum> operator&(const Bmp<TNum> && other)
    {
        Bmp tmp(*this);
        xwbmpop_and(tmp.mData, other.mData, TNum);
        return tmp;
    }
    Bmp<TNum> operator|(const Bmp<TNum> & other)
    {
        Bmp tmp(*this);
        xwbmpop_or(tmp.mData, other.mData, TNum);
        return tmp;
    }
    Bmp<TNum> operator|(const Bmp<TNum> && other)
    {
        Bmp tmp(*this);
        xwbmpop_or(tmp.mData, other.mData, TNum);
        return tmp;
    }
    Bmp<TNum> operator^(const Bmp<TNum> & other)
    {
        Bmp tmp(*this);
        xwbmpop_xor(tmp.mData, other.mData, TNum);
        return tmp;
    }
    Bmp<TNum> operator^(const Bmp<TNum> && other)
    {
        Bmp tmp(*this);
        xwbmpop_xor(tmp.mData, other.mData, TNum);
        return tmp;
    }
    Bmp<TNum> & operator&=(const Bmp<TNum> & other)
    {
        xwbmpop_and(mData, other.mData, TNum);
        return *this;
    }
    Bmp<TNum> & operator&=(const Bmp<TNum> && other)
    {
        xwbmpop_and(mData, other.mData, TNum);
        return *this;
    }
    Bmp<TNum> & operator|=(const Bmp<TNum> & other)
    {
        xwbmpop_or(mData, other.mData, TNum);
        return *this;
    }
    Bmp<TNum> & operator|=(const Bmp<TNum> && other)
    {
        xwbmpop_or(mData, other.mData, TNum);
        return *this;
    }
    Bmp<TNum> & operator^=(const Bmp<TNum> & other)
    {
        xwbmpop_xor(mData, other.mData, TNum);
        return *this;
    }
    Bmp<TNum> & operator^=(const Bmp<TNum> && other)
    {
        xwbmpop_xor(mData, other.mData, TNum);
        return *this;
    }
    Bmp<TNum> & operator~()
    {
        xwbmpop_not(mData, TNum);
        return *this;
    }
    bool operator==(const Bmp<TNum> & other)
    {
        return (0 == xwbmpop_cmp(mData, other.mData, TNum));
    }
    bool operator==(const Bmp<TNum> && other)
    {
        return (0 == xwbmpop_cmp(mData, other.mData, TNum));
    }
    bool operator!=(const Bmp<TNum> & other)
    {
        return (0 != xwbmpop_cmp(mData, other.mData, TNum));
    }
    bool operator!=(const Bmp<TNum> && other)
    {
        return (0 != xwbmpop_cmp(mData, other.mData, TNum));
    }
    bool operator>(const Bmp<TNum> & other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) > 0);
    }
    bool operator>(const Bmp<TNum> && other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) > 0);
    }
    bool operator<(const Bmp<TNum> & other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) < 0);
    }
    bool operator<(const Bmp<TNum> && other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) < 0);
    }
    bool operator>=(const Bmp<TNum> & other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) >= 0);
    }
    bool operator>=(const Bmp<TNum> && other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) >= 0);
    }
    bool operator<=(const Bmp<TNum> & other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) <= 0);
    }
    bool operator<=(const Bmp<TNum> && other)
    {
        return (xwbmpop_cmp(mData, other.mData, TNum) <= 0);
    }
    explicit operator bool()
    {
        return any();
    }
    Bit operator[](xwsq_t pos)
    {
        return Bit(this, pos);
    }
};

/**
 * @} xwos_cxx_Bmp
 */

} // namespace xwos

#endif /* xwos/cxx/Bmp.hxx */
