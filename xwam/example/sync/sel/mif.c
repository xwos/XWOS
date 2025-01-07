/**
 * @file
 * @brief 示例：信号选择器
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/swt.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/osal/sync/flg.h>
#include <xwos/osal/sync/br.h>
#include <xwos/osal/sync/sel.h>
#include <xwam/example/sync/sel/mif.h>

#define LOGTAG "sel"
#define sellogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define SELDEMO_CONSUMER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
#define SELDEMO_PRODUCER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 2)
#define SELDEMO_BRTHD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

void seldemo_swt0_callback(struct xwos_swt * swt, void * arg);
void seldemo_swt1_callback(struct xwos_swt * swt, void * arg);
xwer_t seldemo_consumer_func(void * arg);
xwer_t seldemo_producer_func(void * arg);
xwer_t seldemo_syncthd_func(void * arg);

#define SELDEMO_BMP_BITNUM (sizeof(xwbmp_t) * BITS_PER_XWU8_T)

xwbmpop_define(seldemo_sel0_bmp, SELDEMO_BMP_BITNUM) = {0,};
xwbmpop_define(seldemo_sel0_msk, SELDEMO_BMP_BITNUM) = {0,};
struct xwos_sel seldemo_sel0;

struct xwos_sem seldemo_sem1;
struct xwos_sem seldemo_sem2;

xwbmpop_define(seldemo_flg3_bmp, SELDEMO_BMP_BITNUM) = {0,};
xwbmpop_define(seldemo_flg3_msk, SELDEMO_BMP_BITNUM) = {0,};
struct xwos_flg seldemo_flg3;

xwbmpop_define(seldemo_sel4_bmp, SELDEMO_BMP_BITNUM) = {0,};
xwbmpop_define(seldemo_sel4_msk, SELDEMO_BMP_BITNUM) = {0,};
struct xwos_sel seldemo_sel4;

struct xwos_cond seldemo_cond5;
struct xwos_cond seldemo_cond6;

struct xwos_br seldemo_br7;

const struct xwos_thd_desc seldemo_consumer_thd_desc = {
        .attr = {
                .name = "seldemo.consumer",
                .stack = NULL,
                .stack_size = 4096,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = SELDEMO_CONSUMER_PRIORITY,
                .detached = false,
                .privileged = true,
        },
        .func = (xwos_thd_f)seldemo_consumer_func,
        .arg = NULL,
};
xwos_thd_d seldemo_consumer;

const struct xwos_thd_desc seldemo_producer_thd_desc = {
        .attr = {
                .name = "seldemo.producer",
                .stack = NULL,
                .stack_size = 2048,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = SELDEMO_PRODUCER_PRIORITY,
                .detached = false,
                .privileged = true,
        },
        .func = (xwos_thd_f)seldemo_producer_func,
        .arg = NULL,
};
xwos_thd_d seldemo_producer;

struct xwos_swt seldemo_swt0;
struct xwos_swt seldemo_swt1;

const struct xwos_thd_desc seldemo_sync_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "seldemo.syncthd.0",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = SELDEMO_BRTHD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)seldemo_syncthd_func,
                .arg = (void *)0,
        },
        [1] = {
                .attr = {
                        .name = "seldemo.syncthd.1",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = SELDEMO_BRTHD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)seldemo_syncthd_func,
                .arg = (void *)1,
        },
};
xwos_thd_d seldemo_syncthd[xw_array_size(seldemo_sync_thd_desc)];
xwbmpop_define(seldemo_br7_bmp, xw_array_size(seldemo_sync_thd_desc)) = {0,};
xwbmpop_define(seldemo_br7_msk, xw_array_size(seldemo_sync_thd_desc)) = {0,};

/**
 * @brief 测试模块的启动函数
 */
