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

/* #define XWMM_BMA_LOG */ /**< 调试日志开关  */
#if defined(XWMM_BMA_LOG)
#  define xwmm_bmalogf(lv, fmt, ...) xwlogf(lv, "BMA", fmt, ##__VA_ARGS__)
#else
#  define xwmm_bmalogf(lv, fmt, ...)
#endif

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
                            xwu8_t odr,
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
                xwmm_bmalogf(ERR, "Size of memory(0x%lX, 0x%lX) is error!\n",
                             origin, size);
                goto err_size;
        }
        bma->name = name;
        bma->zone.origin = origin;
        bma->zone.size = blksize * num;
        bma->blksize = blksize;
        bma->blkodr = blkodr;
        xwos_splk_init(&bma->lock);
        bma->orderlists = (struct xwmm_bma_orderlist *)&bma[(xwsz_t)1];
        bma->bcbs = (struct xwmm_bma_bcb *)&bma->orderlists[(xwsz_t)1 + blkodr];
        xwmm_bmalogf(DEBUG,
                     "memory:(0x%lX,0x%lX),orderlists:0x%lX,bcbs:0x%lX,"
                     "blocksize:0x%lX, blockorder:0x%lX\n",
                     origin, size, (xwptr_t)bma->orderlists, (xwptr_t)bma->bcbs,
                     bma->blksize, bma->blkodr);

        for (i = 0; i < num; i++) {
                bma->bcbs[i].order = XWMM_BMA_INUSED | XWMM_BMA_COMBINED;
        }
        bma->bcbs[0].order = (xwu8_t)blkodr | XWMM_BMA_INUSED;

        for (i = 0; i <= blkodr; i++) {
                xwlib_bclst_init_head(&bma->orderlists[i].head);
        }
        xwmm_bma_orderlist_add(bma, &bma->orderlists[blkodr], blkodr, &bma->bcbs[0]);
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
 * @param[in] odr: 阶链表的阶
 * @param[in] bcb: 内存块的控制块指针
 */
static __xwos_code
void xwmm_bma_orderlist_add(struct xwmm_bma * bma,
                            struct xwmm_bma_orderlist * ol,
                            xwu8_t odr,
                            struct xwmm_bma_bcb * bcb)
{
        struct xwlib_bclst_node * n;

        /* 即将加入的块不在阶链表中，是本地数据。*/
        if ((XWMM_BMA_INUSED | odr) == bcb->order) {
                n = xwmm_bma_bcb_to_mem(bma, bcb);
                xwmm_bmalogf(DEBUG,
                             "[OL:0x%lX][+] bcb(idx:0x%lX,odr:0x%X),mem(0x%lX)\n",
                             (xwptr_t)ol,
                             (((xwptr_t)bcb - (xwptr_t)xwmm_bma->bcbs) /
                              sizeof(struct xwmm_bma_bcb)),
                             bcb->order, (xwptr_t)n);
                xwlib_bclst_init_node(n);
                xwlib_bclst_add_head(&ol->head, n);
                /* 当块被加入到阶链表中，就变成共享数据。*/
                bcb->order &= XWMM_BMA_ORDER_MASK;
        }
}

/**
 * @brief 从阶链表中删除一块内存
 * @param[in] bma: 伙伴算法内存块分配器对象的指针
 * @param[in] ol: 阶链表的指针
 * @param[in] odr: 阶链表的阶
 * @param[in] bcb: 内存块的控制块指针
 * @return 错误码
 * @retval XWOK: 删除成功
 * @retval -ESRCH: 阶链表中不存在此内存块
 */
static __xwos_code
xwer_t xwmm_bma_orderlist_remove(struct xwmm_bma * bma,
                                 struct xwmm_bma_orderlist * ol,
                                 xwu8_t odr,
                                 struct xwmm_bma_bcb * bcb)
{
        struct xwlib_bclst_node * n;
        xwer_t rc;

        XWOS_UNUSED(ol);
        n = xwmm_bma_bcb_to_mem(bma, bcb);
        xwmm_bmalogf(DEBUG,
                     "[OL:0x%lX][-] bcb(idx:0x%lX,odr:0x%X),mem(0x%lX)\n",
                     (xwptr_t)ol,
                     (((xwptr_t)bcb - (xwptr_t)xwmm_bma->bcbs) /
                      sizeof(struct xwmm_bma_bcb)),
                     bcb->order, (xwptr_t)n);
        /* 即将被删除的块在阶链表中，是共享数据。*/
        if (odr != bcb->order) {
                rc = -ESRCH;
        } else {
                xwlib_bclst_del_init(n);
                bcb->order |= XWMM_BMA_INUSED;
                rc = XWOK;
        }
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

