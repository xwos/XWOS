/**
 * @file
 * @brief xwmd::isc::xwioc 操作系统间通讯协议
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwbop.h>
#include "xwmd/isc/xwioc/mi.h"

#include <xwos/lib/xwlog.h>
#define LOGTAG "xwioc"

/* #define XWIOC_DBG */
#if defined(XWIOC_DBG)
#  define xwioclogd(fmt, ...) xwlogf(D, LOGTAG, fmt, ##__VA_ARGS__)
#else
#  define xwioclogd(fmt, ...)
#endif
#define xwioclogi(fmt, ...) xwlogf(I, LOGTAG, fmt, ##__VA_ARGS__)
#define xwiocloge(fmt, ...) xwlogf(E, LOGTAG, fmt, ##__VA_ARGS__)

/**
 * @addtogroup xwmd_isc_xwioc
 * @{
 */

xwer_t xwioc_init(struct xwioc * xwioc, xwu64_t shm_origin,
                  xwu64_t cq_mr_offset, xwu64_t cq_mr_size,
                  xwu64_t block_mr_offset, xwu64_t block_mr_size,
                  xwu64_t order, xwioc_sgi_f sgi)
{
        xwer_t rc;
        xwu64_t i;
        xwu64_t j;
        xwu64_t cqm_size;

        if (cq_mr_size < sizeof(struct xwioc_cq) * 2UL) {
                rc = -ENOMEM;
                goto err_nomem;
        }
        xwioc->shm_origin = shm_origin;
        xwioc->cq_mr_offset = cq_mr_offset;
        xwioc->cq_mr_size = cq_mr_size;
        xwioc->block_mr_offset = block_mr_offset;
        xwioc->block_mr_size = block_mr_size;
        xwioc->sgi = sgi;
        if (0UL == order) {
                xwioc->txcq = (struct xwioc_cq *)(shm_origin + cq_mr_offset);
                xwioc->rxcq = &xwioc->txcq[1];
        } else {
                xwioc->rxcq = (struct xwioc_cq *)(shm_origin + cq_mr_offset);
                xwioc->txcq = &xwioc->rxcq[1];
        }
        cqm_size = (block_mr_size / (XWMDCFG_isc_xwioc_PORT_NUM * 2UL));
        for (i = 0; i < XWMDCFG_isc_xwioc_PORT_NUM; i++) {
                xwos_sem_init(&xwioc->rxsem[i], 0, XWSSQ_MAX);
        }
        xwioclogd("shm_origin: 0x%lX\r\n", xwioc->shm_origin);
        xwioclogd("cq_mr_origin: 0x%lX, cq_mr_size: %ld\r\n",
                  xwioc->shm_origin + xwioc->cq_mr_offset, xwioc->cq_mr_size);
        xwioclogd("block_mr_origin: 0x%lX, block_mr_size: %ld\r\n",
                  xwioc->shm_origin + xwioc->block_mr_offset, xwioc->block_mr_size);

        xwos_splk_init(&xwioc->txcq->splk);
        xwioc->txcq->port_num = XWMDCFG_isc_xwioc_PORT_NUM;
        xwioc->txcq->portmap = 0UL;
        for (i = 0, j = 0; i < XWMDCFG_isc_xwioc_PORT_NUM; i++, j++) {
                xwioc->txcq->port[i].extra_memory.origin = 0;
                xwioc->txcq->port[i].extra_memory.size = 0;
                xwioc->txcq->port[i].cq.size = cqm_size;
                xwioc->txcq->port[i].cq.shadow = XWIOC_SHADOW_SIZE;
                xwioc->txcq->port[i].cq.num = cqm_size / XWIOC_BLOCK_SIZE;
                xwioc->txcq->port[i].cq.num -= (XWIOC_SHADOW_SIZE / XWIOC_BLOCK_SIZE);
                xwioc->txcq->port[i].cq.pos = 0UL;
                xwioc->txcq->port[i].cq.tail = 0UL;
                xwioc->txcq->port[i].cq.m = cqm_size * j;
                xwioc->txcq->port[i].connection = 0UL;
        }

        xwos_splk_init(&xwioc->rxcq->splk);
        xwioc->rxcq->port_num = XWMDCFG_isc_xwioc_PORT_NUM;
        xwioc->rxcq->portmap = 0UL;
        for (i = 0; i < XWMDCFG_isc_xwioc_PORT_NUM; i++, j++) {
                xwioc->rxcq->port[i].extra_memory.origin = 0;
                xwioc->rxcq->port[i].extra_memory.size = 0;
                xwioc->rxcq->port[i].cq.size = cqm_size;
                xwioc->rxcq->port[i].cq.shadow = XWIOC_SHADOW_SIZE;
                xwioc->rxcq->port[i].cq.num = cqm_size / XWIOC_BLOCK_SIZE;
                xwioc->rxcq->port[i].cq.num -= (XWIOC_SHADOW_SIZE / XWIOC_BLOCK_SIZE);
                xwioc->rxcq->port[i].cq.pos = 0UL;
                xwioc->rxcq->port[i].cq.tail = 0UL;
                xwioc->rxcq->port[i].cq.m = cqm_size * j;
                xwioc->rxcq->port[i].connection = 0UL;
        }

        return XWOK;

err_nomem:
        return rc;
}

