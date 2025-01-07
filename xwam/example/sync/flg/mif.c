/**
 * @file
 * @brief 示例：事件标志
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
#include <xwos/osal/sync/flg.h>
#include <xwam/example/sync/flg/mif.h>

#define LOGTAG "flag"
#define flglogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define FLGDEMO_CONSUMER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 2)
#define FLGDEMO_PRODUCER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t flgdemo_consumer_func(void * arg);
xwer_t flgdemo_producer_func(void * arg);

/**
 * @brief 静态定义的事件标志
 */
#define FLGDEMO_FLG_NUM (sizeof(xwbmp_t) * BITS_PER_XWU8_T)
xwbmpop_define(flgdemo_flg_bmp, FLGDEMO_FLG_NUM) = {0,};
xwbmpop_define(flgdemo_flg_msk, FLGDEMO_FLG_NUM) = {0,};
struct xwos_flg flgdemo_flg;

/**
 * @brief 消费线程
 */
const struct xwos_thd_desc flgdemo_consumer_thd_desc = {
        .attr = {
                .name = "example.flag.consumer",
                .stack = NULL,
                .stack_size = 4096,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = FLGDEMO_CONSUMER_PRIORITY,
                .detached = false,
                .privileged = true,
        },
        .func = (xwos_thd_f)flgdemo_consumer_func,
        .arg = NULL,
};
xwos_thd_d flgdemo_consumer;

/**
 * @brief 生产线程
 */
const struct xwos_thd_desc flgdemo_producer_thd_desc = {
        .attr = {
                .name = "example.flag.producer",
                .stack = NULL,
                .stack_size = 4096,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = FLGDEMO_PRODUCER_PRIORITY,
                .detached = false,
                .privileged = true,
        },
        .func = (xwos_thd_f)flgdemo_producer_func,
        .arg = NULL,
};
xwos_thd_d flgdemo_producer;

/**
 * @brief 测试模块的启动函数
 */
xwer_t xwos_example_flg(void)
{
        xwer_t rc;

        /* 初始化事件标志 */
        rc = xwos_flg_init(&flgdemo_flg, FLGDEMO_FLG_NUM,
                           flgdemo_flg_bmp, flgdemo_flg_msk);
        if (rc < 0) {
                goto err_flg_init;
        }

        /* 创建等待事件标志的线程 */
        rc = xwos_thd_create(&flgdemo_consumer,
                             &flgdemo_consumer_thd_desc.attr,
                             flgdemo_consumer_thd_desc.func,
                             flgdemo_consumer_thd_desc.arg);
        if (rc < 0) {
                goto err_consumer_create;
        }

        /* 创建发布事件标志的线程 */
        rc = xwos_thd_create(&flgdemo_producer,
                             &flgdemo_producer_thd_desc.attr,
                             flgdemo_producer_thd_desc.func,
                             flgdemo_producer_thd_desc.arg);
        if (rc < 0) {
                goto err_producer_create;
        }

        return XWOK;


err_producer_create:
        xwos_thd_quit(flgdemo_consumer);
err_consumer_create:
        xwos_flg_fini(&flgdemo_flg);
err_flg_init:
        return rc;
}

/**
 * @brief 等待事件标志的线程函数（消费线程）
 */
xwer_t flgdemo_consumer_func(void * arg)
{
        xwtm_t now;
        xwer_t rc = XWOK;
        xwbmpop_define(msk, FLGDEMO_FLG_NUM) = {0,};
        xwbmpop_define(org, FLGDEMO_FLG_NUM) = {0,};
        xwbmpop_define(tgevt, FLGDEMO_FLG_NUM) = {0,};

        XWOS_UNUSED(arg);
        flglogf(INFO, "[等待线程] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：SET_ALL，是否消费事件：是...\n",
                        msk[0]);
                rc = xwos_flg_wait(&flgdemo_flg,
                                   XWOS_FLG_TRIGGER_SET_ALL,
                                   true,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                now, tgevt[0], org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：SET_ANY，是否消费事件：否...\n",
                        msk[0]);
                rc = xwos_flg_wait(&flgdemo_flg,
                                   XWOS_FLG_TRIGGER_SET_ANY,
                                   false,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                now, tgevt[0], org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：CLR_ALL，是否消费事件：是...\n",
                        msk[0]);
                rc = xwos_flg_wait(&flgdemo_flg,
                                   XWOS_FLG_TRIGGER_CLR_ALL,
                                   true,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                now, tgevt[0], org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：CLR_ANY，是否消费事件：否...\n",
                        msk[0]);
                rc = xwos_flg_wait(&flgdemo_flg,
                                   XWOS_FLG_TRIGGER_CLR_ANY,
                                   false,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                now, tgevt[0], org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }

                rc = xwos_flg_read(&flgdemo_flg, org); /* 读取初始值 */
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：TGL_ALL，初始值：0x%X...\n",
                        msk[0], org[0]);
                rc = xwos_flg_wait(&flgdemo_flg,
                                   XWOS_FLG_TRIGGER_TGL_ALL,
                                   false,
                                   org, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，事件：0x%X。\n\n",
                                now, org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：TGL_ANY，初始值：0x%X...\n",
                        msk[0], org[0]);
                rc = xwos_flg_wait(&flgdemo_flg,
                                   XWOS_FLG_TRIGGER_TGL_ANY,
                                   false,
                                   org, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，事件：0x%X。\n\n",
                                now, org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                now, rc);
                }
        }

        flglogf(INFO, "[等待线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 触发事件的线程函数（生产线程）
 */
xwer_t flgdemo_producer_func(void * arg)
{
        xwbmpop_define(org, FLGDEMO_FLG_NUM) = {0,};
        xwbmpop_define(msk, FLGDEMO_FLG_NUM) = {0,};
        xwtm_t now;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        flglogf(INFO, "[触发线程] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                /* 休眠1000ms，让出CPU使用权 */
                xwos_cthd_sleep(XWTM_MS(1000));

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部置1 */
                rc = xwos_flg_s1m(&flgdemo_flg, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "设置事件：掩码0x%X，当前事件：0x%X。\n",
                                now, msk[0], org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                now, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                xwos_cthd_sleep(XWTM_MS(1000));

                /* 将事件位图的第3位置1 */
                rc = xwos_flg_s1i(&flgdemo_flg, 3);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "设置事件：第3位，当前事件：0x%X。\n",
                                now, org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                now, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                xwos_cthd_sleep(XWTM_MS(1000));

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部清0 */
                rc = xwos_flg_c0m(&flgdemo_flg, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "清零事件：掩码0x%X，当前事件：0x%X。\n",
                                now, msk[0], org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                now, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                xwos_cthd_sleep(XWTM_MS(1000));

                /* 将事件位图的第7位清0 */
                rc = xwos_flg_c0i(&flgdemo_flg, 7);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "清零事件：第7位，当前事件：0x%X。\n",
                                now, org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                now, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                xwos_cthd_sleep(XWTM_MS(1000));

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部翻转 */
                rc = xwos_flg_x1m(&flgdemo_flg, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "翻转事件：掩码0x%X，当前事件：0x%X。\n",
                                now, msk[0], org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                now, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                xwos_cthd_sleep(XWTM_MS(1000));

                /* 将事件位图的第5位翻转 */
                rc = xwos_flg_x1i(&flgdemo_flg, 5);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&flgdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "翻转事件：第5位，当前事件：0x%X。\n",
                                now, org[0]);
                } else {
                        now = xwtm_nowts();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                now, rc);
                }
        }

        flglogf(INFO, "[触发线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
