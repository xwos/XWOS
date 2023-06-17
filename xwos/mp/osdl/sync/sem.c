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

#include <xwos/standard.h>
#include <xwos/mp/osdl/sync/sem.h>

#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
__xwmp_code
xwer_t xwosdl_sem_create(xwosdl_sem_d * semd, xwssq_t val, xwssq_t max)
{
        xwer_t rc;
        struct xwmp_sem * sem;

        XWOS_VALIDATE((semd), "nullptr", -EFAULT);
        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = xwmp_sem_create(&sem, XWMP_SEM_TYPE_RT, val, max);
        if (XWOK == rc) {
                semd->sem = sem;
                semd->tik = sem->synobj.xwobj.tik;
        } else {
                *semd = XWOSDL_SEM_NILD;
        }
        return rc;
}
#elif defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
__xwmp_code
xwer_t xwosdl_sem_create(xwosdl_sem_d * semd, xwssq_t val, xwssq_t max)
{
        xwer_t rc;
        struct xwmp_sem * sem;

        XWOS_VALIDATE((semd), "nullptr", -EFAULT);

        rc = xwmp_sem_create(&sem, XWMP_SEM_TYPE_PIPELINE, val, max);
        if (XWOK == rc) {
                semd->sem = sem;
                semd->tik = sem->synobj.xwobj.tik;
        } else {
                *semd = XWOSDL_SEM_NILD;
        }
        return rc;
}
#else
#  error "Can't find the semaphore configuration!"
#endif
