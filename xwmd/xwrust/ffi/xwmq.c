/**
 * @file
 * @brief XWRUST FFI：消息队列
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwmd/isc/xwmq/mi.h>


xwer_t xwrustffi_xwmq_acquire(struct xwmq * mq, xwsq_t tik)
{
        xwmq_d mqd = {
                .mq = mq,
                .tik = tik,
        };
        return xwmq_acquire(mqd);
}

xwer_t xwrustffi_xwmq_release(struct xwmq * mq, xwsq_t tik)
{
        xwmq_d mqd = {
                .mq = mq,
                .tik = tik,
        };
        return xwmq_release(mqd);
}
