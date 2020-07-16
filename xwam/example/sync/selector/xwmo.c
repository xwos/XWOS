/**
 * @file
 * @brief 示例：sync/selector
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
#include <xwos/osal/scheduler.h>
#include <xwos/osal/thread.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwos/osal/sync/condition.h>
#include <xwos/osal/sync/selector.h>
#include <xwam/example/sync/selector/xwmo.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define EXAMPLE_SELECTOR_WTHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 1)

#define EXAMPLE_SELECTOR_ITHRD_PRIORITY \
        XWOSAL_SD_PRIORITY_DROP(XWOSAL_SD_PRIORITY_RT_MAX, 2)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t example_selector_wthrd_func(void * arg);
xwer_t example_selector_ithrd_func(void * arg);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwosal_selector xwtst_sync_slt0;
struct xwosal_smr xwtst_sync_smr1;
struct xwosal_smr xwtst_sync_smr2;
struct xwosal_smr xwtst_sync_smr3;
struct xwosal_selector xwtst_sync_slt4;
struct xwosal_cdt xwtst_sync_cdt5;
struct xwosal_cdt xwtst_sync_cdt6;

const struct xwosal_thrd_desc example_selector_wthrd_td = {
        .name = "example.selector.wthrd",
        .prio = EXAMPLE_SELECTOR_WTHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)example_selector_wthrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t example_selector_wthrd;

const struct xwosal_thrd_desc example_selector_ithrd_td = {
        .name = "example.selector.ithrd",
        .prio = EXAMPLE_SELECTOR_ITHRD_PRIORITY,
        .stack = XWOSAL_THRD_STACK_DYNAMIC,
        .stack_size = 2048,
        .func = (xwosal_thrd_f)example_selector_ithrd_func,
        .arg = NULL,
        .attr = XWSDOBJ_ATTR_PRIVILEGED,
};
xwid_t example_selector_ithrd;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 测试模块的启动函数
 */
