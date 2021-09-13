/**
 * @file
 * @brief 玄武设备栈：CAN总线接收器
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
#include <xwos/lib/xwlog.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwcd/ds/can/transceiver.h>

static __xwds_vop
xwer_t xwds_cantrcv_vop_probe(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_remove(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_start(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_stop(struct xwds_cantrcv * cantrcv);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_vop
xwer_t xwds_cantrcv_vop_suspend(struct xwds_cantrcv * cantrcv);

static __xwds_vop
xwer_t xwds_cantrcv_vop_resume(struct xwds_cantrcv * cantrcv);
#endif

__xwds_rodata const struct xwds_virtual_operation xwds_cantrcv_vop = {
        .probe = (void *)xwds_cantrcv_vop_probe,
        .remove = (void *)xwds_cantrcv_vop_remove,
        .start = (void *)xwds_cantrcv_vop_start,
        .stop = (void *)xwds_cantrcv_vop_stop,
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        .suspend = (void *)xwds_cantrcv_vop_suspend,
        .resume = (void *)xwds_cantrcv_vop_resume,
#endif
};

/******** ******** ******** constructor & destructor ******** ******** ********/
/**
 * @brief XWDS API：CAN接收器的构造函数
 * @param[in] cantrcv: CAN接收器对象指针
 */
__xwds_api
void xwds_cantrcv_construct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_construct(&cantrcv->bc.dev);
        cantrcv->bc.dev.vop = &xwds_cantrcv_vop;
}

/**
 * @brief XWDS API：CAN接收器对象的析构函数
 * @param[in] cantrcv: CAN接收器对象指针
 */
__xwds_api
void xwds_cantrcv_destruct(struct xwds_cantrcv * cantrcv)
{
        xwds_device_destruct(&cantrcv->bc.dev);
}

/******** ******** base virtual operations ******** ********/
/**
 * @brief XWDS VOP：探测CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_probe(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_probe(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：移除CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_remove(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_remove(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：启动CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_start(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE(cantrcv->cfg, "nullptr", -EFAULT);

        rc = xwds_device_vop_start(&cantrcv->bc.dev);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_dev_vop_start;
        }
        cantrcv->wkuprs = XWDS_CANTRCV_WKUPRS_NONE;
        rc = xwds_cantrcv_set_opmode(cantrcv, cantrcv->cfg->init_opmode);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_set_opmode;
        }
        return XWOK;

err_set_opmode:
        xwds_device_vop_stop(&cantrcv->bc.dev);
err_dev_vop_start:
        return rc;
}

/**
 * @brief XWDS VOP：停止CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_stop(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_vop_stop(&cantrcv->bc.dev);
        return rc;
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/******** ******** pm ******** ********/
/**
 * @brief XWDS VOP：暂停CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_suspend(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_vop_suspend(&cantrcv->bc.dev);
        return rc;
}

/**
 * @brief XWDS VOP：继续CAN接收器
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 */
static __xwds_vop
xwer_t xwds_cantrcv_vop_resume(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        rc = xwds_device_vop_resume(&cantrcv->bc.dev);
        return rc;
}
#endif

