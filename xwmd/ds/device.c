/**
 * @file
 * @brief xwmd设备栈：设备基类
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
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/xwds.h>
#include <xwmd/ds/object.h>
#include <xwmd/ds/device.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 实现面向对象“多态”的函数表
 */
__xwds_rodata const struct xwds_base_virtual_operations xwds_dev_cvops = {
        .probe = xwds_device_cvop_probe,
        .remove = xwds_device_cvop_remove,
        .start = xwds_device_cvop_start,
        .stop = xwds_device_cvop_stop,
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        .suspend = xwds_device_cvop_suspend,
        .resume = xwds_device_cvop_resume,
#endif /* XWMDCFG_ds_PM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 通过描述得到寄存器资源
 * @param base: (I) 寄存器资源数组的基地址
 * @param num: (I) 寄存器资源数量
 * @param descay: (I) 寄存器描述数组
 * @param descnum: (I) 寄存器描述数组数量
 * @param ret: (O) 返回寄存器资源地址的缓存
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOKEY: 找不到描述的资源
 */
__xwds_code
xwer_t xwds_get_regrsc(const struct xwds_resource_reg base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_reg ** ret)
{
        xwer_t rc;
        xwsz_t i, m;

        for (i = 0; i < num; i++) {
                rc = OK;
                for (m = 0; m < descnum; m++) {
                        if (!strstr(base[i].description, descay[m])) {
                                rc = -ENOKEY;
                                break;
                        }
                }
                if (OK == rc) {
                        break;
                }
        }
        if (OK == rc) {
                *ret = &base[i];
        } else {
                *ret = NULL;
        }
        return rc;
}

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief 设备的构造函数
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_code
void xwds_device_construct(struct xwds_device * dev)
{
        xwds_obj_construct(&dev->obj);
        dev->state = XWDS_DEVICE_STATE_INVALID;
        dev->cvops = &xwds_dev_cvops;
        dev->ds = NULL;
}

/**
 * @brief 设备的析构函数
 * @param dev: (I) 设备对象的指针
 */
__xwds_code
void xwds_device_destruct(struct xwds_device * dev)
{
        xwds_obj_destruct(&dev->obj);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief 设备基本操作函数：探测设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_cvop_probe(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->probe)) {
                rc = drv->probe(dev);
        } else {
                rc = OK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：删除设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_cvop_remove(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->remove)) {
                rc = drv->remove(dev);
        } else {
                rc = OK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：启动设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_cvop_start(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->start)) {
                rc = drv->start(dev);
        } else {
                rc = OK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：停止设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_cvop_stop(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->stop)) {
                rc = drv->stop(dev);
        } else {
                rc = OK;
        }
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/**
 * @brief 设备基本操作函数：暂停设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_cvop_suspend(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->suspend)) {
                rc = drv->suspend(dev);
        } else {
                rc = OK;
        }
        return rc;
}

/**
 * @brief 设备基本操作函数：继续设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_vop
xwer_t xwds_device_cvop_resume(struct xwds_device * dev)
{
        const struct xwds_driver * drv;
        xwer_t rc;

        drv = dev->drv;
        if ((drv) && (drv->resume)) {
                rc = drv->resume(dev);
        } else {
                rc = OK;
        }
        return rc;
}
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：探测设备
 * @param ds: (I) 设备栈控制块指针
 * @param dev: (I) 设备对象的指针
 * @param gcfunc: (I) 垃圾回收函数
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_probe(struct xwds * ds, struct xwds_device * dev,
                         xwobj_gc_f gcfunc)
{
        const struct xwds_base_virtual_operations * cvops;
        xwer_t rc;

        XWDS_VALIDATE(ds, "nullptr", -EFAULT);
        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwds_obj_activate(&dev->obj, gcfunc);
        if (__unlikely(rc < 0)) {
                goto err_dev_activate;
        }

        rc = xwaop_teq_then_write(xwsq_t, &dev->state,
                                  XWDS_DEVICE_STATE_INVALID,
                                  XWDS_DEVICE_STATE_PROBING,
                                  NULL);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_dev_set_state;
        }

        /* probe device */
        cvops = dev->cvops;
        if (cvops && cvops->probe) {
                rc = cvops->probe(dev);
        } else {
                rc = xwds_device_cvop_probe(dev);
        }
        if (__unlikely(rc < 0)) {
                goto err_dev_cvops_probe;
        }

        /* add to device stack */
        rc = xwds_obj_add(ds, &dev->obj);
        if (__unlikely(rc < 0)) {
                goto err_xwds_obj_add;
        }
        dev->ds = ds;

        /* set device state */
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_STOPED, NULL);
        return OK;

