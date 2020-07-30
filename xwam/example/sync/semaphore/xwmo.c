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
#define XWSMRDEMO_THRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define XWSMRDEMO_LOG_TAG     "smr"
  #define smrlogf(lv, fmt, ...) \
        xwlogf(lv, XWSMRDEMO_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define smrlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwsmrdemo_thrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 线程描述表
 */
const struct xwosal_thrd_desc xwsmrdemo_tbd = {
        .name = "xwsmrdemo.thrd",
        .prio = XWSMRDEMO_THRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwsmrdemo_thrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t xwsmrdemo_tid;

struct xwosal_swt xwsmrdemo_swt;
xwid_t xwsmrdemo_swtid;

struct xwosal_smr xwsmrdemo_smr;
xwid_t xwsmrdemo_smrid;

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
        rc = xwosal_smr_init(&xwsmrdemo_smr, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr_init;
        }
        xwsmrdemo_smrid = xwosal_smr_get_id(&xwsmrdemo_smr);

        /* 初始化定时器 */
        rc = xwosal_swt_init(&xwsmrdemo_swt, "xwsmrdemo_swt",
                             XWOSAL_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt_init;
        }
        xwsmrdemo_swtid = xwosal_swt_get_id(&xwsmrdemo_swt);

        /* 创建线程 */
        rc = xwosal_thrd_create(&xwsmrdemo_tid,
                                xwsmrdemo_tbd.name,
                                xwsmrdemo_tbd.func,
                                xwsmrdemo_tbd.arg,
                                xwsmrdemo_tbd.stack_size,
                                xwsmrdemo_tbd.prio,
                                xwsmrdemo_tbd.attr);
        if (rc < 0) {
                goto err_thrd_create;
        }

        return XWOK;

err_thrd_create:
        xwosal_swt_destroy(&xwsmrdemo_swt);
err_swt_init:
        xwosal_smr_destroy(&xwsmrdemo_smr);
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
void xwsmrdemo_swt_callback(struct xwosal_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwosal_smr_post(xwsmrdemo_smrid);
}

/**
 * @brief 线程1的主函数
 */
xwer_t xwsmrdemo_thrd_func(void * arg)
{
        xwtm_t base, ts;
        xwer_t rc;

        XWOS_UNUSED(arg);

        smrlogf(INFO, "[线程] 启动。\n");

        smrlogf(INFO, "[线程] 启动定时器。\n");
        base = xwosal_scheduler_get_timetick_lc();
        rc = xwosal_swt_start(xwsmrdemo_swtid, base, 1000 * XWTM_MS,
                              xwsmrdemo_swt_callback, NULL);

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                ts = 500 * XWTM_MS;
                rc = xwosal_smr_timedwait(xwsmrdemo_smrid, &ts);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        smrlogf(INFO,
                                "[线程] 定时器唤醒，时间戳：%lld 纳秒。\n",
                                ts);
                } else if (-ETIMEDOUT == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        smrlogf(INFO,
                                "[线程] 等待超时，时间戳：%lld 纳秒。\n",
                                ts);
                }
        }

        smrlogf(INFO, "[线程] 退出。\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
