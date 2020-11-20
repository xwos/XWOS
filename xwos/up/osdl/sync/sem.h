/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：信号量
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_osdl_sync_sem_h__
#define __xwos_up_osdl_sync_sem_h__

#include <xwos/up/osdl/sync/sel.h>

#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)

#include <xwos/up/sync/rtsem.h>

#define xwosdl_sem xwup_rtsem

static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwup_rtsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_destroy(struct xwosdl_sem * sem)
{
        return xwup_rtsem_destroy(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_create(struct xwosdl_sem ** semp, xwssq_t val, xwssq_t max)
{
        xwer_t rc;

        if (NULL != semp) {
                *semp = NULL;
                rc = xwup_rtsem_create(semp, val, max);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem)
{
        return xwup_rtsem_delete(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_bind(struct xwosdl_sem * sem, struct xwosdl_sel * sel,
                       xwsq_t pos)
{
        return xwup_rtsem_bind(sem, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_sem_unbind(struct xwosdl_sem * sem, struct xwosdl_sel * sel)
{
        return xwup_rtsem_unbind(sem, sel);
}

static __xwcc_inline
xwer_t xwosdl_sem_freeze(struct xwosdl_sem * sem)
{
        return xwup_rtsem_freeze(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_thaw(struct xwosdl_sem * sem)
{
        return xwup_rtsem_thaw(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_post(struct xwosdl_sem * sem)
{
        return xwup_rtsem_post(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait(struct xwosdl_sem * sem)
{
        return xwup_rtsem_wait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_trywait(struct xwosdl_sem * sem)
{
        return xwup_rtsem_trywait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_timedwait(struct xwosdl_sem * sem, xwtm_t * xwtm)
{
        return xwup_rtsem_timedwait(sem, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_unintr(struct xwosdl_sem * sem)
{
        return xwup_rtsem_wait_unintr(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_getvalue(struct xwosdl_sem * sem, xwssq_t * sval)
{
        return xwup_rtsem_getvalue(sem, sval);
}

#elif defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)

#include <xwos/up/sync/plsem.h>

#define xwosdl_sem xwup_plsem

static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwup_plsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_destroy(struct xwosdl_sem * sem)
{
        return xwup_plsem_destroy(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_create(struct xwosdl_sem ** semp, xwssq_t val, xwssq_t max)
{
        xwer_t rc;

        if (NULL != semp) {
                *semp = NULL;
                rc = xwup_plsem_create(semp, val, max);
        } else {
                rc = -EFAULT;
        }
        return rc;
}

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem)
{
        return xwup_plsem_delete(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_bind(struct xwosdl_sem * sem, struct xwosdl_sel * sel,
                       xwsq_t pos)
{
        return xwup_plsem_bind(sem, sel, pos);
}

static __xwcc_inline
xwer_t xwosdl_sem_unbind(struct xwosdl_sem * sem, struct xwosdl_sel * sel)
{
        return xwup_plsem_unbind(sem, sel);
}

static __xwcc_inline
xwer_t xwosdl_sem_freeze(struct xwosdl_sem * sem)
{
        return xwup_plsem_freeze(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_thaw(struct xwosdl_sem * sem)
{
        return xwup_plsem_thaw(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_post(struct xwosdl_sem * sem)
{
        return xwup_plsem_post(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait(struct xwosdl_sem * sem)
{
        return xwup_plsem_wait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_trywait(struct xwosdl_sem * sem)
{
        return xwup_plsem_trywait(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_timedwait(struct xwosdl_sem * sem, xwtm_t * xwtm)
{
        return xwup_plsem_timedwait(sem, xwtm);
}

static __xwcc_inline
xwer_t xwosdl_sem_wait_unintr(struct xwosdl_sem * sem)
{
        return xwup_plsem_wait_unintr(sem);
}

static __xwcc_inline
xwer_t xwosdl_sem_getvalue(struct xwosdl_sem * sem, xwssq_t * sval)
{
        return xwup_plsem_getvalue(sem, sval);
}

#else /* XWUPCFG_SYNC_PLSEM */
  #error "Can't find the semaphore configuration !"
#endif /* !XWUPCFG_SYNC_PLSEM */

#endif /* xwos/up/osdl/sync/sem.h */
