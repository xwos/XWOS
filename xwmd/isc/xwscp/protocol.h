/**
 * @file
 * @brief 精简的点对点通讯协议：协议层
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_xwscp_protocol_h__
#define __xwmd_xwscp_protocol_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwlog.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/lock/mutex.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/isc/xwscp/hwifal.h>
#include <xwmd/isc/xwscp/api.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      macros       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define SOSCP_PERIOD            (XWMDCFG_isc_xwscp_PERIOD)
#define SOSCP_RETRY_NUM         (XWMDCFG_isc_xwscp_RETRY_NUM)
#define SOSCP_SDU_MAX_SIZE      (XWMDCFG_isc_xwscp_SDU_MAX_SIZE)
#define SOSCP_FRMSLOT_NUM       (XWMDCFG_isc_xwscp_FRMSLOT_NUM + 1)
#define SOSCP_SYNC_KEY          (XWMDCFG_isc_xwscp_SYNC_KEY)

#define SOSCP_CHKSUM_SIZE        (4U)
#define SOSCP_VERSION            ("1.0")

#define SOSCP_PORT_MSK          (0x7FU)
#define SOSCP_PORT(port)        ((port) & SOSCP_PORT_MSK)
#define SOSCP_PORT_CMD          SOSCP_PORT(0)
#define SOSCP_PORT_DATA         SOSCP_PORT(1)
#define SOSCP_PORT_QOS(port)    ((xwu8_t)((xwu8_t)(port) & SOSCP_QOS(1)))
#define SOSCP_QOS(q)            ((xwu8_t)((q) << 7))

#define SOSCP_ID_MSK            (0x7FU)
#define SOSCP_ID(id)            ((xwu8_t)((xwu8_t)(id) & SOSCP_ID_MSK))
#define SOSCP_ID_SYNC           SOSCP_ID(0)
#define SOSCP_ID_ACK            BIT(7)

/**
 * @brief 调试SOSCP的日志函数
 */
#if defined(XWMDCFG_isc_xwscp_LOG) && (1 == XWMDCFG_isc_xwscp_LOG)
  #define xwscplogf(lv, fmt, ...)        xwlogf(lv, fmt, ##__VA_ARGS__)
#else /* XWMDCFG_isc_xwscp_LOG */
  #define xwscplogf(lv, fmt, ...)
#endif /* !XWMDCFG_isc_xwscp_LOG */

#if defined(XWMDCFG_CHECK_PARAMETERS) && (1 == XWMDCFG_CHECK_PARAMETERS)
#define SOSCP_VALIDATE(exp, errstr, ...)         \
        if (__unlikely(!(exp))) {               \
            return __VA_ARGS__;                 \
        }
#else /* XWMDCFG_CHECK_PARAMETERS */
#define SOSCP_VALIDATE(exp, errstr, ...)
#endif /* !XWMDCFG_CHECK_PARAMETERS */

#define XWSCP_BUG()     XWOS_BUG()
#define XWSCP_BUG_ON(x) XWOS_BUG_ON(x)

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 应答信号枚举
 */
enum xwscp_ack_em {
        SOSCP_ACK_OK = 0,  /**< 正确 */
        SOSCP_ACK_ESIZE, /**< 大小错误 */
        SOSCP_ACK_EALREADY,  /**< 帧重复 */
        SOSCP_ACK_ECONNRESET,  /**< 链接被服务 */
        SOSCP_ACK_NUM, /**< 应答信号的数量 */
};

/**
 * @brief SOSCP帧的信息头
 */
struct __packed xwscp_frmhead {
        xwu8_t frmlen; /**< 帧的长度：信息头+消息帧+CRC32校验码的总长度 */
        xwu8_t mirror; /**< 帧的长度的镜像反转（xwbop_rbit8(frmlen)）：
                            作为frmlen的检验 */
        xwu8_t port; /**< bit7: QoS，1:可靠消息；0:不可靠消息
                          bit6 ~ bit0: 端口，
                                       SOSCP_PORT_DATA 数据
                                       SOSCP_PORT_CMD 控制命令 */
        xwu8_t id; /**< bit6 ~ bit0:消息的ID；
                        bit7: 应答标志
                        ID0: 作为同步消息ID */
};

