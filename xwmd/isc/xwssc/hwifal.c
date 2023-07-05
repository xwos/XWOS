/**
 * @file
 * @brief SOC间点对点通讯协议：硬件接口抽象层
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/xwlog.h>
#include <xwmd/isc/xwssc/hwifal.h>
#include <xwmd/isc/xwssc/protocol.h>

/**
 * @ingroup xwmd_isc_xwssc_hwifal
 * @{
 */

extern const uint8_t xwssc_frm_blank[8];

/**
 * @brief 打开硬件接口
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] hwifcb: 硬件接口控制块指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_hwifal_open(struct xwssc * xwssc, void * hwifcb)
{
        xwer_t rc;

        xwssc->hwifcb = hwifcb;
        if ((NULL != xwssc->hwifops) && (NULL != xwssc->hwifops->open)) {
                rc = xwssc->hwifops->open(xwssc);
        } else {
                rc = XWOK;
        }
        if (rc < 0) {
                goto err_hwifops_open;
        }

        xwaop_write(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_RX, NULL);
        xwssclogf(INFO, "[HWIFAL] Open HWIF ... [OK]\n");
        return rc;

err_hwifops_open:
        xwssc->hwifcb = NULL;
        xwssclogf(INFO, "[HWIFAL] Failed to open HWIF ... <rc:%d>\n", rc);
        return rc;
}

/**
 * @brief 关闭硬件接口
 * @param[in] xwssc: XWSSC对象的指针
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_hwifal_close(struct xwssc * xwssc)
{
        xwer_t rc;

        rc = xwaop_t1ma_then_c0m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_RX,
                                 NULL, NULL);
        if (rc < 0) {
                goto err_xwssc_clear_hwifst;
        }

        if ((NULL != xwssc->hwifops) && (NULL != xwssc->hwifops->close)) {
                rc = xwssc->hwifops->close(xwssc);
        } else {
                rc = XWOK;
        }
        if (rc < 0) {
                goto err_hwifops_close;
        }
        xwssc->hwifcb = NULL;
        xwssclogf(INFO, "[HWIFAL] close HWIF ... [OK]\n");
        return XWOK;

err_hwifops_close:
        xwaop_s1m(xwsq_t, &xwssc->hwifst, XWSSC_HWIFST_RX, NULL, NULL);
err_xwssc_clear_hwifst:
        xwssclogf(INFO, "[HWIFAL] Open HWIF ... <rc:%d>\n", rc);
        return rc;
}

/**
 * @brief 通过硬件接口发送数据
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] stream: 数据
 * @param[in] size: 数据长度
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_hwifal_tx(struct xwssc * xwssc, xwu8_t * stream, xwsz_t size)
{
        xwer_t rc;

        rc = xwssc->hwifops->tx(xwssc, stream, size);
        return rc;
}

/**
 * @brief 通过硬件接口接收一帧
 * @param[in] xwssc: XWSSC对象的指针
 * @param[out] slotbuf: 指向缓冲区的指针，通过此缓冲区返回union xwssc_slot *
 * @return 错误码
 */
