/**
 * @file
 * @brief xwmd设备栈：SOC DMA
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
#include <xwos/lib/string.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwmd/ds/soc/dma.h>

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
xwer_t xwds_dma_req(struct xwds_soc * soc, xwid_t ch)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(((xwid_t)ch < soc->dma.ch_num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        rc = xwbmpaop_t0i_then_s1i(soc->dma.chstatus, ch);
        if (__unlikely(rc < 0)) {
                rc = -EBUSY;
                goto err_busy;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_req)) {
                rc = drv->dma_req(soc, ch);
                if (__unlikely(rc < 0)) {
                        goto err_drv_dma_req;
                }
        }
        return OK;

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

        if (__unlikely(!xwbmpaop_t1i(soc->dma.chstatus, ch))) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_rls)) {
                rc = drv->dma_rls(soc, ch);
                if (__unlikely(rc < 0)) {
                        goto err_drv_dma_rls;
                }
        }
        xwbmpaop_c0i(soc->dma.chstatus, ch);
        xwds_soc_put(soc);
        return OK;

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

        if (__unlikely(!xwbmpaop_t1i(soc->dma.chstatus, ch))) {
                rc = -EPERM;
                goto err_notreq;
        }

#if defined(XWMDCFG_ds_SOC_DMA_ROCBT) && (1 == XWMDCFG_ds_SOC_DMA_ROCBT)
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

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_cfg)) {
                rc = drv->dma_cfg(soc, ch, cfg);
                if (__unlikely(rc < 0)) {
                        goto err_drv_dma_cfg;
                }
        }
        return OK;

err_drv_dma_cfg:
#if !defined(XWMDCFG_ds_SOC_DMA_ROCBT) || (1 != XWMDCFG_ds_SOC_DMA_ROCBT)
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

        if (__unlikely(!xwbmpaop_t1i(soc->dma.chstatus, ch))) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_enable)) {
                rc = drv->dma_enable(soc, ch);
                if (__unlikely(rc < 0)) {
                        goto err_drv_dma_enable;
                }
        }
        return OK;

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

        if (__unlikely(!xwbmpaop_t1i(soc->dma.chstatus, ch))) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_disable)) {
                rc = drv->dma_disable(soc, ch);
                if (__unlikely(rc < 0)) {
                        goto err_drv_dma_disable;
                }
        }
        return OK;

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

        if (__unlikely(!xwbmpaop_t1i(soc->dma.chstatus, ch))) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_start)) {
                rc = drv->dma_start(soc, ch);
                if (__unlikely(rc < 0)) {
                        goto err_drv_dma_start;
                }
        }
        return OK;

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

        if (__unlikely(!xwbmpaop_t1i(soc->dma.chstatus, ch))) {
                rc = -EPERM;
                goto err_notreq;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->dma_stop)) {
                rc = drv->dma_stop(soc, ch);
                if (__unlikely(rc < 0)) {
                        goto err_drv_dma_stop;
                }
        }
        return OK;

err_drv_dma_stop:
err_notreq:
        return rc;
}

/**
 * @brief XWDS LIB：通过描述得到DMA资源
 * @param base: (I) DMA资源数组的基地址
 * @param num: (I) DMA资源数量
 * @param descay: (I) 寄存器描述数组
 * @param descnum: (I) 寄存器描述数组数量
 * @param ret: (O) 返回DMA资源地址的缓存
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOKEY: 找不到描述的资源
 */
__xwds_lib_code
xwer_t xwds_get_dmarsc(const struct xwds_resource_dma base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_dma ** ret)
{
        xwer_t rc;
        xwsz_t i, m;

        for (i = 0; i < num; i++) {
                rc = OK;
                for (m = 0; m < descnum; m++) {
                        if (!strstr(base[i].description, descay[m])) {
                                rc = -ENOKEY;
                                break;
                        }
                }
                if (OK == rc) {
                        break;
                }
        }
        if (OK == rc) {
                *ret = &base[i];
        } else {
                *ret = NULL;
        }
        return rc;
}
