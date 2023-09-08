/**
 * @file
 * @brief 操作系统抽象层：事件标志
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_sync_flg_h__
#define __xwos_osal_sync_flg_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/flg.h>
#include <xwos/osal/time.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwos_sync_flg 事件标志
 * @ingroup xwos_sync
 * 技术参考手册： [事件标志](../docs/TechRefManual/Sync/Flg)
 *
 *
 * ## 事件标志的静态初始化、销毁
 *
 * + `xwos_flg_init()` ：静态初始化
 * + `xwos_flg_fini()` ：销毁
 *
 *
 * ## 事件标志的动态创建、删除
 *
 * + `xwos_flg_create()` ：动态创建
 * + `xwos_flg_delete()` ：删除
 *
 *
 * ## 产生事件
 *
 * + `xwos_flg_s1m()` ：同时设置多个事件标志位，可在 **任意** 上下文使用
 * + `xwos_flg_s1i()` ：设置单个事件标志位，可在 **任意** 上下文使用
 * + `xwos_flg_c0m()` ：同时清除多个事件标志位，可在 **任意** 上下文使用
 * + `xwos_flg_c0i()` ：清除单个事件标志位，可在 **任意** 上下文使用
 * + `xwos_flg_x1m()` ：同时翻转多个事件标志位，可在 **任意** 上下文使用
 * + `xwos_flg_x1i()` ：翻转单个事件标志位，可在 **任意** 上下文使用
 *
 *
 * ## 获取事件的状态
 *
 * + `xwos_flg_get_num()` ：获取事件标志中总共有多少个事件，可在 **任意** 上下文使用
 * + `xwos_flg_read()` ：读取事件标志中位图的值，可在 **任意** 上下文使用
 *
 *
 * ## 等待事件
 *
 * + `xwos_flg_wait()` ：等待事件，只能在 **线程** 上下文使用
 * + `xwos_flg_wait_to()` ：限时等待事件，只能在 **线程** 上下文使用
 * + `xwos_flg_trywait()` ：仅测试事件，可在 **任意** 上下文使用
 *
 *
 * ## 使用信号选择器选择事件标志
 *
 * + `xwos_flg_bind()` ：将事件标志绑定到 [信号选择器](../docs/TechRefManual/Sync/Sel) 上
 * + `xwos_flg_unbind()` ：从 [信号选择器](../docs/TechRefManual/Sync/Sel) 上解绑
 *
 *
 * ## 事件标志对象的生命周期管理
 *
 * + 通过 **对象指针** 管理生命周期：
 *   + `xwos_flg_grab()` ：增加引用计数
 *   + `xwos_flg_put()` ：减少引用计数
 * + 通过 **对象描述符** 管理生命周期：
 *   + `xwos_flg_acquire()` ：增加引用计数
 *   + `xwos_flg_release()` ：减少引用计数
 *
 *
 * ## 对象描述符和对象标签
 *
 * 已知事件标志对象的指针的情况下，可以通过 `xwos_flg_get_d()` 获取 @ref xwos_flg_d ，
 * 或可以通过 `xwos_flg_get_tik()` 获取对象标签。
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/sync/flg.hxx
 * @{
 */

/**
 * @brief XWOS API：事件标志对象
 */
struct xwos_flg {
        struct xwosdl_flg osflg;
};

/**
 * @brief XWOS API：事件标志对象描述符
 */
