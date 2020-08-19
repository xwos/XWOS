/**
 * @file
 * @brief 示例：线程的创建与退出
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwam/example/thread/create/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define THRD_1_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define THRD_2_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define EXAMPLE_THREAD_CREATE_LOG_TAG         "thrdcrt"
  #define thrdcrtlogf(lv, fmt, ...)     \
        xwlogf(lv, EXAMPLE_THREAD_CREATE_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define thrdcrtlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t thrd_1_func(void * arg);

xwer_t thrd_2_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 动态创建的线程描述表
 */
const struct xwosal_thrd_desc dynamic_tbd[] = {
        [0] = {
                .name = "Thread-1",
                .prio = THRD_1_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)thrd_1_func,
                .arg = (void *)3,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};
xwid_t dynamic_tid[xw_array_size(dynamic_tbd)];

/**
 * @brief 静态初始化的线程的栈内存
 */
xwstk_t __xwcc_aligned(8) static_thrd_stack[1][512]; /* 大小 == sizeof(xwstk_t) * 256 */

/**
 * @brief 静态初始化的线程描述表
 */
const struct xwosal_thrd_desc static_tbd[] = {
        [0] = {
                .name = "Thread-2",
                .prio = THRD_2_PRIORITY,
                .stack = static_thrd_stack[0],
                .stack_size = sizeof(static_thrd_stack[0]),
                .func = (xwosal_thrd_f)thrd_2_func,
                .arg = (void *)6,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};
struct xwosal_tcb static_tcb[xw_array_size(static_tbd)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 模块的加载函数
 */
xwer_t example_thread_create_start(void)
{
        xwer_t rc;
        xwsq_t i;


        for (i = 0; i < xw_array_size(dynamic_tbd); i++) {
                rc = xwosal_thrd_create(&dynamic_tid[i],
                                        dynamic_tbd[i].name,
                                        dynamic_tbd[i].func,
                                        dynamic_tbd[i].arg,
                                        dynamic_tbd[i].stack_size,
                                        dynamic_tbd[i].prio,
                                        dynamic_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return XWOK;

err_thrd_create:
        return rc;
}

/**
 * @brief 线程1的主函数
 */
xwer_t thrd_1_func(void * arg)
{
        xwtm_t time;
        xwer_t rc = XWOK;
        xwer_t childrc;
        xwsq_t argv = (xwsq_t)arg; /* 获取创建线程时提供的参数 */
        xwsq_t i;

        thrdcrtlogf(INFO, "Thread-1 starts.\n");

        /* 静态初始化线程2 */
        for (i = 0; i < xw_array_size(static_tbd); i++) {
                rc = xwosal_thrd_init(&static_tcb[i],
                                      static_tbd[i].name,
                                      static_tbd[i].func,
                                      static_tbd[i].arg,
                                      static_tbd[i].stack,
                                      static_tbd[i].stack_size,
                                      static_tbd[i].prio,
                                      static_tbd[i].attr);
                if (rc < 0) {
                }
        }

        /* 循环argv次 */
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                time = 1 * XWTM_S;
                xwosal_cthrd_sleep(&time);
                thrdcrtlogf(INFO, "Thread-1 says: %d.\n", argv);
                argv--;
                if (0 == argv) {
                        break;
                }
        }

        /* 结束刚创建的线程2 */
        for (i = 0; i < xw_array_size(static_tbd); i++) {
                /* 等待线程2结束，并从childrc中获取线程2的返回值 */
                rc = xwosal_thrd_terminate(xwosal_thrd_get_id(&static_tcb[i]),
                                           &childrc);
                if (rc < 0) {
                        thrdcrtlogf(ERR, "Failed to terminate %s, rc:%d.\n",
                                    static_tbd[i].name, rc);
                } else {
                        thrdcrtlogf(INFO,
                                    "%s returns: %d (%s).\n",
                                    static_tbd[i].name,
                                    childrc,
                                    strerror(-childrc));
                }
        }

        /* 线程1结束 */
        thrdcrtlogf(INFO, "Thread-1 exits. argv == %d\n", argv);
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}

/**
 * @brief 线程2的主函数
 */
xwer_t thrd_2_func(void * arg)
{
        xwer_t rc;
        xwtm_t time;
        xwsq_t argv = (xwsq_t)arg; /* 获取初始化线程时提供的参数 */

        thrdcrtlogf(INFO, "Thread-2 starts.\n");

        /* 循环argv次 */
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                time = 1 * XWTM_S;
                rc = xwosal_cthrd_sleep(&time);
                thrdcrtlogf(INFO, "Thread-2 says: %d.\n", argv);
                argv--;
                if (0 == argv) {
                        break;
                }
        }
        if (argv > 0) {
                rc = -EINTR;
        } else {
                rc = XWOK;
        }

        /* 线程2结束 */
        thrdcrtlogf(INFO, "Thread-2 exits. argv == %d\n", argv);
        xwosal_thrd_destroy(xwosal_cthrd_get_obj());
        return rc; /* 抛出返回值 */
}
