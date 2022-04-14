/**
 * @file
 * @brief 玄武设备栈：LIN控制器
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

#ifndef __xwcd_ds_lin_controller_h__
#define __xwcd_ds_lin_controller_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/device.h>

#define XWDS_LIN_DIAG_MSTREQ_MSG        0x3C
#define XWDS_LIN_DIAG_SLCREP_MSG        0x3D
#define XWDS_LIN_DEFAULT_DATA_SIZE      8

/**
 * @brief LIN接口模式枚举
 */
enum xwds_lin_mode_em {
        XWDS_LIN_MODE_CLOSE = 0,
        XWDS_LIN_MODE_TX = XWBOP_BIT(0),
        XWDS_LIN_MODE_RX = XWBOP_BIT(1),
};

/**
 * @brief LIN协议类型
 */
enum xwds_lin_chksum_type_em {
        XWDS_LIN_CHKSUM_CLASSIC = 0,
        XWDS_LIN_CHKSUM_ENHANCED,
};

/**
 * @brief LIN消息结构体
 */
struct xwds_lin_msg {
        xwu8_t id;
        xwu8_t data[XWDS_LIN_DEFAULT_DATA_SIZE];
        xwu8_t size;
};

/**
 * @brief LIN消息信息
 */
struct xwds_lin_msg_info {
        xwu8_t protected_id; /**< LIN保护ID */
        xwu8_t size; /**< 消息大小 */
};

/**
 * @brief LIN控制器配置
 */
struct xwds_lin_cfg {
        xwu32_t baudrate; /**< 波特率 */
        union {
                struct {
                        xwu32_t mode:8; /**< 模式 */
                        xwu32_t chksum:1; /**< 校验方式 */
                        xwu32_t reserved:23;
                } bit;
                xwu32_t u32;
        } attr;
        void * xwccfg; /**< SOC芯片私有配置 */
};

struct xwds_linc;

/**
 * @brief LIN控制器驱动函数表
 */
struct xwds_linc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* msttx)(struct xwds_linc *, xwu8_t, struct xwds_lin_msg *,
                         xwtm_t /*to*/); /**< 主机节点发送 */
        xwer_t (* slvtx)(struct xwds_linc *, struct xwds_lin_msg *,
                         xwtm_t /*to*/); /**< 从机节点发送 */
        xwer_t (* rx)(struct xwds_linc *, struct xwds_lin_msg *,
                      xwtm_t /*to*/); /**< 接收 */
};

/**
 * @brief LIN控制器
 */
struct xwds_linc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_lin_cfg * cfg; /**< 配置s */
        const struct xwds_lin_msg_info * msgitable; /**< LIN消息信息表 */
        xwsz_t msgitable_itemnum; /**< LIN消息信息表条目数量 */
        xwer_t (*get_msg_size)(struct xwds_linc *,
                               xwu8_t ,
                               xwu8_t *); /**< 虚函数：从LIN消息表中查询消息的大小。
                                               默认 @ref xwds_linc_get_msg_size()
                                               用户可以重载这个函数以便实现
                                               高效率的查询算法。*/

        /* private */
        struct xwos_mtx txlock; /**< 提供多线程访问安全的锁 */
};

void xwds_linc_construct(struct xwds_linc * linc);
void xwds_linc_destruct(struct xwds_linc * linc);

xwer_t xwds_linc_msttx(struct xwds_linc * linc,
                       xwu8_t id, struct xwds_lin_msg * msg,
                       xwtm_t to);
xwer_t xwds_linc_slvtx(struct xwds_linc * linc,
                       struct xwds_lin_msg * msg,
                       xwtm_t to);
xwer_t xwds_linc_rx(struct xwds_linc * linc,
                    struct xwds_lin_msg * msgbuf,
                    xwtm_t to);
xwer_t xwds_linc_get_msg_size(struct xwds_linc * linc,
                              xwu8_t protected_id,
                              xwu8_t * ret);

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] linc: LIN控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline_api
xwer_t xwds_linc_grab(struct xwds_linc * linc)
{
        return xwds_device_grab(&linc->dev);
}

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] linc: LIN控制器对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline_api
xwer_t xwds_linc_put(struct xwds_linc * linc)
{
        return xwds_device_put(&linc->dev);
}

#endif /* xwcd/ds/lin/controller.h */
