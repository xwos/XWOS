/**
 * @file
 * @brief 玄武设备栈：电源管理框架
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __xwcd_ds_xwds_h__
#define __xwcd_ds_xwds_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>

/**
 * @defgroup xwcd_ds XWOS的设备栈
 * @ingroup xwcd
 * @{
 */

/**
 * @brief 设备栈
 */
struct xwds {
        struct xwlib_bclst_head devhead; /**< 链表头 */
        struct xwos_sqlk devlistlock; /**< 保护设备栈链表的锁 */
};

/**
 * @brief XWDS API：初始化设备栈
 * @param[in] ds: 设备栈指针
 */
void xwds_init(struct xwds * ds);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
/**
 * @brief XWDS API：暂停设备栈
 * @param[in] ds: 设备栈控制块指针
 * @return 错误码
 * @note
 * + 上下文：中断、中断底半部、线程
 */
xwer_t xwds_pm_suspend(struct xwds * ds);

/**
 * @brief XWDS API：继续设备栈
 * @param[in] ds: 设备栈控制块指针
 * @return 错误码
 * @note
 * + 上下文：中断
 */
xwer_t xwds_pm_resume(struct xwds * ds);
#endif

/**
 * @} xwcd_ds
 */

#endif /* xwcd/ds/xwds.h */
