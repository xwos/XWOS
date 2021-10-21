/**
 * @file
 * @brief 精简的点对点通讯协议：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwscp_mif_h__
#define __xwmd_isc_xwscp_mif_h__

#include <xwos/standard.h>

/**
 * @defgroup xwmd_isc_xwscp 精简的点对点通讯协议
 * @ingroup xwmd_isc
 * @{
 */

#if (XWMDCFG_isc_xwscp_MEMBLK_SIZE & (XWMDCFG_isc_xwscp_MEMBLK_SIZE - 1))
#  error "XWMDCFG_isc_xwscp_MEMBLK_SIZE must be the power of 2!"
#endif
#if ((1 << XWMDCFG_isc_xwscp_MEMBLK_ODR) > 32768U)
#  error "power(2, XWMDCFG_isc_xwscp_MEMBLK_ODR) must be <= 32768!"
#endif

#define XWSCP_VERSION           ("3.0.0") /**< XWSCP的版本号字符串 */
#define XWSCP_VERSION_MAJOR     3U /**< XWSCP的大版本号 */
#define XWSCP_VERSION_MINOR     0U /**< XWSCP的小版本号 */
#define XWSCP_VERSION_REVISION  0U /**< XWSCP的修订版本号 */

#define XWSCP_RETRY_PERIOD      (XWMDCFG_isc_xwscp_PERIOD) /**< 重试发送的周期 */
#define XWSCP_MEMBLK_SIZE       (XWMDCFG_isc_xwscp_MEMBLK_SIZE) /**< 单位内存块的大小 */
#define XWSCP_MEMBLK_ODR        (XWMDCFG_isc_xwscp_MEMBLK_ODR) /**< 内存块数量的阶，内存块数量等于1左移“阶”次 */
#define XWSCP_MEMBLK_NUM        (1 << XWSCP_MEMBLK_ODR) /**< 内存块的数量 */
#define XWSCP_MEMPOOL_SIZE      (XWSCP_MEMBLK_SIZE * XWSCP_MEMBLK_NUM) /**< 内存池的总大小 */

/**
 * @brief 定义内存池
 * @param[in] name: 数值名
 */
#define XWSCP_DEF_MEMPOOL(name) __xwcc_alignl1cache xwu8_t name[XWSCP_MEMPOOL_SIZE]

/**
 * @brief 服务质量枚举
 */
enum xwscp_msg_qos_em {
        XWSCP_MSG_QOS_0 = 0, /**< 不可靠消息，无CRC校验码 */
        XWSCP_MSG_QOS_1, /**< 不可靠消息，有CRC校验码 */
        XWSCP_MSG_QOS_2, /**< 可靠消息，无CRC校验码 */
        XWSCP_MSG_QOS_3, /**< 可靠消息，有CRC校验码 */
        XWSCP_MSG_QOS_NUM,

        XWSCP_MSG_QOS_CHKSUM_MSK = (1 << 0U),
        XWSCP_MSG_QOS_ACK_MSK = (1 << 1U),
};

struct xwscp;
struct xwscp_hwifal_operations;

xwer_t xwscp_start(struct xwscp * xwscp, const char * name,
                   const struct xwscp_hwifal_operations * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize);
xwer_t xwscp_stop(struct xwscp * xwscp);
xwer_t xwscp_connect(struct xwscp * xwscp, xwtm_t * xwtm);
xwer_t xwscp_tx(struct xwscp * xwscp,
                const xwu8_t data[], xwsz_t * size, xwu8_t qos,
                xwtm_t * xwtm);
xwer_t xwscp_rx(struct xwscp * xwscp,
                xwu8_t buf[], xwsz_t * size,
                xwtm_t * xwtm);
xwer_t xwscp_try_rx(struct xwscp * xwscp,
                    xwu8_t buf[], xwsz_t * size);

/**
 * @} xwmd_isc_xwscp
 */

#endif /* xwmd/isc/xwscp/mif.h */
