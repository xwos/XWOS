/**
 * @file
 * @brief 示例：线程栅栏
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/barrier.h>
#include <xwam/example/sync/barrier/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define EXAMPLE_BARRIER_THRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define EXAMPLE_BARRIER_LOG_TAG       "barrier"
  #define barlogf(lv, fmt, ...) \
        xwlogf(lv, EXAMPLE_BARRIER_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define barlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t example_barrier_thrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
const struct xwosal_thrd_desc example_barrier_thrd_td[] = {
        [0] = {
                .name = "example.barrier.thrd.0",
                .prio = EXAMPLE_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)example_barrier_thrd_func,
                .arg = (void *)0,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [1] = {
                .name = "example.barrier.thrd.1",
                .prio = EXAMPLE_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)example_barrier_thrd_func,
                .arg = (void *)1,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [2] = {
                .name = "example.barrier.thrd.2",
                .prio = EXAMPLE_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)example_barrier_thrd_func,
                .arg = (void *)2,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [3] = {
                .name = "example.barrier.thrd.3",
                .prio = EXAMPLE_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)example_barrier_thrd_func,
                .arg = (void *)3,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [4] = {
                .name = "example.barrier.thrd.4",
                .prio = EXAMPLE_BARRIER_THRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)example_barrier_thrd_func,
                .arg = (void *)4,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};
xwid_t example_barrier_thrd[xw_array_size(example_barrier_thrd_td)];

struct xwosal_barrier xwtst_sync_barrier;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 测试模块的启动函数
 */
xwer_t example_barrier_start(void)
{
        xwsq_t i;
        xwer_t rc;

        /* 初始化线程屏障 */
        rc = xwosal_barrier_init(&xwtst_sync_barrier);
        if (rc < 0) {
                goto err_barrier_init;
        }

        /* 创建5个线程 */
        for (i = 0; i < xw_array_size(example_barrier_thrd_td); i++) {
                rc = xwosal_thrd_create(&example_barrier_thrd[i],
                                        example_barrier_thrd_td[i].name,
                                        example_barrier_thrd_td[i].func,
                                        example_barrier_thrd_td[i].arg,
                                        example_barrier_thrd_td[i].stack_size,
                                        example_barrier_thrd_td[i].prio,
                                        example_barrier_thrd_td[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return XWOK;

err_thrd_create:
        xwosal_barrier_destroy(&xwtst_sync_barrier);
err_barrier_init:
        return rc;
}

/**
 * @brief 线程函数
 */
xwer_t example_barrier_thrd_func(void * arg)
{
        xwosal_barrier_declare_bitmap(msk);
        xwid_t barid;
        xwer_t rc;

        xwsq_t pos = (xwsq_t)arg; /* 获取线程的各自的位置 */

        barlogf(INFO, "[线程%d] 启动。\n", pos);

        xwbmpop_c0all(msk, XWOSAL_BARRIER_MAXNUM);

        /* 设置位图掩码 */
        for (xwsq_t i = 0; i < xw_array_size(example_barrier_thrd_td); i++) {
                xwbmpop_s1i(msk, i);
        }

        barid = xwosal_barrier_get_id(&xwtst_sync_barrier); /* 获取线程屏障的ID */

        /* 同步线程 */
        rc = xwosal_barrier_sync(barid, pos, msk);
        if (XWOK == rc) {
                barlogf(INFO, "[线程%d] 同步。\n", pos);
        }

        barlogf(INFO, "[线程%d] 退出。\n", pos);
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
