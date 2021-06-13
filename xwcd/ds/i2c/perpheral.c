/**
 * @file
 * @brief 玄武设备栈：I2C外设
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
#include <xwcd/ds/i2c/perpheral.h>

static __xwds_vop
xwer_t xwds_i2cp_vop_probe(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_vop_remove(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_vop_start(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_vop_stop(struct xwds_i2cp * i2cp);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_i2cp_vop_suspend(struct xwds_i2cp * i2cp);

static __xwds_vop
xwer_t xwds_i2cp_vop_resume(struct xwds_i2cp * i2cp);
#endif /* XWCDCFG_ds_PM */

__xwds_rodata const struct xwds_virtual_operation xwds_i2cp_vop = {
        .probe = (void *)xwds_i2cp_vop_probe,
        .remove = (void *)xwds_i2cp_vop_remove,
        .start = (void *)xwds_i2cp_vop_start,
        .stop = (void *)xwds_i2cp_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_i2cp_vop_suspend,
        .resume = (void *)xwds_i2cp_vop_resume,
#endif /* XWCDCFG_ds_PM */
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：I2C外设的构造函数
 * @param[in] i2cp: I2C外设对象指针
 */
__xwds_api
void xwds_i2cp_construct(struct xwds_i2cp * i2cp)
{
        xwds_device_construct(&i2cp->dev);
        i2cp->dev.vop = &xwds_i2cp_vop;
}

/**
 * @brief XWDS API：I2C外设的构造函数
 * @param[in] i2cp: I2C外设对象指针
 */
__xwds_api
void xwds_i2cp_destruct(struct xwds_i2cp * i2cp)
{
        xwds_device_destruct(&i2cp->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测I2C外设
 * @param[in] i2cp: I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_vop_probe(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        rc = xwds_device_vop_probe(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_probe;
        }
        return XWOK;

err_dev_vop_probe:
        return rc;
}

/**
 * @brief XWDS VOP：移除I2C外设
 * @param[in] i2cp: I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_vop_remove(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cp->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_vop_remove(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_remove;
        }
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动I2C外设
 * @param[in] i2cp: I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_vop_start(struct xwds_i2cp * i2cp)
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
        rc = xwds_device_vop_start(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_start;
        }
        return XWOK;

err_dev_vop_start:
        xwds_i2cm_put(i2cp->bus);
err_i2cm_grab:
        return rc;
}

/**
 * @brief XWDS VOP：停止I2C外设
 * @param[in] i2cp: I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_vop_stop(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cp->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_vop_stop(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_stop;
        }
        xwds_i2cm_put(i2cp->bus);
        return XWOK;

err_dev_vop_stop:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停I2C外设
 * @param[in] i2cp: I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_vop_suspend(struct xwds_i2cp * i2cp)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cp->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_vop_suspend(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_suspend;
        }
        xwds_i2cm_put(i2cp->bus);
        return XWOK;

err_dev_vop_suspend:
        return rc;
}

/**
 * @brief XWDS VOP：继续I2C外设
 * @param[in] i2cp: I2C外设对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cp_vop_resume(struct xwds_i2cp * i2cp)
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
        rc = xwds_device_vop_resume(&i2cp->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_resume;
        }
        return XWOK;

err_dev_vop_resume:
        xwds_i2cm_put(i2cp->bus);
err_i2cm_grab:
        return rc;
}
#endif /* XWCDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：I2C外设输入、输出、控制
 * @param[in] i2cp: I2C外设对象指针
 * @param[in] cmd: 命令
 * @param[in] ...: 参数表
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
        xwds_i2cp_put(i2cp);
        return XWOK;

err_drv_ioctl:
        xwds_i2cp_put(i2cp);
err_i2cp_grab:
        return rc;
}
