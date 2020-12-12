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

#define XWEXITDEMO_THRD_PRIORITY                                \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
#define EXAMPLE_THREAD_EXIT_LOG_TAG        "thrdexit"
#define thrdexitlogf(lv, fmt, ...)                              \
        xwlogf(lv, EXAMPLE_THREAD_EXIT_LOG_TAG, "<%lld>" fmt,   \
               xwos_skd_get_timetick_lc(), ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
#define thrdexitlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

xwer_t xwexitdemo_detached_thrd0_func(void * arg);
xwer_t xwexitdemo_detached_thrd1_func(void * arg);
xwer_t xwexitdemo_parent_thrd_func(void * arg);
xwer_t xwexitdemo_child0_thrd_func(void * arg);
xwer_t xwexitdemo_child1_thrd_func(void * arg);

/**
 * @brief Detached线程描述表
 */
const struct xwos_thrd_desc xwexitdemo_detached_tbd[] = {
        [0] = {
                .name = "xwexitdemo.detached.thrd.0",
                .prio = XWEXITDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwexitdemo_detached_thrd0_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED | XWOS_SKDATTR_DETACHED,
        },
        [1] = {
                .name = "xwexitdemo.detached.thrd.1",
                .prio = XWEXITDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwexitdemo_detached_thrd1_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
xwid_t xwexitdemo_detached_tid[xw_array_size(xwexitdemo_detached_tbd)];

/**
 * @brief Joinable线程描述表
 */
const struct xwos_thrd_desc xwexitdemo_joinable_tbd[] = {
        [0] = {
                .name = "xwexitdemo.parent.thrd",
                .prio = XWEXITDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwexitdemo_parent_thrd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [1] = {
                .name = "xwexitdemo.child.thrd.0",
                .prio = XWEXITDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwexitdemo_child0_thrd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [2] = {
                .name = "xwexitdemo.child.thrd.1",
                .prio = XWEXITDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwexitdemo_child1_thrd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
xwid_t xwexitdemo_parent_tid;
xwid_t xwexitdemo_child0_tid;
xwid_t xwexitdemo_child1_tid;

/**
 * @brief 模块的加载函数
 */
xwer_t example_thread_exit_start(void)
{
        xwer_t rc;
        xwsq_t i;


        for (i = 0; i < xw_array_size(xwexitdemo_detached_tbd); i++) {
                rc = xwos_thrd_create(&xwexitdemo_detached_tid[i],
                                      xwexitdemo_detached_tbd[i].name,
                                      xwexitdemo_detached_tbd[i].func,
                                      xwexitdemo_detached_tbd[i].arg,
                                      xwexitdemo_detached_tbd[i].stack_size,
                                      xwexitdemo_detached_tbd[i].prio,
                                      xwexitdemo_detached_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        rc = xwos_thrd_create(&xwexitdemo_parent_tid,
                              xwexitdemo_joinable_tbd[0].name,
                              xwexitdemo_joinable_tbd[0].func,
                              xwexitdemo_joinable_tbd[0].arg,
                              xwexitdemo_joinable_tbd[0].stack_size,
                              xwexitdemo_joinable_tbd[0].prio,
                              xwexitdemo_joinable_tbd[0].attr);
        if (rc < 0) {
                goto err_thrd_create;
        }

        return XWOK;

err_thrd_create:
        return rc;
}

/**
 * @brief Detached线程0的主函数
 */
xwer_t xwexitdemo_detached_thrd0_func(void * arg)
{
        xwtm_t time;

        XWOS_UNUSED(arg);

        thrdexitlogf(INFO, "[Detached][线程0]启动\n");
        thrdexitlogf(INFO, "[Detached][线程0] sleep(1s) ...\n");
        time = 1 * XWTM_S;
        xwos_cthrd_sleep(&time);
        thrdexitlogf(INFO, "[Detached][线程0] 退出 ...\n");
        xwos_cthrd_exit(XWOK);
        return XWOK;
}

/**
 * @brief Detached线程1的主函数
 */
xwer_t xwexitdemo_detached_thrd1_func(void * arg)
{
        xwtm_t time;

        XWOS_UNUSED(arg);

        thrdexitlogf(INFO, "[Detached][线程1] 启动\n");
        thrdexitlogf(INFO, "[Detached][线程1] Detech\n");
        xwos_thrd_detach(xwos_cthrd_id());
        thrdexitlogf(INFO, "[Detached][线程1] sleep(2s) ...\n");
        time = 2 * XWTM_S;
        xwos_cthrd_sleep(&time);
        thrdexitlogf(INFO, "[Detached][线程1] 退出 ...\n");
        return XWOK;
}

/**
 * @brief Joinable父线程的主函数
 */
xwer_t xwexitdemo_parent_thrd_func(void * arg)
{
        xwer_t rc, childrc;
        xwtm_t time;

        XWOS_UNUSED(arg);

        thrdexitlogf(INFO, "[Joinable][父线程] 启动\n");

        thrdexitlogf(INFO, "[Joinable][父线程] sleep(3s)\n");
        time = 3 * XWTM_S;
        xwos_cthrd_sleep(&time);

        rc = xwos_thrd_create(&xwexitdemo_child0_tid,
                              xwexitdemo_joinable_tbd[1].name,
                              xwexitdemo_joinable_tbd[1].func,
                              xwexitdemo_joinable_tbd[1].arg,
                              xwexitdemo_joinable_tbd[1].stack_size,
                              xwexitdemo_joinable_tbd[1].prio,
                              xwexitdemo_joinable_tbd[1].attr);
        if (XWOK == rc) {
                thrdexitlogf(INFO, "[Joinable][父线程] join(子线程0) ...\n");
                rc = xwos_thrd_join(xwexitdemo_child0_tid, &childrc);
                thrdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n", childrc);
        }

        rc = xwos_thrd_create(&xwexitdemo_child1_tid,
                              xwexitdemo_joinable_tbd[2].name,
                              xwexitdemo_joinable_tbd[2].func,
                              xwexitdemo_joinable_tbd[2].arg,
                              xwexitdemo_joinable_tbd[2].stack_size,
                              xwexitdemo_joinable_tbd[2].prio,
                              xwexitdemo_joinable_tbd[2].attr);
        if (XWOK == rc) {
                thrdexitlogf(INFO, "[Joinable][父线程] sleep(1s) ...\n");
                time = 1 * XWTM_S;
                xwos_cthrd_sleep(&time);
                thrdexitlogf(INFO, "[Joinable][父线程] stop(子线程1) ...\n");
                rc = xwos_thrd_stop(xwexitdemo_child1_tid, &childrc);
                thrdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n", childrc);
        }

        rc = xwos_thrd_create(&xwexitdemo_child1_tid,
                              xwexitdemo_joinable_tbd[2].name,
                              xwexitdemo_joinable_tbd[2].func,
                              xwexitdemo_joinable_tbd[2].arg,
                              xwexitdemo_joinable_tbd[2].stack_size,
                              xwexitdemo_joinable_tbd[2].prio,
                              xwexitdemo_joinable_tbd[2].attr);
        if (XWOK == rc) {
                thrdexitlogf(INFO, "[Joinable][父线程] sleep(1s) ...\n");
                time = 1 * XWTM_S;
                xwos_cthrd_sleep(&time);
                thrdexitlogf(INFO, "[Joinable][父线程] cancel(子线程1) ...\n");
                rc = xwos_thrd_cancel(xwexitdemo_child1_tid);
                if (XWOK == rc) {
                        thrdexitlogf(INFO, "[Joinable][父线程] join(子线程1) ...\n");
                        rc = xwos_thrd_join(xwexitdemo_child1_tid, &childrc);
                        thrdexitlogf(INFO, "[Joinable][父线程] 子线程返回值：%d\n",
                                     childrc);
                }
        }

        xwos_thrd_delete(xwos_cthrd_id());
        return rc;
}

/**
 * @brief Joinable子线程的主函数
 */
xwer_t xwexitdemo_child0_thrd_func(void * arg)
{
        xwtm_t time;

        XWOS_UNUSED(arg);

        thrdexitlogf(INFO, "[Joinable][子线程0] 启动\n");
        thrdexitlogf(INFO, "[Joinable][子线程0] sleep(4s) ...\n");
        time = 4 * XWTM_S;
        xwos_cthrd_sleep(&time);
        return -ECHILD;
}

/**
 * @brief Joinable子线程的主函数
 */
xwer_t xwexitdemo_child1_thrd_func(void * arg)
{
        xwtm_t time;
        xwer_t rc;

        XWOS_UNUSED(arg);

        rc = XWOK;
        thrdexitlogf(INFO, "[Joinable][子线程1] 启动\n");
        while (!xwos_cthrd_shld_stop()) {
                thrdexitlogf(INFO, "[Joinable][子线程1] sleep(2s) ...\n");
                time = 2 * XWTM_S;
                rc = xwos_cthrd_sleep(&time);
        }
        return rc;
}
