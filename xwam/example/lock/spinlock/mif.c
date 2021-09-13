/**
 * @file
 * @brief 示例：自旋锁
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/swt.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwam/example/lock/spinlock/mif.h>

#define LOGTAG "swt"
#define swtlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWSPLKDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
void xwsplkdemo_swt_callback(struct xwos_swt * swt, void * arg);
xwer_t xwsplkdemo_thd_func(void * arg);

/**
 * @brief 线程描述表
 */
const struct xwos_thd_desc xwsplkdemo_thd_desc = {
        .name = "xwsplkdemo.thd",
        .prio = XWSPLKDEMO_THD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thd_f)xwsplkdemo_thd_func,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
struct xwos_thd * xwsplkdemo_thd;

struct xwos_swt xwsplkdemo_swt;

struct xwos_splk xwsplkdemo_lock;
xwsq_t xwsplkdemo_shared_count = 0;

/**
 * @brief 模块的加载函数
 */
xwer_t example_spinlock_start(void)
{
        xwer_t rc;

        /* 初始化自旋锁 */
        xwos_splk_init(&xwsplkdemo_lock);

        /* 初始化定时器 */
        rc = xwos_swt_init(&xwsplkdemo_swt, "xwsplkdemo_swt",
                           XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt_init;
        }

        /* 创建线程 */
        rc = xwos_thd_create(&xwsplkdemo_thd,
                             xwsplkdemo_thd_desc.name,
                             xwsplkdemo_thd_desc.func,
                             xwsplkdemo_thd_desc.arg,
                             xwsplkdemo_thd_desc.stack_size,
                             xwsplkdemo_thd_desc.prio,
                             xwsplkdemo_thd_desc.attr);
        if (rc < 0) {
                goto err_thd_create;
        }

        return XWOK;

err_thd_create:
        xwos_swt_fini(&xwsplkdemo_swt);
err_swt_init:
        return rc;
}

/**
 * @brief 定时器的回调函数
 * @note
 * - UP系统
 *   - 当配置(XWUPCFG_SKD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWUPCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * - MP系统
 *   - 当配置(XWMPCFG_SKD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWMPCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void xwsplkdemo_swt_callback(struct xwos_swt * swt, void * arg)
{
        xwreg_t cpuirq;

        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_splk_lock_cpuirqsv(&xwsplkdemo_lock, &cpuirq);
        /* 临界区 */
        xwsplkdemo_shared_count++;
        xwos_splk_unlock_cpuirqrs(&xwsplkdemo_lock, cpuirq);
}

/**
 * @brief 线程的主函数
 */
xwer_t xwsplkdemo_thd_func(void * arg)
{
        xwtm_t ts;
        xwsq_t cnt;
        xwer_t rc;

        XWOS_UNUSED(arg);

        swtlogf(INFO, "[线程] 启动。\n");

        ts = xwos_skd_get_timetick_lc();

        swtlogf(INFO, "[线程] 启动定时器。\n");
        rc = xwos_swt_start(&xwsplkdemo_swt,
                            ts, 500 * XWTM_MS,
                            xwsplkdemo_swt_callback, NULL);

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                ts = 1 * XWTM_S;
                xwos_cthd_sleep(&ts);

                xwos_splk_lock_cpuirq(&xwsplkdemo_lock);
                /* 临界区 */
                cnt = xwsplkdemo_shared_count;
                xwos_splk_unlock_cpuirq(&xwsplkdemo_lock);

                ts = xwos_skd_get_timestamp_lc();
                swtlogf(INFO,
                        "[线程] 时间戳：%lld 纳秒，计数器：%d。\n",
                        ts, cnt);

        }

        swtlogf(INFO, "[线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
