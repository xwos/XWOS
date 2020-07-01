/**
 * @file
 * @brief XuanWuOS的同步机制：信号量的虚基类
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_vsmr_h__
#define __xwos_up_sync_vsmr_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
  #include <xwos/up/sync/object.h>
#endif /* XWUPCFG_SYNC_EVT */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwsync_evt;

/**
 * @brief 信号量的虚基类
 */
/* abstract */ struct xwsync_vsmr {
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        struct xwsync_object xwsyncobj; /**< C语言面向对象：继承struct xwsync_object */
#endif /* XWUPCFG_SYNC_EVT */
        xwssq_t count; /**< 信号量计数器：如果这个成员的值<0，信号量处于负状态。*/
        xwssq_t max; /**< 信号量计数器的最大值 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 信号量的负值
 */
#define XWSDYNC_VSMR_NEGTIVE            ((xwssq_t)(-1))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwsync_vsmr_activate(struct xwsync_vsmr * smr);

__xwos_code
xwer_t xwsync_vsmr_bind(struct xwsync_vsmr * smr, struct xwsync_evt * evt,
                        xwsq_t pos);

__xwos_code
xwer_t xwsync_vsmr_unbind(struct xwsync_vsmr * smr, struct xwsync_evt * evt);

__xwos_code
xwer_t xwsync_vsmr_freeze(struct xwsync_vsmr * smr);

__xwos_code
xwer_t xwsync_vsmr_thaw(struct xwsync_vsmr * smr, xwssq_t val, xwssq_t max);

__xwos_code
xwer_t xwsync_vsmr_getvalue(struct xwsync_vsmr * smr, xwssq_t * sval);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     virtual function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/*
__xwos_api
xwer_t xwsync_vsmr_init(struct xwsync_vsmr * smr, xwssq_t val, xwssq_t max) = 0;

__xwos_api
xwer_t xwsync_vsmr_destroy(struct xwsync_vsmr * smr) = 0;

__xwos_api
xwer_t xwsync_vsmr_create(struct xwsync_vsmr ** ptrbuf, xwssq_t val, xwssq_t max) = 0;

__xwos_api
xwer_t xwsync_vsmr_delete(struct xwsync_vsmr * smr) = 0;

__xwos_api
xwer_t xwsync_vsmr_post(struct xwsync_vsmr * smr) = 0;

__xwos_api
xwer_t xwsync_vsmr_trywait(struct xwsync_vsmr * smr) = 0;

__xwos_api
xwer_t xwsync_vsmr_timedwait(struct xwsync_vsmr * smr, xwtm_t * xwtm) = 0;

__xwos_api
xwer_t xwsync_vsmr_wait_unintr(struct xwsync_vsmr * smr) = 0;
*/

#endif /* xwos/up/sync/vsmr.h */
