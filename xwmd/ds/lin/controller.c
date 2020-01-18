/**
 * @file
 * @brief xwmd设备栈：LIN控制器
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
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/lin/controller.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_vop
xwer_t xwds_linc_cvop_probe(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_remove(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_start(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_stop(struct xwds_linc * linc);

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
static __xwds_vop
xwer_t xwds_linc_cvop_suspend(struct xwds_linc * linc);

static __xwds_vop
xwer_t xwds_linc_cvop_resume(struct xwds_linc * linc);
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
__xwds_rodata const struct xwds_base_virtual_operations xwds_linc_cvops = {
        .probe = (void *)xwds_linc_cvop_probe,
        .remove = (void *)xwds_linc_cvop_remove,
        .start = (void *)xwds_linc_cvop_start,
        .stop = (void *)xwds_linc_cvop_stop,
#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
        .suspend = (void *)xwds_linc_cvop_suspend,
        .resume = (void *)xwds_linc_cvop_resume,
#endif /* XWMDCFG_ds_LPM */
};
#endif /* !XWMDCFG_ds_NANO */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief LIN控制器的构造函数
 * @param linc: (I) LIN控制器对象指针
 */
__xwds_code
void xwds_linc_construct(struct xwds_linc * linc)
{
        xwds_device_construct(&linc->dev);
        linc->dev.cvops = &xwds_linc_cvops;
}

/**
 * @brief LIN控制器对象的析构函数
 * @param linc: (I) LIN控制器对象指针
 */
__xwds_code
void xwds_linc_destruct(struct xwds_linc * linc)
{
        xwds_device_destruct(&linc->dev);
}
#endif /* !XWMDCFG_ds_NANO */

/******** ******** base virtual operations ******** ********/
/**
 * @brief SODS VOP：探测LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
static __xwds_vop
xwer_t xwds_linc_cvop_probe(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwosal_mtx_init(&linc->txlock, XWOSAL_SD_PRIORITY_RT_MIN);
        if (__unlikely(rc < 0)) {
                goto err_mtx_init;
        }
        if (is_err_or_null(linc->get_msg_size)) {
                linc->get_msg_size = xwds_linc_get_msg_size;
        }
        rc = xwds_device_cvop_probe(&linc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_probe;
        }
        return OK;

err_dev_cvop_probe:
        xwosal_mtx_destroy(&linc->txlock);
err_mtx_init:
        return rc;
}

/**
 * @brief SODS VOP：移除LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
static __xwds_vop
xwer_t xwds_linc_cvop_remove(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_remove(&linc->dev);
        if (__unlikely(rc < 0)) {
                goto err_dev_cvop_remove;
        }
        xwosal_mtx_destroy(&linc->txlock);
        return OK;

err_dev_cvop_remove:
        return rc;
}

/**
 * @brief SODS VOP：启动LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
static __xwds_vop
xwer_t xwds_linc_cvop_start(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_start(&linc->dev);
        return rc;
}

/**
 * @brief SODS VOP：停止LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
static __xwds_vop
xwer_t xwds_linc_cvop_stop(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_stop(&linc->dev);
        return rc;
}

#if defined(XWMDCFG_ds_LPM) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS VOP：暂停LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
static __xwds_vop
xwer_t xwds_linc_cvop_suspend(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_suspend(&linc->dev);
        return rc;
}

/**
 * @brief SODS VOP：继续LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
static __xwds_vop
xwer_t xwds_linc_cvop_resume(struct xwds_linc * linc)
{
        xwer_t rc;

        rc = xwds_device_cvop_resume(&linc->dev);
        return rc;
}
#endif /* XWMDCFG_ds_LPM */

/******** ******** ******** APIs ******** ******** ********/
#if defined(XWMDCFG_ds_NANO) && (1 == XWMDCFG_ds_NANO)
/**
 * @brief SODS API：探测LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
xwer_t xwds_linc_probe(struct xwds_linc * linc)
{
        xwer_t rc;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);

        rc = xwds_linc_cvop_probe(linc);
        return rc;
}

/**
 * @brief SODS API：移除LIN控制器
 * @param ds: (I) 设备栈控制块指针
 * @param linc: (I) LIN控制器对象指针
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
xwer_t xwds_linc_remove(struct xwds_linc * linc)
{
        xwer_t rc;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);

        rc = xwds_linc_cvop_remove(linc);
        return rc;
}

/**
 * @brief SODS API：启动LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
xwer_t xwds_linc_start(struct xwds_linc * linc)
{
        xwer_t rc;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);

        rc = xwds_linc_cvop_start(linc);
        return rc;
}

/**
 * @brief SODS API：停止LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
xwer_t xwds_linc_stop(struct xwds_linc * linc)
{
        xwer_t rc;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);

        rc = xwds_linc_cvop_stop(linc);
        return rc;
}

#if (defined(XWMDCFG_ds_LPM)) && (1 == XWMDCFG_ds_LPM)
/******** ******** pm ******** ********/
/**
 * @brief SODS API：暂停LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
xwer_t xwds_linc_suspend(struct xwds_linc * linc)
{
        xwer_t rc;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);

        rc = xwds_linc_cvop_suspend(linc);
        return rc;
}

/**
 * @brief SODS API：继续LIN控制器
 * @param linc: (I) LIN控制器对象指针
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
xwer_t xwds_linc_resume(struct xwds_linc * linc)
{
        xwer_t rc;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);

        rc = xwds_linc_cvop_resume(linc);
        return rc;
}

#endif /* XWMDCFG_ds_LPM */
#endif /* XWMDCFG_ds_NANO */

