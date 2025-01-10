/**
 * @file
 * @brief 操作系统抽象层：信号选择器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_sync_sel_h__
#define __xwos_osal_sync_sel_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/sel.h>
#include <xwos/osal/time.h>

/**
 * @defgroup xwos_sync_sel 信号选择器
 * @ingroup xwos_sync
 * 技术参考手册： [信号选择器](../Docs/TechRefManual/Sync/Sel)
 *
 *
  * ## 信号选择器的静态初始化、销毁
 *
 * + `xwos_sel_init()` ：静态初始化
 * + `xwos_sel_fini()` ：销毁
 *
 *
 * ## 信号选择器的动态创建、删除
 *
 * + `xwos_sel_create()` ：动态创建
 * + `xwos_sel_delete()` ：删除
 *
 *
 * ## 获取信号选择器的状态
 *
 * + `xwos_sel_get_num()` ：获取信号选择器中的信号槽的数量，可在 **任意** 上下文使用
 *
 *
 * ## 选择信号
 *
 * + `xwos_sel_select()` ：等待 **选择信号** ，只能在 **线程** 上下文使用
 * + `xwos_sel_select_to()` ：限时等待 **选择信号** ，只能在 **线程** 上下文使用
 * + `xwos_sel_tryselect()` ：仅测试是否有 **选择信号** ，可在 **任意** 上下文使用
 * + `xwos_sel_select_unintr()` ：等待 **选择信号** ，且等待不可被中断，只能在 **线程** 上下文使用
 *
 *
 * ## 使用信号选择器选择信号选择器
 *
 * + `xwos_sel_bind()` ：将信号选择器绑定到另一个信号选择器上
 * + `xwos_sel_unbind()` ：从信号选择器上解绑
 *
 *
 * ## 信号选择器对象的生命周期管理
 *
 * + 通过 **对象指针** 管理生命周期：
 *   + `xwos_sel_grab()` ：增加引用计数
 *   + `xwos_sel_put()` ：减少引用计数
 * + 通过 **对象描述符** 管理生命周期：
 *   + `xwos_sel_acquire()` ：增加引用计数
 *   + `xwos_sel_release()` ：减少引用计数
 *
 *
 * ## 对象描述符和对象标签
 *
 * 已知信号选择器对象的指针的情况下，可以通过 `xwos_sel_get_d()` 获取 @ref xwos_sel_d ，
 * 或可以通过 `xwos_sel_get_tik()` 获取对象标签。
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/sync/sel.hxx
 * @{
 */

/**
 * @brief XWOS API：信号选择器对象
 */
struct xwos_sel {
        struct xwosdl_sel ossel;
};

/**
 * @brief XWOS API：信号选择器对象描述符
 */
typedef struct {
        struct xwos_sel * sel; /**< 信号选择器对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_sel_d;

/**
 * @brief XWOS API：空的信号选择器对象描述符
 */
#define XWOS_SEL_NILD ((xwos_sel_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化信号选择器
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] num: 信号选择器中信号的数量
 * @param[in] bmp: 信号选择器用来记录信号状态的位图缓冲区
 * @param[in] msk: 信号选择器用来记录掩码状态的位图缓冲区
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_init(struct xwos_sel * sel, xwsz_t num,
                     xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwosdl_sel_init(&sel->ossel, num, bmp, msk);
}

