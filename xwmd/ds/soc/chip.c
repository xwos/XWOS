/**
 * @file
 * @brief xwmd设备栈：SOC设备
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
#include <xwos/lib/string.h>
#include <xwmd/ds/soc/chip.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_soc_cvop_probe(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_remove(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_start(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_stop(struct xwds_soc * soc);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
static __xwds_vop
xwer_t xwds_soc_cvop_suspend(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_resume(struct xwds_soc * soc);
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_rodata const struct xwds_base_virtual_operations xwds_soc_cvops = {
        .probe = (void *)xwds_soc_cvop_probe,
        .remove = (void *)xwds_soc_cvop_remove,
        .start = (void *)xwds_soc_cvop_start,
        .stop = (void *)xwds_soc_cvop_stop,
#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
        .suspend = (void *)xwds_soc_cvop_suspend,
        .resume = (void *)xwds_soc_cvop_resume,
#endif /* XWMDCFG_ds_LPM */
};
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief SOC构造函数
 * @param soc: (I) SOC对象指针
 */
__xwds_code
void xwds_soc_construct(struct xwds_soc * soc)
{
        xwds_device_construct(&soc->dev);
        soc->dev.cvops = &xwds_soc_cvops;
}

/**
 * @brief SOC对象的析构函数
 * @param soc: (I) SOC对象指针
 */
__xwds_code
void xwds_soc_destruct(struct xwds_soc * soc)
{
        xwds_device_destruct(&soc->dev);
}
#endif /* !XWMDCFG_ds_NANO */

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_soc_cvop_probe(struct xwds_soc * soc)
{
        xwer_t rc;

        SODS_VALIDATE(soc->xwccfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_probe(&soc->dev);
        return rc;
}

/**
 * @brief SODS VOP：移除SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_soc_cvop_remove(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&soc->dev);
        return rc;
}

/**
 * @brief SODS VOP：启动SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_soc_cvop_start(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&soc->dev);
        return rc;
}

/**
 * @brief SODS VOP：停止SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_soc_cvop_stop(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&soc->dev);
        return rc;
}

#if (defined(XWMDCFG_ds_LPM)) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_soc_cvop_suspend(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&soc->dev);
        return rc;
}

/**
 * @brief SODS VOP：继续SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
static __xwds_vop
xwer_t xwds_soc_cvop_resume(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&soc->dev);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
/**
 * @brief SODS API：探测SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_soc_probe(struct xwds_soc * soc)
{
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_cvop_probe(soc);
        return rc;
}

/**
 * @brief SODS API：移除SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_soc_remove(struct xwds_soc * soc)
{
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_cvop_remove(soc);
        return rc;
}

/**
 * @brief SODS API：启动SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_soc_start(struct xwds_soc * soc)
{
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_cvop_start(soc);
        return rc;
}

/**
 * @brief SODS API：停止SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_soc_stop(struct xwds_soc * soc)
{
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_cvop_stop(soc);
        return rc;
}

#if (defined(XWMDCFG_ds_LPM)) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS API：暂停SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_soc_suspend(struct xwds_soc * soc)
{
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_cvop_suspend(soc);
        return rc;
}

/**
 * @brief SODS API：继续SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_soc_resume(struct xwds_soc * soc)
{
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_cvop_resume(soc);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

/**
 * @brief SODS API：SOC输入、输出、控制
 * @param soc: (I) SOC对象指针
 * @param cmd: (I) 命令
 * @param ...: (I) 参数表
 * @return 错误码
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 中断上下文：依赖于CMD的实现
 * - 中断底半部：依赖于CMD的实现
 * - 线程上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_soc_ioctl(struct xwds_soc * soc, xwsq_t cmd, ...)
{
        const struct xwds_soc_driver * drv;
        va_list args;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }
#endif /* !XWMDCFG_ds_NANO */

        va_start(args, cmd);
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->ioctl)) {
                rc = drv->ioctl(soc, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_ioctl:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_soc_put(soc);
err_soc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}
