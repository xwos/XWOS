/**
 * @file
 * @brief 示例：软件定时器
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
#include <xwos/osal/sync/flag.h>
#include <xwam/example/timer/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSWTDEMO_THRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define XWSWTDEMO_LOG_TAG     "swt"
  #define swtlogf(lv, fmt, ...) \
        xwlogf(lv, XWSWTDEMO_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define swtlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwswtdemo_thrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 线程描述表
 */
const struct xwosal_thrd_desc xwswtdemo_tbd = {
        .name = "xwswtdemo.thrd",
        .prio = XWSWTDEMO_THRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwswtdemo_thrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t xwswtdemo_tid;

struct xwosal_swt xwswtdemo_swt0;
xwid_t xwswtdemo_swt0id;
xwid_t xwswtdemo_swt1id;

struct xwosal_flg swtflg;
xwid_t swtflgid;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 模块的加载函数
 */
xwer_t example_timer_start(void)
{
        xwer_t rc;

        /* 初始化事件信号旗 */
        rc = xwosal_flg_init(&swtflg, NULL);
        if (rc < 0) {
                goto err_flg_init;
        }
        swtflgid = xwosal_flg_get_id(&swtflg);

        /* 初始化定时器0 */
        rc = xwosal_swt_init(&xwswtdemo_swt0, "xwswtdemo_swt0",
                             XWOSAL_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt0_init;
        }
        xwswtdemo_swt0id = xwosal_swt_get_id(&xwswtdemo_swt0);

        /* 创建定时器1 */
        rc = xwosal_swt_create(&xwswtdemo_swt1id, "xwswtdemo_swt1",
                               XWOSAL_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt1_create;
        }

        /* 创建线程 */
        rc = xwosal_thrd_create(&xwswtdemo_tid,
                                xwswtdemo_tbd.name,
                                xwswtdemo_tbd.func,
                                xwswtdemo_tbd.arg,
                                xwswtdemo_tbd.stack_size,
                                xwswtdemo_tbd.prio,
                                xwswtdemo_tbd.attr);
        if (rc < 0) {
                goto err_thrd_create;
        }

        return XWOK;

err_thrd_create:
        xwosal_swt_delete(xwswtdemo_swt1id);
err_swt1_create:
        xwosal_swt_destroy(&xwswtdemo_swt0);
err_swt0_init:
        xwosal_flg_destroy(&swtflg);
err_flg_init:
        return rc;
}

/**
 * @brief 定时器0的回调函数
 * @note
 * - UP系统
 *   - 当配置(XWUPCFG_SD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWUPCFG_SD_BH == 0)，此函数运行在中断上下文；
 * - SMP系统
 *   - 当配置(XWSMPCFG_SD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWSMPCFG_SD_BH == 0)，此函数运行在中断上下文；
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void xwswtdemo_swt0_callback(struct xwosal_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwosal_flg_s1i(swtflgid, 0);
}

/**
 * @brief 定时器1的回调函数
 * @note
 * - UP系统
 *   - 当配置(XWUPCFG_SD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWUPCFG_SD_BH == 0)，此函数运行在中断上下文；
 * - SMP系统
 *   - 当配置(XWSMPCFG_SD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWSMPCFG_SD_BH == 0)，此函数运行在中断上下文；
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void xwswtdemo_swt1_callback(struct xwosal_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwosal_flg_s1i(swtflgid, 1);
}

/**
 * @brief 线程1的主函数
 */
xwer_t xwswtdemo_thrd_func(void * arg)
{
        xwtm_t base, ts;
        xwer_t rc;
        xwosal_flg_declare_bitmap(msk);
        xwosal_flg_declare_bitmap(trg);

        XWOS_UNUSED(arg);

        swtlogf(INFO, "[线程] 启动。\n");

        base = xwosal_scheduler_get_timetick_lc();

        swtlogf(INFO, "[线程] 启动定时器0。\n");
        rc = xwosal_swt_start(xwswtdemo_swt0id,
                              base, 500 * XWTM_MS,
                              xwswtdemo_swt0_callback, NULL);

        swtlogf(INFO, "[线程] 启动定时器1。\n");
        rc = xwosal_swt_start(xwswtdemo_swt1id,
                              base, 800 * XWTM_MS,
                              xwswtdemo_swt1_callback, NULL);

        /* 设置掩码位为bit0:1共2位 */
        memset(msk, 0, sizeof(msk));
        msk[0] = BIT(0) | BIT(1);
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                /* 等待事件 */
                rc = xwosal_flg_wait(swtflgid,
                                     XWOSAL_FLG_TRIGGER_SET_ANY,
                                     XWOSAL_FLG_ACTION_CONSUMPTION,
                                     trg, msk);
                if (XWOK == rc) {
                        /* 测试第0位是否被置1 */
                        if (xwbmpop_t1i(trg, 0)) {
                                ts = xwosal_scheduler_get_timestamp_lc();
                                swtlogf(INFO,
                                        "[线程] 定时器0唤醒，时间戳：%lld 纳秒。\n",
                                        ts);
                        }

                        /* 测试第1位是否被置1 */
                        if (xwbmpop_t1i(trg, 1)) {
                                ts = xwosal_scheduler_get_timestamp_lc();
                                swtlogf(INFO,
                                        "[线程] 定时器1唤醒，时间戳：%lld 纳秒。\n",
                                        ts);
                        }
                } else {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        swtlogf(ERR,
                                "[线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }
        }

        swtlogf(INFO, "[线程] 退出。\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
