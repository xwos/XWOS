/**
 * @file
 * @brief SOC间点对点通讯协议：用户应用接口层
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
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbmpaop.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/bma.h>
#include <xwos/osal/thd.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwmd/isc/xwssc/hwifal.h>
#include <xwmd/isc/xwssc/protocol.h>
#include <xwmd/isc/xwssc/mif.h>

/**
 * @ingroup xwmd_isc_xwssc_mif
 * @{
 */

#define XWSSC_TXTHD_PRIORITY XWMDCFG_isc_xwssc_TXTHD_PRIORITY
#define XWSSC_RXTHD_PRIORITY XWMDCFG_isc_xwssc_RXTHD_PRIORITY

static __xwmd_code
void xwssc_txcb_notify(struct xwssc * xwssc, xwssc_txh_t txh, xwer_t rc, void * arg);

static __xwmd_code
xwer_t xwssc_gc(struct xwos_object * obj);

__xwmd_api
xwer_t xwssc_start(struct xwssc * xwssc, const char * name,
                   const struct xwssc_hwifal_operation * hwifops, void * hwifcb,
                   xwu8_t * mem, xwsz_t memsize)
{
        struct xwos_thd_attr attr;
        xwer_t rc;
        xwssq_t i;
        xwssq_t j;

        XWSSC_VALIDATE((NULL != xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != hwifops), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != hwifops->tx), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != hwifops->rx), "nullptr", -EFAULT);
        // cppcheck-suppress [misra-c2012-15.5]
        XWSSC_VALIDATE_FORCE((NULL != mem), "nullptr", -EFAULT);
        // cppcheck-suppress [misra-c2012-15.5]
        XWSSC_VALIDATE_FORCE((XWSSC_MEMPOOL_SIZE == memsize),
                             "memsize-error",
                             -ESIZE);

        xwssclogf(DEBUG, "[API] Starting XWSSC-%s ...\n", XWSSC_VERSION);

        xwos_object_construct(&xwssc->xwobj);
        xwssc->hwifst = XWSSC_HWIFST_CLOSED;
        xwssc->hwifcb = NULL;
        xwssc->name = name;
        xwssc->hwifops = hwifops;
        xwssc->txthd = XWOS_THD_NILD;
        xwssc->rxthd = XWOS_THD_NILD;
        xwssc->mempool = NULL;
        rc = xwos_object_activate(&xwssc->xwobj, xwssc_gc);
        if (rc < 0) {
                xwssclogf(ERR, "[API] Activate xwssc ... <rc:%d>\n", rc);
                goto err_xwobj_activate; // cppcheck-suppress [misra-c2012-15.2]
        }

        /* 创建内存池 */
        xwssc->mempool = (struct xwmm_bma *)&xwssc->mempool_bma_raw;
        rc = xwmm_bma_init(xwssc->mempool, "xwssc.mempool",
                           (xwptr_t)mem,
                           memsize, XWSSC_MEMBLK_SIZE, XWSSC_MEMBLK_ODR);
        if (rc < 0) {
                xwssclogf(ERR, "[API] Init BMA ... <rc:%d>\n", rc);
                goto err_bma_init; // cppcheck-suppress [misra-c2012-15.2]
        }
        xwssclogf(DEBUG, "[API] Create BMA ... [OK]\n");

        /* 初始化发送状态机 */
        xwssc->txq.cnt = 0;
        for (i = 0; (xwsq_t)i < XWSSC_MEMBLK_NUM; i++) {
                xwlib_bclst_init_node(&xwssc->txq.car[i].node);
                xwssc->txq.car[i].state = XWSSC_CRS_IDLE;
                xwssc->txq.car[i].idx = i;
                xwssc->txq.car[i].pri = XWSSC_INVALID_PRI;
                xwssc->txq.car[i].slot = NULL;
        }
        // cppcheck-suppress [misra-c2012-17.7]
        memset((void *)&xwssc->txq.carbmp, 0, sizeof(xwssc->txq.carbmp));
        for (i = 0; (xwsq_t)i < XWSSC_PRI_NUM; i++) {
                xwlib_bclst_init_head(&xwssc->txq.q[i]);
        }
        // cppcheck-suppress [misra-c2012-17.7]
        memset(&xwssc->txq.qnebmp, 0, sizeof(xwssc->txq.qnebmp));
        xwos_splk_init(&xwssc->txq.qlock);
        rc = xwos_sem_init(&xwssc->txq.qsem, 0, XWSSC_MEMBLK_NUM);
        if (rc < 0) {
                xwssclogf(ERR, "[API] Init TXQ semaphore ... <rc:%d>\n", rc);
                goto err_txqsem_init; // cppcheck-suppress [misra-c2012-15.2]
        }
        rc = xwos_mtx_init(&xwssc->txq.csmtx, XWOS_SKD_PRIORITY_RT_MIN);
        if (rc < 0) {
                xwssclogf(ERR, "[API] Init xwssc->csmtx ... <rc:%d>\n", rc);
                goto err_csmtx_init; // cppcheck-suppress [misra-c2012-15.2]
        }
        rc = xwos_cond_init(&xwssc->txq.cscond);
        if (rc < 0) {
                xwssclogf(ERR, "[API] Init xwssc->cscond ... <rc:%d>\n", rc);
                goto err_cscond_init; // cppcheck-suppress [misra-c2012-15.2]
        }
        xwssc->txq.remote.ack = 0;
        xwssc->txq.remote.id = 0;
        xwssc->txq.tmp = NULL;
        xwos_splk_init(&xwssc->txq.notiflk);

        /* 初始化接收状态机 */
        xwssc->rxq.cnt = 0;
        for (i = 0; (xwsq_t)i < XWSSC_PORT_NUM; i++) {
                xwlib_bclst_init_head(&xwssc->rxq.q[i]);
                xwos_splk_init(&xwssc->rxq.lock[i]);
                rc = xwos_sem_init(&xwssc->rxq.sem[i], 0, XWSSC_MEMBLK_NUM);
                if (rc < 0) {
                        xwssclogf(ERR, "[API] Init RXQ semaphore[%d] ... <rc:%d>\n",
                                  i, rc);
                        goto err_rxqsem_init; // cppcheck-suppress [misra-c2012-15.2]
                }
        }

        /* 打开硬件接口 */
        rc = xwssc_hwifal_open(xwssc, hwifcb);
        if (rc < 0) {
                goto err_hwifal_open; // cppcheck-suppress [misra-c2012-15.2]
        }

        /* 创建线程 */
        xwos_thd_attr_init(&attr);
        attr.name = "xwmd.isc.xwssc.rxthd";
        attr.stack = (xwstk_t *)xwssc->rxthd_stack;
        attr.stack_size = sizeof(xwssc->rxthd_stack);
        attr.priority = XWSSC_RXTHD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_init(&xwssc->rxthdobj, &xwssc->rxthd,
                           &attr,
                           (xwos_thd_f)xwssc_rxthd, xwssc);
        if (rc < 0) {
                goto err_rxthd_init; // cppcheck-suppress [misra-c2012-15.2]
        }

        xwos_thd_attr_init(&attr);
        attr.name = "xwmd.isc.xwssc.txthd";
        attr.stack = (xwstk_t *)xwssc->txthd_stack;
        attr.stack_size = sizeof(xwssc->txthd_stack);
        attr.priority = XWSSC_TXTHD_PRIORITY;
        attr.detached = false;
        attr.privileged = true;
        rc = xwos_thd_init(&xwssc->txthdobj, &xwssc->txthd,
                           &attr,
                           (xwos_thd_f)xwssc_txthd, xwssc);
        if (rc < 0) {
                goto err_txthd_init;
        }

        return XWOK;

