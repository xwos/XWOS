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

#ifndef __xwos_mp_thd_h__
#define __xwos_mp_thd_h__

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

struct xwmp_plwq;
struct xwmp_rtwq;
struct xwmp_skd;
struct xwmp_thd;

/**
 * @brief XWOS MP线程对象
 */
struct xwmp_thd {
        struct xwos_object xwobj; /**< C语言面向对象：继承struct xwos_object */
        struct xwmp_skd * xwskd; /**< 当前线程所属于的调度器 */
        struct xwmp_skd_stack_info stack; /**< 栈 */
        struct xwlib_bclst_node thdnode; /**< 调度器线程链表中的节点 */

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

#if defined(XWMPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWMPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
        /* 线程私有数据 */
        void * data[XWMPCFG_SKD_THD_LOCAL_DATA_NUM];
#endif

#if defined(XWMDCFG_libc_newlibac) && (1 == XWMDCFG_libc_newlibac)
        struct {
                int __errno;
        } newlib;
#endif
};

xwer_t xwmp_thd_chprio_once(struct xwmp_thd * thd, xwpr_t dprio,
                            struct xwmp_mtx ** pmtx);
void xwmp_thd_chprio(struct xwmp_thd * thd);
xwer_t xwmp_thd_rq_add_head(struct xwmp_thd * thd, xwpr_t prio);
xwer_t xwmp_thd_rq_add_tail(struct xwmp_thd * thd, xwpr_t prio);
xwer_t xwmp_thd_rq_remove(struct xwmp_thd * thd);
xwer_t xwmp_thd_wakeup(struct xwmp_thd * thd);
void xwmp_thd_ttn_callback(void * entry);
void xwmp_thd_wqn_callback(void * entry);
xwer_t xwmp_thd_tt_add_locked(struct xwmp_thd * thd, struct xwmp_tt * xwtt,
                              xwtm_t expected, xwreg_t cpuirq);
void xwmp_thd_eq_rtwq_locked(struct xwmp_thd * thd,
                             struct xwmp_rtwq * xwrtwq,
                             xwu16_t type,
                             xwpr_t dprio);
void xwmp_thd_eq_plwq_locked(struct xwmp_thd * thd,
                             struct xwmp_plwq * xwplwq,
                             xwu16_t type);
xwer_t xwmp_thd_do_unlock(void * lock, xwsq_t lktype, void * lkdata);
xwer_t xwmp_thd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm, void * lkdata);
xwer_t xwmp_thd_reqfrz_lic(struct xwmp_thd * thd);
xwer_t xwmp_thd_thaw_lic_pmlk(struct xwmp_thd * thd);

xwer_t xwmp_thd_exit_lic(struct xwmp_thd * thd, xwer_t rc);
xwer_t xwmp_thd_freeze_lic(struct xwmp_thd * thd);
void xwmp_thd_immigrate_lic(struct xwmp_thd * thd);
xwer_t xwmp_thd_outmigrate_lic(struct xwmp_thd * thd, xwid_t dstcpu);

#if defined(XWMPCFG_SKD_THD_MEMSLICE) && (1 == XWMPCFG_SKD_THD_MEMSLICE)
xwer_t xwmp_thd_cache_init(xwptr_t zone_origin, xwsz_t zone_size);
#endif

xwer_t xwmp_thd_init(struct xwmp_thd * thd,
                     const char * name,
                     xwmp_thd_f mainfunc, void * arg,
                     xwstk_t * stack, xwsz_t stack_size,
                     xwpr_t priority, xwsq_t attr);
xwer_t xwmp_thd_fini(struct xwmp_thd * thd);
xwer_t xwmp_thd_create(struct xwmp_thd ** thdpbuf, const char * name,
                       xwmp_thd_f manfunc, void * arg,
                       xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr);
xwer_t xwmp_thd_delete(struct xwmp_thd * thd);
xwer_t xwmp_thd_acquire(struct xwmp_thd * thd, xwsq_t tik);
xwer_t xwmp_thd_release(struct xwmp_thd * thd, xwsq_t tik);
xwer_t xwmp_thd_grab(struct xwmp_thd * thd);
xwer_t xwmp_thd_put(struct xwmp_thd * thd);
void xwmp_cthd_yield(void);
void xwmp_cthd_exit(xwer_t rc);
xwer_t xwmp_thd_stop(struct xwmp_thd * thd, xwer_t * trc);
xwer_t xwmp_thd_cancel(struct xwmp_thd * thd);
xwer_t xwmp_thd_join(struct xwmp_thd * thd, xwer_t * trc);
xwer_t xwmp_thd_detach(struct xwmp_thd * thd);
xwer_t xwmp_thd_intr(struct xwmp_thd * thd);
xwer_t xwmp_cthd_sleep(xwtm_t * xwtm);
xwer_t xwmp_cthd_sleep_from(xwtm_t * origin, xwtm_t inc);
xwer_t xwmp_cthd_freeze(void);
bool xwmp_cthd_shld_frz(void);
bool xwmp_cthd_shld_stop(void);
bool xwmp_cthd_frz_shld_stop(bool * frozen);
xwer_t xwmp_thd_migrate(struct xwmp_thd * thd, xwid_t dstcpu);

#if defined(XWMPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWMPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
xwer_t xwmp_thd_set_data(struct xwmp_thd * thd, xwsq_t pos, void * data);
xwer_t xwmp_thd_get_data(struct xwmp_thd * thd, xwsq_t pos, void ** databuf);
xwer_t xwmp_cthd_set_data(xwsq_t pos, void * data);
xwer_t xwmp_cthd_get_data(xwsq_t pos, void ** databuf);
#endif

#endif /* xwos/mp/thd.h */
