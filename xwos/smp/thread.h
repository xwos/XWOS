/**
 * @file
 * @brief XuanWuOS内核：线程
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_thread_h__
#define __xwos_smp_thread_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/object.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/sync/condition.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/wqn.h>
#include <xwos/smp/mutextree.h>
#include <xwos/smp/tt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_plwq;
struct xwos_rtwq;
struct xwos_scheduler;
struct xwos_tcb;

/**
 * @brief XWOS线程控制块
 */
struct xwos_tcb {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        struct xwos_scheduler * xwsd; /**< 当前线程所属于的调度器 */
        struct xwos_sdobj_stack_info stack; /**< 栈 */
        struct xwlib_bclst_node tcbnode; /**< 调度器线程链表中的节点 */

        /**< 线程状态 */
        struct xwlk_splk stlock; /**< 保护状态state的自旋锁 */
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
        struct xwos_ttn ttn; /**< 继承：时间树节点, 被锁tt->lock保护 */

        /* 阻塞状态 */
        struct xwos_wqn wqn; /**< 等待队列 */
        struct xwsync_cdt self; /**< 用于实现线程自身的pause与continue */

        /* 退出状态 */
        struct xwsync_cdt completion; /**< 线程退出时的事件信号量 */

        /* 线程优先级 */
        struct xwos_mtxtree mtxtree; /**< 互斥锁树：线程已经获得的所有互斥锁都
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
__xwos_code
xwer_t xwos_thrd_chprio_once(struct xwos_tcb * tcb, xwpr_t dprio,
                             struct xwlk_mtx ** pmtx);

__xwos_code
void xwos_thrd_chprio(struct xwos_tcb * tcb);

__xwos_code
xwer_t xwos_thrd_rq_add_head(struct xwos_tcb * tcb, xwpr_t prio);

__xwos_code
xwer_t xwos_thrd_rq_add_tail(struct xwos_tcb * tcb, xwpr_t prio);

__xwos_code
xwer_t xwos_thrd_rq_remove(struct xwos_tcb * tcb);

__xwos_code
xwer_t xwos_thrd_wakeup(struct xwos_tcb * tcb);

__xwos_code
void xwos_thrd_ttn_callback(void * entry);

__xwos_code
void xwos_thrd_wqn_callback(void * entry);

__xwos_code
xwer_t xwos_thrd_tt_add_locked(struct xwos_tcb * tcb, struct xwos_tt * xwtt,
                               xwtm_t expected, xwreg_t cpuirq);

__xwos_code
void xwos_thrd_eq_rtwq_locked(struct xwos_tcb * tcb,
                              struct xwos_rtwq * xwrtwq,
                              xwu16_t type,
                              xwpr_t dprio);

__xwos_code
void xwos_thrd_eq_plwq_locked(struct xwos_tcb * tcb,
                              struct xwos_plwq * xwplwq,
                              xwu16_t type);

__xwos_code
xwer_t xwos_thrd_do_unlock(void * lock, xwsq_t lktype,
                           void * lkdata, xwsz_t datanum);

__xwos_code
xwer_t xwos_thrd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm,
                         void * lkdata, xwsz_t datanum);

__xwos_code
xwer_t xwos_thrd_reqfrz_lic(struct xwos_tcb * tcb);

/******** XWOS Lib for BSP Adaptation Code ********/
__xwos_code
xwer_t xwos_thrd_exit_lic(struct xwos_tcb * tcb, xwer_t rc);

__xwos_code
xwer_t xwos_thrd_freeze_lic(struct xwos_tcb * tcb);

__xwos_code
xwer_t xwos_thrd_thaw_lic_pmlk(struct xwos_tcb * tcb);

__xwos_code
void xwos_thrd_immigrate_lic(struct xwos_tcb * tcb);

__xwos_code
xwer_t xwos_thrd_outmigrate_lic(struct xwos_tcb * tcb, xwid_t dstcpu);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SD_TCB_MEMSLICE) && (1 == XWSMPCFG_SD_TCB_MEMSLICE)
__xwos_init_code
xwer_t xwos_tcb_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif /* XWSMPCFG_SD_TCB_MEMSLICE */

__xwos_api
xwer_t xwos_thrd_init(struct xwos_tcb * tcb,
                      const char * name,
                      xwos_thrd_f mainfunc, void * arg,
                      xwstk_t * stack, xwsz_t stack_size,
                      xwpr_t priority, xwsq_t attr);

__xwos_api
xwer_t xwos_thrd_destroy(struct xwos_tcb * tcb);

__xwos_api
xwer_t xwos_thrd_create(struct xwos_tcb ** tcbpbuf, const char * name,
                        xwos_thrd_f manfunc, void * arg,
                        xwsz_t stack_size,
                        xwpr_t priority, xwsq_t attr);

__xwos_api
xwer_t xwos_thrd_delete(struct xwos_tcb * tcb);

__xwos_api
void xwos_cthrd_yield(void);

__xwos_api
void xwos_cthrd_exit(xwer_t rc);

__xwos_api
xwer_t xwos_thrd_terminate(struct xwos_tcb * tcb, xwer_t * trc);

__xwos_api
void xwos_cthrd_wait_exit(void);

__xwos_api
xwer_t xwos_thrd_intr(struct xwos_tcb * tcb);

__xwos_api
xwer_t xwos_thrd_continue(struct xwos_tcb * tcb);

__xwos_api
xwer_t xwos_cthrd_timedpause(void * lock, xwsq_t lktype,
                             void * lkdata, xwsz_t datanum,
                             xwtm_t * xwtm, xwsq_t * lkst);

__xwos_api
xwer_t xwos_cthrd_sleep(xwtm_t * xwtm);

__xwos_api
xwer_t xwos_cthrd_sleep_from(xwtm_t * origin, xwtm_t inc);

__xwos_api
xwer_t xwos_cthrd_freeze(void);

__xwos_api
bool xwos_cthrd_shld_frz(void);

__xwos_api
bool xwos_cthrd_shld_stop(void);

__xwos_api
bool xwos_cthrd_frz_shld_stop(bool * frozen);

__xwos_api
xwer_t xwos_thrd_migrate(struct xwos_tcb * tcb, xwid_t dstcpu);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：增加对象的引用计数
 * @param tcb: (I) 线程控制块对象指针
 * @return 错误码
 */
static __xwos_inline_api
xwer_t xwos_thrd_grab(struct xwos_tcb * tcb)
{
        return xwos_object_grab(&tcb->xwobj);
}

/**
 * @brief XWOS API：减少对象的引用计数
 * @param tcb: (I) 线程控制块对象指针
 * @return 错误码
 */
static __xwos_inline_api
xwer_t xwos_thrd_put(struct xwos_tcb * tcb)
{
        return xwos_object_put(&tcb->xwobj);
}

/**
 * @brief XWOS API：暂停当前线程
 * @param lock: (I) 锁的地址
 * @param lktype: (I) 锁的类型
 * @param lkdata: (I) 锁的数据
 * @param datanum: (I) 锁的数据数量
 * @param lkst: (O) 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 * @retval OK: OK
 * @retval -EINTR: 暂停过程被中断
 * @retval -ENOTINTHRD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
static __xwos_inline_api
xwer_t xwos_cthrd_pause(void * lock, xwsq_t lktype,
                        void * lkdata, xwsz_t datanum,
                        xwsq_t * lkst)
{
        xwtm_t expected;
        expected = XWTM_MAX;
        return xwos_cthrd_timedpause(lock, lktype,
                                     lkdata, datanum,
                                     &expected, lkst);
}

#endif /* xwos/smp/thread.h */
