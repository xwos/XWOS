/**
 * @file
 * @brief 示例：CPU总中断控制
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/irq.h>
#include <xwos/osal/swt.h>
#include <xwam/example/lock/cpuirq/mif.h>

#define LOGTAG "cpuirq"
#define cpuirqlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define CPUIRQDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
void cpuirqdemo_swt_callback(struct xwos_swt * swt, void * arg);
xwer_t cpuirqdemo_thd_func(void * arg);

xwos_thd_d cpuirqdemo_thd;

struct xwos_swt cpuirqdemo_swt;

xwsq_t cpuirqdemo_shared_count = 0;

/**
 * @brief 模块的加载函数
 */
xwer_t xwos_example_cpuirq(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        /* 初始化定时器 */
        rc = xwos_swt_init(&cpuirqdemo_swt, "cpuirqdemo_swt",
                           XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt_init;
        }

        /* 创建线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "cpuirqdemo.thd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = CPUIRQDEMO_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&cpuirqdemo_thd, &attr, cpuirqdemo_thd_func, NULL);
        if (rc < 0) {
                goto err_thd_create;
        }

        return XWOK;

err_thd_create:
        xwos_swt_fini(&cpuirqdemo_swt);
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
void cpuirqdemo_swt_callback(struct xwos_swt * swt, void * arg)
{
        xwreg_t cpuirq;

        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_cpuirq_save_lc(&cpuirq);
        /* 临界区 */
        cpuirqdemo_shared_count++;
        xwos_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief 线程的主函数
 */
xwer_t cpuirqdemo_thd_func(void * arg)
{
        xwtm_t ts;
        xwsq_t cnt;
        xwer_t rc;

        XWOS_UNUSED(arg);

        cpuirqlogf(INFO, "[线程] 启动。\n");

        ts = xwtm_now();

        cpuirqlogf(INFO, "[线程] 启动定时器。\n");
        rc = xwos_swt_start(&cpuirqdemo_swt,
                            ts, XWTM_MS(500),
                            cpuirqdemo_swt_callback, NULL);

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwos_cthd_sleep(XWTM_S(1));

                xwos_cpuirq_disable_lc();
                /* 临界区 */
                cnt = cpuirqdemo_shared_count;
                xwos_cpuirq_enable_lc();

                ts = xwtm_nowts();
                cpuirqlogf(INFO,
                        "[线程] 时间戳：%lld 纳秒，计数器：%d。\n",
                        ts, cnt);
        }

        cpuirqlogf(INFO, "[线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
