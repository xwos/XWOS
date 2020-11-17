/**
 * @file
 * @brief xwcd设备栈：SOC设备
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
#include <string.h>
#include <xwcd/ds/standard.h>
#include <xwcd/ds/soc/chip.h>

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

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_soc_cvop_suspend(struct xwds_soc * soc);

static __xwds_vop
xwer_t xwds_soc_cvop_resume(struct xwds_soc * soc);
#endif /* XWCDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_rodata const struct xwds_base_virtual_operations xwds_soc_cvops = {
        .probe = (void *)xwds_soc_cvop_probe,
        .remove = (void *)xwds_soc_cvop_remove,
        .start = (void *)xwds_soc_cvop_start,
        .stop = (void *)xwds_soc_cvop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_soc_cvop_suspend,
        .resume = (void *)xwds_soc_cvop_resume,
#endif /* XWCDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：SOC构造函数
 * @param soc: (I) SOC对象指针
 */
__xwds_api
void xwds_soc_construct(struct xwds_soc * soc)
{
        xwds_device_construct(&soc->dev);
        soc->dev.cvops = &xwds_soc_cvops;
}

/**
 * @brief XWDS API：SOC对象的析构函数
 * @param soc: (I) SOC对象指针
 */
__xwds_api
void xwds_soc_destruct(struct xwds_soc * soc)
{
        xwds_device_destruct(&soc->dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_probe(struct xwds_soc * soc)
{
        xwer_t rc;

        XWDS_VALIDATE(soc->xwccfg, "nullptr", -EFAULT);

        rc = xwds_device_cvop_probe(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_remove(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_start(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：停止SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_stop(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&soc->dev);
        return rc;
}

#if (defined(XWCDCFG_ds_PM)) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_suspend(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&soc->dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续SOC
 * @param soc: (I) SOC对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_soc_cvop_resume(struct xwds_soc * soc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&soc->dev);
        return rc;
}
#endif /* XWCDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：SOC输入、输出、控制
 * @param soc: (I) SOC对象指针
 * @param cmd: (I) 命令
 * @param ...: (I) 参数表
 * @return 错误码
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_soc_ioctl(struct xwds_soc * soc, xwsq_t cmd, ...)
{
        const struct xwds_soc_driver * drv;
        va_list args;
        xwer_t rc;

        XWDS_VALIDATE(soc, "nullptr", -EFAULT);

        rc = xwds_soc_grab(soc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_soc_grab;
        }

        va_start(args, cmd);
        drv = xwds_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->ioctl)) {
                rc = drv->ioctl(soc, cmd, args);
        } else {
                rc = -ENOSYS;
        }
        va_end(args);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_drv_ioctl;
        }

        xwds_soc_put(soc);
        return XWOK;

err_drv_ioctl:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}