err_txthd_init:
        xwos_thd_detach(xwssc->rxthd);
        xwos_thd_quit(xwssc->rxthd);
        xwssc->rxthd = XWOS_THD_NILD;
err_rxthd_init:
        xwssc_hwifal_close(xwssc); // cppcheck-suppress [misra-c2012-17.7]
err_hwifal_open:
err_rxqsem_init:
        for (j = i - 1; j >= 0; j--) {
                xwos_sem_fini(&xwssc->rxq.sem[j]);
        }
        xwos_cond_fini(&xwssc->txq.cscond);
err_cscond_init:
        xwos_mtx_fini(&xwssc->txq.csmtx);
err_csmtx_init:
        xwos_sem_fini(&xwssc->txq.qsem);
err_txqsem_init:
err_bma_init:
        xwos_object_rawput(&xwssc->xwobj); // cppcheck-suppress [misra-c2012-17.7]
err_xwobj_activate:
        return rc;
}

__xwmd_api
xwer_t xwssc_stop(struct xwssc * xwssc)
{
        XWSSC_VALIDATE((NULL != xwssc), "nullptr", -EFAULT);

        return xwssc_put(xwssc);
}

/**
 * @brief XWSSC的垃圾回收函数
 * @param[in] obj: XWSSC对象的指针
 * @return 错误码
 */
