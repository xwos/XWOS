/**
 * @file
 * @brief 操作系统抽象层：事件标志
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_flg_h__
#define __xwos_osal_sync_flg_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/flg.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwos_sync_flg 事件标志
 * @ingroup xwos_sync
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个事件标志对象，不可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个事件标志对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_fini(struct xwos_flg * flg)
{
        return xwosdl_flg_fini(&flg->osflg);
}

/**
 * @brief XWOS API：动态方式创建事件标志对象
 * @param[out] flgbuf: 指向缓冲区的指针，通过此缓冲区返回事件标志对象的指针
 * @param[in] num: 线程栅栏中的线程数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_create(struct xwos_flg ** flgbuf, xwsz_t num)
{
        return xwosdl_flg_create((struct xwosdl_flg **)flgbuf, num);
}

/**
 * @brief XWOS API：删除动态方式创建的事件标志对象
 * @param[in] flg: 事件标志对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个事件标志对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_delete(struct xwos_flg * flg)
{
        return xwosdl_flg_delete(&flg->osflg);
}

/**
 * @brief XWOS API：获取事件标志对象的标签
 * @param[in] flg: 事件标志对象的指针
 * @return 事件标志对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_flg_gettik(struct xwos_flg * flg)
{
        return xwosdl_flg_gettik(&flg->osflg);
}

/**
 * @brief XWOS API：获取事件标志对象的描述符
 * @param[in] flg: 事件标志对象的指针
 * @return 事件标志对象的描述符
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwos_flg_d xwos_flg_getd(struct xwos_flg * flg)
{
        xwos_flg_d flgd;

        flgd.flg = flg;
        flgd.tik = xwosdl_flg_gettik(&flg->osflg);
        return flgd;
}

/**
 * @brief XWOS API：检查事件标志对象的标签并增加引用计数
 * @param[in] flgd: 事件标志对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_acquire(xwos_flg_d flgd)
{
        return xwosdl_flg_acquire(&flgd.flg->osflg, flgd.tik);
}

/**
 * @brief XWOS API：检查事件标志对象的标签并减少引用计数
 * @param[in] flgd: 事件标志对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_release(xwos_flg_d flgd)
{
        return xwosdl_flg_release(&flgd.flg->osflg, flgd.tik);
}

/**
 * @brief XWOS API：增加事件标志对象的引用计数
 * @param[in] flg: 事件标志对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
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
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_put(struct xwos_flg * flg)
{
        return xwosdl_flg_put(&flg->osflg);
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个事件标志对象，不可重入
 * - 绑定方式：非独占绑定
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个事件标志对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_unbind(struct xwos_flg * flg, struct xwos_sel * sel)
{
        return xwosdl_flg_unbind(&flg->osflg, &sel->ossel);
}

/**
 * @brief XWOS API：中断事件标志对象等待队列中的所有线程
 * @param[in] flg: 事件标志对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_intr_all(struct xwos_flg * flg)
{
        return xwosdl_flg_intr_all(&flg->osflg);
}

/**
 * @brief XWOS API：获取事件标志中事件槽的数量
 * @param[in] flg: 事件标志对象的指针
 * @param[out] numbuf: 指向缓冲区的指针，通过此缓冲区返回事件槽的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
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
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_x1i(struct xwos_flg * flg, xwsq_t pos)
{
        return xwosdl_flg_x1i(&flg->osflg, pos);
}

/**
 * @brief XWOS API：读取事件标志对象中事件标志位图的值
 * @param[in] flg: 事件标志对象指针
 * @param[out] out: 指向缓冲区的指针，通过此缓冲区返回事件标志对象位图的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_read(struct xwos_flg * flg, xwbmp_t out[])
{
        return xwosdl_flg_read(&flg->osflg, out);
}

/**
 * @brief XWOS API：等待事件标志对象的触发事件
 * @param[in] flg: 事件标志对象指针
 * @param[in] trigger: 事件触发条件，取值：
 *   @arg XWOS_FLG_TRIGGER_SET_ALL: 掩码中的所有位同时为1
 *   @arg XWOS_FLG_TRIGGER_SET_ANY: 掩码中的任意位为1
 *   @arg XWOS_FLG_TRIGGER_CLR_ALL: 掩码中的所有位同时为0
 *   @arg XWOS_FLG_TRIGGER_CLR_ANY: 掩码中的任意位为0
 *   @arg XWOS_FLG_TRIGGER_TGL_ALL: 掩码中的所有位同时发生翻转
 *   @arg XWOS_FLG_TRIGGER_TGL_ANY: 掩码中的任意位发生翻转
 * @param[in] action: 事件触发后的动作，
 * + 当trigger为@ref XWOS_FLG_TRIGGER_SET_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_SET_ANY ，或@ref XWOS_FLG_TRIGGER_CLR_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_CLR_ANY 时有效，取值：
 *   @arg XWOS_FLG_ACTION_CONSUMPTION: 消费事件
 *   @arg XWOS_FLG_ACTION_NOME: 无操作
 * + 当trigger为@ref XWOS_FLG_TRIGGER_TGL_ALL ，或@ref XWOS_FLG_TRIGGER_TGL_ANY 时，
 *   此参数没有用，可填：
 *   @arg XWOS_UNUSED_ARGUMENT
 * @param[in,out] origin: 指向缓冲区的指针：
 * + 当trigger为@ref XWOS_FLG_TRIGGER_SET_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_SET_ANY ，或@ref XWOS_FLG_TRIGGER_CLR_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_CLR_ANY 时
 *   - (O) 返回触发时事件对象中位图状态（action之前）
 * + 当trigger为@ref XWOS_FLG_TRIGGER_TGL_ALL ，或@ref XWOS_FLG_TRIGGER_TGL_ANY 时
 *   - (I) 作为输入时，作为用于比较的初始值
 *   - (O) 作为输出时，返回事件对象中位图状态（可作为下一次调用的初始值）
 * @param[in] msk: 事件的位图掩码，表示只关注掩码部分的事件
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_wait(struct xwos_flg * flg, xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[])
{
        return xwosdl_flg_wait(&flg->osflg, trigger, action, origin, msk);
}

/**
 * @brief XWOS API：检查事件标志，立即返回，不会等待
 * @param[in] flg: 事件标志对象指针
 * @param[in] trigger: 事件触发条件，取值：
 *   @arg XWOS_FLG_TRIGGER_SET_ALL: 掩码中的所有位同时为1
 *   @arg XWOS_FLG_TRIGGER_SET_ANY: 掩码中的任意位为1
 *   @arg XWOS_FLG_TRIGGER_CLR_ALL: 掩码中的所有位同时为0
 *   @arg XWOS_FLG_TRIGGER_CLR_ANY: 掩码中的任意位为0
 *   @arg XWOS_FLG_TRIGGER_TGL_ALL: 掩码中的所有位同时发生翻转
 *   @arg XWOS_FLG_TRIGGER_TGL_ANY: 掩码中的任意位发生翻转
 * @param[in] action: 事件触发后的动作，
 * + 当trigger为@ref XWOS_FLG_TRIGGER_SET_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_SET_ANY ，或@ref XWOS_FLG_TRIGGER_CLR_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_CLR_ANY 时有效，取值：
 *   @arg XWOS_FLG_ACTION_CONSUMPTION: 消费事件
 *   @arg XWOS_FLG_ACTION_NOME: 无操作
 * + 当trigger为@ref XWOS_FLG_TRIGGER_TGL_ALL ，或@ref XWOS_FLG_TRIGGER_TGL_ANY 时，
 *   此参数没有用，可填：
 *   @arg XWOS_UNUSED_ARGUMENT
 * @param[in,out] origin: 指向缓冲区的指针：
 * + 当trigger为@ref XWOS_FLG_TRIGGER_SET_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_SET_ANY ，或@ref XWOS_FLG_TRIGGER_CLR_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_CLR_ANY 时
 *   - (O) 返回触发时事件对象中位图状态（action之前）
 * + 当trigger为@ref XWOS_FLG_TRIGGER_TGL_ALL ，或@ref XWOS_FLG_TRIGGER_TGL_ANY 时
 *   - (I) 作为输入时，作为用于比较的初始值
 *   - (O) 作为输出时，返回事件对象中位图状态（可作为下一次调用的初始值）
 * @param[in] msk: 事件的位图掩码，表示只关注掩码部分的事件
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ENODATA: 没有任何事件触发
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_trywait(struct xwos_flg * flg, xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[])
{
        return xwosdl_flg_trywait(&flg->osflg, trigger, action, origin, msk);
}

/**
 * @brief XWOS API：限时等待事件标志对象的触发事件
 * @param[in] flg: 事件标志对象指针
 * @param[in] trigger: 事件触发条件，取值：
 *   @arg XWOS_FLG_TRIGGER_SET_ALL: 掩码中的所有位同时为1
 *   @arg XWOS_FLG_TRIGGER_SET_ANY: 掩码中的任意位为1
 *   @arg XWOS_FLG_TRIGGER_CLR_ALL: 掩码中的所有位同时为0
 *   @arg XWOS_FLG_TRIGGER_CLR_ANY: 掩码中的任意位为0
 *   @arg XWOS_FLG_TRIGGER_TGL_ALL: 掩码中的所有位同时发生翻转
 *   @arg XWOS_FLG_TRIGGER_TGL_ANY: 掩码中的任意位发生翻转
 * @param[in] action: 事件触发后的动作，
 * + 当trigger为@ref XWOS_FLG_TRIGGER_SET_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_SET_ANY ，或@ref XWOS_FLG_TRIGGER_CLR_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_CLR_ANY 时有效 ，取值：
 *   @arg XWOS_FLG_ACTION_CONSUMPTION: 消费事件
 *   @arg XWOS_FLG_ACTION_NOME: 无操作
 * + 当trigger为@ref XWOS_FLG_TRIGGER_TGL_ALL ，或@ref XWOS_FLG_TRIGGER_TGL_ANY 时，
 *   此参数没有用，可填：
 *   @arg XWOS_UNUSED_ARGUMENT
 * @param[in,out] origin: 指向缓冲区的指针：
 * + 当trigger为@ref XWOS_FLG_TRIGGER_SET_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_SET_ANY ，或@ref XWOS_FLG_TRIGGER_CLR_ALL ，
 *   或@ref XWOS_FLG_TRIGGER_CLR_ANY 时
 *   - (O) 返回触发时事件对象中位图状态（action之前）
 * + 当trigger为XWOS_FLG_TRIGGER_TGL_ALL ，或@ref XWOS_FLG_TRIGGER_TGL_ANY 时
 *   - (I) 作为输入时，作为用于比较的初始值
 *   - (O) 作为输出时，返回事件对象中位图状态（可作为下一次调用的初始值）
 * @param[in] msk: 事件的位图掩码，表示只关注掩码部分的事件
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时，xwtm指向的缓冲区内的时间会减为0
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_timedwait(struct xwos_flg * flg, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t * xwtm)
{
        return xwosdl_flg_timedwait(&flg->osflg, trigger, action, origin, msk, xwtm);
}

/**
 * @} xwos_sync_flg
 */

#endif /* xwos/osal/sync/flg.h */
