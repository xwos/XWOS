/**
 * @file
 * @brief 示例：互斥锁
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
#include <xwos/osal/lock/mtx.h>
#include <xwam/example/lock/mtx/mif.h>

#define LOGTAG "mtx"
#define mtxlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define MTXDEMO_THD_0_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 0)
#define MTXDEMO_THD_1_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t mtxdemo_thd_0_func(void * arg);
xwer_t mtxdemo_thd_1_func(void * arg);

/**
 * @brief 线程描述表
 */
const struct xwos_thd_desc mtxdemo_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "mtxdemo.thd.0",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = MTXDEMO_THD_0_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)mtxdemo_thd_0_func,
                .arg = NULL,
        },
        [1] = {
                .attr = {
                        .name = "mtxdemo.thd.1",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = MTXDEMO_THD_1_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)mtxdemo_thd_1_func,
                .arg = (void *)3,
        },
};
xwos_thd_d mtxdemo_thd[xw_array_size(mtxdemo_thd_desc)];

xwos_mtx_d dymtx;
xwsq_t mtxdemo_shared_count = 0;

/**
 * @brief 模块的加载函数
 */
xwer_t xwos_example_mtx(void)
{
        xwer_t rc;
        xwsq_t i;

        rc = xwos_mtx_create(&dymtx, XWOS_SKD_PRIORITY_RT_MAX);
        if (rc < 0) {
                goto err_mtx_create;
        }

        for (i = 0; i < xw_array_size(mtxdemo_thd_desc); i++) {
                rc = xwos_thd_create(&mtxdemo_thd[i],
                                     &mtxdemo_thd_desc[i].attr,
                                     mtxdemo_thd_desc[i].func,
                                     mtxdemo_thd_desc[i].arg);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }

        return XWOK;

err_thd_create:
        xwos_mtx_delete(dymtx);
err_mtx_create:
        return rc;
}

/**
 * @brief 线程0的主函数
 */
xwer_t mtxdemo_thd_0_func(void * arg)
{
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        mtxlogf(INFO, "[线程0] 启动。\n");

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwos_cthd_sleep(XWTM_MS(500));
                rc = xwos_mtx_lock(dymtx.mtx);
                if (XWOK == rc) {
                        ts = xwtm_nowts();
                        mtxlogf(INFO,
                                "[线程0] 计数器：%d, 时间戳：%lld 纳秒。\n",
                                mtxdemo_shared_count, ts);
                        mtxdemo_shared_count++;
                        xwos_mtx_unlock(dymtx.mtx);
                }
        }
        mtxlogf(INFO, "[线程0] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 线程1的主函数
 */
xwer_t mtxdemo_thd_1_func(void * arg)
{
        xwtm_t ts;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);
        mtxlogf(INFO, "[线程1] 开始。\n");

        while (!xwos_cthd_frz_shld_stop(NULL)) {
                xwos_cthd_sleep(XWTM_MS(500));
                rc = xwos_mtx_lock(dymtx.mtx);
                if (XWOK == rc) {
                        ts = xwtm_nowts();
                        mtxlogf(INFO,
                                "[线程1] 计数器：%d, 时间戳：%lld 纳秒。\n",
                                mtxdemo_shared_count, ts);
                        mtxdemo_shared_count++;
                        xwos_mtx_unlock(dymtx.mtx);
                }
        }
        mtxlogf(INFO, "[线程1] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