        if (xwlib_bclst_tst_empty(&ol->head)) {
                bcb = err_ptr(-ENOENT);
        } else {
                n = ol->head.next;
                xwlib_bclst_del_init(n);
                bcb = xwmm_bma_mem_to_bcb(bma, n);
                xwmm_bmalogf(DEBUG,
                             "[OL:0x%lX][C] bcb(idx:0x%lX,odr:0x%X)\n",
                             (xwptr_t)ol,
                             (((xwptr_t)bcb - (xwptr_t)xwmm_bma->bcbs) /
                              sizeof(struct xwmm_bma_bcb)),
                             bcb->order);
                /* 即将被选择的块在阶链表中，是共享数据。*/
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
                /* 加入阶链表之前， `buddy` 是本地数据。*/
                XWOS_BUG_ON((XWMM_BMA_COMBINED | XWMM_BMA_INUSED) != buddy->order);
                buddy->order = (xwu8_t)blk_odr | XWMM_BMA_INUSED;
                xwmm_bma_orderlist_add(bma, curr_ol, blk_odr, buddy);
                /* `bcb` 是本地数据。*/
                bcb->order = (xwu8_t)blk_odr | XWMM_BMA_INUSED;
                xwmm_bmalogf(DEBUG,
                             "[ALLOC][D] bcb(idx:0x%lX,odr:0x%X),"
                             "buddy(idx:0x%lX,odr:0x%X)\n",
                             (((xwptr_t)bcb - (xwptr_t)xwmm_bma->bcbs) /
                              sizeof(struct xwmm_bma_bcb)),
                             bcb->order,
                             (((xwptr_t)buddy - (xwptr_t)xwmm_bma->bcbs) /
                              sizeof(struct xwmm_bma_bcb)),
                             buddy->order);
        }
}

__xwos_api
xwer_t xwmm_bma_alloc(struct xwmm_bma * bma, xwsq_t order, void ** membuf)
{
        xwer_t rc;
        xwreg_t flag;
        xwsq_t o;
        struct xwmm_bma_orderlist * ol;
        struct xwmm_bma_bcb * bcb;

        XWOS_VALIDATE((bma), "nullptr", -EFAULT);
        XWOS_VALIDATE((membuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((order <= (xwsq_t)XWSSQ_MAX), "out-of-range", -ERANGE);

        ol = NULL;
        bcb = err_ptr(-ENOENT);
        xwos_splk_lock_cpuirqsv(&bma->lock, &flag);
        for (o = order; o <= bma->blkodr; o++) {
                ol = &bma->orderlists[o];
                bcb = xwmm_bma_orderlist_choose(bma, ol);
                if (!is_err(bcb)) {
                        break;
                }
        }
        if (is_err(bcb)) { // cppcheck-suppress [misra-c2012-14.4]
                xwos_splk_unlock_cpuirqrs(&bma->lock, flag);
                rc = -ENOMEM;
                *membuf = NULL;
        } else {
                xwmm_bmalogf(DEBUG,
                             "[ALLOC] bcb(idx:0x%lX,odr:0x%X)\n",
                             (((xwptr_t)bcb - (xwptr_t)xwmm_bma->bcbs) /
                              sizeof(struct xwmm_bma_bcb)),
                             bcb->order);
                xwmm_bma_divide_block(bma, bcb, order, ol);
                xwos_splk_unlock_cpuirqrs(&bma->lock, flag);
                rc = XWOK;
                *membuf = xwmm_bma_bcb_to_mem(bma, bcb);
        }
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
        xwsq_t curr_odr;
        xwsq_t target_odr;
        xwer_t rc;

        curr_odr = (bcb->order & XWMM_BMA_ORDER_MASK);
        target_odr = curr_odr + (xwsq_t)1;
        while (target_odr <= bma->blkodr) {
                buddy = xwmm_bma_find_buddy(bma, bcb);
                xwmm_bmalogf(DEBUG,
                             "[FREE][M] bcb(idx:0x%lX,odr:0x%X),"
                             "buddy(idx:0x%lX,odr:0x%X)\n",
                             (((xwptr_t)bcb - (xwptr_t)xwmm_bma->bcbs) /
                              sizeof(struct xwmm_bma_bcb)),
                             bcb->order,
                             (((xwptr_t)buddy - (xwptr_t)xwmm_bma->bcbs) /
                              sizeof(struct xwmm_bma_bcb)),
                             buddy->order);
                XWOS_BUG_ON((XWMM_BMA_COMBINED | XWMM_BMA_INUSED) == buddy->order);
                rc = xwmm_bma_orderlist_remove(bma,
                                               &bma->orderlists[curr_odr],
                                               curr_odr,
                                               buddy);
                if (rc < 0) {
                        break;
                }
                /* 此时 `buddy->order` 以及 `bcb->order` 都是本地数据。 */
                if (buddy > bcb) {
                        buddy->order = XWMM_BMA_COMBINED | XWMM_BMA_INUSED;
                        bcb->order = (xwu8_t)target_odr | XWMM_BMA_INUSED;
                } else {
                        bcb->order = XWMM_BMA_COMBINED | XWMM_BMA_INUSED;
                        buddy->order = (xwu8_t)target_odr | XWMM_BMA_INUSED;
                        bcb = buddy;
                }
                curr_odr = target_odr;
                target_odr++;
        }
        xwmm_bma_orderlist_add(bma, &bma->orderlists[curr_odr], curr_odr, bcb);
}

__xwos_api
xwer_t xwmm_bma_free(struct xwmm_bma * bma, void * mem)
{
        struct xwmm_bma_bcb * bcb;
        xwreg_t flag;
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
        xwmm_bmalogf(DEBUG,
                     "[FREE] mem:0x%lX,bcb(idx:0x%lX,odr:0x%X)\n",
                     (xwptr_t)mem,
                     (((xwptr_t)bcb - (xwptr_t)xwmm_bma->bcbs) /
                      sizeof(struct xwmm_bma_bcb)),
                     bcb->order);
        if (0 == (XWMM_BMA_INUSED & bcb->order)) {
                rc = -EINVAL;
                goto err_invalmem;
        }
        xwos_splk_lock_cpuirqsv(&bma->lock, &flag);
        xwmm_bma_combine(bma, bcb);
        xwos_splk_unlock_cpuirqrs(&bma->lock, flag);
        return XWOK;

err_invalmem:
err_range:
        return rc;
}
