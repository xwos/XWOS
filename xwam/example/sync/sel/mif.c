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

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/swt.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/osal/sync/flg.h>
#include <xwos/osal/sync/br.h>
#include <xwos/osal/sync/sel.h>
#include <xwam/example/sync/sel/mif.h>

#define LOGTAG "sel"
#define sellogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWSELDEMO_CONSUMER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
#define XWSELDEMO_PRODUCER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 2)
#define XWSELDEMO_BRTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

void xwseldemo_swt0_callback(struct xwos_swt * swt, void * arg);
void xwseldemo_swt1_callback(struct xwos_swt * swt, void * arg);
xwer_t xwseldemo_consumer_func(void * arg);
xwer_t xwseldemo_producer_func(void * arg);
xwer_t xwseldemo_syncthd_func(void * arg);

#define XWSELDEMO_BMP_BITNUM (sizeof(xwbmp_t) * BITS_PER_XWU8_T)

xwbmpop_declare(xwseldemo_sel0_bmp, XWSELDEMO_BMP_BITNUM) = {0,};
xwbmpop_declare(xwseldemo_sel0_msk, XWSELDEMO_BMP_BITNUM) = {0,};
struct xwos_sel xwseldemo_sel0;

struct xwos_sem xwseldemo_sem1;
struct xwos_sem xwseldemo_sem2;

xwbmpop_declare(xwseldemo_flg3_bmp, XWSELDEMO_BMP_BITNUM) = {0,};
xwbmpop_declare(xwseldemo_flg3_msk, XWSELDEMO_BMP_BITNUM) = {0,};
struct xwos_flg xwseldemo_flg3;

xwbmpop_declare(xwseldemo_sel4_bmp, XWSELDEMO_BMP_BITNUM) = {0,};
xwbmpop_declare(xwseldemo_sel4_msk, XWSELDEMO_BMP_BITNUM) = {0,};
struct xwos_sel xwseldemo_sel4;

struct xwos_cond xwseldemo_cond5;
struct xwos_cond xwseldemo_cond6;

xwbmpop_declare(xwseldemo_br7_bmp, XWSELDEMO_BMP_BITNUM) = {0,};
xwbmpop_declare(xwseldemo_br7_msk, XWSELDEMO_BMP_BITNUM) = {0,};
struct xwos_br xwseldemo_br7;

