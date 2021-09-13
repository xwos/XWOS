/**
 * @file
 * @brief 循环队列
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMDCFG_isc_xwcq_STDC_MM) && (1 == XWMDCFG_isc_xwcq_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwmd/isc/xwcq/mif.h>

/**
 * @brief 循环队列对象的构造函数
 * @param[in] cq: 循环队列对象的指针
 */
static __xwmd_code
void xwcq_construct(struct xwcq * cq, xwu8_t * mem)
{
        xwos_object_construct(&cq->xwobj);
        cq->name = NULL;
        cq->slotsize = 0;
        cq->slotnum = 0;
        cq->rear = 0;
        cq->front = 0;
        cq->q = mem;
}

/**
 * @brief 循环队列对象的析构函数
 * @param[in] cq: 循环队列对象的指针
 */
static __xwmd_code
void xwcq_destruct(struct xwcq * cq)
{
        xwos_object_destruct(&cq->xwobj);
}

/**
 * @brief 动态创建一个对象
 * @return 循环队列对象的指针
 */
static __xwmd_code
struct xwcq * xwcq_alloc(xwsz_t qsize)
{
#if defined(XWMDCFG_isc_xwcq_STDC_MM) && (1 == XWMDCFG_isc_xwcq_STDC_MM)
        struct xwcq * cq;

        cq = malloc(sizeof(struct xwcq) + qsize);
        if (NULL == cq) {
                cq = (struct xwcq *)err_ptr(-ENOMEM);
        } else {
                xwcq_construct(cq, (xwu8_t *)&cq[1]);
        }
        return cq;
#else
        union {
                struct xwcq * cq;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwcq) + qsize, XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwcq_construct(mem.cq, (xwu8_t *)&mem.cq[1]);
        } else {
                mem.cq = err_ptr(-ENOMEM);
        }
        return mem.cq;
#endif
}

/**
 * @brief 释放循环队列对象
 * @param[in] cq: 循环队列对象的指针
 */
static __xwmd_code
void xwcq_free(struct xwcq * cq)
{
#if defined(XWMDCFG_isc_xwcq_STDC_MM) && (1 == XWMDCFG_isc_xwcq_STDC_MM)
        xwcq_destruct(cq);
        free(cq);
#else
        xwcq_destruct(cq);
        xwmm_kma_free(cq);
#endif
}

