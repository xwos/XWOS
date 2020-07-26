/**
 * @file
 * @brief 示例：信号量
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/swt.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwam/example/sync/semaphore/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SMRTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define EXAMPLE_THREAD_SLEEP_LOG_TAG        "smrthrd"
  #define smrlogf(lv, fmt, ...) \
        xwlogf(lv, EXAMPLE_THREAD_SLEEP_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define smrlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t smrthrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 线程描述表
 */
const struct xwosal_thrd_desc smrthrd_tbd = {
        .name = "Thread-1",
        .prio = SMRTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)smrthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t smrthrd_tid;

struct xwosal_swt smrswt;
xwid_t smrswtid;

struct xwosal_smr smr;
xwid_t smrid;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 模块的加载函数
 */
xwer_t example_semaphore_start(void)
{
        xwer_t rc;

        /* 初始化信号量 */
        rc = xwosal_smr_init(&smr, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr_init;
        }
        smrid = xwosal_smr_get_id(&smr);

        /* 初始化定时器 */
        rc = xwosal_swt_init(&smrswt, "smrswt", XWOSAL_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_smrswt_init;
        }
        smrswtid = xwosal_swt_get_id(&smrswt);

        /* 创建线程 */
        rc = xwosal_thrd_create(&smrthrd_tid,
                                smrthrd_tbd.name,
                                smrthrd_tbd.func,
                                smrthrd_tbd.arg,
                                smrthrd_tbd.stack_size,
                                smrthrd_tbd.prio,
                                smrthrd_tbd.attr);
        if (rc < 0) {
                goto err_thrd_create;
        }

        return XWOK;

err_thrd_create:
        xwosal_swt_destroy(&smrswt);
err_smrswt_init:
        xwosal_smr_destroy(&smr);
err_smr_init:
        return rc;
}

/**
 * @brief 定时器的回调函数
 * @note
 * - UP系统
 *   - 当配置(XWUPCFG_SD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWUPCFG_SD_BH == 0)，此函数运行在中断上下文；
 * - SMP系统
 *   - 当配置(XWSMPCFG_SD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWSMPCFG_SD_BH == 0)，此函数运行在中断上下文；
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void smrswt_callback(struct xwosal_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwosal_smr_post(smrid);
}

/**
 * @brief 线程1的主函数
 */
xwer_t smrthrd_func(void * arg)
{
        xwtm_t base, time;
        xwer_t rc;

        XWOS_UNUSED(arg);

        smrlogf(INFO, "[线程] 启动。\n");

        smrlogf(INFO, "[线程] 启动定时器。\n");
        base = xwosal_scheduler_get_timetick_lc();
        rc = xwosal_swt_start(smrswtid, base, 1000 * XWTM_MS, smrswt_callback, NULL);

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                time = 500 * XWTM_MS;
                rc = xwosal_smr_timedwait(smrid, &time);
                if (XWOK == rc) {
                        time = xwosal_scheduler_get_timestamp_lc();
                        smrlogf(INFO,
                                "[线程] 定时器唤醒，时间戳：%lld 纳秒。\n",
                                time);
                } else if (-ETIMEDOUT == rc) {
                        time = xwosal_scheduler_get_timestamp_lc();
                        smrlogf(INFO,
                                "[线程] 等待超时，时间戳：%lld 纳秒。\n",
                                time);
                }
        }

        smrlogf(INFO, "[线程] 退出。\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
