/**
 * @file
 * @brief 原子操作库：xwaop__xwu32_t__tst_then_op
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
#include <arch_irq.h>
#include <xwos/lib/xwaop32.h>

__xwlib_code
xwer_t xwaop__xwu32_t__tst_then_op(__xwcc_atomic xwu32_t * a,
                                   xwaop_tst_f tst, void * tst_args,
                                   xwaop_op_f op, void * op_args,
                                   xwu32_t * nv, xwu32_t * ov)
{
        xwu32_t n;
        xwu32_t o;
        xwer_t rc;
        xwreg_t flag;

        arch_cpuirq_save_lc(&flag);
        o = *a;
        if (tst) {
                if (tst((const void *)&o, tst_args)) {
                        if (op) {
                                op(&n, (const void *)&o, op_args);
                                *a = n;
                        } else {
                                n = o;
                        }
                        rc = XWOK;
                } else {
                        rc = -EACCES;
                        n = o;
                }
        } else {
                if (op) {
                        op(&n, (const void *)&o, op_args);
                        *a = n;
                } else {
                        n = o;
                }
                rc = XWOK;
        }
        arch_cpuirq_restore_lc(flag);
        if (nv) {
                *nv = n;
        }
        if (ov) {
                *ov = o;
        }
        return rc;
}
