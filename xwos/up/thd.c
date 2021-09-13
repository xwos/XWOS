/**
 * @file
 * @brief 玄武OS UP内核：线程
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/lib/rbtree.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWUPCFG_SKD_THD_STDC_MM) && (1 == XWUPCFG_SKD_THD_STDC_MM)
#  include <stdlib.h>
#endif
#include <xwos/ospl/irq.h>
#include <xwos/ospl/skd.h>
#include <xwos/up/lock/seqlock.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/rtrq.h>
#if (1 == XWUPRULE_SKD_WQ_RT)
#  include <xwos/up/rtwq.h>
#endif
#if (1 == XWUPRULE_SKD_WQ_PL)
#  include <xwos/up/plwq.h>
#endif
#include <xwos/up/tt.h>
#include <xwos/up/thd.h>
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
#  include <xwos/up/lock/mtx.h>
#  include <xwos/up/mtxtree.h>
#endif
#if defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)
#  include <xwos/up/lock/fakemtx.h>
#endif
#if defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
#  include <xwos/up/sync/plsem.h>
#endif
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
#  include <xwos/up/sync/rtsem.h>
#endif
#if defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND)
#  include <xwos/up/sync/cond.h>
#endif

#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
extern __xwup_code
xwer_t board_thd_stack_pool_alloc(xwsz_t stack_size, xwstk_t ** membuf);

extern __xwup_code
xwer_t board_thd_stack_pool_free(xwstk_t * stk);
#endif

#if defined(BRDCFG_XWSKD_THD_POSTINIT_HOOK) && (1 == BRDCFG_XWSKD_THD_POSTINIT_HOOK)
extern
void board_thd_postinit_hook(struct xwup_thd * thd);
#endif

static __xwup_code
struct xwup_thd * xwup_thd_alloc(void);

static __xwup_code
void xwup_thd_free(struct xwup_thd * thd);

static __xwup_code
xwstk_t * xwup_thd_stack_alloc(xwsz_t stack_size);

static __xwup_code
xwer_t xwup_thd_stack_free(xwstk_t * stk);

static __xwup_code
void xwup_thd_activate(struct xwup_thd * thd,
                       const char * name,
                       xwup_thd_f mainfunc, void * arg,
                       xwstk_t * stack, xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr);

static __xwup_code
void xwup_thd_launch(struct xwup_thd * thd, xwup_thd_f mainfunc, void * arg);

#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
static __xwup_code
xwer_t xwup_thd_stop_unlock_cb(struct xwup_thd * thd);
#endif

/**
 * @brief 从线程对象缓存中申请一个对象
 * @return 线程对象的指针
 */
static __xwup_code
struct xwup_thd * xwup_thd_alloc(void)
{
#if defined(XWUPCFG_SKD_THD_STDC_MM) && (1 == XWUPCFG_SKD_THD_STDC_MM)
        struct xwup_thd * thd;

        thd = malloc(sizeof(struct xwup_thd));
        if (NULL == thd) {
                thd = err_ptr(-ENOMEM);
        }
        return thd;
#else
        union {
                struct xwup_thd * thd;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_thd), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.thd = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.thd;
#endif
}

/**
 * @brief 释放线程对象
 * @param[in] thd: 线程对象的指针
 */
static __xwup_code
void xwup_thd_free(struct xwup_thd * thd)
{
#if defined(XWUPCFG_SKD_THD_STDC_MM) && (1 == XWUPCFG_SKD_THD_STDC_MM)
        free(thd);
#else
        xwmm_kma_free(thd);
#endif
}

/**
 * @brief 动态分配线程栈
 * @param[in] stack_size: 线程栈的大小
 * @return 线程栈的首地址或指针类型的错误码或空指针
 */
static __xwup_code
xwstk_t * xwup_thd_stack_alloc(xwsz_t stack_size)
{
#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        union {
                xwstk_t * stkbase;
                void * anon;
        } mem;
        xwer_t rc;

        rc = board_thd_stack_pool_alloc(stack_size, &mem.stkbase);
        if (rc < 0) {
                mem.stkbase = err_ptr(rc);
        }/* else {} */
        return mem.stkbase;
#elif defined(XWUPCFG_SKD_THD_STDC_MM) && (1 == XWUPCFG_SKD_THD_STDC_MM)
        xwstk_t * stkbase;

        stkbase = malloc(stack_size);
        if (NULL == stkbase) {
                stkbase = err_ptr(-ENOMEM);
        }
        return stkbase;
#else
        union {
                xwstk_t * stkbase;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(stack_size, XWMMCFG_STACK_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.stkbase = err_ptr(rc);
        }/* else {} */
        return mem.stkbase;
#endif
}

/**
 * @brief 释放动态分配的线程栈内存
 * @param[in] stk: 线程栈的首地址
 * @return 错误码
 */
