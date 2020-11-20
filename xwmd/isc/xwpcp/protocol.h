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
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwlog.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwmd/isc/xwpcp/hwifal.h>
#include <xwmd/isc/xwpcp/api.h>

#if (XWMDCFG_isc_xwpcp_MEMBLK_SIZE & (XWMDCFG_isc_xwpcp_MEMBLK_SIZE - 1))
  #error "XWMDCFG_isc_xwpcp_MEMBLK_SIZE must be the order of 2!"
#endif
#if (XWMDCFG_isc_xwpcp_MEMBLK_NUM & (XWMDCFG_isc_xwpcp_MEMBLK_NUM - 1))
  #error "XWMDCFG_isc_xwpcp_MEMBLK_NUM must be the order of 2!"
#endif
#if (XWMDCFG_isc_xwpcp_PORT_NUM > 128)
  #error "XWPCP Only supports 128 ports (0 ~ 127)!"
#endif
#if (XWMDCFG_isc_xwpcp_PRIORITY_NUM > 64)
  #error "XWPCP Only supports 64 priorities!"
#endif

#define XWPCP_PRIORITY_NUM      (XWMDCFG_isc_xwpcp_PRIORITY_NUM)
#define XWPCP_MAX_PRIORITY      (XWPCP_PRIORITY_NUM - 1)
#define XWPCP_PORT_NUM          (XWMDCFG_isc_xwpcp_PORT_NUM)
#define XWPCP_RETRY_PERIOD      (XWMDCFG_isc_xwpcp_RETRY_PERIOD)
#define XWPCP_RETRY_NUM         (XWMDCFG_isc_xwpcp_RETRY_NUM)
#define XWPCP_MEMBLK_SIZE       (XWMDCFG_isc_xwpcp_MEMBLK_SIZE)
#define XWPCP_MEMBLK_NUM        (XWMDCFG_isc_xwpcp_MEMBLK_NUM)
#define XWPCP_SYNC_KEY          (XWMDCFG_isc_xwpcp_SYNC_KEY)

#define XWPCP_CHKSUM_SIZE       (4U)
#define XWPCP_INVALID_PRIORITY  (0xFFU)

#define XWPCP_PORT_MSK          (0x7FU)
#define XWPCP_PORT(port)        ((port) & XWPCP_PORT_MSK)
#define XWPCP_PORT_CMD          XWPCP_PORT(0)
#define XWPCP_PORT_QOS(p)       ((xwu8_t)((xwu8_t)(p) & XWPCP_QOS_MSK))

#define XWPCP_ID_MSK            (0x7FU)
#define XWPCP_ID(id)            ((xwu8_t)((xwu8_t)(id) & XWPCP_ID_MSK))
#define XWPCP_ID_SYNC           XWPCP_ID(0)
#define XWPCP_ID_ACK            BIT(7)

#define XWPCP_VERSION           ("2.0.0")
#define XWPCP_VERSION_MAJOR     2U
#define XWPCP_VERSION_MINOR     0U
#define XWPCP_VERSION_REVISION  0U

#define XWPCP_MEMPOOL_SIZE      (XWPCP_MEMBLK_SIZE * XWPCP_MEMBLK_NUM)
#define XWPCP_SDU_MAX_SIZE      (XWPCP_MEMPOOL_SIZE / 8)

#define XWPCP_FRM_MINSIZE       (sizeof(struct xwpcp_frmhead) + XWPCP_CHKSUM_SIZE)

#if defined(XWMDCFG_isc_xwpcp_LOG) && (1 == XWMDCFG_isc_xwpcp_LOG)
  #define XWPCP_LOG_TAG                 "xwpcp"
  #define xwpcplogf(lv, fmt, ...)       xwlogf(lv, XWPCP_LOG_TAG, fmt, ##__VA_ARGS__)
#else /* XWMDCFG_isc_xwpcp_LOG */
  #define xwpcplogf(lv, fmt, ...)
#endif /* !XWMDCFG_isc_xwpcp_LOG */

#define XWPCP_BUG()     XWOS_BUG()
#define XWPCP_BUG_ON(x) XWOS_BUG_ON(x)

#if defined(XWMDCFG_CHECK_PARAMETERS) && (1 == XWMDCFG_CHECK_PARAMETERS)
#define XWPCP_VALIDATE(exp, errstr, ...)         \
        if (__xwcc_unlikely(!(exp))) {           \
            return __VA_ARGS__;                  \
        }
#else /* XWMDCFG_CHECK_PARAMETERS */
#define XWPCP_VALIDATE(exp, errstr, ...)
#endif /* !XWMDCFG_CHECK_PARAMETERS */

/**
 * @brief 应答信号枚举
 */
enum xwpcp_frame_ack_em {
        XWPCP_ACK_OK = 0, /**< OK */
        XWPCP_ACK_ECONNRESET, /**< 链接被重置（远端复位？） */
        XWPCP_ACK_EALREADY, /**< 已经完成 */
        XWPCP_ACK_NUM, /**< ACK的数量 */
};

/**
 * @brief XWPCP帧的信息头
 */
