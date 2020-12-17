/**
 * @file
 * @brief 操作系统抽象层：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_osal_sync_sem_h__
#define __xwos_osal_sync_sem_h__

#include <xwos/standard.h>
#include <xwos/osal/jack/sync/sem.h>
#include <xwos/osal/sync/sel.h>

/**
 * @brief XWOS API：信号量对象
 */
struct xwos_sem {
        struct xwosdl_sem ossem;
};

/**
 * @brief XWOS API：信号量对象描述符
 */
typedef struct {
        struct xwos_sem * sem; /**< 信号量对象的指针 */
        xwsq_t tik; /**< 标签 */
} xwos_sem_d;

/**
 * @brief XWOS API：空的信号量对象描述符
 */
#define XWOS_SEM_NILD ((xwos_sem_d){NULL, 0,})

/**
 * @brief XWOS API：静态方式初始化信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_init(struct xwos_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwosdl_sem_init(&sem->ossem, val, max);
}

/**
 * @brief XWOS API：销毁静态方式初始化的信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_destroy(struct xwos_sem * sem)
{
        return xwosdl_sem_destroy(&sem->ossem);
}

/**
 * @brief XWOS API：动态方式创建信号量对象
 * @param sembuf: (O) 指向缓冲区的指针，通过此缓冲区返回信号量对象指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_create(struct xwos_sem ** sembuf, xwssq_t val, xwssq_t max)
{
        return xwosdl_sem_create((struct xwosdl_sem **)sembuf, val, max);
}

/**
 * @brief XWOS API：删除动态方式创建的信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_delete(struct xwos_sem * sem)
{
        return xwosdl_sem_delete(&sem->ossem);
}

/**
 * @brief XWOS API：获取信号量对象的标签
 * @param sem: (I) 信号量对象的指针
 * @return 信号量对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwsq_t xwos_sem_gettik(struct xwos_sem * sem)
{
        return xwosdl_sem_gettik(&sem->ossem);
}

/**
 * @brief XWOS API：获取信号量对象的描述符
 * @param sem: (I) 信号量对象的指针
 * @return 信号量对象的描述符
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwos_sem_d xwos_sem_getd(struct xwos_sem * sem)
{
        xwos_sem_d semd;

        semd.sem = sem;
        semd.tik = xwosdl_sem_gettik(&sem->ossem);
        return semd;
}

/**
 * @brief XWOS API：检查信号量对象的标签并增加引用计数
 * @param semd: (I) 信号量对象的描述符
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_acquire(xwos_sem_d semd)
{
        return xwosdl_sem_acquire(&semd.sem->ossem, semd.tik);
}

/**
 * @brief XWOS API：检查对象的标签并减少引用计数
 * @param semd: (I) 信号量对象的描述符
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
xwer_t xwos_sem_release(xwos_sem_d semd)
{
        return xwosdl_sem_release(&semd.sem->ossem, semd.tik);
}

/**
 * @brief XWOS API：增加信号量对象的引用计数
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval -EFAULT: 空指针
 * @retval XWOK: OK
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_grab(struct xwos_sem * sem)
{
        return xwosdl_sem_grab(&sem->ossem);
}

/**
 * @brief XWOS API：减少信号量对象的引用计数
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: OK
 * @retval -EFAULT: 空指针
 * @retval -EOBJDEAD: 对象无效
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_put(struct xwos_sem * sem)
{
        return xwosdl_sem_put(&sem->ossem);
}

/**
 * @brief XWOS API：绑定信号量对象到信号选择器
 * @param sem: (I) 信号量对象的指针
 * @param sel: (I) 信号选择器的指针
 * @param pos: (I) 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_bind(struct xwos_sem * sem, struct xwos_sel * sel, xwsq_t pos)
{
        return xwosdl_sem_bind(&sem->ossem, &sel->ossel, pos);
}

/**
 * @brief XWOS API：从信号选择器上解绑信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param sel: (I) 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_unbind(struct xwos_sem * sem, struct xwos_sel * sel)
{
        return xwosdl_sem_unbind(&sem->ossem, &sel->ossel);
}

/**
 * @brief XWOS API：冻结信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EALREADY: 信号量对象已被冻结
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 被冻结的信号量对象不允许增加(V操作)，但可以被测试(P操作)，
 *   测试信号量对象的线程会被加入到信号量对象的等待队列。
 */
static __xwos_inline_api
xwer_t xwos_sem_freeze(struct xwos_sem * sem)
{
        return xwosdl_sem_freeze(&sem->ossem);
}

/**
 * @brief XWOS API：解冻信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EALREADY: 信号量对象未被冻结
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量对象起作用，对未冻结的信号量对象调用此函数将返回错误码。
 */
static __xwos_inline_api
xwer_t xwos_sem_thaw(struct xwos_sem * sem)
{
        return xwosdl_sem_thaw(&sem->ossem);
}

/**
 * @brief XWOS API：发布信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENEGATIVE: 信号量对象已被冻结
 * @retval -ERANGE: 信号量对象的值已经最大
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对未冻结的信号量对象起作用，已冻结的信号量对象将得到错误码-ENEGATIVE。
 */
static __xwos_inline_api
xwer_t xwos_sem_post(struct xwos_sem * sem)
{
        return xwosdl_sem_post(&sem->ossem);
}

/**
 * @brief XWOS API：等待并获取信号量对象
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_wait(struct xwos_sem * sem)
{
        return xwosdl_sem_wait(&sem->ossem);
}

/**
 * @brief XWOS API：检测一下信号量对象，不会阻塞调用者
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 信号量对象资源不可用（信号量对象无法被获取）
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_trywait(struct xwos_sem * sem)
{
        return xwosdl_sem_trywait(&sem->ossem);
}

/**
 * @brief XWOS API：限时等待并获取信号量对象
 * @param sem: (I) 信号量对象的指针
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回**-ETIMEDOUT**时，**xwtm**指向的缓冲区内的值减为0。
 */
static __xwos_inline_api
xwer_t xwos_sem_timedwait(struct xwos_sem * sem, xwtm_t * xwtm)
{
        return xwosdl_sem_timedwait(&sem->ossem, xwtm);
}

/**
 * @brief XWOS API：等待并获取信号量对象，且等待不可被中断
 * @param sem: (I) 信号量对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_wait_unintr(struct xwos_sem * sem)
{
        return xwosdl_sem_wait_unintr(&sem->ossem);
}

/**
 * @brief XWOS API：获取信号量对象计数器的值
 * @param sem: (I) 信号量对象的指针
 * @param sval: (O) 指向缓冲区的指针，通过此缓冲区返回信号量对象计数器的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
static __xwos_inline_api
xwer_t xwos_sem_getvalue(struct xwos_sem * sem, xwssq_t * sval)
{
        return xwosdl_sem_getvalue(&sem->ossem, sval);
}

#endif /* xwos/osal/sync/sem.h */
