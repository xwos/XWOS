/**
 * @file
 * @brief 示例：循环队列
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
#include <xwos/lib/xwlog.h>
#include <xwos/osal/thd.h>
#include <xwmd/isc/xwcq/mif.h>
#include <xwam/example/isc/xwcq/mif.h>

#define LOGTAG "xwcq"
#define cqlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWCQDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

xwer_t xwcqdemo_producer_main(void * arg);
xwos_thd_d xwcqdemo_producer;

xwer_t xwcqdemo_consumer_main(void * arg);
xwos_thd_d xwcqdemo_consumer;

#define XWCQDEMO_SLOT_SIZE      64
#define XWCQDEMO_SLOT_NUM       8

XWCQ_DEF_MEMPOOL(xwcqdemo_cq_mempool, XWCQDEMO_SLOT_SIZE, XWCQDEMO_SLOT_NUM);
struct xwcq xwcqdemo_cq;

/**
 * @brief 模块的加载函数
 */
xwer_t xwmd_example_xwcq(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        rc = xwcq_init(&xwcqdemo_cq, XWCQDEMO_SLOT_SIZE, XWCQDEMO_SLOT_NUM,
                       xwcqdemo_cq_mempool);
        if (rc < 0) {
                goto err_xwcq_init;
        }

        /* 创建线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "demo.xwcq.producer";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = XWCQDEMO_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&xwcqdemo_producer, &attr, xwcqdemo_producer_main,
                             &xwcqdemo_cq);
        if (rc < 0) {
                goto err_producer_create;
        }

        xwos_thd_attr_init(&attr);
        attr.name = "demo.xwcq.consumer";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = XWCQDEMO_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&xwcqdemo_consumer, &attr, xwcqdemo_consumer_main,
                             &xwcqdemo_cq);
        if (rc < 0) {
                goto err_consumer_create;
        }

        return XWOK;

err_consumer_create:
        xwos_thd_stop(xwcqdemo_producer, NULL);
err_producer_create:
        xwcq_fini(&xwcqdemo_cq);
err_xwcq_init:
        return rc;
}

/**
 * @brief 生产者线程的主函数
 */
xwer_t xwcqdemo_producer_main(void * arg)
{
        xwer_t rc;
        xwsz_t size;
        struct xwcq * cq;

        cq = arg;
        rc = XWOK;
        cqlogf(INFO, "[生产者] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                if (XWOK == rc) {
                        size = sizeof("Message Content");
                        rc = xwcq_eq(cq, (const xwu8_t *)"Message Content", &size);
                        if (XWOK == rc) {
                                cqlogf(INFO,
                                       "[生产者] 发送消息{data = %s}。\n",
                                       "Message Content");
                        }
                }
                xwos_cthd_sleep(XWTM_S(1));
        }
        cqlogf(INFO, "[生产者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 消费者线程的主函数
 */
xwer_t xwcqdemo_consumer_main(void * arg)
{
        xwer_t rc;
        struct xwcq * cq;
        xwu8_t data[XWCQDEMO_SLOT_SIZE];
        xwsz_t size;

        cq = arg;
        rc = XWOK;
        cqlogf(INFO, "[消费者] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                size = XWCQDEMO_SLOT_SIZE;
                rc = xwcq_dq(cq, data, &size);
                if (XWOK == rc) {
                        cqlogf(INFO,
                               "[消费者] 接收消息{data = %s, size = %d}。\n",
                               data, size);
                }
        }
        cqlogf(INFO, "[消费者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
