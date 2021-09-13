/**
 * @file
 * @brief 玄武设备栈：设备基类
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
#include <string.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/xwds.h>
#include <xwcd/ds/object.h>
#include <xwcd/ds/device.h>

/**
 * @brief 实现面向对象“多态”的函数表
 */
__xwds_rodata const struct xwds_virtual_operation xwds_dev_vop = {
        .probe = xwds_device_vop_probe,
        .remove = xwds_device_vop_remove,
        .start = xwds_device_vop_start,
        .stop = xwds_device_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = xwds_device_vop_suspend,
        .resume = xwds_device_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：设备的构造函数
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 */
__xwds_api
void xwds_device_construct(struct xwds_device * dev)
{
        xwds_obj_construct(&dev->obj);
        dev->vop = &xwds_dev_vop;
        dev->ds = NULL;
}

/**
 * @brief XWDS API：设备的析构函数
 * @param[in] dev: 设备对象的指针
 */
__xwds_api
void xwds_device_destruct(struct xwds_device * dev)
{
        xwds_obj_destruct(&dev->obj);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief 设备基本操作函数：探测设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_vop_probe(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->probe)) {
                rc = drv->probe(dev);
        } else {
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：删除设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_vop_remove(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->remove)) {
                rc = drv->remove(dev);
        } else {
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：启动设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_vop_start(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->start)) {
                rc = drv->start(dev);
        } else {
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：停止设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_vop_stop(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->stop)) {
                rc = drv->stop(dev);
        } else {
                rc = XWOK;
        }
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief 设备基本操作函数：暂停设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_vop_suspend(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->suspend)) {
                rc = drv->suspend(dev);
        } else {
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：继续设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_vop_resume(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->resume)) {
                rc = drv->resume(dev);
        } else {
                rc = XWOK;
        }
        return rc;
}
#endif

/******** ******** ******** APIs ******** ******** ********/
__xwds_code
xwer_t xwds_device_gc(void * obj)
{
        struct xwds_device * dev;
        struct xwds * ds;
        const struct xwds_virtual_operation * vop;
        xwer_t rc;

        dev = obj;
        ds = dev->ds;
        if (ds) {
                xwds_obj_del(ds, &dev->obj);
        }
        dev->ds = NULL;

        /* remove device */
        vop = dev->vop;
        if (vop && vop->remove) {
                rc = vop->remove(dev);
        } else {
                rc = xwds_device_vop_remove(dev);
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_remove;
        }
        return XWOK;

err_dev_vop_remove:
        xwds_obj_add(ds, &dev->obj);
        dev->ds = ds;
        return rc;
}

/**
 * @brief XWDS API：探测设备
 * @param[in] ds: 设备栈控制块指针
 * @param[in] dev: 设备对象的指针
 * @param[in] gcfunc: 垃圾回收函数
 * @return 错误码
 * @retval XWOK: 没有错误发生
 * @retval -EPERM: 引用计数错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_probe(struct xwds * ds, struct xwds_device * dev,
                         xwobj_gc_f gcfunc)
{
        const struct xwds_virtual_operation * vop;
        xwer_t rc;

        XWDS_VALIDATE(ds, "nullptr", -EFAULT);
        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwds_obj_probe(&dev->obj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_probe;
        }
        /* refcnt == XWDS_OBJ_REF_SHUTDOWN */

        /* probe device */
        vop = dev->vop;
        if (vop && vop->probe) {
                rc = vop->probe(dev);
        } else {
                rc = xwds_device_vop_probe(dev);
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_probe;
        }

        /* add to device stack */
        rc = xwds_obj_add(ds, &dev->obj);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwds_obj_add;
        }
        dev->ds = ds;

        return XWOK;

err_xwds_obj_add:
        if ((vop) && (vop->remove)) {
                vop->remove(dev);
        } else {
                xwds_device_vop_probe(dev);
        }
err_dev_vop_probe:
        xwos_object_rawput(&dev->obj.xwobj);
err_obj_probe:
        return rc;
}