static __xwup_code
xwer_t xwup_thd_stack_free(xwstk_t * stk)
{
#if defined(BRDCFG_XWSKD_THD_STACK_POOL) && (1 == BRDCFG_XWSKD_THD_STACK_POOL)
        return board_thd_stack_pool_free(stk);
#elif defined(XWUPCFG_SKD_THD_STDC_MM) && (1 == XWUPCFG_SKD_THD_STDC_MM)
        free(stk);
        return XWOK;
#else
        return xwmm_kma_free(stk);
#endif
}

/**
 * @brief 激活线程对象
 * @param[in] thd: 线程对象的指针
 * @param[in] name: 线程的名字
 * @param[in] mainfunc: 线程函数的指针
 * @param[in] arg: 线程函数的参数
 * @param[in] stack: 线程栈的首地址指针
 * @param[in] stack_size: 线程栈的大小，以字节(byte)为单位
 * @param[in] priority: 线程的优先级
 * @param[in] attr: 线程属性，取值：@ref xwup_skdobj_attr_em中的一项
 */
static __xwup_code
void xwup_thd_activate(struct xwup_thd * thd,
                       const char * name,
                       xwup_thd_f mainfunc, void * arg,
                       xwstk_t * stack, xwsz_t stack_size,
                       xwpr_t priority, xwsq_t attr)
{
        thd->state = XWUP_SKDOBJ_DST_UNKNOWN;
        thd->attribute = attr;
        if (XWUP_SKDATTR_DETACHED & attr) {
                thd->state |= XWUP_SKDOBJ_DST_DETACHED;
        }
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        xwlib_bclst_init_node(&thd->frznode);
#endif
        xwlib_bclst_init_node(&thd->rqnode);
        xwup_ttn_init(&thd->ttn, (xwptr_t)thd, XWUP_TTN_TYPE_THD);
        xwup_wqn_init(&thd->wqn, thd);
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
        xwup_mtxtree_init(&thd->mtxtree);
#endif

        /* 优先级 */
        if (priority >= XWUP_SKD_PRIORITY_RT_NUM) {
                priority = XWUP_SKD_PRIORITY_RT_MAX;
        } else if (priority <= XWUP_SKD_PRIORITY_INVALID) {
                priority = XWUP_SKD_PRIORITY_RT_MIN;
        }
        thd->prio.s = priority;
        thd->prio.d = priority;

        /* 栈信息 */
        thd->stack.name = name;
        stack_size = (stack_size + XWMM_ALIGNMENT_MASK) & (~XWMM_ALIGNMENT_MASK);
        thd->stack.size = stack_size;
        thd->stack.base = stack;
#if defined(XWMMCFG_FD_STACK) && (1 == XWMMCFG_FD_STACK)
        thd->stack.sp = thd->stack.base + (stack_size / sizeof(xwstk_t));
#elif defined(XWMMCFG_ED_STACK) && (1 == XWMMCFG_ED_STACK)
        thd->stack.sp = thd->stack.base + (stack_size / sizeof(xwstk_t)) - 1;
#elif defined(XWMMCFG_FA_STACK) && (1 == XWMMCFG_FA_STACK)
        thd->stack.sp = thd->stack.base - 1;
#elif defined(XWMMCFG_EA_STACK) && (1 == XWMMCFG_EA_STACK)
        thd->stack.sp = thd->stack.base;
#else
#  error "Unknown stack type!"
#endif
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        xwup_cond_init(&thd->completion);
#endif
        xwlib_bclst_init_node(&thd->thdnode);

#if defined(XWUPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWUPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
        for (xwsq_t i = 0; i < XWUPCFG_SKD_THD_LOCAL_DATA_NUM; i++) {
                thd->data[i] = NULL;
        }
#endif
#if defined(XWMDCFG_libc_newlibac) && (1 == XWMDCFG_libc_newlibac)
        thd->newlib.__errno = XWOK;
#endif

#if defined(BRDCFG_XWSKD_THD_POSTINIT_HOOK) && (1 == BRDCFG_XWSKD_THD_POSTINIT_HOOK)
        board_thd_postinit_hook(thd);
#endif

        if (mainfunc) {
                xwup_thd_launch(thd, mainfunc, arg);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_STANDBY);
        }
}

/**
 * @brief 使得线程对象无效
 * @param[in] thd: 线程对象的指针
 */
static __xwup_code
void xwup_thd_deactivate(struct xwup_thd * thd)
{
        XWOS_UNUSED(thd);
}

/**
 * @brief 加载线程
 * @param[in] thd: 线程对象的指针
 * @param[in] mainfunc: 线程主函数
 * @param[in] arg: 线程主函数的参数
 */
static __xwup_code
void xwup_thd_launch(struct xwup_thd * thd, xwup_thd_f mainfunc, void * arg)
{
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        /* add to ready queue */
        xwskd = xwup_skd_get_lc();
        thd->stack.main = mainfunc;
        thd->stack.arg = arg;
        xwospl_skd_init_stack(&thd->stack, xwup_cthd_exit, thd->attribute);
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_STANDBY);
        xwup_thd_rq_add_tail(thd);
        xwskd->thd_num++;
        xwlib_bclst_add_tail(&xwskd->thdlist, &thd->thdnode);
        xwospl_cpuirq_restore_lc(cpuirq);
}