void xwioc_set_txcq_extra_memory(struct xwioc * xwioc, xwu64_t port,
                                 void * origin, xwu64_t size)
{
        if (port < xwioc->txcq->port_num) {
                if ((NULL == origin) && (0UL == size)) {
                        xwioc->txcq->port[port].extra_memory.origin = 0UL;
                        xwioc->txcq->port[port].extra_memory.size = 0UL;
                } else if ((xwu64_t)origin >= xwioc->shm_origin) {
                        xwioc->txcq->port[port].extra_memory.origin =
                                ((xwu64_t)origin - xwioc->shm_origin);
                        xwioc->txcq->port[port].extra_memory.size = size;
                } else {}
        }
}

void xwioc_set_rxcq_extra_memory(struct xwioc * xwioc, xwu64_t port,
                                 void * origin, xwu64_t size)
{
        if (port < xwioc->rxcq->port_num) {
                if ((NULL == origin) && (0UL == size)) {
                        xwioc->rxcq->port[port].extra_memory.origin = 0UL;
                        xwioc->rxcq->port[port].extra_memory.size = 0UL;
                } else if ((xwu64_t)origin >= xwioc->shm_origin) {
                        xwioc->rxcq->port[port].extra_memory.origin =
                                ((xwu64_t)origin - xwioc->shm_origin);
                        xwioc->rxcq->port[port].extra_memory.size = size;
                } else {}
        }
}

xwer_t xwioc_connect(struct xwioc * xwioc, xwu64_t port)
{
        xwreg_t cpuirq;
        xwer_t rc;

        if (NULL == xwioc) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == xwioc->txcq) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == xwioc->rxcq) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (port >= XWMDCFG_isc_xwioc_PORT_NUM) {
                rc = -ECHRNG;
                goto err_outofrange;
        }
        xwos_splk_lock_cpuirqsv(&xwioc->txcq->splk, &cpuirq);
        xwioc->txcq->port[port].connection |= XWIOC_SOCKET_TX_POS;
        xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);

        xwos_splk_lock_cpuirqsv(&xwioc->rxcq->splk, &cpuirq);
        xwioc->rxcq->port[port].connection |= XWIOC_SOCKET_RX_POS;
        xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);

        return XWOK;

err_outofrange:
err_fault:
        return rc;
}

xwer_t xwioc_disconnect(struct xwioc * xwioc, xwu64_t port)
{
        xwreg_t cpuirq;
        xwer_t rc;

        if (NULL == xwioc) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == xwioc->txcq) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == xwioc->rxcq) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (port >= XWMDCFG_isc_xwioc_PORT_NUM) {
                rc = -ECHRNG;
                goto err_outofrange;
        }
        xwos_splk_lock_cpuirqsv(&xwioc->txcq->splk, &cpuirq);
        xwioc->txcq->port[port].connection &= ~XWIOC_SOCKET_TX_POS;
        xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);

        xwos_splk_lock_cpuirqsv(&xwioc->rxcq->splk, &cpuirq);
        xwioc->rxcq->port[port].connection &= ~XWIOC_SOCKET_RX_POS;
        xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);

        return XWOK;

