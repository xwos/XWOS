/**
 * @file
 * @brief 示例：线程退出
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
#include <xwam/example/thd/exit/mif.h>

#define LOGTAG "thdexit"
#define thdexitlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define THDEXITDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t thdexitdemo_detached_thd0_func(void * arg);
xwer_t thdexitdemo_detached_thd1_func(void * arg);
xwer_t thdexitdemo_parent_thd_func(void * arg);
xwer_t thdexitdemo_child0_thd_func(void * arg);
xwer_t thdexitdemo_child1_thd_func(void * arg);

/**
 * @brief Detached线程描述表
 */
const struct xwos_thd_desc thdexitdemo_detached_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "thdexitdemo.detached.thd.0",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = THDEXITDEMO_THD_PRIORITY,
                        .detached = true,
                        .privileged = true,
                },
                .func = (xwos_thd_f)thdexitdemo_detached_thd0_func,
                .arg = NULL,
        },
        [1] = {
                .attr = {
                        .name = "thdexitdemo.detached.thd.1",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = THDEXITDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)thdexitdemo_detached_thd1_func,
                .arg = NULL,
        },
};
xwos_thd_d thdexitdemo_detached_thd[xw_array_size(thdexitdemo_detached_thd_desc)];

/**
 * @brief Joinable线程描述表
 */
const struct xwos_thd_desc thdexitdemo_joinable_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "thdexitdemo.parent.thd",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = THDEXITDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)thdexitdemo_parent_thd_func,
                .arg = NULL,
        },
        [1] = {
                .attr = {
                        .name = "thdexitdemo.child.thd.0",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = THDEXITDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)thdexitdemo_child0_thd_func,
                .arg = NULL,
        },
        [2] = {
                .attr = {
                        .name = "thdexitdemo.child.thd.1",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = THDEXITDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)thdexitdemo_child1_thd_func,
                .arg = NULL,
        },
};
xwos_thd_d thdexitdemo_parent_thd;
xwos_thd_d thdexitdemo_child0_thd;
xwos_thd_d thdexitdemo_child1_thd;

/**
 * @brief 模块的加载函数
 */
xwer_t xwos_example_thd_exit(void)
{
        xwer_t rc;
        xwsq_t i;


        for (i = 0; i < xw_array_size(thdexitdemo_detached_thd_desc); i++) {
                rc = xwos_thd_create(&thdexitdemo_detached_thd[i],
                                     &thdexitdemo_detached_thd_desc[i].attr,
                                     thdexitdemo_detached_thd_desc[i].func,
                                     thdexitdemo_detached_thd_desc[i].arg);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }

        rc = xwos_thd_create(&thdexitdemo_parent_thd,
                             &thdexitdemo_joinable_thd_desc[0].attr,
                             thdexitdemo_joinable_thd_desc[0].func,
                             thdexitdemo_joinable_thd_desc[0].arg);
        if (rc < 0) {
                goto err_thd_create;
        }

        return XWOK;

err_thd_create:
        return rc;
}

/**
 * @brief Detached线程0的主函数
 */
xwer_t thdexitdemo_detached_thd0_func(void * arg)
{
        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Detached][线程0]启动\n");
        thdexitlogf(INFO, "[Detached][线程0] sleep(500ms) ...\n");
        xwos_cthd_sleep(XWTM_MS(500));
        thdexitlogf(INFO, "[Detached][线程0] 退出 ...\n");
        xwos_cthd_exit(XWOK);
        return XWOK;
}

/**
 * @brief Detached线程1的主函数
 */
xwer_t thdexitdemo_detached_thd1_func(void * arg)
{
        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Detached][线程1] 启动\n");
        thdexitlogf(INFO, "[Detached][线程1] Detech\n");
        xwos_thd_detach(xwos_cthd_self());
        thdexitlogf(INFO, "[Detached][线程1] sleep(500ms) ...\n");
        xwos_cthd_sleep(XWTM_MS(500));
        thdexitlogf(INFO, "[Detached][线程1] 退出 ...\n");
        return XWOK;
}

/**
 * @brief Joinable父线程的主函数
 */
xwer_t thdexitdemo_parent_thd_func(void * arg)
{
        xwer_t rc, childrc;

        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Joinable][父线程] 启动\n");
        thdexitlogf(INFO, "[Joinable][父线程] sleep(500ms)\n");
        xwos_cthd_sleep(XWTM_MS(500));

        rc = xwos_thd_create(&thdexitdemo_child0_thd,
                             &thdexitdemo_joinable_thd_desc[1].attr,
                             thdexitdemo_joinable_thd_desc[1].func,
                             thdexitdemo_joinable_thd_desc[1].arg);
        if (XWOK == rc) {
                thdexitlogf(INFO, "[Joinable][父线程] join(子线程0) ...\n");
                rc = xwos_thd_join(thdexitdemo_child0_thd, &childrc);
                thdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n", childrc);
        }

        rc = xwos_thd_create(&thdexitdemo_child1_thd,
                             &thdexitdemo_joinable_thd_desc[2].attr,
                             thdexitdemo_joinable_thd_desc[2].func,
                             thdexitdemo_joinable_thd_desc[2].arg);
        if (XWOK == rc) {
                thdexitlogf(INFO, "[Joinable][父线程] sleep(500ms) ...\n");
                xwos_cthd_sleep(XWTM_MS(500));
                thdexitlogf(INFO, "[Joinable][父线程] stop(子线程1) ...\n");
                rc = xwos_thd_stop(thdexitdemo_child1_thd, &childrc);
                thdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n", childrc);
        }

        rc = xwos_thd_create(&thdexitdemo_child1_thd,
                             &thdexitdemo_joinable_thd_desc[2].attr,
                             thdexitdemo_joinable_thd_desc[2].func,
                             thdexitdemo_joinable_thd_desc[2].arg);
        if (XWOK == rc) {
                thdexitlogf(INFO, "[Joinable][父线程] sleep(500ms) ...\n");
                xwos_cthd_sleep(XWTM_MS(500));
                thdexitlogf(INFO, "[Joinable][父线程] quit(子线程1) ...\n");
                rc = xwos_thd_quit(thdexitdemo_child1_thd);
                if (XWOK == rc) {
                        thdexitlogf(INFO, "[Joinable][父线程] join(子线程1) ...\n");
                        rc = xwos_thd_join(thdexitdemo_child1_thd, &childrc);
                        thdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n",
                                    childrc);
                }
        }

        /* 退出前变成分离态，以便系统回收资源 */
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief Joinable子线程的主函数
 */
xwer_t thdexitdemo_child0_thd_func(void * arg)
{
        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Joinable][子线程0] 启动\n");
        thdexitlogf(INFO, "[Joinable][子线程0] sleep(500ms) ...\n");
        xwos_cthd_sleep(XWTM_MS(500));
        return -ECHILD;
}

/**
 * @brief Joinable子线程的主函数
 */
xwer_t thdexitdemo_child1_thd_func(void * arg)
{
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = XWOK;
        thdexitlogf(INFO, "[Joinable][子线程1] 启动\n");
        while (!xwos_cthd_shld_stop()) {
                thdexitlogf(INFO, "[Joinable][子线程1] sleep(500ms) ...\n");
                rc = xwos_cthd_sleep(XWTM_MS(500));
                thdexitlogf(INFO, "[Joinable][子线程1] 唤醒\n");
        }
        return rc;
}
