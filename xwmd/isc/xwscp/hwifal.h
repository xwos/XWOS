/**
 * @file
 * @brief 精简的点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwscp_hwifal_h__
#define __xwmd_isc_xwscp_hwifal_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SOSCP_HWIFAL_SOF                        ((xwu8_t)'S')
#define SOSCP_HWIFAL_EOF                        ((xwu8_t)(SOSCP_HWIFAL_SOF ^ 0xFF))
#define SOSCP_HWIFAL_SOF_SIZE                   (1U)
#define SOSCP_HWIFAL_EOF_SIZE                   (1U)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwscp;
struct xwscp_frmslot;
struct xwscp_frame;

/**
 * @breif 硬件接口抽象层操作函数集
 */
struct xwscp_hwifal_operations {
        xwer_t (*open)(struct xwscp *); /**< 打开硬件接口 */
        xwer_t (*close)(struct xwscp *); /**< 关闭硬件接口 */
        xwer_t (*tx)(struct xwscp *, const xwu8_t *, xwsz_t, xwtm_t *); /**< 发送数据 */
        xwer_t (*rx)(struct xwscp *, xwu8_t *, xwsz_t *); /**< 接收数据 */
        void (*notify)(struct xwscp *, xwsq_t); /**< 通知事件 */
};

enum xwscp_hwifal_state_em {
        SOSCP_HWIFST_CLOSED, /**< 硬件接口已经关闭 */
        SOSCP_HWIFST_OPENED, /**< 硬件接口已经打开 */
};

enum xwscp_hwifal_notification_em {
        SOSCP_HWIFNTF_CONNECT = 0, /**< 与远程端链接 */
        SOSCP_HWIFNTF_NETUNREACH = 1, /**< 远程端无响应 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_code
xwer_t xwscp_hwifal_open(struct xwscp * xwscp);

__xwmd_code
xwer_t xwscp_hwifal_close(struct xwscp * xwscp);

__xwmd_code
xwer_t xwscp_hwifal_tx(struct xwscp * xwscp, struct xwscp_frame * frm, xwtm_t * xwtm);

__xwmd_code
xwer_t xwscp_hwifal_rx(struct xwscp * xwscp, struct xwscp_frmslot ** frmslotbuf);

__xwmd_code
void xwscp_hwifal_notify(struct xwscp * xwscp, xwsq_t evt, xwtm_t * xwtm);

#endif /* xwmd/isc/xwscp/hwifal.h */