struct __xwcc_packed xwpcp_frmhead {
        xwu8_t frmlen; /**< 帧的长度：信息头+消息帧+CRC32校验码的总长度 */
        xwu8_t mirror; /**< 帧的长度的镜像反转（xwbop_rbit8(frmlen)）：
                            作为frmlen的检验 */
        xwu8_t port; /**< bit7 QoS: 1:可靠消息；0:不可靠消息
                          bit6:0 端口，端口0为协议的内部使用的端口 */
        xwu8_t id; /**< bit6 ~ bit0:消息的ID；
                        bit7: 应答标志
                        ID0: 作为同步消息ID */
};

/**
 * @brief XWPCP的帧
 */
struct __xwcc_packed xwpcp_frame {
        xwu8_t sof; /* 帧首定界符（SOF:Start-of-Frame Delimiter） */
        struct xwpcp_frmhead head; /**< 信息头 */
        xwu8_t sdu[0]; /**< 报文 */
        /* CRC32 */
        /* eof 帧尾定界符（EOF:End-of-Frame Delimiter） */
};

/**
 * @brief XWPCP帧槽
 */
struct xwpcp_frmslot {
        xwpcp_ntf_f cbfunc; /**< 回调函数 */
        void * cbarg; /**< 回调函数的参数 */
        struct xwlib_bclst_node node; /**< 链表节点 */
        xwu8_t priority; /**< 帧优先级 */
        struct xwpcp_frame frm; /**< 帧 */
};

/**
 * @brief XWPCP实例的引用计数枚举
 */
enum xwpcp_refcnt_em {
        XWPCP_REFCNT_STOPPED = 0, /**< XWPCP已经停止 */
        XWPCP_REFCNT_STARTED, /**< XWPCP已经启动 */
        XWPCP_REFCNT_INUSED, /**< XWPCP正在被使用 */
};

/**
 * @breif XWPCP对象
 */
struct xwpcp {
        /* 基本信息 */
        const char * name; /**< 名字 */
        xwsq_a refcnt; /**< 引用计数 */
        xwsq_a hwifst; /**< 硬件层状态 */
        const struct xwpcp_hwifal_operations * hwifops; /**< 硬件接口抽象层操作函数 */
        void * hwifcb; /**< 接口硬件 */

        /* 帧槽内存池 */
        struct {
                struct xwmm_bma * pool; /**< 内存池 */
                xwu8_t __xwcc_aligned_l1cacheline mempool[XWPCP_MEMPOOL_SIZE];
                                        /**< 内存池 */
        } slot; /**< 帧槽 */

        /* 发送状态机 */
        xwid_t txtid; /**< 发送线程的ID */
        struct {
                xwu32_a cnt; /**< 发送计数器 */
                struct xwlib_bclst_head q[XWPCP_PRIORITY_NUM]; /**< 队列 */
                xwbmpop_declare(nebmp, XWPCP_PRIORITY_NUM); /**< 非空的索引位图 */
                struct xwos_splk lock; /**< 保护发送队列的自旋锁 */
                struct xwos_sem sem; /**< 指示待发送帧的数量的信号量 */
                struct xwos_mtx csmtx; /**< 保护发送和接收线程的共享数据的锁 */
                struct xwos_cond cscond; /**< 同步发送和接收线程的条件量 */
                union {
                        struct xwpcp_frmslot * sender; /**< 正在发送的帧槽 */
                        struct {
                                xwu8_t ack; /**< 远端回复的应答 */
                                xwu8_t id; /**< 远端的消息id */
                        } remote; /** 远端信息 */
                } txi; /**< 正在发送的帧信息 */
                struct xwpcp_frmslot * tmp; /**< 缓存正在发送的帧槽 */
                struct xwos_splk notiflk; /**< 保证通知回调函数原子性的自旋锁 */
        } txq; /**< 发送队列 */

        /* 接收状态机 */
        xwid_t rxtid; /**< 接收线程的ID */
        struct {
                xwu32_a cnt; /**< 接收计数器 */
                struct xwlib_bclst_head q[XWPCP_PORT_NUM]; /**< 每个端口的接收队列 */
                struct xwos_splk lock[XWPCP_PORT_NUM]; /**< 保护每个接收队列的锁 */
                struct xwos_sem sem[XWPCP_PORT_NUM]; /**< 每个接收队列的信号量 */
        } rxq; /**< 接收队列 */
};

struct xwpcp_frmslot * xwpcp_txq_choose(struct xwpcp * xwpcp);
xwer_t xwpcp_eq_msg(struct xwpcp * xwpcp,
                    const struct xwpcp_msg * msg,
                    xwu8_t prio,
                    xwpcp_ntf_f cbfunc, void * cbarg,
                    xwpcp_fhdl_t * fhdlbuf);
struct xwpcp_frmslot * xwpcp_rxq_choose(struct xwpcp * xwpcp, xwu8_t port);
xwer_t xwpcp_rxthrd(struct xwpcp * xwpcp);
xwer_t xwpcp_txthrd(struct xwpcp * xwpcp);
xwer_t xwpcp_prestop(struct xwpcp * xwpcp);
xwer_t xwpcp_grab(struct xwpcp * xwpcp);
xwer_t xwpcp_put(struct xwpcp * xwpcp);

#endif /* xwmd/isc/xwpcp/protocol.h */