xwer_t xwos_example_sel(void)
{
        xwer_t rc;
        xwsq_t i;

        /* 初始化信号选择器0 */
        rc = xwos_sel_init(&seldemo_sel0, SELDEMO_BMP_BITNUM,
                           seldemo_sel0_bmp, seldemo_sel0_msk);
        if (rc < 0) {
                goto err_sel0_init;
        }
        /* 初始化信号量1 */
        rc = xwos_sem_init(&seldemo_sem1, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_sem1_init;
        }
        /* 初始化信号量2 */
        rc = xwos_sem_init(&seldemo_sem2, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_sem2_init;
        }
        /* 初始化事件标志3 */
        rc = xwos_flg_init(&seldemo_flg3, SELDEMO_BMP_BITNUM,
                           seldemo_flg3_bmp, seldemo_flg3_msk);
        if (rc < 0) {
                goto err_flg3_init;
        }
        /* 初始化信号选择器4 */
        rc = xwos_sel_init(&seldemo_sel4, SELDEMO_BMP_BITNUM,
                           seldemo_sel4_bmp, seldemo_sel4_msk);
        if (rc < 0) {
                goto err_sel4_init;
        }
        /* 初始化条件量5 */
        rc = xwos_cond_init(&seldemo_cond5);
        if (rc < 0) {
                goto err_cond5_init;
        }
        /* 初始化条件量6 */
        rc = xwos_cond_init(&seldemo_cond6);
        if (rc < 0) {
                goto err_cond6_init;
        }
        /* 初始化线程屏障7 */
        xwbmpop_c0all(seldemo_br7_msk, xw_array_size(seldemo_sync_thd_desc));
        rc = xwos_br_init(&seldemo_br7, xw_array_size(seldemo_sync_thd_desc),
                          seldemo_br7_bmp, seldemo_br7_msk);
        if (rc < 0) {
                goto err_br7_init;
        }

        /* 初始化定时器0 */
        rc = xwos_swt_init(&seldemo_swt0, XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt0_init;
        }

        /* 初始化定时器1 */
        rc = xwos_swt_init(&seldemo_swt1, XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt1_init;
        }

        /* 绑定信号量1到信号选择器0的位置为1 */
        rc = xwos_sem_bind(&seldemo_sem1, &seldemo_sel0, 1);
        if (rc < 0) {
                goto err_sem1_bind;
        }
        /* 绑定信号量2到信号选择器0的位置为2 */
        rc = xwos_sem_bind(&seldemo_sem2, &seldemo_sel0, 2);
        if (rc < 0) {
                goto err_sem2_bind;
        }
        /* 绑定事件标志3到信号选择器0的位置为3 */
        rc = xwos_flg_bind(&seldemo_flg3, &seldemo_sel0, 3);
        if (rc < 0) {
                goto err_flg3_bind;
        }

        /* 绑定信号选择器4到信号选择器0的位置为4 */
        rc = xwos_sel_bind(&seldemo_sel4, &seldemo_sel0, 4);
        if (rc < 0) {
                goto err_sel4_bind;
        }

        /* 绑定条件量5到信号选择器4的位置为5 */
        rc = xwos_cond_bind(&seldemo_cond5, &seldemo_sel4, 5);
        if (rc < 0) {
                goto err_cond5_bind;
        }
        /* 绑定条件量6到信号选择器4的位置为6 */
        rc = xwos_cond_bind(&seldemo_cond6, &seldemo_sel4, 6);
        if (rc < 0) {
                goto err_cond6_bind;
        }
        /* 绑定线程屏障7到信号选择器4的位置7 */
        rc = xwos_br_bind(&seldemo_br7, &seldemo_sel4, 7);
        if (rc < 0) {
                goto err_br7_bind;
        }

        /* 创建等待同步对象的线程 */
        rc = xwos_thd_create(&seldemo_consumer,
                             &seldemo_consumer_thd_desc.attr,
                             seldemo_consumer_thd_desc.func,
                             seldemo_consumer_thd_desc.arg);
        if (rc < 0) {
                goto err_consumer_create;
        }

        /* 建立发布同步对象的线程 */
        rc = xwos_thd_create(&seldemo_producer,
                             &seldemo_producer_thd_desc.attr,
                             seldemo_producer_thd_desc.func,
                             seldemo_producer_thd_desc.arg);
        if (rc < 0) {
                goto err_producer_create;
        }

        /* 创建同步线程 */
        for (i = 0; i < xw_array_size(seldemo_sync_thd_desc); i++) {
                rc = xwos_thd_create(&seldemo_syncthd[i],
                                     &seldemo_sync_thd_desc[i].attr,
                                     seldemo_sync_thd_desc[i].func,
                                     seldemo_sync_thd_desc[i].arg);
                if (rc < 0) {
                        goto err_syncthd_create;
                }
        }

        return XWOK;

err_syncthd_create:
        xwos_thd_quit(seldemo_producer);
err_producer_create:
        xwos_thd_quit(seldemo_consumer);
err_consumer_create:
        xwos_br_unbind(&seldemo_br7, &seldemo_sel4);
err_br7_bind:
        xwos_cond_unbind(&seldemo_cond6, &seldemo_sel4);
err_cond6_bind:
        xwos_cond_unbind(&seldemo_cond5, &seldemo_sel4);
err_cond5_bind:
        xwos_sel_unbind(&seldemo_sel4, &seldemo_sel0);
err_sel4_bind:
        xwos_flg_unbind(&seldemo_flg3, &seldemo_sel0);
err_flg3_bind:
        xwos_sem_unbind(&seldemo_sem2, &seldemo_sel0);
err_sem2_bind:
        xwos_sem_unbind(&seldemo_sem1, &seldemo_sel0);
err_sem1_bind:
        xwos_swt_fini(&seldemo_swt1);
err_swt1_init:
        xwos_swt_fini(&seldemo_swt0);
err_swt0_init:
        xwos_br_fini(&seldemo_br7);
err_br7_init:
        xwos_cond_fini(&seldemo_cond6);
err_cond6_init:
        xwos_cond_fini(&seldemo_cond5);
err_cond5_init:
        xwos_sel_fini(&seldemo_sel4);
err_sel4_init:
        xwos_flg_fini(&seldemo_flg3);
err_flg3_init:
        xwos_sem_fini(&seldemo_sem2);
err_sem2_init:
        xwos_sem_fini(&seldemo_sem1);
err_sem1_init:
        xwos_sel_fini(&seldemo_sel0);
err_sel0_init:
        return rc;
}

