/**
 * @file
 * @brief 玄武OS MP内核操作系统接口描述层：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_osdl_sync_sem_h__
#define __xwos_mp_osdl_sync_sem_h__

#include <xwos/mp/osdl/sync/sel.h>
#include <xwos/mp/sync/sem.h>

#define xwosdl_sem xwmp_sem

static __xwcc_inline
xwer_t xwosdl_sem_getvalue(struct xwosdl_sem * sem, xwssq_t * sval)
{
        return xwmp_sem_getvalue(sem, sval);
}

static __xwcc_inline
xwer_t xwosdl_sem_bind(struct xwosdl_sem * sem, struct xwosdl_sel * sel, xwsq_t pos)
{
        return xwmp_sem_bind(sem, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_sem_unbind(struct xwosdl_sem * sem, struct xwosdl_sel * sel)
{
        return xwmp_sem_unbind(sem, sel);
}

#if defined(XWMPCFG_SYNC_RTSEM) && (1 == XWMPCFG_SYNC_RTSEM)
static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwmp_rtsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_fini(struct xwosdl_sem * sem)
{
        return xwmp_sem_fini(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_create(struct xwosdl_sem ** sembuf, xwssq_t val, xwssq_t max)
{
        return xwmp_sem_create(sembuf, XWMP_SEM_TYPE_RT, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem)
{
        return xwmp_sem_delete(sem);
}

static __xwcc_inline
xwsq_t xwosdl_sem_gettik(struct xwosdl_sem * sem)
{
        xwsq_t tik;

        if (sem) {
                tik = sem->synobj.xwobj.tik;
        } else {
                tik = 0;
        }
        return tik;
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
xwer_t xwosdl_sem_grab(struct xwosdl_sem * sem)
{
        return xwmp_sem_grab(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_put(struct xwosdl_sem * sem)
{
        return xwmp_sem_put(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_freeze(struct xwosdl_sem * sem)
{
        return xwmp_rtsem_freeze(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_thaw(struct xwosdl_sem * sem)
{
        return xwmp_rtsem_thaw(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_post(struct xwosdl_sem * sem)
{
        return xwmp_rtsem_post(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait(struct xwosdl_sem * sem)
{
        return xwmp_rtsem_wait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_trywait(struct xwosdl_sem * sem)
{
        return xwmp_rtsem_trywait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_timedwait(struct xwosdl_sem * sem, xwtm_t * xwtm)
{
        return xwmp_rtsem_timedwait(sem, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_unintr(struct xwosdl_sem * sem)
{
        return xwmp_rtsem_wait_unintr(sem);
}

#elif defined(XWMPCFG_SYNC_PLSEM) && (1 == XWMPCFG_SYNC_PLSEM)

static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwmp_plsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_fini(struct xwosdl_sem * sem)
{
        return xwmp_sem_fini(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_create(struct xwosdl_sem ** sembuf, xwssq_t val, xwssq_t max)
{
        return xwmp_sem_create(&sem, XWMP_SEM_TYPE_PIPELINE, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem)
{
        return xwmp_sem_delete(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_freeze(struct xwosdl_sem * sem)
{
        return xwmp_plsem_freeze(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_thaw(struct xwosdl_sem * sem)
{
        return xwmp_plsem_thaw(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_post(struct xwosdl_sem * sem)
{
        return xwmp_plsem_post(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait(struct xwosdl_sem * sem)
{
        return xwmp_plsem_wait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_trywait(struct xwosdl_sem * sem)
{
        return xwmp_plsem_trywait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_timedwait(struct xwosdl_sem * sem, xwtm_t * xwtm)
{
        return xwmp_plsem_timedwait(sem, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_unintr(struct xwosdl_sem * sem)
{
        return xwmp_plsem_wait_unintr(sem);
}

#else
#  error "Can't find the semaphore configuration!"
#endif

#endif /* xwos/mp/osdl/sync/sem.h */
