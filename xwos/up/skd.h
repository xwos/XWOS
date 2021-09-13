/**
 * @file
 * @brief 玄武OS UP内核：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_up_skd_h__
#define __xwos_up_skd_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/up/rtrq.h>
#include <xwos/up/pm.h>
#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
#  include <xwos/up/bh.h>
#endif
#include <xwos/up/tt.h>

#define XWUP_SKD_PRIORITY_RT_NUM                ((xwpr_t)XWUPCFG_SKD_PRIORITY_RT_NUM)
#define XWUP_SKD_PRIORITY_RT_MIN                ((xwpr_t)0)
#define XWUP_SKD_PRIORITY_RT_MAX                (XWUP_SKD_PRIORITY_RT_NUM - 1)
#define XWUP_SKD_PRIORITY_INVALID               ((xwpr_t) - 1)
#define XWUP_SKD_PRIORITY_RAISE(base, inc)      ((base) + (inc))
#define XWUP_SKD_PRIORITY_DROP(base, dec)       ((base) - (dec))

#define XWUP_SKD_IDLE_STK(xwskd)                (&((xwskd)->idle))
#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
#  define XWUP_SKD_BH_STK(xwskd)                (&((xwskd)->bh))
#endif

struct xwup_skd;
struct xwup_thd;

/**
 * @brief 调度策略枚举
 */
enum xwup_skd_policy_em {
        XWUP_SKD_POLICY_RT = XWBOP_BIT(0), /**< realtime */
};

/**
 * @brief 调度对象状态
 */
enum xwup_skdobj_state_em {
        XWUP_SKDOBJ_DST_UNKNOWN = 0U, /**< 未知状态 */
        XWUP_SKDOBJ_DST_STANDBY = (1U << 0U), /**< 待机 */
        XWUP_SKDOBJ_DST_RUNNING = (1U << 1U), /**< 运行 */
        XWUP_SKDOBJ_DST_READY = (1U << 2U), /**< 就绪 */
        XWUP_SKDOBJ_DST_SLEEPING = (1U << 3U), /**< 睡眠 */
        XWUP_SKDOBJ_DST_BLOCKING = (1U << 4U), /**< 阻塞 */
        XWUP_SKDOBJ_DST_RESERVED5 = (1U << 5U), /**< 保留位5 */
        XWUP_SKDOBJ_DST_FROZEN = (1U << 6U), /**< 已经被冻结 */
        XWUP_SKDOBJ_DST_EXITING = (1U << 7U), /**< 正在退出 */
        XWUP_SKDOBJ_DST_RESERVED8 = (1U << 8U), /**< 保留位8 */
        XWUP_SKDOBJ_DST_DETACHED = (1U << 9U), /**< 分离态 */
        XWUP_SKDOBJ_DST_UNINTERRUPTED = (1U << 15U), /**< 阻塞态不可中断 */
        XWUP_SKDOBJ_DST_MASK = (XWUP_SKDOBJ_DST_STANDBY |
                                XWUP_SKDOBJ_DST_RUNNING |
                                XWUP_SKDOBJ_DST_READY |
                                XWUP_SKDOBJ_DST_SLEEPING |
                                XWUP_SKDOBJ_DST_BLOCKING |
                                XWUP_SKDOBJ_DST_FROZEN |
                                XWUP_SKDOBJ_DST_EXITING |
                                XWUP_SKDOBJ_DST_UNINTERRUPTED),
};

/**
 * @brief 调度对象属性
 */
enum xwup_skdobj_attr_em {
        XWUP_SKDATTR_PRIVILEGED = (1U << 0U), /**< 拥有超级权限 */

        XWUP_SKDATTR_DETACHED = (1U << 1U), /**< DETACHED态，类似于pthread的DETACHED */
        XWUP_SKDATTR_JOINABLE = 0, /**< JOINABLE态，类似于pthread的JOINABLE */
};

/**
 * @brief 线程主函数
 */
typedef xwer_t (* xwup_thd_f)(void *);

/**
 * @brief 线程栈信息
 */
struct xwup_skd_stack_info {
        xwstk_t * sp; /**< 栈指针 */
        xwstk_t * base; /**< 栈基地址 */
        xwsz_t size; /**< 栈大小，单位：字节 */
        xwup_thd_f main; /**< 主函数 */
        void * arg; /**< 主函数的参数 */
        const char * name; /**< 名字字符串 */
};

/**
 * @brief 调度器上下文枚举
 */
enum xwup_skd_context_em {
        XWUP_SKD_CONTEXT_INIT_EXIT = 0, /**< 初始化与反初始化 */
        XWUP_SKD_CONTEXT_THD, /**< 线程 */
        XWUP_SKD_CONTEXT_ISR, /**< 中断 */
        XWUP_SKD_CONTEXT_BH, /**< 中断底半部 */
        XWUP_SKD_CONTEXT_IDLE, /**< 空闲任务 */
};

