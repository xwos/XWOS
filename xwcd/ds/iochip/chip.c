/**
 * @file
 * @brief 玄武设备栈：IO扩展芯片
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
#include <xwcd/ds/iochip/chip.h>

static __xwds_vop
xwer_t xwds_iochip_vop_probe(struct xwds_iochip * iochip);

static __xwds_vop
xwer_t xwds_iochip_vop_remove(struct xwds_iochip * iochip);

static __xwds_vop
xwer_t xwds_iochip_vop_start(struct xwds_iochip * iochip);

static __xwds_vop
xwer_t xwds_iochip_vop_stop(struct xwds_iochip * iochip);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_iochip_vop_suspend(struct xwds_iochip * iochip);

static __xwds_vop
xwer_t xwds_iochip_vop_resume(struct xwds_iochip * iochip);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_iochip_vop = {
        .probe = (void *)xwds_iochip_vop_probe,
        .remove = (void *)xwds_iochip_vop_remove,
        .start = (void *)xwds_iochip_vop_start,
        .stop = (void *)xwds_iochip_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_iochip_vop_suspend,
        .resume = (void *)xwds_iochip_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_iochip_construct(struct xwds_iochip * iochip)
{
        xwds_device_construct(&iochip->bc.dev);
        iochip->bc.dev.vop = &xwds_iochip_vop;
}

__xwds_api
void xwds_iochip_destruct(struct xwds_iochip * iochip)
{
        xwds_device_destruct(&iochip->bc.dev);
}

__xwds_api
xwer_t xwds_iochip_grab(struct xwds_iochip * iochip)
{
        return xwds_device_grab(&iochip->bc.dev);
}

__xwds_api
xwer_t xwds_iochip_put(struct xwds_iochip * iochip)
{
        return xwds_device_put(&iochip->bc.dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测IOCHIP
 * @param[in] iochip: IOCHIP对象指针
 */
static __xwds_vop
xwer_t xwds_iochip_vop_probe(struct xwds_iochip * iochip)
{
        xwer_t rc;

        XWDS_VALIDATE(iochip->xwccfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_probe(&iochip->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除IOCHIP
 * @param[in] iochip: IOCHIP对象指针
 */
static __xwds_vop
xwer_t xwds_iochip_vop_remove(struct xwds_iochip * iochip)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&iochip->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动IOCHIP
 * @param[in] iochip: IOCHIP对象指针
 */
static __xwds_vop
xwer_t xwds_iochip_vop_start(struct xwds_iochip * iochip)
{
        xwer_t rc;

        rc = xwds_device_vop_start(&iochip->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止IOCHIP
 * @param[in] iochip: IOCHIP对象指针
 */
static __xwds_vop
xwer_t xwds_iochip_vop_stop(struct xwds_iochip * iochip)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&iochip->bc.dev);
        return rc;
}

#if (defined(XWCDCFG_ds_PM)) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停IOCHIP
 * @param[in] iochip: IOCHIP对象指针
 */
static __xwds_vop
xwer_t xwds_iochip_vop_suspend(struct xwds_iochip * iochip)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&iochip->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续IOCHIP
 * @param[in] iochip: IOCHIP对象指针
 */
static __xwds_vop
xwer_t xwds_iochip_vop_resume(struct xwds_iochip * iochip)
{
        xwer_t rc;

        rc = xwds_device_vop_resume(&iochip->bc.dev);
        return rc;
}
#endif
