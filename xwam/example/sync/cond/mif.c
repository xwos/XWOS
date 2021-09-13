/**
 * @file
 * @brief 示例：条件量
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
#include <xwos/osal/sync/cond.h>
#include <xwam/example/sync/cond/mif.h>

#define LOGTAG "cond"
#define condlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWCONDDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t xwconddemo_thd_func(void * arg);

/**
 * @brief 线程描述表
 */
const struct xwos_thd_desc xwconddemo_thd_desc = {
        .name = "xwconddemo.thd",
        .prio = XWCONDDEMO_THD_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thd_f)xwconddemo_thd_func,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
struct xwos_thd * xwconddemo_thd;

struct xwos_swt xwconddemo_swt;
struct xwos_cond xwconddemo_cond;
struct xwos_splk xwconddemo_lock;
xwsq_t xwconddemo_shared_count = 0;
bool is_updated = false;

/**
 * @brief 模块的加载函数
 */
xwer_t example_cond_start(void)
{
        xwer_t rc;

        /* 初始化自旋锁 */
        xwos_splk_init(&xwconddemo_lock);

        /* 初始化条件量 */
        rc = xwos_cond_init(&xwconddemo_cond);
        if (rc < 0) {
                goto err_cond_init;
        }

        /* 初始化定时器 */
        rc = xwos_swt_init(&xwconddemo_swt, "xwconddemo_swt",
                           XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt_init;
        }

        /* 创建线程 */
        rc = xwos_thd_create(&xwconddemo_thd,
                             xwconddemo_thd_desc.name,
                             xwconddemo_thd_desc.func,
                             xwconddemo_thd_desc.arg,
                             xwconddemo_thd_desc.stack_size,
                             xwconddemo_thd_desc.prio,
                             xwconddemo_thd_desc.attr);
        if (rc < 0) {
                goto err_thd_create;
        }

        return XWOK;

err_thd_create:
        xwos_swt_fini(&xwconddemo_swt);
err_swt_init:
        xwos_cond_fini(&xwconddemo_cond);
err_cond_init:
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
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数；
 * - 中断上下文与中断底半部同属于中断，访问共享数据需要关闭中断，为了保持与
 *   MP系统的兼容性，玄武OS不建议直接调用关闭中断的API，应该使用自旋锁；如果
 *   此处的定时器回调运行在CPU0，而下文中的线程运行在CPU1，传统的关闭中断做法
 *   无法保证共享数据的安全性，此时只能使用自旋锁已经其派生锁。
 */
void xwconddemo_swt_callback(struct xwos_swt * swt, void * arg)
{
        xwreg_t cpuirq;
        bool ntf;

        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        /* xwos_cpuirq_save_lc(&cpuirq); */ /* 传统RTOS方法，不推荐 */
        xwos_splk_lock_cpuirqsv(&xwconddemo_lock, &cpuirq); /* 多核OS的方法，推荐 */
        if (false == is_updated) {
                xwconddemo_shared_count++;
                is_updated = true;
                ntf = is_updated;
        } else {
                ntf = is_updated;
        }
        xwos_splk_unlock_cpuirqrs(&xwconddemo_lock, cpuirq); /* 多核OS的方法，推荐 */
        /* xwos_cpuirq_restore_lc(cpuirq); */ /* 传统RTOS做法，不推荐 */
        if (ntf) {
                xwos_cond_broadcast(&xwconddemo_cond);
        }
}

/**
 * @brief 线程1的主函数
 */
xwer_t xwconddemo_thd_func(void * arg)
{
        xwer_t rc;
        xwtm_t base, ts;
        xwreg_t cpuirq;
        xwsq_t lkst;
        union xwos_ulock ulk;
        xwsq_t cnt;

        XWOS_UNUSED(arg);

        condlogf(INFO, "[线程] 启动。\n");
        condlogf(INFO, "[线程] 启动定时器。\n");
        base = xwos_skd_get_timetick_lc();
        rc = xwos_swt_start(&xwconddemo_swt, base, 1000 * XWTM_MS,
                            xwconddemo_swt_callback, NULL);

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwos_splk_lock_cpuirqsv(&xwconddemo_lock, &cpuirq);
                if (is_updated) {
                        rc = XWOK;
                        is_updated = false;
                        cnt = xwconddemo_shared_count;
                } else {
                        ts = 500 * XWTM_MS;
                        ulk.osal.splk = &xwconddemo_lock;
                        /* 等待条件量，同时解锁自旋锁 */
                        rc = xwos_cond_timedwait(&xwconddemo_cond,
                                                 ulk, XWOS_LK_SPLK,
                                                 NULL, &ts, &lkst);
                        if (XWOK == rc) {
                                /* 等待到条件量，自旋锁自动上锁 */
                                is_updated = false;
                        } else {
                                /* 错误返回，自旋锁不会自动上锁 */
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwos_splk_lock(&xwconddemo_lock);
                                }
                        }
                        cnt = xwconddemo_shared_count;
                }
                xwos_splk_unlock_cpuirqrs(&xwconddemo_lock, cpuirq);

                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        condlogf(INFO,
                                 "[线程] 定时器唤醒，时间戳：%lld 纳秒，"
                                 "计数器：%d。\n",
                                 ts, cnt);
                } else if (-ETIMEDOUT == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        condlogf(INFO,
                                 "[线程] 等待超时，时间戳：%lld 纳秒，"
                                 "计数器：%d，错误码：%d。\n",
                                 ts, cnt, rc);
                }
        }
        condlogf(INFO, "[线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