const struct xwos_thd_desc xwseldemo_consumer_thd_desc = {
        .name = "xwseldemo.consumer",
        .prio = XWSELDEMO_CONSUMER_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 4096,
        .func = (xwos_thd_f)xwseldemo_consumer_func,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
struct xwos_thd * xwseldemo_consumer;

const struct xwos_thd_desc xwseldemo_producer_thd_desc = {
        .name = "xwseldemo.producer",
        .prio = XWSELDEMO_PRODUCER_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwos_thd_f)xwseldemo_producer_func,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
struct xwos_thd * xwseldemo_producer;

struct xwos_swt xwseldemo_swt0;
struct xwos_swt xwseldemo_swt1;

const struct xwos_thd_desc xwseldemo_sync_thd_desc[] = {
        [0] = {
                .name = "xwseldemo.syncthd.0",
                .prio = XWSELDEMO_BRTHD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwseldemo_syncthd_func,
                .arg = (void *)0,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [1] = {
                .name = "xwseldemo.syncthd.1",
                .prio = XWSELDEMO_BRTHD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwseldemo_syncthd_func,
                .arg = (void *)1,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd * xwseldemo_syncthd[xw_array_size(xwseldemo_sync_thd_desc)];

/**
 * @brief 测试模块的启动函数
 */
xwer_t example_sel_start(void)
{
        xwer_t rc;
        xwsq_t i;

        /* 初始化信号选择器0 */
        rc = xwos_sel_init(&xwseldemo_sel0, XWSELDEMO_BMP_BITNUM,
                           xwseldemo_sel0_bmp, xwseldemo_sel0_msk);
        if (rc < 0) {
                goto err_sel0_init;
        }
        /* 初始化信号量1 */
        rc = xwos_sem_init(&xwseldemo_sem1, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_sem1_init;
        }
        /* 初始化信号量2 */
        rc = xwos_sem_init(&xwseldemo_sem2, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_sem2_init;
        }
        /* 初始化事件标志3 */
        rc = xwos_flg_init(&xwseldemo_flg3, XWSELDEMO_BMP_BITNUM,
                           xwseldemo_flg3_bmp, xwseldemo_flg3_msk);
        if (rc < 0) {
                goto err_flg3_init;
        }
        /* 初始化信号选择器4 */
        rc = xwos_sel_init(&xwseldemo_sel4, XWSELDEMO_BMP_BITNUM,
                           xwseldemo_sel4_bmp, xwseldemo_sel4_msk);
        if (rc < 0) {
                goto err_sel4_init;
        }
        /* 初始化条件量5 */
        rc = xwos_cond_init(&xwseldemo_cond5);
        if (rc < 0) {
                goto err_cond5_init;
        }
        /* 初始化条件量6 */
        rc = xwos_cond_init(&xwseldemo_cond6);
        if (rc < 0) {
                goto err_cond6_init;
        }
        /* 初始化线程屏障7 */
        rc = xwos_br_init(&xwseldemo_br7, XWSELDEMO_BMP_BITNUM,
                          xwseldemo_br7_bmp, xwseldemo_br7_msk);
        if (rc < 0) {
                goto err_br7_init;
        }

        /* 初始化定时器0 */
        rc = xwos_swt_init(&xwseldemo_swt0, "xwseldemo_swt0",
                           XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt0_init;
        }

        /* 初始化定时器1 */
        rc = xwos_swt_init(&xwseldemo_swt1, "xwseldemo_swt1",
                           XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt1_init;
        }

        /* 绑定信号量1到信号选择器0的位置为1 */
        rc = xwos_sem_bind(&xwseldemo_sem1, &xwseldemo_sel0, 1);
        if (rc < 0) {
                goto err_sem1_bind;
        }
        /* 绑定信号量2到信号选择器0的位置为2 */
        rc = xwos_sem_bind(&xwseldemo_sem2, &xwseldemo_sel0, 2);
        if (rc < 0) {
                goto err_sem2_bind;
        }
        /* 绑定事件标志3到信号选择器0的位置为3 */
        rc = xwos_flg_bind(&xwseldemo_flg3, &xwseldemo_sel0, 3);
        if (rc < 0) {
                goto err_flg3_bind;
        }

        /* 绑定信号选择器4到信号选择器0的位置为4 */
        rc = xwos_sel_bind(&xwseldemo_sel4, &xwseldemo_sel0, 4);
        if (rc < 0) {
                goto err_sel4_bind;
        }

        /* 绑定条件量5到信号选择器4的位置为5 */
        rc = xwos_cond_bind(&xwseldemo_cond5, &xwseldemo_sel4, 5);
        if (rc < 0) {
                goto err_cond5_bind;
        }
        /* 绑定条件量6到信号选择器4的位置为6 */
        rc = xwos_cond_bind(&xwseldemo_cond6, &xwseldemo_sel4, 6);
        if (rc < 0) {
                goto err_cond6_bind;
        }
        /* 绑定线程屏障7到信号选择器4的位置7 */
        rc = xwos_br_bind(&xwseldemo_br7, &xwseldemo_sel4, 7);
        if (rc < 0) {
                goto err_br7_bind;
        }

        /* 创建等待同步对象的线程 */
        rc = xwos_thd_create(&xwseldemo_consumer,
                             xwseldemo_consumer_thd_desc.name,
                             xwseldemo_consumer_thd_desc.func,
                             xwseldemo_consumer_thd_desc.arg,
                             xwseldemo_consumer_thd_desc.stack_size,
                             xwseldemo_consumer_thd_desc.prio,
                             xwseldemo_consumer_thd_desc.attr);
        if (rc < 0) {
                goto err_consumer_create;
        }

        /* 建立发布同步对象的线程 */
        rc = xwos_thd_create(&xwseldemo_producer,
                             xwseldemo_producer_thd_desc.name,
                             xwseldemo_producer_thd_desc.func,
                             xwseldemo_producer_thd_desc.arg,
                             xwseldemo_producer_thd_desc.stack_size,
                             xwseldemo_producer_thd_desc.prio,
                             xwseldemo_producer_thd_desc.attr);
        if (rc < 0) {
                goto err_producer_create;
        }

        /* 创建同步线程 */
        for (i = 0; i < xw_array_size(xwseldemo_sync_thd_desc); i++) {
                rc = xwos_thd_create(&xwseldemo_syncthd[i],
                                     xwseldemo_sync_thd_desc[i].name,
                                     xwseldemo_sync_thd_desc[i].func,
                                     xwseldemo_sync_thd_desc[i].arg,
                                     xwseldemo_sync_thd_desc[i].stack_size,
                                     xwseldemo_sync_thd_desc[i].prio,
                                     xwseldemo_sync_thd_desc[i].attr);
                if (rc < 0) {
                        goto err_syncthd_create;
                }
        }

        return XWOK;

err_syncthd_create:
        xwos_thd_cancel(xwseldemo_producer);
err_producer_create:
        xwos_thd_cancel(xwseldemo_consumer);
err_consumer_create:
        xwos_br_unbind(&xwseldemo_br7, &xwseldemo_sel4);
err_br7_bind:
        xwos_cond_unbind(&xwseldemo_cond6, &xwseldemo_sel4);
err_cond6_bind:
        xwos_cond_unbind(&xwseldemo_cond5, &xwseldemo_sel4);
err_cond5_bind:
        xwos_sel_unbind(&xwseldemo_sel4, &xwseldemo_sel0);
err_sel4_bind:
        xwos_flg_unbind(&xwseldemo_flg3, &xwseldemo_sel0);
err_flg3_bind:
        xwos_sem_unbind(&xwseldemo_sem2, &xwseldemo_sel0);
err_sem2_bind:
        xwos_sem_unbind(&xwseldemo_sem1, &xwseldemo_sel0);
err_sem1_bind:
        xwos_swt_fini(&xwseldemo_swt1);
err_swt1_init:
        xwos_swt_fini(&xwseldemo_swt0);
err_swt0_init:
        xwos_br_fini(&xwseldemo_br7);
err_br7_init:
        xwos_cond_fini(&xwseldemo_cond6);
err_cond6_init:
        xwos_cond_fini(&xwseldemo_cond5);
err_cond5_init:
        xwos_sel_fini(&xwseldemo_sel4);
err_sel4_init:
        xwos_flg_fini(&xwseldemo_flg3);
err_flg3_init:
        xwos_sem_fini(&xwseldemo_sem2);
err_sem2_init:
        xwos_sem_fini(&xwseldemo_sem1);
err_sem1_init:
        xwos_sel_fini(&xwseldemo_sel0);
err_sel0_init:
        return rc;
}

/**
 * @brief 消费者线程
 */
xwer_t xwseldemo_consumer_func(void * arg)
{
        xwbmpop_declare(selmsk, XWSELDEMO_BMP_BITNUM);
        xwbmpop_declare(trg, XWSELDEMO_BMP_BITNUM);
        xwbmpop_declare(flgmsk, XWSELDEMO_BMP_BITNUM);
        xwbmpop_declare(flgtrg, XWSELDEMO_BMP_BITNUM);
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        sellogf(INFO, "[消费者] 启动。\n");
        sellogf(INFO, "[消费者] 启动定时器0。\n");
        ts = xwos_skd_get_timetick_lc();
        rc = xwos_swt_start(&xwseldemo_swt0, ts, 500 * XWTM_MS,
                            xwseldemo_swt0_callback, NULL);

        sellogf(INFO, "[消费者] 启动定时器1。\n");
        rc = xwos_swt_start(&xwseldemo_swt1, ts, 800 * XWTM_MS,
                            xwseldemo_swt1_callback, NULL);

        memset(selmsk, 0, sizeof(selmsk));
        memset(flgmsk, 0, sizeof(flgmsk));
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                /* 设置掩码位为bit1 ~ bit7共7位 */
                selmsk[0] = 0xFF;
                ts = xwos_skd_get_timestamp_lc();
                sellogf(INFO,
                        "[消费者] 等待信号：0x%X，时间戳：%lld 纳秒...\n",
                        selmsk[0], ts);
                ts = 1 * XWTM_S;
                rc = xwos_sel_timedselect(&xwseldemo_sel0, selmsk, trg, &ts);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(INFO,
                                "[消费者] 唤醒，触发信号：0x%X，时间戳：%lld 纳秒。\n",
                                trg[0], ts);

                        if (xwbmpop_t1i(trg, 1)) {
                                rc = xwos_sem_trywait(&xwseldemo_sem1);
                                if (XWOK == rc) {
                                        ts = xwos_skd_get_timestamp_lc();
                                        sellogf(INFO,
                                                "[消费者] 信号量1触发，"
                                                "时间戳：%lld 纳秒。\n", ts);
                                }
                        }

                        if (xwbmpop_t1i(trg, 2)) {
                                rc = xwos_sem_trywait(&xwseldemo_sem2);
                                if (XWOK == rc) {
                                        ts = xwos_skd_get_timestamp_lc();
                                        sellogf(INFO,
                                                "[消费者] 信号量2触发，"
                                                "时间戳：%lld 纳秒。\n", ts);
                                }
                        }

                        if (xwbmpop_t1i(trg, 3)) {
                                ts = xwos_skd_get_timestamp_lc();
                                sellogf(INFO,
                                        "[消费者] 事件标志3触发，"
                                        "时间戳：%lld 纳秒。\n", ts);
                                flgmsk[0] = XWBOP_BIT(0) | XWBOP_BIT(1);
                                rc = xwos_flg_trywait(&xwseldemo_flg3,
                                                      XWOS_FLG_TRIGGER_SET_ANY,
                                                      XWOS_FLG_ACTION_CONSUMPTION,
                                                      flgtrg, flgmsk);
                                if (XWOK == rc) {
                                        ts = xwos_skd_get_timestamp_lc();
                                        if (xwbmpop_t1i(flgtrg, 0)) {
                                                sellogf(INFO,
                                                        "[消费者] 定时器0触发，"
                                                        "时间戳：%lld 纳秒。\n", ts);
                                        }
                                        if (xwbmpop_t1i(flgtrg, 1)) {
                                                sellogf(INFO,
                                                        "[消费者] 定时器1触发，"
                                                        "时间戳：%lld 纳秒。\n", ts);
                                        }
                                }
                        }

                        if (xwbmpop_t1i(trg, 4)) {
                                ts = xwos_skd_get_timestamp_lc();
                                sellogf(INFO,
                                        "[消费者] 选择器4触发，"
                                        "时间戳：%lld 纳秒。\n", ts);
                                /* 第4位为子信号选择器4，再次测试 */
                                rc = xwos_sel_tryselect(&xwseldemo_sel4, selmsk, trg);
                                if (XWOK == rc) {
                                        ts = xwos_skd_get_timestamp_lc();
                                        if (xwbmpop_t1i(trg, 5)) {
                                                sellogf(INFO,
                                                        "[消费者] 条件量5触发，"
                                                        "时间戳：%lld 纳秒。\n", ts);
                                        }
                                        if (xwbmpop_t1i(trg, 6)) {
                                                sellogf(INFO,
                                                        "[消费者] 条件量6触发，"
                                                        "时间戳：%lld 纳秒。\n", ts);
                                        }
                                        if (xwbmpop_t1i(trg, 7)) {
                                                sellogf(INFO,
                                                        "[消费者] 线程栅栏7触发，"
                                                        "时间戳：%lld 纳秒。\n", ts);
                                        }
                                }
                        }

                        sellogf(INFO, "\n");
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(ERR,
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
 *   - 当配置(XWUPCFG_SKD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWUPCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * - MP系统
 *   - 当配置(XWMPCFG_SKD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWMPCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void xwseldemo_swt0_callback(struct xwos_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_flg_s1i(&xwseldemo_flg3, 0);
}

/**
 * @brief 定时器1的回调函数
 * @note
 * - UP系统
 *   - 当配置(XWUPCFG_SKD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWUPCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * - MP系统
 *   - 当配置(XWMPCFG_SKD_BH == 1)，此函数运行在中断底半部；
 *   - 当配置(XWMPCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * - 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void xwseldemo_swt1_callback(struct xwos_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_flg_s1i(&xwseldemo_flg3, 1);
}

/**
 * @brief 生产者线程
 */
xwer_t xwseldemo_producer_func(void * arg)
{
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        sellogf(INFO, "[生产者] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwos_sem_post(&xwseldemo_sem1);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(INFO,
                                "[生产者] 发布信号量1，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                rc = xwos_sem_post(&xwseldemo_sem2);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(INFO,
                                "[生产者] 发布信号量2，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                rc = xwos_cond_broadcast(&xwseldemo_cond5);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(INFO,
                                "[生产者] 发布条件量5，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                rc = xwos_cond_broadcast(&xwseldemo_cond6);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(INFO,
                                "[生产者] 发布条件量6，时间戳：%lld 纳秒\n", ts);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);
        }

        sellogf(INFO, "[生产者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 竞赛线程
 */
xwer_t xwseldemo_syncthd_func(void * arg)
{
        xwbmpop_declare(brmsk, XWSELDEMO_BMP_BITNUM);
        xwtm_t sleep;
        xwsq_t pos = (xwsq_t)arg; /* 获取线程的各自的位置 */
        xwer_t rc;

        sellogf(INFO, "[同步线程%d] 启动。\n", pos);
        xwbmpop_c0all(brmsk, XWSELDEMO_BMP_BITNUM);

        /* 设置位图掩码 */
        for (xwsq_t i = 0; i < xw_array_size(xwseldemo_sync_thd_desc); i++) {
                xwbmpop_s1i(brmsk, i);
        }
        sleep = (pos + 1) * XWTM_S;
        xwos_cthd_sleep(&sleep);

        /* 同步线程 */
        rc = xwos_br_sync(&xwseldemo_br7, pos, brmsk);
        if (XWOK == rc) {
                sellogf(INFO, "[同步线程%d] 已就位。\n", pos);
        }

        sellogf(INFO, "[同步线程%d] 退出。\n", pos);
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
