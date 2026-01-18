/**
 * @file
 * @brief XWOS内核的初始化
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#if defined(XWCFG_CORE__mp)
#  include <xwos/mp/init.h>
#elif defined(XWCFG_CORE__up)
#  include <xwos/up/init.h>
#else
#  error "Can't find the configuration XWCFG_CORE!"
#endif
#include <xwos/lib/object.h>
#include <xwos/osal/skd.h>
#include <xwos/init.h>

__xwos_init_code
void xwos_init(void)
{
        xwid_t cpuid;

        cpuid = xwos_skd_get_cpuid_lc();
        if ((xwid_t)0 == cpuid) {
                xwos_objtik_init();
        }
#if defined(XWCFG_CORE__mp)
        xwmp_init(); // cppcheck-suppress [misra-c2012-17.7]
#elif defined(XWCFG_CORE__up)
        xwup_init(); // cppcheck-suppress [misra-c2012-17.7]
#endif
}
