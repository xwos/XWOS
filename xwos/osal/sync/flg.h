/**
 * @file
 * @brief 操作系统抽象层：事件标志
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
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
 * @defgroup xwos_flg_trigger_em
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
 */
#define XWOS_FLG_TRIGGER_TGL_ALL      XWOSDL_FLG_TRIGGER_TGL_ALL
/**
 * @brief XWOS API：任意事件位发生翻转触发
 */
#define XWOS_FLG_TRIGGER_TGL_ANY      XWOSDL_FLG_TRIGGER_TGL_ANY
/**
 * @}
 */

/**
 * @defgroup xwos_flg_action_em
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
 * @brief XWOS API：事件标志对象中事件的最大数量
 */
#define XWOS_FLG_MAXNUM               XWOSDL_FLG_MAXNUM

/**
 * @brief 声明事件标志对象位图
 */
#define xwos_flg_declare_bitmap(name) xwosdl_flg_declare_bitmap(name)

/**
 * @brief XWOS API：静态方式初始化事件标志对象
 * @param flg: (I) 事件标志对象的指针
 * @param initval: (I) 事件标志对象位图数组的初始值，如果为NULL，初始值全部为0
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个事件标志对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_init(struct xwos_flg * flg, xwbmp_t initval[])
{
        return xwosdl_flg_init(&flg->osflg, initval);
}

/**
 * @brief XWOS API：销毁静态方式初始化的事件标志对象
 * @param flg: (I) 事件标志对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个事件标志对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_flg_destroy(struct xwos_flg * flg)
{
        return xwosdl_flg_destroy(&flg->osflg);
}

/**
 * @brief XWOS API：动态方式创建事件标志对象
 * @param flgbuf: (O) 指向缓冲区的指针，通过此缓冲区返回事件标志对象的指针
 * @param initval: (I) 事件标志对象位图数组的初始值，如果为NULL，初始值全部为0
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
xwer_t xwos_flg_create(struct xwos_flg ** flgbuf, xwbmp_t initval[])
{
        return xwosdl_flg_create((struct xwosdl_flg **)flgbuf, initval);
}

/**
 * @brief XWOS API：删除动态方式创建的事件标志对象
 * @param flg: (I) 事件标志对象指针
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
 * @param flg: (I) 事件标志对象的指针
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
 * @param flg: (I) 事件标志对象的指针
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
 * @param flgd: (I) 事件标志对象的描述符
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
 * @param flgd: (I) 事件标志对象的描述符
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
 * @param flg: (I) 事件标志对象的指针
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
 * @param flg: (I) 事件标志对象的指针
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
 * @param flg: (I) 事件标志对象的指针
 * @param sel: (I) 信号选择器的指针
 * @param pos: (I) 事件标志对象映射到位图中的位置
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
 */
static __xwos_inline_api
xwer_t xwos_flg_bind(struct xwos_flg * flg, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_flg_bind(&flg->osflg, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑事件标志对象
 * @param flg: (I) 事件标志对象的指针
 * @param sel: (I) 信号选择器的指针
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
 * @brief XWOS API：中断事件标志对象等待队列中的所有节点
 * @param flg: (I) 事件标志对象指针
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
 * @brief XWOS API：同时设置多个事件标志位
 * @param flg: (I) 事件标志对象指针
 * @param msk: (I) 事件标志对象的位图掩码
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
 * @param flg: (I) 事件标志对象指针
 * @param pos: (I) 事件标志对象的序号
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
 * @param flg: (I) 事件标志对象指针
 * @param msk: (I) 事件标志对象的位图掩码
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
 * @param flg: (I) 事件标志对象指针
 * @param pos: (I) 事件标志对象的序号
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
 * @param flg: (I) 事件标志对象指针
 * @param msk: (I) 事件标志对象的位图掩码
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
 * @param flg: (I) 事件标志对象指针
 * @param pos: (I) 事件标志对象的序号
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
 * @param flg: (I) 事件标志对象指针
 * @param out: (O) 指向缓冲区的指针，通过此缓冲区返回事件标志对象位图的值
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
 * @brief XWOS API：检测一下事件标志对象的触发事件，不会阻塞调用者
 * @param flg: (I) 事件标志对象指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwos_flg_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwos_flg_action_em，
 *                    仅trigger当取值
 *                    @ref XWOS_FLG_TRIGGER_SET_ALL
 *                    @ref XWOS_FLG_TRIGGER_SET_ANY
 *                    @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                    @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWOS_FLG_TRIGGER_SET_ALL
 *                  @ref XWOS_FLG_TRIGGER_SET_ANY
 *                  @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                  @ref XWOS_FLG_TRIGGER_CLR_ANY
 *                  (O) 返回触发时事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWOS_FLG_TRIGGER_TGL_ALL
 *                  @ref XWOS_FLG_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件标志对象的位图掩码，表示只关注掩码部分的事件
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
 * @brief XWOS API：等待事件标志对象的触发事件
 * @param flg: (I) 事件标志对象指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwos_flg_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwos_flg_action_em，
 *                    仅trigger当取值
 *                    @ref XWOS_FLG_TRIGGER_SET_ALL
 *                    @ref XWOS_FLG_TRIGGER_SET_ANY
 *                    @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                    @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWOS_FLG_TRIGGER_SET_ALL
 *                  @ref XWOS_FLG_TRIGGER_SET_ANY
 *                  @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                  @ref XWOS_FLG_TRIGGER_CLR_ANY
 *                  (O) 返回触发时事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWOS_FLG_TRIGGER_TGL_ALL
 *                  @ref XWOS_FLG_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件标志对象的位图掩码，表示只关注掩码部分的事件
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
 * @brief XWOS API：限时等待事件标志对象的触发事件
 * @param flg: (I) 事件标志对象指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwos_flg_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwos_flg_action_em，
 *                    仅trigger当取值
 *                    @ref XWOS_FLG_TRIGGER_SET_ALL
 *                    @ref XWOS_FLG_TRIGGER_SET_ANY
 *                    @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                    @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWOS_FLG_TRIGGER_SET_ALL
 *                  @ref XWOS_FLG_TRIGGER_SET_ANY
 *                  @ref XWOS_FLG_TRIGGER_CLR_ALL
 *                  @ref XWOS_FLG_TRIGGER_CLR_ANY
 *                  (O) 返回触发时事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWOS_FLG_TRIGGER_TGL_ALL
 *                  @ref XWOS_FLG_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件标志对象的位图掩码，表示只关注掩码部分的事件
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回**-ETIMEDOUT**时，**xwtm**指向的缓冲区内的期望时间会减为0。
 */
static __xwos_inline_api
xwer_t xwos_flg_timedwait(struct xwos_flg * flg, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t * xwtm)
{
        return xwosdl_flg_timedwait(&flg->osflg, trigger, action, origin, msk, xwtm);
}

#endif /* xwos/osal/sync/flg.h */
