/**
 * @file
 * @brief 示例：线程栅栏
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/sync/br.h>
#include <xwam/example/sync/br/mif.h>

#define LOGTAG "br"
#define brlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define BRDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t brdemo_thd_func(void * arg);

const struct xwos_thd_desc brdemo_thd_desc[] = {
        [0] = {
                .attr = {
                        .name = "example.br.thd.0",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = BRDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)brdemo_thd_func,
                .arg = (void *)0,
        },
        [1] = {
                .attr = {
                        .name = "example.br.thd.1",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = BRDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)brdemo_thd_func,
                .arg = (void *)1,
        },
        [2] = {
                .attr = {
                        .name = "example.br.thd.2",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = BRDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)brdemo_thd_func,
                .arg = (void *)2,
        },
        [3] = {
                .attr = {
                        .name = "example.br.thd.3",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = BRDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)brdemo_thd_func,
                .arg = (void *)3,
        },
        [4] = {
                .attr = {
                        .name = "example.br.thd.4",
                        .stack = NULL,
                        .stack_size = 2048,
                        .stack_guard_size = XWOS_STACK_GUARD_SIZE_DEFAULT,
                        .priority = BRDEMO_THD_PRIORITY,
                        .detached = false,
                        .privileged = true,
                },
                .func = (xwos_thd_f)brdemo_thd_func,
                .arg = (void *)4,
        },
};
xwos_thd_d brdemo_thd[xw_array_size(brdemo_thd_desc)];

/**
 * @brief 静态初始化线程栅栏所需要的位图数组缓冲区
 */
xwbmpop_define(brdemo_br_bmp, xw_array_size(brdemo_thd_desc)) = {0,};
xwbmpop_define(brdemo_br_msk, xw_array_size(brdemo_thd_desc)) = {0,};

/**
 * @brief 静态定义的线程栅栏
 */
struct xwos_br brdemo_br;

/**
 * @brief 测试模块的启动函数
 */
xwer_t xwos_example_br(void)
{
        xwsq_t i;
        xwer_t rc;

        /* 设置需要同步的线程的位图掩码 */
        /* 初始化线程屏障 */
        xwbmpop_c0all(brdemo_br_msk, xw_array_size(brdemo_thd_desc));
        rc = xwos_br_init(&brdemo_br, xw_array_size(brdemo_thd_desc),
                          brdemo_br_bmp, brdemo_br_msk);
        if (rc < 0) {
                goto err_br_init;
        }

        /* 创建线程 */
        for (i = 0; i < xw_array_size(brdemo_thd_desc); i++) {
                rc = xwos_thd_create(&brdemo_thd[i],
                                     &brdemo_thd_desc[i].attr,
                                     brdemo_thd_desc[i].func,
                                     brdemo_thd_desc[i].arg);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }
        return XWOK;

err_thd_create:
        xwos_br_fini(&brdemo_br);
err_br_init:
        return rc;
}

/**
 * @brief 线程函数
 */
xwer_t brdemo_thd_func(void * arg)
{
        xwer_t rc;

        brlogf(INFO, "[线程%d] 启动。\n", (xwsq_t)arg);
        /* 与位图掩码中的线程进行同步，
           当这些线程都运行到此处时，同时解除阻塞，继续往下运行 */
        rc = xwos_br_wait(&brdemo_br);
        if (XWOK == rc) {
                brlogf(INFO, "[线程%d] 开始运行。\n", (xwsq_t)arg);
        }
        brlogf(INFO, "[线程%d] 退出。\n", (xwsq_t)arg);
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
