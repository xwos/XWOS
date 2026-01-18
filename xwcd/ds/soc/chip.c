/**
 * @file
 * @brief 玄武设备栈：SOC
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
#include <xwcd/ds/soc/chip.h>

static __xwds_vop
xwer_t xwds_soc_vop_probe(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_vop_remove(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_vop_start(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_vop_stop(struct xwds_soc * soc);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_soc_vop_suspend(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_vop_resume(struct xwds_soc * soc);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_soc_vop = {
        .probe = (void *)xwds_soc_vop_probe,
        .remove = (void *)xwds_soc_vop_remove,
        .start = (void *)xwds_soc_vop_start,
        .stop = (void *)xwds_soc_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_soc_vop_suspend,
        .resume = (void *)xwds_soc_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_soc_construct(struct xwds_soc * soc)
{
        xwds_device_construct(&soc->dev);
        soc->dev.vop = &xwds_soc_vop;
}

__xwds_api
void xwds_soc_destruct(struct xwds_soc * soc)
{
        xwds_device_destruct(&soc->dev);
}

__xwds_api
xwer_t xwds_soc_grab(struct xwds_soc * soc)
{
        return xwds_device_grab(&soc->dev);
}

__xwds_api
xwer_t xwds_soc_put(struct xwds_soc * soc)
{
        return xwds_device_put(&soc->dev);
}

__xwds_api
xwer_t xwds_soc_get_reset_reason(struct xwds_soc * soc, xwu64_t * reason)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(reason, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (rc < 0) {
                goto err_soc_grab;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->get_reset_reason)) {
                rc = drv->get_reset_reason(soc, reason);
        } else {
                rc = -ENOSYS;
        }
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

__xwds_api
xwer_t xwds_soc_get_wkup_source(struct xwds_soc * soc, xwu64_t * source)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);
        XWDS_VALIDATE(reason, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (rc < 0) {
                goto err_soc_grab;
        }
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->get_wkup_source)) {
                rc = drv->get_wkup_source(soc, source);
        } else {
                rc = -ENOSYS;
        }
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测SOC
 * @param[in] soc: SOC对象指针
 */
static __xwds_vop
xwer_t xwds_soc_vop_probe(struct xwds_soc * soc)
{
        xwer_t rc;

        XWDS_VALIDATE(soc->xwccfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_probe(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除SOC
 * @param[in] soc: SOC对象指针
 */
static __xwds_vop
xwer_t xwds_soc_vop_remove(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动SOC
 * @param[in] soc: SOC对象指针
 */
static __xwds_vop
xwer_t xwds_soc_vop_start(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_vop_start(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止SOC
 * @param[in] soc: SOC对象指针
 */
static __xwds_vop
xwer_t xwds_soc_vop_stop(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&soc->dev);
        return rc;
}

#if (defined(XWCDCFG_ds_PM)) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停SOC
 * @param[in] soc: SOC对象指针
 */
static __xwds_vop
xwer_t xwds_soc_vop_suspend(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续SOC
 * @param[in] soc: SOC对象指针
 */
static __xwds_vop
xwer_t xwds_soc_vop_resume(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_vop_resume(&soc->dev);
        return rc;
}
#endif