/**
 * @brief 消费者线程
 */
xwer_t seldemo_consumer_func(void * arg)
{
        xwbmpop_define(selmsk, SELDEMO_BMP_BITNUM);
        xwbmpop_define(trg, SELDEMO_BMP_BITNUM);
        xwbmpop_define(flgmsk, SELDEMO_BMP_BITNUM);
        xwbmpop_define(flgtrg, SELDEMO_BMP_BITNUM);
        xwtm_t now;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        sellogf(INFO, "[消费者] 启动。\n");
        sellogf(INFO, "[消费者] 启动定时器0。\n");
        now = xwtm_now();
        rc = xwos_swt_start(&seldemo_swt0, now, XWTM_MS(500),
                            seldemo_swt0_callback, NULL);

        sellogf(INFO, "[消费者] 启动定时器1。\n");
        rc = xwos_swt_start(&seldemo_swt1, now, XWTM_MS(800),
                            seldemo_swt1_callback, NULL);

        memset(selmsk, 0, sizeof(selmsk));
        memset(flgmsk, 0, sizeof(flgmsk));
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                /* 设置掩码位为bit1 ~ bit7共7位 */
                selmsk[0] = 0xFF;
                now = xwtm_nowts();
                sellogf(INFO,
                        "[消费者] 等待信号：0x%X，时间戳：%lld 纳秒...\n",
                        selmsk[0], now);
                rc = xwos_sel_select_to(&seldemo_sel0, selmsk, trg,
                                        xwtm_ft(XWTM_S(1)));
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        sellogf(INFO,
                                "[消费者] 唤醒，触发信号：0x%X，时间戳：%lld 纳秒。\n",
                                trg[0], now);

                        if (xwbmpop_t1i(trg, 1)) {
                                rc = xwos_sem_trywait(&seldemo_sem1);
                                if (XWOK == rc) {
                                        now = xwtm_nowts();
                                        sellogf(INFO,
                                                "[消费者] 信号量1触发，"
                                                "时间戳：%lld 纳秒。\n", now);
                                }
                        }

                        if (xwbmpop_t1i(trg, 2)) {
                                rc = xwos_sem_trywait(&seldemo_sem2);
                                if (XWOK == rc) {
                                        now = xwtm_nowts();
                                        sellogf(INFO,
                                                "[消费者] 信号量2触发，"
                                                "时间戳：%lld 纳秒。\n", now);
                                }
                        }

                        if (xwbmpop_t1i(trg, 3)) {
                                now = xwtm_nowts();
                                sellogf(INFO,
                                        "[消费者] 事件标志3触发，"
                                        "时间戳：%lld 纳秒。\n", now);
                                flgmsk[0] = XWBOP_BIT(0) | XWBOP_BIT(1);
                                rc = xwos_flg_trywait(&seldemo_flg3,
                                                      XWOS_FLG_TRIGGER_SET_ANY,
                                                      true,
                                                      flgtrg, flgmsk);
                                if (XWOK == rc) {
                                        now = xwtm_nowts();
                                        if (xwbmpop_t1i(flgtrg, 0)) {
                                                sellogf(INFO,
                                                        "[消费者] 定时器0触发，"
                                                        "时间戳：%lld 纳秒。\n", now);
                                        }
                                        if (xwbmpop_t1i(flgtrg, 1)) {
                                                sellogf(INFO,
                                                        "[消费者] 定时器1触发，"
                                                        "时间戳：%lld 纳秒。\n", now);
                                        }
                                }
                        }

                        if (xwbmpop_t1i(trg, 4)) {
                                now = xwtm_nowts();
                                sellogf(INFO,
                                        "[消费者] 选择器4触发，"
                                        "时间戳：%lld 纳秒。\n", now);
                                /* 第4位为子信号选择器4，再次测试 */
                                rc = xwos_sel_tryselect(&seldemo_sel4, selmsk, trg);
                                if (XWOK == rc) {
                                        now = xwtm_nowts();
                                        if (xwbmpop_t1i(trg, 5)) {
                                                sellogf(INFO,
                                                        "[消费者] 条件量5触发，"
                                                        "时间戳：%lld 纳秒。\n", now);
                                        }
                                        if (xwbmpop_t1i(trg, 6)) {
                                                sellogf(INFO,
                                                        "[消费者] 条件量6触发，"
                                                        "时间戳：%lld 纳秒。\n", now);
                                        }
                                        if (xwbmpop_t1i(trg, 7)) {
                                                sellogf(INFO,
                                                        "[消费者] 线程栅栏7触发，"
                                                        "时间戳：%lld 纳秒。\n", now);
                                        }
                                }
                        }

                        sellogf(INFO, "\n");
                } else {
                        now = xwtm_nowts();
                        sellogf(ERR,
                                "[消费者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }
        }
        return rc;
}