/**
 * @brief 循环队列对象的垃圾回收函数
 * @param[in] cq: 循环队列对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwcq_gc(void * obj)
{
        struct xwcq * cq;

        cq = obj;
        xwos_sem_fini(&cq->sem);
        xwcq_free(cq);
        return XWOK;
}

/**
 * @brief XWCQ API：获取循环队列对象的标签
 * @param[in] cq: 循环队列对象的指针
 * @return 循环队列对象的标签
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwsq_t xwcq_gettik(struct xwcq * cq)
{
        xwsq_t tik;

        if (cq) {
                tik = cq->xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
}

/**
 * @brief XWCQ API：检查循环队列对象的标签并增加引用计数
 * @param[in] cq: 循环队列对象指针
 * @param[in] tik: 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_acquire(struct xwcq * cq, xwsq_t tik)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_acquire(&cq->xwobj, tik);
}

/**
 * @brief XWCQ API：检查循环队列对象的标签并增加引用计数
 * @param[in] cq: 循环队列对象指针
 * @param[in] tik: 标签
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_release(struct xwcq * cq, xwsq_t tik)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_release(&cq->xwobj, tik);
}

/**
 * @brief XWCQ API：增加循环队列对象的引用计数
 * @param[in] cq: 循环队列控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_grab(struct xwcq * cq)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_grab(&cq->xwobj);
}

/**
 * @brief XWCQ API：减少循环队列对象的引用计数
 * @param[in] cq: 循环队列控制块对象指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_put(struct xwcq * cq)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_put(&cq->xwobj);
}

/**
 * @brief 激活循环队列对象
 * @param[in] cq: 循环队列对象的指针
 * @param[in] name: 名字
 * @param[in] slotsize: 数据槽的大小
 * @param[in] slotnum: 数据槽的个数
 * @param[in] gcfunc: 垃圾回收函数：当对象应用计数为0，调用此函数回收资源。
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwmd_code
xwer_t xwcq_activate(struct xwcq * cq, const char * name,
                     xwsz_t slotsize, xwsz_t slotnum,
                     xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&cq->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }
        xwos_splk_init(&cq->lock);
        xwos_sem_init(&cq->sem, 0, (xwssq_t)slotnum);
        cq->name = name;
        cq->slotsize = slotsize;
        cq->slotnum = slotnum;
        cq->rear = 0;
        cq->front = 0;
        return XWOK;

err_xwobj_activate:
        return rc;
}

/**
 * @brief XWCQ API：静态方式初始化循环队列
 * @param[in] cq: 循环队列对象的指针
 * @param[in] name: 名字
 * @param[in] slotsize: 数据槽的大小
 * @param[in] slotnum: 数据槽的个数
 * @param[in] mem: 数据缓冲区的指针，数据缓冲区大小不能小于slotsize * slotnum
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwcq_init(struct xwcq * cq, const char * name,
                 xwsz_t slotsize, xwsz_t slotnum, xwu8_t * mem)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        XWOS_VALIDATE((name), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);
        XWOS_VALIDATE((slotsize > 0), "zero-size", -ESIZE);
        XWOS_VALIDATE((slotsize & XWMM_ALIGNMENT_MASK), "size-not-aligned", -EALIGN);
        XWOS_VALIDATE((slotnum > 0), "zero-num", -ESIZE);

        xwcq_construct(cq, mem);
        return xwcq_activate(cq, name, slotsize, slotnum, NULL);
}

/**
 * @brief XWCQ API：销毁静态方式初始化的循环队列对象
 * @param[in] cq: 循环队列对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwcq_fini(struct xwcq * cq)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwcq_put(cq);
}

/**
 * @brief XWCQ API：动态创建循环队列对象
 * @param[out] ptrbuf: 指向缓冲区的指针，此缓冲区用于返回循环队列对象的指针
 * @param[in] name: 名字
 * @param[in] slotsize: 数据槽的大小
 * @param[in] slotnum: 数据槽的个数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwcq_create(struct xwcq ** ptrbuf, const char * name,
                   xwsz_t slotsize, xwsz_t slotnum)
{
        struct xwcq * cq;
        xwsz_t qsize;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((name), "nullptr", -EFAULT);
        XWOS_VALIDATE((slotsize > 0), "zero-size", -ESIZE);
        XWOS_VALIDATE((slotsize & XWMM_ALIGNMENT_MASK), "size-not-aligned", -EALIGN);
        XWOS_VALIDATE((slotnum > 0), "zero-num", -ESIZE);

        *ptrbuf = NULL;
        qsize = slotsize * slotnum;
        cq = xwcq_alloc(qsize);
        if (is_err(cq)) {
                rc = ptr_err(cq);
                goto err_cq_alloc;
        }
        rc = xwcq_activate(cq, name, slotsize, slotnum, xwcq_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_cq_activate;
        }
        *ptrbuf = cq;
        return XWOK;

err_cq_activate:
        xwcq_free(cq);
err_cq_alloc:
        return rc;
}

/**
 * @brief XWCQ API：删除动态创建的循环队列对象
 * @param[in] cq: 循环队列对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmd_api
xwer_t xwcq_delete(struct xwcq * cq)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwcq_put(cq);
}

/**
 * @brief XWCQ API：将数据放入循环队列的尾端（入队 enqueue）
 * @param[in] cq: 循环队列对象的指针
 * @param[in] data: 数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示待入队的数据大小
 * + (O) 作为输出时，返回实际入队的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ESIZE: 数据长度太大
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_eq(struct xwcq * cq, xwu8_t * data, xwsz_t * size)
{
        xwreg_t cpuirq;
        xwsz_t eqsize;
        xwer_t rc;

        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);
        XWOS_VALIDATE((size), "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        eqsize = *size;
        eqsize = eqsize > cq->slotsize ? cq->slotsize : eqsize;
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        memcpy(&cq->q[(xwsz_t)cq->rear * cq->slotsize], data, eqsize);
        cq->rear++;
        if (cq->rear >= (xwssq_t)cq->slotnum) {
                cq->rear = 0;
        }
        if (cq->rear == cq->front) {
                /* Queue is overflow. Discard the oldest data */
                cq->front++;
                if (cq->front >= (xwssq_t)cq->slotnum) {
                        cq->front = 0;
                }
        }
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        xwos_sem_post(&cq->sem);
        xwcq_put(cq);
        *size = eqsize;
        return XWOK;

