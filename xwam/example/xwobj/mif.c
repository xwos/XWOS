/**
 * @file
 * @brief 示例：线程栅栏
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
#include <xwos/osal/swt.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/osal/sync/flg.h>
#include <xwos/osal/sync/br.h>
#include <xwos/osal/sync/sel.h>
#include <xwmd/isc/xwcq/mif.h>
#include <xwmd/isc/xwmq/mif.h>
#include <xwmd/isc/xwpcp/protocol.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwam/example/xwobj/mif.h>

#define LOGTAG "xwobj"
#define xwobjlogf(lv, fmt, ...) xwlogf(lv, LOGTAG, fmt, ##__VA_ARGS__)

/**
 * @brief 测试模块的启动函数
 */
xwer_t xwos_example_xwobj(void)
{
        xwobjlogf(INFO, "sizeof(struct xwos_thd): %d\n",
                  sizeof(struct xwos_thd));
        xwobjlogf(INFO, "sizeof(struct xwos_swt): %d\n",
                  sizeof(struct xwos_swt));
        xwobjlogf(INFO, "sizeof(struct xwos_mtx): %d\n",
                  sizeof(struct xwos_mtx));
        xwobjlogf(INFO, "sizeof(struct xwos_splk): %d\n",
                  sizeof(struct xwos_splk));
        xwobjlogf(INFO, "sizeof(struct xwos_sqlk): %d\n",
                  sizeof(struct xwos_sqlk));
        xwobjlogf(INFO, "sizeof(struct xwos_sem): %d\n",
                  sizeof(struct xwos_sem));
        xwobjlogf(INFO, "sizeof(struct xwos_cond): %d\n",
                  sizeof(struct xwos_cond));
        xwobjlogf(INFO, "sizeof(struct xwos_flg): %d\n",
                  sizeof(struct xwos_flg));
        xwobjlogf(INFO, "sizeof(struct xwos_br): %d\n",
                  sizeof(struct xwos_br));
        xwobjlogf(INFO, "sizeof(struct xwos_sel): %d\n",
                  sizeof(struct xwos_sel));
        xwobjlogf(INFO, "sizeof(struct xwcq): %d\n",
                  sizeof(struct xwcq));
        xwobjlogf(INFO, "sizeof(struct xwmq): %d, sizeof(struct xwmq_msg): %d\n",
                  sizeof(struct xwmq), sizeof(struct xwmq_msg));
        xwobjlogf(INFO, "sizeof(struct xwpcp): %d\n",
                  sizeof(struct xwpcp));
        xwobjlogf(INFO, "sizeof(struct xwscp): %d\n",
                  sizeof(struct xwscp));
        return XWOK;
}
