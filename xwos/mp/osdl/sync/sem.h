/**
 * @file
 * @brief XWOS MP内核操作系统接口描述层：信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_osdl_sync_sem_h__
#define __xwos_mp_osdl_sync_sem_h__

#include <xwos/mp/irq.h>
#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/sem.h>

#define xwosdl_sem xwmp_sem

typedef struct {
        struct xwosdl_sem * sem;
        xwsq_t tik;
} xwosdl_sem_d;

#define XWOSDL_SEM_NILD ((xwosdl_sem_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_sem_fini(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_sem_fini(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_grab(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_sem_grab(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_put(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_sem_put(sem);
}

xwer_t xwosdl_sem_create(xwosdl_sem_d * semd, xwssq_t val, xwssq_t max);

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwmp_sem_delete(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_acquire(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwmp_sem_acquire(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_release(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwmp_sem_release(sem, tik);
}

static __xwcc_inline
xwsq_t xwosdl_sem_get_tik(struct xwosdl_sem * sem)
{
        return (NULL != sem) ? sem->synobj.xwobj.tik : (xwsq_t)0;
}

static __xwcc_inline
xwer_t xwosdl_sem_get_max(struct xwosdl_sem * sem, xwssq_t * max)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((max), "nullptr", -EFAULT);

        return xwmp_sem_get_max(sem, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_get_value(struct xwosdl_sem * sem, xwssq_t * val)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((val), "nullptr", -EFAULT);

        return xwmp_sem_get_value(sem, val);
}

static __xwcc_inline
xwer_t xwosdl_sem_bind(struct xwosdl_sem * sem, struct xwosdl_sel * sel, xwsq_t pos)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWMP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwmp_sem_bind(sem, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_sem_unbind(struct xwosdl_sem * sem, struct xwosdl_sel * sel)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel), "nullptr", -EFAULT);
        XWOS_VALIDATE((sel->type == XWMP_EVT_TYPE_SEL), "type-error", -ETYPE);

        return xwmp_sem_unbind(sem, sel);
}

#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        return xwmp_rtsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_freeze(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_rtsem_freeze(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_thaw(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_rtsem_thaw(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_post(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_rtsem_post(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-ETHDCTX == xwmp_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwmp_rtsem_wait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_to(struct xwosdl_sem * sem, xwtm_t to)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-ETHDCTX == xwmp_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwmp_rtsem_wait_to(sem, to);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_unintr(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-ETHDCTX == xwmp_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwmp_rtsem_wait_unintr(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_trywait(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_rtsem_trywait(sem);
}

#elif defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)

static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        return xwmp_plsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_freeze(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_plsem_freeze(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_thaw(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_plsem_thaw(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_post(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_plsem_post(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-ETHDCTX == xwmp_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwmp_plsem_wait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_to(struct xwosdl_sem * sem, xwtm_t to)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-ETHDCTX == xwmp_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwmp_plsem_wait_to(sem, to);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_unintr(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        XWOS_VALIDATE((-ETHDCTX == xwmp_irq_get_id(NULL)), "not-thd-ctx", -ENOTTHDCTX);

        return xwmp_plsem_wait_unintr(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_trywait(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);

        return xwmp_plsem_trywait(sem);
}

#else
#  error "Can't find the semaphore configuration!"
#endif

#endif /* xwos/mp/osdl/sync/sem.h */