/**
 * @brief SODS API：主机节点发送一条LIN消息
 * @param linc: (I) LIN控制器对象指针
 * @param id: (I) 主机节点调度消息的ID
 * @param msg: (I) LIN消息结构体指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 控制器不支持主机模式发送
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_linc_msttx(struct xwds_linc * linc,
                       xwu8_t id, struct xwds_lin_msg * msg,
                       xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);
        SODS_VALIDATE(msg, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_linc_grab(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_grab;
        }

        rc = xwds_linc_request(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_request;
        }
#endif

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&linc->txlock), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_linc_txlock;
        }
        drv = xwds_static_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if ((drv) && (drv->msttx)) {
                rc = drv->msttx(linc, id, msg, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_msttx;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_linc_release(linc);
        xwds_linc_put(linc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_msttx:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));
err_linc_txlock:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_linc_release(linc);
err_linc_request:
        xwds_linc_put(linc);
err_linc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：从机节点发送一条LIN消息
 * @param linc: (I) LIN控制器对象指针
 * @param msg: (I) LIN消息结构体指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 控制器不支持从机模式发送
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_linc_slvtx(struct xwds_linc * linc,
                       struct xwds_lin_msg * msg,
                       xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);
        SODS_VALIDATE(msg, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_linc_grab(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_grab;
        }
        rc = xwds_linc_request(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_request;
        }
#endif /* !XWMDCFG_ds_NANO */

        rc = xwosal_mtx_timedlock(xwosal_mtx_get_id(&linc->txlock), xwtm);
        if (__unlikely(rc < 0)) {
                goto err_linc_txlock;
        }
        drv = xwds_static_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if (__likely((drv) && (drv->slvtx))) {
                rc = drv->slvtx(linc, msg, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_slvtx;
        }
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_linc_release(linc);
        xwds_linc_put(linc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_slvtx:
        xwosal_mtx_unlock(xwosal_mtx_get_id(&linc->txlock));
err_linc_txlock:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_linc_release(linc);
err_linc_request:
        xwds_linc_put(linc);
err_linc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：接收一条LIN消息
 * @param linc: (I) LIN控制器对象指针
 * @param msgbuf: 接收消息的缓冲区的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENOSYS: 控制器不支持接收
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个设备不可重入；对于不同设备可重入
 */
__xwds_api
xwer_t xwds_linc_rx(struct xwds_linc * linc,
                    struct xwds_lin_msg * msgbuf,
                    xwtm_t * xwtm)
{
        xwer_t rc;
        const struct xwds_linc_driver * drv;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);
        SODS_VALIDATE(msgbuf, "nullptr", -EFAULT);
        SODS_VALIDATE(xwtm, "nullptr", -EFAULT);

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        rc = xwds_linc_grab(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_grab;
        }
        rc = xwds_linc_request(linc);
        if (__unlikely(rc < 0)) {
                goto err_linc_request;
        }
#endif /* !XWMDCFG_ds_NANO */

        drv = xwds_static_cast(const struct xwds_linc_driver *, linc->dev.drv);
        if (__likely((drv) && (drv->rx))) {
                rc = drv->rx(linc, msgbuf, xwtm);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_rx;
        }

#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_linc_release(linc);
        xwds_linc_put(linc);
#endif /* !XWMDCFG_ds_NANO */

        return OK;

err_drv_rx:
#if !defined(XWMDCFG_ds_NANO) || (1 != XWMDCFG_ds_NANO)
        xwds_linc_release(linc);
err_linc_request:
        xwds_linc_put(linc);
err_linc_grab:
#endif /* !XWMDCFG_ds_NANO */
        return rc;
}

/**
 * @brief SODS API：通过LIN保护ID查询消息大小
 * @param linc: (I) LIN控制器对象指针
 * @param protected_id: (I) 消息的LIN保护ID
 * @param ret: (O) 指向缓冲区的指针，通过此缓冲区返回消息大小
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ENODEV: 找不到ID
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：可重入
 */
__xwds_api
xwer_t xwds_linc_get_msg_size(struct xwds_linc * linc,
                              xwu8_t protected_id,
                              xwu8_t * ret)
{
        const struct xwds_lin_msg_info * msgitbl;
        xwsz_t itemnum;
        xwer_t rc;
        xwsq_t i;

        SODS_VALIDATE(linc, "nullptr", -EFAULT);
        SODS_VALIDATE(ret, "nullptr", -EFAULT);

        msgitbl = linc->msgitable;
        itemnum = linc->msgitable_itemnum;
        rc = -ENODEV;
        if (!is_err_or_null(msgitbl)) {
                for (i = 0; i < itemnum; i++) {
                        if (protected_id == msgitbl[i].protected_id) {
                                *ret = msgitbl[i].size;
                                rc = OK;
                                break;
                        }
                }
                if (-ENODEV == rc) {
                        *ret = SODS_LIN_DEFAULT_DATA_SIZE;
                }
        } else {
                rc = OK;
                *ret = SODS_LIN_DEFAULT_DATA_SIZE;
        }
        return rc;
}
