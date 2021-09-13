/**
 * @file
 * @brief 示例：互斥锁
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
#include <xwos/osal/lock/mtx.h>
#include <xwam/example/lock/mtx/mif.h>

#define LOGTAG "mtx"
#define mtxlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWMTXDEMO_THD_0_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
#define XWMTXDEMO_THD_1_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t xwmtxdemo_thd_0_func(void * arg);
xwer_t xwmtxdemo_thd_1_func(void * arg);

/**
 * @brief 动态创建的线程描述表
 */
const struct xwos_thd_desc xwmtxdemo_thd_desc[] = {
        [0] = {
                .name = "xwmtxdemo.thd.0",
                .prio = XWMTXDEMO_THD_0_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwmtxdemo_thd_0_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [1] = {
                .name = "xwmtxdemo.thd.1",
                .prio = XWMTXDEMO_THD_1_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwmtxdemo_thd_1_func,
                .arg = (void *)3,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd * xwmtxdemo_thd[xw_array_size(xwmtxdemo_thd_desc)];

struct xwos_mtx * xwmtxdemo_mtx;
xwsq_t xwmtxdemo_shared_count = 0;

/**
 * @brief 模块的加载函数
 */
xwer_t example_mtx_start(void)
{
        xwer_t rc;
        xwsq_t i;

        rc = xwos_mtx_create(&xwmtxdemo_mtx, XWOS_SKD_PRIORITY_RT_MAX);
        if (rc < 0) {
                goto err_mtx_create;
        }

        for (i = 0; i < xw_array_size(xwmtxdemo_thd_desc); i++) {
                rc = xwos_thd_create(&xwmtxdemo_thd[i],
                                     xwmtxdemo_thd_desc[i].name,
                                     xwmtxdemo_thd_desc[i].func,
                                     xwmtxdemo_thd_desc[i].arg,
                                     xwmtxdemo_thd_desc[i].stack_size,
                                     xwmtxdemo_thd_desc[i].prio,
                                     xwmtxdemo_thd_desc[i].attr);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }

        return XWOK;

err_thd_create:
        xwos_mtx_delete(xwmtxdemo_mtx);
err_mtx_create:
        return rc;
}

/**
 * @brief 线程0的主函数
 */
xwer_t xwmtxdemo_thd_0_func(void * arg)
{
        xwtm_t time;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        mtxlogf(INFO, "[线程0] 启动。\n");

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                time = 500 * XWTM_MS;
                xwos_cthd_sleep(&time);
                rc = xwos_mtx_lock(xwmtxdemo_mtx);
                if (XWOK == rc) {
                        time = xwos_skd_get_timestamp_lc();
                        mtxlogf(INFO,
                                "[线程0] 计数器：%d, 时间戳：%lld 纳秒。\n",
                                xwmtxdemo_shared_count, time);
                        xwmtxdemo_shared_count++;
                        xwos_mtx_unlock(xwmtxdemo_mtx);
                }
        }
        mtxlogf(INFO, "[线程0] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 线程1的主函数
 */
xwer_t xwmtxdemo_thd_1_func(void * arg)
{
        xwtm_t time;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        mtxlogf(INFO, "[线程1] 开始。\n");

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                time = 500 * XWTM_MS;
                xwos_cthd_sleep(&time);
                rc = xwos_mtx_lock(xwmtxdemo_mtx);
                if (XWOK == rc) {
                        time = xwos_skd_get_timestamp_lc();
                        mtxlogf(INFO,
                                "[线程1] 计数器：%d, 时间戳：%lld 纳秒。\n",
                                xwmtxdemo_shared_count, time);
                        xwmtxdemo_shared_count++;
                        xwos_mtx_unlock(xwmtxdemo_mtx);
                }
        }
        mtxlogf(INFO, "[线程1] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
