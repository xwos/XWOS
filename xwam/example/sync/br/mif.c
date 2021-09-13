/**
 * @file
 * @brief 示例：线程栅栏
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/skd.h>
#include <xwos/osal/sync/br.h>
#include <xwam/example/sync/br/mif.h>

#define LOGTAG "br"
#define brlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWBRDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t xwbrdemo_thd_func(void * arg);

const struct xwos_thd_desc xwbrdemo_thd_desc[] = {
        [0] = {
                .name = "example.br.thd.0",
                .prio = XWBRDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwbrdemo_thd_func,
                .arg = (void *)0, /* 线程在线程栅栏中的的位置 */
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [1] = {
                .name = "example.br.thd.1",
                .prio = XWBRDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwbrdemo_thd_func,
                .arg = (void *)1, /* 线程在线程栅栏中的位置 */
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [2] = {
                .name = "example.br.thd.2",
                .prio = XWBRDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwbrdemo_thd_func,
                .arg = (void *)2, /* 线程在线程栅栏中的位置 */
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [3] = {
                .name = "example.br.thd.3",
                .prio = XWBRDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwbrdemo_thd_func,
                .arg = (void *)3, /* 线程在线程栅栏中的位置 */
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [4] = {
                .name = "example.br.thd.4",
                .prio = XWBRDEMO_THD_PRIORITY,
                .stack = XWOS_THD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thd_f)xwbrdemo_thd_func,
                .arg = (void *)4, /* 线程在线程栅栏中的位置 */
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
struct xwos_thd * xwbrdemo_thd[xw_array_size(xwbrdemo_thd_desc)];

/**
 * @brief 静态初始化线程栅栏所需要的位图数组缓冲区
 */
xwbmpop_declare(xwbrdemo_br_bmp, xw_array_size(xwbrdemo_thd_desc)) = {0,};
xwbmpop_declare(xwbrdemo_br_msk, xw_array_size(xwbrdemo_thd_desc)) = {0,};

/**
 * @brief 静态定义的线程栅栏
 */
struct xwos_br xwbrdemo_br;

/**
 * @brief 测试模块的启动函数
 */
xwer_t example_br_start(void)
{
        xwsq_t i;
        xwer_t rc;

        /* 初始化线程屏障 */
        rc = xwos_br_init(&xwbrdemo_br, xw_array_size(xwbrdemo_thd_desc),
                          xwbrdemo_br_bmp, xwbrdemo_br_msk);
        if (rc < 0) {
                goto err_br_init;
        }

        /* 创建线程 */
        for (i = 0; i < xw_array_size(xwbrdemo_thd_desc); i++) {
                rc = xwos_thd_create(&xwbrdemo_thd[i],
                                     xwbrdemo_thd_desc[i].name,
                                     xwbrdemo_thd_desc[i].func,
                                     xwbrdemo_thd_desc[i].arg,
                                     xwbrdemo_thd_desc[i].stack_size,
                                     xwbrdemo_thd_desc[i].prio,
                                     xwbrdemo_thd_desc[i].attr);
                if (rc < 0) {
                        goto err_thd_create;
                }
        }
        return XWOK;

err_thd_create:
        xwos_br_fini(&xwbrdemo_br);
err_br_init:
        return rc;
}

/**
 * @brief 线程函数
 */
xwer_t xwbrdemo_thd_func(void * arg)
{
        xwer_t rc;
        xwsq_t pos = (xwsq_t)arg; /* 获取线程的各自的位置 */
        xwbmpop_declare(msk, xw_array_size(xwbrdemo_thd_desc));

        brlogf(INFO, "[线程%d] 启动。\n", pos);
        /* 设置需要同步的线程的位图掩码 */
        xwbmpop_c0all(msk, xw_array_size(xwbrdemo_thd_desc));
        for (xwsq_t i = 0; i < xw_array_size(xwbrdemo_thd_desc); i++) {
                xwbmpop_s1i(msk, i);
        }

        /* 与位图掩码中的线程进行同步，
           当这些线程都运行到此处时，同时解除阻塞，继续往下运行 */
        rc = xwos_br_sync(&xwbrdemo_br, pos, msk);
        if (XWOK == rc) {
                brlogf(INFO, "[线程%d] 同步。\n", pos);
        }
        brlogf(INFO, "[线程%d] 退出。\n", pos);
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