/**
 * @brief XWOS API：销毁静态方式初始化的信号选择器
 * @param[in] sel: 信号选择器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_fini(struct xwos_sel * sel)
{
        return xwosdl_sel_fini(&sel->ossel);
}

/**
 * @brief XWOS API：增加信号选择器对象的引用计数
 * @param[in] sel: 信号选择器对象的指针
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
xwer_t xwos_sel_grab(struct xwos_sel * sel)
{
        return xwosdl_sel_grab(&sel->ossel);
}

/**
 * @brief XWOS API：减少信号选择器对象的引用计数
 * @param[in] sel: 信号选择器对象的指针
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
xwer_t xwos_sel_put(struct xwos_sel * sel)
{
        return xwosdl_sel_put(&sel->ossel);
}

/**
 * @brief XWOS API：动态方式创建信号选择器
 * @param[out] seld: 指向缓冲区的指针，通过此缓冲区返回信号选择器对象描述符
 * @param[in] num: 信号选择器中信号的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_create(xwos_sel_d * seld, xwsz_t num)
{
        return xwosdl_sel_create((xwosdl_sel_d *)seld, num);
}

/**
 * @brief XWOS API：删除动态方式创建的信号选择器
 * @param[in] seld: 信号选择器对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_delete(xwos_sel_d seld)
{
        return xwosdl_sel_delete(&seld.sel->ossel, seld.tik);
}

/**
 * @brief XWOS API：检查信号选择器对象的标签并增加引用计数
 * @param[in] seld: 信号选择器对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_acquire(xwos_sel_d seld)
{
        return xwosdl_sel_acquire(&seld.sel->ossel, seld.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] seld: 信号选择器对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_release(xwos_sel_d seld)
{
        return xwosdl_sel_release(&seld.sel->ossel, seld.tik);
}

/**
 * @brief XWOS API：获取信号选择器对象的标签
 * @param[in] sel: 信号选择器对象的指针
 * @return 信号选择器对象的标签
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwsq_t xwos_sel_get_tik(struct xwos_sel * sel)
{
        return xwosdl_sel_get_tik(&sel->ossel);
}

/**
 * @brief XWOS API：获取信号选择器对象描述符
 * @param[in] sel: 信号选择器对象的指针
 * @return 信号选择器对象描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwos_sel_d xwos_sel_get_d(struct xwos_sel * sel)
{
        xwos_sel_d seld;

        seld.sel = sel;
        seld.tik = xwosdl_sel_get_tik(&sel->ossel);
        return seld;
}

/**
 * @brief XWOS API：绑定源信号选择器到目的信号选择器
 * @param[in] src: 源信号选择器对象的指针
 * @param[in] dst: 目的信号选择器对象的指针
 * @param[in] pos: 同步对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * + 上下文：任意
 * + 绑定方式：非独占绑定
 * @details
 * 多个信号选择器可以依次绑定，形成信号传递链。
 * 但不可循环绑定，否则会造成无限循环传递。
 */
static __xwos_inline_api
xwer_t xwos_sel_bind(struct xwos_sel * src, struct xwos_sel * dst, xwsq_t pos)
{
        return xwosdl_sel_bind(&src->ossel, &dst->ossel, pos);
}

/**
 * @brief XWOS API：从目的信号选择器上解绑源信号选择器
 * @param[in] src: 源信号选择器对象的指针
 * @param[in] dst: 目的信号选择器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_unbind(struct xwos_sel * src, struct xwos_sel * dst)
{
        return xwosdl_sel_unbind(&src->ossel, &dst->ossel);
}

/**
 * @brief XWOS API：获取信号选择器中的信号槽的数量
 * @param[in] sel: 信号选择器对象的指针
 * @param[out] numbuf: 指向缓冲区的指针，通过此缓冲区返回信号槽的数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_sel_get_num(struct xwos_sel * sel, xwsz_t * numbuf)
{
        return xwosdl_sel_get_num(&sel->ossel, numbuf);
}

/**
 * @brief XWOS API：等待信号选择器中的 **选择信号**
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
 * @param[out] trg: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_sel_select(struct xwos_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwosdl_sel_select(&sel->ossel, msk, trg);
}

/**
 * @brief XWOS API：限时等待信号选择器中的 **选择信号**
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
 * @param[out] trg: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_sel_select_to(struct xwos_sel * sel,
                          xwbmp_t msk[], xwbmp_t trg[],
                          xwtm_t to)
{
        return xwosdl_sel_select_to(&sel->ossel, msk, trg, to);
}

/**
 * @brief XWOS API：检测信号选择器中是否有 **选择信号**
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
 * @param[out] trg: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 没有任何同步对象向信号选择器发送 **选择信号**
 * @note
 * + 上下文：任意
 * @details
 * 若没有 **选择信号**，立即返回错误码 `-ENODATA` ，不会等待。
 */
static __xwos_inline_api
xwer_t xwos_sel_tryselect(struct xwos_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwosdl_sel_select(&sel->ossel, msk, trg);
}

/**
 * @brief XWOS API：等待信号选择器中的 **选择信号** ，且等待不可被中断
 * @param[in] sel: 信号选择器对象的指针
 * @param[in] msk: 同步对象位图掩码，表示只关注掩码内的同步对象
 * @param[out] trg: 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_sel_select_unintr(struct xwos_sel * sel, xwbmp_t msk[], xwbmp_t trg[])
{
        return xwosdl_sel_select_unintr(&sel->ossel, msk, trg);
}

/**
 * @} xwos_sync_sel
 */

#endif /* xwos/osal/sync/sel.h */
