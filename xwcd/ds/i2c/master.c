/**
 * @file
 * @brief 玄武设备栈：I2C主机
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
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/i2c/master.h>

static __xwds_vop
xwer_t xwds_i2cm_cvop_probe(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_remove(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_start(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_stop(struct xwds_i2cm * i2cm);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_i2cm_cvop_suspend(struct xwds_i2cm * i2cm);

static __xwds_vop
xwer_t xwds_i2cm_cvop_resume(struct xwds_i2cm * i2cm);
#endif /* XWCDCFG_ds_PM */

__xwds_rodata const struct xwds_base_virtual_operations xwds_i2cm_cvops = {
        .probe = (void *)xwds_i2cm_cvop_probe,
        .remove = (void *)xwds_i2cm_cvop_remove,
        .start = (void *)xwds_i2cm_cvop_start,
        .stop = (void *)xwds_i2cm_cvop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_i2cm_cvop_suspend,
        .resume = (void *)xwds_i2cm_cvop_resume,
#endif /* XWCDCFG_ds_PM */
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：I2C主机控制器的构造函数
 * @param i2cm: (I) I2C主机控制器对象指针
 */
__xwds_api
void xwds_i2cm_construct(struct xwds_i2cm * i2cm)
{
        xwds_device_construct(&i2cm->dev);
        i2cm->dev.cvops = &xwds_i2cm_cvops;
}

/**
 * @brief XWDS API：I2C主机控制器对象的析构函数
 * @param i2cm: (I) I2C主机控制器对象指针
 */
__xwds_api
void xwds_i2cm_destruct(struct xwds_i2cm * i2cm)
{
        xwds_device_destruct(&i2cm->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_probe(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        XWDS_VALIDATE(i2cm->xwccfg, "nullptr", -EFAULT);

        rc = xwos_mtx_init(&i2cm->xfer.apimtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xfer_mtx_init;
        }
        rc = xwos_mtx_init(&i2cm->abort.apimtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_abort_mtx_init;
        }
        rc = xwds_device_cvop_probe(&i2cm->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_probe;
        }
        return XWOK;

err_dev_cvop_probe:
        xwos_mtx_destroy(&i2cm->abort.apimtx);
err_abort_mtx_init:
        xwos_mtx_destroy(&i2cm->xfer.apimtx);
err_xfer_mtx_init:
        return rc;
}

/**
 * @brief XWDS VOP：移除I2C主机控制器
 * @param ds: (I) 设备栈控制块指针
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_remove(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&i2cm->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }
        xwos_mtx_destroy(&i2cm->abort.apimtx);
        xwos_mtx_destroy(&i2cm->xfer.apimtx);
        return XWOK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_start(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&i2cm->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_stop(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&i2cm->dev);
        return rc;
}

/******** ******** pm ******** ********/
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief XWDS VOP：暂停I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_suspend(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&i2cm->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续I2C主机控制器
 * @param i2cm: (I) I2C主机控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_i2cm_cvop_resume(struct xwds_i2cm * i2cm)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&i2cm->dev);
        return rc;
}
#endif /* XWCDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：传输I2C消息
 * @param i2cm: (I) I2C主机控制器对象指针
 * @param msg: (I) I2C消息结构体的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_i2cm_xfer(struct xwds_i2cm * i2cm, struct xwds_i2c_msg * msg,
                      xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_i2cm_driver * drv;

        XWDS_VALIDATE(i2cm, "nullptr", -EFAULT);
        XWDS_VALIDATE(msg, "nullptr", -EFAULT);
        XWDS_VALIDATE((msg->flag & XWDS_I2C_F_DIRMSK), "no-direction", -EINVAL);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_i2cm_grab(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_grab;
        }
        rc = xwds_i2cm_request(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_request;
        }

        rc = xwos_mtx_timedlock(&i2cm->xfer.apimtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_lock;
        }
        drv = xwds_cast(const struct xwds_i2cm_driver *, i2cm->dev.drv);
        if ((drv) && (drv->xfer)) {
                rc = drv->xfer(i2cm, msg, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_xfer;
        }
        xwos_mtx_unlock(&i2cm->xfer.apimtx);
        xwds_i2cm_release(i2cm);
        xwds_i2cm_put(i2cm);
        return XWOK;

err_drv_xfer:
        xwos_mtx_unlock(&i2cm->xfer.apimtx);
err_i2cm_lock:
        xwds_i2cm_release(i2cm);
err_i2cm_request:
        xwds_i2cm_put(i2cm);
err_i2cm_grab:
        return rc;
}

/**
 * @brief XWDS API：中止I2C总线传输
 * @param i2cm: (I) I2C主机控制器对象指针
 * @param address: (I) I2C地址
 * @param addrmode: (I) I2C地址模式，取值：
 *                      @ref XWDS_I2C_F_7BITADDR 7位地址模式
 *                      @ref XWDS_I2C_F_10BITADDR 10位地址模式
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ESHUTDOWN: 设备没有运行
 * @retval -EADDRNOTAVAIL: 地址无响应
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_i2cm_abort(struct xwds_i2cm * i2cm,
                       xwu16_t address, xwu16_t addrmode,
                       xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_i2cm_driver * drv;

        XWDS_VALIDATE(i2cm, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_i2cm_grab(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_grab;
        }
        rc = xwds_i2cm_request(i2cm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_request;
        }

        rc = xwos_mtx_timedlock(&i2cm->abort.apimtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_i2cm_lock;
        }
        drv = xwds_cast(const struct xwds_i2cm_driver *, i2cm->dev.drv);
        if ((drv) && (drv->abort)) {
                rc = drv->abort(i2cm, address, addrmode, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_abort;
        }
        xwos_mtx_unlock(&i2cm->abort.apimtx);
        xwds_i2cm_release(i2cm);
        xwds_i2cm_put(i2cm);
        return XWOK;

err_drv_abort:
        xwos_mtx_unlock(&i2cm->abort.apimtx);
err_i2cm_lock:
        xwds_i2cm_release(i2cm);
err_i2cm_request:
        xwds_i2cm_put(i2cm);
err_i2cm_grab:
        return rc;
}