/**
 * @file
 * @brief 操作系统抽象层：条件量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_osal_sync_cond_h__
#define __xwos_osal_sync_cond_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/cond.h>
#include <xwos/osal/time.h>
#include <xwos/osal/sync/sel.h>
#include <xwos/osal/lock/mtx.h>

/**
 * @defgroup xwos_sync_cond 条件量
 * @ingroup xwos_sync
 * @{
 */

/**
 * @brief XWOS API：条件量对象
 */
struct xwos_cond {
        struct xwosdl_cond oscond;
};

/**
 * @brief XWOS API：条件量对象描述符
 */
typedef struct {
        struct xwos_cond * cond; /**< 条件量对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_cond_d;

/**
 * @brief XWOS API：空的条件量对象描述符
 */
#define XWOS_COND_NILD ((xwos_cond_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个条件量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_init(struct xwos_cond * cond)
{
        return xwosdl_cond_init(&cond->oscond);
}

/**
 * @brief XWOS API：销毁静态方式初始化的条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个条件量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_fini(struct xwos_cond * cond)
{
        return xwosdl_cond_fini(&cond->oscond);
}

/**
 * @brief XWOS API：增加条件量对象的引用计数
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * 此函数主要用于管理**静态对象**的引用计数。
 * 若用于**动态对象**，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_cond_grab(struct xwos_cond * cond)
{
        return xwosdl_cond_grab(&cond->oscond);
}

/**
 * @brief XWOS API：减少条件量对象的引用计数
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @details
 * 此函数主要用于管理**静态对象**的引用计数。
 * 若用于**动态对象**，需要确保对象的指针一定不是野指针。
 */
static __xwos_inline_api
xwer_t xwos_cond_put(struct xwos_cond * cond)
{
        return xwosdl_cond_put(&cond->oscond);
}

/**
 * @brief XWOS API：动态方式创建条件量对象
 * @param[out] condd: 指向缓冲区的指针，通过此缓冲区返回条件量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_create(xwos_cond_d * condd)
{
        return xwosdl_cond_create((xwosdl_cond_d *)condd);
}

/**
 * @brief XWOS API：删除动态方式创建的条件量对象
 * @param[in] condd: 条件量对象描述符
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个条件量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_delete(xwos_cond_d condd)
{
        return xwosdl_cond_delete(&condd.cond->oscond, condd.tik);
}

/**
 * @brief XWOS API：检查条件量对象的标签并增加引用计数
 * @param[in] condd: 条件量对象描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_acquire(xwos_cond_d condd)
{
        return xwosdl_cond_acquire(&condd.cond->oscond, condd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param[in] condd: 条件量对象描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -ENILOBJD: 空的对象描述符
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_release(xwos_cond_d condd)
{
        return xwosdl_cond_release(&condd.cond->oscond, condd.tik);
}

/**
 * @brief XWOS API：获取条件量对象的标签
 * @param[in] cond: 条件量对象的指针
 * @return 条件量对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_cond_gettik(struct xwos_cond * cond)
{
        return xwosdl_cond_gettik(&cond->oscond);
}

/**
 * @brief XWOS API：获取条件量对象描述符
 * @param[in] cond: 条件量对象的指针
 * @return 条件量对象描述符
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwos_cond_d xwos_cond_getd(struct xwos_cond * cond)
{
        xwos_cond_d condd;

        condd.cond = cond;
        condd.tik = xwosdl_cond_gettik(&cond->oscond);
        return condd;
}

/**
 * @brief XWOS API：绑定条件量对象到信号选择器
 * @param[in] cond: 条件量对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 条件量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个条件量对象，不可重入
 * - 绑定方式：非独占绑定
 * @note
 * - 绑定了信号选择器的条件量对象，只有广播才会向信号选择器发送信号。
 */
static __xwos_inline_api
xwer_t xwos_cond_bind(struct xwos_cond * cond, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_cond_bind(&cond->oscond, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑条件量对象
 * @param[in] cond: 条件量对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个条件量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_unbind(struct xwos_cond * cond, struct xwos_sel * sel)
{
        return xwosdl_cond_unbind(&cond->oscond, &sel->ossel);
}

/**
 * @brief XWOS API：冻结条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EALREADY: 条件量对象已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的条件量对象不允许单播或广播，但可以被等待，
 *   测试条件量对象的线程会加入到条件量对象的等待队列中阻塞等待。
 */
static __xwos_inline_api
xwer_t xwos_cond_freeze(struct xwos_cond * cond)
{
        return xwosdl_cond_freeze(&cond->oscond);
}

/**
 * @brief XWOS API：解冻条件量对象
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EALREADY: 条件量对象未被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的条件量对象起作用，对未冻结的条件量对象调用此函数将返回错误码。
 */
static __xwos_inline_api
xwer_t xwos_cond_thaw(struct xwos_cond * cond)
{
        return xwosdl_cond_thaw(&cond->oscond);
}

/**
 * @brief XWOS API：中断条件量对象等待队列中所有线程
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数会将等待队列中的所有线程中断。
 */
static __xwos_inline_api
xwer_t xwos_cond_intr_all(struct xwos_cond * cond)
{
        return xwosdl_cond_intr_all(&cond->oscond);
}

/**
 * @brief XWOS API：广播条件量对象，等待队列中的所有线程都会被唤醒
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENEGATIVE: 条件量对象已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量对象起作用，已冻结的条件量对象将得到错误码**-ENEGATIVE**。
 */
static __xwos_inline_api
xwer_t xwos_cond_broadcast(struct xwos_cond * cond)
{
        return xwosdl_cond_broadcast(&cond->oscond);
}

/**
 * @brief XWOS API：单播条件量对象，只会唤醒第一个线程
 * @param[in] cond: 条件量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -ENEGATIVE: 条件量对象已被冻结
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的条件量对象起作用，已冻结的条件量对象将得到错误码**-ENEGATIVE**。
 */
static __xwos_inline_api
xwer_t xwos_cond_unicast(struct xwos_cond * cond)
{
        return xwosdl_cond_unicast(&cond->oscond);
}

/**
 * @brief XWOS API：等待条件量对象
 * @param[in] cond: 条件量对象的指针
 * @param[in] lock: 锁
 * @param[in] lktype: 锁的类型，取值：@ref xwos_lock_type_em
 * @param[in] lkdata: 锁的数据
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_cond_wait(struct xwos_cond * cond,
                      union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                      xwsq_t * lkst)
{
        return xwosdl_cond_wait(&cond->oscond, lock, lktype, lkdata, lkst);
}

/**
 * @brief XWOS API：限时等待条件量对象
 * @param[in] cond: 条件量对象的指针
 * @param[in] lock: 锁
 * @param[in] lktype: 锁的类型，取值：@ref xwos_lock_type_em
 * @param[in] lkdata: 锁的数据
 * @param[in] to: 期望唤醒的时间点
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 无效的指针或空指针
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTTHDCTX: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @details
 * 如果 ```to``` 是过去的时间点，将直接返回 `-ETIMEDOUT` 。
 */
static __xwos_inline_api
xwer_t xwos_cond_wait_to(struct xwos_cond * cond,
                         union xwos_ulock lock, xwsq_t lktype, void * lkdata,
                         xwtm_t to, xwsq_t * lkst)
{
        return xwosdl_cond_wait_to(&cond->oscond, lock, lktype, lkdata, to, lkst);
}

/**
 * @} xwos_sync_cond
 */

#endif /* xwos/osal/sync/cond.h */
