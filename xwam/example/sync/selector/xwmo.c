/**
 * @file
 * @brief 示例：信号选择器
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
#include <xwos/osal/swt.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwos/osal/sync/condition.h>
#include <xwos/osal/sync/flag.h>
#include <xwos/osal/sync/barrier.h>
#include <xwos/osal/sync/selector.h>
#include <xwam/example/sync/selector/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWSLTDEMO_CONSUMER_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define XWSLTDEMO_PRODUCER_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

#define XWSLTDEMO_BARRIERTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define XWSLTDEMO_LOG_TAG      "slt"
  #define sltlogf(lv, fmt, ...) xwlogf(lv, XWSLTDEMO_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define sltlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
void xwsltdemo_swt0_callback(struct xwosal_swt * swt, void * arg);
void xwsltdemo_swt1_callback(struct xwosal_swt * swt, void * arg);
xwer_t xwsltdemo_consumer_func(void * arg);
xwer_t xwsltdemo_producer_func(void * arg);
xwer_t xwsltdemo_syncthrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_selector xwsltdemo_slt0;
xwid_t xwsltdemo_slt0id;

struct xwosal_smr xwsltdemo_smr1;
xwid_t xwsltdemo_smr1id;

struct xwosal_smr xwsltdemo_smr2;
xwid_t xwsltdemo_smr2id;

struct xwosal_flg xwsltdemo_flg3;
xwid_t xwsltdemo_flg3id;

struct xwosal_selector xwsltdemo_slt4;
xwid_t xwsltdemo_slt4id;

struct xwosal_cdt xwsltdemo_cdt5;
xwid_t xwsltdemo_cdt5id;

struct xwosal_cdt xwsltdemo_cdt6;
xwid_t xwsltdemo_cdt6id;

struct xwosal_barrier xwsltdemo_br7;
xwid_t xwsltdemo_br7id;

const struct xwosal_thrd_desc xwsltdemo_consumer_td = {
        .name = "xwsltdemo.consumer",
        .prio = XWSLTDEMO_CONSUMER_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwsltdemo_consumer_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t xwsltdemo_consumer;

const struct xwosal_thrd_desc xwsltdemo_producer_td = {
        .name = "xwsltdemo.producer",
        .prio = XWSLTDEMO_PRODUCER_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)xwsltdemo_producer_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t xwsltdemo_producer;

struct xwosal_swt xwsltdemo_swt0;
xwid_t xwsltdemo_swt0id;

struct xwosal_swt xwsltdemo_swt1;
xwid_t xwsltdemo_swt1id;

const struct xwosal_thrd_desc xwsltdemo_syncthrd_td[] = {
        [0] = {
                .name = "xwsltdemo.syncthrd.0",
                .prio = XWSLTDEMO_BARRIERTHRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)xwsltdemo_syncthrd_func,
                .arg = (void *)0,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
        [1] = {
                .name = "xwsltdemo.syncthrd.1",
                .prio = XWSLTDEMO_BARRIERTHRD_PRIORITY,
                .stack = XWOSAL_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwosal_thrd_f)xwsltdemo_syncthrd_func,
                .arg = (void *)1,
                .attr = XWSDOBJ_ATTR_PRIVILEGED,
        },
};
xwid_t xwsltdemo_syncthrd[xw_array_size(xwsltdemo_syncthrd_td)];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 测试模块的启动函数
 */
