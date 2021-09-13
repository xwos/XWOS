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

#ifndef __xwcd_ds_xwds_h__
#define __xwcd_ds_xwds_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>

/**
 * @brief 设备栈
 */
struct xwds {
        struct xwlib_bclst_head devhead; /**< 链表头 */
        struct xwos_sqlk devlistlock; /**< 保护设备栈链表的锁 */
};

void xwds_init(struct xwds * ds);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_pm_suspend(struct xwds * ds);
xwer_t xwds_pm_resume(struct xwds * ds);
#endif

#endif /* xwcd/ds/xwds.h */
