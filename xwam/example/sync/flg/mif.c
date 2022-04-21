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

#define XWFLGDEMO_CONSUMER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 2)
#define XWFLGDEMO_PRODUCER_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t xwflgdemo_consumer_func(void * arg);
xwer_t xwflgdemo_producer_func(void * arg);

/**
 * @brief 静态定义的事件标志
 */
#define XWFLGDEMO_FLG_NUM (sizeof(xwbmp_t) * BITS_PER_XWU8_T)
xwbmpop_declare(xwflgdemo_flg_bmp, XWFLGDEMO_FLG_NUM) = {0,};
xwbmpop_declare(xwflgdemo_flg_msk, XWFLGDEMO_FLG_NUM) = {0,};
struct xwos_flg xwflgdemo_flg;

/**
 * @brief 消费线程
 */
const struct xwos_thd_desc xwflgdemo_consumer_thd_desc = {
        .attr = {
                .name = "example.flag.consumer",
                .stack = NULL,
                .stack_size = 4096,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = XWFLGDEMO_CONSUMER_PRIORITY,
                .detached = false,
                .privileged = true,
        },
        .func = (xwos_thd_f)xwflgdemo_consumer_func,
        .arg = NULL,
};
xwos_thd_d xwflgdemo_consumer;

/**
 * @brief 生产线程
 */
const struct xwos_thd_desc xwflgdemo_producer_thd_desc = {
        .attr = {
                .name = "example.flag.producer",
                .stack = NULL,
                .stack_size = 4096,
                .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                .priority = XWFLGDEMO_PRODUCER_PRIORITY,
                .detached = false,
                .privileged = true,
        },
        .func = (xwos_thd_f)xwflgdemo_producer_func,
        .arg = NULL,
};
xwos_thd_d xwflgdemo_producer;

/**
 * @brief 测试模块的启动函数
 */
xwer_t example_flg_start(void)
{
        xwer_t rc;

        /* 初始化事件标志 */
        rc = xwos_flg_init(&xwflgdemo_flg, XWFLGDEMO_FLG_NUM,
                           xwflgdemo_flg_bmp, xwflgdemo_flg_msk);
        if (rc < 0) {
                goto err_flg_init;
        }

        /* 创建等待事件标志的线程 */
        rc = xwos_thd_create(&xwflgdemo_consumer,
                             &xwflgdemo_consumer_thd_desc.attr,
                             xwflgdemo_consumer_thd_desc.func,
                             xwflgdemo_consumer_thd_desc.arg);
        if (rc < 0) {
                goto err_consumer_create;
        }

        /* 创建发布事件标志的线程 */
        rc = xwos_thd_create(&xwflgdemo_producer,
                             &xwflgdemo_producer_thd_desc.attr,
                             xwflgdemo_producer_thd_desc.func,
                             xwflgdemo_producer_thd_desc.arg);
        if (rc < 0) {
                goto err_producer_create;
        }

        return XWOK;


err_producer_create:
        xwos_thd_quit(xwflgdemo_consumer);
err_consumer_create:
        xwos_flg_fini(&xwflgdemo_flg);
err_flg_init:
        return rc;
}

/**
 * @brief 等待事件标志的线程函数（消费线程）
 */
xwer_t xwflgdemo_consumer_func(void * arg)
{
        xwtm_t now;
        xwer_t rc = XWOK;
        xwbmpop_declare(msk, XWFLGDEMO_FLG_NUM) = {0,};
        xwbmpop_declare(org, XWFLGDEMO_FLG_NUM) = {0,};
        xwbmpop_declare(tgevt, XWFLGDEMO_FLG_NUM) = {0,};

        XWOS_UNUSED(arg);
        flglogf(INFO, "[等待线程] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：SET_ALL，是否消费事件：是...\n",
                        msk[0]);
                rc = xwos_flg_wait(&xwflgdemo_flg,
                                   XWOS_FLG_TRIGGER_SET_ALL,
                                   XWOS_FLG_ACTION_CONSUMPTION,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                rc = xwos_flg_wait(&xwflgdemo_flg,
                                   XWOS_FLG_TRIGGER_SET_ANY,
                                   XWOS_FLG_ACTION_NONE,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                rc = xwos_flg_wait(&xwflgdemo_flg,
                                   XWOS_FLG_TRIGGER_CLR_ALL,
                                   XWOS_FLG_ACTION_CONSUMPTION,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                rc = xwos_flg_wait(&xwflgdemo_flg,
                                   XWOS_FLG_TRIGGER_CLR_ANY,
                                   XWOS_FLG_ACTION_NONE,
                                   tgevt, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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

                rc = xwos_flg_read(&xwflgdemo_flg, org); /* 读取初始值 */
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：TGL_ALL，初始值：0x%X...\n",
                        msk[0], org[0]);
                rc = xwos_flg_wait(&xwflgdemo_flg,
                                   XWOS_FLG_TRIGGER_TGL_ALL,
                                   XWOS_UNUSED_ARGUMENT,
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
                rc = xwos_flg_wait(&xwflgdemo_flg,
                                   XWOS_FLG_TRIGGER_TGL_ANY,
                                   XWOS_UNUSED_ARGUMENT,
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
xwer_t xwflgdemo_producer_func(void * arg)
{
        xwbmpop_declare(org, XWFLGDEMO_FLG_NUM) = {0,};
        xwbmpop_declare(msk, XWFLGDEMO_FLG_NUM) = {0,};
        xwtm_t now;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        flglogf(INFO, "[触发线程] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                /* 休眠1000ms，让出CPU使用权 */
                xwos_cthd_sleep(1000 * XWTM_MS);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部置1 */
                rc = xwos_flg_s1m(&xwflgdemo_flg, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                xwos_cthd_sleep(1000 * XWTM_MS);

                /* 将事件位图的第3位置1 */
                rc = xwos_flg_s1i(&xwflgdemo_flg, 3);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                xwos_cthd_sleep(1000 * XWTM_MS);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部清0 */
                rc = xwos_flg_c0m(&xwflgdemo_flg, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                xwos_cthd_sleep(1000 * XWTM_MS);

                /* 将事件位图的第7位清0 */
                rc = xwos_flg_c0i(&xwflgdemo_flg, 7);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                xwos_cthd_sleep(1000 * XWTM_MS);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部翻转 */
                rc = xwos_flg_x1m(&xwflgdemo_flg, msk);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
                xwos_cthd_sleep(1000 * XWTM_MS);

                /* 将事件位图的第5位翻转 */
                rc = xwos_flg_x1i(&xwflgdemo_flg, 5);
                if (XWOK == rc) {
                        now = xwtm_nowts();
                        xwos_flg_read(&xwflgdemo_flg, org);
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
