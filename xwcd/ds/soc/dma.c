/**
 * @file
 * @brief 玄武设备栈：SOC：DMA
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
#include <xwos/lib/xwaop.h>
#include <xwcd/ds/soc/dma.h>

__xwds_api
xwer_t xwds_dma_req(struct xwds_soc * soc, xwid_t ch)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (rc < 0) {
                goto err_soc_grab;
        }
        rc = xwbmpaop_t0i_then_s1i(soc->dma.chstatus, ch);
        if (rc < 0) {
                rc = -EBUSY;
                goto err_busy;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_req)) {
                rc = drv->dma_req(soc, ch);
                if (rc < 0) {
                        goto err_drv_dma_req;
                }
        }
        return XWOK;

err_drv_dma_req:
        xwbmpaop_s1i(soc->dma.chstatus, ch);
err_busy:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_dma_rls(struct xwds_soc * soc, xwid_t ch)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        if (!xwbmpaop_t1i(soc->dma.chstatus, ch)) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_rls)) {
                rc = drv->dma_rls(soc, ch);
                if (rc < 0) {
                        goto err_drv_dma_rls;
                }
        }
        xwbmpaop_c0i(soc->dma.chstatus, ch);
        xwds_soc_put(soc);
        return XWOK;

err_drv_dma_rls:
err_notreq:
        return rc;
}

__xwds_api
xwer_t xwds_dma_cfg(struct xwds_soc * soc, xwid_t ch, void * cfg,
                    xwds_dma_f cb, xwds_dma_cbarg_t arg)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        if (!xwbmpaop_t1i(soc->dma.chstatus, ch)) {
                rc = -EPERM;
                goto err_notreq;
        }
#if defined(XWCDCFG_ds_SOC_DMA_ROCBT) && (1 == XWCDCFG_ds_SOC_DMA_ROCBT)
        XWOS_UNUSED(cb);
        XWOS_UNUSED(arg);
#else
        if (soc->dma.chcbs) {
                soc->dma.chcbs[ch] = cb;
        }
        if (soc->dma.chcbargs) {
                soc->dma.chcbargs[ch] = arg;
        }
#endif
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_cfg)) {
                rc = drv->dma_cfg(soc, ch, cfg);
                if (rc < 0) {
                        goto err_drv_dma_cfg;
                }
        }
        return XWOK;

err_drv_dma_cfg:
#if !defined(XWCDCFG_ds_SOC_DMA_ROCBT) || (1 != XWCDCFG_ds_SOC_DMA_ROCBT)
        if (soc->dma.chcbs) {
                soc->dma.chcbs[ch] = NULL;
        }
        if (soc->dma.chcbargs) {
                soc->dma.chcbargs[ch] = NULL;
        }
#endif
err_notreq:
        return rc;
}

__xwds_api
xwer_t xwds_dma_enable(struct xwds_soc * soc, xwid_t ch)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        if (!xwbmpaop_t1i(soc->dma.chstatus, ch)) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_enable)) {
                rc = drv->dma_enable(soc, ch);
                if (rc < 0) {
                        goto err_drv_dma_enable;
                }
        }
        return XWOK;

err_drv_dma_enable:
err_notreq:
        return rc;
}

__xwds_api
xwer_t xwds_dma_disable(struct xwds_soc * soc, xwid_t ch)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        if (!xwbmpaop_t1i(soc->dma.chstatus, ch)) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_disable)) {
                rc = drv->dma_disable(soc, ch);
                if (rc < 0) {
                        goto err_drv_dma_disable;
                }
        }
        return XWOK;

err_drv_dma_disable:
err_notreq:
        return rc;
}

__xwds_api
xwer_t xwds_dma_start(struct xwds_soc * soc, xwid_t ch)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        if (!xwbmpaop_t1i(soc->dma.chstatus, ch)) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_start)) {
                rc = drv->dma_start(soc, ch);
                if (rc < 0) {
                        goto err_drv_dma_start;
                }
        }
        return XWOK;

err_drv_dma_start:
err_notreq:
        return rc;
}

__xwds_api
xwer_t xwds_dma_stop(struct xwds_soc * soc, xwid_t ch)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        if (!xwbmpaop_t1i(soc->dma.chstatus, ch)) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_stop)) {
                rc = drv->dma_stop(soc, ch);
                if (rc < 0) {
                        goto err_drv_dma_stop;
                }
        }
        return XWOK;

err_drv_dma_stop:
err_notreq:
        return rc;
}
