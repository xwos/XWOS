/**
 * @file
 * @brief 循环队列
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
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
 * @param[in] mem: 数据缓冲区
 */
static __xwmd_code
void xwcq_construct(struct xwcq * cq, xwu8_t * mem)
{
        xwos_object_construct(&cq->xwobj);
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
 * @brief 循环队列对象的垃圾回收函数
 * @param[in] obj: 循环队列对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwcq_gc(void * obj)
{
        struct xwcq * cq;

        cq = obj;
        xwos_sem_fini(&cq->sem);
        xwcq_destruct(cq);
        return XWOK;
}

/**
 * @brief 激活循环队列对象
 * @param[in] cq: 循环队列对象的指针
 * @param[in] slotsize: 数据槽的大小
 * @param[in] slotnum: 数据槽的个数
 * @param[in] gcfunc: 垃圾回收函数：当对象应用计数为0，调用此函数回收资源。
 * @return 错误码
 */
static __xwmd_code
xwer_t xwcq_activate(struct xwcq * cq, xwsz_t slotsize, xwsz_t slotnum,
                     xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwos_object_activate(&cq->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }
        xwos_splk_init(&cq->lock);
        xwos_sem_init(&cq->sem, 0, (xwssq_t)slotnum);
        cq->slotsize = slotsize;
        cq->slotnum = slotnum;
        cq->rear = 0;
        cq->front = 0;
        return XWOK;

err_xwobj_activate:
        return rc;
}

__xwmd_api
xwer_t xwcq_init(struct xwcq * cq, xwsz_t slotsize, xwsz_t slotnum, xwu8_t * mem)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);
        XWOS_VALIDATE((slotsize > 0), "zero-size", -ESIZE);
        XWOS_VALIDATE(!(slotsize & XWMM_ALIGNMENT_MASK), "size-not-aligned", -EALIGN);
        XWOS_VALIDATE((slotnum > 0), "zero-num", -ESIZE);

        xwcq_construct(cq, mem);
        return xwcq_activate(cq, slotsize, slotnum, xwcq_gc);
}

__xwmd_api
xwer_t xwcq_fini(struct xwcq * cq)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwcq_put(cq);
}

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

__xwmd_api
xwer_t xwcq_acquire(struct xwcq * cq, xwsq_t tik)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_acquire(&cq->xwobj, tik);
}

__xwmd_api
xwer_t xwcq_release(struct xwcq * cq, xwsq_t tik)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_release(&cq->xwobj, tik);
}

__xwmd_api
xwer_t xwcq_grab(struct xwcq * cq)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_grab(&cq->xwobj);
}

__xwmd_api
xwer_t xwcq_put(struct xwcq * cq)
{
        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        return xwos_object_put(&cq->xwobj);
}

__xwmd_api
xwer_t xwcq_eq(struct xwcq * cq, const xwu8_t * data, xwsz_t * size)
{
        xwreg_t cpuirq;
        xwsz_t eqsize;

        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);
        XWOS_VALIDATE((size), "nullptr", -EFAULT);

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
        *size = eqsize;
        return XWOK;
}

__xwmd_api
xwer_t xwcq_jq(struct xwcq * cq, const xwu8_t * data, xwsz_t * size)
{
        xwreg_t cpuirq;
        xwsz_t eqsize;

        XWOS_VALIDATE((cq), "nullptr", -EFAULT);
        XWOS_VALIDATE((data), "nullptr", -EFAULT);
        XWOS_VALIDATE((size), "nullptr", -EFAULT);

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
        *size = eqsize;
        return XWOK;
}

__xwmd_api
xwer_t xwcq_dq(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        return xwcq_dq_to(cq, buf, size, XWTM_MAX);
}

__xwmd_api
xwer_t xwcq_dq_to(struct xwcq * cq, xwu8_t * buf, xwsz_t * size, xwtm_t to)
{
        xwreg_t cpuirq;
        xwsz_t bufsz;
        xwsz_t cpsz;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);

        bufsz = *size;
        cpsz = bufsz > cq->slotsize? cq->slotsize : bufsz;
        rc = xwos_sem_wait_to(&cq->sem, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_wait_to;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        memcpy(buf, &cq->q[(xwsz_t)cq->front * cq->slotsize], cpsz);
        cq->front++;
        if (cq->front >= (xwssz_t)cq->slotnum) {
                cq->front = 0;
        }
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        *size = cpsz;
        return XWOK;

err_sem_wait_to:
        return rc;
}

