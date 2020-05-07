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
#include <xwmd/ds/standard.h>
#include <xwmd/ds/object.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/xwds.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
static __xwds_code
void xwds_pm_report_devices_suspending(struct xwds * ds, xwer_t rc);

static __xwds_code
void xwds_pm_report_devices_resuming(struct xwds * ds, xwer_t rc);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief SODS API：初始化设备栈
 * @param ds: (I) 设备栈指针
 */
__xwds_code
void xwds_init(struct xwds * ds)
{
        xwlib_bclst_init_node(&ds->devhead);
        xwosal_sqlk_init(&ds->devlistlock);
}

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/**
 * @brief SODS API：暂停设备栈
 * @param ds: (I) 设备栈控制块指针
 * @return 错误码
 */
__xwds_api
xwer_t xwds_pm_suspend(struct xwds * ds)
{
        xwer_t rc;

        rc = xwds_device_suspend_all(ds, false);
        xwds_pm_report_devices_suspending(ds, rc);
        return rc;
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
 * @brief SODS API：继续设备栈
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

        rc = xwds_device_resume_all(ds, false);
        xwds_pm_report_devices_resuming(ds, rc);
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
#endif /* XWMDCFG_ds_PM */
