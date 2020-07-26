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
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/condition.h>
#include <xwam/example/sync/condition/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define CDTTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define EXAMPLE_THREAD_SLEEP_LOG_TAG        "cdtthrd"
  #define cdtlogf(lv, fmt, ...) \
        xwlogf(lv, EXAMPLE_THREAD_SLEEP_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define cdtlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t cdtthrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 线程描述表
 */
const struct xwosal_thrd_desc cdtthrd_tbd = {
        .name = "Thread-1",
        .prio = CDTTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)cdtthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t cdtthrd_tid;

struct xwosal_swt cdtswt;
xwid_t cdtswtid;

struct xwosal_cdt cdt;
xwid_t cdtid;

struct xwosal_splk cdtlock;
xwsq_t shared_count = 0;
bool is_updated = false;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 模块的加载函数
 */
xwer_t example_condition_start(void)
{
        xwer_t rc;

        /* 初始化自旋锁 */
        xwosal_splk_init(&cdtlock);

        /* 初始化条件量 */
        rc = xwosal_cdt_init(&cdt);
        if (rc < 0) {
                goto err_cdt_init;
        }
        cdtid = xwosal_cdt_get_id(&cdt);

        /* 初始化定时器 */
        rc = xwosal_swt_init(&cdtswt, "cdtswt", XWOSAL_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_cdtswt_init;
        }
        cdtswtid = xwosal_swt_get_id(&cdtswt);

        /* 创建线程 */
        rc = xwosal_thrd_create(&cdtthrd_tid,
                                cdtthrd_tbd.name,
                                cdtthrd_tbd.func,
                                cdtthrd_tbd.arg,
                                cdtthrd_tbd.stack_size,
                                cdtthrd_tbd.prio,
                                cdtthrd_tbd.attr);
        if (rc < 0) {
                goto err_thrd_create;
        }

        return XWOK;

err_thrd_create:
        xwosal_swt_destroy(&cdtswt);
err_cdtswt_init:
        xwosal_cdt_destroy(&cdt);
err_cdt_init:
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
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数；
 * - 中断上下文与中断底半部同属于中断，访问共享数据需要关闭中断，为了保持与
 *   SMP系统的兼容性，玄武OS不建议直接调用关闭中断的API，应该使用自旋锁；如果
 *   此处的定时器回调运行在CPU0，而下文中的线程运行在CPU1，传统的关闭中断做法
 *   无法保证共享数据的安全性，此时只能使用自旋锁已经其派生锁。
 */
void cdtswt_callback(struct xwosal_swt * swt, void * arg)
{
        xwreg_t cpuirq;
        bool ntf;

        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        /* xwosal_cpuirq_save_lc(&cpuirq); */ /* 传统RTOS方法，不推荐 */
        xwosal_splk_lock_cpuirqsv(&cdtlock, &cpuirq); /* 多核OS的方法，推荐 */
        if (false == is_updated) {
                shared_count++;
                is_updated = true;
                ntf = is_updated;
        } else {
                ntf = is_updated;
        }
        xwosal_splk_unlock_cpuirqrs(&cdtlock, cpuirq); /* 多核OS的方法，推荐 */
        /* xwosal_cpuirq_restore_lc(cpuirq); */ /* 传统RTOS做法，不推荐 */
        if (ntf) {
                xwosal_cdt_broadcast(cdtid);
        }
}

/**
 * @brief 线程1的主函数
 */
xwer_t cdtthrd_func(void * arg)
{
        xwer_t rc;
        xwtm_t base, time;
        xwreg_t cpuirq;
        xwsq_t lkst;
        union xwlk_ulock ulk;
        xwsq_t cnt;

        XWOS_UNUSED(arg);

        cdtlogf(INFO, "[线程] 启动。\n");

        cdtlogf(INFO, "[线程] 启动定时器。\n");
        base = xwosal_scheduler_get_timetick_lc();
        rc = xwosal_swt_start(cdtswtid, base, 1000 * XWTM_MS, cdtswt_callback, NULL);

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                xwosal_splk_lock_cpuirqsv(&cdtlock, &cpuirq);
                if (is_updated) {
                        rc = XWOK;
                        is_updated = false;
                        cnt = shared_count;
                } else {
                        time = 500 * XWTM_MS;
                        ulk.osal.splk = &cdtlock;
                        /* 等待条件量，同时解锁自旋锁 */
                        rc = xwosal_cdt_timedwait(cdtid, ulk, XWLK_TYPE_SPLK,
                                                  NULL, &time, &lkst);
                        if (XWOK == rc) {
                                /* 等待到条件量，自旋锁自动上锁 */
                                is_updated = false;
                        } else {
                                /* 错误返回，自旋锁不会自动上锁 */
                                if (XWLK_STATE_UNLOCKED == lkst) {
                                        xwosal_splk_lock(&cdtlock);
                                }
                        }
                        cnt = shared_count;
                }
                xwosal_splk_unlock_cpuirqrs(&cdtlock, cpuirq);

                if (XWOK == rc) {
                        time = xwosal_scheduler_get_timestamp_lc();
                        cdtlogf(INFO,
                                "[线程] 定时器唤醒，时间戳：%lld 纳秒，"
                                "计数器：%d。\n",
                                time, cnt);
                } else if (-ETIMEDOUT == rc) {
                        time = xwosal_scheduler_get_timestamp_lc();
                        cdtlogf(INFO,
                                "[线程] 等待超时，时间戳：%lld 纳秒，"
                                "计数器：%d，错误码：%d。\n",
                                time, cnt, rc);
                }
        }

        cdtlogf(INFO, "[线程] 退出。\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
