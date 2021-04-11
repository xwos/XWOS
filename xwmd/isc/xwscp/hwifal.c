/**
 * @file
 * @brief 精简的点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwmd/isc/xwscp/protocol.h>
#include <xwmd/isc/xwscp/hwifal.h>

/**
 * @brief 空白帧
 */
__xwmd_rodata const xwu8_t xwscp_frm_blank[] = {
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
        (xwu8_t)XWSCP_HWIFAL_EOF,
};

/**
 * @brief 打开硬件接口
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_open(struct xwscp * xwscp, void * hwifcb)
{
        xwer_t rc;

        xwscp->hwifcb = hwifcb;
        if (__xwcc_likely((xwscp->hwifops) && (xwscp->hwifops->open))) {
                rc = xwscp->hwifops->open(xwscp);
        } else {
                rc = XWOK;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_hwifops_open;
        }

        xwaop_write(xwsq, &xwscp->hwifst, XWSCP_HWIFST_RX, NULL);
        xwscplogf(INFO, "Open HWIF ... [OK]\n");
        return rc;

err_hwifops_open:
        xwscp->hwifcb = NULL;
        xwscplogf(INFO, "Failed to open HWIF ... [rc:%d]\n", rc);
        return rc;
}

/**
 * @brief 关闭硬件接口
 * @param xwscp: (I) XWSCP对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_close(struct xwscp * xwscp)
{
        xwer_t rc;

        rc = xwaop_t1ma_then_c0m(xwsq, &xwscp->hwifst, XWSCP_HWIFST_RX,
                                 NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwscp_clear_hwifst;
        }

        if ((xwscp->hwifops) && (xwscp->hwifops->close)) {
                rc = xwscp->hwifops->close(xwscp);
        } else {
                rc = XWOK;
        }
        if (__xwcc_unlikely(rc < 0)) {
                goto err_hwifops_close;
        }
        xwscp->hwifcb = NULL;
        xwscplogf(INFO, "close HWIF ... [OK]\n");
        return XWOK;

err_hwifops_close:
        xwaop_s1m(xwsq, &xwscp->hwifst, XWSCP_HWIFST_RX, NULL, NULL);
err_xwscp_clear_hwifst:
        xwscplogf(INFO, "Open HWIF ... [rc:%d]\n", rc);
        return rc;
}

/**
 * @brief 通过硬件接口发送数据
 * @param xwscp: (I) XWSCP对象的指针
 * @param stream: (I) 数据
 * @param size: (I) 数据长度
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_tx(struct xwscp * xwscp, xwu8_t * stream, xwsz_t size)
{
        xwer_t rc;

        rc = xwscp->hwifops->tx(xwscp, stream, size);
        return rc;
}

/**
 * @brief 通过硬件接口接收一帧
 * @param xwscp: (I) XWSCP对象的指针
 * @param slotbuf: (O) 指向缓冲区的指针，通过此缓冲区返回union xwscp_slot *
 * @return 错误码
 */
__xwmd_code
xwer_t xwscp_hwifal_rx(struct xwscp * xwscp, union xwscp_slot ** slotbuf)
{
        xwu8_t delim;
        xwsz_t delimcnt;

        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;

        union {
                xwu8_t data[XWSCP_FRMHEAD_MAXSIZE];
                struct xwscp_frmhead head;
        } stream;
        xwu8_t headsize;
        xwu8_t headsize_mirror;
        bool headchk;
        xwsz_t ecsize, sdusize;

        xwsz_t need, neednum;
        xwssq_t odr;
        void * mem;
        xwu8_t * sdupos;
        union xwscp_slot * slot;
        xwer_t rc;

        /* 接收帧首定界符 */
        delimcnt = 0;
        do {
                rxsize = 1;
                rc = xwscp->hwifops->rx(xwscp, &delim, &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_sof_ifrx;
                }
                if (XWSCP_SOF == delim) {
                        delimcnt++;
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWSCP_SOF_SIZE) {
                rc = -EAGAIN;
                goto err_sof_ifrx;
        }

        /* 接收Head */
        stream.head.headsize = delim;
        headsize = XWSCP_FRMHEAD_SIZE(stream.head.headsize);
        headsize_mirror = XWSCP_FRMHEAD_SIZE_MIRROR(stream.head.headsize);
        if (headsize_mirror != xwbop_rbit(xwu8_t, headsize)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        total = headsize;
        rest = (xwssz_t)total - 1;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwscp->hwifops->rx(xwscp,
                                        &stream.data[total - rxsize],
                                        &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_head_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);
        headchk = xwscp_chk_head((xwu8_t *)&stream.head, headsize);
        if (!headchk) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        xwscp_decode_sdusize(stream.head.ecsdusz, &sdusize);

        /* 申请接收帧槽 */
        ecsize = XWSCP_ECSIZE(&stream.head);
        need = sizeof(union xwscp_slot) + ecsize + sdusize +
               XWSCP_CRC32_SIZE + XWSCP_EOF_SIZE;
        neednum = XWBOP_DIV_ROUND_UP(need, XWSCP_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWSCP_MEMBLK_SIZE << odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwscp->mempool, (xwsq_t)odr, &mem);
        if (__xwcc_unlikely(rc < 0)) {
                xwscp_tx_sdu_ack(xwscp, stream.head.id, XWSCP_ACK_NOMEM);
                goto err_bma_alloc;
        }
        slot = mem;
        xwlib_bclst_init_node(&slot->rx.node);
        slot->rx.frmsize = sizeof(struct xwscp_frm) + ecsize + sdusize +
                           XWSCP_CRC32_SIZE + XWSCP_EOF_SIZE;
        memset(slot->rx.frm.sof, XWSCP_SOF, XWSCP_SOF_SIZE);
        memcpy(&slot->rx.frm.head, &stream.data,
               sizeof(struct xwscp_frmhead) + ecsize);
        sdupos = &slot->rx.frm.head.ecsdusz[ecsize];

        /* 接收body */
        total = sdusize + XWSCP_CRC32_SIZE;
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwscp->hwifops->rx(xwscp,
                                        &sdupos[total - rxsize],
                                        &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_body_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);

        /* 接收帧尾定界符 */
        delimcnt = 0;
        do {
                rxsize = 1;
                rc = xwscp->hwifops->rx(xwscp, &delim, &rxsize);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_eof_ifrx;
                }
                if (XWSCP_EOF == delim) {
                        delimcnt++;
                        if (delimcnt >= XWSCP_EOF_SIZE) {
                                break;
                        }
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWSCP_EOF_SIZE) {
                rc = -EAGAIN;
                goto err_eof_ifrx;
        }
        memset(&sdupos[sdusize + XWSCP_CRC32_SIZE], XWSCP_EOF, XWSCP_EOF_SIZE);

        *slotbuf = slot;
        return XWOK;

err_eof_ifrx:
err_body_ifrx:
        xwmm_bma_free(xwscp->mempool, mem);
err_bma_alloc:
err_head_ifrx:
err_sof_ifrx:
        return rc;
}

/**
 * @brief 通知协议层的事件到硬件接口层
 * @param xwscp: (I) XWSCP对象的指针
 * @param evt: (I) 事件
 */
__xwmd_code
void xwscp_hwifal_notify(struct xwscp * xwscp, xwsq_t evt)
{
        if (XWSCP_HWIFNTF_NETUNREACH == evt) {
                xwscp->hwifops->tx(xwscp, xwscp_frm_blank, sizeof(xwscp_frm_blank));
        }
        if (xwscp->hwifops->notify) {
                xwscp->hwifops->notify(xwscp, evt);
        }
}
