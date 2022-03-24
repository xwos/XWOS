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
#include <xwos/osal/skd.h>
#include <xwmd/isc/xwmq/mif.h>
#include <xwam/example/isc/xwmq/mif.h>

#define LOGTAG "xwmq"
#define mqlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

#define XWMQDEMO_THD_PRIORITY XWOS_SKD_PRIORITY_DROP(XWOS_SKD_PRIORITY_RT_MAX, 1)

xwer_t xwmqdemo_producer_func(void * arg);
struct xwos_thd * xwmqdemo_producer;

xwer_t xwmqdemo_consumer_func(void * arg);
struct xwos_thd * xwmqdemo_consumer;

struct xwmq xwmqdemo_mq;

/**
 * @brief 模块的加载函数
 */
xwer_t example_xwmq_start(void)
{
        struct xwos_thd_attr attr;
        xwer_t rc;

        rc = xwmq_init(&xwmqdemo_mq, "demo.xwmq");
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
        rc = xwos_thd_create(&xwmqdemo_producer, &attr, xwmqdemo_producer_func, NULL);
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
        rc = xwos_thd_create(&xwmqdemo_consumer, &attr, xwmqdemo_consumer_func, NULL);
        if (rc < 0) {
                goto err_consumer_create;
        }

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
xwer_t xwmqdemo_producer_func(void * arg)
{
        xwer_t rc;
        xwtm_t sleep;
        int topic;
        struct xwmq_msg * msg;

        XWOS_UNUSED(arg);
        rc = XWOK;
        mqlogf(INFO, "[生产者] 启动。\n");
        topic = 0;
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwmq_msg_create(&msg);
                if (XWOK == rc) {
                        rc = xwmq_eq(&xwmqdemo_mq, msg, topic, "Message Content");
                        if (XWOK == rc) {
                                mqlogf(INFO,
                                       "[生产者] 发送消息{topic = %d, data = %s}。\n",
                                       topic++, "Message Content");
                        }
                }
                sleep = 1 * XWTM_S;
                xwos_cthd_sleep(&sleep);
        }
        mqlogf(INFO, "[生产者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}

/**
 * @brief 消费者线程的主函数
 */
xwer_t xwmqdemo_consumer_func(void * arg)
{
        xwer_t rc;
        struct xwmq_msg * msg;

        XWOS_UNUSED(arg);
        rc = XWOK;
        mqlogf(INFO, "[消费者] 启动。\n");
        while (!xwos_cthd_frz_shld_stop(NULL)) {
                rc = xwmq_dq(&xwmqdemo_mq, &msg);
                if (XWOK == rc) {
                        mqlogf(INFO,
                               "[消费者] 接收消息{topic = %d, data = %s}。\n",
                               msg->topic, msg->data);
                        xwmq_msg_fini(msg);
                }
        }
        mqlogf(INFO, "[消费者] 退出。\n");
        xwos_thd_detach(xwos_cthd_self());
        return rc;
}
