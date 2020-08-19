/**
 * @file
 * @brief xwmd设备栈：SOC时钟
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
#include <string.h>
#include <xwmd/ds/soc/clock.h>

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
xwer_t xwds_clk_req(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_req)) {
                rc = drv->clk_req(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_clk_req;
        }
        return XWOK;

err_drv_clk_req:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_clk_rls(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);

        drv = xwds_static_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_rls)) {
                rc = drv->clk_rls(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_clk_rls;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_clk_rls:
        return rc;
}

__xwds_api
xwer_t xwds_clk_getfqcy(struct xwds_soc * soc, xwid_t id,
                        xwu32_t * buf, xwsz_t * num)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(num, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        drv = xwds_static_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_getfqcy)) {
                rc = drv->clk_getfqcy(soc, id, buf, num);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_getfqcy;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_getfqcy:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_get_clkrsc(const struct xwds_resource_clk base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_clk ** ret)
{
        xwer_t rc;
        xwsz_t i, m;

        for (i = 0; i < num; i++) {
                rc = XWOK;
                for (m = 0; m < descnum; m++) {
                        if (!strstr(base[i].description, descay[m])) {
                                rc = -ENOSR;
                                break;
                        }
                }
                if (XWOK == rc) {
                        break;
                }
        }
        if (XWOK == rc) {
                *ret = &base[i];
        } else {
                *ret = NULL;
        }
        return rc;
}
