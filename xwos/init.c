/**
 * @file
 * @brief XWOS内核的初始化
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#if defined(XWOS_CFG_CORE__mp)
#  include <xwos/mp/init.h>
#elif defined(XWOS_CFG_CORE__up)
#  include <xwos/up/init.h>
#else
#  error "Can't find the configuration XWOS_CFG_CORE!"
#endif
#include <xwos/lib/object.h>
#include <xwos/osal/skd.h>
#include <xwos/init.h>

__xwos_init_code
void xwos_init(void)
{
        xwid_t cpuid;

        cpuid = xwos_skd_id_lc();
        if (0 == cpuid) {
                xwos_objtik_init();
        }
#if defined(XWOS_CFG_CORE__mp)
        xwmp_init();
#elif defined(XWOS_CFG_CORE__up)
        xwup_init();
#endif
}
