/**
 * @file
 * @brief 原子操作库: xwaop__xws32_t__teq_then_write
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
#include <armv7m_isa.h>
#include <xwos/lib/xwaop.h>

__xwlib_code
xwer_t xwaop__xws32_t__teq_then_write(atomic_xws32_t * a,
                                      xws32_t t,
                                      xws32_t v,
                                      xws32_t * ov)
{
        xws32_t o;
        xwer_t rc;

        do {
                o = (xws32_t)cm_ldrex(a);
                if (o == t) {
                        xwmb_mp_mb();
                        rc = cm_strex(a, (xwu32_t)v);
                } else {
                        rc = -EACCES;
                        xwmb_ddb();
                        break;
                }
        } while (rc);
        if (ov) {
                *ov = o;
        }
        return rc;
}
