/**
 * @file
 * @brief 示例：线程的创建与退出
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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
#include <xwos/osal/thd.h>
#include <xwam/example/thread/create/mif.h>

#define LOGTAG "thdcrt"
#define thdcrtlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define THD_1_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
#define THD_2_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 2)
xwer_t thd_1_func(void * arg);
xwer_t thd_2_func(void * arg);

/**
 * @brief 动态创建的线程
 */
xwos_thd_d dynamic_thdd;

/**
 * @brief 静态初始化的线程的栈内存
 */
xwstk_t __xwcc_aligned(8) static_thd_stack[512];

/**
 * @brief 初始化为线程对象的内存
 */
struct xwos_thd static_thd;

/**
 * @brief 静态初始化的线程对象描述符
 */
xwos_thd_d static_thdd;

/**
 * @brief 模块的加载函数
 */
xwer_t example_thread_create_start(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        xwos_thd_attr_init(&attr);
        attr.name = "dynamic.thd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = THD_1_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&dynamic_thdd, &attr, thd_1_func, (void *)3);
        return rc;
}

/**
 * @brief 线程1的主函数
 */
xwer_t thd_1_func(void * arg)
{
        struct xwos_thd_attr attr;
        xwer_t rc = XWOK;
        xwer_t childrc;
        xwsq_t argv = (xwsq_t)arg; /* 获取创建线程时提供的参数 */

        thdcrtlogf(INFO, "[线程1] 开始运行。\n");

        /* 初始化static线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "static.thd";
        attr.stack = static_thd_stack;
        attr.stack_size = sizeof(static_thd_stack);
        attr.priority = THD_2_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_init(&static_thd, &static_thdd, &attr, thd_2_func, (void *)6);
        if (rc < 0) {
                thdcrtlogf(INFO, "[线程1] 初始化[%s]失败。\n", attr.name);
        }

        /* 循环argv次 */
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwos_cthd_sleep(1 * XWTM_S);
                thdcrtlogf(INFO, "[线程1] %d\n", argv);
                argv--;
                if (0 == argv) {
                        break;
                }
        }

        /* 结束刚创建的线程2 */
        rc = xwos_thd_stop(static_thdd, &childrc);
        if (rc < 0) {
                thdcrtlogf(ERR, "[线程1] 终止线程2失败，rc:%d。\n", rc);
        } else {
                thdcrtlogf(INFO, "终止线程2返回 %d (%s)。\n",
                           childrc, strerror(-childrc));
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
        xwsq_t argv = (xwsq_t)arg; /* 获取初始化线程时提供的参数 */

        thdcrtlogf(INFO, "[线程2] 开始运行。\n");

        /* 循环argv次 */
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwos_cthd_sleep(1 * XWTM_S);
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
        return rc; /* 抛出返回值 */
}
