/**
 * @file
 * @brief 玄武设备栈：电源管理框架
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
#include <xwcd/ds/object.h>
#include <xwcd/ds/device.h>
#include <xwcd/ds/xwds.h>

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
static __xwds_code
void xwds_pm_report_devices_suspending(struct xwds * ds, xwer_t rc);

static __xwds_code
void xwds_pm_report_devices_resuming(struct xwds * ds, xwer_t rc);
#endif

/**
 * @brief XWDS API：初始化设备栈
 * @param[in] ds: 设备栈指针
 */
__xwds_code
void xwds_init(struct xwds * ds)
{
        xwlib_bclst_init_node(&ds->devhead);
        xwos_sqlk_init(&ds->devlistlock);
}

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief XWDS API：暂停设备栈
 * @param[in] ds: 设备栈控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
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
 * @param[in] ds: 设备栈控制块指针
 * @param[in] rc: 错误码
 */
static __xwds_code
void xwds_pm_report_devices_suspending(struct xwds * ds, xwer_t rc)
{
        /* TODO:FIXME */
        XWOS_UNUSED(ds);
        XWDS_BUG_ON(rc < 0);
}

/**
 * @brief XWDS API：继续设备栈
 * @param[in] ds: 设备栈控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断
 * - 重入性：不可重入
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
 * @param[in] ds: 设备栈控制块指针
 * @param[in] rc: 错误码
 */
static __xwds_code
void xwds_pm_report_devices_resuming(struct xwds * ds, xwer_t rc)
{
        /* TODO:FIXME */
        XWOS_UNUSED(ds);
        XWDS_BUG_ON(rc < 0);
}
#endif
