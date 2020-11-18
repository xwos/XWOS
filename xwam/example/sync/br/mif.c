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

#define XWBRDEMO_THRD_PRIORITY                                  \
        XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

#if defined(XWLIBCFG_LOG) && (1 == XWLIBCFG_LOG)
#define XWBRDEMO_LOG_TAG      "br"
#define brlogf(lv, fmt, ...)                                    \
        xwlogf(lv, XWBRDEMO_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWLIBCFG_LOG */
#define brlogf(lv, fmt, ...)
#endif /* !XWLIBCFG_LOG */

xwer_t xwbrdemo_thrd_func(void * arg);

const struct xwos_thrd_desc xwbrdemo_td[] = {
        [0] = {
                .name = "example.br.thrd.0",
                .prio = XWBRDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwbrdemo_thrd_func,
                .arg = (void *)0,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [1] = {
                .name = "example.br.thrd.1",
                .prio = XWBRDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwbrdemo_thrd_func,
                .arg = (void *)1,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [2] = {
                .name = "example.br.thrd.2",
                .prio = XWBRDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwbrdemo_thrd_func,
                .arg = (void *)2,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [3] = {
                .name = "example.br.thrd.3",
                .prio = XWBRDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwbrdemo_thrd_func,
                .arg = (void *)3,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
        [4] = {
                .name = "example.br.thrd.4",
                .prio = XWBRDEMO_THRD_PRIORITY,
                .stack = XWOS_THRD_STACK_DYNAMIC,
                .stack_size = 2048,
                .func = (xwos_thrd_f)xwbrdemo_thrd_func,
                .arg = (void *)4,
                .attr = XWOS_SKDATTR_PRIVILEGED,
        },
};
xwid_t xwbrdemo_tid[xw_array_size(xwbrdemo_td)];

struct xwos_br xwbrdemo_br;

/**
 * @brief 测试模块的启动函数
 */
xwer_t example_br_start(void)
{
        xwsq_t i;
        xwer_t rc;

        /* 初始化线程屏障 */
        rc = xwos_br_init(&xwbrdemo_br);
        if (rc < 0) {
                goto err_br_init;
        }

        /* 创建5个线程 */
        for (i = 0; i < xw_array_size(xwbrdemo_td); i++) {
                rc = xwos_thrd_create(&xwbrdemo_tid[i],
                                      xwbrdemo_td[i].name,
                                      xwbrdemo_td[i].func,
                                      xwbrdemo_td[i].arg,
                                      xwbrdemo_td[i].stack_size,
                                      xwbrdemo_td[i].prio,
                                      xwbrdemo_td[i].attr);
                if (rc < 0) {
                        goto err_thrd_create;
                }
        }

        return XWOK;

err_thrd_create:
        xwos_br_destroy(&xwbrdemo_br);
err_br_init:
        return rc;
}

/**
 * @brief 线程函数
 */
xwer_t xwbrdemo_thrd_func(void * arg)
{
        xwos_br_declare_bitmap(msk);
        xwer_t rc;

        xwsq_t pos = (xwsq_t)arg; /* 获取线程的各自的位置 */

        brlogf(INFO, "[线程%d] 启动。\n", pos);

        xwbmpop_c0all(msk, XWOS_BR_MAXNUM);

        /* 设置位图掩码 */
        for (xwsq_t i = 0; i < xw_array_size(xwbrdemo_td); i++) {
                xwbmpop_s1i(msk, i);
        }

        /* 同步线程 */
        rc = xwos_br_sync(&xwbrdemo_br, pos, msk);
        if (XWOK == rc) {
                brlogf(INFO, "[线程%d] 同步。\n", pos);
        }

        brlogf(INFO, "[线程%d] 退出。\n", pos);
        xwos_thrd_delete(xwos_cthrd_id());
        return rc;
}
