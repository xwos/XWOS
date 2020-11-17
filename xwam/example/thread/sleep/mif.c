/**
 * @file
 * @brief 示例：线程睡眠
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
#include <xwos/osal/skd.h>
#include <xwam/example/thread/sleep/mif.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSLPDEMO_THRD_PRIORITY                                 \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
#define EXAMPLE_THREAD_SLEEP_LOG_TAG        "thrdsleep"
#define thrdslplogf(lv, fmt, ...)                               \
        xwlogf(lv, EXAMPLE_THREAD_SLEEP_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
#define thrdslplogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwslpdemo_thrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 动态创建的线程描述表
 */
const struct xwos_thrd_desc xwslpdemo_tbd[] = {
        [0] = {
                .name = "xwslpdemo.thrd",
                .prio = XWSLPDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwslpdemo_thrd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
xwid_t xwslpdemo_tid[xw_array_size(xwslpdemo_tbd)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 模块的加载函数
 */
xwer_t example_thread_sleep_start(void)
{
        xwer_t rc;
        xwsq_t i;


        for (i = 0; i < xw_array_size(xwslpdemo_tbd); i++) {
                rc = xwos_thrd_create(&xwslpdemo_tid[i],
                                      xwslpdemo_tbd[i].name,
                                      xwslpdemo_tbd[i].func,
                                      xwslpdemo_tbd[i].arg,
                                      xwslpdemo_tbd[i].stack_size,
                                      xwslpdemo_tbd[i].prio,
                                      xwslpdemo_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return XWOK;

err_thrd_create:
        return rc;
}

/**
 * @brief 线程的主函数
 */
xwer_t xwslpdemo_thrd_func(void * arg)
{
        xwtm_t tk, ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        thrdslplogf(INFO, "[线程] 启动。\n");

        thrdslplogf(INFO, "[线程] 睡眠1秒 ...\n");
        tk = 1 * XWTM_S;
        xwos_cthrd_sleep(&tk);

        tk = xwos_skd_get_timetick_lc();
        while (!xwos_cthrd_frz_shld_stop(NULL)) {
                xwos_cthrd_sleep_from(&tk, 500 * XWTM_MS);
                ts = xwos_skd_get_timestamp_lc();
                thrdslplogf(INFO, "[线程] 时间戳：%lld 纳秒。\n", ts);
        }

        thrdslplogf(INFO, "[线程] 退出。\n");
        xwos_thrd_delete(xwos_cthrd_id());
        return rc;
}
