/**
 * @file
 * @brief xwcd设备栈：I2C外设
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

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwcd/ds/standard.h>
#include <xwos/osal/skd.h>
#include <xwcd/ds/i2c/perpheral.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_i2cp_cvop_probe(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_cvop_remove(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_cvop_start(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_cvop_stop(struct xwds_i2cp * i2cp);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_i2cp_cvop_suspend(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_cvop_resume(struct xwds_i2cp * i2cp);
#endif /* XWCDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_i2cp_cvops = {
        .probe = (void *)xwds_i2cp_cvop_probe,
        .remove = (void *)xwds_i2cp_cvop_remove,
        .start = (void *)xwds_i2cp_cvop_start,
        .stop = (void *)xwds_i2cp_cvop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_i2cp_cvop_suspend,
        .resume = (void *)xwds_i2cp_cvop_resume,
#endif /* XWCDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：I2C外设的构造函数
 * @param i2cp: (I) I2C外设对象指针
 */
__xwds_api
void xwds_i2cp_construct(struct xwds_i2cp * i2cp)
{
        xwds_device_construct(&i2cp->dev);
        i2cp->dev.cvops = &xwds_i2cp_cvops;
}

/**
 * @brief XWDS API：I2C外设的构造函数
 * @param i2cp: (I) I2C外设对象指针
 */
__xwds_api
void xwds_i2cp_destruct(struct xwds_i2cp * i2cp)
{
        xwds_device_destruct(&i2cp->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测I2C外设
 * @param i2cp: (I) I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_cvop_probe(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        if (i2cp->bus) {
                rc = xwds_i2cm_grab(i2cp->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_grab;
        }

        rc = xwds_device_cvop_probe(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_probe;
        }
        return XWOK;

err_dev_cvop_probe:
        xwds_i2cm_put(i2cp->bus);
err_i2cm_grab:
        return rc;
}

/**
 * @brief XWDS VOP：移除I2C外设
 * @param i2cp: (I) I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_cvop_remove(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cp->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_remove(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }

        xwds_i2cm_put(i2cp->bus);
        return XWOK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动I2C外设
 * @param i2cp: (I) I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_cvop_start(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        if (i2cp->bus) {
                rc = xwds_i2cm_request(i2cp->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_request;
        }

        rc = xwds_device_cvop_start(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_start;
        }
        return XWOK;

err_dev_cvop_start:
        xwds_i2cm_release(i2cp->bus);
err_i2cm_request:
        return rc;
}

/**
 * @brief XWDS VOP：停止I2C外设
 * @param i2cp: (I) I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_cvop_stop(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cp->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_stop(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_stop;
        }

        xwds_i2cm_release(i2cp->bus);
        return XWOK;

err_dev_cvop_stop:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停I2C外设
 * @param i2cp: (I) I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_cvop_suspend(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cp->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_suspend(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_suspend;
        }

        xwds_i2cm_release(i2cp->bus);
        return XWOK;

err_dev_cvop_suspend:
        return rc;
}

/**
 * @brief XWDS VOP：继续I2C外设
 * @param i2cp: (I) I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_cvop_resume(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        if (i2cp->bus) {
                rc = xwds_i2cm_request(i2cp->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_request;
        }

        rc = xwds_device_cvop_resume(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_resume;
        }
        return XWOK;

err_dev_cvop_resume:
        xwds_i2cm_release(i2cp->bus);
err_i2cm_request:
        return rc;
}
#endif /* XWCDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：I2C外设输入、输出、控制
 * @param i2cp: (I) I2C外设对象指针
 * @param cmd: (I) 命令
 * @param ...: (I) 参数表
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOSYS: 无效CMD
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_i2cp_ioctl(struct xwds_i2cp * i2cp, xwsq_t cmd, ...)
{
        const struct xwds_i2cp_driver * drv;
        va_list args;
        xwer_t rc;

        XWDS_VALIDATE(i2cp, "nullptr", -EFAULT);

        rc = xwds_i2cp_grab(i2cp);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cp_grab;
        }
        rc = xwds_i2cp_request(i2cp);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cp_request;
        }

        va_start(args, cmd);
        drv = xwds_cast(const struct xwds_i2cp_driver *, i2cp->dev.drv);
        if ((drv) && (drv->ioctl)) {
                rc = drv->ioctl(i2cp, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

        xwds_i2cp_release(i2cp);
        xwds_i2cp_put(i2cp);
        return XWOK;

err_drv_ioctl:
        xwds_i2cp_release(i2cp);
err_i2cp_request:
        xwds_i2cp_put(i2cp);
err_i2cp_grab:
        return rc;
}
