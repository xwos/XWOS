/**
 * @file
 * @brief 玄武设备栈：SPI主机模式控制器
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/spi/master.h>

static __xwds_vop
xwer_t xwds_spim_vop_probe(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_vop_remove(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_vop_start(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_vop_stop(struct xwds_spim * spim);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_spim_vop_suspend(struct xwds_spim * spim);

static __xwds_vop
xwer_t xwds_spim_vop_resume(struct xwds_spim * spim);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_spim_vop = {
        .probe = (void *)xwds_spim_vop_probe,
        .remove = (void *)xwds_spim_vop_remove,
        .start = (void *)xwds_spim_vop_start,
        .stop = (void *)xwds_spim_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_spim_vop_suspend,
        .resume = (void *)xwds_spim_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：SPI主机模式控制器对象的构造函数
 * @param[in] spim: SPI主机模式控制器对象指针
 */
__xwds_api
void xwds_spim_construct(struct xwds_spim * spim)
{
        xwds_device_construct(&spim->dev);
        spim->dev.vop = &xwds_spim_vop;
}

/**
 * @brief XWDS API：SPI主机模式控制器对象的析构函数
 * @param[in] spim: SPI主机模式控制器对象指针
 */
__xwds_api
void xwds_spim_destruct(struct xwds_spim * spim)
{
        struct xwds_device * dev;

        dev = xwds_cast(struct xwds_device *, spim);
        xwds_device_destruct(dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测设备
 * @param[in] spim: SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_vop_probe(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwos_mtx_init(&spim->xfer.apimtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xfer_apimtx_init;
        }
        rc = xwds_device_vop_probe(&spim->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_probe;
        }
        return XWOK;

err_dev_probe:
        xwos_mtx_fini(&spim->xfer.apimtx);
err_xfer_apimtx_init:
        return rc;
}

/**
 * @brief XWDS VOP：删除设备
 * @param[in] spim: SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_vop_remove(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_vop_remove(&spim->dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_remove;
        }
        xwos_mtx_fini(&spim->xfer.apimtx);
        return XWOK;

err_dev_vop_remove:
        return rc;
}

/**
 * @brief XWDS VOP：启动设备
 * @param[in] spim: SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_vop_start(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_vop_start(&spim->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止设备
 * @param[in] spim: SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_vop_stop(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_vop_stop(&spim->dev);
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停设备
 * @param[in] spim: SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_vop_suspend(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_vop_suspend(&spim->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续设备
 * @param[in] spim: SPI主机模式控制器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_spim_vop_resume(struct xwds_spim * spim)
{
        xwer_t rc;

        rc = xwds_device_vop_resume(&spim->dev);
        return rc;
}
#endif

/******** ******** ******** SPI Master Device APIs ******** ******** ********/
/**
 * @brief XWDS API：配置总线
 * @param[in] spim: SPI主机模式控制器对象指针
 * @param[in] cfgid: 总线配置ID
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOSYS: 不支持配置总线操作
 * @retval -ECHRNG: 配置ID不在配置表范围内
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_spim_buscfg(struct xwds_spim * spim, xwid_t cfgid, xwtm_t * xwtm)
{
        const struct xwds_spim_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(spim, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_spim_grab(spim);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_grab;
        }
        if (NULL == spim->buscfg) {
                rc = -ENOSYS;
                goto err_nosys;
        }
        if (cfgid >= spim->buscfg_num) {
                rc = -ECHRNG;
                goto err_chrng;
        }

        rc = xwos_mtx_timedlock(&spim->xfer.apimtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_lock;
        }
        drv = xwds_cast(const struct xwds_spim_driver *, spim->dev.drv);
        if (__xwcc_likely((drv) && (drv->buscfg))) {
                rc = drv->buscfg(spim, cfgid, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_buscfg;
        }
        xwos_mtx_unlock(&spim->xfer.apimtx);
        xwds_spim_put(spim);
        return XWOK;

err_drv_buscfg:
        xwos_mtx_unlock(&spim->xfer.apimtx);
err_spim_lock:
err_chrng:
err_nosys:
        xwds_spim_put(spim);
err_spim_grab:
        return rc;
}

/**
 * @brief XWDS API：启动SPI总线传输
 * @param[in] spim: SPI主机模式控制器对象指针
 * @param[in] txd: 发送数据缓冲区，可为NULL表示不发送数据
 * @param[out] rxb: 接收数据缓冲区，可为NULL表示不接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示缓冲区大小（单位：字节）
 * + (O) 作为输出时，返回实际传输的数据大小
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持主机模式传输
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_spim_xfer(struct xwds_spim * spim,
                      const xwu8_t txd[], xwu8_t * rxb,
                      xwsz_t * size, xwtm_t * xwtm)
{
        const struct xwds_spim_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(spim, "nullptr", -EFAULT);
        XWDS_VALIDATE(((txd) || (rxb)), "invalid", -EINVAL);
        XWDS_VALIDATE((size), "nullptr", -EFAULT);
        XWDS_VALIDATE((xwtm), "nullptr", -EFAULT);

        rc = xwds_spim_grab(spim);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_grab;
        }
        rc = xwos_mtx_timedlock(&spim->xfer.apimtx, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_lock;
        }
        drv = xwds_cast(const struct xwds_spim_driver *, spim->dev.drv);
        if (__xwcc_likely((drv) && (drv->xfer))) {
                rc = drv->xfer(spim, txd, rxb, size, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_xfer;
        }
        xwos_mtx_unlock(&spim->xfer.apimtx);
        xwds_spim_put(spim);
        return XWOK;

err_drv_xfer:
        xwos_mtx_unlock(&spim->xfer.apimtx);
err_spim_lock:
        xwds_spim_put(spim);
err_spim_grab:
        return rc;
}

/**
 * @brief XWDS API：中止SPI总线传输
 * @param[in] spim: SPI主机控制器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 设备对象不可引用
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_spim_abort(struct xwds_spim * spim, xwtm_t * xwtm)
{
        const struct xwds_spim_driver * drv;
        xwer_t rc;

        XWDS_VALIDATE(spim, "nullptr", -EFAULT);
        XWDS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwds_spim_grab(spim);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_spim_grab;
        }
        drv = xwds_cast(const struct xwds_spim_driver *, spim->dev.drv);
        if (__xwcc_likely((drv) && (drv->abort))) {
                rc = drv->abort(spim, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_abort;
        }
        xwds_spim_put(spim);
        return XWOK;

err_drv_abort:
        xwds_spim_put(spim);
err_spim_grab:
        return rc;
}
