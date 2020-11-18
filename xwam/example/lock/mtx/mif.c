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

#define XWMTXDEMO_THRD_0_PRIORITY                               \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)

#define XWMTXDEMO_THRD_1_PRIORITY                               \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
#define XWMTXDEMO_LOG_TAG             "mtx"
#define mtxlogf(lv, fmt, ...)                                   \
        xwlogf(lv, XWMTXDEMO_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
#define mtxlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

xwer_t xwmtxdemo_thrd_0_func(void * arg);
xwer_t xwmtxdemo_thrd_1_func(void * arg);

/**
 * @brief 动态创建的线程描述表
 */
const struct xwos_thrd_desc xwmtxdemo_tbd[] = {
        [0] = {
                .name = "xwmtxdemo.thrd.0",
                .prio = XWMTXDEMO_THRD_0_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwmtxdemo_thrd_0_func,
                .arg = NULL,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [1] = {
                .name = "xwmtxdemo.thrd.1",
                .prio = XWMTXDEMO_THRD_1_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwmtxdemo_thrd_1_func,
                .arg = (void *)3,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
xwid_t xwmtxdemo_tid[xw_array_size(xwmtxdemo_tbd)];

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

        for (i = 0; i < xw_array_size(xwmtxdemo_tbd); i++) {
                rc = xwos_thrd_create(&xwmtxdemo_tid[i],
                                      xwmtxdemo_tbd[i].name,
                                      xwmtxdemo_tbd[i].func,
                                      xwmtxdemo_tbd[i].arg,
                                      xwmtxdemo_tbd[i].stack_size,
                                      xwmtxdemo_tbd[i].prio,
                                      xwmtxdemo_tbd[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return XWOK;

err_thrd_create:
        xwos_mtx_delete(xwmtxdemo_mtx);
err_mtx_create:
        return rc;
}

/**
 * @brief 线程0的主函数
 */
xwer_t xwmtxdemo_thrd_0_func(void * arg)
{
        xwtm_t time;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        mtxlogf(INFO, "[线程0] 启动。\n");

        while (!xwos_cthrd_frz_shld_stop(NULL)) {
                time = 500 * XWTM_MS;
                xwos_cthrd_sleep(&time);
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
        xwos_thrd_delete(xwos_cthrd_id());
        return rc;
}

/**
 * @brief 线程1的主函数
 */
xwer_t xwmtxdemo_thrd_1_func(void * arg)
{
        xwtm_t time;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        mtxlogf(INFO, "[线程1] 开始。\n");

        while (!xwos_cthrd_frz_shld_stop(NULL)) {
                time = 500 * XWTM_MS;
                xwos_cthrd_sleep(&time);
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
        xwos_thrd_delete(xwos_cthrd_id());
        return rc;
}