__xwup_api
xwer_t xwup_thd_init(struct xwup_thd * thd,
                     const char * name,
                     xwup_thd_f mainfunc, void * arg,
                     xwstk_t * stack, xwsz_t stack_size,
                     xwpr_t priority, xwsq_t attr)
{
        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((NULL != stack), "nullptr", -EFAULT);

        xwup_thd_activate(thd, name,
                          mainfunc, arg,
                          stack, stack_size,
                          priority, attr);
        return XWOK;
}

__xwup_api
xwer_t xwup_thd_fini(struct xwup_thd * thd)
{
        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwup_thd_deactivate(thd);
        return XWOK;
}

__xwup_api
xwer_t xwup_thd_create(struct xwup_thd ** thdpbuf,
                       const char * name,
                       xwup_thd_f mainfunc, void * arg,
                       xwsz_t stack_size, xwpr_t priority,
                       xwsq_t attr)
{
        struct xwup_thd * thd;
        xwstk_t * stk;
        xwer_t rc;

        XWOS_VALIDATE((thdpbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE((mainfunc), "nullptr", -EFAULT);

        stk = xwup_thd_stack_alloc(stack_size);
        if (__xwcc_unlikely(is_err(stk))) {
                rc = ptr_err(stk);
                goto err_stack_alloc;
        }

        thd = xwup_thd_alloc();
        if (is_err(thd)) {
                rc = ptr_err(thd);
                goto err_thd_alloc;
        }

        xwup_thd_activate(thd, name,
                          mainfunc, arg,
                          stk, stack_size,
                          priority, attr);
        *thdpbuf = thd;
        return XWOK;

err_thd_alloc:
        xwup_thd_stack_free(stk);
err_stack_alloc:
        *thdpbuf = NULL;
        return rc;
}

__xwup_api
xwer_t xwup_thd_delete(struct xwup_thd * thd)
{
        xwstk_t * base;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwup_thd_deactivate(thd);
        base = ((struct xwup_thd *)thd)->stack.base;
        xwup_thd_stack_free(base);
        xwup_thd_free(thd);
        return XWOK;
}

__xwup_api
void xwup_cthd_exit(xwer_t rc)
{
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        xwospl_thd_exit_lc(cthd, rc);
#else
        xwtm_t time;

        XWOS_UNUSED(rc);
        time = XWTM_MAX;
        while (true) {
                xwup_cthd_sleep(&time);
        }
#endif
}

/**
 * @brief 执行退出线程
 * @param[in] thd: 线程对象的指针
 * @param[in] rc: 线程的返回值
 */
__xwup_code
xwer_t xwup_thd_exit_lic(struct xwup_thd * thd, xwer_t rc)
{
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_EXITING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_STANDBY);
        thd->stack.arg = err_ptr(rc);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_cond_broadcast(&thd->completion);
        xwospl_cpuirq_disable_lc();
        xwlib_bclst_del_init(&thd->thdnode);
        xwskd->thd_num--;
        if (XWUP_SKDATTR_DETACHED & thd->attribute) {
                xwlib_bclst_add_tail(&xwskd->thdelist, &thd->thdnode);
        }
#  if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_notify_allfrz_lic();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_req_swcx();
        }
#  else
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_req_swcx();
#  endif
#else
        XWOS_UNUSED(thd);
        XWOS_UNUSED(rc);
#endif
        return XWOK;
}

#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
/**
 * @brief @ref xwup_thd_stop()中使用的回调锁的解锁函数
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 */
static __xwup_code
xwer_t xwup_thd_stop_unlock_cb(struct xwup_thd * thd)
{
        xwup_thd_intr(thd);
        return XWOK;
}

__xwup_api
xwer_t xwup_thd_stop(struct xwup_thd * thd, xwer_t * trc)
{
        struct xwup_thd * cthd;
        struct xwos_cblk lockcb;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);
        cthd = xwup_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_stop_self;
        }

        lockcb.unlock = (xwer_t (*)(void *))xwup_thd_stop_unlock_cb;
        lockcb.lock = NULL;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_DETACHED & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINVAL;
        } else if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_DETACHED);
                xwospl_cpuirq_restore_lc(cpuirq);
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                rc = xwup_thd_delete(thd);
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_EXITING);
                rc = xwup_cond_wait(&thd->completion,
                                    &lockcb, XWOS_LK_CALLBACK, thd,
                                    &lkst);
                if (XWOK == rc) {
                        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_DETACHED);
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                }
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWOK == rc) {
                        rc = xwup_thd_delete(thd);
                }
        }

err_stop_self:
        return rc;
}

__xwup_api
xwer_t xwup_thd_cancel(struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_EXITING);
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_intr(thd);
                rc = XWOK;
        }
        return rc;
}

