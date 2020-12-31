/**
 * @file
 * @brief 玄武设备栈：MISC设备
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
#include <xwcd/ds/misc/chip.h>

static __xwds_vop
xwer_t xwds_misc_cvop_probe(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_remove(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_start(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_stop(struct xwds_misc * misc);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_misc_cvop_suspend(struct xwds_misc * misc);

static __xwds_vop
xwer_t xwds_misc_cvop_resume(struct xwds_misc * misc);
#endif /* XWCDCFG_ds_PM */

__xwds_rodata const struct xwds_base_virtual_operations xwds_misc_cvops = {
        .probe = (void *)xwds_misc_cvop_probe,
        .remove = (void *)xwds_misc_cvop_remove,
        .start = (void *)xwds_misc_cvop_start,
        .stop = (void *)xwds_misc_cvop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_misc_cvop_suspend,
        .resume = (void *)xwds_misc_cvop_resume,
#endif /* XWCDCFG_ds_PM */
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：MISC设备的构造函数
 * @param misc: (I) MISC设备对象指针
 */
__xwds_api
void xwds_misc_construct(struct xwds_misc * misc)
{
        xwds_device_construct(&misc->dev);
        misc->dev.cvops = &xwds_misc_cvops;
}

/**
 * @brief XWDS API：MISC设备对象的析构函数
 * @param misc: (I) MISC设备对象指针
 */
__xwds_api
void xwds_misc_destruct(struct xwds_misc * misc)
{
        xwds_device_destruct(&misc->dev);
}

/**
 * @brief XWDS API：增加对象的引用计数
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
__xwds_api
xwer_t xwds_misc_grab(struct xwds_misc * misc)
{
        return xwds_device_grab(&misc->dev);
}

/**
 * @brief XWDS API：减少对象的引用计数
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
__xwds_api
xwer_t xwds_misc_put(struct xwds_misc * misc)
{
        return xwds_device_put(&misc->dev);
}

/**
 * @brief XWDS API：增加设备运行状态计数器
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
__xwds_api
xwer_t xwds_misc_request(struct xwds_misc * misc)
{
        return xwds_device_request(&misc->dev);
}

/**
 * @brief XWDS API：减少设备运行状态计数器
 * @param misc: (I) MISC设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
__xwds_api
xwer_t xwds_misc_release(struct xwds_misc * misc)
{
        return xwds_device_release(&misc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测MISC设备
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
 * @brief XWDS VOP：移除MISC设备
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
 * @brief XWDS VOP：启动MISC设备
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
 * @brief XWDS VOP：停止MISC设备
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

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停MISC设备
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
 * @brief XWDS VOP：继续MISC设备
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
#endif /* XWCDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：MISC设备输入、输出、控制
 * @param misc: (I) MISC设备对象指针
 * @param cmd: (I) 命令
 * @param ...: (I) 参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效指针
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_misc_ioctl(struct xwds_misc * misc, xwsq_t cmd, ...)
{
        const struct xwds_misc_driver * drv;
        va_list args;
        xwer_t rc;

        XWDS_VALIDATE(misc, "nullptr", -EFAULT);

        rc = xwds_misc_grab(misc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_misc_grab;
        }

        va_start(args, cmd);
        drv = xwds_cast(const struct xwds_misc_driver *, misc->dev.drv);
        if ((drv) && (drv->ioctl)) {
                rc = drv->ioctl(misc, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

        xwds_misc_put(misc);
        return XWOK;

err_drv_ioctl:
        xwds_misc_put(misc);
err_misc_grab:
        return rc;
}