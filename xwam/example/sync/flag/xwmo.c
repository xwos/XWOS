/**
 * @file
 * @brief 示例：sync/flag
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
#include <xwos/osal/sync/flag.h>
#include <xwam/example/sync/flag/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define EXAMPLE_FLAG_WTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

#define EXAMPLE_FLAG_ITHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
  #define EXAMPLE_FLAG_LOG_TAG          "flag"
  #define flglogf(lv, fmt, ...) \
        xwlogf(lv, EXAMPLE_FLAG_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
  #define flglogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t example_flag_wthrd_func(void * arg);
xwer_t example_flag_ithrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_flg xwtst_sync_flag;

const struct xwosal_thrd_desc example_flag_wthrd_td = {
        .name = "example.flag.wthrd",
        .prio = EXAMPLE_FLAG_WTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 4096,
        .func = (xwosal_thrd_f)example_flag_wthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t example_flag_wthrd;

const struct xwosal_thrd_desc example_flag_ithrd_td = {
        .name = "example.flag.ithrd",
        .prio = EXAMPLE_FLAG_ITHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 4096,
        .func = (xwosal_thrd_f)example_flag_ithrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t example_flag_ithrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 测试模块的启动函数
 */
xwer_t example_flag_start(void)
{
        xwer_t rc;

        /* 初始化事件信号旗 */
        rc = xwosal_flg_init(&xwtst_sync_flag, NULL);
        if (rc < 0) {
                goto err_flg_init;
        }

        /* 创建等待事件信号旗的线程 */
        rc = xwosal_thrd_create(&example_flag_wthrd,
                                example_flag_wthrd_td.name,
                                example_flag_wthrd_td.func,
                                example_flag_wthrd_td.arg,
                                example_flag_wthrd_td.stack_size,
                                example_flag_wthrd_td.prio,
                                example_flag_wthrd_td.attr);
        if (rc < 0) {
                goto err_wthrd_create;
        }

        /* 创建发布事件信号旗的线程 */
        rc = xwosal_thrd_create(&example_flag_ithrd,
                                example_flag_ithrd_td.name,
                                example_flag_ithrd_td.func,
                                example_flag_ithrd_td.arg,
                                example_flag_ithrd_td.stack_size,
                                example_flag_ithrd_td.prio,
                                example_flag_ithrd_td.attr);
        if (rc < 0) {
                goto err_ithrd_create;
        }

        return XWOK;


err_ithrd_create:
        xwosal_thrd_terminate(example_flag_wthrd, NULL);
        xwosal_thrd_delete(example_flag_wthrd);
err_wthrd_create:
        xwosal_flg_destroy(&xwtst_sync_flag);
err_flg_init:
        return rc;
}

/**
 * @brief 等待事件信号旗的线程
 */