__xwup_api
xwer_t xwup_thd_join(struct xwup_thd * thd, xwer_t * trc)
{
        struct xwup_thd * cthd;
        xwsq_t lkst;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        cthd = xwup_skd_get_cthd_lc();
        if (thd == cthd) {
                rc = -EPERM;
                goto err_join_self;
        }
        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_DETACHED & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = -EINVAL;
        } else if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                rc = XWOK;
                if (!is_err_or_null(trc)) {
                        *trc = (xwer_t)thd->stack.arg;
                }
                xwup_thd_delete(thd);
        } else {
                rc = xwup_cond_wait(&thd->completion,
                                    NULL, XWOS_LK_NONE, NULL,
                                    &lkst);
                if (XWOK == rc) {
                        if (!is_err_or_null(trc)) {
                                *trc = (xwer_t)thd->stack.arg;
                        }
                } else {
                }
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWOK == rc) {
                        xwup_thd_delete(thd);
                }
        }

err_join_self:
        return rc;
}

__xwup_api
xwer_t xwup_thd_detach(struct xwup_thd * thd)
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((NULL != thd), "nullptr", -EFAULT);

        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_DETACHED);
        if (XWUP_SKDOBJ_DST_STANDBY & thd->state) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_thd_delete(thd);
        } else {
                thd->attribute |= XWUP_SKDATTR_DETACHED;
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return XWOK;
}

#else
__xwup_api
xwer_t xwup_thd_stop(struct xwup_thd * thd, xwer_t * trc)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}

__xwup_api
xwer_t xwup_thd_cancel(struct xwup_thd * thd)
{
        XWOS_UNUSED(thd);
        return -ENOSYS;
}

__xwup_api
xwer_t xwup_thd_join(struct xwup_thd * thd, xwer_t * trc)
{
        XWOS_UNUSED(thd);
        XWOS_UNUSED(trc);
        return -ENOSYS;
}

__xwup_api
xwer_t xwup_thd_detach(struct xwup_thd * thd)
{
        XWOS_UNUSED(thd);
        return -ENOSYS;
}
#endif

#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
/**
 * @brief 改变线程的动态优先级一次
 * @param[in] thd: 线程对象的指针
 * @param[in] dprio: 动态优先级
 * @param[out] pmtx: 指向缓冲区的指针，通过此缓冲区返回互斥锁对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESTALE: 状态已经被其他CPU改变
 * @note
 * - 此函数将线程(thd)的优先级改成(dprio)，并返回接下来需要修改优先级的互斥锁的指针。
 */
__xwup_code
void xwup_thd_chprio_once(struct xwup_thd * thd, xwpr_t dprio,
                          struct xwup_mtx ** pmtx)
{
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_FROZEN) & thd->state) {
                thd->prio.d = dprio;
        } else if (XWUP_SKDOBJ_DST_READY & thd->state) {
                if (thd->prio.d != dprio) {
                        xwup_thd_rq_remove(thd);
                        thd->prio.d = dprio;
                        xwup_thd_rq_add_tail(thd);
                }
        } else if (XWUP_SKDOBJ_DST_BLOCKING & thd->state) {
                if (thd->prio.d == dprio) {
                } else if (XWUP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_baseof(thd->wqn.wq, struct xwup_mtx, rtwq);
                        xwup_rtwq_remove(&mtx->rtwq, &thd->wqn);
                        thd->prio.d = dprio;
                        xwup_rtwq_add(&mtx->rtwq, &thd->wqn, dprio);
                        *pmtx = mtx;
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
                } else if (XWUP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_baseof(thd->wqn.wq, struct xwup_rtsem, rtwq);
                        xwup_rtwq_remove(&sem->rtwq, &thd->wqn);
                        thd->prio.d = dprio;
                        xwup_rtwq_add(&sem->rtwq, &thd->wqn, dprio);
                } else {
#endif
                        thd->prio.d = dprio;
                }
        } else {
                XWOS_BUG_ON(XWUP_SKDOBJ_DST_STANDBY & thd->state);
                thd->prio.d = dprio;
        }
}

/**
 * @brief 重新设定线程的动态优先级
 * @param[in] thd: 线程对象的指针
 */
__xwup_code
void xwup_thd_chprio(struct xwup_thd * thd)
{
        struct xwup_mtx * unused;
        struct xwup_mtxtree * mt;
        xwpr_t dprio;

        mt = &thd->mtxtree;
        dprio = thd->prio.s > mt->maxprio ? thd->prio.s : mt->maxprio;
        xwup_thd_chprio_once(thd, dprio, &unused);
}
#endif

/**
 * @brief 将线程加入到调度器就绪队列的头部
 * @param[in] thd: 线程对象的指针
 * @param[in] prio: 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 */
__xwup_code
xwer_t xwup_thd_rq_add_head(struct xwup_thd * thd)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_READY) & thd->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_READY);
                xwup_rtrq_add_head(xwrtrq, thd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程加入到调度器就绪队列的尾部
 * @param[in] thd: 线程对象的指针
 * @param[in] prio: 动态优先级
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EEXIST: 线程已经存在
 */
__xwup_code
xwer_t xwup_thd_rq_add_tail(struct xwup_thd * thd)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        if ((XWUP_SKDOBJ_DST_RUNNING | XWUP_SKDOBJ_DST_READY) & thd->state) {
                rc = -EEXIST;
        } else {
                xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_READY);
                xwup_rtrq_add_tail(xwrtrq, thd);
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief 将线程从就绪队列中删除
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 没有这个线程
 */
__xwup_code
void xwup_thd_rq_remove(struct xwup_thd * thd)
{
        struct xwup_skd * xwskd;
        struct xwup_rtrq * xwrtrq;

        xwskd = xwup_skd_get_lc();
        xwrtrq = &xwskd->rq.rt;
        xwup_rtrq_remove(xwrtrq, thd);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_READY);
}

