/**
 * @file
 * @brief 示例：信号量
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
#include <xwos/osal/swt.h>
#include <xwos/osal/sync/sem.h>
#include <xwam/example/sync/sem/mif.h>

#define LOGTAG "sem"
#define semlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWSEMDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t xwsemdemo_thd_func(void * arg);

/**
 * @brief 线程描述表
 */
const struct xwos_thd_desc xwsemdemo_thd_desc = {
        .attr = {
                .name = "xwsemdemo.thd",
                .stack = NULL,
                .stack_size = 2048,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = XWSEMDEMO_THD_PRIORITY,
                .detached = false,
                .privileged = true,
        },
        .func = (xwos_thd_f)xwsemdemo_thd_func,
        .arg = NULL,
};
xwos_thd_d xwsemdemo_thd;

struct xwos_swt xwsemdemo_swt;
struct xwos_sem xwsemdemo_sem;

/**
 * @brief 模块的加载函数
 */
xwer_t example_sem_start(void)
{
        xwer_t rc;

        /* 初始化信号量 */
        rc = xwos_sem_init(&xwsemdemo_sem, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_sem_init;
        }

        /* 初始化定时器 */
        rc = xwos_swt_init(&xwsemdemo_swt, "xwsemdemo_swt",
                           XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt_init;
        }

        /* 创建线程 */
        rc = xwos_thd_create(&xwsemdemo_thd,
                             &xwsemdemo_thd_desc.attr,
                             xwsemdemo_thd_desc.func,
                             xwsemdemo_thd_desc.arg);
        if (rc < 0) {
                goto err_thd_create;
        }

        return XWOK;

err_thd_create:
        xwos_swt_fini(&xwsemdemo_swt);
err_swt_init:
        xwos_sem_fini(&xwsemdemo_sem);
err_sem_init:
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
void xwsemdemo_swt_callback(struct xwos_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_sem_post(&xwsemdemo_sem);
}

/**
 * @brief 线程1的主函数
 */
xwer_t xwsemdemo_thd_func(void * arg)
{
        xwtm_t now;
        xwer_t rc;

        XWOS_UNUSED(arg);

        semlogf(INFO, "[线程] 启动。\n");

        semlogf(INFO, "[线程] 启动定时器。\n");
        now = xwtm_now();
        rc = xwos_swt_start(&xwsemdemo_swt, now, 1000 * XWTM_MS,
                            xwsemdemo_swt_callback, NULL);

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwos_sem_wait_to(&xwsemdemo_sem, xwtm_ft(500 * XWTM_MS));
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        semlogf(INFO,
                                "[线程] 定时器唤醒，时间戳：%lld 纳秒。\n",
                                now);
                } else if (-ETIMEDOUT == rc) {
                        now = xwtm_nowts();
                        semlogf(INFO,
                                "[线程] 等待超时，时间戳：%lld 纳秒。\n",
                                now);
                }
        }

        semlogf(INFO, "[线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