xwer_t example_flag_wthrd_func(void * arg)
{
        xwosal_flg_declare_bitmap(msk);
        xwosal_flg_declare_bitmap(org);
        xwosal_flg_declare_bitmap(tgevt);
        xwid_t flgid;
        xwtm_t ts;
        xwer_t rc;

        XWOS_UNUSED(arg);

        flglogf(INFO, "[等待线程] 启动。\n");

        flgid = xwosal_flg_get_id(&xwtst_sync_flag); /**< 获取事件信号旗的ID */
        memset(msk, 0, sizeof(msk));

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 等待事件：0x%X，"
                        "触发条件：SET_ALL，是否消费事件：是...\n",
                        msk[0]);
                rc = xwosal_flg_wait(flgid,
                                     XWOSAL_FLG_TRIGGER_SET_ALL,
                                     XWOSAL_FLG_ACTION_CONSUMPTION,
                                     tgevt, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                /* 休眠100ms，让出CPU使用权 */
                ts = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 等待事件：0x%X，"
                        "触发条件：SET_ANY，是否消费事件：否...\n",
                        msk[0]);
                rc = xwosal_flg_wait(flgid,
                                     XWOSAL_FLG_TRIGGER_SET_ANY,
                                     XWOSAL_FLG_ACTION_NONE,
                                     tgevt, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                /* 休眠100ms，让出CPU使用权 */
                ts = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 等待事件：0x%X，"
                        "触发条件：CLR_ALL，是否消费事件：是...\n",
                        msk[0]);
                rc = xwosal_flg_wait(flgid,
                                     XWOSAL_FLG_TRIGGER_CLR_ALL,
                                     XWOSAL_FLG_ACTION_CONSUMPTION,
                                     tgevt, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                /* 休眠100ms，让出CPU使用权 */
                ts = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 等待事件：0x%X，"
                        "触发条件：CLR_ANY，是否消费事件：否...\n",
                        msk[0]);
                rc = xwosal_flg_wait(flgid,
                                     XWOSAL_FLG_TRIGGER_CLR_ANY,
                                     XWOSAL_FLG_ACTION_NONE,
                                     tgevt, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，"
                                "触发事件：0x%X, 当前事件：0x%X。\n\n",
                                ts, tgevt[0], org[0]);
                } else {
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                /* 休眠100ms，让出CPU使用权 */
                ts = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                rc = xwosal_flg_read(flgid, org); /* 读取初始值 */
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 等待事件：0x%X，"
                        "触发条件：TGL_ALL，初始值：0x%X...\n",
                        msk[0], org[0]);
                rc = xwosal_flg_wait(flgid,
                                     XWOSAL_FLG_TRIGGER_TGL_ALL,
                                     XWOS_UNUSED_ARGUMENT,
                                     org, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，事件：0x%X。\n\n",
                                ts, org[0]);
                } else {
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                /* 休眠100ms，让出CPU使用权 */
                ts = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                flglogf(INFO,
                        "[等待线程] 等待事件：0x%X，"
                        "触发条件：TGL_ANY，初始值：0x%X...\n",
                        msk[0], org[0]);
                rc = xwosal_flg_wait(flgid,
                                     XWOSAL_FLG_TRIGGER_TGL_ANY,
                                     XWOS_UNUSED_ARGUMENT,
                                     org, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        flglogf(INFO,
                                "[等待线程] 唤醒，时间戳：%lld 纳秒，事件：0x%X。\n\n",
                                ts, org[0]);
                } else {
                        flglogf(ERR,
                                "[等待线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n\n",
                                ts, rc);
                }

                /* 休眠100ms，让出CPU使用权 */
                ts = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);
        }

        flglogf(INFO, "[等待线程] 退出。\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}

/**
 * @brief 触发事件的线程
 */
xwer_t example_flag_ithrd_func(void * arg)
{
        xwosal_flg_declare_bitmap(org);
        xwosal_flg_declare_bitmap(msk);
        xwid_t flgid;
        xwtm_t ts;
        xwer_t rc;

        XWOS_UNUSED(arg);

        flglogf(INFO, "[触发线程] 启动。\n");
        flgid = xwosal_flg_get_id(&xwtst_sync_flag); /**< 获取事件信号旗的ID */
        memset(msk, 0, sizeof(msk));

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部置1 */
                rc = xwosal_flg_s1m(flgid, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "设置事件：掩码0x%X，当前事件：0x%X。\n",
                                ts, msk[0], org[0]);
                } else {
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                /* 将事件位图的第3位置1 */
                rc = xwosal_flg_s1i(flgid, 3);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "设置事件：第3位，当前事件：0x%X。\n",
                                ts, org[0]);
                } else {
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部清0 */
                rc = xwosal_flg_c0m(flgid, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "清零事件：掩码0x%X，当前事件：0x%X。\n",
                                ts, msk[0], org[0]);
                } else {
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                /* 将事件位图的第7位清0 */
                rc = xwosal_flg_c0i(flgid, 7);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "清零事件：第7位，当前事件：0x%X。\n",
                                ts, org[0]);
                } else {
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                msk[0] = 0xFF; /* 设置事件位的掩码bit0~bit7共8位 */
                /* 将事件位图掩码部分全部翻转 */
                rc = xwosal_flg_x1m(flgid, msk);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "翻转事件：掩码0x%X，当前事件：0x%X。\n",
                                ts, msk[0], org[0]);
                } else {
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);

                 /* 将事件位图的第5位翻转 */
                rc = xwosal_flg_x1i(flgid, 5);
                if (XWOK == rc) {
                        ts = xwosal_scheduler_get_timestamp_lc();
                        xwosal_flg_read(flgid, org);
                        flglogf(INFO,
                                "[触发线程] 时间戳：%lld 纳秒，"
                                "翻转事件：第5位，当前事件：0x%X。\n",
                                ts, org[0]);
                } else {
                        flglogf(ERR,
                                "[触发线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }

                /* 休眠1000ms，让出CPU使用权 */
                ts = 1000 * XWTM_MS;
                xwosal_cthrd_sleep(&ts);
        }

        flglogf(INFO, "[触发线程] 退出。\n");
        xwosal_thrd_delete(xwosal_cthrd_get_id());
        return rc;
}
