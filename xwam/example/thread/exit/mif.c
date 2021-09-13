/**
 * @file
 * @brief 示例：线程退出
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
#include <xwam/example/thread/exit/mif.h>

#define LOGTAG "thdexit"
#define thdexitlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWEXITDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t xwexitdemo_detached_thd0_func(void * arg);
xwer_t xwexitdemo_detached_thd1_func(void * arg);
xwer_t xwexitdemo_parent_thd_func(void * arg);
xwer_t xwexitdemo_child0_thd_func(void * arg);
xwer_t xwexitdemo_child1_thd_func(void * arg);

/**
 * @brief Detached线程描述表
 */
const struct xwos_thd_desc xwexitdemo_detached_thd_desc[] = {
        [0] = {
                .name = "xwexitdemo.detached.thd.0",
                .prio = XWEXITDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwexitdemo_detached_thd0_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED | XWOS_SKDATTR_DETACHED,
        },
        [1] = {
                .name = "xwexitdemo.detached.thd.1",
                .prio = XWEXITDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwexitdemo_detached_thd1_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd * xwexitdemo_detached_thd[xw_array_size(xwexitdemo_detached_thd_desc)];

/**
 * @brief Joinable线程描述表
 */
const struct xwos_thd_desc xwexitdemo_joinable_thd_desc[] = {
        [0] = {
                .name = "xwexitdemo.parent.thd",
                .prio = XWEXITDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwexitdemo_parent_thd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [1] = {
                .name = "xwexitdemo.child.thd.0",
                .prio = XWEXITDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwexitdemo_child0_thd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [2] = {
                .name = "xwexitdemo.child.thd.1",
                .prio = XWEXITDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwexitdemo_child1_thd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd * xwexitdemo_parent_thd;
struct xwos_thd * xwexitdemo_child0_thd;
struct xwos_thd * xwexitdemo_child1_thd;

/**
 * @brief 模块的加载函数
 */
xwer_t example_thread_exit_start(void)
{
        xwer_t rc;
        xwsq_t i;


        for (i = 0; i < xw_array_size(xwexitdemo_detached_thd_desc); i++) {
                rc = xwos_thd_create(&xwexitdemo_detached_thd[i],
                                     xwexitdemo_detached_thd_desc[i].name,
                                     xwexitdemo_detached_thd_desc[i].func,
                                     xwexitdemo_detached_thd_desc[i].arg,
                                     xwexitdemo_detached_thd_desc[i].stack_size,
                                     xwexitdemo_detached_thd_desc[i].prio,
                                     xwexitdemo_detached_thd_desc[i].attr);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }

        rc = xwos_thd_create(&xwexitdemo_parent_thd,
                             xwexitdemo_joinable_thd_desc[0].name,
                             xwexitdemo_joinable_thd_desc[0].func,
                             xwexitdemo_joinable_thd_desc[0].arg,
                             xwexitdemo_joinable_thd_desc[0].stack_size,
                             xwexitdemo_joinable_thd_desc[0].prio,
                             xwexitdemo_joinable_thd_desc[0].attr);
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
xwer_t xwexitdemo_detached_thd0_func(void * arg)
{
        xwtm_t time;

        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Detached][线程0]启动\n");
        thdexitlogf(INFO, "[Detached][线程0] sleep(1s) ...\n");
        time = 1 * XWTM_S;
        xwos_cthd_sleep(&time);
        thdexitlogf(INFO, "[Detached][线程0] 退出 ...\n");
        xwos_cthd_exit(XWOK);
        return XWOK;
}

/**
 * @brief Detached线程1的主函数
 */
xwer_t xwexitdemo_detached_thd1_func(void * arg)
{
        xwtm_t time;

        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Detached][线程1] 启动\n");
        thdexitlogf(INFO, "[Detached][线程1] Detech\n");
        xwos_thd_detach(xwos_cthd_self());
        thdexitlogf(INFO, "[Detached][线程1] sleep(2s) ...\n");
        time = 2 * XWTM_S;
        xwos_cthd_sleep(&time);
        thdexitlogf(INFO, "[Detached][线程1] 退出 ...\n");
        return XWOK;
}

/**
 * @brief Joinable父线程的主函数
 */
xwer_t xwexitdemo_parent_thd_func(void * arg)
{
        xwer_t rc, childrc;
        xwtm_t time;

        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Joinable][父线程] 启动\n");

        thdexitlogf(INFO, "[Joinable][父线程] sleep(3s)\n");
        time = 3 * XWTM_S;
        xwos_cthd_sleep(&time);

        rc = xwos_thd_create(&xwexitdemo_child0_thd,
                             xwexitdemo_joinable_thd_desc[1].name,
                             xwexitdemo_joinable_thd_desc[1].func,
                             xwexitdemo_joinable_thd_desc[1].arg,
                             xwexitdemo_joinable_thd_desc[1].stack_size,
                             xwexitdemo_joinable_thd_desc[1].prio,
                             xwexitdemo_joinable_thd_desc[1].attr);
        if (XWOK == rc) {
                thdexitlogf(INFO, "[Joinable][父线程] join(子线程0) ...\n");
                rc = xwos_thd_join(xwexitdemo_child0_thd, &childrc);
                thdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n", childrc);
        }

        rc = xwos_thd_create(&xwexitdemo_child1_thd,
                             xwexitdemo_joinable_thd_desc[2].name,
                             xwexitdemo_joinable_thd_desc[2].func,
                             xwexitdemo_joinable_thd_desc[2].arg,
                             xwexitdemo_joinable_thd_desc[2].stack_size,
                             xwexitdemo_joinable_thd_desc[2].prio,
                             xwexitdemo_joinable_thd_desc[2].attr);
        if (XWOK == rc) {
                thdexitlogf(INFO, "[Joinable][父线程] sleep(1s) ...\n");
                time = 1 * XWTM_S;
                xwos_cthd_sleep(&time);
                thdexitlogf(INFO, "[Joinable][父线程] stop(子线程1) ...\n");
                rc = xwos_thd_stop(xwexitdemo_child1_thd, &childrc);
                thdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n", childrc);
        }

        rc = xwos_thd_create(&xwexitdemo_child1_thd,
                             xwexitdemo_joinable_thd_desc[2].name,
                             xwexitdemo_joinable_thd_desc[2].func,
                             xwexitdemo_joinable_thd_desc[2].arg,
                             xwexitdemo_joinable_thd_desc[2].stack_size,
                             xwexitdemo_joinable_thd_desc[2].prio,
                             xwexitdemo_joinable_thd_desc[2].attr);
        if (XWOK == rc) {
                thdexitlogf(INFO, "[Joinable][父线程] sleep(1s) ...\n");
                time = 1 * XWTM_S;
                xwos_cthd_sleep(&time);
                thdexitlogf(INFO, "[Joinable][父线程] cancel(子线程1) ...\n");
                rc = xwos_thd_cancel(xwexitdemo_child1_thd);
                if (XWOK == rc) {
                        thdexitlogf(INFO, "[Joinable][父线程] join(子线程1) ...\n");
                        rc = xwos_thd_join(xwexitdemo_child1_thd, &childrc);
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
xwer_t xwexitdemo_child0_thd_func(void * arg)
{
        xwtm_t time;

        XWOS_UNUSED(arg);

        thdexitlogf(INFO, "[Joinable][子线程0] 启动\n");
        thdexitlogf(INFO, "[Joinable][子线程0] sleep(4s) ...\n");
        time = 4 * XWTM_S;
        xwos_cthd_sleep(&time);
        return -ECHILD;
}

/**
 * @brief Joinable子线程的主函数
 */
xwer_t xwexitdemo_child1_thd_func(void * arg)
{
        xwtm_t time;
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = XWOK;
        thdexitlogf(INFO, "[Joinable][子线程1] 启动\n");
        while (!xwos_cthd_shld_stop()) {
                thdexitlogf(INFO, "[Joinable][子线程1] sleep(2s) ...\n");
                time = 2 * XWTM_S;
                rc = xwos_cthd_sleep(&time);
        }
        return rc;
}
