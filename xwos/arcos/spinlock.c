/**
 * @file
 * @brief xwos::autosar::cp::os::spinlock
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/arcos/spinlock.h>

xwer_t xwarcos_spinlock_get(struct xwarcos_spinlock * splk)
{
        xwer_t rc;

        if (NULL == splk) {
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        rc = E_XWARCOS_OK;
        xwos_splk_lock_bh(&splk->splk);

err_id:
        return rc;
}

xwer_t xwarcos_spinlock_try_to_get(struct xwarcos_spinlock * splk,
                                   enum xwarcos_try_to_get_spinlock_status_em * status)
{
        xwer_t rc;

        if (NULL == splk) {
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        if (NULL == status) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        rc = xwos_splk_trylock_bh(&splk->splk);
        if (rc < 0) {
                *status = XWARCOS_TRYTOGETSPINLOCK_NOSUCCESS;
        } else {
                *status = XWARCOS_TRYTOGETSPINLOCK_SUCCESS;
        }
        rc = E_XWARCOS_OK;

err_nullptr:
err_id:
        return rc;
}

xwer_t xwarcos_spinlock_release(struct xwarcos_spinlock * splk)
{
        xwer_t rc;

        if (NULL == splk) {
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        rc = E_XWARCOS_OK;
        xwos_splk_unlock_bh(&splk->splk);

err_id:
        return rc;
}
