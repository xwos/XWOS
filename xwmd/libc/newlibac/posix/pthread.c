/**
 * @file
 * @brief newlib适配代码：POSIX/pthread
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwmd/libc/newlibac/sys/types.h>
#include <pthread.h>

int pthread_attr_init(pthread_attr_t * attr)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        xwos_thd_attr_init(&attr->xwthdattr);
        return 0;
}

int pthread_attr_destroy(pthread_attr_t * attr)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        return 0;
}

int pthread_attr_setstack(pthread_attr_t * attr, void * stack, size_t stacksize)
{
        int rc;

        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);

        if (stacksize < PTHREAD_STACK_MIN) {
                rc = -EINVAL;
        } else {
                attr->xwthdattr.stack = stack;
                attr->xwthdattr.stack_size = stacksize;
                rc = 0;
        }
        return rc;
}

int pthread_attr_getstack(const pthread_attr_t * attr,
                          void ** stackaddr, size_t * stacksize)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stackaddr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stacksize), "nullptr", -EFAULT);

        *stackaddr = attr->xwthdattr.stack;
        *stacksize = attr->xwthdattr.stack_size;
        return 0;

}

int pthread_attr_setstacksize(pthread_attr_t * attr, size_t stacksize)
{
        int rc;

        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        if (stacksize < PTHREAD_STACK_MIN) {
                rc = -EINVAL;
        } else {
                attr->xwthdattr.stack_size = stacksize;
                rc = 0;
        }
        return rc;
}

int pthread_attr_getstacksize(const pthread_attr_t * attr, size_t * stacksize)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stacksize), "nullptr", -EFAULT);

        *stacksize = attr->xwthdattr.stack_size;
        return 0;
}

int pthread_attr_setstackaddr(pthread_attr_t * attr, void * stackaddr)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);

        attr->xwthdattr.stack = stackaddr;
        return 0;
}

int pthread_attr_getstackaddr(const pthread_attr_t * attr, void ** stackaddr)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stackaddr), "nullptr", -EFAULT);

        *stackaddr = attr->xwthdattr.stack;
        return 0;
}

int pthread_attr_setdetachstate(pthread_attr_t * attr, int detachstate)
{
        int rc;

        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);

        if ((detachstate < PTHREAD_CREATE_JOINABLE) ||
            (detachstate > PTHREAD_CREATE_DETACHED)) {
                rc = -EINVAL;
        } else {
                attr->xwthdattr.detached = !!detachstate;
                rc = 0;
        }
        return rc;
}


int pthread_attr_getdetachstate(const pthread_attr_t * attr, int * detachstate)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != detachstate), "nullptr", -EFAULT);

        *detachstate = attr->xwthdattr.detached ?
                        PTHREAD_CREATE_DETACHED :
                        PTHREAD_CREATE_JOINABLE;
        return 0;
}

int pthread_attr_setguardsize(pthread_attr_t * attr, size_t guardsize)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);

        attr->xwthdattr.stack_guard_size = guardsize;
        return 0;
}

int pthread_attr_getguardsize(const pthread_attr_t * attr, size_t * guardsize)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);

        *guardsize = attr->xwthdattr.stack_guard_size;
        return 0;
}

int pthread_attr_setschedparam(pthread_attr_t * attr,
                               const struct sched_param * param)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != param), "nullptr", -EFAULT);

        attr->xwthdattr.priority = param->sched_priority;
        return 0;
}

int pthread_attr_getschedparam(const pthread_attr_t * attr,
                               struct sched_param * param)
{
        XWOS_VALIDATE((NULL != attr), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != param), "nullptr", -EFAULT);

        param->sched_priority = attr->xwthdattr.priority;
        return 0;
}

int pthread_create(pthread_t * thd, const pthread_attr_t * attr,
                   void * (* func)(void *), void * arg)
{
        xwer_t rc;
        struct xwos_thd * xwosthd;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        rc = xwos_thd_create(&xwosthd, &attr->xwthdattr, (xwos_thd_f)func, arg);
        if (XWOK == rc) {
                *thd = xwos_thd_getd(xwosthd);
        } else {
                *thd = XWOS_THD_NILD;
        }
        return (int)rc;
}

int pthread_join(pthread_t thd, void ** val)
{
        xwer_t rc;
        xwer_t trc;

        rc = xwos_thd_acquire(thd);
        if (XWOK == rc) {
                rc = xwos_thd_join(thd.thd, &trc);
                xwos_thd_put(thd.thd);
                if (XWOK == rc) {
                        *val = (void *)trc;
                }
        } else {
                rc = -ESRCH;
        }
        return (int)rc;
}

int pthread_detach(pthread_t thd)
{
        xwer_t rc;

        rc = xwos_thd_acquire(thd);
        if (XWOK == rc) {
                rc = xwos_thd_detach(thd.thd);
                xwos_thd_put(thd.thd);
        } else {
                rc = -ESRCH;
        }
        return (int)rc;
}

void pthread_exit(void * rc)
{
        xwos_cthd_exit((xwer_t)rc);
        while (true) {
        }
}

pthread_t pthread_self(void)
{
        return xwos_thd_getd(xwos_cthd_self());
}

int pthread_equal(pthread_t thd1, pthread_t thd2)
{
        return !!((thd1.thd == thd2.thd) && (thd1.tik) == (thd2.tik));
}

int pthread_cancel(pthread_t thd)
{
        xwer_t rc;

        rc = xwos_thd_acquire(thd);
        if (XWOK == rc) {
                rc = xwos_thd_cancel(thd.thd);
                xwos_thd_put(thd.thd);
        } else {
                rc = -ESRCH;
        }
        return (int)rc;
}

/* int pthread_setcancelstate(int state, int * oldstate) */
/* { */
/* } */

/* int pthread_setcanceltype(int type, int * oldtype) */
/* { */
/* } */

/* void pthread_testcancel(void) */
/* { */
/* } */
