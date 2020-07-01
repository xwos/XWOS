/**
 * @file
 * @brief xwmd设备栈：电源管理框架
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_xwds_h__
#define __xwmd_ds_xwds_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 设备栈
 */
struct xwds {
        struct xwlib_bclst_head devhead; /**< 链表头 */
        struct xwosal_sqlk devlistlock; /**< 保护设备栈链表的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWDS API：初始化设备栈
 * @param ds: (I) 设备栈指针
 */
__xwds_api
void xwds_init(struct xwds * ds);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
/**
 * @brief XWDS API：暂停设备栈
 * @param ds: (I) 设备栈控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_pm_suspend(struct xwds * ds);

/**
 * @brief XWDS API：继续设备栈
 * @param ds: (I) 设备栈控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 上下文：中断
 * - 重入性：不可重入
 */
__xwds_api
xwer_t xwds_pm_resume(struct xwds * ds);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

#endif /* xwmd/ds/xwds.h */
