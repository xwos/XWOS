/**
 * @file
 * @brief 玄武设备栈：SOC：外部中断
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
#include <xwcd/ds/soc/gpio.h>
#include <xwcd/ds/soc/eirq.h>

__xwds_api
xwer_t xwds_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid, xwsq_t eiflag,
                     xwds_eirq_f isr, xwds_eirq_arg_t arg)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((eiid < soc->eirq.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (rc < 0) {
                goto err_soc_grab;
        }

#if defined(XWCDCFG_ds_SOC_EIRQ_ROISRT) && (1 == XWCDCFG_ds_SOC_EIRQ_ROISRT)
        XWOS_UNUSED(isr);
        XWOS_UNUSED(arg);
        if (NULL == soc->eirq.isrs[eiid]) {
                rc = -EPERM;
                goto err_perm;
        }
#else
        if (soc->eirq.isrs[eiid]) {
                rc = -EBUSY;
                goto err_perm;
        }
        soc->eirq.isrs[eiid] = isr ? isr : (xwds_eirq_f)err_ptr(-EBUSY);
        soc->eirq.isrargs[eiid] = arg;
#endif

        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eirq_req)) {
                rc = drv->eirq_req(soc, port, pinmask, eiid, eiflag);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_eirq_req;
        }
        return XWOK;

err_drv_eirq_req:
#if !defined(XWCDCFG_ds_SOC_EIRQ_ROISRT) || (1 != XWCDCFG_ds_SOC_EIRQ_ROISRT)
        soc->eirq.isrs[eiid] = NULL;
        soc->eirq.isrargs[eiid] = NULL;
#endif
err_perm:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask, xwid_t eiid)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE((eiid < soc->eirq.num), "out-of-range", -ERANGE);

        if (NULL == soc->eirq.isrs[eiid]) {
                rc = -EPERM;
                goto err_notinused;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eirq_rls)) {
                rc = drv->eirq_rls(soc, port, pinmask, eiid);
        } else {
                rc = -ENOSYS;
        }
        if (rc < 0) {
                goto err_drv_rlsei;
        }
#if !defined(XWCDCFG_ds_SOC_EIRQ_ROISRT) || (1 != XWCDCFG_ds_SOC_EIRQ_ROISRT)
        soc->eirq.isrs[eiid] = NULL;
        soc->eirq.isrargs[eiid] = NULL;
#endif

        xwds_soc_put(soc);
        return XWOK;

err_drv_rlsei:
err_notinused:
        return rc;
}
