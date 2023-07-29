/**
 * @file
 * @brief XWRUST FFI：信号量
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/osal/sync/sem.h>

xwer_t xwrustffi_sem_init(struct xwos_sem * sem, xwssq_t val, xwssq_t max)
{
        return xwos_sem_init(sem, val, max);
}

xwer_t xwrustffi_sem_fini(struct xwos_sem * sem)
{
        return xwos_sem_fini(sem);
}

xwer_t xwrustffi_sem_grab(struct xwos_sem * sem)
{
        return xwos_sem_grab(sem);
}

xwer_t xwrustffi_sem_put(struct xwos_sem * sem)
{
        return xwos_sem_put(sem);
}

xwsq_t xwrustffi_sem_gettik(struct xwos_sem * sem)
{
        return xwos_sem_gettik(sem);
}

xwer_t xwrustffi_sem_acquire(struct xwos_sem * sem, xwsq_t tik)
{
        xwos_sem_d semd = {
                .sem = sem,
                .tik = tik,
        };
        return xwos_sem_acquire(semd);
}

xwer_t xwrustffi_sem_release(struct xwos_sem * sem, xwsq_t tik)
{
        xwos_sem_d semd = {
                .sem = sem,
                .tik = tik,
        };
        return xwos_sem_release(semd);
}

xwer_t xwrustffi_sem_bind(struct xwos_sem * sem, struct xwos_sel * sel, xwsq_t pos)
{
        return xwos_sem_bind(sem, sel, pos);
}

xwer_t xwrustffi_sem_unbind(struct xwos_sem * sem, struct xwos_sel * sel)
{
        return xwos_sem_unbind(sem, sel);
}

xwer_t xwrustffi_sem_freeze(struct xwos_sem * sem)
{
        return xwos_sem_freeze(sem);
}

xwer_t xwrustffi_sem_thaw(struct xwos_sem * sem)
{
        return xwos_sem_thaw(sem);
}

xwer_t xwrustffi_sem_post(struct xwos_sem * sem)
{
        return xwos_sem_post(sem);
}

xwer_t xwrustffi_sem_wait(struct xwos_sem * sem)
{
        return xwos_sem_wait(sem);
}

xwer_t xwrustffi_sem_trywait(struct xwos_sem * sem)
{
        return xwos_sem_trywait(sem);
}

xwer_t xwrustffi_sem_wait_to(struct xwos_sem * sem, xwtm_t to)
{
        return xwos_sem_wait_to(sem, to);
}

xwer_t xwrustffi_sem_wait_unintr(struct xwos_sem * sem)
{
        return xwos_sem_wait_unintr(sem);
}

xwer_t xwrustffi_sem_get_value(struct xwos_sem * sem, xwssq_t * sval)
{
        return xwos_sem_get_value(sem, sval);
}
