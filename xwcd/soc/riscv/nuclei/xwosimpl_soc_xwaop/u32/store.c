/**
 * @file
 * @brief 原子操作库: xwaop__xwu32_t__load
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#include <riscv_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xwu32_t xwaop__xwu32_t__store(atomic_xwu32_t * a,
                              const enum xwaop_memory_order_em mo,
                              xwu32_t v)
{
        switch (mo) {
        case xwaop_mo_relaxed:
                *a = v;
                break;
        case xwaop_mo_consume:
                *a = v;
                break;
        case xwaop_mo_acquire:
                *a = v;
                break;
        case xwaop_mo_release:
                xwmb_mp_mb();
                *a = v;
                break;
        case xwaop_mo_acq_rel:
                xwmb_mp_mb();
                *a = v;
                break;
        case xwaop_mo_seq_cst:
                xwmb_mp_mb();
                *a = v;
                xwmb_mp_mb();
                break;
        default:
                *a = v;
                break;
        }
        return v;
}