err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API：将数据放入循环队列的首端（插队 jump the queue）
 * @param[in] cq: 循环队列对象的指针
 * @param[in] data: 数据
 * @param[in] size: 数据的大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ESIZE: 数据长度太大
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_jq(struct xwcq * cq, xwu8_t * data, xwsz_t * size)
{
        xwreg_t cpuirq;
        xwsz_t eqsize;
        xwer_t rc;

        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);
        XWOS_VALIDATE((size), "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        eqsize = *size;
        eqsize = eqsize > cq->slotsize ? cq->slotsize : eqsize;
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        cq->front--;
        if (cq->front < 0) {
                cq->front = (xwssz_t)cq->slotnum - 1;
        }
        if (cq->front == cq->rear) {
                /* Queue is overflow. Discard the newest data */
                cq->rear--;
                if (cq->rear < 0) {
                        cq->rear = (xwssz_t)cq->slotnum;
                }
        }
        memcpy(&cq->q[(xwsz_t)cq->front * cq->slotsize], data, eqsize);
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        xwos_sem_post(&cq->sem);
        xwcq_put(cq);
        *size = eqsize;
        return XWOK;

err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 从循环队列首端取出一份数据（离队 dequeue），
 *                  若循环队列为空，就无限等待
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 等待被中断
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_dq(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        xwtm_t expected;

        expected = XWTM_MAX;
        return xwcq_timedq(cq, buf, size, &expected);
}

/**
 * @brief XWCQ API: 从循环队列首端取出一份数据（离队 dequeue），
 *                  若循环队列为空，就限时等待
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_timedq(struct xwcq * cq, xwu8_t * buf, xwsz_t * size, xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        xwsz_t bufsz;
        xwsz_t cpsz;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);
        XWOS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        bufsz = *size;
        cpsz = bufsz > cq->slotsize? cq->slotsize : bufsz;
        rc = xwos_sem_timedwait(&cq->sem, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_timedwait;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        memcpy(buf, &cq->q[(xwsz_t)cq->front * cq->slotsize], cpsz);
        cq->front++;
        if (cq->front >= (xwssz_t)cq->slotnum) {
                cq->front = 0;
        }
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        xwcq_put(cq);
        *size = cpsz;
        return XWOK;

err_sem_timedwait:
        xwcq_put(cq);
err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 尝试从循环队列首端取出一份数据（离队 dequeue），
 *                  若循环队列为空立即返回
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_trydq(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        xwsz_t bufsz, cpsz;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        bufsz = *size;
        cpsz = bufsz > cq->slotsize ? cq->slotsize : bufsz;
        rc = xwos_sem_trywait(&cq->sem);
        if (rc < 0) {
                goto err_sem_trywait;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        memcpy(buf, &cq->q[(xwsz_t)cq->front * cq->slotsize], cpsz);
        cq->front++;
        if (cq->front >= (xwssz_t)cq->slotnum) {
                cq->front = 0;
        }
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        xwcq_put(cq);
        *size = cpsz;
        return XWOK;

err_sem_trywait:
        xwcq_put(cq);
err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 从循环队列首端拷贝一份数据（数据可被再次获取），
 *                  若循环队列为空，就限时等待
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINTR: 等待被中断
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_peek(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        xwtm_t expected;

        expected = XWTM_MAX;
        return xwcq_timedpeek(cq, buf, size, &expected);
}

/**
 * @brief XWCQ API: 从循环队列首端拷贝一份数据（数据还可被再次获取），
 *                  若循环队列为空，就限时等待
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINTR: 等待被中断
 * @retval -ETIMEDOUT: 超时
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_timedpeek(struct xwcq * cq, xwu8_t * buf, xwsz_t * size, xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        xwsz_t bufsz;
        xwsz_t cpsz;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);
        XWOS_VALIDATE(xwtm, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        bufsz = *size;
        cpsz = bufsz > cq->slotsize? cq->slotsize : bufsz;
        rc = xwos_sem_timedwait(&cq->sem, xwtm);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_timedwait;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        memcpy(buf, &cq->q[(xwsz_t)cq->front * cq->slotsize], cpsz);
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        xwos_sem_post(&cq->sem);
        xwcq_put(cq);
        *size = cpsz;
        return XWOK;

err_sem_timedwait:
        xwcq_put(cq);
err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 从循环队列首端拷贝一份数据（数据可被再次获取），
 *                  若循环队列为空立即返回
 * @param[in] cq: 循环队列对象的指针
 * @param[out] buf: 指向缓冲区的指针，此缓冲区用于接收数据
 * @param[in,out] size: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示接收缓冲区的大小
 * + (O) 作为输出时，返回实际接收的数据大小
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENODATA: 接收队列为空
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmd_api
xwer_t xwcq_trypeek(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        xwreg_t cpuirq;
        xwsz_t bufsz;
        xwsz_t cpsz;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        bufsz = *size;
        cpsz = bufsz > cq->slotsize? cq->slotsize : bufsz;
        rc = xwos_sem_trywait(&cq->sem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_trywait;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        memcpy(buf, &cq->q[(xwsz_t)cq->front * cq->slotsize], cpsz);
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        xwos_sem_post(&cq->sem);
        xwcq_put(cq);
        *size = cpsz;
        return XWOK;

err_sem_trywait:
        xwcq_put(cq);
err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 清空循环队列
 * @param[in] cq: 循环队列对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwer_t xwcq_flush(struct xwcq * cq)
{
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        xwos_sem_freeze(&cq->sem);
        cq->rear = 0;
        cq->front = 0;
        xwos_sem_thaw(&cq->sem);
        xwcq_put(cq);

err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 获取循环队列的容量
 * @param[in] cq: 循环队列对象的指针
 * @param[out] capbuf: 指向缓冲区的指针，此缓冲区用于返回容量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwer_t xwcq_get_capacity(struct xwcq * cq, xwsz_t * capbuf)
{
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(capbuf, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        *capbuf = cq->slotnum;
        xwcq_put(cq);

err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 获取循环队列中有效数据的数量
 * @param[in] cq: 循环队列对象的指针
 * @param[out] szbuf: 指向缓冲区的指针，此缓冲区用于返回数量
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwer_t xwcq_get_size(struct xwcq * cq, xwsz_t * szbuf)
{
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(szbuf, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        if (cq->rear >= cq->front) {
                *szbuf = (xwsz_t)cq->rear - (xwsz_t)cq->front;
        } else {
                *szbuf = cq->slotnum - (xwsz_t)cq->front + (xwsz_t)cq->rear;
        }
        xwcq_put(cq);

err_cq_grab:
        return rc;
}

/**
 * @brief XWCQ API: 测试循环队列是否为空
 * @param[in] cq: 循环队列对象的指针
 * @param[out] emptybuf: 指向缓冲区的指针，此缓冲区用于返回测试结果
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
xwer_t xwcq_tst_empty(struct xwcq * cq, bool * emptybuf)
{
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(emptybuf, "nullptr", -EFAULT);

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        *emptybuf = !!(cq->rear == cq->front);
        xwcq_put(cq);

err_cq_grab:
        return rc;
}

/**
 * @brief XWOS API：绑定循环队列对象到信号选择器
 * @param[in] cq: 循环队列对象的指针
 * @param[in] sel: 信号选择器的指针
 * @param[in] pos: 循环队列对象对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 * - 绑定方式：独占绑定
 */
xwer_t xwcq_bind(struct xwcq * cq, struct xwos_sel * sel, xwsq_t pos)
{
        xwer_t rc;

        rc = xwcq_grab(cq);
        if (rc < 0) {
                goto err_cq_grab;
        }
        rc = xwos_sem_bind(&cq->sem, sel, pos);

err_cq_grab:
        return rc;
}

/**
 * @brief XWOS API：从信号选择器上解绑循环队列对象
 * @param[in] cq: 循环队列对象的指针
 * @param[in] sel: 信号选择器的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个信号量对象，不可重入
 */
xwer_t xwcq_unbind(struct xwcq * cq, struct xwos_sel * sel)
{
        xwer_t rc;

        rc = xwos_sem_unbind(&cq->sem, sel);
        if (rc < 0) {
                goto err_unbind;
        }
        xwcq_put(cq);
        return XWOK;

err_unbind:
        return rc;
}