err_xwds_obj_add:
        if ((cvops) && (cvops->remove)) {
                cvops->remove(dev);
        } else {
                xwds_device_cvop_probe(dev);
        }
err_dev_cvops_probe:
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_INVALID, NULL);
err_dev_set_state:
        xwds_device_put(dev);
err_dev_activate:
        return rc;
}

/**
 * @brief XWDS API：删除设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_remove(struct xwds_device * dev)
{
        struct xwds * ds;
        const struct xwds_base_virtual_operations * cvops;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwaop_teq_then_write(xwsq_t, &dev->state,
                                  XWDS_DEVICE_STATE_STOPED,
                                  XWDS_DEVICE_STATE_REMOVING,
                                  NULL);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_dev_set_state;
        }

        ds = dev->ds;
        if (ds) {
                rc = xwds_obj_remove(ds, &dev->obj);
        } else {
                rc = -EOWNER;
        }
        if (__unlikely(rc < 0)) {
                goto err_xwds_obj_remove;
        }
        dev->ds = NULL;

        /* remove device */
        cvops = dev->cvops;
        if (cvops && cvops->remove) {
                rc = cvops->remove(dev);
        } else {
                rc = xwds_device_cvop_remove(dev);
        }
        if (__unlikely(rc < 0)) {
                goto err_dev_cvops_remove;
        }

        /* set device state */
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_INVALID, NULL);
        xwds_device_put(dev);
        return OK;

err_dev_cvops_remove:
        xwds_obj_add(ds, &dev->obj);
        dev->ds = ds;
err_xwds_obj_remove:
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_STOPED, NULL);
err_dev_set_state:
        return rc;
}

/**
 * @brief XWDS API：启动设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_start(struct xwds_device * dev)
{
        const struct xwds_base_virtual_operations * cvops;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwds_device_grab(dev);
        if (__unlikely(rc < 0)) {
                rc = -EOWNERDEAD;
                goto err_dev_grab;
        }
        rc = xwaop_teq_then_write(xwsq_t, &dev->state,
                                  XWDS_DEVICE_STATE_STOPED,
                                  XWDS_DEVICE_STATE_STARTING,
                                  NULL);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_dev_set_state;
        }

        /* start device */
        cvops = dev->cvops;
        if (cvops && cvops->start) {
                rc = cvops->start(dev);
        } else {
                rc = xwds_device_cvop_start(dev);
        }
        if (__unlikely(rc < 0)) {
                goto err_dev_cvops_start;
        }

        /* set device state */
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_RUNNING, NULL);
        return OK;

err_dev_cvops_start:
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_STOPED, NULL);
err_dev_set_state:
        xwds_device_put(dev);
err_dev_grab:
        return rc;
}

/**
 * @brief XWDS API：停止设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_stop(struct xwds_device * dev)
{
        const struct xwds_base_virtual_operations * cvops;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwaop_teq_then_write(xwsq_t, &dev->state,
                                  XWDS_DEVICE_STATE_RUNNING,
                                  XWDS_DEVICE_STATE_STOPING,
                                  NULL);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_dev_set_state;
        }

        /* The device must has no child in running state or suspending state.
           So the refence count must be 2. */
        if (2 == xwds_device_get_refcnt(dev)) {
                rc = -EBUSY;
                goto err_dev_refcnt;
        }

        /* stop device */
        cvops = dev->cvops;
        if ((cvops) && (cvops->stop)) {
                rc = cvops->stop(dev);
        } else {
                rc = xwds_device_cvop_stop(dev);
        }
        if (__unlikely(rc < 0)) {
                goto err_dev_cvops_stop;
        }

        /* set device state */
        xwds_device_put(dev);
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_STOPED, NULL);
        return OK;