__xwup_api
xwer_t xwup_thd_intr(struct xwup_thd * thd)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        if (XWUP_SKDOBJ_DST_UNINTERRUPTED & thd->state) {
                rc = -EPERM;
                xwospl_cpuirq_restore_lc(cpuirq);
        } else if (XWUP_SKDOBJ_DST_BLOCKING & thd->state) {
                xwup_wqn_f cb;

                if (XWUP_WQTYPE_NULL == thd->wqn.type) {
                        thd->wqn.wq = NULL;
                        thd->wqn.type = XWUP_WQTYPE_UNKNOWN;
                        thd->wqn.reason = XWUP_WQN_REASON_INTR;
                        cb = thd->wqn.cb;
                        thd->wqn.cb = NULL;
                        xwospl_cpuirq_restore_lc(cpuirq);
                        cb(thd);
                        rc = XWOK;
#if defined(XWUPCFG_SYNC_PLSEM) && (1 == XWUPCFG_SYNC_PLSEM)
                } else if (XWUP_WQTYPE_PLSEM == thd->wqn.type) {
                        struct xwup_plsem * sem;

                        sem = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_plsem,
                                          plwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_plsem_intr(sem, &thd->wqn);
#endif
#if defined(XWUPCFG_SYNC_RTSEM) && (1 == XWUPCFG_SYNC_RTSEM)
                } else if (XWUP_WQTYPE_RTSEM == thd->wqn.type) {
                        struct xwup_rtsem * sem;

                        sem = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_rtsem,
                                          rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_rtsem_intr(sem, &thd->wqn);
#endif
#if defined(XWUPCFG_SYNC_COND) && (1 == XWUPCFG_SYNC_COND)
                } else if (XWUP_WQTYPE_COND == thd->wqn.type) {
                        struct xwup_cond * sem;

                        sem = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_cond,
                                          wq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_cond_intr(sem, &thd->wqn);
#endif
#if defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)
                } else if (XWUP_WQTYPE_MTX == thd->wqn.type) {
                        struct xwup_mtx * mtx;

                        mtx = xwcc_baseof(thd->wqn.wq,
                                          struct xwup_mtx,
                                          rtwq);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = xwup_mtx_intr(mtx, thd);
#endif
                } else {
                        xwospl_cpuirq_restore_lc(cpuirq);
                        rc = -EBUG;
                        XWOS_BUG();
                }
        } else if (XWUP_SKDOBJ_DST_SLEEPING & thd->state) {
                struct xwup_skd * xwskd;
                struct xwup_tt * xwtt;

                xwskd = xwup_skd_get_lc();
                xwtt = &xwskd->tt;
                xwup_sqlk_wr_lock(&xwtt->lock);
                rc = xwup_tt_remove_locked(xwtt, &thd->ttn);
                if (XWOK == rc) {
                        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
                        xwup_sqlk_wr_unlock(&xwtt->lock);
                        xwup_thd_wakeup(thd);
                        xwospl_cpuirq_restore_lc(cpuirq);
                        xwup_skd_chkpmpt();
                } else {
                        xwup_sqlk_wr_unlock(&xwtt->lock);
                        xwospl_cpuirq_restore_lc(cpuirq);
                }
        } else {
                rc = -EINVAL;
                xwospl_cpuirq_restore_lc(cpuirq);
        }
        return rc;
}

/**
 * @brief 线程的时间树节点回调函数
 * @param[in] entry: 线程对象的地址
 */
