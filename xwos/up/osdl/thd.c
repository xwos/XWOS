/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：线程
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/up/osdl/thd.h>

__xwup_code
xwer_t xwosdl_thd_stop(struct xwosdl_thd * thd, xwer_t * trc)
{
        xwer_t rc;

        xwup_thd_quit(thd);
        rc = xwup_thd_join(thd, trc);
        return rc;
}
