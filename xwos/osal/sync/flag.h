/**
 * @file
 * @brief 操作系统抽象层：事件信号旗
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_flag_h__
#define __xwos_osal_sync_flag_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/osdl/os.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL：事件信号旗
 */
struct xwosal_flg {
        struct xwosdl_flg osflg; /**< 操作系统的事件信号旗 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @defgroup xwosal_flg_trigger_em
 * @{
 */

/**
 * @brief 触发条件：所有事件位被置1
 */
#define XWOSAL_FLG_TRIGGER_SET_ALL      XWOSDL_FLG_TRIGGER_SET_ALL
/**
 * @brief 触发条件：任意事件位被置1
 */
#define XWOSAL_FLG_TRIGGER_SET_ANY      XWOSDL_FLG_TRIGGER_SET_ANY
/**
 * @brief 触发条件：所有事件位被清0
 */
#define XWOSAL_FLG_TRIGGER_CLR_ALL      XWOSDL_FLG_TRIGGER_CLR_ALL
/**
 * @brief 触发条件：任意事件位被清0
 */
#define XWOSAL_FLG_TRIGGER_CLR_ANY      XWOSDL_FLG_TRIGGER_CLR_ANY

/**
 * @brief 触发条件：所有事件位发生翻转
 */
#define XWOSAL_FLG_TRIGGER_TGL_ALL      XWOSDL_FLG_TRIGGER_TGL_ALL
/**
 * @brief 触发条件：任意事件位发生翻转
 */
#define XWOSAL_FLG_TRIGGER_TGL_ANY      XWOSDL_FLG_TRIGGER_TGL_ANY
/**
 * @}
 */

/**
 * @defgroup xwosal_flg_action_em
 * @{
 */
/**
 * @brief 事件触发后不做任何操作
 */
#define XWOSAL_FLG_ACTION_NONE          XWOSDL_FLG_ACTION_NONE

/**
 * @brief 事件触发后清除触发条件
 * @note
 * + 仅当触发条件为下面时有效：
 *   - XWOSAL_FLG_TRIGGER_SET_ALL
 *   - XWOSAL_FLG_TRIGGER_SET_ANY
 *   - XWOSAL_FLG_TRIGGER_CLR_ALL
 *   - XWOSAL_FLG_TRIGGER_CLR_ANY
 */
#define XWOSAL_FLG_ACTION_CONSUMPTION   XWOSDL_FLG_ACTION_CONSUMPTION
/**
 * @}
 */

/**
 * @brief 事件信号旗中事件的最大数量
 */
#define XWOSAL_FLG_MAXNUM               XWOSDL_FLG_MAXNUM

/**
 * @brief 声明事件信号旗位图
 */
#define xwosal_flg_declare_bitmap(name) xwosdl_flg_declare_bitmap(name)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       APIs        ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOSAL API：静态方式初始化事件信号旗
 * @param flg: (I) 事件信号旗的指针
 * @param initval: (I) 事件信号旗位图数组的初始值，如果为NULL，初始值全部为0
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *flg* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_init(struct xwosal_flg * flg, xwbmp_t initval[])
{
        return xwosdl_flg_init(&flg->osflg, initval);
}

/**
 * @brief XWOSAL API：销毁静态方式初始化的事件信号旗
 * @param flg: (I) 事件信号旗的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *flg* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_destroy(struct xwosal_flg * flg)
{
        return xwosdl_flg_destroy(&flg->osflg);
}

/**
 * @brief XWOSAL API：动态方式创建事件信号旗
 * @param flgidbuf: (O) 指向缓冲区的指针，通过此缓冲区返回ID
 * @param initval: (I) 事件信号旗位图数组的初始值，如果为NULL，初始值全部为0
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_create(xwid_t * flgidbuf, xwbmp_t initval[])
{
        return xwosdl_flg_create(flgidbuf, initval);
}

/**
 * @brief XWOSAL API：删除动态方式创建的事件信号旗
 * @param flgid: (I) 事件信号旗ID
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *flg* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_delete(xwid_t flgid)
{
        return xwosdl_flg_delete(flgid);
}

/**
 * @brief XWOSAL API：从软件事件信号旗对象指针获取其ID
 * @param flg: (I) 事件信号旗对象的指针
 * @return 事件信号旗ID
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwid_t xwosal_flg_get_id(struct xwosal_flg * flg)
{
        return (xwid_t)xwosdl_flg_get_id(&flg->osflg);
}

/**
 * @brief XWOSAL API：从事件信号旗ID获取对象的指针
 * @param flgid: (I) 事件信号旗ID
 * @return 事件信号旗对象的指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
struct xwosal_flg * xwosal_flg_get_obj(xwid_t flgid)
{
        return (struct xwosal_flg *)xwosdl_flg_get_obj(flgid);
}

/**
 * @brief XWOSAL API：绑定事件信号旗到信号选择器
 * @param flgid: (I) 事件信号旗的ID
 * @param sltid: (I) 信号选择器的ID
 * @param pos: (I) 事件信号旗对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 信号选择器或事件信号旗类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *flgid* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_bind(xwid_t flgid, xwid_t sltid, xwsq_t pos)
{
        return xwosdl_flg_bind(flgid, sltid, pos);
}

/**
 * @brief XWOSAL API：从信号选择器上解绑事件信号旗
 * @param flgid: (I) 事件信号旗的ID
 * @param sltid: (I) 信号选择器的ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 信号选择器类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *flgid* ，不可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_unbind(xwid_t flgid, xwid_t sltid)
{
        return xwosdl_flg_unbind(flgid, sltid);
}

/**
 * @brief XWOSAL API：中断事件信号旗等待队列中的所有节点
 * @param flgid: (I) 事件信号旗ID
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_intr_all(xwid_t flgid)
{
        return xwosdl_flg_intr_all(flgid);
}

/**
 * @brief XWOSAL API：同时设置多个事件标志位
 * @param flgid: (I) 事件信号旗ID
 * @param msk: (I) 事件信号旗的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_s1m(xwid_t flgid, xwbmp_t msk[])
{
        return xwosdl_flg_s1m(flgid, msk);
}

/**
 * @brief XWOSAL API：设置单个事件标志位
 * @param flgid: (I) 事件信号旗ID
 * @param pos: (I) 事件信号旗的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_s1i(xwid_t flgid, xwsq_t pos)
{
        return xwosdl_flg_s1i(flgid, pos);
}

/**
 * @brief XWOSAL API：同时清除多个事件标志位
 * @param flgid: (I) 事件信号旗ID
 * @param msk: (I) 事件信号旗的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_c0m(xwid_t flgid, xwbmp_t msk[])
{
        return xwosdl_flg_c0m(flgid, msk);
}

/**
 * @brief XWOSAL API：清除单个事件标志位
 * @param flgid: (I) 事件信号旗ID
 * @param pos: (I) 事件信号旗的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_c0i(xwid_t flgid, xwsq_t pos)
{
        return xwosdl_flg_c0i(flgid, pos);
}

/**
 * @brief XWOSAL API：同时翻转多个事件标志位
 * @param flgid: (I) 事件信号旗ID
 * @param msk: (I) 事件信号旗的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_x1m(xwid_t flgid, xwbmp_t msk[])
{
        return xwosdl_flg_x1m(flgid, msk);
}

/**
 * @brief XWOSAL API：翻转单个事件标志位
 * @param flgid: (I) 事件信号旗ID
 * @param pos: (I) 事件信号旗的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_x1i(xwid_t flgid, xwsq_t pos)
{
        return xwosdl_flg_x1i(flgid, pos);
}

/**
 * @brief XWOSAL API：读取事件信号旗中事件标志位图的值
 * @param flgid: (I) 事件信号旗ID
 * @param out: (O) 指向缓冲区的指针，通过此缓冲区返回事件信号旗位图的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_read(xwid_t flgid, xwbmp_t out[])
{
        return xwosdl_flg_read(flgid, out);
}

/**
 * @brief XWOSAL API：尝试等一下事件信号旗的触发事件
 * @param flgid: (I) 事件信号旗ID
 * @param trigger: (I) 事件触发条件，取值 @ref xwosal_flg_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwosal_flg_action_em，
 *                    仅trigger当取值
 *                    @ref XWOSAL_FLG_TRIGGER_SET_ALL
 *                    @ref XWOSAL_FLG_TRIGGER_SET_ANY
 *                    @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                    @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWOSAL_FLG_TRIGGER_SET_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_SET_ANY
 *                  @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWOSAL_FLG_TRIGGER_TGL_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件信号旗的位图掩码，表示只关注掩码部分的信号旗
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_trywait(xwid_t flgid, xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[])
{
        return xwosdl_flg_trywait(flgid, trigger, action, origin, msk);
}

/**
 * @brief XWOSAL API：等待事件信号旗的触发事件
 * @param flgid: (I) 事件信号旗ID
 * @param trigger: (I) 事件触发条件，取值 @ref xwosal_flg_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwosal_flg_action_em，
 *                    仅trigger当取值
 *                    @ref XWOSAL_FLG_TRIGGER_SET_ALL
 *                    @ref XWOSAL_FLG_TRIGGER_SET_ANY
 *                    @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                    @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWOSAL_FLG_TRIGGER_SET_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_SET_ANY
 *                  @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWOSAL_FLG_TRIGGER_TGL_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件信号旗的位图掩码，表示只关注掩码部分的信号旗
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwosal_flg_wait(xwid_t flgid, xwsq_t trigger, xwsq_t action,
                       xwbmp_t origin[], xwbmp_t msk[])
{
        return xwosdl_flg_wait(flgid, trigger, action, origin, msk);
}

/**
 * @brief XWOSAL API：限时等待事件信号旗的触发事件
 * @param flgid: (I) 事件信号旗ID
 * @param trigger: (I) 事件触发条件，取值 @ref xwosal_flg_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwosal_flg_action_em，
 *                    仅trigger当取值
 *                    @ref XWOSAL_FLG_TRIGGER_SET_ALL
 *                    @ref XWOSAL_FLG_TRIGGER_SET_ANY
 *                    @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                    @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWOSAL_FLG_TRIGGER_SET_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_SET_ANY
 *                  @ref XWOSAL_FLG_TRIGGER_CLR_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWOSAL_FLG_TRIGGER_TGL_ALL
 *                  @ref XWOSAL_FLG_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件信号旗的位图掩码（表示只关注掩码部分的信号旗）
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
static __xwos_inline_api
xwer_t xwosal_flg_timedwait(xwid_t flgid, xwsq_t trigger, xwsq_t action,
                            xwbmp_t origin[], xwbmp_t msk[],
                            xwtm_t * xwtm)
{
        return xwosdl_flg_timedwait(flgid, trigger, action, origin, msk, xwtm);
}

#endif /* xwos/osal/sync/flag.h */