err_outofrange:
err_fault:
        return rc;
}

bool xwioc_tst_txcq_connected(struct xwioc * xwioc, xwu64_t port)
{
        xwreg_t cpuirq;
        bool rc;

        if (NULL == xwioc) {
                rc = false;
                goto err_fault;
        }
        if (NULL == xwioc->txcq) {
                rc = false;
                goto err_fault;
        }
        if (port >= XWMDCFG_isc_xwioc_PORT_NUM) {
                rc = false;
                goto err_outofrange;
        }
        xwos_splk_lock_cpuirqsv(&xwioc->txcq->splk, &cpuirq);
        rc = !!(XWIOC_SOCKET_CONNECTED == xwioc->txcq->port[port].connection);
        xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);

err_outofrange:
err_fault:
        return rc;
}

bool xwioc_tst_rxcq_connected(struct xwioc * xwioc, xwu64_t port)
{
        xwreg_t cpuirq;
        bool rc;

        if (NULL == xwioc) {
                rc = false;
                goto err_fault;
        }
        if (NULL == xwioc->rxcq) {
                rc = false;
                goto err_fault;
        }
        if (port >= XWMDCFG_isc_xwioc_PORT_NUM) {
                rc = false;
                goto err_outofrange;
        }
        xwos_splk_lock_cpuirqsv(&xwioc->rxcq->splk, &cpuirq);
        rc = !!(XWIOC_SOCKET_CONNECTED == xwioc->rxcq->port[port].connection);
        xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);

err_outofrange:
err_fault:
        return rc;
}

xwer_t xwioc_write(struct xwioc * xwioc, xwu64_t port,
                   const xwu8_t sdu[], xwsz_t * dlc)
{
        xwreg_t cpuirq;
        xwu64_t num;
        xwu64_t pos;
        xwu64_t tail;
        xwioc_block_t * m;
        xwsz_t wr_size;
        xwu64_t wr_block;
        xwu64_t empty;
        xwu64_t cursor;
        struct xwioc_cq_protocol * msg;
        xwer_t rc;

        if (NULL == xwioc) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == xwioc->txcq) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == dlc) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (port >= xwioc->txcq->port_num) {
                rc = -ECHRNG;
                goto err_outofrange;
        }
        if (*dlc > XWIOC_SHADOW_SIZE) {
                rc = -EMSGSIZE;
                goto err_msgsize;
        }

        m = (xwioc_block_t *)(xwioc->shm_origin + xwioc->block_mr_offset +
                              xwioc->txcq->port[port].cq.m);
        wr_size = *dlc + sizeof(struct xwioc_cq_protocol_head);
        wr_block = ((wr_size + XWIOC_BLOCK_SIZE - 1UL) / XWIOC_BLOCK_SIZE);
        xwos_splk_lock_cpuirqsv(&xwioc->txcq->splk, &cpuirq);
        if (XWIOC_SOCKET_CONNECTED != xwioc->txcq->port[port].connection) {
                xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);
                rc = -ENOTCONN;
                goto err_notconn;
        }
        num = xwioc->txcq->port[port].cq.num;
        pos = xwioc->txcq->port[port].cq.pos;
        tail = xwioc->txcq->port[port].cq.tail;
        if (tail >= pos) {
                cursor = tail;
                tail += wr_block;
                if (tail >= num) {
                        tail = 0UL;
                        if (tail >= pos) {
                                xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);
                                rc = -ENOSPC;
                                goto err_nospc;
                        }
                }
                xwioc->txcq->port[port].cq.tail = tail;
                msg = (struct xwioc_cq_protocol *)&m[cursor];
                msg->head.sof = XWIOC_PROTOCOL_HEAD_SOF;
                msg->head.dlc = *dlc;
                memcpy(msg->sdu, sdu, *dlc);
                xwioc->txcq->portmap |= (1UL << port);
                xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);
        } else {
                empty = pos - tail;
                if (wr_block >= empty) {
                        rc = ENOSPC;
                        xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);
                        goto err_msgsize;
                }
                cursor = tail;
                tail += wr_block;
                xwioc->txcq->port[port].cq.tail = tail;
                msg = (struct xwioc_cq_protocol *)&m[cursor];
                msg->head.sof = XWIOC_PROTOCOL_HEAD_SOF;
                msg->head.dlc = *dlc;
                memcpy(msg->sdu, sdu, *dlc);
                xwioc->txcq->portmap |= (1UL << port);
                xwos_splk_unlock_cpuirqrs(&xwioc->txcq->splk, cpuirq);
        }
        if (NULL != xwioc->sgi) {
                xwioc->sgi();
        }
        return XWOK;

