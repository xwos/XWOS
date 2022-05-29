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

#include <xwos/standard.h>
#include <xwos/up/osdl/sync/sem.h>

#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
__xwup_code
xwer_t xwosdl_sem_grab(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwup_rtsem_grab(sem);
}

__xwup_code
xwer_t xwosdl_sem_put(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwup_rtsem_put(sem);
}

__xwup_code
xwer_t xwosdl_sem_create(xwosdl_sem_d * semd, xwssq_t val, xwssq_t max)
{
        xwer_t rc;
        struct xwup_rtsem * sem;

        XWOS_VALIDATE((semd), "nullptr", -EFAULT);

        rc = xwup_rtsem_create(&sem, val, max);
        if (XWOK == rc) {
                semd->sem = sem;
                semd->tik = sem->vsem.synobj.xwobj.tik;
        } else {
                *semd = XWOSDL_SEM_NILD;
        }
        return rc;
}
#elif defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
__xwup_code
xwer_t xwosdl_sem_grab(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwup_plsem_grab(sem);
}

__xwup_code
xwer_t xwosdl_sem_put(struct xwosdl_sem * sem)
{
        XWOS_VALIDATE((sem), "nullptr", -EFAULT);
        return xwup_plsem_put(sem);
}

__xwup_code
xwer_t xwosdl_sem_create(xwosdl_sem_d * semd, xwssq_t val, xwssq_t max)
{
        xwer_t rc;
        struct xwup_plsem * sem;

        XWOS_VALIDATE((semd), "nullptr", -EFAULT);

        rc = xwup_plsem_create(&sem, val, max);
        if (XWOK == rc) {
                semd->sem = sem;
                semd->tik = sem->vsem.synobj.xwobj.tik;
        } else {
                *semd = XWOSDL_SEM_NILD;
        }
        return rc;
}
#else
#  error "Can't find the semaphore configuration!"
#endif
