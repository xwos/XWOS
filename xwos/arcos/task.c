/**
 * @file
 * @brief xwos::autosar::cp::os::task
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/arcos/task.h>
#include <xwos/arcos/hook.h>

xwer_t xwarcos_task_entry(void * arg)
{
        xwer_t rc;
        int code;
        bool terminate;
        struct xwarcos_task * task = arg;

        while (!xwos_cthd_shld_stop()) {
                if (xwos_cthd_shld_frz()) {
                        xwos_cthd_freeze();
                } else {
                        rc = xwos_sem_wait(&task->sem);
                        if (XWOK == rc) {
                                xwos_splk_lock_cpuirq(&task->splk);
                                task->state = XWARCOS_TASK_STATE_RUNNING;
                                xwos_splk_unlock_cpuirq(&task->splk);
                                if (NULL != xwarcos_oshook.pre_task_hook) {
                                        xwarcos_oshook.pre_task_hook();
                                }
                                code = xwlib_setjmp(task->termination);
                                if (0 == code) {
                                        task->func();
                                        terminate = false;
                                } else {
                                        terminate = true;
                                }
                                if (terminate) {
                                } else {
                                        if (NULL != xwarcos_oshook.error_hook) {
                                                xwarcos_oshook.error_hook(
                                                        -E_XWARCOS_MISSINGEND);
                                        }
                                }
                                /* TODO: release resource */
                                if (NULL != xwarcos_oshook.post_task_hook) {
                                        xwarcos_oshook.post_task_hook();
                                }
                                xwos_splk_lock_cpuirq(&task->splk);
                                task->state = XWARCOS_TASK_STATE_SUSPENDED;
                                xwos_splk_unlock_cpuirq(&task->splk);
                        }
                }
        }
        return XWOK;
}

xwer_t xwarcos_task_validate_and_init(struct xwarcos_task * task)
{
        const struct xwarcos_task_config * cfg;
        struct xwos_thd_attr attr;
        xwer_t rc;

        if (NULL == task) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        if (NULL == task->cfg) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        cfg = task->cfg;
        task->state = XWARCOS_TASK_STATE_SUSPENDED;
        xwos_splk_init(&task->splk);
        rc = xwos_sem_init(&task->sem, (xwssq_t)0, (xwssq_t)cfg->activations);
        if (rc < 0) {
                rc = -E_XWARCOS_VALUE;
                goto err_sem_init;
        }
        xwos_thd_attr_init(&attr);
        attr.name = "xwarcos::task";
        attr.stack = cfg->stack;
        attr.stack_size = cfg->stack_size;
        attr.stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT;
        attr.priority = cfg->priority;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_init(&task->thd, &task->thdd, &attr, xwarcos_task_entry, task);
        if (rc < 0) {
                rc = -E_XWARCOS_SYS_XWOS_THD_INIT;
        }

err_sem_init:
err_nullptr:
        return rc;
}

xwer_t xwarcos_task_activate(struct xwarcos_task * task)
{
        xwer_t rc;
        xwreg_t cpuirq;

        if (NULL == task) {
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        xwos_splk_lock_cpuirqsv(&task->splk, &cpuirq);
        task->state = XWARCOS_TASK_STATE_READY;
        xwos_splk_unlock_cpuirqrs(&task->splk, cpuirq);
        rc = xwos_sem_post(&task->sem);
        if (rc < 0) {
                rc = -E_XWARCOS_LIMIT;
        }

err_id:
        return rc;
}

xwer_t xwarcos_task_terminate(void)
{
        xwer_t rc;
        xwsq_t ctx;
        struct xwarcos_task * ctask;
        xwos_thd_d cthdd;

        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_THD != ctx) {
                rc = -E_XWARCOS_CALLEVEL;
                goto err_notthd;
        }
        /* TODO: Check resource & spinlock & IRQ switch */
        cthdd = xwos_cthd_self();
        ctask = xwcc_derof(cthdd.thd, struct xwarcos_task, thd);
        xwlib_longjmp(ctask->termination, 1);
        rc = XWOK;

err_notthd:
        return rc;
}

xwer_t xwarcos_task_chain(struct xwarcos_task * task)
{
        xwer_t rc;
        xwsq_t ctx;
        struct xwarcos_task * ctask;
        xwos_thd_d cthdd;

        if (NULL == task) {
                rc = -E_XWARCOS_ID;
                goto err_id;
        }
        xwos_skd_get_context_lc(&ctx, NULL);
        if (XWOS_SKD_CONTEXT_THD != ctx) {
                rc = -E_XWARCOS_CALLEVEL;
                goto err_notthd;
        }
        xwos_splk_lock_cpuirq(&task->splk);
        task->state = XWARCOS_TASK_STATE_READY;
        xwos_splk_unlock_cpuirq(&task->splk);
        rc = xwos_sem_post(&task->sem);
        if (rc < 0) {
                rc = -E_XWARCOS_LIMIT;
                goto err_act;
        }
        /* TODO: Check resource & spinlock & IRQ switch */
        cthdd = xwos_cthd_self();
        ctask = xwcc_derof(cthdd.thd, struct xwarcos_task, thd);
        xwlib_longjmp(ctask->termination, 1);

err_act:
err_notthd:
err_id:
        return rc;
}

xwer_t xwarcos_task_get(struct xwarcos_task ** task)
{
        xwer_t rc;
        xwsq_t ctx;
        xwos_thd_d thdd;

        if (NULL == task) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        rc = XWOK;
        xwos_skd_get_context_lc(&ctx, NULL);
        if ((XWOS_SKD_CONTEXT_BOOT == ctx) || (XWOS_SKD_CONTEXT_IDLE == ctx)) {
                *task = NULL;
        } else {
                thdd = xwos_cthd_self();
                *task = xwcc_derof(thdd.thd, struct xwarcos_task, thd);
        }

err_nullptr:
        return rc;
}

xwer_t xwarcos_task_get_state(struct xwarcos_task * task,
                              enum xwarcos_task_state_em * state)
{
        xwer_t rc;
        xwreg_t cpuirq;

        if (NULL == task) {
                rc = -E_XWARCOS_ID;
                goto err_id;
        }

        if (NULL != state) {
                rc = -E_XWARCOS_ILLEGAL_ADDRESS;
                goto err_nullptr;
        }
        rc = XWOK;
        xwos_splk_lock_cpuirqsv(&task->splk, &cpuirq);
        *state = task->state;
        xwos_splk_unlock_cpuirqrs(&task->splk, cpuirq);

err_nullptr:
err_id:
        return rc;
}