#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
/**
 * @brief 电源管理回调函数集合
 */
struct xwup_skd_pm_callback {
        xwup_skd_pm_cb_f resume; /**< 电源管理从暂停模式恢复的回调函数 */
        xwup_skd_pm_cb_f suspend; /**< 电源管理进入暂停模式的回调函数 */
        xwup_skd_pm_cb_f wakeup; /**< 唤醒电源管理的回调函数 */
        xwup_skd_pm_cb_f sleep; /**< 电源管理休眠的回调函数 */
        void * arg; /**< 各回调函数的参数 */
};

/**
 * @brief 调度器电源管理控制块
 */
struct xwup_skd_pm {
        __xw_io xwsq_t wklkcnt; /**< 唤醒锁，
                                     取值@ref xwup_skd_wakelock_cnt_em */
        xwsz_t frz_thd_cnt; /**< 已冻结的线程计数器 */
        struct xwlib_bclst_head frzlist; /**< 已冻结的线程链表 */
        struct xwup_skd_pm_callback cb; /**< 电源管理回调函数集合 */
};
#endif

/**
 * @brief XWOS UP 调度器
 */
struct xwup_skd {
        struct xwup_skd_stack_info * cstk; /**< 当前正在运行的线程的栈信息的指针
                                                偏移：0，
                                                汇编代码中会使用这个成员。*/
        struct xwup_skd_stack_info * pstk; /**< 前一个线程的栈信息的指针
                                                偏移：sizeof(long)，
                                                汇编代码中会使用这个成员。*/
        bool state; /**< 调度器状态 */
        struct {
                struct xwup_rtrq rt; /**< 实时就绪队列 */
        } rq; /**< 就绪队列 */
        struct xwup_skd_stack_info idle; /**< 空闲任务的栈信息 */
        xwsq_t req_schedule_cnt; /**< 请求调度的计数器 */
        xwsq_t req_chkpmpt_cnt; /**< 请求检查抢占的计数器 */
        xwsq_t dis_pmpt_cnt; /**< 关闭抢占的计数器 */
#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
        xwsq_t req_bh_cnt; /**< 请求进入中断底半部的计数器 */
        xwsq_t dis_bh_cnt; /**< 关闭中断底半部的计数器 */
        struct xwup_bh_cb bhcb; /**< 中断底半部控制块 */
        struct xwup_skd_stack_info bh; /**< 中断底半部任务的栈信息 */
#endif
        struct xwup_tt tt; /**< 时间树 */
#if defined(XWUPCFG_SKD_PM) && (1 == XWUPCFG_SKD_PM)
        struct xwup_skd_pm pm; /**< 调度器低功耗控制块 */
#endif
        struct xwlib_bclst_head thdlist; /**< 本调度器中所有线程的链表头 */
        xwsz_t thd_num; /**< 本调度器中的线程数量 */
        struct xwlib_bclst_head thdelist; /**< 本调度器中所有待删除的线程的链表头 */
};


struct xwup_skd * xwup_skd_get_lc(void);
struct xwup_thd * xwup_skd_get_cthd_lc(void);

#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
xwer_t xwup_skd_req_bh(void);
#endif

void xwup_skd_chkpmpt(void);
xwer_t xwup_skd_req_swcx(void);
xwer_t xwup_skd_inc_wklkcnt(void);
xwer_t xwup_skd_dec_wklkcnt(void);
xwer_t xwup_skd_wakelock_lock(void);
xwer_t xwup_skd_wakelock_unlock(void);
void xwup_skd_intr_all(void);
xwer_t xwup_skd_notify_allfrz_lic(void);

struct xwup_skd * xwup_skd_pre_swcx_lic(struct xwup_skd * xwskd);
struct xwup_skd * xwup_skd_post_swcx_lic(struct xwup_skd * xwskd);
xwer_t xwup_skd_suspend_lic(struct xwup_skd * xwskd);
xwer_t xwup_skd_resume_lic(struct xwup_skd * xwskd);

xwer_t xwup_skd_init_lc(void);
xwer_t xwup_skd_start_lc(void);
xwer_t xwup_skd_start_syshwt_lc(void);
xwer_t xwup_skd_stop_syshwt_lc(void);

#if defined(XWUPCFG_SKD_BH) && (1 == XWUPCFG_SKD_BH)
struct xwup_skd * xwup_skd_dsbh_lc(void);
struct xwup_skd * xwup_skd_enbh_lc(void);
bool xwup_skd_tst_in_bh_lc(void);
#endif

struct xwup_skd * xwup_skd_dspmpt_lc(void);
struct xwup_skd * xwup_skd_enpmpt_lc(void);
void xwup_skd_get_context_lc(xwsq_t * ctx, xwirq_t * irqn);
bool xwup_skd_prio_tst_valid(xwpr_t prio);

#endif /* xwos/up/skd.h */
