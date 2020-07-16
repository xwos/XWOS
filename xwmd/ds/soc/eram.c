/**
 * @file
 * @brief xwmd设备栈：SOC External RAM
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/soc/eram.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_api
xwer_t xwds_eram_test(struct xwds_soc * soc, xwptr_t * erraddr)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(erraddr, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
        rc = xwds_soc_request(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_request;
        }

        drv = xwds_static_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eram_tst)) {
                rc = drv->eram_tst(soc, erraddr);
                if (__unlikely(rc < 0)) {
                        rc = EFAULT;
                }
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_tst;
        }

        xwds_soc_release(soc);
        xwds_soc_put(soc);
        return XWOK;

err_drv_tst:
        xwds_soc_release(soc);
err_soc_request:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