xwer_t example_selector_start(void)
{
        xwer_t rc;
        xwsq_t i;

        /* 初始化信号选择器0 */
        rc = xwosal_selector_init(&xwsltdemo_slt0);
        if (rc < 0) {
                goto err_slt0_init;
        }
        /* 初始化信号量1 */
        rc = xwosal_smr_init(&xwsltdemo_smr1, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr1_init;
        }
        /* 初始化信号量2 */
        rc = xwosal_smr_init(&xwsltdemo_smr2, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr2_init;
        }
        /* 初始化事件信号旗3 */
        rc = xwosal_flg_init(&xwsltdemo_flg3, NULL);
        if (rc < 0) {
                goto err_flg3_init;
        }
        /* 初始化信号选择器4 */
        rc = xwosal_selector_init(&xwsltdemo_slt4);
        if (rc < 0) {
                goto err_slt4_init;
        }
        /* 初始化条件量5 */
        rc = xwosal_cdt_init(&xwsltdemo_cdt5);
        if (rc < 0) {
                goto err_cdt5_init;
        }
        /* 初始化条件量6 */
        rc = xwosal_cdt_init(&xwsltdemo_cdt6);
        if (rc < 0) {
                goto err_cdt6_init;
        }
        /* 初始化线程屏障7 */
        rc = xwosal_barrier_init(&xwsltdemo_br7);
        if (rc < 0) {
                goto err_br7_init;
        }

        /* 初始化定时器0 */
        rc = xwosal_swt_init(&xwsltdemo_swt0, "xwsltdemo_swt0",
                             XWOSAL_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt0_init;
        }

        /* 初始化定时器1 */
        rc = xwosal_swt_init(&xwsltdemo_swt1, "xwsltdemo_swt1",
                             XWOSAL_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt1_init;
        }

        /* 获取各同步对象的ID */
        xwsltdemo_slt0id = xwosal_selector_get_id(&xwsltdemo_slt0);
        xwsltdemo_smr1id = xwosal_smr_get_id(&xwsltdemo_smr1);
        xwsltdemo_smr2id = xwosal_smr_get_id(&xwsltdemo_smr2);
        xwsltdemo_flg3id = xwosal_flg_get_id(&xwsltdemo_flg3);
        xwsltdemo_slt4id = xwosal_selector_get_id(&xwsltdemo_slt4);
        xwsltdemo_cdt5id = xwosal_cdt_get_id(&xwsltdemo_cdt5);
        xwsltdemo_cdt6id = xwosal_cdt_get_id(&xwsltdemo_cdt6);
        xwsltdemo_br7id = xwosal_barrier_get_id(&xwsltdemo_br7);
        xwsltdemo_swt0id = xwosal_swt_get_id(&xwsltdemo_swt0);
        xwsltdemo_swt1id = xwosal_swt_get_id(&xwsltdemo_swt1);

        /* 绑定信号量1到信号选择器0的位置为1 */
        rc = xwosal_smr_bind(xwsltdemo_smr1id, xwsltdemo_slt0id, 1);
        if (rc < 0) {
                goto err_smr1_bind;
        }
        /* 绑定信号量2到信号选择器0的位置为2 */
        rc = xwosal_smr_bind(xwsltdemo_smr2id, xwsltdemo_slt0id, 2);
        if (rc < 0) {
                goto err_smr2_bind;
        }
        /* 绑定事件信号旗3到信号选择器0的位置为3 */
        rc = xwosal_flg_bind(xwsltdemo_flg3id, xwsltdemo_slt0id, 3);
        if (rc < 0) {
                goto err_flg3_bind;
        }

        /* 绑定信号选择器4到信号选择器0的位置为4 */
        rc = xwosal_selector_bind(xwsltdemo_slt4id, xwsltdemo_slt0id, 4);
        if (rc < 0) {
                goto err_slt4_bind;
        }

        /* 绑定条件量5到信号选择器4的位置为5 */
        rc = xwosal_cdt_bind(xwsltdemo_cdt5id, xwsltdemo_slt4id, 5);
        if (rc < 0) {
                goto err_cdt5_bind;
        }
        /* 绑定条件量6到信号选择器4的位置为6 */
        rc = xwosal_cdt_bind(xwsltdemo_cdt6id, xwsltdemo_slt4id, 6);
        if (rc < 0) {
                goto err_cdt6_bind;
        }
        /* 绑定线程屏障7到信号选择器4的位置7 */
        rc = xwosal_barrier_bind(xwsltdemo_br7id, xwsltdemo_slt4id, 7);
        if (rc < 0) {
                goto err_br7_bind;
        }

        /* 创建等待同步对象的线程 */
        rc = xwosal_thrd_create(&xwsltdemo_consumer,
                                xwsltdemo_consumer_td.name,
                                xwsltdemo_consumer_td.func,
                                xwsltdemo_consumer_td.arg,
                                xwsltdemo_consumer_td.stack_size,
                                xwsltdemo_consumer_td.prio,
                                xwsltdemo_consumer_td.attr);
        if (rc < 0) {
                goto err_consumer_create;
        }

        /* 建立发布同步对象的线程 */
        rc = xwosal_thrd_create(&xwsltdemo_producer,
                                xwsltdemo_producer_td.name,
                                xwsltdemo_producer_td.func,
                                xwsltdemo_producer_td.arg,
                                xwsltdemo_producer_td.stack_size,
                                xwsltdemo_producer_td.prio,
                                xwsltdemo_producer_td.attr);
        if (rc < 0) {
                goto err_producer_create;
        }

        /* 创建同步线程 */
        for (i = 0; i < xw_array_size(xwsltdemo_syncthrd_td); i++) {
                rc = xwosal_thrd_create(&xwsltdemo_syncthrd[i],
                                        xwsltdemo_syncthrd_td[i].name,
                                        xwsltdemo_syncthrd_td[i].func,
                                        xwsltdemo_syncthrd_td[i].arg,
                                        xwsltdemo_syncthrd_td[i].stack_size,
                                        xwsltdemo_syncthrd_td[i].prio,
                                        xwsltdemo_syncthrd_td[i].attr);
                if (rc < 0) {
                        goto err_syncthrd_create;
                }
        }

        return XWOK;

err_syncthrd_create:
        xwosal_thrd_terminate(xwsltdemo_producer, NULL);
err_producer_create:
        xwosal_thrd_terminate(xwsltdemo_consumer, NULL);
err_consumer_create:
        xwosal_barrier_unbind(xwsltdemo_br7id, xwsltdemo_slt4id);
err_br7_bind:
        xwosal_cdt_unbind(xwsltdemo_cdt6id, xwsltdemo_slt4id);
err_cdt6_bind:
        xwosal_cdt_unbind(xwsltdemo_cdt5id, xwsltdemo_slt4id);
err_cdt5_bind:
        xwosal_selector_unbind(xwsltdemo_slt4id, xwsltdemo_slt0id);
err_slt4_bind:
        xwosal_flg_unbind(xwsltdemo_flg3id, xwsltdemo_slt0id);
err_flg3_bind:
        xwosal_smr_unbind(xwsltdemo_smr2id, xwsltdemo_slt0id);
err_smr2_bind:
        xwosal_smr_unbind(xwsltdemo_smr1id, xwsltdemo_slt0id);
err_smr1_bind:
        xwosal_swt_destroy(&xwsltdemo_swt1);
err_swt1_init:
        xwosal_swt_destroy(&xwsltdemo_swt0);
err_swt0_init:
        xwosal_barrier_destroy(&xwsltdemo_br7);
err_br7_init:
        xwosal_cdt_destroy(&xwsltdemo_cdt6);
err_cdt6_init:
        xwosal_cdt_destroy(&xwsltdemo_cdt5);
err_cdt5_init:
        xwosal_selector_destroy(&xwsltdemo_slt4);
err_slt4_init:
        xwosal_flg_destroy(&xwsltdemo_flg3);
err_flg3_init:
        xwosal_smr_destroy(&xwsltdemo_smr2);
err_smr2_init:
        xwosal_smr_destroy(&xwsltdemo_smr1);
err_smr1_init:
        xwosal_selector_destroy(&xwsltdemo_slt0);
err_slt0_init:
        return rc;
}

