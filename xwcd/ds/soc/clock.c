/**
 * @file
 * @brief 玄武设备栈：SOC：时钟
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

#include <xwcd/ds/standard.h>
#include <string.h>
#include <xwcd/ds/soc/clock.h>

__xwds_api
xwer_t xwds_clk_req(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->clk.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (rc < 0) {
                goto err_soc_grab;
        }

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_req)) {
                rc = drv->clk_req(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
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

        drv = xwds_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_rls)) {
                rc = drv->clk_rls(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
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
        if (rc < 0) {
                goto err_soc_grab;
        }

        drv = xwds_cast(struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->clk_getfqcy)) {
                rc = drv->clk_getfqcy(soc, id, buf, num);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_getfqcy;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_getfqcy:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
