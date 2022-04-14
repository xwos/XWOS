/**
 * @file
 * @brief W25Qxx编程器：硬件接口抽象层
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

#ifndef __xwam_application_w25qrpt_hwifal_h__
#define __xwam_application_w25qrpt_hwifal_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

#define W25QRPT_SDU_SIZE_HBYTE_MSK      0x3FU

struct w25qrpt;
struct w25qrpt_msg;

/**
 * @brief 硬件接口抽象层操作函数集
 */
struct w25qrpt_hwifal_operations {
        xwer_t (* open)(struct w25qrpt * /*w25qrpt*/); /**< 打开硬件接口 */
        xwer_t (* close)(struct w25qrpt * /*w25qrpt*/); /**< 关闭硬件接口 */
        xwer_t (* tx)(struct w25qrpt * /*w25qrpt*/,
                      const xwu8_t * /*data*/, xwsz_t * /*size*/,
                      xwtm_t /*to*/); /**< 发送数据 */
        xwer_t (* rx)(struct w25qrpt * /*w25qrpt*/,
                      xwu8_t * /*buf*/, xwsz_t * /*size*/,
                      xwtm_t /*to*/); /**< 接收数据 */
        void (* notify)(struct w25qrpt * /*w25qrpt*/, xwsq_t /*event*/); /**< 通知事件 */
};

/**
 * @brief 硬件接口状态
 */
enum w25qrpt_hwifal_state_em {
        W25QRPT_HWIFST_CLOSED, /**< 硬件接口已经关闭 */
        W25QRPT_HWIFST_OPENED, /**< 硬件接口已经打开 */
};

xwer_t w25qrpt_hwifal_open(struct w25qrpt * w25qrpt, void * hwifcb);
xwer_t w25qrpt_hwifal_close(struct w25qrpt * w25qrpt);
xwer_t w25qrpt_hwifal_tx(struct w25qrpt * w25qrpt, struct w25qrpt_msg * msg);
xwer_t w25qrpt_hwifal_rx(struct w25qrpt * w25qrpt, struct w25qrpt_msg * msgbuf);

void w25qrpt_fill_msg_parity(struct w25qrpt_msg * msg);
xwer_t w25qrpt_chk_size_parity(xwu8_t * size);
xwsz_t w25qrpt_get_sdu_size(struct w25qrpt_msg * msg);

#endif /* xwam/application/w25qrpt/hwifal.h */