__xwup_code
void xwup_thd_ttn_callback(void * entry)
{
        struct xwup_thd * thd;
        xwreg_t cpuirq;

        thd = entry;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_SLEEPING);
        xwup_thd_wakeup(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief 将线程加入到时间树上
 * @param[in] thd: 线程对象的指针
 * @param[in] xwtt: 时间树的指针
 * @param[in] expected: 期望被唤醒的时间
 * @param[in] cpuirq: 本地CPU的中断标志
 * @return 错误码
 * @note
 * - 此函数只能在取得写锁xwtt->lock以及关闭本地CPU的中断时才可调用。
 */
__xwup_code
xwer_t xwup_thd_tt_add_locked(struct xwup_thd * thd, struct xwup_tt * xwtt,
                              xwtm_t expected, xwreg_t cpuirq)
{
        xwer_t rc;

        /* add to time tree */
        thd->ttn.wkup_xwtm = expected;
        thd->ttn.wkuprs = XWUP_TTN_WKUPRS_UNKNOWN;
        thd->ttn.xwtt = xwtt;
        thd->ttn.cb = xwup_thd_ttn_callback;
        rc = xwup_tt_add_locked(xwtt, &thd->ttn, cpuirq);
        return rc;
}

/**
 * @brief 线程的等待队列节点回调函数
 * @param[in] entry: 线程对象的地址
 */
__xwup_code
void xwup_thd_wqn_callback(void * entry)
{
        struct xwup_thd * thd;
        xwreg_t cpuirq;

        thd = entry;
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state,
                  XWUP_SKDOBJ_DST_BLOCKING | XWUP_SKDOBJ_DST_UNINTERRUPTED);
        xwup_thd_wakeup(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
        xwup_skd_chkpmpt();
}

#if (1 == XWUPRULE_SKD_WQ_RT)
/**
 * @brief 将线程加入到实时（红黑树）等待队列中
 * @param[in] thd: 线程对象的指针
 * @param[in] xwrtwq: 实时（红黑树）等待队列
 * @param[in] type: 等待队列的类型
 */
__xwup_code
void xwup_thd_eq_rtwq(struct xwup_thd * thd, struct xwup_rtwq * xwrtwq,
                      xwu16_t type)
{
        thd->wqn.wq = xwrtwq;
        thd->wqn.type = type;
        thd->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
        thd->wqn.cb = xwup_thd_wqn_callback;
        xwup_rtwq_add(xwrtwq, &thd->wqn, thd->prio.d);
}
#endif

#if (1 == XWUPRULE_SKD_WQ_PL)
/**
 * @brief 将线程加入到管道（双循环链表）等待队列中
 * @param[in] thd: 线程对象的指针
 * @param[in] xwplwq: 管道（双循环链表）等待队列
 * @param[in] type: 等待队列的类型
 */
__xwup_code
void xwup_thd_eq_plwq(struct xwup_thd * thd, struct xwup_plwq * xwplwq,
                      xwu16_t type)
{
        thd->wqn.wq = xwplwq;
        thd->wqn.type = type;
        thd->wqn.reason = XWUP_WQN_REASON_UNKNOWN;
        thd->wqn.cb = xwup_thd_wqn_callback;
        xwup_plwq_add_tail(xwplwq, &thd->wqn);
}
#endif

__xwup_api
void xwup_cthd_yield(void)
{
        struct xwup_thd * cthd;
        xwreg_t cpuirq;

        cthd = xwup_skd_get_cthd_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwup_thd_rq_add_tail(cthd);
        xwospl_cpuirq_enable_lc();
        xwup_skd_req_swcx();
        xwospl_cpuirq_restore_lc(cpuirq);
}

#if (1 == XWUPRULE_SKD_THD_DO_UNLOCK)
/**
 * @brief 解锁给定类型的锁
 * @param[in] lock: 锁的地址
 * @param[in] lktype: 锁的类型
 * @param[in] lkdata: 锁的数据
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 */
__xwup_code
xwer_t xwup_thd_do_unlock(void * lock, xwsq_t lktype, void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
        case XWOS_LK_MTX_UNINTR:
#if ((defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
     (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)))
                rc = xwup_mtx_unlock(ulk.xwup.mtx);
#endif
                break;
        case XWOS_LK_SPLK:
                xwup_splk_unlock(ulk.xwup.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwup_sqlk_wr_unlock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwup_sqlk_rdex_unlock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (ulk.cb->unlock) {
                        rc = ulk.cb->unlock(lkdata);
                }
                break;
        case XWOS_LK_NONE:
        default:
                break;
        }
        return rc;
}
#endif

#if (1 == XWUPRULE_SKD_THD_DO_LOCK)
/**
 * @brief 锁定给定类型的锁
 * @param[in] lock: 锁的地址
 * @param[in] lktype: 锁的类型
 * @param[in] lkdata: 锁的数据
 * @param[in,out] xwtm: 指向缓冲区的指针，此缓冲区：
 * + (I) 作为输入时，表示期望的阻塞等待时间
 * + (O) 作为输出时，返回剩余的期望时间
 * @param[out] lkst: 指向缓冲区的指针，通过此缓冲区返回锁的状态
 * @return 错误码
 */
__xwup_code
xwer_t xwup_thd_do_lock(void * lock, xwsq_t lktype, xwtm_t * xwtm,
                        void * lkdata)
{
        xwer_t rc;
        union xwos_ulock ulk;

        ulk.anon = lock;

        rc = XWOK;
        switch (lktype) {
        case XWOS_LK_MTX:
#if ((defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
     (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)))
                if (xwtm) {
                        rc = xwup_mtx_timedlock(ulk.xwup.mtx, xwtm);
                } else {
                        rc = xwup_mtx_lock(ulk.xwup.mtx);
                }
#endif
                break;
        case XWOS_LK_MTX_UNINTR:
#if ((defined(XWUPCFG_LOCK_MTX) && (1 == XWUPCFG_LOCK_MTX)) || \
     (defined(XWUPCFG_LOCK_FAKEMTX) && (1 == XWUPCFG_LOCK_FAKEMTX)))
                rc = xwup_mtx_lock_unintr(ulk.xwup.mtx);
