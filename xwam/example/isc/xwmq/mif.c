/**
 * @file
 * @brief 示例：消息队列
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
#include <xwmd/isc/xwmq/mif.h>
#include <xwam/example/isc/xwmq/mif.h>

#define LOGTAG "xwmq"
#define mqlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWMQDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

xwer_t xwmqdemo_producer_main(void * arg);
xwos_thd_d xwmqdemo_producer;

xwer_t xwmqdemo_consumer_main(void * arg);
xwos_thd_d xwmqdemo_consumer;

struct xwmq xwmqdemo_mq;
struct xwmq_msg xwmqdemo_mq_txq[16];

/**
 * @brief 模块的加载函数
 */
xwer_t xwos_example_xwmq(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        rc = xwmq_init(&xwmqdemo_mq, "demo.xwmq",
                       xwmqdemo_mq_txq, xw_array_size(xwmqdemo_mq_txq));
        if (rc < 0) {
                goto err_xwmq_init;
        }

        /* 创建线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "demo.xwmq.producer";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = XWMQDEMO_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&xwmqdemo_producer, &attr, xwmqdemo_producer_main,
                             &xwmqdemo_mq);
        if (rc < 0) {
                goto err_producer_create;
        }

        xwos_thd_attr_init(&attr);
        attr.name = "demo.xwmq.consumer";
        attr.stack = NULL;
        attr.stack_size = 2048;
        attr.priority = XWMQDEMO_THD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_create(&xwmqdemo_consumer, &attr, xwmqdemo_consumer_main,
                             &xwmqdemo_mq);
        if (rc < 0) {
                goto err_consumer_create;
        }

        mqlogf(INFO,
               "sizeof(struct xwmq): %d, sizeof(struct xwmq_msg): %d\n",
               sizeof(struct xwmq), sizeof(struct xwmq_msg));

        return XWOK;

err_consumer_create:
        xwos_thd_stop(xwmqdemo_producer, NULL);
err_producer_create:
        xwmq_fini(&xwmqdemo_mq);
err_xwmq_init:
        return rc;
}

/**
 * @brief 生产者线程的主函数
 */
xwer_t xwmqdemo_producer_main(void * arg)
{
        xwer_t rc;
        xwsq_t topic;
        struct xwmq * mq;

        mq = arg;
        rc = XWOK;
        mqlogf(INFO, "[生产者] 启动。\n");
        topic = 0;
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                if (XWOK == rc) {
                        rc = xwmq_eq(mq, topic, "Message Content");
                        if (XWOK == rc) {
                                mqlogf(INFO,
                                       "[生产者] 发送消息{topic = %d, data = %s}。\n",
                                       topic++, "Message Content");
                        }
                }
                xwos_cthd_sleep(XWTM_S(1));
        }
        mqlogf(INFO, "[生产者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 消费者线程的主函数
 */
xwer_t xwmqdemo_consumer_main(void * arg)
{
        xwer_t rc;
        char * msg;
        xwsq_t topic;
        struct xwmq * mq;

        mq = arg;
        rc = XWOK;
        mqlogf(INFO, "[消费者] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwmq_dq(mq, &topic, (void **)&msg);
                if (XWOK == rc) {
                        mqlogf(INFO,
                               "[消费者] 接收消息{topic = %d, data = %s}。\n",
                               topic, msg);
                }
        }
        mqlogf(INFO, "[消费者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
