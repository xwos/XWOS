/**
 * @file
 * @brief 位图原子操作汇编库: xwbmpaop_flz_then_s1i
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <armv7m_core.h>

__xwlib_code
xwssq_t xwbmpaop_flz_then_s1i(xwbmp_a * bmp, xwsz_t num)
{
        xwsz_t total = BITS_TO_XWBMP_T(num);
        xwsq_t i;
        xwbmp_t msk;
        xwbmp_t tmp, ov, nv, m;
        xwssq_t pos;
        xwer_t rc;

        do {
                i = total;
                pos = -ENODATA;
                msk = ((xwbmp_t)XWBOP_BIT(num % BITS_PER_XWBMP_T) - (xwbmp_t)1);
                if ((xwbmp_t)0 == msk) {
                        msk = ~(xwbmp_t)0;
                }
                do {
                        i--;
                        ov = cm_ldrex((xwu32_t *)&bmp[i]);
                        tmp = (xwbmp_t)(~ov) & msk;
                        if (tmp) {
                                pos = xwbop_flz(xwbmp_t, tmp);
                                break;
                        }
                        msk = ~(xwbmp_t)0;
                } while (i > 0);
                if (pos < 0) {
                        pos = -ENODATA;
                        xwmb_mp_ddb();
                        break;
                } else {
                        m = (1U << (xwsq_t)pos);
                        nv = ov | m;
                        pos += (xwssq_t)(i * BITS_PER_XWBMP_T);
                        xwmb_mp_mb();
                        rc = cm_strex((xwu32_t *)&bmp[i], nv);
                }
        } while (rc);
        return pos;
}
