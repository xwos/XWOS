/**
 * @file
 * @brief xwmd硬件抽象层：SOC外部中断
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
#include <xwmd/ds/soc/gpio.h>
#include <xwmd/ds/soc/eirq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SODS API：申请外部中断
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口
 * @param pinmask: (I) GPIO PIN
 * @param eiid: (I) 外部中断ID
 * @param eiflag: (I) 触发标志
 * @param isr: (I) 中断响应函数
 * @param arg: (I) 中断响应函数参数
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个外部中断不可重入；对于不同外部中断可重入
 * - 移植：SOC的EIRQ ISR需要判断(!is_err_or_null(soc->eirq.isrs[x]))才可调用
 *   soc->eirq.isrs[x]
 */
__xwds_api
xwer_t xwds_eirq_req(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask,
                     xwid_t eiid, xwsq_t eiflag,
                     xwds_eirq_f isr, xwds_eirq_arg_t arg)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((eiid < soc->eirq.num), "out-of-range", -ERANGE);

        rc = xwds_soc_grab(soc);
        if (__unlikely(rc < 0)) {
                goto err_soc_grab;
        }

#if defined(XWMDCFG_ds_SOC_EIRQ_ROISRT) && (1 == XWMDCFG_ds_SOC_EIRQ_ROISRT)
        XWOS_UNUSED(isr);
        XWOS_UNUSED(arg);
        if (__unlikely(NULL == soc->eirq.isrs[eiid])) {
                rc = -EPERM;
                goto err_perm;
        }
#else
        if (__unlikely(soc->eirq.isrs[eiid])) {
                rc = -EBUSY;
                goto err_perm;
        }
        soc->eirq.isrs[eiid] = isr ? isr : err_ptr(-EBUSY);
        soc->eirq.isrargs[eiid] = arg;
#endif

        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eirq_req)) {
                rc = drv->eirq_req(soc, port, pinmask, eiid, eiflag);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_eirq_req;
        }
        return OK;

err_drv_eirq_req:
#if !defined(XWMDCFG_ds_SOC_EIRQ_ROISRT) || (1 != XWMDCFG_ds_SOC_EIRQ_ROISRT)
        soc->eirq.isrs[eiid] = NULL;
        soc->eirq.isrargs[eiid] = NULL;
#endif
err_perm:
        xwds_soc_put(soc);
err_soc_grab:
        return rc;
}

/**
 * @brief SODS API：释放外部中断
 * @param soc: (I) SOC对象指针
 * @param port: (I) GPIO端口
 * @param pinmask: (I) GPIO PIN
 * @param eiid: (I) 外部中断ID
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 无效指针
 * @retval -ERANGE: 外部中断ID错误
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * - 重入性：对于同一个外部中断不可重入；对于不同外部中断可重入
 */
__xwds_api
xwer_t xwds_eirq_rls(struct xwds_soc * soc, xwid_t port, xwsq_t pinmask, xwid_t eiid)
{
        const struct xwds_soc_driver * drv;
        xwer_t rc;

        SODS_VALIDATE(soc, "nullptr", -EFAULT);
        SODS_VALIDATE((eiid < soc->eirq.num), "out-of-range", -ERANGE);

        if (__unlikely(NULL == soc->eirq.isrs[eiid])) {
                rc = -EPERM;
                goto err_notinused;
        }
        drv = xwds_static_cast(const struct xwds_soc_driver *, soc->dev.drv);
        if ((drv) && (drv->eirq_rls)) {
                rc = drv->eirq_rls(soc, port, pinmask, eiid);
        } else {
                rc = -ENOSYS;
        }
        if (__unlikely(rc < 0)) {
                goto err_drv_rlsei;
        }
#if !defined(XWMDCFG_ds_SOC_EIRQ_ROISRT) || (1 != XWMDCFG_ds_SOC_EIRQ_ROISRT)
        soc->eirq.isrs[eiid] = NULL;
        soc->eirq.isrargs[eiid] = NULL;
#endif

        xwds_soc_put(soc);
        return OK;

err_drv_rlsei:
err_notinused:
        return rc;
}
