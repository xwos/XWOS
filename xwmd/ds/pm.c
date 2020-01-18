/**
 * @file
 * @brief xwmd设备栈：电源管理框架
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
#include <xwos/smp/pm.h>
#include <xwmd/ds/object.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/pm.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwds_code
void xwds_pm_lpmntf(struct xwos_pmdm * pmdm, void * data);

static __xwds_code
void xwds_pm_report_devices_suspending(struct xwds * ds, xwer_t rc);

static __xwds_code
void xwds_pm_report_devices_resuming(struct xwds * ds, xwer_t rc);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SODS API：初始化电源管理控制块
 * @param ds: (I) 设备栈控制块指针
 * @param xwpmdm: (I) XWOS电源管理领域控制块指针
 * @param suspend: (I) 进入低功耗的回调函数
 * @param resume: (I) 退出低功耗的回调函数
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwds_api
xwer_t xwds_pm_init(struct xwds *ds, struct xwos_pmdm * xwpmdm,
                    xwds_suspend_f suspend, xwds_resume_f resume)
{
        SODS_VALIDATE(ds, "nullptr", -EFAULT);
        SODS_VALIDATE(xwpmdm, "nullptr", -EFAULT);

        ds->xwpmdm = xwpmdm;
        ds->lpm.suspend = suspend;
        ds->lpm.resume = resume;
        xwos_pmdm_set_lpmntf_cb(ds->xwpmdm,
                                xwds_pm_lpmntf,
                                ds);
        return OK;
}

/**
 * @brief SODS API：进入低功耗模式
 * @param ds: (I) 设备栈控制块指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_pm_suspend(struct xwds * ds)
{
        return xwos_pmdm_suspend(ds->xwpmdm);
}

/**
 * @brief XWOS电源管理领域中的所有调度器进入暂停模式时的回调函数
 * @param xwpmdm: (I) XWOS电源管理领域控制块指针
 * @param arg: (I) 回调函数参数（设备栈控制块指针）
 */
static __xwds_code
void xwds_pm_lpmntf(struct xwos_pmdm * xwpmdm, void * arg)
{
        struct xwds * ds;
        xwer_t rc;

        ds = arg;
        if (xwpmdm == ds->xwpmdm) {
                rc = xwds_device_suspend_all(ds, false);
                if (OK == rc) {
                        if (!is_err_or_null(ds->lpm.suspend)) {
                                ds->lpm.suspend(ds);
                        }/* else {} */
                } else {
                        xwds_pm_report_devices_suspending(ds, rc);
                }
        }/* else {} */
}

/**
 * @brief 报告设备暂停操作异常
 * @param ds: (I) 设备栈控制块指针
 * @param rc: (I) 错误码
 */
static __xwds_code
void xwds_pm_report_devices_suspending(struct xwds * ds, xwer_t rc)
{
        /* TODO:FIXME */
        XWOS_UNUSED(ds);
        SODS_BUG_ON(rc < 0);
}

/**
 * @brief SODS API：退出低功耗模式
 * @param ds: (I) 设备栈控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 中断上下文：可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：不可以使用
 */
__xwds_api
xwer_t xwds_pm_resume(struct xwds * ds)
{
        xwer_t rc;


        if (!is_err_or_null(ds->lpm.resume)) {
                ds->lpm.resume(ds);
        }
        rc = xwds_device_resume_all(ds, false);
        if (__unlikely(rc < 0)) {
                xwds_pm_report_devices_resuming(ds, rc);
                goto err_xwds_device_resume_all;
        }
        rc = xwos_pmdm_resume(ds->xwpmdm);
        if (__unlikely(rc < 0)) {
                goto err_xwos_pmdm_resume;
        }
        return OK;

err_xwos_pmdm_resume:
err_xwds_device_resume_all:
        return rc;
}


/**
 * @brief 报告设备继续操作异常
 * @param ds: (I) 设备栈控制块指针
 * @param rc: (I) 错误码
 */
static __xwds_code
void xwds_pm_report_devices_resuming(struct xwds * ds, xwer_t rc)
{
        /* TODO:FIXME */
        XWOS_UNUSED(ds);
        SODS_BUG_ON(rc < 0);
}
