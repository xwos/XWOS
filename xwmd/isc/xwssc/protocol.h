/**
 * @file
 * @brief SOC间点对点通讯协议：协议层
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwssc_protocol_h__
#define __xwmd_isc_xwssc_protocol_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwmd/isc/xwssc/hwifal.h>
#include <xwmd/isc/xwssc/mif.h>

/**
 * @ingroup xwmd_isc_xwssc_protocol
 * @{
 */

#define XWSSC_SOF               ((xwu8_t)0xAA)
#define XWSSC_EOF               (XWSSC_SOF ^ (xwu8_t)0xFF)
#define XWSSC_SOF_SIZE          (2U)
#define XWSSC_EOF_SIZE          (2U)

#define XWSSC_FRMHEAD_SIZE(size)        ((size) & 0xFU)
#define XWSSC_FRMHEAD_SIZE_MIRROR(size) ((size) & 0xF0U)
#define XWSSC_FRMHEAD_MAXSIZE           (16U)
#define XWSSC_FRM_ONSTACK_MAXSIZE       (64U)

#define XWSSC_ID_MSK            (0xFFU)
#define XWSSC_ID(id)            ((xwu8_t)((xwu8_t)(id) & XWSSC_ID_MSK))

#define XWSSC_ECSIZE(head)      (XWSSC_FRMHEAD_SIZE((head)->headsize) - \
                                 sizeof(struct xwssc_frmhead))
#define XWSSC_SDUPOS(head)      (&((head)->ecsdusz[XWSSC_ECSIZE(head)]))
#define XWSSC_RXSDUSIZE(slot)   ((slot)->frmsize - \
                                 XWSSC_FRMHEAD_SIZE((slot)->frm.head.headsize) - \
                                 XWSSC_CRC32_SIZE - XWSSC_SOF_SIZE - XWSSC_EOF_SIZE)
#define XWSSC_CRC32_SIZE        (4U)
#define XWSSC_SDU_MAX_SIZE      (XWSSC_MEMPOOL_SIZE / 8)

#define XWSSC_TXTHD_STACK       (4096U)
#define XWSSC_RXTHD_STACK       (4096U)

#if defined(XWMDCFG_isc_xwssc_LOG) && (1 == XWMDCFG_isc_xwssc_LOG)
#  define xwssclogf(xwssc, lv, fmt, ...) xwlogf(lv, "%s", \
                                                fmt, xwssc->name, ##__VA_ARGS__)
#else
#  define xwssclogf(xwssc, lv, fmt, ...)
#endif

#define XWSSC_BUG()     XWOS_BUG()
#define XWSSC_BUG_ON(x) XWOS_BUG_ON(x)

#if defined(XWMDCFG_CHECK_PARAMETERS) && (1 == XWMDCFG_CHECK_PARAMETERS)
#define XWSSC_VALIDATE(exp, errstr, ...)        \
        if (!(exp)) {                           \
            return __VA_ARGS__;                 \
        }
#else
#  define XWSSC_VALIDATE(exp, errstr, ...)
#endif

#define XWSSC_VALIDATE_FORCE(exp, errstr, ...)  \
        if (!(exp)) {                           \
            return __VA_ARGS__;                 \
        }

/**
 * @brief 协议内部标志
 */
enum xwssc_flag_em {
        XWSSC_FLAG_CONNECT = 0x10U, /**< 连接标志 */
        XWSSC_FLAG_ACK = 0x80U, /**< 应答标志 */
        XWSSC_FLAG_SDU = 0x00U, /**< 数据 */
        XWSSC_FLAG_MSK = 0xF0U,
};

/**
 * @brief 应答信号枚举
 */
enum xwssc_frm_ack_em {
        XWSSC_ACK_OK = 0, /**< OK */
        XWSSC_ACK_ECONNRESET, /**< 链接被重置（远端复位？） */
        XWSSC_ACK_EALREADY, /**< 已经完成 */
        XWSSC_ACK_NOMEM, /**< 内存不足 */
        XWSSC_ACK_NUM, /**< ACK的数量 */
};

/**
 * @brief XWSSC帧的信息头
 */
struct __xwcc_packed xwssc_frmhead {
        xwu8_t headsize; /**< 帧头的长度 | 镜像反转 */
        xwu8_t chksum; /**< 帧头的校验和 */
        xwu8_t port; /**< 端口 */
        xwu8_t id; /**< 消息的ID */
        xwu8_t qos; /**< [0:3]: QoS, [4:7]: 协议内部标签 */
        xwu8_t ecsdusz[0]; /**< 数据长度：变长编码 */
};

/**
 * @brief XWSSC的帧
 */
struct __xwcc_packed xwssc_frm {
        xwu8_t sof[XWSSC_SOF_SIZE]; /**< 帧首定界符（SOF:Start-of-Frame Delimiter） */
        struct xwssc_frmhead head; /**< 帧头 */
        /* xwu8_t sdu[x]; 数据 */
        /* xwu8_t sducrc32[4]; 数据的CRC32校验码 */
        /* xwu8_t eof[XWSSC_EOF_SIZE]; 帧尾定界符（EOF:End-of-Frame Delimiter） */
};

/**
 * @brief XWSSC帧槽
 */