err_dev_cvops_stop:
err_dev_refcnt:
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_RUNNING, NULL);
err_dev_set_state:
        return rc;
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/**
 * @brief XWDS API：暂停设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_suspend(struct xwds_device * dev)
{
        const struct xwds_base_virtual_operations * cvops;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwaop_teq_then_write(xwsq_t, &dev->state,
                                  XWDS_DEVICE_STATE_RUNNING,
                                  XWDS_DEVICE_STATE_SUSPENDING,
                                  NULL);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_dev_set_state;
        }

        /* suspend device */
        cvops = dev->cvops;
        if (cvops && cvops->suspend) {
                rc = cvops->suspend(dev);
        } else {
                rc = xwds_device_cvop_suspend(dev);
        }
        if (__unlikely(rc < 0)) {
                goto err_cvops_suspend;
        }

        /* set device state */
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_SUSPENDED, NULL);
        return OK;

err_cvops_suspend:
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_RUNNING, NULL);
err_dev_set_state:
        return rc;
}

/**
 * @brief XWDS API：继续设备
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_resume(struct xwds_device * dev)
{

        const struct xwds_base_virtual_operations * cvops;
        xwer_t rc;

        XWDS_VALIDATE(dev, "nullptr", -EFAULT);

        rc = xwaop_teq_then_write(xwsq_t, &dev->state,
                                  XWDS_DEVICE_STATE_SUSPENDED,
                                  XWDS_DEVICE_STATE_RESUMING,
                                  NULL);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
                goto err_set_state;
        }

        /* resume device */
        cvops = dev->cvops;
        if (cvops && cvops->resume) {
                rc = cvops->resume(dev);
        } else {
                rc = xwds_device_cvop_resume(dev);
        }
        if (__unlikely(rc < 0)) {
                goto err_cvops_resume;
        }

        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_RUNNING, NULL);
        return OK;

err_cvops_resume:
        xwaop_write(xwsq_t, &dev->state, XWDS_DEVICE_STATE_SUSPENDED, NULL);
err_set_state:
        return rc;
}

/**
 * @brief XWDS API：暂停所有设备
 * @param ds: (I) 设备栈控制块指针
 * @param ign_err: (I) 是否忽略错误：若为假，发生错误时，函数会中止并返回
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_suspend_all(struct xwds * ds, bool ign_err)
{
        struct xwds_device * c, * n;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(ds, "nullptr", -EFAULT);

        rc = OK;
        xwosal_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        xwds_itr_prev_device_safe(ds, c, n) {
                xwosal_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
                rc = xwds_device_suspend(c);
                xwosal_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
                if (__unlikely(rc < 0)) {
                        xwisrlogf(ERR, XWDS_LOG_TAG,
                                  "Failed to suspend device: <%s:%d>, rc: %d",
                                  c->name, c->id, rc);
                        if (ign_err) {
                                rc = OK;
                        } else {
                                break;
                        }
                }
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}

/**
 * @brief XWDS API：继续所有设备
 * @param ds: (I) 设备栈控制块指针
 * @param ign_err: (I) 是否忽略错误：若为假，发生错误时，函数会中止并返回
 * @return 错误码
 */
__xwds_api
xwer_t xwds_device_resume_all(struct xwds * ds, bool ign_err)
{
        struct xwds_device * c, * n;
        xwreg_t cpuirq;
        xwer_t rc;

        XWDS_VALIDATE(ds, "nullptr", -EFAULT);

        rc = OK;
        xwosal_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
        xwds_itr_next_device_safe(ds, c, n) {
                xwosal_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
                rc = xwds_device_resume(c);
                xwosal_sqlk_wr_lock_cpuirqsv(&ds->devlistlock, &cpuirq);
                if (__unlikely(rc < 0)) {
                        xwisrlogf(ERR, XWDS_LOG_TAG,
                                  "Failed to resume device: <%s:%d>, rc: %d",
                                  c->name, c->id, rc);
                        if (ign_err) {
                                rc = OK;
                        } else {
                                break;
                        }
                }
        }
        xwosal_sqlk_wr_unlock_cpuirqrs(&ds->devlistlock, cpuirq);
        return rc;
}
#endif /* XWMDCFG_ds_PM */
