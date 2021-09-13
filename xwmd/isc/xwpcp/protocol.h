/**
 * @file
 * @brief 点对点通讯协议：协议层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_isc_xwpcp_protocol_h__
#define __xwmd_isc_xwpcp_protocol_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/mif.h>

#define XWPCP_SOF               ((xwu8_t)0xAA)
#define XWPCP_EOF               ((xwu8_t)(XWPCP_SOF ^ 0xFF))
#define XWPCP_SOF_SIZE          (2U)
#define XWPCP_EOF_SIZE          (2U)

#define XWPCP_FRMHEAD_SIZE(size)        ((size) & 0xFU)
#define XWPCP_FRMHEAD_SIZE_MIRROR(size) ((size) & 0xF0U)
#define XWPCP_FRMHEAD_MAXSIZE           (16U)

#define XWPCP_PORT_CMD          (0U)

#define XWPCP_ID_MSK            (0x7FU)
#define XWPCP_ID(id)            ((xwu8_t)((xwu8_t)(id) & XWPCP_ID_MSK))
#define XWPCP_ID_ACK            XWBOP_BIT(7)

#define XWPCP_ECSIZE(head)      (XWPCP_FRMHEAD_SIZE((head)->headsize) - \
                                 sizeof(struct xwpcp_frmhead))
#define XWPCP_SDUPOS(head)      (&((head)->ecsdusz[XWPCP_ECSIZE(head)]))
#define XWPCP_RXSDUSIZE(slot)   ((slot)->frmsize - \
                                 XWPCP_FRMHEAD_SIZE((slot)->frm.head.headsize) - \
                                 XWPCP_CRC32_SIZE - XWPCP_SOF_SIZE - XWPCP_EOF_SIZE)
#define XWPCP_CRC32_SIZE        (4U)

#define XWPCP_SDU_MAX_SIZE      (XWPCP_MEMPOOL_SIZE / 8)
#define XWPCP_FRM_MINSIZE       (sizeof(struct xwpcp_frmhead) + 1 + XWPCP_CRC32_SIZE)

#if defined(XWMDCFG_isc_xwpcp_LOG) && (1 == XWMDCFG_isc_xwpcp_LOG)
#  define XWPCP_LOG_TAG                 "xwpcp"
#  define xwpcplogf(lv, fmt, ...)       xwlogf(lv, XWPCP_LOG_TAG, fmt, ##__VA_ARGS__)
#else
#  define xwpcplogf(lv, fmt, ...)
#endif

#define XWPCP_BUG()     XWOS_BUG()
#define XWPCP_BUG_ON(x) XWOS_BUG_ON(x)

#if defined(XWMDCFG_CHECK_PARAMETERS) && (1 == XWMDCFG_CHECK_PARAMETERS)
#define XWPCP_VALIDATE(exp, errstr, ...)        \
        if (__xwcc_unlikely(!(exp))) {          \
            return __VA_ARGS__;                 \
        }
#else
#  define XWPCP_VALIDATE(exp, errstr, ...)
#endif

#define XWPCP_VALIDATE_FORCE(exp, errstr, ...)  \
        if (__xwcc_unlikely(!(exp))) {          \
            return __VA_ARGS__;                 \
        }

/**
 * @brief 端口0命令
 */
enum xwpcp_port0_cmd_em {
        XWPCP_PORT0_CMDID_CONNECT,
};

/**
 * @brief 应答信号枚举
 */
enum xwpcp_frm_ack_em {
        XWPCP_ACK_OK = 0, /**< OK */
        XWPCP_ACK_ECONNRESET, /**< 链接被重置（远端复位？） */
        XWPCP_ACK_EALREADY, /**< 已经完成 */
        XWPCP_ACK_NOMEM, /**< 内存不足 */
        XWPCP_ACK_NUM, /**< ACK的数量 */
};

/**
 * @brief XWPCP帧的信息头
 */
struct __xwcc_packed xwpcp_frmhead {
        xwu8_t headsize; /**< 帧头的长度 | 镜像反转 */
        xwu8_t chksum; /**< 帧头的校验和 */
        xwu8_t port; /**< 端口，端口0保留为协议内部使用 */
        xwu8_t id; /**< bit[0:6]:消息的ID
                        bit[7]: 应答标志 */
        xwu8_t qos; /**< QoS */
        xwu8_t ecsdusz[0]; /**< 数据长度：变长编码 */
};

/**
 * @brief XWPCP的帧
 */
struct __xwcc_packed xwpcp_frm {
        xwu8_t sof[XWPCP_SOF_SIZE]; /**< 帧首定界符（SOF:Start-of-Frame Delimiter） */
        struct xwpcp_frmhead head; /**< 帧头 */
        /* xwu8_t sdu[x]; 数据 */
        /* xwu8_t sducrc32[4]; 数据的CRC32校验码 */
        /* xwu8_t eof[XWPCP_EOF_SIZE]; 帧尾定界符（EOF:End-of-Frame Delimiter） */
};

/**
 * @brief XWPCP帧槽
 */