__xwmd_api
xwer_t xwcq_trydq(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        xwsz_t bufsz, cpsz;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);

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
        *size = cpsz;
        return XWOK;

err_sem_trywait:
        return rc;
}

__xwmd_api
xwer_t xwcq_rq(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        return xwcq_rq_to(cq, buf, size, XWTM_MAX);
}

__xwmd_api
xwer_t xwcq_rq_to(struct xwcq * cq, xwu8_t * buf, xwsz_t * size, xwtm_t to)
{
        xwreg_t cpuirq;
        xwsz_t bufsz;
        xwsz_t cpsz;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);

        bufsz = *size;
        cpsz = bufsz > cq->slotsize? cq->slotsize : bufsz;
        rc = xwos_sem_wait_to(&cq->sem, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_wait_to;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        cq->rear--;
        if (cq->rear < 0) {
                cq->rear = (xwssz_t)cq->slotnum - 1;
        }
        memcpy(buf, &cq->q[(xwsz_t)cq->rear * cq->slotsize], cpsz);
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        *size = cpsz;
        return XWOK;

err_sem_wait_to:
        return rc;
}

__xwmd_api
xwer_t xwcq_tryrq(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        xwreg_t cpuirq;
        xwsz_t bufsz;
        xwsz_t cpsz;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);

        bufsz = *size;
        cpsz = bufsz > cq->slotsize? cq->slotsize : bufsz;
        rc = xwos_sem_trywait(&cq->sem);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_trywait;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        cq->rear--;
        if (cq->rear < 0) {
                cq->rear = (xwssz_t)cq->slotnum - 1;
        }
        memcpy(buf, &cq->q[(xwsz_t)cq->rear * cq->slotsize], cpsz);
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        *size = cpsz;
        return XWOK;

err_sem_trywait:
        return rc;
}

__xwmd_api
xwer_t xwcq_peek(struct xwcq * cq, xwu8_t * buf, xwsz_t * size)
{
        return xwcq_peek_to(cq, buf, size, XWTM_MAX);
}

__xwmd_api
xwer_t xwcq_peek_to(struct xwcq * cq, xwu8_t * buf, xwsz_t * size, xwtm_t to)
{
        xwreg_t cpuirq;
        xwsz_t bufsz;
        xwsz_t cpsz;
        xwer_t rc;

        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(buf, "nullptr", -EFAULT);
        XWOS_VALIDATE(size, "nullptr", -EFAULT);

        bufsz = *size;
        cpsz = bufsz > cq->slotsize? cq->slotsize : bufsz;
        rc = xwos_sem_wait_to(&cq->sem, to);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_sem_wait_to;
        }
        xwos_splk_lock_cpuirqsv(&cq->lock, &cpuirq);
        memcpy(buf, &cq->q[(xwsz_t)cq->front * cq->slotsize], cpsz);
        xwos_splk_unlock_cpuirqrs(&cq->lock, cpuirq);
        xwos_sem_post(&cq->sem);
        *size = cpsz;
        return XWOK;

err_sem_wait_to:
        return rc;
}

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
        *size = cpsz;
        return XWOK;

err_sem_trywait:
        return rc;
}

xwer_t xwcq_flush(struct xwcq * cq)
{
        XWOS_VALIDATE(cq, "nullptr", -EFAULT);

        xwos_sem_freeze(&cq->sem);
        cq->rear = 0;
        cq->front = 0;
        xwos_sem_thaw(&cq->sem);
        return XWOK;
}

xwer_t xwcq_get_capacity(struct xwcq * cq, xwsz_t * capbuf)
{
        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(capbuf, "nullptr", -EFAULT);

        *capbuf = cq->slotnum;
        return XWOK;
}

xwer_t xwcq_get_size(struct xwcq * cq, xwsz_t * szbuf)
{
        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(szbuf, "nullptr", -EFAULT);

        if (cq->rear >= cq->front) {
                *szbuf = (xwsz_t)cq->rear - (xwsz_t)cq->front;
        } else {
                *szbuf = cq->slotnum - (xwsz_t)cq->front + (xwsz_t)cq->rear;
        }
        return XWOK;
}

xwer_t xwcq_tst_empty(struct xwcq * cq, bool * emptybuf)
{
        XWOS_VALIDATE(cq, "nullptr", -EFAULT);
        XWOS_VALIDATE(emptybuf, "nullptr", -EFAULT);

        *emptybuf = !!(cq->rear == cq->front);
        return XWOK;
}

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
