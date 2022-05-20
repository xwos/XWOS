/**
 * @file
 * @brief 示例：线程睡眠
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
#include <xwam/example/thd/sleep/mif.h>

#define LOGTAG "thdsleep"
#define thdslplogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define SLEEPDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t sleepdemo_thd_func(void * arg);
xwos_thd_d sleepdemo_thd;

/**
 * @brief 模块的加载函数
 */
xwer_t xwos_example_thd_sleep(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        xwos_thd_attr_init(&attr);
        attr.name = "sleepdemo.thd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = SLEEPDEMO_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&sleepdemo_thd, &attr, sleepdemo_thd_func, NULL);
        return rc;
}

/**
 * @brief 线程的主函数
 */
xwer_t sleepdemo_thd_func(void * arg)
{
        xwtm_t ts;

        XWOS_UNUSED(arg);

        thdslplogf(INFO, "[线程] 启动。\n");
        thdslplogf(INFO, "[线程] 睡眠1秒 ...\n");
        xwos_cthd_sleep(XWTM_S(1));

        ts = xwtm_now();
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwos_cthd_sleep_from(&ts, XWTM_MS(500));
                ts = xwtm_nowts();
                thdslplogf(INFO, "[线程] 时间戳：%lld 纳秒。\n", ts);
        }
        thdslplogf(INFO, "[线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return XWOK;
}
