/**
 * @file
 * @brief XWOS内存管理：伙伴算法内存块分配器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/mm/common.h>
#include <xwos/mm/bma.h>

static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_mem_to_bcb(struct xwmm_bma * bma, void * mem);

static __xwos_code
void * xwmm_bma_bcb_to_mem(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb);

static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_find_buddy(struct xwmm_bma * bma,
                                          struct xwmm_bma_bcb * bcb);

static __xwos_code
void xwmm_bma_orderlist_add(struct xwmm_bma * bma,
                            struct xwmm_bma_orderlist * ol,
                            struct xwmm_bma_bcb * bcb);

static __xwos_code
xwer_t xwmm_bma_orderlist_remove(struct xwmm_bma * bma,
                                 struct xwmm_bma_orderlist * ol,
                                 xwu8_t odr,
                                 struct xwmm_bma_bcb * bcb);

static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_orderlist_choose(struct xwmm_bma * bma,
                                                struct xwmm_bma_orderlist * ol);

static __xwos_code
void xwmm_bma_divide_block(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb,
                           xwsq_t target_odr, struct xwmm_bma_orderlist * curr_ol);

static __xwos_code
void xwmm_bma_combine(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb);

__xwos_api
xwer_t xwmm_bma_init(struct xwmm_bma * bma, const char * name,
                     xwptr_t origin, xwsz_t size,
                     xwsz_t blksize, xwsz_t blkodr)
{
        xwer_t rc;
        xwsz_t num;
        xwsz_t i;

        XWOS_VALIDATE((bma), "nullptr", -EFAULT);

        num = 1U << blkodr;
        if (size != (num * blksize)) {
                rc = -ESIZE;
                xwmm_bmalogf(ERR, "Size of memory(0x%X, %d) is error!\n",
                             origin, size);
                goto err_size;
        }
        bma->name = name;
        bma->zone.origin = origin;
        bma->zone.size = blksize * num;
        bma->blksize = blksize;
        bma->blkodr = blkodr;
        bma->orderlists = (struct xwmm_bma_orderlist *)&bma[(xwsz_t)1];
        bma->bcbs = (struct xwmm_bma_bcb *)&bma->orderlists[(xwsz_t)1 + blkodr];

        /* Init all blocks */
        for (i = 0; i < num; i++) {
                bma->bcbs[i].order = XWMM_BMA_COMBINED;
        }
        bma->bcbs[0].order = (xwu8_t)blkodr;

        /* Init all order lists */
        for (i = 0; i <= blkodr; i++) {
                xwlib_bclst_init_head(&bma->orderlists[i].head);
                xwos_splk_init(&bma->orderlists[i].lock);
        }
        xwmm_bma_orderlist_add(bma, &bma->orderlists[blkodr], &bma->bcbs[0]);
        return XWOK;

err_size:
        return rc;
}

/**
 * @brief 从内存块首地址获得其控制块的指针
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] mem: 内存块的首地址
 * @return 内存块的控制块指针或类型为指针的错误码
 */
static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_mem_to_bcb(struct xwmm_bma * bma, void * mem)
{
        struct xwmm_bma_bcb * bcb;
        xwptr_t ofs;
        xwptr_t idx;

        ofs = (xwptr_t)mem - bma->zone.origin;
        idx = ofs / (xwptr_t)bma->blksize;
        if (ofs == (idx * bma->blksize)) {
                bcb = &bma->bcbs[idx];
        } else {
                bcb = err_ptr(-EINVAL);
        }
        return bcb;
}

/**
 * @brief 从内存块的控制块指针获得内存块的首地址
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] bcb: 内存块的控制块指针
 * @return 内存块的首地址
 */
static __xwos_code
void * xwmm_bma_bcb_to_mem(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb)
{
        xwptr_t idx;
        xwptr_t mem;

        idx = ((xwptr_t)bcb - (xwptr_t)bma->bcbs) / sizeof(struct xwmm_bma_bcb);
        mem = (idx * bma->blksize) + bma->zone.origin;
        return (void *)mem;
}

/**
 * @brief 寻找内存块的伙伴
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] bcb: 内存块的控制块指针
 * @return 伙伴内存块的控制块指针
 */
static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_find_buddy(struct xwmm_bma * bma,
                                          struct xwmm_bma_bcb * bcb)
{
        xwsq_t seq;
        xwsq_t odr;

