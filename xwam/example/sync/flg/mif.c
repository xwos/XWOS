/**
 * @file
 * @brief 示例：事件标志
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
#include <xwos/osal/sync/flg.h>
#include <xwam/example/sync/flg/mif.h>

#define XWFLGDEMO_CONSUMER_PRIORITY                             \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 2)

#define XWFLGDEMO_PRODUCER_PRIORITY                             \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
#define XWFLGDEMO_LOG_TAG             "flag"
#define flglogf(lv, fmt, ...)                                   \
        xwlogf(lv, XWFLGDEMO_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
#define flglogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

xwer_t xwflgdemo_consumer_func(void * arg);
xwer_t xwflgdemo_producer_func(void * arg);

struct xwos_flg xwsltdemo_flg;

const struct xwos_thd_desc xwflgdemo_consumer_td = {
        .name = "example.flag.consumer",
        .prio = XWFLGDEMO_CONSUMER_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 4096,
        .func = (xwos_thd_f)xwflgdemo_consumer_func,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
xwos_thd_d xwflgdemo_consumer;

const struct xwos_thd_desc xwflgdemo_producer_td = {
        .name = "example.flag.producer",
        .prio = XWFLGDEMO_PRODUCER_PRIORITY,
        .stack = XWOS_THD_STACK_DYNAMIC,
        .stack_size = 4096,
        .func = (xwos_thd_f)xwflgdemo_producer_func,
        .arg = NULL,
        .attr = XWOS_SKDATTR_PRIVILEGED,
};
xwos_thd_d xwflgdemo_producer;

/**
 * @brief 测试模块的启动函数
 */
xwer_t example_flg_start(void)
{
        xwer_t rc;

        /* 初始化事件标志 */
        rc = xwos_flg_init(&xwsltdemo_flg, NULL);
        if (rc < 0) {
                goto err_flg_init;
        }

        /* 创建等待事件标志的线程 */
        rc = xwos_thd_create(&xwflgdemo_consumer,
                             xwflgdemo_consumer_td.name,
                             xwflgdemo_consumer_td.func,
                             xwflgdemo_consumer_td.arg,
                             xwflgdemo_consumer_td.stack_size,
                             xwflgdemo_consumer_td.prio,
                             xwflgdemo_consumer_td.attr);
        if (rc < 0) {
                goto err_consumer_create;
        }

        /* 创建发布事件标志的线程 */
        rc = xwos_thd_create(&xwflgdemo_producer,
                             xwflgdemo_producer_td.name,
                             xwflgdemo_producer_td.func,
                             xwflgdemo_producer_td.arg,
                             xwflgdemo_producer_td.stack_size,
                             xwflgdemo_producer_td.prio,
                             xwflgdemo_producer_td.attr);
        if (rc < 0) {
                goto err_producer_create;
        }

        return XWOK;


err_producer_create:
        xwos_thd_cancel(xwflgdemo_consumer);
err_consumer_create:
        xwos_flg_destroy(&xwsltdemo_flg);
err_flg_init:
        return rc;
}

/**
 * @brief 等待事件标志的线程
 */
xwer_t xwflgdemo_consumer_func(void * arg)
{
        xwos_flg_declare_bitmap(msk);
        xwos_flg_declare_bitmap(org);
        xwos_flg_declare_bitmap(tgevt);
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        flglogf(INFO, "[等待线程] 启动。\n");

        memset(msk, 0, sizeof(msk));

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：SET_ALL，是否消费事件：是...\n",
                        msk[0]);
                rc = xwos_flg_wait(&xwsltdemo_flg,
                                   XWOS_FLG_TRIGGER_SET_ALL,
                                   XWOS_FLG_ACTION_CONSUMPTION,
                                   tgevt, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：SET_ANY，是否消费事件：否...\n",
                        msk[0]);
                rc = xwos_flg_wait(&xwsltdemo_flg,
                                   XWOS_FLG_TRIGGER_SET_ANY,
                                   XWOS_FLG_ACTION_NONE,
                                   tgevt, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：CLR_ALL，是否消费事件：是...\n",
                        msk[0]);
                rc = xwos_flg_wait(&xwsltdemo_flg,
                                   XWOS_FLG_TRIGGER_CLR_ALL,
                                   XWOS_FLG_ACTION_CONSUMPTION,
                                   tgevt, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：CLR_ANY，是否消费事件：否...\n",
                        msk[0]);
                rc = xwos_flg_wait(&xwsltdemo_flg,
                                   XWOS_FLG_TRIGGER_CLR_ANY,
                                   XWOS_FLG_ACTION_NONE,
                                   tgevt, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                rc = xwos_flg_read(&xwsltdemo_flg, org); /* 读取初始值 */
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：TGL_ALL，初始值：0x%X...\n",
                        msk[0], org[0]);
                rc = xwos_flg_wait(&xwsltdemo_flg,
                                   XWOS_FLG_TRIGGER_TGL_ALL,
                                   XWOS_UNUSED_ARGUMENT,
                                   org, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，事件：0x%X。\n\n",
                                ts, org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 事件掩码：0x%X，"
                        "触发条件：TGL_ANY，初始值：0x%X...\n",
                        msk[0], org[0]);
                rc = xwos_flg_wait(&xwsltdemo_flg,
                                   XWOS_FLG_TRIGGER_TGL_ANY,
                                   XWOS_UNUSED_ARGUMENT,
                                   org, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，事件：0x%X。\n\n",
                                ts, org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }
        }

        flglogf(INFO, "[等待线程] 退出。\n");
        xwos_thd_delete(xwos_cthd_getd());
        return rc;
}

/**
 * @brief 触发事件的线程
 */
xwer_t xwflgdemo_producer_func(void * arg)
{
        xwos_flg_declare_bitmap(org);
        xwos_flg_declare_bitmap(msk);
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        flglogf(INFO, "[触发线程] 启动。\n");
        memset(msk, 0, sizeof(msk));

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部置1 */
                rc = xwos_flg_s1m(&xwsltdemo_flg, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "设置事件：掩码0x%X，当前事件：0x%X。\n",
                                ts, msk[0], org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                /* 将事件位图的第3位置1 */
                rc = xwos_flg_s1i(&xwsltdemo_flg, 3);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "设置事件：第3位，当前事件：0x%X。\n",
                                ts, org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部清0 */
                rc = xwos_flg_c0m(&xwsltdemo_flg, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "清零事件：掩码0x%X，当前事件：0x%X。\n",
                                ts, msk[0], org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                /* 将事件位图的第7位清0 */
                rc = xwos_flg_c0i(&xwsltdemo_flg, 7);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "清零事件：第7位，当前事件：0x%X。\n",
                                ts, org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部翻转 */
                rc = xwos_flg_x1m(&xwsltdemo_flg, msk);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "翻转事件：掩码0x%X，当前事件：0x%X。\n",
                                ts, msk[0], org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwos_cthd_sleep(&ts);

                /* 将事件位图的第5位翻转 */
                rc = xwos_flg_x1i(&xwsltdemo_flg, 5);
                if (XWOK == rc) {
                        ts = xwos_skd_get_timestamp_lc();
                        xwos_flg_read(&xwsltdemo_flg, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "翻转事件：第5位，当前事件：0x%X。\n",
                                ts, org[0]);
                } else {
                        ts = xwos_skd_get_timestamp_lc();
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }
        }

        flglogf(INFO, "[触发线程] 退出。\n");
        xwos_thd_delete(xwos_cthd_getd());
        return rc;
}