xwer_t example_selector_start(void)
{
        xwid_t slt0id, slt4id;
        xwid_t smr1id, smr2id, smr3id;
        xwid_t cdt5id, cdt6id;
        xwer_t rc;

        /* 初始化信号选择器0 */
        rc = xwosal_selector_init(&xwtst_sync_slt0);
        if (rc < 0) {
                goto err_slt0_init;
        }
        /* 初始化信号量1 */
        rc = xwosal_smr_init(&xwtst_sync_smr1, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr1_init;
        }
        /* 初始化信号量2 */
        rc = xwosal_smr_init(&xwtst_sync_smr2, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr2_init;
        }
        /* 初始化信号量3 */
        rc = xwosal_smr_init(&xwtst_sync_smr3, 0, XWSSQ_MAX);
        if (rc < 0) {
                goto err_smr3_init;
        }
        /* 初始化信号选择器4 */
        rc = xwosal_selector_init(&xwtst_sync_slt4);
        if (rc < 0) {
                goto err_slt4_init;
        }
        /* 初始化条件量5 */
        rc = xwosal_cdt_init(&xwtst_sync_cdt5);
        if (rc < 0) {
                goto err_cdt5_init;
        }
        /* 初始化条件量6 */
        rc = xwosal_cdt_init(&xwtst_sync_cdt6);
        if (rc < 0) {
                goto err_cdt6_init;
        }

        /* 获取各同步对象的ID */
        slt0id = xwosal_selector_get_id(&xwtst_sync_slt0);
        smr1id = xwosal_smr_get_id(&xwtst_sync_smr1);
        smr2id = xwosal_smr_get_id(&xwtst_sync_smr2);
        smr3id = xwosal_smr_get_id(&xwtst_sync_smr3);
        slt4id = xwosal_selector_get_id(&xwtst_sync_slt4);
        cdt5id = xwosal_cdt_get_id(&xwtst_sync_cdt5);
        cdt6id = xwosal_cdt_get_id(&xwtst_sync_cdt6);

        /* 将信号量1绑定到信号选择器0上，位图中的位置为1 */
        rc = xwosal_smr_bind(smr1id, slt0id, 1);
        if (rc < 0) {
                goto err_smr1_bind;
        }
        /* 将信号量2绑定到信号选择器0上，位图中的位置为2 */
        rc = xwosal_smr_bind(smr2id, slt0id, 2);
        if (rc < 0) {
                goto err_smr2_bind;
        }
        /* 将信号量3绑定到信号选择器0上，位图中的位置为3 */
        rc = xwosal_smr_bind(smr3id, slt0id, 3);
        if (rc < 0) {
                goto err_smr3_bind;
        }

        /* 将信号选择器4绑定到信号选择器0上，位图中的位置为4 */
        rc = xwosal_selector_bind(slt4id, slt0id, 4);
        if (rc < 0) {
                goto err_slt4_bind;
        }

        /* 将条件量5绑定到信号选择器4上，位图中的位置为5 */
        rc = xwosal_cdt_bind(cdt5id, slt4id, 5);
        if (rc < 0) {
                goto err_cdt5_bind;
        }
        /* 将条件量6绑定到信号选择器4上，位图中的位置为6 */
        rc = xwosal_cdt_bind(cdt6id, slt4id, 6);
        if (rc < 0) {
                goto err_cdt6_bind;
        }

        /* 创建等待同步对象的线程 */
        rc = xwosal_thrd_create(&example_selector_wthrd,
                                example_selector_wthrd_td.name,
                                example_selector_wthrd_td.func,
                                example_selector_wthrd_td.arg,
                                example_selector_wthrd_td.stack_size,
                                example_selector_wthrd_td.prio,
                                example_selector_wthrd_td.attr);
        if (rc < 0) {
                goto err_wthrd_create;
        }

        /* 建立发布同步对象的线程 */
        rc = xwosal_thrd_create(&example_selector_ithrd,
                                example_selector_ithrd_td.name,
                                example_selector_ithrd_td.func,
                                example_selector_ithrd_td.arg,
                                example_selector_ithrd_td.stack_size,
                                example_selector_ithrd_td.prio,
                                example_selector_ithrd_td.attr);
        if (rc < 0) {
                goto err_ithrd_create;
        }

        return XWOK;


err_ithrd_create:
        xwosal_thrd_terminate(example_selector_wthrd, NULL);
        xwosal_thrd_delete(example_selector_wthrd);
err_wthrd_create:
        xwosal_cdt_unbind(cdt6id, slt4id);
err_cdt6_bind:
        xwosal_cdt_unbind(cdt5id, slt4id);
err_cdt5_bind:
        xwosal_selector_unbind(slt4id, slt0id);
err_slt4_bind:
        xwosal_smr_unbind(smr3id, slt0id);
err_smr3_bind:
        xwosal_smr_unbind(smr2id, slt0id);
err_smr2_bind:
        xwosal_smr_unbind(smr1id, slt0id);
err_smr1_bind:
        xwosal_cdt_destroy(&xwtst_sync_cdt6);
err_cdt6_init:
        xwosal_cdt_destroy(&xwtst_sync_cdt5);
err_cdt5_init:
        xwosal_selector_destroy(&xwtst_sync_slt4);
err_slt4_init:
        xwosal_smr_destroy(&xwtst_sync_smr3);
err_smr3_init:
        xwosal_smr_destroy(&xwtst_sync_smr2);
err_smr2_init:
        xwosal_smr_destroy(&xwtst_sync_smr1);
err_smr1_init:
        xwosal_selector_destroy(&xwtst_sync_slt0);
err_slt0_init:
        return rc;
}

/**
 * @brief 等待同步对象的线程
 */
