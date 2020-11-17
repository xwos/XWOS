/**
 * @file
 * @brief 玄武OS MP内核：线程
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_thrd_h__
#define __xwos_mp_thrd_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/sync/cond.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/wqn.h>
#include <xwos/mp/mtxtree.h>
#include <xwos/mp/tt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwmp_plwq;
struct xwmp_rtwq;
struct xwmp_skd;
struct xwmp_tcb;

/**
 * @brief XWOS MP线程控制块
 */
struct xwmp_tcb {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        struct xwmp_skd * xwskd; /**< 当前线程所属于的调度器 */
        struct xwmp_skd_stack_info stack; /**< 栈 */
        struct xwlib_bclst_node tcbnode; /**< 调度器线程链表中的节点 */

        /**< 线程状态 */
        struct xwmp_splk stlock; /**< 保护状态state的自旋锁 */
        xwsq_t state; /**< 线程状态 */
        xwsq_t attribute; /**< 线程属性 */

        /* 冻结状态 */
        struct xwlib_bclst_node frznode; /**< 冻结链表节点 */

        /* 迁移状态 */
        struct {
                xwid_t dst; /**< 目的CPU的ID */
        } migration; /**< 迁移状态成员 */

        /* 就绪状态 */
        struct xwlib_bclst_node rqnode; /**< 就绪队列节点，
                                           此成员被锁rtrq->lock保护 */

        /* 睡眠状态 */
        struct xwmp_ttn ttn; /**< 继承：时间树节点, 被锁tt->lock保护 */

        /* 阻塞状态 */
        struct xwmp_wqn wqn; /**< 等待队列 */

        /* 退出状态 */
        struct xwmp_cond completion; /**< 线程退出时的事件信号量 */

        /* 线程优先级 */
        struct xwmp_mtxtree mtxtree; /**< 互斥锁树：线程已经获得的所有互斥锁都
                                        需要加入到互斥锁树 */
        xwpr_t sprio; /**< 线程的固有优先级 */
        struct {
                xwpr_t r; /**< 线程的运行态优先级 */
                xwpr_t rq; /**< 线程的就绪态优先级 */
                xwpr_t wq; /**< 线程的等待态优先级 */
        } dprio; /**< 线程的动态优先级，被<em><b>stlock</b></em>保护 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
xwer_t xwmp_thrd_grab(struct xwmp_tcb * tcb);
xwer_t xwmp_thrd_put(struct xwmp_tcb * tcb);
xwer_t xwmp_thrd_chprio_once(struct xwmp_tcb * tcb, xwpr_t dprio,
                             struct xwmp_mtx ** pmtx);
void xwmp_thrd_chprio(struct xwmp_tcb * tcb);
xwer_t xwmp_thrd_rq_add_head(struct xwmp_tcb * tcb, xwpr_t prio);
xwer_t xwmp_thrd_rq_add_tail(struct xwmp_tcb * tcb, xwpr_t prio);
xwer_t xwmp_thrd_rq_remove(struct xwmp_tcb * tcb);
xwer_t xwmp_thrd_wakeup(struct xwmp_tcb * tcb);
void xwmp_thrd_ttn_callback(void * entry);
void xwmp_thrd_wqn_callback(void * entry);
xwer_t xwmp_thrd_tt_add_locked(struct xwmp_tcb * tcb, struct xwmp_tt * xwtt,
                               xwtm_t expected, xwreg_t cpuirq);
void xwmp_thrd_eq_rtwq_locked(struct xwmp_tcb * tcb,
                              struct xwmp_rtwq * xwrtwq,
                              xwu16_t type,
                              xwpr_t dprio);
void xwmp_thrd_eq_plwq_locked(struct xwmp_tcb * tcb,
                              struct xwmp_plwq * xwplwq,
                              xwu16_t type);
xwer_t xwmp_thrd_do_unlock(void * lock, xwsq_t lktype, void * lkdata);
xwer_t xwmp_thrd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm, void * lkdata);
xwer_t xwmp_thrd_reqfrz_lic(struct xwmp_tcb * tcb);
xwer_t xwmp_thrd_thaw_lic_pmlk(struct xwmp_tcb * tcb);

/******** XWOS MP IRQ Callback for BSP Adaptation Code ********/
xwer_t xwmp_thrd_exit_lic(struct xwmp_tcb * tcb, xwer_t rc);
xwer_t xwmp_thrd_freeze_lic(struct xwmp_tcb * tcb);
void xwmp_thrd_immigrate_lic(struct xwmp_tcb * tcb);
xwer_t xwmp_thrd_outmigrate_lic(struct xwmp_tcb * tcb, xwid_t dstcpu);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWMPCFG_SKD_TCB_MEMSLICE) && (1 == XWMPCFG_SKD_TCB_MEMSLICE)
xwer_t xwmp_tcb_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif /* XWMPCFG_SKD_TCB_MEMSLICE */

xwer_t xwmp_thrd_init(struct xwmp_tcb * tcb,
                      const char * name,
                      xwmp_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr);
xwer_t xwmp_thrd_destroy(struct xwmp_tcb * tcb);
xwer_t xwmp_thrd_create(struct xwmp_tcb ** tcbpbuf, const char * name,
                        xwmp_thrd_f manfunc, void * arg,
                        xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr);
xwer_t xwmp_thrd_delete(struct xwmp_tcb * tcb);
void xwmp_cthrd_yield(void);
void xwmp_cthrd_exit(xwer_t rc);
xwer_t xwmp_thrd_terminate(struct xwmp_tcb * tcb, xwer_t * trc);
void xwmp_cthrd_wait_exit(void);
xwer_t xwmp_thrd_intr(struct xwmp_tcb * tcb);
xwer_t xwmp_cthrd_sleep(xwtm_t * xwtm);
xwer_t xwmp_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc);
xwer_t xwmp_cthrd_freeze(void);
bool xwmp_cthrd_shld_frz(void);
bool xwmp_cthrd_shld_stop(void);
bool xwmp_cthrd_frz_shld_stop(bool * frozen);
xwer_t xwmp_thrd_migrate(struct xwmp_tcb * tcb, xwid_t dstcpu);

#endif /* xwos/mp/thrd.h */
