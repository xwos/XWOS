/**
 * @file
 * @brief 原子操作库: xwaop__xwu8__tst_then_op
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
#include <armv7m_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xwer_t xwaop__xwu8__tst_then_op(xwu8_a * a,
                                xwaop_tst_f tst, void * tst_args,
                                xwaop_op_f op, void * op_args,
                                xwu8_t * nv, xwu8_t * ov)
{
        xwu8_t o;
        xwu8_t n;
        xwer_t rc;

        do {
                o = (xwu8_t)cm_ldrexb(a);
                if (tst) {
                        rc = tst((const void *)&o, tst_args);
                        if (XWOK == rc) {
                                if (op) {
                                        op(&n, (const void *)&o, op_args);
                                        xwmb_mp_mb();
                                        rc = cm_strexb(a, (xwu8_t)n);
                                } else {
                                        n = o;
                                        xwmb_mp_mb();
                                        break;
                                }
                        } else {
                                n = o;
                                xwmb_mp_ddb();
                                break;
                        }
                } else {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                xwmb_mp_mb();
                                rc = cm_strexb(a, (xwu8_t)n);
                        } else {
                                rc = XWOK;
                                n = o;
                                xwmb_mp_mb();
                                break;
                        }
                }
        } while (rc);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}