xwer_t example_selector_wthrd_func(void * arg)
{
        xwosal_selector_declare_bitmap(msk);
        xwosal_selector_declare_bitmap(trg);
        xwid_t slt0id, slt4id;
        xwid_t smr1id, smr2id, smr3id;
        xwtm_t sleep;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        /* 获取各同步对象的ID */
        slt0id = xwosal_selector_get_id(&xwtst_sync_slt0);
        smr1id = xwosal_smr_get_id(&xwtst_sync_smr1);
        smr2id = xwosal_smr_get_id(&xwtst_sync_smr2);
        smr3id = xwosal_smr_get_id(&xwtst_sync_smr3);
        slt4id = xwosal_selector_get_id(&xwtst_sync_slt4);

        while (!xwosal_cthrd_frz_shld_stop(NULL)) {
                /* 设置掩码位为bit1 ~ bit6共6位 */
                msk[0] = BIT(1) | BIT(2) | BIT(3) | BIT(4) | BIT(5) | BIT(6);
                /* 清0触发状态位图，trg用于返回触发结果 */
                xwbmpop_c0all(trg, XWOSAL_SELECTOR_MAXNUM);

                /* 通过信号选择器同时等待bit1 ~ bit6共6个同步对象 */
                rc = xwosal_selector_select(slt0id, msk, trg);
                if (XWOK == rc) {
                        /* 等待成功 */

                        /* 测试第1位是否被置1 */
                        if (xwbmpop_t1i(trg, 1)) {
                                rc = xwosal_smr_trywait(smr1id);
                        }

                        /* 测试第2位是否被置1 */
                        if (xwbmpop_t1i(trg, 2)) {
                                rc = xwosal_smr_trywait(smr2id);
                        }

                        /* 测试第3位是否被置1 */
                        if (xwbmpop_t1i(trg, 3)) {
                                rc = xwosal_smr_trywait(smr3id);
                        }

                        /* 测试第4位是否被置1 */
                        if (xwbmpop_t1i(trg, 4)) {
                                /* 第4位为子信号选择器4，再次测试（不等待） */
                                rc = xwosal_selector_tryselect(slt4id, msk, trg);
                                if (XWOK == rc) {
                                        /* 测试成功 */
                                        /* 测试第5位是否触发 */
                                        if (xwbmpop_t1i(trg, 5)) {
                                                /* 条件量5被广播 */
                                        }
                                        /* 测试第6位是否触发 */
                                        if (xwbmpop_t1i(trg, 6)) {
                                                /* 条件量6被广播 */
                                        }
                                }
                        }
                }
                sleep = 100 * XWTM_MS;
                xwosal_cthrd_sleep(&sleep);
        }
        return rc;
}

/**
 * @brief 发布同步对象的线程
 */
xwer_t example_selector_ithrd_func(void * arg)
{
        xwid_t smr1id, smr2id, smr3id;
        xwid_t cdt5id, cdt6id;
        xwtm_t sleep;
        xwer_t rc = XWOK;

        XWOS_UNUSED(arg);

        /* 获取各同步对象的ID */
        smr1id = xwosal_smr_get_id(&xwtst_sync_smr1);
        smr2id = xwosal_smr_get_id(&xwtst_sync_smr2);
        smr3id = xwosal_smr_get_id(&xwtst_sync_smr3);
        cdt5id = xwosal_cdt_get_id(&xwtst_sync_cdt5);
        cdt6id = xwosal_cdt_get_id(&xwtst_sync_cdt6);

        /* 发布信号量1，将信号选择器0的位1被置1 */
        rc = xwosal_smr_post(smr1id);
        if (rc < 0) {
                goto err_post_smr1;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        /* 发布信号量2，将信号选择器0的位2被置1 */
        rc = xwosal_smr_post(smr2id);
        if (rc < 0) {
                goto err_post_smr2;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        /* 发布信号量3，将信号选择器0的位3被置1 */
        rc = xwosal_smr_post(smr3id);
        if (rc < 0) {
                goto err_post_smr3;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        /* 广播条件量5，将信号选择器4的位5被置1 */
        /* 由于信号选择器4又绑定到信号选择器0的第4位，又会将信号选择器0的位4置1 */
        rc = xwosal_cdt_broadcast(cdt5id);
        if (rc < 0) {
                goto err_bdc_cdt5id;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

        /* 广播条件量6，将信号选择器4的位6被置1 */
        /* 由于信号选择器4又绑定到信号选择器0的第4位，又会将信号选择器0的位4置1 */
        rc = xwosal_cdt_broadcast(cdt6id);
        if (rc < 0) {
                goto err_bdc_cdt6id;
        }
        sleep = 1000 * XWTM_MS;
        xwosal_cthrd_sleep(&sleep);

err_bdc_cdt6id:
err_bdc_cdt5id:
err_post_smr3:
err_post_smr2:
err_post_smr1:
        return rc;
}