/**
 * @brief 消费者线程
 */
xwer_t xwsltdemo_consumer_func(void * arg)
{
        xwosal_selector_declare_bitmap(msk);
        xwosal_selector_declare_bitmap(trg);
        xwosal_flg_declare_bitmap(flgmsk);
        xwosal_flg_declare_bitmap(flgtrg);
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        sltlogf(INFO, "[消费者] 启动。\n");

        sltlogf(INFO, "[消费者] 启动定时器0。\n");
        ts = xwosal_scheduler_get_timetick_lc();
        rc = xwosal_swt_start(xwsltdemo_swt0id, ts, 500 * XWTM_MS,
                              xwsltdemo_swt0_callback, NULL);

        sltlogf(INFO, "[消费者] 启动定时器1。\n");
        rc = xwosal_swt_start(xwsltdemo_swt1id, ts, 800 * XWTM_MS,
                              xwsltdemo_swt1_callback, NULL);

        memset(msk, 0, sizeof(msk));
        memset(flgmsk, 0, sizeof(msk));
        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                /* 设置掩码位为bit1 ~ bit7共7位 */
                msk[0] = BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6) | BIT(7);

                ts = xwosal_scheduler_get_timestamp_lc();
                sltlogf(INFO,
                        "[消费者] 等待信号：0x%X，时间戳：%lld 纳秒...\n",
                        msk[0], ts);
                ts = 1 * XWTM_S;
                rc = xwosal_selector_timedselect(xwsltdemo_slt0id, msk, trg, &ts);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(INFO,
                                "[消费者] 唤醒，时间戳：%lld 纳秒，触发信号：0x%X。\n",
                                trg[0], ts);

                        if (xwbmpop_t1i(trg, 1)) {
                                rc = xwosal_smr_trywait(xwsltdemo_smr1id);
                                if (XWOK == rc) {
                                        ts = xwosal_scheduler_get_timestamp_lc();
                                        sltlogf(INFO,
                                                "[消费者] 信号量1触发，"
                                                "时间戳：%lld 纳秒，。\n", ts);
                                }
                        }

                        if (xwbmpop_t1i(trg, 2)) {
                                rc = xwosal_smr_trywait(xwsltdemo_smr2id);
                                if (XWOK == rc) {
                                        ts = xwosal_scheduler_get_timestamp_lc();
                                        sltlogf(INFO,
                                                "[消费者] 信号量2触发，"
                                                "时间戳：%lld 纳秒，。\n", ts);
                                }
                        }

                        if (xwbmpop_t1i(trg, 3)) {
                                flgmsk[0] = BIT(0) | BIT(1);
                                rc = xwosal_flg_trywait(xwsltdemo_flg3id,
                                                        XWOSAL_FLG_TRIGGER_SET_ANY,
                                                        XWOSAL_FLG_ACTION_CONSUMPTION,
                                                        flgtrg, flgmsk);
                                if (XWOK == rc) {
                                        ts = xwosal_scheduler_get_timestamp_lc();
                                        if (xwbmpop_t1i(flgtrg, 0)) {
                                                sltlogf(INFO,
                                                        "[消费者] 定时器0触发，"
                                                        "时间戳：%lld 纳秒，。\n", ts);
                                        }
                                        if (xwbmpop_t1i(flgtrg, 1)) {
                                                sltlogf(INFO,
                                                        "[消费者] 定时器1触发，"
                                                        "时间戳：%lld 纳秒，。\n", ts);
                                        }
                                }
                        }

                        if (xwbmpop_t1i(trg, 4)) {
                                /* 第4位为子信号选择器4，再次测试 */
                                rc = xwosal_selector_tryselect(xwsltdemo_slt4id,
                                                               msk, trg);
                                if (XWOK == rc) {
                                        ts = xwosal_scheduler_get_timestamp_lc();
                                        if (xwbmpop_t1i(trg, 5)) {
                                                sltlogf(INFO,
                                                        "[消费者] 条件量5触发，"
                                                        "时间戳：%lld 纳秒，。\n", ts);
                                        }
                                        if (xwbmpop_t1i(trg, 6)) {
                                                sltlogf(INFO,
                                                        "[消费者] 条件量6触发，"
                                                        "时间戳：%lld 纳秒，。\n", ts);
                                        }
                                }
                        }

                        if (xwbmpop_t1i(trg, 7)) {
                                if (XWOK == rc) {
                                        ts = xwosal_scheduler_get_timestamp_lc();
                                        sltlogf(INFO,
                                                "[消费者] 线程栅栏7触发，"
                                                "时间戳：%lld 纳秒，。\n", ts);
                                }
                        }
                        sltlogf(INFO, "\n");
                } else {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(ERR,
                                "[消费者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
        }
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
void xwsltdemo_swt0_callback(struct xwosal_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwosal_flg_s1i(xwsltdemo_flg3id, 0);
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
void xwsltdemo_swt1_callback(struct xwosal_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwosal_flg_s1i(xwsltdemo_flg3id, 1);
}

/**
 * @brief 生产者线程
 */
xwer_t xwsltdemo_producer_func(void * arg)
{
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        sltlogf(INFO, "[生产者] 启动。\n");

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                rc = xwosal_smr_post(xwsltdemo_smr1id);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(INFO,
                                "[生产者] 发布信号量1，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                rc = xwosal_smr_post(xwsltdemo_smr2id);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(INFO,
                                "[生产者] 发布信号量2，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                rc = xwosal_cdt_broadcast(xwsltdemo_cdt5id);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(INFO,
                                "[生产者] 发布条件量5，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                rc = xwosal_cdt_broadcast(xwsltdemo_cdt6id);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(INFO,
                                "[生产者] 发布条件量6，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        sltlogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);
        }

        sltlogf(INFO, "[生产者] 退出。\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}

/**
 * @brief 竞赛线程
 */
xwer_t xwsltdemo_syncthrd_func(void * arg)
{
        xwosal_barrier_declare_bitmap(msk);
        xwsq_t pos = (xwsq_t)arg; /* 获取线程的各自的位置 */
        xwer_t rc;

        sltlogf(INFO, "[同步线程%d] 启动。\n", pos);

        xwbmpop_c0all(msk, XWOSAL_BARRIER_MAXNUM);

        /* 设置位图掩码 */
        for (xwsq_t i = 0; i < xw_array_size(xwsltdemo_syncthrd_td); i++) {
                xwbmpop_s1i(msk, i);
        }

        /* 同步线程 */
        rc = xwosal_barrier_sync(xwsltdemo_br7id, pos, msk);
        if (XWOK == rc) {
                sltlogf(INFO, "[同步线程%d] 已就位。\n", pos);
        }

        sltlogf(INFO, "[同步线程%d] 退出。\n", pos);
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
