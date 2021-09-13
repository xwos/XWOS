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

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwam/example/thread/create/mif.h>

#define LOGTAG "thdcrt"
#define thdcrtlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define THD_1_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
#define THD_2_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 2)
xwer_t thd_1_func(void * arg);
xwer_t thd_2_func(void * arg);

/**
 * @brief 动态创建的线程描述表
 */
const struct xwos_thd_desc dynamic_thd_desc[] = {
        [0] = {
                .name = "Thread-1",
                .prio = THD_1_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)thd_1_func,
                .arg = (void *)3,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd * dynamic_thd[xw_array_size(dynamic_thd_desc)];

/**
 * @brief 静态初始化的线程的栈内存
 */
xwstk_t __xwcc_aligned(8) static_thd_stack[1][512]; /* 大小 == sizeof(xwstk_t) * 256 */

/**
 * @brief 静态初始化的线程描述表
 */
const struct xwos_thd_desc static_thd_desc[] = {
        [0] = {
                .name = "Thread-2",
                .prio = THD_2_PRIORITY,
                .stack = static_thd_stack[0],
                .stack_size = sizeof(static_thd_stack[0]),
                .func = (xwos_thd_f)thd_2_func,
                .arg = (void *)6,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd static_thd[xw_array_size(static_thd_desc)];

/**
 * @brief 模块的加载函数
 */
xwer_t example_thread_create_start(void)
{
        xwer_t rc;
        xwsq_t i;

        /* 建立dynamic_thd_desc中的线程 */
        for (i = 0; i < xw_array_size(dynamic_thd_desc); i++) {
                rc = xwos_thd_create(&dynamic_thd[i],
                                     dynamic_thd_desc[i].name,
                                     dynamic_thd_desc[i].func,
                                     dynamic_thd_desc[i].arg,
                                     dynamic_thd_desc[i].stack_size,
                                     dynamic_thd_desc[i].prio,
                                     dynamic_thd_desc[i].attr);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }

        return XWOK;

err_thd_create:
        return rc;
}

/**
 * @brief 线程1的主函数
 */
xwer_t thd_1_func(void * arg)
{
        xwtm_t time;
        xwer_t rc = XWOK;
        xwer_t childrc;
        xwsq_t argv = (xwsq_t)arg; /* 获取创建线程时提供的参数 */
        xwsq_t i;

        thdcrtlogf(INFO, "[线程1] 开始运行。\n");

        /* 初始化static_thd_desc中的线程 */
        for (i = 0; i < xw_array_size(static_thd_desc); i++) {
                rc = xwos_thd_init(&static_thd[i],
                                   static_thd_desc[i].name,
                                   static_thd_desc[i].func,
                                   static_thd_desc[i].arg,
                                   static_thd_desc[i].stack,
                                   static_thd_desc[i].stack_size,
                                   static_thd_desc[i].prio,
                                   static_thd_desc[i].attr);
                if (rc < 0) {
                        thdcrtlogf(INFO, "[线程1] 初始化[%s]失败。\n",
                                   static_thd_desc[i].name);
                }
        }

        /* 循环argv次 */
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                time = 1 * XWTM_S;
                xwos_cthd_sleep(&time);
                thdcrtlogf(INFO, "[线程1] %d\n", argv);
                argv--;
                if (0 == argv) {
                        break;
                }
        }

        /* 结束刚创建的线程2 */
        for (i = 0; i < xw_array_size(static_thd_desc); i++) {
                /* 等待线程2结束，并从childrc中获取线程2的返回值 */
                rc = xwos_thd_stop(&static_thd[i], &childrc);
                if (rc < 0) {
                        thdcrtlogf(ERR, "[线程1] 终止线程 [%s] 失败，rc:%d。\n",
                                   static_thd_desc[i].name, rc);
                } else {
                        thdcrtlogf(INFO,
                                   "[%s] 返回 %d (%s)。\n",
                                   static_thd_desc[i].name,
                                   childrc,
                                   strerror(-childrc));
                }
        }

        thdcrtlogf(INFO, "[线程1] 退出，argv：%d。\n", argv);
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 线程2的主函数
 */
xwer_t thd_2_func(void * arg)
{
        xwer_t rc;
        xwtm_t time;
        xwsq_t argv = (xwsq_t)arg; /* 获取初始化线程时提供的参数 */

        thdcrtlogf(INFO, "[线程2] 开始运行。\n");

        /* 循环argv次 */
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                time = 1 * XWTM_S;
                rc = xwos_cthd_sleep(&time);
                thdcrtlogf(INFO, "[线程2] %d\n", argv);
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

        thdcrtlogf(INFO, "[线程2] 退出，argv：%d。\n", argv);
        xwos_thd_detach(xwos_cthd_self());
        return rc; /* 抛出返回值 */
}