/******** ******** ******** CAN operations ******** ******** ********/
/**
 * @brief XWDS API：设置CAN接收器的运行模式
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[in] opmode: 唤醒模式
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ERANGE: 不支持的模式
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_cantrcv_set_opmode(struct xwds_cantrcv * cantrcv, xwsq_t opmode)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);
        XWDS_VALIDATE((opmode < XWDS_CANTRCV_OPMODE_NUM), "out-of-range", -ERANGE);

        drv = xwds_cast(const struct xwds_cantrcv_driver *, cantrcv->bc.dev.drv);
        if ((drv) && (drv->set_opmode)) {
                rc = drv->set_opmode(cantrcv, opmode);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_ops_set_opmode;
        }
        cantrcv->opmode = opmode;
        return XWOK;

err_ops_set_opmode:
        return rc;
}

/**
 * @brief XWDS API：设置CAN接收器的模式
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[out] opmode: 指向缓冲区的指针，通过次缓冲区返回模式
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
void xwds_cantrcv_get_opmode(struct xwds_cantrcv * cantrcv, xwsq_t * opmode)
{
        XWDS_VALIDATE(cantrcv, "nullptr");
        XWDS_VALIDATE(opmode, "nullptr");

        *opmode = cantrcv->opmode;
}

/**
 * @brief XWDS API：开启CAN接收器的唤醒
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_cantrcv_enable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;
        const struct xwds_cantrcv_cfg * cfg;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        cfg = cantrcv->cfg;
        if (cfg->wkup_supported) {
                drv = xwds_cast(const struct xwds_cantrcv_driver *,
                                cantrcv->bc.dev.drv);
                if ((drv) && (drv->enable_wkup)) {
                        rc = drv->enable_wkup(cantrcv);
                } else {
                        rc = -ENOSYS;
                }
        } else {
                rc = -EOPNOTSUPP;
        }
        return rc;
}

/**
 * @brief XWDS API：关闭CAN接收器的唤醒
 * @param[in] cantrcv: CAN接收器对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOPNOTSUPP: 不支持此API
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_cantrcv_disable_wkup(struct xwds_cantrcv * cantrcv)
{
        xwer_t rc;
        const struct xwds_cantrcv_driver * drv;
        const struct xwds_cantrcv_cfg * cfg;

        XWDS_VALIDATE(cantrcv, "nullptr", -EFAULT);

        cfg = cantrcv->cfg;
        if (cfg->wkup_supported) {
                drv = xwds_cast(const struct xwds_cantrcv_driver *,
                                cantrcv->bc.dev.drv);
                if ((drv) && (drv->disable_wkup)) {
                        rc = drv->disable_wkup(cantrcv);
                } else {
                        rc = -ENOSYS;
                }
        } else {
                rc = -EOPNOTSUPP;
        }
        return rc;
}

/**
 * @brief XWDS API：设置唤醒中断的回调函数
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[in] isr: 回调函数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 * - 必须调用xwds_cantrcv_disable_wkup()后才可修改回调函数
 */
__xwds_api
void xwds_cantrcv_set_wkup_isr(struct xwds_cantrcv * cantrcv,
                               xwds_cantrcv_wkup_isr_f isr)
{
        cantrcv->wkup_isr = isr;
}

/**
 * @brief XWDS API：获取CAN接收器的唤醒原因
 * @param[in] cantrcv: CAN接收器对象指针
 * @param[out] wkuprs: 指向缓冲区的指针，通过次缓冲区返回唤醒原因
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwds_api
void xwds_cantrcv_get_wkuprs(struct xwds_cantrcv * cantrcv, xwsq_t * wkuprs)
{
        const struct xwds_cantrcv_cfg * cfg;

        XWDS_VALIDATE(cantrcv, "nullptr");
        XWDS_VALIDATE(wkuprs, "nullptr");

        cfg = cantrcv->cfg;
        if (cfg->wkup_supported) {
                *wkuprs = cantrcv->wkuprs;
        } else {
                *wkuprs = XWDS_CANTRCV_WKUPRS_NOT_SUPPORTED;
        }
}

/**
 * @brief XWDS API：清除CAN接收器的唤醒原因
 * @param[in] cantrcv: CAN接收器对象指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
void xwds_cantrcv_clear_wkuprs(struct xwds_cantrcv * cantrcv)
{
        XWDS_VALIDATE(cantrcv, "nullptr");

        cantrcv->wkuprs = XWDS_CANTRCV_WKUPRS_NONE;
}

/**
 * @brief CAN接收器回调函数：CAN接收器的唤醒通知
 * @param[in] cantrcv: CAN接收器对象指针
 * @note
 * - 这个回调函数在中断上下文中被调用，用于通知唤醒
 */
__xwds_code
void xwds_cantrcv_drvcb_wakeup_notification(struct xwds_cantrcv * cantrcv)
{
        if (cantrcv->wkup_isr) {
                cantrcv->wkup_isr(cantrcv, cantrcv->canc);
        }/* else {} */
}
