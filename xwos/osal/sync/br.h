/**
 * @file
 * @brief 操作系统抽象层：线程栅栏
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_sync_br_h__
#define __xwos_osal_sync_br_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/br.h>
#include <xwos/osal/time.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwos_sync_br 线程栅栏
 * @ingroup xwos_sync
 * 技术参考手册： [线程栅栏](../Docs/TechRefManual/Sync/Br)
 *
 *
 * ## 线程栅栏的静态初始化、销毁
 *
 * + `xwos_br_init()` ：静态初始化
 * + `xwos_br_fini()` ：销毁
 *
 *
 * ## 线程栅栏的动态创建、删除
 *
 * + `xwos_br_create()` ：动态创建
 * + `xwos_br_delete()` ：删除
 *
 *
 * ## 获取事件的状态
 *
 * + `xwos_br_get_num()` ：获取线程栅栏中线程槽数量，可在 **任意** 上下文使用
 *
 *
 * ## 同步线程
 *
 * + `xwos_br_wait()` ：等待所有线程到达栅栏，只能在 **线程** 上下文使用
 * + `xwos_br_wait_to()` ：限时等待所有线程到达栅栏，只能在 **线程** 上下文使用
 * + `xwos_br_wait_unintr()` ：等待所有线程到达栅栏，且等待不可被中断，
 *   只能在 **线程** 上下文使用
 *
 *
 * ## 使用信号选择器选择线程栅栏
 *
 * + `xwos_br_bind()` ：将线程栅栏绑定到 [信号选择器](../Docs/TechRefManual/Sync/Sel) 上
 * + `xwos_br_unbind()` ：从 [信号选择器](../Docs/TechRefManual/Sync/Sel) 上解绑
 *
 *
 * ## 线程栅栏对象的生命周期管理
 *
 * + 通过 **对象指针** 管理生命周期：
 *   + `xwos_br_grab()` ：增加引用计数
 *   + `xwos_br_put()` ：减少引用计数
 * + 通过 **对象描述符** 管理生命周期：
 *   + `xwos_br_acquire()` ：增加引用计数
 *   + `xwos_br_release()` ：减少引用计数
 *
 *
 * ## 对象描述符和对象标签
 *
 * 已知线程栅栏对象的指针的情况下，可以通过 `xwos_br_get_d()` 获取 @ref xwos_br_d ，
 * 或可以通过 `xwos_br_get_tik()` 获取对象标签。
 *
 *
 * ## C++
 *
 * C++头文件： @ref xwos/osal/sync/br.hxx
 * @{
 */

/**
 * @brief XWOS API：线程栅栏
 */
struct xwos_br {
        struct xwosdl_br osbr;
};

/**
 * @brief XWOS API：线程栅栏对象描述符
 */
