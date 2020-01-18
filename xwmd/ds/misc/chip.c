/**
 * @file
 * @brief xwmd设备栈：MISC设备
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
#include <xwmd/ds/misc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_misc_cvop_probe(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_remove(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_start(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_stop(struct xwds_misc * misc);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
static __xwds_vop
xwer_t xwds_misc_cvop_suspend(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_resume(struct xwds_misc * misc);
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_rodata const struct xwds_base_virtual_operations xwds_misc_cvops = {
        .probe = (void *)xwds_misc_cvop_probe,
        .remove = (void *)xwds_misc_cvop_remove,
        .start = (void *)xwds_misc_cvop_start,
        .stop = (void *)xwds_misc_cvop_stop,
#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
        .suspend = (void *)xwds_misc_cvop_suspend,
        .resume = (void *)xwds_misc_cvop_resume,
#endif /* XWMDCFG_ds_LPM */
};
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief MISC设备的构造函数
 * @param misc: (I) MISC设备对象指针
 */
__xwds_code
void xwds_misc_construct(struct xwds_misc * misc)
{
        xwds_device_construct(&misc->dev);
        misc->dev.cvops = &xwds_misc_cvops;
}

/**
 * @brief MISC设备对象的析构函数
 * @param misc: (I) MISC设备对象指针
 */
__xwds_code
void xwds_misc_destruct(struct xwds_misc * misc)
{
        xwds_device_destruct(&misc->dev);
}
#endif /* !XWMDCFG_ds_NANO */

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_probe(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_probe(&misc->dev);
        return rc;
}

/**
 * @brief SODS VOP：移除MISC设备
 * @param ds: (I) 设备栈控制块指针
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_remove(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&misc->dev);
        return rc;
}

/**
 * @brief SODS VOP：启动MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_start(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&misc->dev);
        return rc;
}

/**
 * @brief SODS VOP：停止MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_stop(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&misc->dev);
        return rc;
}

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_suspend(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&misc->dev);
        return rc;
}

/**
 * @brief SODS VOP：继续MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_misc_cvop_resume(struct xwds_misc * misc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&misc->dev);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
/**
 * @brief SODS API：探测MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_misc_probe(struct xwds_misc * misc)
{
        xwer_t rc;

        SODS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_cvop_probe(misc);
        return rc;
}

/**
 * @brief SODS API：移除MISC设备
 * @param ds: (I) 设备栈控制块指针
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_misc_remove(struct xwds_misc * misc)
{
        xwer_t rc;

        SODS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_cvop_remove(misc);
        return rc;
}

/**
 * @brief SODS API：启动MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_misc_start(struct xwds_misc * misc)
{
        xwer_t rc;

        SODS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_cvop_start(misc);
        return rc;
}

/**
 * @brief SODS API：停止MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_misc_stop(struct xwds_misc * misc)
{
        xwer_t rc;

        SODS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_cvop_stop(misc);
        return rc;
}

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS API：暂停MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_misc_suspend(struct xwds_misc * misc)
{
        xwer_t rc;

        SODS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_cvop_suspend(misc);
        return rc;
}

/**
 * @brief SODS API：继续MISC设备
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_misc_resume(struct xwds_misc * misc)
{
        xwer_t rc;

        SODS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_cvop_resume(misc);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

/**
 * @brief SODS API：MISC设备输入、输出、控制
 * @param misc: (I) MISC设备对象指针
 * @param cmd: (I) 命令
 * @param ...: (I) 参数
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 中断上下文：依赖于CMD的实现
 * - 中断底半部：依赖于CMD的实现
 * - 线程上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_misc_ioctl(struct xwds_misc * misc, xwsq_t cmd, ...)
{
        const struct xwds_misc_driver * drv;
        va_list args;
        xwer_t rc;

        SODS_VALIDATE(misc, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_misc_grab(misc);
        if (__unlikely(rc < 0)) {
                goto err_misc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        va_start(args, cmd);
        drv = xwds_static_cast(const struct xwds_misc_driver *, misc->dev.drv);
        if ((drv) && (drv->ioctl)) {
                rc = drv->ioctl(misc, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_misc_put(misc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_ioctl:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_misc_put(misc);
err_misc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}
