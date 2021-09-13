/**
 * @file
 * @brief 玄武设备栈：SPI外设控制器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
#include <xwos/osal/skd.h>
#include <xwcd/ds/spi/perpheral.h>

static __xwds_vop
xwer_t xwds_spip_vop_probe(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_vop_remove(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_vop_start(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_vop_stop(struct xwds_spip * spip);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_spip_vop_suspend(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_vop_resume(struct xwds_spip * spip);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_spip_vop = {
        .probe = (void *)xwds_spip_vop_probe,
        .remove = (void *)xwds_spip_vop_remove,
        .start = (void *)xwds_spip_vop_start,
        .stop = (void *)xwds_spip_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_spip_vop_suspend,
        .resume = (void *)xwds_spip_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：SPI外设控制器对象的构造函数
 * @param[in] spip: SPI外设控制器对象指针
 */
__xwds_api
void xwds_spip_construct(struct xwds_spip * spip)
{
        xwds_device_construct(&spip->dev);
        spip->dev.vop = &xwds_spip_vop;
}

/**
 * @brief XWDS API：SPI外设控制器对象的析构函数
 * @param[in] spip: SPI外设控制器对象指针
 */
__xwds_api
void xwds_spip_destruct(struct xwds_spip * spip)
{
        xwds_device_destruct(&spip->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测设备
 * @param[in] spip: SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_vop_probe(struct xwds_spip * spip)
{
        xwer_t rc;

        rc = xwds_device_vop_probe(&spip->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_probe;
        }
        return XWOK;

err_dev_vop_probe:
        return rc;
}

/**
 * @brief XWDS VOP：删除设备
 * @param[in] spip: SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_vop_remove(struct xwds_spip * spip)
{
        xwer_t rc;

        XWDS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_vop_remove(&spip->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_remove;
        }
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动设备
 * @param[in] spip: SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_vop_start(struct xwds_spip * spip)
{
        xwer_t rc;

        if (spip->bus) {
                rc = xwds_spim_grab(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_grab;
        }
        rc = xwds_device_vop_start(&spip->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_start;
        }
        return XWOK;

err_dev_vop_start:
        xwds_spim_put(spip->bus);
err_spim_grab:
        return rc;
}

/**
 * @brief XWDS VOP：停止设备
 * @param[in] spip: SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_vop_stop(struct xwds_spip * spip)
{
        xwer_t rc;

        XWDS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_vop_stop(&spip->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_stop;
        }
        xwds_spim_put(spip->bus);
        return XWOK;

err_dev_vop_stop:
        return rc;
}

#if (defined(XWCDCFG_ds_PM)) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停设备
 * @param[in] spip: SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_vop_suspend(struct xwds_spip * spip)
{
        xwer_t rc;

        XWDS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_vop_suspend(&spip->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_suspend;
        }
        xwds_spim_put(spip->bus);
        return XWOK;

err_dev_vop_suspend:
        return rc;
}

/**
 * @brief XWDS VOP：继续设备
 * @param[in] spip: SPI外设控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spip_vop_resume(struct xwds_spip * spip)
{
        xwer_t rc;

        if (spip->bus) {
                rc = xwds_spim_grab(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_grab;
        }
        rc = xwds_device_vop_resume(&spip->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_resume;
        }
        return XWOK;

err_dev_vop_resume:
        xwds_spim_put(spip->bus);
err_spim_grab:
        return rc;
}
#endif
