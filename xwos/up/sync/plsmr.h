/**
 * @file
 * @brief XuanWuOS的同步机制：管道信号量
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_sync_plsmr_h__
#define __xwos_up_sync_plsmr_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/up/plwq.h>
#include <xwos/up/sync/vsmr.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 管道信号量对象
 */
struct xwsync_plsmr {
        struct xwsync_vsmr vsmr; /**< C语言面向对象：继承struct xwsync_vsmr */
        struct xwos_plwq plwq; /**< 实时等待队列 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
xwer_t xwsync_plsmr_intr(struct xwsync_plsmr * smr, struct xwos_wqn * wqn);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_api
xwer_t xwsync_plsmr_init(struct xwsync_plsmr * smr, xwssq_t val, xwssq_t max);

__xwos_api
xwer_t xwsync_plsmr_destroy(struct xwsync_plsmr * smr);

__xwos_api
xwer_t xwsync_plsmr_create(struct xwsync_plsmr ** ptrbuf, xwssq_t val, xwssq_t max);

__xwos_api
xwer_t xwsync_plsmr_delete(struct xwsync_plsmr * smr);

__xwos_api
xwer_t xwsync_plsmr_post(struct xwsync_plsmr * smr);

__xwos_api
xwer_t xwsync_plsmr_trywait(struct xwsync_plsmr * smr);

__xwos_api
xwer_t xwsync_plsmr_timedwait(struct xwsync_plsmr * smr, xwtm_t * xwtm);

__xwos_api
xwer_t xwsync_plsmr_wait_unintr(struct xwsync_plsmr * smr);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_EVT) && (1 == XWSMPCFG_SYNC_EVT)
/**
 * @brief XWOS API：绑定管道信号量到事件对象，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR。
 * @param smr: (I) 管道信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 管道信号量对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_inline_api
xwer_t xwsync_plsmr_bind(struct xwsync_plsmr * smr, struct xwsync_evt * evt,
                         xwsq_t pos)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_vsmr_bind(&smr->vsmr, evt, pos);
}

/**
 * @brief XWOS API：从事件对象上解绑管道信号量，
 *                  事件对象类型为XWSYNC_EVT_TYPE_SELECTOR。
 * @param smr: (I) 管道信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *smr* ，不可重入
 */
static __xwos_inline_api
xwer_t xwsync_plsmr_unbind(struct xwsync_plsmr * smr, struct xwsync_evt * evt)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwsync_vsmr_unbind(&smr->vsmr, evt);
}
#endif /* XWSMPCFG_SYNC_EVT */

/**
 * @brief XWOS API：冻结管道信号量（值设置为负）。
 * @param smr: (I) 信号量对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EALREADY: 信号量已为负
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)。
 *   测试的线程会被加入到信号量的等待队列。
 */
static __xwos_inline_api
xwer_t xwsync_plsmr_freeze(struct xwsync_plsmr * smr)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);

        return xwsync_vsmr_freeze(&smr->vsmr);
}

/**
 * @brief XWOS API：解冻管道信号量，并重新初始化。
 * @param smr: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -EALREADY: 信号量不为负
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用。
 */
static __xwos_inline_api
xwer_t xwsync_plsmr_thaw(struct xwsync_plsmr * smr, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        return xwsync_vsmr_thaw(&smr->vsmr, val, max);
}

/**
 * @brief XWOS API：获取管道信号量，若不能获取，就阻塞等待
 * @param smr: (I) 信号量对象指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
static __xwos_inline_api
xwer_t xwsync_plsmr_wait(struct xwsync_plsmr * smr)
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_plsmr_timedwait(smr, &expected);
}

/**
 * @brief XWOS API：获取管道信号量计数器的值。
 * @param smr: (I) 信号量对象的指针
 * @param sval: (O) 指向缓冲区的指针，通过此缓冲区返回信号量计数器的值
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwsync_plsmr_getvalue(struct xwsync_plsmr * smr, xwssq_t * sval)
{
        XWOS_VALIDATE((smr), "nullptr", -EFAULT);
        XWOS_VALIDATE((sval), "nullptr", -EFAULT);

        return xwsync_vsmr_getvalue(&smr->vsmr, sval);
}

#endif /* xwos/up/sync/plsmr.h */