static __xwmd_code
xwer_t xwssc_gc(struct xwos_object * obj)
{
        struct xwssc * xwssc;
        struct xwssc_carrier * car;
        union xwssc_slot * slot;
        xwer_t rc;
        xwer_t childrc;
        xwssq_t j;

        xwssc = xwcc_derof(obj, struct xwssc, xwobj);
        if (NULL != xwssc->txthd.thd) {
                rc = xwos_thd_stop(xwssc->txthd, &childrc);
                if (XWOK == rc) {
                        xwssc->txthd = XWOS_THD_NILD;
                        xwssclogf(INFO, "[API] Stop XWSSC TX thread... [OK]\n");
                }
        }

        if (NULL != xwssc->rxthd.thd) {
                rc = xwos_thd_stop(xwssc->rxthd, &childrc);
                if (XWOK == rc) {
                        xwssc->rxthd = XWOS_THD_NILD;
                        xwssclogf(INFO, "[API] Stop XWSSC RX thread... [OK]\n");
                }
        }

        xwssc_hwifal_close(xwssc); // cppcheck-suppress [misra-c2012-17.7]

        /* 释放TXQ中的剩余帧 */
        do {
                rc = xwos_sem_trywait(&xwssc->txq.qsem);
                if (rc < 0) {
                        break;
                }
                car = xwssc_txq_choose(xwssc);
                // cppcheck-suppress [misra-c2012-17.7]
                xwmm_bma_free(xwssc->mempool, car->slot);
                car->slot = NULL;
                xwssc_txq_carrier_free(xwssc, car);
        } while (true);

        /* 释放RXQ中的剩余帧 */
        for (j = (xwssq_t)XWSSC_PORT_NUM - 1; j >= 0; j--) {
                do {
                        rc = xwos_sem_trywait(&xwssc->rxq.sem[j]);
                        if (rc < 0) {
                                break;
                        }
                        slot = xwssc_rxq_choose(xwssc, (xwu8_t)j);
                        // cppcheck-suppress [misra-c2012-17.7]
                        xwmm_bma_free(xwssc->mempool, slot);
                } while (true);
        }

        for (j = (xwssq_t)XWSSC_PORT_NUM - 1; j >= 0; j--) {
                xwos_sem_fini(&xwssc->rxq.sem[j]);
        }
        xwos_cond_fini(&xwssc->txq.cscond);
        xwos_mtx_fini(&xwssc->txq.csmtx);
        xwos_sem_fini(&xwssc->txq.qsem);
        return XWOK;
}

/**
 * @brief 用于实现 @ref xwssc_tx() 的回调函数参数结构体
 * @details
 * 此结构体被锁 `xwssc->txq.notiflk` 保护。上锁顺序：
 * ① `xwssc->txq.notiflk`
 * ② `xwssc_txcb_arg.splk`
 */
struct xwssc_txcb_arg {
        struct xwos_splk splk;
        struct xwos_cond cond;
        xwer_t rc; /**< 发送结果，被锁 `splk` 保护 */
};

/**
 * @brief 用于实现 @ref xwssc_tx() 的通知发送结果的回调函数
 * @param[in] xwssc: XWSSC对象的指针
 * @param[in] txh: 发送句柄
 * @param[in] rc: 发送结果
 * @param[in] arg: 回调函数的参数（此处用于传递struct xwssc_txcb_arg的指针）
 */
static __xwmd_code
void xwssc_txcb_notify(struct xwssc * xwssc, xwssc_txh_t txh, xwer_t rc, void * arg)
{
        struct xwssc_carrier * car;
        struct xwssc_txcb_arg * cbarg;

        XWOS_UNUSED(xwssc);
        car = txh;
        xwssclogf(DEBUG, "[API] txh:%p, rc:%d\n", txh, rc);
        if ((xwu32_t)XWSSC_CRS_FINISH == car->state) {
                cbarg = arg;
                xwos_splk_lock(&cbarg->splk);
                cbarg->rc = rc;
                xwos_splk_unlock(&cbarg->splk);
                xwos_cond_unicast(&cbarg->cond);
        }
}