union xwssc_slot {
        struct {
                xwssc_ntf_f ntfcb; /**< 通知发送结果的回调函数 */
                void * cbarg; /**< 调用回调函数时的用户数据 */
                xwsz_t frmsize; /**< 帧的总长度 */
                struct xwssc_frm frm; /**< 帧 */
        } tx; /**< 发送时的帧槽 */
        struct {
                struct xwlib_bclst_node node; /**< 链表节点 */
                xwsz_t frmsize; /**< 帧的总长度 */
                struct xwssc_frm frm; /**< 帧 */
        } rx; /**< 接收时的帧槽 */
};

/**
 * @brief 包含待发送帧的发送器
 */
struct xwssc_carrier {
        struct xwlib_bclst_node node; /**< 链表节点 */
        atomic_xwu32_t state; /**< 状态 */
        xwu16_t idx; /**< 序号 */
        xwu8_t pri; /**< 优先级 */
        union xwssc_slot * slot;
};

/**
 * @brief XWSSC对象
 */
struct xwssc {
        /* 基本信息 */
        struct xwos_object xwobj; /**< C语言面向对象：继承 `struct xwos_object` */
        const char * name;
        atomic_xwsq_t hwifst; /**< 硬件层状态 */
        const struct xwssc_hwifal_operation * hwifops; /**< 硬件接口抽象层操作函数 */
        void * hwifcb; /**< 接口硬件 */

        /* 内存池 */
        XWMM_BMA_RAWOBJ_DEF(mempool_bma_raw, XWSSC_MEMBLK_ODR); /**< 内存池分配器的原始内存 */
        struct xwmm_bma * mempool; /**< 内存池分配器 */

        /* 发送状态机 */
        __xwcc_alignl1cache xwu8_t txthd_stack[XWSSC_TXTHD_STACK]; /**< 发送线程的栈 */
        struct xwos_thd txthdobj; /**< 发送线程的线程结构体 */
        xwos_thd_d txthd; /**< 发送线程的线程描述符 */
        struct {
                atomic_xwu32_t cnt; /**< 发送计数器 */
                struct xwssc_carrier car[XWSSC_MEMBLK_NUM]; /**< 包含待发送帧的发送器 */
                xwbmpaop_define(carbmp, XWSSC_MEMBLK_NUM); /**< 发送器的索引位图 */
                struct xwlib_bclst_head q[XWSSC_PRI_NUM]; /**< 每优先级发送队列 */
                xwbmpop_define(qnebmp, XWSSC_PRI_NUM); /**< 发送队列非空的索引位图 */
                struct xwos_splk qlock; /**< 保护发送队列的自旋锁 */
                struct xwos_sem qsem; /**< 指示待发送帧的数量的信号量 */
                struct xwos_mtx csmtx; /**< 保护发送和接收线程的共享数据的锁 */
                struct xwos_cond cscond; /**< 同步发送和接收线程的条件量 */
                struct {
                        xwu8_t ack; /**< 远端回复的应答 */
                        xwu8_t id; /**< 远端的消息id */
                } remote; /**< 正在发送的帧信息 */
                struct xwssc_carrier * tmp; /**< 缓存正在发送的帧槽 */
                struct xwos_splk notiflk; /**< 保证通知回调函数原子性的自旋锁 */
        } txq; /**< 发送队列 */

        /* 接收状态机 */
        __xwcc_alignl1cache xwu8_t rxthd_stack[XWSSC_RXTHD_STACK]; /**< 接收线程的栈 */
        struct xwos_thd rxthdobj; /**< 接收线程的线程结构体 */
        xwos_thd_d rxthd; /**< 接收线程的线程描述符 */
        struct {
                atomic_xwu32_t cnt; /**< 接收计数器 */
                struct xwlib_bclst_head q[XWSSC_PORT_NUM]; /**< 每个端口的接收队列 */
                struct xwos_splk lock[XWSSC_PORT_NUM]; /**< 保护每个接收队列的锁 */
                struct xwos_sem sem[XWSSC_PORT_NUM]; /**< 每个接收队列的信号量 */
        } rxq; /**< 接收队列 */
};

xwer_t xwssc_grab(struct xwssc * xwssc);
xwer_t xwssc_put(struct xwssc * xwssc);
void xwssc_encode_sdusize(xwsz_t sdusize, xwu8_t * ecsdusz, xwu8_t * ecsize);
void xwssc_decode_sdusize(xwu8_t * ecsdusz, xwsz_t * sdusize);
xwu8_t xwssc_cal_head_chksum(xwu8_t data[], xwsz_t size);
bool xwssc_chk_head(xwu8_t data[], xwsz_t size);
union xwssc_slot * xwssc_rxq_choose(struct xwssc * xwssc, xwu8_t port);
xwer_t xwssc_rxthd(struct xwssc * xwssc);
struct xwssc_carrier * xwssc_txq_carrier_alloc(struct xwssc * xwssc);
void xwssc_txq_carrier_free(struct xwssc * xwssc, struct xwssc_carrier * car);
struct xwssc_carrier * xwssc_txq_choose(struct xwssc * xwssc);
xwer_t xwssc_eq_msg(struct xwssc * xwssc,
                    const xwu8_t sdu[], xwsz_t sdusize,
                    xwu8_t pri, xwu8_t port, xwu8_t qos,
                    xwssc_ntf_f ntfcb, void * cbarg,
                    xwssc_txh_t * txhbuf);
xwer_t xwssc_tx_ack_sdu(struct xwssc * xwssc, xwu8_t port, xwu8_t id, xwu8_t ack);
xwer_t xwssc_txthd(struct xwssc * xwssc);

/**
 * @} xwmd_isc_xwssc_protocol
 */

#endif /* xwmd/isc/xwssc/protocol.h */