        seq = ((xwptr_t)bcb - (xwptr_t)bma->bcbs) / sizeof(struct xwmm_bma_bcb);
        odr = (xwsq_t)bcb->order & XWMM_BMA_ORDER_MASK;
        seq ^= (1U << odr);
        return &bma->bcbs[seq];
}

/**
 * @brief 将一块内存加入到阶链表
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] ol: 阶链表的指针
 * @param[in] bcb: 内存块的控制块指针
 */
static __xwos_code
void xwmm_bma_orderlist_add(struct xwmm_bma * bma,
                            struct xwmm_bma_orderlist * ol,
                            struct xwmm_bma_bcb * bcb)
{
        struct xwlib_bclst_node * n;
        xwreg_t flag;

        n = xwmm_bma_bcb_to_mem(bma, bcb);
        xwlib_bclst_init_node(n);
        xwos_splk_lock_cpuirqsv(&ol->lock, &flag);
        xwlib_bclst_add_tail(&ol->head, n);
        xwos_splk_unlock_cpuirqrs(&ol->lock, flag);
}

/**
 * @brief 从阶链表中删除一块内存
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] ol: 阶链表的指针
 * @param[in] odr: 阶链表的阶
 * @param[in] bcb: 内存块的控制块指针
 */
static __xwos_code
xwer_t xwmm_bma_orderlist_remove(struct xwmm_bma * bma,
                                 struct xwmm_bma_orderlist * ol,
                                 xwu8_t odr,
                                 struct xwmm_bma_bcb * bcb)
{
        struct xwlib_bclst_node * n;
        xwreg_t flag;
        xwer_t rc;

        n = xwmm_bma_bcb_to_mem(bma, bcb);

        xwos_splk_lock_cpuirqsv(&ol->lock, &flag);
        if (odr != bcb->order) {
                rc = -ESRCH;
        } else {
                rc = XWOK;
                xwlib_bclst_del_init(n);
        }
        xwos_splk_unlock_cpuirqrs(&ol->lock, flag);
        return rc;
}

/**
 * @brief 从阶链表中选择一块内存，并返回其块控制块
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] ol: 阶链表的指针
 * @return 块控制块指针或类型为指针的错误码
 * @retval -ENOENT: 阶链表为空
 */
static __xwos_code
struct xwmm_bma_bcb * xwmm_bma_orderlist_choose(struct xwmm_bma * bma,
                                                struct xwmm_bma_orderlist * ol)
{
        struct xwlib_bclst_node * n;
        struct xwmm_bma_bcb * bcb;
        xwreg_t flag;

        xwos_splk_lock_cpuirqsv(&ol->lock, &flag);
        if (xwlib_bclst_tst_empty(&ol->head)) {
                xwos_splk_unlock_cpuirqrs(&ol->lock, flag);
                bcb = err_ptr(-ENOENT);
        } else {
                n = ol->head.next;
                xwlib_bclst_del_init(n);
                xwos_splk_unlock_cpuirqrs(&ol->lock, flag);
                bcb = xwmm_bma_mem_to_bcb(bma, n);
                bcb->order |= XWMM_BMA_INUSED;
        }
        return bcb;
}

/**
 * @brief 将大内存块分割成小块
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] bcb: 大块的内存块的控制块的指针
 * @param[in] target_odr: 目标块数量的阶
 * @param[in] curr_ol: 当前被分割的大块所在的阶链表
 */
