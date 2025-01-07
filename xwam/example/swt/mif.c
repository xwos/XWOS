/**
 * @file
 * @brief 示例：软件定时器
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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/swt.h>
#include <xwos/osal/sync/flg.h>
#include <xwam/example/swt/mif.h>

#define LOGTAG "swt"
#define swtlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define SWTDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)
xwer_t swtdemo_thd_func(void * arg);

xwos_thd_d swtdemo_thd;
struct xwos_swt swtdemo_swt0;
xwos_swt_d swtdemo_swt1_d;

#define SWTFLG_NUM      (sizeof(xwbmp_t) * BITS_PER_XWU8_T)
xwbmpop_define(swtflg_bmp, SWTFLG_NUM) = {0,};
xwbmpop_define(swtflg_msk, SWTFLG_NUM) = {0,};
struct xwos_flg swtflg;

/**
 * @brief 模块的加载函数
 */
xwer_t xwos_example_swt(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        /* 初始化事件信号旗 */
        rc = xwos_flg_init(&swtflg, SWTFLG_NUM, swtflg_bmp, swtflg_msk);
        if (rc < 0) {
                goto err_flg_init;
        }

        /* 初始化定时器0 */
        rc = xwos_swt_init(&swtdemo_swt0, XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt0_init;
        }

        /* 创建定时器1 */
        rc = xwos_swt_create(&swtdemo_swt1_d, XWOS_SWT_FLAG_RESTART);
        if (rc < 0) {
                goto err_swt1_create;
        }

        /* 创建线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "swtdemo.thd";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = SWTDEMO_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&swtdemo_thd, &attr, swtdemo_thd_func, NULL);
        if (rc < 0) {
                goto err_thd_create;
        }

        return XWOK;

err_thd_create:
        xwos_swt_delete(swtdemo_swt1_d);
err_swt1_create:
        xwos_swt_fini(&swtdemo_swt0);
err_swt0_init:
        xwos_flg_fini(&swtflg);
err_flg_init:
        return rc;
}

/**
 * @brief 定时器0的回调函数
 * @note
 * + 当配置(XWOSCFG_SKD_BH == 1)，此函数运行在中断底半部；
 * + 当配置(XWOSCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * + 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void swtdemo_swt0_callback(struct xwos_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_flg_s1i(&swtflg, 0);
}

/**
 * @brief 定时器1的回调函数
 * @note
 * + 当配置(XWOSCFG_SKD_BH == 1)，此函数运行在中断底半部；
 * + 当配置(XWOSCFG_SKD_BH == 0)，此函数运行在中断上下文；
 * + 此函数中不可调用会导致线程睡眠或阻塞的函数。
 */
void swtdemo_swt1_callback(struct xwos_swt * swt, void * arg)
{
        XWOS_UNUSED(swt);
        XWOS_UNUSED(arg);

        xwos_flg_s1i(&swtflg, 1);
}

/**
 * @brief 线程1的主函数
 */
xwer_t swtdemo_thd_func(void * arg)
{
        xwtm_t ts;
        xwer_t rc;
        xwbmpop_define(msk, SWTFLG_NUM) = {0,};
        xwbmpop_define(trg, SWTFLG_NUM) = {0,};

        XWOS_UNUSED(arg);

        swtlogf(INFO, "[线程] 启动。\n");
        swtlogf(INFO, "[线程] 启动定时器0。\n");
        ts = xwtm_now();
        rc = xwos_swt_start(&swtdemo_swt0,
                            ts, XWTM_MS(500),
                            swtdemo_swt0_callback, NULL);

        swtlogf(INFO, "[线程] 启动定时器1。\n");
        rc = xwos_swt_start(swtdemo_swt1_d.swt,
                            ts, XWTM_MS(800),
                            swtdemo_swt1_callback, NULL);

        /* 设置掩码位为bit0:1共2位 */
        memset(msk, 0, sizeof(msk));
        msk[0] = XWBOP_BIT(0) | XWBOP_BIT(1);
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                /* 等待事件 */
                rc = xwos_flg_wait(&swtflg,
                                   XWOS_FLG_TRIGGER_SET_ANY,
                                   true,
                                   trg, msk);
                if (XWOK == rc) {
                        /* 测试第0位是否被置1 */
                        if (xwbmpop_t1i(trg, 0)) {
                                ts = xwtm_nowts();
                                swtlogf(INFO,
                                        "[线程] 定时器0唤醒，时间戳：%lld 纳秒。\n",
                                        ts);
                        }

                        /* 测试第1位是否被置1 */
                        if (xwbmpop_t1i(trg, 1)) {
                                ts = xwtm_nowts();
                                swtlogf(INFO,
                                        "[线程] 定时器1唤醒，时间戳：%lld 纳秒。\n",
                                        ts);
                        }
                } else {
                        ts = xwtm_nowts();
                        swtlogf(ERR,
                                "[线程] 错误，时间戳：%lld 纳秒，错误码：%d。\n",
                                ts, rc);
                }
        }

        swtlogf(INFO, "[线程] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
