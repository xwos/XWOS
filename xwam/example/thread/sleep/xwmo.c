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
#include <xwam/example/thread/sleep/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SLEEPTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define EXAMPLE_THREAD_SLEEP_LOG_TAG        "thrdsleep"
  #define thrdslplogf(lv, fmt, ...) \
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
xwer_t sleepthrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 动态创建的线程描述表
 */
const struct xwosal_thrd_desc sleepthrd_tbd[] = {
        [0] = {
                .name = "Thread-1",
                .prio = SLEEPTHRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)sleepthrd_func,
                .arg = NULL,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};
xwid_t sleepthrd_tid[xw_array_size(sleepthrd_tbd)];

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


        for (i = 0; i < xw_array_size(sleepthrd_tbd); i++) {
                rc = xwosal_thrd_create(&sleepthrd_tid[i],
                                        sleepthrd_tbd[i].name,
                                        sleepthrd_tbd[i].func,
                                        sleepthrd_tbd[i].arg,
                                        sleepthrd_tbd[i].stack_size,
                                        sleepthrd_tbd[i].prio,
                                        sleepthrd_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_sleep;
                }
        }

        return XWOK;

err_thrd_sleep:
        return rc;
}

/**
 * @brief 线程的主函数
 */
xwer_t sleepthrd_func(void * arg)
{
        xwtm_t timetick, timestamp;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        thrdslplogf(INFO, "Thread starts.\n");

        thrdslplogf(INFO, "Thread sleeps 1s ...\n");
        timetick = 1 * XWTM_S;
        xwosal_cthrd_sleep(&timetick);

        timetick = xwosal_scheduler_get_timetick_lc();
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwosal_cthrd_sleep_from(&timetick, 500 * XWTM_MS);
                timestamp = xwosal_scheduler_get_timestamp_lc();
                thrdslplogf(INFO, "current timestamp: %lld ns.\n", timestamp);
        }

        thrdslplogf(INFO, "Thread exits.\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
