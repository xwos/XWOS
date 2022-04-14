/**
 * @file
 * @brief 循环队列：模块接口
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_isc_xwcq_mif_h__
#define __xwmd_isc_xwcq_mif_h__

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/sel.h>

/**
 * @defgroup xwmd_isc_xwcq 循环队列
 * @ingroup xwmd_isc
 * @{
 */

/**
 * @brief 定义循环队列缓冲区
 * @param[in] name: 变量名称
 * @param[in] slotsize: 数据槽的大小
 * @param[in] slotnum: 数据槽的个数
 */
#define XWCQ_DEF_MEMPOOL(name, slotsize, slotnum) \
        __xwcc_alignl1cache xwu8_t name[(slotsize) * (slotnum)]

/**
 * @brief 循环队列对象
 */
struct xwcq {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        const char * name; /**< 名字 */
        xwsz_t slotsize; /**< 队列中数据槽的大小 */
        xwsz_t slotnum; /**< 队列中数据槽的数量 */
        struct xwos_sem sem; /**< 队列中的可用数据信号量 */
        struct xwos_splk lock; /**< 保护队列的锁 */
        xwssz_t rear; /**< 可用于接收的空白数据槽的位置 */
        xwssz_t front; /**< 包含有效数据的数据槽的位置 */
        xwu8_t * q; /**< 队列 */
};

xwer_t xwcq_init(struct xwcq * cq, const char * name,
                 xwsz_t slotsize, xwsz_t slotnum, xwu8_t * mem);
xwer_t xwcq_fini(struct xwcq * cq);
xwer_t xwcq_create(struct xwcq ** ptrbuf, const char * name,
                   xwsz_t slotsize, xwsz_t slotnum);
xwer_t xwcq_delete(struct xwcq * cq);
xwsq_t xwcq_gettik(struct xwcq * cq);
xwer_t xwcq_acquire(struct xwcq * cq, xwsq_t tik);
xwer_t xwcq_release(struct xwcq * cq, xwsq_t tik);
xwer_t xwcq_grab(struct xwcq * cq);
xwer_t xwcq_put(struct xwcq * cq);

xwer_t xwcq_eq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);
xwer_t xwcq_jq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);
xwer_t xwcq_dq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);
xwer_t xwcq_dq_to(struct xwcq * cq, xwu8_t * data, xwsz_t * size, xwtm_t to);
xwer_t xwcq_trydq(struct xwcq * cq, xwu8_t * data, xwsz_t * size);
xwer_t xwcq_peek(struct xwcq * cq, xwu8_t * data, xwsz_t * size);
xwer_t xwcq_peek_to(struct xwcq * cq, xwu8_t * data, xwsz_t * size, xwtm_t to);
xwer_t xwcq_trypeek(struct xwcq * cq, xwu8_t * data, xwsz_t * size);
xwer_t xwcq_flush(struct xwcq * cq);
xwer_t xwcq_get_capacity(struct xwcq * cq, xwsz_t * capbuf);
xwer_t xwcq_get_size(struct xwcq * cq, xwsz_t * szbuf);
xwer_t xwcq_tst_empty(struct xwcq * cq, bool * emptybuf);
xwer_t xwcq_bind(struct xwcq * cq, struct xwos_sel * sel, xwsq_t pos);
xwer_t xwcq_unbind(struct xwcq * cq, struct xwos_sel * sel);

/**
 * @} xwmd_isc_xwcq
 */

#endif /* xwmd/isc/xwcq/mif.h */