union xwpcp_slot {
        struct {
                xwpcp_ntf_f ntfcb; /**< 通知发送结果的回调函数 */
                void * cbarg; /**< 调用回调函数时的用户数据 */
                xwsz_t frmsize; /**< 帧的总长度 */
                struct xwpcp_frm frm; /**< 帧 */
        } tx; /**< 发送时的帧槽 */
        struct {
                struct xwlib_bclst_node node; /**< 链表节点 */
                xwsz_t frmsize; /**< 帧的总长度 */
                struct xwpcp_frm frm; /**< 帧 */
        } rx; /**< 接收时的帧槽 */
};

/**
 * @breif 包含待发送帧的“发送装置”
 */
struct xwpcp_carrier {
        struct xwlib_bclst_node node; /**< 链表节点 */
        xwu32_a state; /**< 状态 */
        xwu16_t idx; /**< 序号 */
        xwu8_t pri; /**< 优先级 */
        union xwpcp_slot * slot;
};

/**
 * @breif XWPCP对象
 */
struct xwpcp {
        /* 基本信息 */
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name;
        xwsq_a hwifst; /**< 硬件层状态 */
        const struct xwpcp_hwifal_operation * hwifops; /**< 硬件接口抽象层操作函数 */
        void * hwifcb; /**< 接口硬件 */

        /* 内存池 */
        XWMM_BMA_DEF(mempool_bma_raw, XWPCP_MEMBLK_ODR); /**< 内存池分配器的原始内存 */
        struct xwmm_bma * mempool; /**< 内存池分配器 */

        /* 发送状态机 */
        struct xwos_thd * txthd; /**< 发送线程 */
        struct {
                xwu32_a cnt; /**< 发送计数器 */
                struct xwpcp_carrier car[XWPCP_MEMBLK_NUM]; /**< 包含待发送帧的“发送装置” */
                xwbmpaop_declare(carbmp, XWPCP_MEMBLK_NUM); /**< “发送装置” 的索引位图 */
                struct xwlib_bclst_head q[XWPCP_PRI_NUM]; /**< 每优先级发送队列 */
                xwbmpop_declare(qnebmp, XWPCP_PRI_NUM); /**< 发送队列非空的索引位图 */
                struct xwos_splk qlock; /**< 保护发送队列的自旋锁 */
                struct xwos_sem qsem; /**< 指示待发送帧的数量的信号量 */
                struct xwos_mtx csmtx; /**< 保护发送和接收线程的共享数据的锁 */
                struct xwos_cond cscond; /**< 同步发送和接收线程的条件量 */
                struct {
                        xwu8_t ack; /**< 远端回复的应答 */
                        xwu8_t id; /**< 远端的消息id */
                } remote; /**< 正在发送的帧信息 */
                struct xwpcp_carrier * tmp; /**< 缓存正在发送的帧槽 */
                struct xwos_splk notiflk; /**< 保证通知回调函数原子性的自旋锁 */
        } txq; /**< 发送队列 */

        /* 接收状态机 */
        struct xwos_thd * rxthd; /**< 接收线程的描述符 */
        struct {
                xwu32_a cnt; /**< 接收计数器 */
                struct xwlib_bclst_head q[XWPCP_PORT_NUM]; /**< 每个端口的接收队列 */
                struct xwos_splk lock[XWPCP_PORT_NUM]; /**< 保护每个接收队列的锁 */
                struct xwos_sem sem[XWPCP_PORT_NUM]; /**< 每个接收队列的信号量 */
        } rxq; /**< 接收队列 */
};

xwer_t xwpcp_grab(struct xwpcp * xwpcp);
xwer_t xwpcp_put(struct xwpcp * xwpcp);
void xwpcp_encode_sdusize(xwsz_t sdusize, xwu8_t * ecsdusz, xwu8_t * ecsize);
void xwpcp_decode_sdusize(xwu8_t * ecsdusz, xwsz_t * sdusize);
xwu8_t xwpcp_cal_head_chksum(xwu8_t data[], xwsz_t size);
bool xwpcp_chk_head(xwu8_t data[], xwsz_t size);
union xwpcp_slot * xwpcp_rxq_choose(struct xwpcp * xwpcp, xwu8_t port);
xwer_t xwpcp_rxthd(struct xwpcp * xwpcp);
struct xwpcp_carrier * xwpcp_txq_carrier_alloc(struct xwpcp * xwpcp);
void xwpcp_txq_carrier_free(struct xwpcp * xwpcp, struct xwpcp_carrier * car);
struct xwpcp_carrier * xwpcp_txq_choose(struct xwpcp * xwpcp);
xwer_t xwpcp_tx_sdu_ack(struct xwpcp * xwpcp, xwu8_t port, xwu8_t id, xwu8_t ack);
xwer_t xwpcp_eq_msg(struct xwpcp * xwpcp,
                    const xwu8_t sdu[], xwsz_t sdusize,
                    xwu8_t pri, xwu8_t port, xwu8_t qos,
                    xwpcp_ntf_f ntfcb, void * cbarg,
                    xwpcp_txh_t * txhbuf);
xwer_t xwpcp_txthd(struct xwpcp * xwpcp);

#endif /* xwmd/isc/xwpcp/protocol.h */
