/**
 * @file
 * @brief 玄武设备栈：SOC：电源
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <xwcd/ds/soc/power.h>

__xwds_api
xwer_t xwds_pwr_req(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->pwr.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (rc < 0) {
                goto err_soc_grab;
        }

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->pwr_req)) {
                rc = drv->pwr_req(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_pwr_req;
        }
        return XWOK;

err_drv_pwr_req:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_pwr_rls(struct xwds_soc * soc, xwid_t id)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->pwr.num), "out-of-range", -ERANGE);

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->pwr_rls)) {
                rc = drv->pwr_rls(soc, id);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_pwr_rls;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_pwr_rls:
        return rc;
}

__xwds_api
xwer_t xwds_pwr_getvltg(struct xwds_soc * soc, xwid_t id,
                        xwu32_t * buf, xwsz_t * num)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((id < soc->pwr.num), "out-of-range", -ERANGE);
        XWDS_VALIDATE(buf, "nullptr", -EFAULT);
        XWDS_VALIDATE(num, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (rc < 0) {
                goto err_soc_grab;
        }

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->pwr_getvltg)) {
                rc = drv->pwr_getvltg(soc, id, buf, num);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_getvltg;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_getvltg:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
