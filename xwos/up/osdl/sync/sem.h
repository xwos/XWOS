/**
 * @file
 * @brief 玄武OS UP内核操作系统接口描述层：信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_up_osdl_sync_sem_h__
#define __xwos_up_osdl_sync_sem_h__

#include <xwos/up/osdl/sync/sel.h>

#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)

#  include <xwos/up/sync/rtsem.h>

#  define xwosdl_sem xwup_rtsem

typedef struct {
        struct xwosdl_sem * sem;
        xwsq_t tik;
} xwosdl_sem_d;

#  define XWOSDL_SEM_NILD ((xwosdl_sem_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwup_rtsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_fini(struct xwosdl_sem * sem)
{
        return xwup_rtsem_fini(sem);
}

xwer_t xwosdl_sem_grab(struct xwosdl_sem * sem);

xwer_t xwosdl_sem_put(struct xwosdl_sem * sem);

xwer_t xwosdl_sem_create(xwosdl_sem_d * semd, xwssq_t val, xwssq_t max);

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwup_rtsem_delete(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_acquire(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwup_rtsem_acquire(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_release(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwup_rtsem_release(sem, tik);
}

static __xwcc_inline
xwsq_t xwosdl_sem_gettik(struct xwosdl_sem * sem)
{
        return sem ? sem->vsem.synobj.xwobj.tik : 0;
}

static __xwcc_inline
xwer_t xwosdl_sem_bind(struct xwosdl_sem * sem, struct xwosdl_sel * sel, xwsq_t pos)
{
#  if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        return xwup_rtsem_bind(sem, sel, pos);
#  else
        XWOS_UNUSED(sem);
        XWOS_UNUSED(sel);
        XWOS_UNUSED(pos);
        return XWOK;
#  endif
}

static __xwcc_inline
xwer_t xwosdl_sem_unbind(struct xwosdl_sem * sem, struct xwosdl_sel * sel)
{
#  if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        return xwup_rtsem_unbind(sem, sel);
#  else
        XWOS_UNUSED(sem);
        XWOS_UNUSED(sel);
        return XWOK;
#  endif
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
xwer_t xwosdl_sem_wait_to(struct xwosdl_sem * sem, xwtm_t to)
{
        return xwup_rtsem_wait_to(sem, to);
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

#  include <xwos/up/sync/plsem.h>

#  define xwosdl_sem xwup_plsem

typedef struct {
        struct xwosdl_sem * sem;
        xwsq_t tik;
} xwosdl_sem_d;

#  define XWOSDL_SEM_NILD ((xwosdl_sem_d){NULL, 0,})

static __xwcc_inline
xwer_t xwosdl_sem_init(struct xwosdl_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwup_plsem_init(sem, val, max);
}

static __xwcc_inline
xwer_t xwosdl_sem_fini(struct xwosdl_sem * sem)
{
        return xwup_plsem_fini(sem);
}

xwer_t xwosdl_sem_grab(struct xwosdl_sem * sem);

xwer_t xwosdl_sem_put(struct xwosdl_sem * sem);

xwer_t xwosdl_sem_create(xwosdl_sem_d * semd, xwssq_t val, xwssq_t max);

static __xwcc_inline
xwer_t xwosdl_sem_delete(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwup_plsem_delete(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_acquire(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwup_plsem_acquire(sem, tik);
}

static __xwcc_inline
xwer_t xwosdl_sem_release(struct xwosdl_sem * sem, xwsq_t tik)
{
        return xwup_plsem_release(sem, tik);
}

static __xwcc_inline
xwsq_t xwosdl_sem_gettik(struct xwosdl_sem * sem)
{
        return sem ? sem->vsem.synobj.xwobj.tik : 0;
}

static __xwcc_inline
xwer_t xwosdl_sem_bind(struct xwosdl_sem * sem, struct xwosdl_sel * sel, xwsq_t pos)
{
#  if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        return xwup_plsem_bind(sem, sel, pos);
#  else
        XWOS_UNUSED(sem);
        XWOS_UNUSED(sel);
        XWOS_UNUSED(pos);
        return XWOK;
#  endif
}

static __xwcc_inline
xwer_t xwosdl_sem_unbind(struct xwosdl_sem * sem, struct xwosdl_sel * sel)
{
#  if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        return xwup_plsem_unbind(sem, sel);
#  else
        XWOS_UNUSED(sem);
        XWOS_UNUSED(sel);
        return XWOK;
#  endif
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
xwer_t xwosdl_sem_wait_to(struct xwosdl_sem * sem, xwtm_t to)
{
        return xwup_plsem_wait_to(sem, to);
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

#else
#  error "Can't find the semaphore configuration !"
#endif

#endif /* xwos/up/osdl/sync/sem.h */
