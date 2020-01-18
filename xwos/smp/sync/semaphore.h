/**
 * @file
 * @brief XuanWuOS的同步机制：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_sync_semaphore_h__
#define __xwos_smp_sync_semaphore_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/smp/rtwq.h>
#include <xwos/smp/plwq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwsync_evt;

/**
 * @brief 信号量类型枚举
 */
enum xwsync_smr_type_em {
        XWSYNC_SMR_TYPE_UNKNOWN = 0,

#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
        XWSYNC_SMR_TYPE_PIPELINE, /**< 管道信号量：先进先出 */
#endif /* XWSMPCFG_SYNC_PLSMR */

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
        XWSYNC_SMR_TYPE_RT, /**< 实时信号量：高优先级先出 */
#endif /* XWSMPCFG_SYNC_RTSMR */
};

/**
 * @brief 信号量对象
 */
struct xwsync_smr {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        xwid_t type; /**< 类型 */
        xwssq_t count; /**< 信号量计数器：<0，信号量处于负状态。*/
        xwssq_t max; /**< 信号量计数器的最大值 */
        union {
#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
                struct xwos_rtwq rt; /**< 实时信号量的等待队列 */
#endif /* XWSMPCFG_SYNC_RTSMR */
#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
                struct xwos_plwq pl; /**< 管道信号量的等待队列 */
#endif /* XWSMPCFG_SYNC_PLSMR */
        } wq; /**< 等待队列 */
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
        struct {
                struct xwsync_evt * evt; /**< 事件对象 */
                xwsq_t pos; /**< 事件对象中的位图位置 */
        } selector; /**< 选择器 */
#endif /* XWSMPCFG_SYNC_EVT */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 负信号量的值
 */
#define XWSYNC_SMR_NEGTIVE              ((xwssq_t)(-1))

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
  #define XWSYNC_SMR_API(api, ...) xwsync_rtsmr_##api(__VA_ARGS__)
#elif defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
  #define XWSYNC_SMR_API(api, ...) xwsync_plsmr_##api(__VA_ARGS__)
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param smr: (I) 信号量对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_smr_grab(struct xwsync_smr * smr)
{
        return xwos_object_grab(&smr->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param smr: (I) 信号量对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_smr_put(struct xwsync_smr * smr)
{
        return xwos_object_put(&smr->xwobj);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
void xwsync_smr_construct(struct xwsync_smr * smr);

__xwos_code
void xwsync_smr_destruct(struct xwsync_smr * smr);

#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
__xwos_code
xwer_t xwsync_plsmr_intr(struct xwsync_smr * smr, struct xwos_wqn * wqn);
#endif /* XWSMPCFG_SYNC_PLSMR */

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
__xwos_code
xwer_t xwsync_rtsmr_intr(struct xwsync_smr * smr, struct xwos_wqn * wqn);
#endif /* XWSMPCFG_SYNC_RTSMR */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_SMR_MEMSLICE) && (1 == XWSMPCFG_SYNC_SMR_MEMSLICE)
__xwos_init_code
xwer_t xwsync_smr_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif /* XWSMPCFG_SYNC_SMR_MEMSLICE */

__xwos_api
xwer_t xwsync_smr_create(struct xwsync_smr ** ptrbuf, xwid_t type,
                         xwssq_t val, xwssq_t max);

__xwos_api
xwer_t xwsync_smr_delete(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_smr_destroy(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_smr_bind(struct xwsync_smr * smr, struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_smr_unbind(struct xwsync_smr * smr, struct xwsync_evt * evt);

#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
__xwos_api
xwer_t xwsync_plsmr_init(struct xwsync_smr * smr, xwssq_t val, xwssq_t max);

__xwos_api
xwer_t xwsync_plsmr_thaw(struct xwsync_smr * smr, xwssq_t val, xwssq_t max);

__xwos_api
xwer_t xwsync_plsmr_freeze(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_plsmr_post(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_plsmr_trywait(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_plsmr_timedwait(struct xwsync_smr * smr, xwtm_t * xwtm);

__xwos_api
xwer_t xwsync_plsmr_wait_unintr(struct xwsync_smr * smr);
#endif /* XWSMPCFG_SYNC_PLSMR */

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
__xwos_api
xwer_t xwsync_rtsmr_init(struct xwsync_smr * smr, xwssq_t val, xwssq_t max);

__xwos_api
xwer_t xwsync_rtsmr_thaw(struct xwsync_smr * smr, xwssq_t val, xwssq_t max);

__xwos_api
xwer_t xwsync_rtsmr_freeze(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_rtsmr_post(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_rtsmr_trywait(struct xwsync_smr * smr);

__xwos_api
xwer_t xwsync_rtsmr_timedwait(struct xwsync_smr * smr, xwtm_t * xwtm);

__xwos_api
xwer_t xwsync_rtsmr_wait_unintr(struct xwsync_smr * smr);
#endif /* XWSMPCFG_SYNC_RTSMR */

__xwos_api
xwer_t xwsync_smr_getvalue(struct xwsync_smr * smr, xwssq_t * sval);

__xwos_api
xwer_t xwsync_smr_gettype(struct xwsync_smr * smr, xwid_t * type);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_PLSMR) && (1 == XWSMPCFG_SYNC_PLSMR)
/**
 * @brief XWOS API：获取管道信号量，若不能获取，就阻塞等待
 * @param smr: (I) 信号量对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号量类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
static __xwos_inline_api
xwer_t xwsync_plsmr_wait(struct xwsync_smr * smr)
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_plsmr_timedwait(smr, &expected);
}
#endif /* XWSMPCFG_SYNC_PLSMR */

#if defined(XWSMPCFG_SYNC_RTSMR) && (1 == XWSMPCFG_SYNC_RTSMR)
/**
 * @brief XWOS API：获取实时信号量，若不能获取，就阻塞等待
 * @param smr: (I) 信号量对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ETYPE: 信号量类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
static __xwos_inline_api
xwer_t xwsync_rtsmr_wait(struct xwsync_smr * smr)
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_rtsmr_timedwait(smr, &expected);
}
#endif /* XWSMPCFG_SYNC_RTSMR */

#endif /* xwos/smp/sync/semaphore.h */
