/**
 * @file
 * @brief xwos::autosar::cp::os::resource
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/arcos/resource.h>

struct xwarcos_resource xwarcos_res_scheduler = XWARCOS_RESOURCE_INITIALIZER;

xwer_t xwarcos_resource_get(struct xwarcos_resource * res)
{
        xwer_t rc;

        if (NULL == res) {
                rc = -E_XWARCOS_ID;
                goto err_nullptr;
        }
        rc = E_XWARCOS_OK;
        xwos_splk_lock_bh(&res->splk);

err_nullptr:
        return rc;
}

xwer_t xwarcos_resource_release(struct xwarcos_resource * res)
{
        xwer_t rc;

        if (NULL == res) {
                rc = -E_XWARCOS_ID;
                goto err_nullptr;
        }
        rc = E_XWARCOS_OK;
        xwos_splk_unlock_bh(&res->splk);

err_nullptr:
        return rc;
}