/**
 * @brief SOSCP的帧
 */
struct __packed xwscp_frame {
        xwu8_t sof; /* 帧首定界符（SOF:Start-of-Frame Delimiter） */
        struct xwscp_frmhead head; /**< 信息头 */
        xwu8_t sdu[SOSCP_SDU_MAX_SIZE]; /**< 报文 */
        xwu8_t crc32[SOSCP_CHKSUM_SIZE]; /**< CRC32校验码，大端格式 */
        xwu8_t eof; /* 帧尾定界符（EOF:End-of-Frame Delimiter） */
};

/**
 * @brief SOSCP帧槽
 */
struct __aligned(XWMMCFG_ALIGNMENT) xwscp_frmslot {
        struct xwlib_bclst_node node; /**< 链表节点 */
        struct xwscp_frame frm; /**< 帧 */
};

/**
 * @breif SOSCP对象
 */
struct xwscp {
        /* 基本信息 */
        const char * name; /**< 名字 */
        __atomic xwsq_t hwifst; /**< 硬件接口抽象层状态 */
        const struct xwscp_hwifal_operations * hwifops; /**< 硬件接口抽象层操作函数 */
        void * hwifcb; /**< 物理层端口 */

        /* 发送状态机 */
        struct xwosal_mtx txmtx; /**< 发送锁 */
        struct xwosal_mtx csmtx; /**< 保护发送和接收线程的共享数据的锁 */
        struct xwosal_cdt cscdt; /**< 同步发送和接收线程的条件量 */
        struct {
                __atomic xwu32_t cnt; /**< 发送计数器 */
                struct xwscp_frame * frm; /**< 正在发送的帧 */
                xwer_t ack; /**< 应答代码 */
        } txi; /**< 正在发送的帧信息 */

        /* 接收状态机 */
        struct {
                __atomic xwu32_t cnt; /**< 接收计数器 */
                struct xwlib_bclst_head head; /**< 链表头 */
                struct xwosal_splk lock; /**< 保护接收队列的自旋锁 */
                struct xwosal_smr smr; /**< 接收队列的信号量 */
        } rxq; /**< 接收队列 */

        /* 帧槽内存池 */
        struct {
                struct xwosal_smr smr; /**< 空的帧槽计数信号量 */
                struct xwlib_bclst_head q; /**< 帧槽链表队列头 */
                struct xwscp_frmslot frm[SOSCP_FRMSLOT_NUM]; /**< 帧槽数组 */
                struct xwosal_splk lock; /**< 保护帧槽队列的自旋锁 */
        } slot;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwmd_code
xwer_t xwscp_alloc_frmslot(struct xwscp * xwscp, struct xwscp_frmslot ** slotbuf);

__xwmd_code
void xwscp_free_frmslot(struct xwscp * xwscp, struct xwscp_frmslot * frmslot);

__xwmd_code
xwer_t xwscp_tx_cfrm_sync(struct xwscp * xwscp, xwtm_t * xwtm);

__xwmd_code
xwer_t xwscp_fmt_msg(struct xwscp * xwscp, struct xwscp_frame * frm,
                     const xwu8_t sdu[], xwu8_t size);

__xwmd_code
void xwscp_rxq_pub(struct xwscp * xwscp, struct xwscp_frmslot * pubfrm);

__xwmd_code
xwer_t xwscp_rxq_choose(struct xwscp * xwscp, struct xwscp_frmslot ** slotbuf,
                        xwtm_t * xwtm);

__xwmd_code
xwer_t xwscp_thrd(struct xwscp * xwscp);

#endif /* xwmd/isc/xwscp/protocol.h */
