/**
 * @file
 * @brief 示例：线程睡眠
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
#include <xwam/example/thread/sleep/mif.h>

#define LOGTAG "thdsleep"
#define thdslplogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWSLPDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t xwslpdemo_thd_func(void * arg);

/**
 * @brief 动态创建的线程描述表
 */
const struct xwos_thd_desc xwslpdemo_thd_desc[] = {
        [0] = {
                .name = "xwslpdemo.thd",
                .prio = XWSLPDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwslpdemo_thd_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd * xwslpdemo_thd[xw_array_size(xwslpdemo_thd_desc)];

/**
 * @brief 模块的加载函数
 */
xwer_t example_thread_sleep_start(void)
{
        xwer_t rc;
        xwsq_t i;


        for (i = 0; i < xw_array_size(xwslpdemo_thd_desc); i++) {
                rc = xwos_thd_create(&xwslpdemo_thd[i],
                                     xwslpdemo_thd_desc[i].name,
                                     xwslpdemo_thd_desc[i].func,
                                     xwslpdemo_thd_desc[i].arg,
                                     xwslpdemo_thd_desc[i].stack_size,
                                     xwslpdemo_thd_desc[i].prio,
                                     xwslpdemo_thd_desc[i].attr);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }

        return XWOK;

err_thd_create:
        return rc;
}

/**
 * @brief 线程的主函数
 */
xwer_t xwslpdemo_thd_func(void * arg)
{
        xwtm_t tk, ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        thdslplogf(INFO, "[线程] 启动。\n");

        thdslplogf(INFO, "[线程] 睡眠1秒 ...\n");
        tk = 1 * XWTM_S;
        xwos_cthd_sleep(&tk);

        tk = xwos_skd_get_timetick_lc();
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwos_cthd_sleep_from(&tk, 500 * XWTM_MS);
                ts = xwos_skd_get_timestamp_lc();
                thdslplogf(INFO, "[线程] 时间戳：%lld 纳秒。\n", ts);
        }

        thdslplogf(INFO, "[线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