#endif
                break;
        case XWOS_LK_SPLK:
                xwup_splk_lock(ulk.xwup.splk);
                break;
        case XWOS_LK_SQLK_WR:
                xwup_sqlk_wr_lock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_SQLK_RDEX:
                xwup_sqlk_rdex_lock(ulk.xwup.sqlk);
                break;
        case XWOS_LK_CALLBACK:
                if (ulk.cb->lock) {
                        rc = ulk.cb->lock(lkdata);
                }
                break;
        case XWOS_LK_NONE:
        default:
                XWOS_UNUSED(xwtm);
                break;
        }
        return rc;
}
#endif

__xwup_api
xwer_t xwup_cthd_sleep(xwtm_t * xwtm)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_thd * cthd;
        xwtm_t expected, currtick;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);

        if (__xwcc_unlikely(xwtm_cmp(*xwtm, 0) <= 0)) {
                rc = -ETIMEDOUT;
        } else {
                cthd = xwup_skd_get_cthd_lc();
                xwskd = xwup_skd_get_lc();
                xwtt = &xwskd->tt;
                hwt = &xwtt->hwt;
                currtick = xwup_syshwt_get_timetick(hwt);
                expected = xwtm_add_safely(currtick, *xwtm);
                xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                rc = xwup_skd_wakelock_lock();
                if (__xwcc_unlikely(rc < 0)) {
                        xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                        /* The skd is in hibernation state. Current thread need to
                           be frozen. So the blocking state needs to be interrupted. */
                        rc = -EINTR;
                } else
#endif
                {
                        XWOS_BUG_ON((XWUP_SKDOBJ_DST_SLEEPING | XWUP_SKDOBJ_DST_READY |
                                     XWUP_SKDOBJ_DST_STANDBY | XWUP_SKDOBJ_DST_FROZEN)
                                    & cthd->state);
                        /* set sleeping state */
                        xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_RUNNING);
                        xwbop_s1m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_SLEEPING);
                        /* add to time tree */
                        xwup_thd_tt_add_locked(cthd, xwtt, expected, cpuirq);
                        /* enable local CPU IRQ to enable schedule */
                        xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                        xwup_skd_wakelock_unlock();
#endif
                        xwup_skd_req_swcx();
                        xwospl_cpuirq_restore_lc(cpuirq);
                        if (XWUP_TTN_WKUPRS_TIMEDOUT == cthd->ttn.wkuprs) {
                                rc = XWOK;
                        } else if (XWUP_TTN_WKUPRS_INTR == cthd->ttn.wkuprs) {
                                rc = -EINTR;
                        } else {
                                XWOS_BUG();
                                rc = -EBUG;
                        }
                }
                currtick = xwup_syshwt_get_timetick(hwt);
                *xwtm = xwtm_sub(expected, currtick);
        }
        return rc;
}

__xwup_api
xwer_t xwup_cthd_sleep_from(xwtm_t * origin, xwtm_t inc)
{
        struct xwup_skd * xwskd;
        struct xwup_tt * xwtt;
        struct xwup_syshwt * hwt;
        struct xwup_thd * cthd;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((NULL != origin), "nullptr", -EFAULT);

        cthd = xwup_skd_get_cthd_lc();
        xwskd = xwup_skd_get_lc();
        xwtt = &xwskd->tt;
        hwt = &xwtt->hwt;
        expected = xwtm_add_safely(*origin, inc);
        xwup_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        rc = xwup_skd_wakelock_lock();
        if (__xwcc_unlikely(rc < 0)) {
                xwup_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                /* 当前CPU调度器处于休眠态，线程需要被冻结，
                   阻塞/睡眠函数将返回-EINTR。*/
                rc = -EINTR;
        } else
#endif
        {
                XWOS_BUG_ON((XWUP_SKDOBJ_DST_SLEEPING | XWUP_SKDOBJ_DST_READY |
                             XWUP_SKDOBJ_DST_STANDBY | XWUP_SKDOBJ_DST_FROZEN)
                            & cthd->state);
                /* set the sleeping state */
                xwbop_c0m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_RUNNING);
                xwbop_s1m(xwsq_t, &cthd->state, XWUP_SKDOBJ_DST_SLEEPING);
                /* add to time tree */
                xwup_thd_tt_add_locked(cthd, xwtt, expected, cpuirq);
                /* enable local CPU IRQ to enable schedule */
                xwup_sqlk_wr_unlock_cpuirq(&xwtt->lock);
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
                xwup_skd_wakelock_unlock();
#endif
                xwup_skd_req_swcx();
                xwospl_cpuirq_restore_lc(cpuirq);
                if (XWUP_TTN_WKUPRS_TIMEDOUT == cthd->ttn.wkuprs) {
                        rc = XWOK;
                } else if (XWUP_TTN_WKUPRS_INTR == cthd->ttn.wkuprs) {
                        rc = -EINTR;
                } else {
                        XWOS_BUG();
                        rc = -EBUG;
                }
                *origin = xwup_syshwt_get_timetick(hwt);
        }
        return rc;
}