typedef struct {
        struct xwos_br * br; /**< 线程栅栏对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_br_d;

/**
 * @brief XWOS API：空的线程栅栏对象描述符
 */
#define XWOS_BR_NILD ((xwos_br_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化线程栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] num: 线程栅栏中的线程数量
 * @param[in] bmp: 线程栅栏用来记录线程抵达事件的位图缓冲区
 * @param[in] msk: 线程栅栏用来记录线程掩码状态的位图缓冲区，长度应与bmp一致
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_init(struct xwos_br * br, xwsz_t num,
                    xwbmp_t * bmp, xwbmp_t * msk)
{
        return xwosdl_br_init(&br->osbr, num, bmp, msk);
}

/**
 * @brief XWOS API：销毁静态方式初始化的线程栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_fini(struct xwos_br * br)
{
        return xwosdl_br_fini(&br->osbr);
}

/**
 * @brief XWOS API：增加线程栅栏对象的引用计数
 * @param[in] br: 线程栅栏对象的指针
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
xwer_t xwos_br_grab(struct xwos_br * br)
{
        return xwosdl_br_grab(&br->osbr);
}

/**
 * @brief XWOS API：减少线程栅栏对象的引用计数
 * @param[in] br: 线程栅栏对象的指针
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
xwer_t xwos_br_put(struct xwos_br * br)
{
        return xwosdl_br_put(&br->osbr);
}

/**
 * @brief XWOS API：动态方式创建线程栅栏
 * @param[out] brd: 指向缓冲区的指针，通过此缓冲区返回线程栅栏对象描述符
 * @param[in] num: 线程栅栏中的线程数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_create(xwos_br_d * brd, xwsz_t num)
{
        return xwosdl_br_create((xwosdl_br_d *)brd, num);
}

/**
 * @brief XWOS API：删除动态方式创建的线程栅栏
 * @param[in] brd: 线程栅栏对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_delete(xwos_br_d brd)
{
        return xwosdl_br_delete(&brd.br->osbr, brd.tik);
}

/**
 * @brief XWOS API：检查线程栅栏对象的标签并增加引用计数
 * @param[in] brd: 线程栅栏对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_acquire(xwos_br_d brd)
{
        return xwosdl_br_acquire(&brd.br->osbr, brd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] brd: 线程栅栏对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_release(xwos_br_d brd)
{
        return xwosdl_br_release(&brd.br->osbr, brd.tik);
}

/**
 * @brief XWOS API：获取线程栅栏对象的标签
 * @param[in] br: 线程栅栏对象的指针
 * @return 线程栅栏对象的标签
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwsq_t xwos_br_get_tik(struct xwos_br * br)
{
        return xwosdl_br_get_tik(&br->osbr);
}

/**
 * @brief XWOS API：获取线程栅栏对象描述符
 * @param[in] br: 线程栅栏对象的指针
 * @return 线程栅栏对象描述符
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwos_br_d xwos_br_get_d(struct xwos_br * br)
{
        xwos_br_d brd;

        brd.br = br;
        brd.tik = xwosdl_br_get_tik(&br->osbr);
        return brd;
}

/**
 * @brief XWOS API：绑定线程栅栏到信号选择器
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 线程栅栏对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 线程数量超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * + 上下文：任意
 * + 绑定方式：非独占绑定
 * @details
 * + 当所有线程到达线程栅栏时，绑定了信号选择器的线程栅栏将向信号选择器发送信号。
 */
static __xwos_inline_api
xwer_t xwos_br_bind(struct xwos_br * br, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_br_bind(&br->osbr, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑线程栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_unbind(struct xwos_br * br, struct xwos_sel * sel)
{
        return xwosdl_br_unbind(&br->osbr, &sel->ossel);
}

/**
 * @brief XWOS API：获取线程栅栏中线程槽数量
 * @param[in] br: 线程栅栏对象的指针
 * @param[out] numbuf: 指向缓冲区的指针，通过此缓冲区返回线程槽数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * + 上下文：任意
 */
static __xwos_inline_api
xwer_t xwos_br_get_num(struct xwos_br * br, xwsz_t * numbuf)
{
        return xwosdl_br_get_num(&br->osbr, numbuf);
}

/**
 * @brief XWOS API：等待所有线程到达栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 线程数量超出范围
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX：不在线程上下文中
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_br_wait(struct xwos_br * br)
{
        return xwosdl_br_wait(&br->osbr);
}

/**
 * @brief XWOS API：限时等待所有线程到达栅栏
 * @param[in] br: 线程栅栏对象的指针
 * @param[in] to: 期望唤醒的时间点
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 线程数量超出范围
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX：不在线程上下文中
 * @note
 * + 上下文：线程
 * @details
 * `to` 表示等待超时的时间点：
 * + `to` 通常是未来的时间，即 **当前系统时间** + `delta` ，
 *   可以使用 `xwtm_ft(delta)` 表示；
 * + 如果 `to` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_br_wait_to(struct xwos_br * br, xwtm_t to)
{
        return xwosdl_br_wait_to(&br->osbr, to);
}

/**
 * @brief XWOS API：等待所有线程到达栅栏，且等待不可被中断
 * @param[in] br: 线程栅栏对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 线程数量超出范围
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX：不在线程上下文中
 * @note
 * + 上下文：线程
 */
static __xwos_inline_api
xwer_t xwos_br_wait_unintr(struct xwos_br * br)
{
        return xwosdl_br_wait_unintr(&br->osbr);
}

/**
 * @} xwos_sync_br
 */

#endif /* xwos/osal/sync/br.h */