__xwmd_api
xwer_t xwssc_tx(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwtm_t to)
{
        struct xwssc_txcb_arg cbarg;
        union xwos_ulock ulk;
        xwssc_txh_t txh;
        xwsq_t lkst;
        xwer_t rc;

        XWSSC_VALIDATE((NULL != xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != data), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((*size <= XWSSC_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWSSC_VALIDATE((pri < XWSSC_PRI_NUM), "pri-invalid", -E2BIG);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);
        XWSSC_VALIDATE((qos < XWSSC_MSG_QOS_NUM), "qos-invalid", -EINVAL);

        rc = xwssc_grab(xwssc);
        if (rc < 0) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        xwos_splk_init(&cbarg.splk);
        xwos_cond_init(&cbarg.cond);
        cbarg.rc = -EINPROGRESS;

        rc = xwssc_eq_msg(xwssc, data, *size, pri, port, qos,
                          xwssc_txcb_notify, &cbarg, &txh);
        if (rc < 0) {
                goto err_xwssc_eq;
        }

        ulk.osal.splk = &cbarg.splk;
        xwos_splk_lock(&cbarg.splk);
        if (-EINPROGRESS == cbarg.rc) {
                rc = xwos_cond_wait_to(&cbarg.cond, ulk, XWOS_LK_SPLK, NULL, to, &lkst);
                if (XWOK == rc) {
                        rc = cbarg.rc;
                        xwos_splk_unlock(&cbarg.splk);
                } else {
                        if ((xwsq_t)XWOS_LKST_LOCKED == lkst) {
                                xwos_splk_unlock(&cbarg.splk);
                        }
                        xwos_splk_lock(&xwssc->txq.notiflk);
                        xwos_splk_lock(&cbarg.splk);
                        if (-EINPROGRESS == cbarg.rc) {
                                txh->slot->tx.ntfcb = NULL;
                                txh->slot->tx.cbarg = NULL;
                        } else {
                                rc = cbarg.rc;
                        }
                        xwos_splk_unlock(&cbarg.splk);
                        xwos_splk_unlock(&xwssc->txq.notiflk);
                }
        } else {
                rc = cbarg.rc;
                xwos_splk_unlock(&cbarg.splk);
        }
        xwos_cond_fini(&cbarg.cond);
        xwssc_put(xwssc); // cppcheck-suppress [misra-c2012-17.7]
        return rc;

err_xwssc_eq:
        xwos_cond_fini(&cbarg.cond);
        xwssc_put(xwssc); // cppcheck-suppress [misra-c2012-17.7]
err_ifnotrdy:
        return rc;
}

__xwmd_api
xwer_t xwssc_eq(struct xwssc * xwssc,
                const xwu8_t data[], xwsz_t * size,
                xwu8_t pri, xwu8_t port, xwu8_t qos,
                xwssc_ntf_f ntfcb, void * cbarg,
                xwssc_txh_t * txhbuf)
{
        xwer_t rc;

        XWSSC_VALIDATE((NULL != xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != data), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((*size <= XWSSC_SDU_MAX_SIZE), "size-invalid", -E2BIG);
        XWSSC_VALIDATE((pri < XWSSC_PRI_NUM), "pri-invalid", -E2BIG);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);
        XWSSC_VALIDATE((qos < XWSSC_MSG_QOS_NUM), "qos-invalid", -EINVAL);

        rc = xwssc_grab(xwssc);
        if (rc < 0) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }
        rc = xwssc_eq_msg(xwssc, data, *size, pri, port, qos, ntfcb, cbarg, txhbuf);
        xwssc_put(xwssc); // cppcheck-suppress [misra-c2012-17.7]

err_ifnotrdy:
        return rc;
}

__xwmd_api
xwer_t xwssc_abort(struct xwssc * xwssc, xwssc_txh_t txh)
{
        xwer_t rc;

        XWSSC_VALIDATE((NULL != xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != txh), "nullptr", -EFAULT);

        XWOS_UNUSED(xwssc);

        rc = xwaop_teq_then_write(xwu32_t, &txh->state,
                                  XWSSC_CRS_READY,
                                  XWSSC_CRS_ABORT,
                                  NULL);
        return rc;
}

__xwmd_api
xwsq_t xwssc_get_txstate(xwssc_txh_t txh)
{
        return txh->state;
}

__xwmd_api
xwer_t xwssc_rx(struct xwssc * xwssc, xwu8_t port,
                xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos,
                xwtm_t to)
{
        union xwssc_slot * slot;
        xwu8_t * sdupos;
        xwsz_t bufsize;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWSSC_VALIDATE((NULL != xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != rxbuf), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);

        rc = xwssc_grab(xwssc);
        if (rc < 0) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        bufsize = *size;
        xwssclogf(DEBUG, "[API] port:%d, buffer size:0x%X\n", port, bufsize);
        rc = xwos_sem_wait_to(&xwssc->rxq.sem[port], to);
        if (rc < 0) {
                goto err_sem_wait_to;
        }

        slot = xwssc_rxq_choose(xwssc, port);
        sdupos = XWSSC_SDUPOS(&slot->rx.frm.head);
        sdusize = XWSSC_RXSDUSIZE(&slot->rx);

        if (bufsize < sdusize) {
                realsize = bufsize;
                xwssclogf(WARNING,
                          "[API] Buffer is too small(0x%X)! Expected size is 0x%X\n",
                          bufsize, sdusize);
        } else {
                realsize = sdusize;
        }

        // cppcheck-suppress [misra-c2012-17.7]
        memcpy(rxbuf, sdupos, realsize);
        if (NULL != qos) {
                *qos = slot->rx.frm.head.qos;
        }
        *size = realsize;

        xwmm_bma_free(xwssc->mempool, slot); // cppcheck-suppress [misra-c2012-17.7]
        xwssc_put(xwssc); // cppcheck-suppress [misra-c2012-17.7]
        return XWOK;

err_sem_wait_to:
        xwssc_put(xwssc); // cppcheck-suppress [misra-c2012-17.7]
err_ifnotrdy:
        return rc;
}

__xwmd_api
xwer_t xwssc_try_rx(struct xwssc * xwssc, xwu8_t port,
                    xwu8_t rxbuf[], xwsz_t * size, xwu8_t * qos)
{
        union xwssc_slot * slot;
        xwu8_t * sdupos;
        xwsz_t bufsize;
        xwsz_t sdusize;
        xwsz_t realsize;
        xwer_t rc;

        XWSSC_VALIDATE((NULL != xwssc), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != rxbuf), "nullptr", -EFAULT);
        XWSSC_VALIDATE((NULL != size), "nullptr", -EFAULT);
        XWSSC_VALIDATE((port < XWSSC_PORT_NUM), "no-such-port", -ENODEV);

        rc = xwssc_grab(xwssc);
        if (rc < 0) {
                rc = -EPERM;
                goto err_ifnotrdy;
        }

        bufsize = *size;
        xwssclogf(DEBUG, "[API] port:%d, buffer size:0x%X\n", port, bufsize);
        rc = xwos_sem_trywait(&xwssc->rxq.sem[port]);
        if (rc < 0) {
                goto err_sem_trywait;
        }

        slot = xwssc_rxq_choose(xwssc, port);
        sdupos = XWSSC_SDUPOS(&slot->rx.frm.head);
        sdusize = XWSSC_RXSDUSIZE(&slot->rx);

        if (bufsize < sdusize) {
                realsize = bufsize;
                xwssclogf(WARNING,
                          "[API] Buffer is too small(0x%X)! Expected size is 0x%X\n",
                          bufsize, sdusize);
        } else {
                realsize = sdusize;
        }
        // cppcheck-suppress [misra-c2012-17.7]
        memcpy(rxbuf, sdupos, realsize);
        if (NULL != qos) {
                *qos = slot->rx.frm.head.qos;
        }
        *size = realsize;

        xwmm_bma_free(xwssc->mempool, slot); // cppcheck-suppress [misra-c2012-17.7]
        xwssc_put(xwssc); // cppcheck-suppress [misra-c2012-17.7]
        return XWOK;

err_sem_trywait:
        xwssc_put(xwssc); // cppcheck-suppress [misra-c2012-17.7]
err_ifnotrdy:
        return rc;
}

/**
 * @} xwmd_isc_xwssc_mif
 */