__xwmd_code
xwer_t xwssc_hwifal_rx(struct xwssc * xwssc, union xwssc_slot ** slotbuf)
{
        xwu8_t delim;
        xwsz_t delimcnt;

        xwsz_t rxsize;
        xwsz_t total;
        xwssz_t rest;

        union {
                xwu8_t data[XWSSC_FRMHEAD_MAXSIZE];
                struct xwssc_frmhead head;
        } stream;
        xwu8_t headsize;
        xwu8_t headsize_mirror;
        bool headchk;
        xwsz_t ecsize;
        xwsz_t sdusize;

        xwsz_t need;
        xwsz_t neednum;
        xwssq_t odr;
        void * mem;
        xwu8_t * sdupos;
        union xwssc_slot * slot;
        xwer_t rc;

        /* 接收帧首定界符 */
        delimcnt = 0;
        do { // cppcheck-suppress [misra-c2012-15.4]
                rxsize = 1;
                rc = xwssc->hwifops->rx(xwssc, &delim, &rxsize);
                if (rc < 0) {
                        goto err_sof_ifrx;
                }
                if (XWSSC_SOF == delim) {
                        delimcnt++;
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWSSC_SOF_SIZE) {
                rc = -EAGAIN;
                goto err_sof_ifrx;
        }

        /* 接收Head */
        stream.head.headsize = delim;
        headsize = XWSSC_FRMHEAD_SIZE(stream.head.headsize);
        headsize_mirror = XWSSC_FRMHEAD_SIZE_MIRROR(stream.head.headsize);
        if (headsize_mirror != xwbop_rbit(xwu8_t, headsize)) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        total = headsize;
        rest = (xwssz_t)total - 1;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwssc->hwifops->rx(xwssc,
                                        &stream.data[total - rxsize],
                                        &rxsize);
                if (rc < 0) {
                        goto err_head_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);
        headchk = xwssc_chk_head((xwu8_t *)&stream.head, headsize);
        if (!headchk) {
                rc = -EAGAIN;
                goto err_head_ifrx;
        }
        xwssc_decode_sdusize(stream.head.ecsdusz, &sdusize);

        /* 申请接收帧槽 */
        ecsize = XWSSC_ECSIZE(&stream.head);
        need = sizeof(union xwssc_slot) + ecsize + sdusize +
               XWSSC_CRC32_SIZE + XWSSC_EOF_SIZE;
        neednum = XWBOP_DIV_ROUND_UP(need, XWSSC_MEMBLK_SIZE);
        odr = xwbop_fls(xwsz_t, neednum);
        if ((odr < 0) || ((XWSSC_MEMBLK_SIZE << odr) < need)) {
                odr++;
        }
        rc = xwmm_bma_alloc(xwssc->mempool, (xwsq_t)odr, &mem);
        if (rc < 0) {
                xwssc_tx_ack_sdu(xwssc,
                                 stream.head.port, stream.head.id,
                                 XWSSC_ACK_NOMEM);
                goto err_bma_alloc;
        }
        slot = mem; // cppcheck-suppress [misra-c2012-11.5]
        xwlib_bclst_init_node(&slot->rx.node);
        slot->rx.frmsize = sizeof(struct xwssc_frm) + ecsize + sdusize +
                           XWSSC_CRC32_SIZE + XWSSC_EOF_SIZE;
        // cppcheck-suppress [misra-c2012-17.7]
        memset(slot->rx.frm.sof, XWSSC_SOF, XWSSC_SOF_SIZE);
        // cppcheck-suppress [misra-c2012-17.7]
        memcpy(&slot->rx.frm.head, &stream.data,
               sizeof(struct xwssc_frmhead) + ecsize);
        sdupos = &slot->rx.frm.head.ecsdusz[ecsize];

        /* 接收body */
        total = sdusize + XWSSC_CRC32_SIZE;
        rest = (xwssz_t)total;
        do {
                rxsize = (xwsz_t)rest;
                rc = xwssc->hwifops->rx(xwssc,
                                        &sdupos[total - rxsize],
                                        &rxsize);
                if (rc < 0) {
                        goto err_body_ifrx;
                }
                rest -= (xwssz_t)rxsize;
        } while (rest > 0);

        /* 接收帧尾定界符 */
        delimcnt = 0;
        do { // cppcheck-suppress [misra-c2012-15.4]
                rxsize = 1;
                rc = xwssc->hwifops->rx(xwssc, &delim, &rxsize);
                if (rc < 0) {
                        goto err_eof_ifrx;
                }
                if (XWSSC_EOF == delim) {
                        delimcnt++;
                        if (delimcnt >= XWSSC_EOF_SIZE) {
                                break;
                        }
                } else {
                        break;
                }
        } while (true);
        if (delimcnt < XWSSC_EOF_SIZE) {
                rc = -EAGAIN;
                goto err_eof_ifrx;
        }
        // cppcheck-suppress [misra-c2012-17.7]
        memset(&sdupos[sdusize + XWSSC_CRC32_SIZE], XWSSC_EOF, XWSSC_EOF_SIZE);

        *slotbuf = slot;
        return XWOK;

err_eof_ifrx:
err_body_ifrx:
        xwmm_bma_free(xwssc->mempool, mem);
err_bma_alloc:
err_head_ifrx:
err_sof_ifrx:
        return rc;
}

/**
 * @brief 通知来自协议层的事件到硬件接口层
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] evt: 事件
 */
__xwmd_code
void xwssc_hwifal_notify(struct xwssc * xwssc, xwsq_t evt)
{
        if (XWSSC_HWIFNTF_NETUNREACH == evt) {
                xwssc->hwifops->tx(xwssc, xwssc_frm_blank, sizeof(xwssc_frm_blank));
        }
        if (NULL != xwssc->hwifops->notify) {
                xwssc->hwifops->notify(xwssc, evt);
        }
}

/**
 * @} xwmd_isc_xwssc_hwifal
 */