typedef struct {
        struct xwos_flg * flg; /**< 事件标志对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_flg_d;

/**
 * @brief XWOS API：空的事件标志对象描述符
 */
#define XWOS_FLG_NILD ((xwos_flg_d){NULL, 0,})

/**
 * @defgroup xwos_flg_trigger_em 事件标志触发条件枚举
 * @{
 */

/**
 * @brief XWOS API：所有事件位被置1触发
 */
#define XWOS_FLG_TRIGGER_SET_ALL      XWOSDL_FLG_TRIGGER_SET_ALL
/**
 * @brief XWOS API：任意事件位被置1触发
 */
#define XWOS_FLG_TRIGGER_SET_ANY      XWOSDL_FLG_TRIGGER_SET_ANY
/**
 * @brief XWOS API：所有事件位被清0触发
 */
#define XWOS_FLG_TRIGGER_CLR_ALL      XWOSDL_FLG_TRIGGER_CLR_ALL
/**
 * @brief XWOS API：任意事件位被清0触发
 */
#define XWOS_FLG_TRIGGER_CLR_ANY      XWOSDL_FLG_TRIGGER_CLR_ANY

/**
 * @brief XWOS API：所有事件位发生翻转触发
 * @note 缩写TGL == toggle
 */
#define XWOS_FLG_TRIGGER_TGL_ALL      XWOSDL_FLG_TRIGGER_TGL_ALL
/**
 * @brief XWOS API：任意事件位发生翻转触发
 * @note 缩写TGL == toggle
 */
#define XWOS_FLG_TRIGGER_TGL_ANY      XWOSDL_FLG_TRIGGER_TGL_ANY
/**
 * @}
 */

/**
 * @defgroup xwos_flg_action_em 事件标志触发后操作枚举
 * @{
 */
/**
 * @brief XWOS API：事件触发后不做任何操作
 */
#define XWOS_FLG_ACTION_NONE          XWOSDL_FLG_ACTION_NONE

/**
 * @brief XWOS API：事件触发后清除触发条件
 * @note
 * + 仅当触发条件为下面时有效：
 *   - XWOS_FLG_TRIGGER_SET_ALL
 *   - XWOS_FLG_TRIGGER_SET_ANY
 *   - XWOS_FLG_TRIGGER_CLR_ALL
 *   - XWOS_FLG_TRIGGER_CLR_ANY
 */
#define XWOS_FLG_ACTION_CONSUMPTION   XWOSDL_FLG_ACTION_CONSUMPTION
/**
 * @}
 */

/**
 * @brief XWOS API：静态方式初始化事件标志对象
 * @param[in] flg: 事件标志对象的指针
 * @param[in] num: 事件标志中的事件数量
 * @param[in] bmp: 事件标志用来记录事件状态的位图缓冲区
 * @param[in] msk: 事件标志用来记录事件掩码的位图缓冲区
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_init(struct xwos_flg * flg, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwosdl_flg_init(&flg->osflg, num, bmp, msk);
}

/**
 * @brief XWOS API：销毁静态方式初始化的事件标志对象
 * @param[in] flg: 事件标志对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_fini(struct xwos_flg * flg)
{
        return xwosdl_flg_fini(&flg->osflg);
}

/**
 * @brief XWOS API：增加事件标志对象的引用计数
 * @param[in] flg: 事件标志对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 * @details
 * 此函数主要用于管理 **静态对象** 的引用计数。
 * 若用于 **动态对象** ，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_flg_grab(struct xwos_flg * flg)
{
        return xwosdl_flg_grab(&flg->osflg);
}

/**
 * @brief XWOS API：减少事件标志对象的引用计数
 * @param[in] flg: 事件标志对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * + 上下文：任意
 * @details
 * 此函数主要用于管理 **静态对象** 的引用计数。
 * 若用于 **动态对象** ，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_flg_put(struct xwos_flg * flg)
{
        return xwosdl_flg_put(&flg->osflg);
}

/**
 * @brief XWOS API：动态方式创建事件标志对象
 * @param[out] flgd: 指向缓冲区的指针，通过此缓冲区返回事件标志对象描述符
 * @param[in] num: 线程栅栏中的线程数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_create(xwos_flg_d * flgd, xwsz_t num)
{
        return xwosdl_flg_create((xwosdl_flg_d *)flgd, num);
}

/**
 * @brief XWOS API：删除动态方式创建的事件标志对象
 * @param[in] flgd: 事件标志对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_delete(xwos_flg_d flgd)
{
        return xwosdl_flg_delete(&flgd.flg->osflg, flgd.tik);
}

/**
 * @brief XWOS API：检查事件标志对象的标签并增加引用计数
 * @param[in] flgd: 事件标志对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_acquire(xwos_flg_d flgd)
{
        return xwosdl_flg_acquire(&flgd.flg->osflg, flgd.tik);
}

/**
 * @brief XWOS API：检查事件标志对象的标签并减少引用计数
 * @param[in] flgd: 事件标志对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_release(xwos_flg_d flgd)
{
        return xwosdl_flg_release(&flgd.flg->osflg, flgd.tik);
}

/**
 * @brief XWOS API：获取事件标志对象的标签
 * @param[in] flg: 事件标志对象的指针
 * @return 事件标志对象的标签
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwsq_t xwos_flg_get_tik(struct xwos_flg * flg)
{
        return xwosdl_flg_get_tik(&flg->osflg);
}

/**
 * @brief XWOS API：获取事件标志对象描述符
 * @param[in] flg: 事件标志对象的指针
 * @return 事件标志对象描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwos_flg_d xwos_flg_get_d(struct xwos_flg * flg)
{
        xwos_flg_d flgd;

        flgd.flg = flg;
        flgd.tik = xwosdl_flg_get_tik(&flg->osflg);
        return flgd;
}

/**
 * @brief XWOS API：绑定事件标志对象到信号选择器
 * @param[in] flg: 事件标志对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 事件标志对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * + 上下文：任意
 * + 绑定方式：非独占绑定
 */
static __xwos_inline_api
xwer_t xwos_flg_bind(struct xwos_flg * flg, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_flg_bind(&flg->osflg, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑事件标志对象
 * @param[in] flg: 事件标志对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_unbind(struct xwos_flg * flg, struct xwos_sel * sel)
{
        return xwosdl_flg_unbind(&flg->osflg, &sel->ossel);
}

/**
 * @brief XWOS API：获取事件标志中事件槽的数量
 * @param[in] flg: 事件标志对象的指针
 * @param[out] numbuf: 指向缓冲区的指针，通过此缓冲区返回事件槽的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_get_num(struct xwos_flg * flg, xwsz_t * numbuf)
{
        return xwosdl_flg_get_num(&flg->osflg, numbuf);
}

/**
 * @brief XWOS API：同时设置多个事件标志位
 * @param[in] flg: 事件标志对象指针
 * @param[in] msk: 事件标志对象的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 * @details
 * 此方法会将事件标志位图中，被掩码 `msk` 覆盖的位全部设置为 **1** 。
 *
 * 此函数除了会修改事件标志位图的状态，还会通过 **广播** 唤醒所有正在等待的线程。
 * 然后，线程通过比对位图状态，确定事件是否已经满足触发条件。
 * 若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
 */
static __xwos_inline_api
xwer_t xwos_flg_s1m(struct xwos_flg * flg, xwbmp_t msk[])
{
        return xwosdl_flg_s1m(&flg->osflg, msk);
}

/**
 * @brief XWOS API：设置单个事件标志位
 * @param[in] flg: 事件标志对象指针
 * @param[in] pos: 事件的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * + 上下文：任意
 * @details
 * 此方法会将事件标志位图中，序号为 `pos` 的单个位设置为 **1** 。
 *
 * 此函数除了会修改事件标志位图的状态，还会通过 **广播** 唤醒所有正在等待的线程。
 * 然后，线程通过比对位图状态，确定事件是否已经满足触发条件。
 * 若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
 */
static __xwos_inline_api
xwer_t xwos_flg_s1i(struct xwos_flg * flg, xwsq_t pos)
{
        return xwosdl_flg_s1i(&flg->osflg, pos);
}

/**
 * @brief XWOS API：同时清除多个事件标志位
 * @param[in] flg: 事件标志对象指针
 * @param[in] msk: 事件的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 * @details
 * 此方法会将事件标志位图中，被掩码 `msk` 覆盖的位全部清 **0** 。
 *
 * 此函数除了会修改事件标志位图的状态，还会通过 **广播** 唤醒所有正在等待的线程。
 * 然后，线程通过比对位图状态，确定事件是否已经满足触发条件。
 * 若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
 */
static __xwos_inline_api
xwer_t xwos_flg_c0m(struct xwos_flg * flg, xwbmp_t msk[])
{
        return xwosdl_flg_c0m(&flg->osflg, msk);
}

/**
 * @brief XWOS API：清除单个事件标志位
 * @param[in] flg: 事件标志对象指针
 * @param[in] pos: 事件的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * + 上下文：任意
 * @details
 * 此方法会将事件标志位图中，序号为 `pos` 的单个位清 **0** 。
 *
 * 此函数除了会修改事件标志位图的状态，还会通过 **广播** 唤醒所有正在等待的线程。
 * 然后，线程通过比对位图状态，确定事件是否已经满足触发条件。
 * 若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
 */
static __xwos_inline_api
xwer_t xwos_flg_c0i(struct xwos_flg * flg, xwsq_t pos)
{
        return xwosdl_flg_c0i(&flg->osflg, pos);
}

/**
 * @brief XWOS API：同时翻转多个事件标志位
 * @param[in] flg: 事件标志对象指针
 * @param[in] msk: 事件的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 * @details
 * 此方法会将事件标志位图中，被掩码 `msk` 覆盖的位全部翻转。
 *
 * 此函数除了会修改事件标志位图的状态，还会通过 **广播** 唤醒所有正在等待的线程。
 * 然后，线程通过比对位图状态，确定事件是否已经满足触发条件。
 * 若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
 */
static __xwos_inline_api
xwer_t xwos_flg_x1m(struct xwos_flg * flg, xwbmp_t msk[])
{
        return xwosdl_flg_x1m(&flg->osflg, msk);
}

/**
 * @brief XWOS API：翻转单个事件标志位
 * @param[in] flg: 事件标志对象指针
 * @param[in] pos: 事件的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * + 上下文：任意
 * @details
 * 此方法会将事件标志位图中，序号为 `pos` 的单个位翻转。
 *
 * 此函数除了会修改事件标志位图的状态，还会通过 **广播** 唤醒所有正在等待的线程。
 * 然后，线程通过比对位图状态，确定事件是否已经满足触发条件。
 * 若满足触发条件，就退出等待；若未满足触发条件，重新进入阻塞等待状态。
 */
static __xwos_inline_api
xwer_t xwos_flg_x1i(struct xwos_flg * flg, xwsq_t pos)
{
        return xwosdl_flg_x1i(&flg->osflg, pos);
}

/**
 * @brief XWOS API：读取事件标志位图的值
 * @param[in] flg: 事件标志对象指针
 * @param[out] out: 指向缓冲区的指针，通过此缓冲区返回事件标志对象位图的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_flg_read(struct xwos_flg * flg, xwbmp_t out[])
{
        return xwosdl_flg_read(&flg->osflg, out);
}

/**
 * @brief XWOS API：等待事件
 * @param[in] flg: 事件标志对象指针
 * @param[in] trigger: 事件触发条件，取值：
 * + 电平触发
 *   @arg @ref XWOS_FLG_TRIGGER_SET_ALL : 掩码中的所有位同时为1
 *   @arg @ref XWOS_FLG_TRIGGER_SET_ANY : 掩码中的任意位为1
 *   @arg @ref XWOS_FLG_TRIGGER_CLR_ALL : 掩码中的所有位同时为0
 *   @arg @ref XWOS_FLG_TRIGGER_CLR_ANY : 掩码中的任意位为0
 * + 边沿触发
 *   @arg @ref XWOS_FLG_TRIGGER_TGL_ALL : 掩码中的所有位同时发生翻转
 *   @arg @ref XWOS_FLG_TRIGGER_TGL_ANY : 掩码中的任意位发生翻转
 * @param[in] action: 事件触发后的动作，
 * + 当trigger为 **电平触发** 时，取值：
 *   @arg @ref XWOS_FLG_ACTION_CONSUMPTION : 消费事件
 *   @arg @ref XWOS_FLG_ACTION_NONE : 无操作
 * + 当trigger为 **边沿触发** 时，此参数没有用，可填 `XWOS_FLG_ACTION_NONE`
 * @param[in,out] origin: 指向缓冲区的指针：
 * + 当 `trigger` 为 **电平触发** 时
 *   + (O) 返回事件发生时的位图的状态
 * + 当trigger为 **边沿触发** 时
 *   + (I) 作为输入时，作为比较的初始状态
 *   + (O) 作为输出时，返回触发后的事件位图状态（常作为下一次调用的初始值）
 * @param[in] msk: 事件的位图掩码，表示只关注掩码部分的事件
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * + 当没有检测到事件时，线程会阻塞等待。
 * + 当检测到事件时，线程被唤醒，然后返回 `XWOK` 。
 * + 当线程阻塞等待被中断时，返回 `-EINTR` 。
 */
static __xwos_inline_api
xwer_t xwos_flg_wait(struct xwos_flg * flg, xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[])
{
        return xwosdl_flg_wait(&flg->osflg, trigger, action, origin, msk);
}

/**
 * @brief XWOS API：限时等待触发事件
 * @param[in] flg: 事件标志对象指针
 * @param[in] trigger: 事件触发条件，取值：
 * + 电平触发
 *   @arg @ref XWOS_FLG_TRIGGER_SET_ALL : 掩码中的所有位同时为1
 *   @arg @ref XWOS_FLG_TRIGGER_SET_ANY : 掩码中的任意位为1
 *   @arg @ref XWOS_FLG_TRIGGER_CLR_ALL : 掩码中的所有位同时为0
 *   @arg @ref XWOS_FLG_TRIGGER_CLR_ANY : 掩码中的任意位为0
 * + 边沿触发
 *   @arg @ref XWOS_FLG_TRIGGER_TGL_ALL : 掩码中的所有位同时发生翻转
 *   @arg @ref XWOS_FLG_TRIGGER_TGL_ANY : 掩码中的任意位发生翻转
 * @param[in] action: 事件触发后的动作，
 * + 当trigger为 **电平触发** 时，取值：
 *   @arg @ref XWOS_FLG_ACTION_CONSUMPTION : 消费事件
 *   @arg @ref XWOS_FLG_ACTION_NONE : 无操作
 * + 当trigger为 **边沿触发** 时，此参数没有用，可填 `XWOS_FLG_ACTION_NONE`
 * @param[in,out] origin: 指向缓冲区的指针：
 * + 当 `trigger` 为 **电平触发** 时
 *   + (O) 返回事件发生时的位图的状态
 * + 当trigger为 **边沿触发** 时
 *   + (I) 作为输入时，作为比较的初始状态
 *   + (O) 作为输出时，返回触发后的事件位图状态（常作为下一次调用的初始值）
 * @param[in] msk: 事件的位图掩码，表示只关注掩码部分的事件
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * + 当没有检测到事件，线程会阻塞等待，等待时会指定一个唤醒时间点 `to` 。
 * + 当检测到事件，线程被唤醒，然后返回 `XWOK` 。
 * + 当线程阻塞等待被中断时，返回 `-EINTR` 。
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 * + 当到达指定的唤醒时间点时，线程被唤醒，并返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_flg_wait_to(struct xwos_flg * flg, xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[],
                        xwtm_t to)
{
        return xwosdl_flg_wait_to(&flg->osflg, trigger, action, origin, msk, to);
}

/**
 * @brief XWOS API：检查触发事件
 * @param[in] flg: 事件标志对象指针
 * @param[in] trigger: 事件触发条件，取值：
 * + 电平触发
 *   @arg @ref XWOS_FLG_TRIGGER_SET_ALL : 掩码中的所有位同时为1
 *   @arg @ref XWOS_FLG_TRIGGER_SET_ANY : 掩码中的任意位为1
 *   @arg @ref XWOS_FLG_TRIGGER_CLR_ALL : 掩码中的所有位同时为0
 *   @arg @ref XWOS_FLG_TRIGGER_CLR_ANY : 掩码中的任意位为0
 * + 边沿触发
 *   @arg @ref XWOS_FLG_TRIGGER_TGL_ALL : 掩码中的所有位同时发生翻转
 *   @arg @ref XWOS_FLG_TRIGGER_TGL_ANY : 掩码中的任意位发生翻转
 * @param[in] action: 事件触发后的动作，
 * + 当trigger为 **电平触发** 时，取值：
 *   @arg @ref XWOS_FLG_ACTION_CONSUMPTION : 消费事件
 *   @arg @ref XWOS_FLG_ACTION_NONE : 无操作
 * + 当trigger为 **边沿触发** 时，此参数没有用，可填 `XWOS_FLG_ACTION_NONE`
 * @param[in,out] origin: 指向缓冲区的指针：
 * + 当 `trigger` 为 **电平触发** 时
 *   + (O) 返回事件发生时的位图的状态
 * + 当trigger为 **边沿触发** 时
 *   + (I) 作为输入时，作为比较的初始状态
 *   + (O) 作为输出时，返回触发后的事件位图状态（常作为下一次调用的初始值）
 * @param[in] msk: 事件的位图掩码，表示只关注掩码部分的事件
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENODATA: 没有任何事件触发
 * @note
 * + 上下文：任意
 * @details
 * + 当检测到事件，立即返回 `XWOK` 。
 * + 当没有检测到事件，立即返回 `-ENODATA` 。
 */
static __xwos_inline_api
xwer_t xwos_flg_trywait(struct xwos_flg * flg, xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[])
{
        return xwosdl_flg_trywait(&flg->osflg, trigger, action, origin, msk);
}

/**
 * @} xwos_sync_flg
 */

#endif /* xwos/osal/sync/flg.h */
