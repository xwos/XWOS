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

#ifndef __xwmd_libc_newlibac_sys_types_h__
#define __xwmd_libc_newlibac_sys_types_h__

#define _POSIX_THREADS                  1 /* enable <pthread.h> */
#define _SYS__PTHREADTYPES_H_           1 /* exclude <sys/_pthreadtypes.h> */

#include <xwos/standard.h>
#include <sys/types.h>
#include <sys/sched.h>

#include <xwos/osal/skd.h>

/* ++++++++ pthread ++++++++ */
/* P1003.1c/D10, p. 141 */
enum {
        PTHREAD_CREATE_JOINABLE = 0,
        PTHREAD_CREATE_DETACHED,
};

#define PTHREAD_STACK_MIN       XWOS_STACK_SIZE_MIN

typedef struct {
        struct xwos_thd_attr xwthdattr;
} pthread_attr_t;

typedef xwos_thd_d pthread_t;

/* -------- pthread -------- */

/* ++++++++ pthread_mutex ++++++++ */
#include <xwos/osal/lock/mtx.h>

typedef struct {
        int type;
} pthread_mutexattr_t;

typedef xwos_mtx_d pthread_mutex_t;

/* -------- pthread_mutex -------- */

/* ++++++++ pthread_cond ++++++++ */
#include <xwos/osal/sync/cond.h>

typedef struct {
        clock_t  clock;
} pthread_condattr_t;

typedef xwos_cond_d pthread_cond_t;

/* -------- pthread_cond -------- */

typedef uint32_t pthread_key_t; /* FIXME: [workaround] 先绕过编译错误，后续开发 */
typedef uint32_t pthread_once_t; /* FIXME: [workaround] 先绕过编译错误，后续开发 */

#endif /* xwmd/libc/newlibac/sys/types.h */