static __xwos_code
void xwmm_bma_divide_block(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb,
                           xwsq_t target_odr, struct xwmm_bma_orderlist * curr_ol)
{
        xwsq_t blk_odr;
        xwsq_t blk_idx;
        xwsq_t blk_ofs;
        struct xwmm_bma_bcb * buddy;

        blk_odr = (xwsq_t)bcb->order & XWMM_BMA_ORDER_MASK;
        blk_idx = ((xwptr_t)bcb - (xwptr_t)bma->bcbs) / sizeof(struct xwmm_bma_bcb);

        while (blk_odr > target_odr) {
                curr_ol--;
                blk_odr--;
                blk_ofs = 1U << blk_odr;
                buddy = &bma->bcbs[blk_idx + blk_ofs];
                buddy->order = (xwu8_t)blk_odr;
                xwmm_bma_orderlist_add(bma, curr_ol, buddy);
                bcb->order = (xwu8_t)blk_odr | XWMM_BMA_INUSED;
                xwmm_bmalogf(DEBUG,
                             "bcb(idx:0x%X,odr:0x%X), buddy(idx:0x%X,odr:0x%X)\n",
                             (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             bcb->order,
                             (buddy - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             buddy->order);
        }
}

__xwos_api
xwer_t xwmm_bma_alloc(struct xwmm_bma * bma, xwsq_t order, void ** membuf)
{
        xwer_t rc;
        xwsq_t o;
        struct xwmm_bma_orderlist * ol;
        struct xwmm_bma_bcb * bcb;

        XWOS_VALIDATE((bma), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((order <= (xwsq_t)XWSSQ_MAX), "out-of-range", -ERANGE);

        ol = NULL;
        bcb = err_ptr(-ENOENT);
        for (o = order; o <= bma->blkodr; o++) {
                ol = &bma->orderlists[o];
                bcb = xwmm_bma_orderlist_choose(bma, ol);
                if (!is_err(bcb)) {
                        break;
                }
        }
        if (is_err(bcb)) { // cppcheck-suppress [misra-c2012-14.4]
                rc = -ENOMEM;
                *membuf = NULL;
                goto err_nomem;
        }
        xwmm_bmalogf(DEBUG,
                     "bcb(idx:0x%X,odr:0x%X)\n",
                     (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                     bcb->order);
        xwmm_bma_divide_block(bma, bcb, order, ol);
        *membuf = xwmm_bma_bcb_to_mem(bma, bcb);
        return XWOK;

err_nomem:
        return rc;
}

/**
 * @brief 合并内存块
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] bcb: 内存块的控制块的指针
 * @return 错误码
 */
static __xwos_code
void xwmm_bma_combine(struct xwmm_bma * bma, struct xwmm_bma_bcb * bcb)
{
        struct xwmm_bma_bcb * buddy;
        xwsq_t odr;
        xwer_t rc;

        bcb->order &= XWMM_BMA_ORDER_MASK; /* clear the `inused' state */
        odr = (xwsq_t)bcb->order + (xwsz_t)1;

        while (odr <= bma->blkodr) {
                buddy = xwmm_bma_find_buddy(bma, bcb);
                XWOS_BUG_ON((xwu8_t)XWMM_BMA_COMBINED == buddy->order);
                xwmm_bmalogf(DEBUG,
                             "bcb(idx:0x%X,odr:0x%X), buddy(idx:0x%X,odr:0x%X)\n",
                             (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             bcb->order,
                             (buddy - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                             buddy->order);
                rc = xwmm_bma_orderlist_remove(bma,
                                               &bma->orderlists[bcb->order],
                                               bcb->order,
                                               buddy);
                if (rc < 0) {
                        break;
                }
                if (buddy > bcb) {
                        buddy->order = XWMM_BMA_COMBINED;
                        bcb->order = (xwu8_t)odr;
                } else {
                        bcb->order = XWMM_BMA_COMBINED;
                        buddy->order = (xwu8_t)odr;
                        bcb = buddy;
                }
                odr++;
        }
        odr = bcb->order;
        xwmm_bma_orderlist_add(bma, &bma->orderlists[odr], bcb);
}

__xwos_api
xwer_t xwmm_bma_free(struct xwmm_bma * bma, void * mem)
{
        struct xwmm_bma_bcb * bcb;
        xwer_t rc;

        XWOS_VALIDATE((bma), "nullptr", -EFAULT);
        XWOS_VALIDATE((mem), "nullptr", -EFAULT);

        if ((((xwptr_t)mem < bma->zone.origin) ||
             ((xwptr_t)mem >= (bma->zone.origin + bma->zone.size)))) {
                rc = -ERANGE;
                goto err_range;
        }
        bcb = xwmm_bma_mem_to_bcb(bma, mem);
        if (is_err(bcb)) {  // cppcheck-suppress [misra-c2012-14.4]
                rc = ptr_err(bcb);
                goto err_invalmem;
        }
        if (!(XWMM_BMA_INUSED & bcb->order)) {
                rc = -EINVAL;
                goto err_invalmem;
        }
        xwmm_bmalogf(DEBUG,
                     "bcb(idx:0x%X,odr:0x%X)\n",
                     (bcb - bma->bcbs)/sizeof(struct xwmm_bma_bcb),
                     bcb->order);
        xwmm_bma_combine(bma, bcb);
        return XWOK;

err_invalmem:
err_range:
        return rc;
}