err_nospc:
err_notconn:
err_msgsize:
err_outofrange:
err_fault:
        return rc;
}

void xwioc_notify(struct xwioc * xwioc)
{
        xwu64_t i;
        xwreg_t cpuirq;

        xwos_splk_lock_cpuirqsv(&xwioc->rxcq->splk, &cpuirq);
        for (i = 0UL; i < xwioc->rxcq->port_num; i++) {
                if (xwioc->rxcq->portmap & (1UL << i)) {
                        xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);
                        xwos_sem_post(&xwioc->rxsem[i]);
                        xwos_splk_lock_cpuirqsv(&xwioc->rxcq->splk, &cpuirq);
                }
        }
        xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);
}

xwer_t xwioc_read(struct xwioc * xwioc, xwu64_t port,
                  xwu8_t sdu[], xwsz_t * dlc, xwtm_t to)
{
        xwreg_t cpuirq;
        xwu64_t num;
        xwu64_t pos;
        xwu64_t tail;
        xwioc_block_t * m;
        xwsz_t rd_size;
        xwu64_t rd_block;
        struct xwioc_cq_protocol * msg;
        xwer_t rc;

        if (NULL == xwioc) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == xwioc->rxcq) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == sdu) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (NULL == dlc) {
                rc = -EFAULT;
                goto err_fault;
        }
        if (port >= xwioc->rxcq->port_num) {
                rc = -ECHRNG;
                goto err_outofrange;
        }

        m = (xwioc_block_t *)(xwioc->shm_origin + xwioc->block_mr_offset +
                              xwioc->rxcq->port[port].cq.m);
        do {
                xwos_splk_lock_cpuirqsv(&xwioc->rxcq->splk, &cpuirq);
                num = xwioc->rxcq->port[port].cq.num;
                pos = xwioc->rxcq->port[port].cq.pos;
                tail = xwioc->rxcq->port[port].cq.tail;
                if (pos == tail) {
                        xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);
                        rc = xwos_sem_wait_to(&xwioc->rxsem[port], to);
                        if (rc < 0) {
                                goto err_sem_wait;
                        }
                } else {
                        rc = 1;
                }
        } while (XWOK == rc);
        msg = (struct xwioc_cq_protocol *)&m[pos];
        if (XWIOC_PROTOCOL_HEAD_SOF != msg->head.sof) {
                rc = -EPROTO;
                xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);
                goto err_proto;
        }
        rd_size = msg->head.dlc + sizeof(struct xwioc_cq_protocol_head);
        rd_block = ((rd_size + XWIOC_BLOCK_SIZE - 1UL) / XWIOC_BLOCK_SIZE);
        if (msg->head.dlc > *dlc) {
                rc = -ENOBUFS;
                xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);
                goto err_size;
        }
        memcpy(sdu, msg->sdu, msg->head.dlc);
        *dlc = msg->head.dlc;
        pos += rd_block;
        if (pos >= num) {
                pos = 0UL;
        }
        xwioc->rxcq->port[port].cq.pos = pos;
        if (pos == tail) {
                xwioc->rxcq->portmap &= ~(1UL << port);
        }
        xwos_splk_unlock_cpuirqrs(&xwioc->rxcq->splk, cpuirq);
        return XWOK;

err_size:
err_proto:
err_sem_wait:
err_outofrange:
err_fault:
        return rc;
}

/**
 * @} xwmd_isc_xwioc
 */