__xwup_api
xwer_t xwup_cthd_freeze(void)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        struct xwup_thd * cthd;
        xwer_t rc;

        xwup_skd_dspmpt_lc();
        cthd = xwup_skd_get_cthd_lc();
        rc = xwospl_thd_freeze_lc(cthd);
        return rc;
#else
        return -ENOSYS;
#endif
}

/**
 * @brief 冻结线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 * @note
 * - 此函数只能在中断上下文中调用并且这个中断只能由@ref xwup_cthd_freeze()触发。
 */
__xwup_code
xwer_t xwup_thd_freeze_lic(struct xwup_thd * thd)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;

        XWOS_BUG_ON(XWUP_SKDOBJ_DST_RUNNING != (thd->state & (XWUP_SKDOBJ_DST_MASK)));
        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_RUNNING);
        xwbop_s1m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_FROZEN);
        xwskd->pm.frz_thd_cnt++;
        xwlib_bclst_add_tail(&xwskd->pm.frzlist, &thd->frznode);
        xwup_skd_enpmpt_lc();
        if (xwskd->pm.frz_thd_cnt == xwskd->thd_num) {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_notify_allfrz_lic();
        } else {
                xwospl_cpuirq_restore_lc(cpuirq);
                xwup_skd_req_swcx();
        }
        return XWOK;
#else
        XWOS_UNUSED(thd);
        return -ENOSYS;
#endif
}

/**
 * @brief 解冻线程
 * @param[in] thd: 线程对象的指针
 * @return 错误码
 */
__xwup_code
xwer_t xwup_thd_thaw_lic(struct xwup_thd * thd)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        struct xwup_skd * xwskd;
        xwreg_t cpuirq;
        xwer_t rc;

        xwskd = xwup_skd_get_lc();
        xwospl_cpuirq_save_lc(&cpuirq);
        XWOS_BUG_ON(!(XWUP_SKDOBJ_DST_FROZEN & thd->state));
        xwlib_bclst_del_init(&thd->frznode);
        xwskd->pm.frz_thd_cnt--;
        xwbop_c0m(xwsq_t, &thd->state, XWUP_SKDOBJ_DST_FROZEN);
        rc = xwup_thd_rq_add_tail(thd);
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
#else
        XWOS_UNUSED(thd);
        return -ENOSYS;
#endif
}

__xwup_api
bool xwup_cthd_shld_frz(void)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        return !!(xwup_skd_get_pm_stage() < XWUP_PM_STAGE_RUNNING);
#else
        return false;
#endif
}

__xwup_api
bool xwup_cthd_shld_stop(void)
{
#if defined(XWUPCFG_SKD_THD_EXIT) && (1 == XWUPCFG_SKD_THD_EXIT)
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return !!(XWUP_SKDOBJ_DST_EXITING & cthd->state);
#else
        return false;
#endif
}

__xwup_api
bool xwup_cthd_frz_shld_stop(bool * frozen)
{
#if (1 == XWUPRULE_SKD_THD_FREEZE)
        bool frz;
        xwer_t rc;

        frz = false;
        if (xwup_cthd_shld_frz()) {
                rc = xwup_cthd_freeze();
                if (XWOK == rc) {
                        frz = true;
                }
                /* else {} */
        }/* else {} */
        if (!(is_err_or_null(frozen))) {
                *frozen = frz;
        }/* else {} */
#else
        if (!(is_err_or_null(frozen))) {
                *frozen = false;
        }/* else {} */
#endif
        return xwup_cthd_shld_stop();
}

#if defined(XWUPCFG_SKD_THD_LOCAL_DATA_NUM) && (XWUPCFG_SKD_THD_LOCAL_DATA_NUM > 0U)
__xwup_api
xwer_t xwup_thd_set_data(struct xwup_thd * thd, xwsq_t pos, void * data)
{
        xwer_t rc;

        XWOS_VALIDATE((thd), "nullptr", -EFAULT);

        if (pos < XWUPCFG_SKD_THD_LOCAL_DATA_NUM) {
                thd->data[pos] = data;
                rc = XWOK;
        } else {
                rc = -ECHRNG;
        }
        return rc;
}

__xwup_api
xwer_t xwup_thd_get_data(struct xwup_thd * thd, xwsq_t pos, void ** databuf)
{
        xwer_t rc;

        XWOS_VALIDATE((thd), "nullptr", -EFAULT);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        if (pos < XWUPCFG_SKD_THD_LOCAL_DATA_NUM) {
                *databuf = thd->data[pos];
                rc = XWOK;
        } else {
                *databuf = NULL;
                rc = -ECHRNG;
        }
        return rc;
}

__xwup_api
xwer_t xwup_cthd_set_data(xwsq_t pos, void * data)
{
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return xwup_thd_set_data(cthd, pos, data);
}

__xwup_api
xwer_t xwup_cthd_get_data(xwsq_t pos, void ** databuf)
{
        struct xwup_thd * cthd;

        cthd = xwup_skd_get_cthd_lc();
        return xwup_thd_set_data(cthd, pos, databuf);

}
#endif
