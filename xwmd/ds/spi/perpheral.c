/**
 * @file
 * @brief xwmd设备栈：SPI外设控制器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osal/scheduler.h>
#include <xwmd/ds/spi/perpheral.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_spip_cvop_probe(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_remove(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_start(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_stop(struct xwds_spip * spip);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
static __xwds_vop
xwer_t xwds_spip_cvop_suspend(struct xwds_spip * spip);

static __xwds_vop
xwer_t xwds_spip_cvop_resume(struct xwds_spip * spip);
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_rodata const struct xwds_base_virtual_operations xwds_spip_cvops = {
        .probe = (void *)xwds_spip_cvop_probe,
        .remove = (void *)xwds_spip_cvop_remove,
        .start = (void *)xwds_spip_cvop_start,
        .stop = (void *)xwds_spip_cvop_stop,
#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
        .suspend = (void *)xwds_spip_cvop_suspend,
        .resume = (void *)xwds_spip_cvop_resume,
#endif /* XWMDCFG_ds_LPM */
};
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief 构造函数
 * @param spip: (I) SPI外设控制器对象指针
 */
__xwds_code
void xwds_spip_construct(struct xwds_spip * spip)
{
        xwds_device_construct(&spip->dev);
        spip->dev.cvops = &xwds_spip_cvops;
}

/**
 * @brief 析构函数
 * @param spip: (I) SPI外设控制器对象指针
 */
__xwds_code
void xwds_spip_destruct(struct xwds_spip * spip)
{
        xwds_device_destruct(&spip->dev);
}
#endif /* !XWMDCFG_ds_NANO */

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_spip_cvop_probe(struct xwds_spip * spip)
{
        xwer_t rc;

#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
        SODS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);
#else /* XWMDCFG_ds_NANO */
        if (!is_err_or_null(spip->bus)) {
                rc = xwds_spim_grab(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__unlikely(rc < 0)) {
                goto err_spim_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        rc = xwds_device_cvop_probe(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_probe;
        }
        return OK;

err_dev_cvop_probe:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spim_put(spip->bus);
err_spim_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS VOP：删除设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_spip_cvop_remove(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_remove(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spim_put(spip->bus);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief SODS VOP：启动设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_spip_cvop_start(struct xwds_spip * spip)
{
        xwer_t rc;

#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
        SODS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);
#else /* XWMDCFG_ds_NANO */
        if (spip->bus) {
                rc = xwds_spim_request(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__unlikely(rc < 0)) {
                goto err_spim_request;
        }
#endif /* !XWMDCFG_ds_NANO */

        rc = xwds_device_cvop_start(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_start;
        }
        return OK;

err_dev_cvop_start:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spim_release(spip->bus);
err_spim_request:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS VOP：停止设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_spip_cvop_stop(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_stop(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_stop;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spim_release(spip->bus);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_dev_cvop_stop:
        return rc;
}

#if (defined(XWMDCFG_ds_LPM)) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_spip_cvop_suspend(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);

        rc = xwds_device_cvop_suspend(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_suspend;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spim_release(spip->bus);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_dev_cvop_suspend:
        return rc;
}

/**
 * @brief SODS VOP：继续设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_spip_cvop_resume(struct xwds_spip * spip)
{
        xwer_t rc;

#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
        SODS_VALIDATE(spip->bus, "nullptr", -EHOSTUNREACH);
#else /* XWMDCFG_ds_NANO */
        if (spip->bus) {
                rc = xwds_spim_request(spip->bus);
        } else {
                rc = -EHOSTUNREACH;
        }
        if (__unlikely(rc < 0)) {
                goto err_spim_request;
        }
#endif /* !XWMDCFG_ds_NANO */

        rc = xwds_device_cvop_resume(&spip->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_resume;
        }
        return OK;

err_dev_cvop_resume:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spim_release(spip->bus);
err_spim_request:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** SPI Perpheral APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
/**
 * @brief SODS API：探测设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_spip_probe(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip, "nullptr", -EFAULT);

        rc = xwds_spip_cvop_probe(spip);
        return rc;
}

/**
 * @brief SODS API：删除设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_spip_remove(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip, "nullptr", -EFAULT);

        rc = xwds_spip_cvop_remove(spip);
        return rc;
}

/**
 * @brief SODS API：启动设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_spip_start(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip, "nullptr", -EFAULT);

        rc = xwds_spip_cvop_start(spip);
        return rc;
}

/**
 * @brief SODS API：停止设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_spip_stop(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip, "nullptr", -EFAULT);

        rc = xwds_spip_cvop_stop(spip);
        return rc;
}

#if (defined(XWMDCFG_ds_LPM)) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS API：暂停设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_spip_suspend(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip, "nullptr", -EFAULT);

        rc = xwds_spip_cvop_suspend(spip);
        return rc;
}

/**
 * @brief SODS API：继续设备
 * @param spip: (I) SPI外设控制器对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -EHOSTUNREACH: SPI总线控制器错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_spip_resume(struct xwds_spip * spip)
{
        xwer_t rc;

        SODS_VALIDATE(spip, "nullptr", -EFAULT);

        rc = xwds_spip_cvop_resume(spip);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

/**
 * @brief SODS API：SPI外设输入、输出、控制
 * @param spip: (I) SPI外设
 * @param cmd: (I) 命令
 * @param ...: (I) 参数
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 不支持的API
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 中断上下文：依赖于CMD的实现
 * - 中断底半部：依赖于CMD的实现
 * - 线程上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_spip_ioctl(struct xwds_spip * spip, xwsq_t cmd, ...)
{
        xwer_t rc;
        va_list args;
        const struct xwds_spip_driver * drv;

        SODS_VALIDATE(spip, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_spip_grab(spip);
        if (__unlikely(rc < 0)) {
                goto err_spip_grab;
        }
        rc = xwds_spip_request(spip);
        if (__unlikely(rc < 0)) {
                goto err_spip_request;
        }
#endif /* !XWMDCFG_ds_NANO */

        va_start(args, cmd);
        drv = xwds_static_cast(const struct xwds_spip_driver *, spip->dev.drv);
        if (drv->ioctl) {
                rc = drv->ioctl(spip, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spip_release(spip);
        xwds_spip_put(spip);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_ioctl:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_spip_release(spip);
err_spip_request:
        xwds_spip_put(spip);
err_spip_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}
