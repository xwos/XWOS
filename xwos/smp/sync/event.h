/**
 * @file
 * @brief XuanWuOS的同步机制：事件
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_sync_event_h__
#define __xwos_smp_sync_event_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/sync/condition.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSYNC_EVT_MAXNUM (XWSMPCFG_SYNC_EVT_MAXNUM) /**< 事件最大数量 */
#define XWSYNC_EVT_DECLARE_BITMAP(name) xwbmp_t name[BITS_TO_BMPS(XWSYNC_EVT_MAXNUM)]

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 事件属性枚举
 */
enum xwsync_evt_attr_em {
        XWSYNC_EVT_TYPE_FLAG = 0, /**< 事件信号旗 */
        XWSYNC_EVT_TYPE_SELECTOR, /**< 选择器 */
        XWSYNC_EVT_TYPE_BARRIER, /**< 线程同步栅栏 */
        XWSYNC_EVT_TYPE_MAX,
        XWSYNC_EVT_TYPE_MASK = 0xFF,
};

/**
 * @brief 事件触发条件枚举
 */
enum xwsync_evt_trigger_em {
        XWSYNC_EVT_TRIGGER_SET_ALL, /**< 掩码中所有位被置1 */
        XWSYNC_EVT_TRIGGER_SET_ANY, /**< 掩码中任何位被置1 */
        XWSYNC_EVT_TRIGGER_CLR_ALL, /**< 掩码中所有位被清0 */
        XWSYNC_EVT_TRIGGER_CLR_ANY, /**< 掩码中任何位被清0 */
        XWSYNC_EVT_TRIGGER_TGL_ALL, /**< 掩码中任何位翻转 */
        XWSYNC_EVT_TRIGGER_TGL_ANY, /**< 掩码中任何位翻转 */
        XWSYNC_EVT_TRIGGER_NUM,
};

/**
 * @brief 事件触发后的动作枚举
 */
enum xwsync_evt_action_em {
        XWSYNC_EVT_ACTION_NONE = 0, /**< 无动作 */
        XWSYNC_EVT_ACTION_CONSUMPTION, /**< 消费事件 */
        XWSYNC_EVT_ACTION_NUM,
};

/**
 * @brief 事件对象
 */
struct xwsync_evt {
        struct xwsync_cdt cdt; /**< C语言面向对象：继承struct xwsync_cdt */
        xwsq_t attr; /**< 属性 */
        DECLARE_BITMAP(bmp, XWSYNC_EVT_MAXNUM); /**< 事件位图 */
        DECLARE_BITMAP(msk, XWSYNC_EVT_MAXNUM); /**< 掩码位图 */
        struct xwlk_splk lock; /**< 保护位图的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param evt: (I) 事件对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_evt_grab(struct xwsync_evt * evt)
{
        return xwsync_cdt_grab(&evt->cdt);
}

/**
 * @brief 减少对象的引用计数
 * @param evt: (I) 事件对象指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwsync_evt_put(struct xwsync_evt * evt)
{
        return xwsync_cdt_put(&evt->cdt);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
xwer_t xwsync_evt_smr_bind(struct xwsync_evt * evt, struct xwsync_smr * smr,
                           xwsq_t pos);

__xwos_code
xwer_t xwsync_evt_smr_unbind(struct xwsync_evt * evt, struct xwsync_smr * smr);

__xwos_code
xwer_t xwsync_evt_smr_s1i(struct xwsync_evt * evt, struct xwsync_smr * smr);

__xwos_code
xwer_t xwsync_evt_smr_c0i(struct xwsync_evt * evt, struct xwsync_smr * smr);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SYNC_EVT_MEMSLICE) && (1 == XWSMPCFG_SYNC_EVT_MEMSLICE)
__xwos_init_code
xwer_t xwsync_evt_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif /* XWSMPCFG_SYNC_EVT_MEMSLICE */

__xwos_api
xwer_t xwsync_evt_init(struct xwsync_evt * evt, xwbmp_t initval[], xwsq_t attr);

__xwos_api
xwer_t xwsync_evt_destroy(struct xwsync_evt * evt);

__xwos_api
xwer_t xwsync_evt_create(struct xwsync_evt ** ptrbuf, xwbmp_t initval[], xwsq_t attr);

__xwos_api
xwer_t xwsync_evt_delete(struct xwsync_evt * evt);

__xwos_api
xwer_t xwsync_evt_intr_all(struct xwsync_evt * evt);

__xwos_api
xwer_t xwsync_evt_s1m(struct xwsync_evt * evt, xwbmp_t msk[]);

__xwos_api
xwer_t xwsync_evt_s1i(struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_evt_c0m(struct xwsync_evt * evt, xwbmp_t msk[]);

__xwos_api
xwer_t xwsync_evt_c0i(struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_evt_x1m(struct xwsync_evt * evt, xwbmp_t msk[]);

__xwos_api
xwer_t xwsync_evt_x1i(struct xwsync_evt * evt, xwsq_t pos);

__xwos_api
xwer_t xwsync_evt_read(struct xwsync_evt * evt, xwbmp_t out[]);

__xwos_api
xwer_t xwsync_evt_timedwait(struct xwsync_evt * evt,
                            xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm);

__xwos_api
xwer_t xwsync_evt_timedselect(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                              xwtm_t * xwtm);

__xwos_api
xwer_t xwsync_evt_timedsync(struct xwsync_evt * evt, xwsq_t pos, xwbmp_t sync[],
                            xwtm_t * xwtm);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：等待事件信号旗，事件对象类型为XWSYNC_EVT_TYPE_FLAG。
 * @param evt: (I) 事件对象的指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwsync_evt_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwsync_evt_action_em，
 *                    仅trigger当取值
 *                    @ref XWSYNC_EVT_TRIGGER_SET_ALL
 *                    @ref XWSYNC_EVT_TRIGGER_SET_ANY
 *                    @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                    @ref XWSYNC_EVT_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针，此缓冲区仅当trigger取值
 *                @ref XWSYNC_EVT_TRIGGER_TGL_ALL 以及
 *                @ref XWSYNC_EVT_TRIGGER_TGL_ANY
 *                时有效，其他情况不使用此参数，可填NULL：
 *                (I) 作为输入时，作为事件信号旗的初始值；
 *                (O) 作为输出时，返回线程被唤醒时的事件对象中信号旗位图状态
 *                    （可作为下一次调用的初始值）。
 * @param msk: (I) 事件信号旗的位图掩码，表示只关注掩码部分的信号旗。
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINVAL: 参数无效
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwsync_evt_wait(struct xwsync_evt * evt,
                       xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_evt_timedwait(evt, trigger, action, origin, msk, &expected);
}

/**
 * @brief XWOS API：等待事件对象中绑定的信号量，任何信号量的V操作都可唤醒线程，
 *                  事件对象类型为XWSYNC_EVT_TYPE_SELECTOR。
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 待触发的信号量的位图掩码，表示只关注掩码部分的信号量。
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的信号量的位图
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwsync_evt_select(struct xwsync_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_evt_timedselect(evt, msk, trg, &expected);
}

/**
 * @brief XWOS API：等待与其他线程同步，事件对象类型为XWSYNC_EVT_TYPE_BARRIER。
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @return 错误码
 * @retval OK: OK
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwsync_evt_sync(struct xwsync_evt * evt, xwsq_t pos, xwbmp_t sync[])
{
        xwtm_t expected = XWTM_MAX;
        return xwsync_evt_timedsync(evt, pos, sync, &expected);
}

#endif /* xwos/smp/sync/event.h */