/**
 * @brief XWDS API：删除设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误发生
 * @retval -EPERM: 引用计数错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_remove(struct xwds_device * dev)
{
        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        return xwds_obj_remove(&dev->obj);
}

/**
 * @brief XWDS API：启动设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误发生
 * @retval -EPERM: 引用计数错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_start(struct xwds_device * dev)
{
        const struct xwds_virtual_operation * vop;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwds_obj_start(&dev->obj);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_start;
        }
        /* refcnt == XWDS_OBJ_REF_RUNNING */

        /* start device */
        vop = dev->vop;
        if (vop && vop->start) {
                rc = vop->start(dev);
        } else {
                rc = xwds_device_vop_start(dev);
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_start;
        }
        return XWOK;

err_dev_vop_start:
        xwds_obj_stop(&dev->obj);
err_obj_start:
        return rc;
}

/**
 * @brief XWDS API：停止设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_stop(struct xwds_device * dev)
{
        const struct xwds_virtual_operation * vop;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwds_obj_stop(&dev->obj);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_stop;
        }
        /* refcnt == XWDS_OBJ_REF_SHUTDOWN */

        /* stop device */
        vop = dev->vop;
        if (vop && vop->stop) {
                rc = vop->stop(dev);
        } else {
                rc = xwds_device_vop_stop(dev);
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_stop;
        }
        return XWOK;

err_dev_vop_stop:
        xwds_obj_start(&dev->obj);
err_obj_stop:
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief XWDS API：暂停设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_suspend(struct xwds_device * dev)
{
        const struct xwds_virtual_operation * vop;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwds_obj_suspend(&dev->obj);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_suspend;
        }
        /* refcnt == XWDS_OBJ_REF_SUSPEND */

        /* suspend device */
        vop = dev->vop;
        if (vop && vop->suspend) {
                rc = vop->suspend(dev);
        } else {
                rc = xwds_device_vop_suspend(dev);
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_suspend;
        }
        return XWOK;

err_dev_vop_suspend:
        xwds_obj_resume(&dev->obj);
err_obj_suspend:
        return rc;
}

/**
 * @brief XWDS API：继续设备
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_resume(struct xwds_device * dev)
{
        const struct xwds_virtual_operation * vop;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwds_obj_resume(&dev->obj);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_obj_resume;
        }
        /* refcnt == XWDS_OBJ_REF_RUNNING */

        /* resume device */
        vop = dev->vop;
        if (vop && vop->resume) {
                rc = vop->resume(dev);
        } else {
                rc = xwds_device_vop_resume(dev);
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_resume;
        }
        return XWOK;

err_dev_vop_resume:
        xwds_obj_suspend(&dev->obj);
err_obj_resume:
        return rc;
}

/**
 * @brief XWDS API：暂停所有设备
 * @param[in] ds: 设备栈控制块指针
 * @param[in] ign_err: 是否忽略错误：若为假，发生错误时，函数会中止并返回
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_suspend_all(struct xwds * ds, bool ign_err)
{
        struct xwds_device * c, * n;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(ds, "nullptr", -EFAULT);

        rc = XWOK;
        xwos_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        xwds_itr_prev_device_safe(ds, c, n) {
                xwos_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
                rc = xwds_device_suspend(c);
                xwos_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
                if (__xwcc_unlikely(rc < 0)) {
                        if (ign_err) {
                                rc = XWOK;
                        } else {
                                break;
                        }
                }
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}

/**
 * @brief XWDS API：继续所有设备
 * @param[in] ds: 设备栈控制块指针
 * @param[in] ign_err: 是否忽略错误：若为假，发生错误时，函数会中止并返回
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_device_resume_all(struct xwds * ds, bool ign_err)
{
        struct xwds_device * c, * n;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(ds, "nullptr", -EFAULT);

        rc = XWOK;
        xwos_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        xwds_itr_next_device_safe(ds, c, n) {
                xwos_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
                rc = xwds_device_resume(c);
                xwos_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
                if (__xwcc_unlikely(rc < 0)) {
                        if (ign_err) {
                                rc = XWOK;
                        } else {
                                break;
                        }
                }
        }
        xwos_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}
#endif
