/**
 * @file
 * @brief 精简的点对点通讯协议：协议层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_xwscp_protocol_h__
#define __xwmd_xwscp_protocol_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/mif.h>

#define XWSCP_SOF               ((xwu8_t)0xAA)
#define XWSCP_EOF               ((xwu8_t)(XWSCP_SOF ^ 0xFF))
#define XWSCP_SOF_SIZE          (2U)
#define XWSCP_EOF_SIZE          (2U)

#define XWSCP_FRMHEAD_SIZE(size)        ((size) & 0xFU)
#define XWSCP_FRMHEAD_SIZE_MIRROR(size) ((size) & 0xF0U)
#define XWSCP_FRMHEAD_MAXSIZE           (16U)

#define XWSCP_PORT_CMD          (0U)
#define XWSCP_PORT_DATA         (1U)

#define XWSCP_ID_MSK            (0x7FU)
#define XWSCP_ID(id)            ((xwu8_t)((xwu8_t)(id) & XWSCP_ID_MSK))
#define XWSCP_ID_ACK            XWBOP_BIT(7)

#define XWSCP_ECSIZE(head)      (XWSCP_FRMHEAD_SIZE((head)->headsize) - \
                                 sizeof(struct xwscp_frmhead))
#define XWSCP_SDUPOS(head)      (&((head)->ecsdusz[XWSCP_ECSIZE(head)]))
#define XWSCP_RXSDUSIZE(slot)   ((slot)->frmsize - \
                                 XWSCP_FRMHEAD_SIZE((slot)->frm.head.headsize) - \
                                 XWSCP_CRC32_SIZE - XWSCP_SOF_SIZE - XWSCP_EOF_SIZE)
#define XWSCP_CRC32_SIZE        (4U)

#define XWSCP_SDU_MAX_SIZE      (XWSCP_MEMPOOL_SIZE / 8)
#define XWSCP_FRM_MINSIZE       (sizeof(struct xwscp_frmhead) + 1 + XWSCP_CRC32_SIZE)

/**
 * @brief 调试XWSCP的日志函数
 */
#if defined(XWMDCFG_isc_xwscp_LOG) && (1 == XWMDCFG_isc_xwscp_LOG)
#  define XWSCP_LOG_TAG                 "xwscp"
#  define xwscplogf(lv, fmt, ...)        xwlogf(lv, XWSCP_LOG_TAG, fmt, ##__VA_ARGS__)
#else
#  define xwscplogf(lv, fmt, ...)
#endif

#define XWSCP_BUG()     XWOS_BUG()
#define XWSCP_BUG_ON(x) XWOS_BUG_ON(x)

#if defined(XWMDCFG_CHECK_PARAMETERS) && (1 == XWMDCFG_CHECK_PARAMETERS)
#  define XWSCP_VALIDATE(exp, errstr, ...)      \
        if (__xwcc_unlikely(!(exp))) {          \
                return __VA_ARGS__;             \
        }
#else
#  define XWSCP_VALIDATE(exp, errstr, ...)
#endif

#define XWSCP_VALIDATE_FORCE(exp, errstr, ...)  \
        if (__xwcc_unlikely(!(exp))) {          \
            return __VA_ARGS__;                 \
        }

/**
 * @brief 端口0命令
 */
enum xwscp_port0_cmd_em {
        XWSCP_PORT0_CMD_CONNECT,
};

/**
 * @brief 应答信号枚举
 */
enum xwscp_ack_em {
        XWSCP_ACK_OK = 0, /**< OK */
        XWSCP_ACK_ECONNRESET, /**< 链接被重置（远端复位？） */
        XWSCP_ACK_EALREADY, /**< 已经完成 */
        XWSCP_ACK_NOMEM, /**< 内存不足 */
        XWSCP_ACK_NUM, /**< ACK的数量 */
};

/**
 * @brief XWSCP帧的信息头
 */
struct __xwcc_packed xwscp_frmhead {
        xwu8_t headsize; /**< 帧头的长度 | 镜像反转 */
        xwu8_t chksum; /**< 帧头的校验和 */
        xwu8_t port; /**< 端口0：协议内部命令端口
                          端口1：数据端口 */
        xwu8_t id; /**< bit[0:6]:消息的ID
                        bit[7]: 应答标志 */
        xwu8_t qos; /**< QoS */
        xwu8_t ecsdusz[0]; /**< 数据长度：变长编码 */
};