/**
 * @brief 定时器0的回调函数
 * @note
 * + 当配置(XWOSCFG_SKD_BH == 1)，此函数运行在中断底半部；
 * + 当配置(XWOSCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * + 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void seldemo_swt0_callback(struct xwos_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_flg_s1i(&seldemo_flg3, 0);
}

/**
 * @brief 定时器1的回调函数
 * @note
 * + 当配置(XWOSCFG_SKD_BH == 1)，此函数运行在中断底半部；
 * + 当配置(XWOSCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * + 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void seldemo_swt1_callback(struct xwos_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_flg_s1i(&seldemo_flg3, 1);
}

/**
 * @brief 生产者线程
 */
xwer_t seldemo_producer_func(void * arg)
{
        xwtm_t now;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        sellogf(INFO, "[生产者] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwos_sem_post(&seldemo_sem1);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        sellogf(INFO,
                                "[生产者] 发布信号量1，时间戳：%lld 纳秒\n", now);
                } else {
                        now = xwtm_nowts();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }

                xwos_cthd_sleep(XWTM_MS(1000));

                rc = xwos_sem_post(&seldemo_sem2);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        sellogf(INFO,
                                "[生产者] 发布信号量2，时间戳：%lld 纳秒\n", now);
                } else {
                        now = xwtm_nowts();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }
                xwos_cthd_sleep(XWTM_MS(1000));

                rc = xwos_cond_broadcast(&seldemo_cond5);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        sellogf(INFO,
                                "[生产者] 发布条件量5，时间戳：%lld 纳秒\n", now);
                } else {
                        now = xwtm_nowts();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }
                xwos_cthd_sleep(XWTM_MS(1000));

                rc = xwos_cond_broadcast(&seldemo_cond6);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        sellogf(INFO,
                                "[生产者] 发布条件量6，时间戳：%lld 纳秒\n", now);
                } else {
                        now = xwtm_nowts();
                        sellogf(ERR,
                                "[生产者] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }
                xwos_cthd_sleep(XWTM_MS(1000));
        }

        sellogf(INFO, "[生产者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 竞赛线程
 */
xwer_t seldemo_syncthd_func(void * arg)
{
        xwtm_t sleep;
        xwsq_t pos = (xwsq_t)arg;
        xwer_t rc;

        sellogf(INFO, "[同步线程%d] 启动。\n", pos);

        /* 睡眠一段时间 */
        sleep = XWTM_S(pos + 1);
        xwos_cthd_sleep(sleep);

        /* 同步线程 */
        sellogf(INFO, "[同步线程%d] 已就位。\n", pos);
        rc = xwos_br_wait(&seldemo_br7);
        if (XWOK == rc) {
                sellogf(INFO, "[同步线程%d] 开始运行。\n", pos);
        }

        sellogf(INFO, "[同步线程%d] 退出。\n", pos);
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