/**
 * @brief XWSCP的帧
 */
struct __xwcc_packed xwscp_frm {
        xwu8_t sof[XWSCP_SOF_SIZE]; /**< 帧首定界符（SOF:Start-of-Frame Delimiter） */
        struct xwscp_frmhead head; /**< 帧头 */
        /* xwu8_t sdu[x]; 数据 */
        /* xwu8_t sducrc32[4]; 数据的CRC32校验码 */
        /* xwu8_t eof[XWSCP_EOF_SIZE]; 帧尾定界符（EOF:End-of-Frame Delimiter） */
};

/**
 * @brief XWSCP帧槽
 */
union __xwcc_aligned(XWMM_ALIGNMENT) xwscp_slot {
        struct {
                struct xwlib_bclst_node node; /**< 链表节点 (占位，发送时未使用) */
                xwsz_t frmsize; /**< 帧的总长度 */
                struct xwscp_frm frm; /**< 帧 */
        } tx; /**< 发送时的帧槽 */
        struct {
                struct xwlib_bclst_node node; /**< 链表节点 */
                xwsz_t frmsize; /**< 帧的总长度 */
                struct xwscp_frm frm; /**< 帧 */
        } rx;
};

/**
 * @breif XWSCP对象
 */
struct xwscp {
        /* 基本信息 */
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name;
        xwsq_a hwifst; /**< 硬件接口抽象层状态 */
        const struct xwscp_hwifal_operations * hwifops; /**< 硬件接口抽象层操作函数 */
        void * hwifcb; /**< 物理层端口 */

        /* 内存池 */
        XWMM_BMA_DEF(mempool_bma_raw, XWSCP_MEMBLK_ODR); /**< 内存池分配器的原始内存 */
        struct xwmm_bma * mempool; /**< 内存池分配器 */

        struct {
                xwu32_a cnt; /**< 发送计数器 */
                struct xwos_mtx mtx; /**< 发送锁 */
                struct xwos_mtx csmtx; /**< 保护发送和接收线程的共享数据的锁 */
                struct xwos_cond cscond; /**< 同步发送和接收线程的条件量 */
                struct {
                        xwu8_t ack; /**< 远端回复的应答 */
                        xwu8_t id; /**< 远端的消息id */
                } remote; /**< 正在发送的帧信息 */
        } tx; /**< 发送状态机 */

        struct {
                struct xwos_thd * thd; /**< 接收线程 */
                xwu32_a cnt; /**< 接收计数器 */
                struct xwlib_bclst_head head; /**< 链表头 */
                struct xwos_splk lock; /**< 保护接收队列的自旋锁 */
                struct xwos_sem sem; /**< 接收队列的信号量 */
        } rx; /**< 接收队列 */
};

xwer_t xwscp_grab(struct xwscp * xwscp);
xwer_t xwscp_put(struct xwscp * xwscp);
void xwscp_encode_sdusize(xwsz_t sdusize, xwu8_t * ecsdusz, xwu8_t * ecsize);
void xwscp_decode_sdusize(xwu8_t * ecsdusz, xwsz_t * sdusize);
xwu8_t xwscp_cal_head_chksum(xwu8_t head[], xwsz_t size);
bool xwscp_chk_head(xwu8_t head[], xwsz_t size);
void xwscp_rxq_pub(struct xwscp * xwscp, union xwscp_slot * slot);
union xwscp_slot * xwscp_rxq_choose(struct xwscp * xwscp);
xwer_t xwscp_thd(struct xwscp * xwscp);
xwer_t xwscp_tx_cmd_connect(struct xwscp * xwscp);
xwer_t xwscp_fmt_msg(struct xwscp * xwscp,
                     const xwu8_t sdu[], xwsz_t sdusize, xwu8_t qos,
                     union xwscp_slot ** out);
xwer_t xwscp_tx_sdu_ack(struct xwscp * xwscp, xwu8_t id, xwu8_t ack);

#endif /* xwmd/isc/xwscp/protocol.h */
