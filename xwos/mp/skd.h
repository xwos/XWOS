/**
 * @file
 * @brief 玄武OS MP内核：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_mp_skd_h__
#define __xwos_mp_skd_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/rtrq.h>
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
#  include <xwos/mp/bh.h>
#endif
#include <xwos/mp/tt.h>

#define XWMP_CPU_NUM                            ((xwid_t)CPUCFG_CPU_NUM)
#define XWMP_SKD_PRIORITY_RT_NUM                ((xwpr_t)XWMPCFG_SKD_PRIORITY_RT_NUM)
#define XWMP_SKD_PRIORITY_RT_MIN                ((xwpr_t)0)
#define XWMP_SKD_PRIORITY_RT_MAX                (XWMP_SKD_PRIORITY_RT_NUM - 1)
#define XWMP_SKD_PRIORITY_INVALID               ((xwpr_t)-1)
#define XWMP_SKD_PRIORITY_RAISE(base, inc)      ((base) + (inc))
#define XWMP_SKD_PRIORITY_DROP(base, dec)       ((base) - (dec))

#define XWMP_SKD_IDLE_STK(xwskd)                (&((xwskd)->idle))
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
#  define XWMP_SKD_BH_STK(xwskd)                (&((xwskd)->bh))
#endif

struct xwmp_pmdm;
struct xwmp_skd;
struct xwmp_thd;

/**
 * @brief 调度策略枚举
 */
enum xwmp_skd_policy_em {
        XWMP_SKD_POLICY_RT = XWBOP_BIT(0), /**< realtime */
};

/**
 * @brief 调度对象状态
 */
enum xwmp_skdobj_state_em {
        XWMP_SKDOBJ_DST_UNKNOWN = 0U, /**< 未知状态 */
        XWMP_SKDOBJ_DST_STANDBY = (1U << 0U), /**< 待机 */
        XWMP_SKDOBJ_DST_RUNNING = (1U << 1U), /**< 运行 */
        XWMP_SKDOBJ_DST_READY = (1U << 2U), /**< 就绪 */
        XWMP_SKDOBJ_DST_SLEEPING = (1U << 3U), /**< 睡眠 */
        XWMP_SKDOBJ_DST_BLOCKING = (1U << 4U), /**< 阻塞 */
        XWMP_SKDOBJ_DST_FREEZABLE = (1U << 5U), /**< 可被冻结 */
        XWMP_SKDOBJ_DST_FROZEN = (1U << 6U), /**< 已经被冻结 */
        XWMP_SKDOBJ_DST_EXITING = (1U << 7U), /**< 正在退出 */
        XWMP_SKDOBJ_DST_MIGRATING = (1U << 8U), /**< 正在迁移 */
        XWMP_SKDOBJ_DST_DETACHED = (1U << 9U), /**< 分离态 */
        XWMP_SKDOBJ_DST_UNINTERRUPTED = (1U << 15U), /**< 阻塞态不可中断 */
        XWMP_SKDOBJ_DST_MASK = (XWMP_SKDOBJ_DST_STANDBY |
                                XWMP_SKDOBJ_DST_RUNNING |
                                XWMP_SKDOBJ_DST_READY |
                                XWMP_SKDOBJ_DST_SLEEPING |
                                XWMP_SKDOBJ_DST_BLOCKING |
                                XWMP_SKDOBJ_DST_FREEZABLE |
                                XWMP_SKDOBJ_DST_FROZEN |
                                XWMP_SKDOBJ_DST_EXITING |
                                XWMP_SKDOBJ_DST_MIGRATING |
                                XWMP_SKDOBJ_DST_UNINTERRUPTED),
};

/**
 * @brief 调度对象属性
 */
enum xwmp_skdattr_em {
        XWMP_SKDATTR_PRIVILEGED = (1U << 0U), /**< 拥有超级权限 */

        XWMP_SKDATTR_DETACHED = (1U << 1U), /**< DETACHED态，类似于pthread的DETACHED */
        XWMP_SKDATTR_JOINABLE = 0, /**< JOINABLE态，类似于pthread的JOINABLE */
};

/**
 * @brief 线程主函数
 */
typedef xwer_t (* xwmp_thd_f)(void *);

/**
 * @brief 线程栈信息
 */
struct xwmp_skd_stack_info {
        xwstk_t * sp; /**< 栈指针 */
        xwstk_t * base; /**< 栈基地址 */
        xwsz_t size; /**< 栈大小，单位：字节 */
        xwmp_thd_f main; /**< 主函数 */
        void * arg; /**< 主函数的参数 */
        const char * name; /**< 名字字符串 */
};

/**
 * @brief 调度器上下文枚举
 */
enum xwmp_skd_context_em {
        XWMP_SKD_CONTEXT_INIT_EXIT = 0, /**< 初始化与反初始化 */
        XWMP_SKD_CONTEXT_THD, /**< 线程 */
        XWMP_SKD_CONTEXT_ISR, /**< 中断 */
        XWMP_SKD_CONTEXT_BH, /**< 中断底半部 */
        XWMP_SKD_CONTEXT_IDLE, /**< 空闲任务 */
};

/**
 * @brief 调度器唤醒锁状态枚举
 */
enum xwmp_skd_wakelock_cnt_em {
        XWMP_SKD_WKLKCNT_SUSPENDED = 0, /**< 调度器已暂停 */
        XWMP_SKD_WKLKCNT_ALLFRZ, /**< 调度器所有线程已冻结 */
        XWMP_SKD_WKLKCNT_FREEZING, /**< 调度器正在暂停 */
        XWMP_SKD_WKLKCNT_THAWING = XWMP_SKD_WKLKCNT_FREEZING, /**< 调度器正在恢复 */
        XWMP_SKD_WKLKCNT_RUNNING, /**< 正常运行 */
        XWMP_SKD_WKLKCNT_UNLOCKED = XWMP_SKD_WKLKCNT_RUNNING, /**< 唤醒锁：未加锁 */
        XWMP_SKD_WKLKCNT_LOCKED, /**< 唤醒锁：已加锁 */
};

/**
 * @brief 调度器电源管理控制块
 */
struct xwmp_skd_pm {
        xwsq_a wklkcnt; /**< 唤醒锁，取值@ref xwmp_skd_wakelock_cnt_em */
        xwsz_t frz_thd_cnt; /**< 已冻结的线程计数器 */
        struct xwlib_bclst_head frzlist; /**< 已冻结的线程链表 */
        struct xwmp_splk lock; /**< 保护链表和计数器的锁 */
        struct xwmp_pmdm * xwpmdm; /**< 归属的电源管理领域 */
};

/**
 * @brief XWOS MP 调度器
 */
struct __xwcc_alignl1cache xwmp_skd {
        struct xwmp_skd_stack_info * cstk; /**< 当前正在运行的线程的栈信息的指针
                                                偏移：0，
                                                汇编代码中会使用这个成员 */
        struct xwmp_skd_stack_info * pstk; /**< 前一个线程的栈信息的指针
                                                偏移：sizeof(long)，
                                                汇编代码中会使用这个成员 */
        xwid_t id; /**< CPU ID */
        bool state; /**< 调度器状态 */
        struct {
                struct xwmp_rtrq rt; /**< 实时就绪队列 */
        } rq; /**< 就绪队列 */
        struct xwmp_skd_stack_info idle; /**< 空闲任务的栈信息 */
        xwsq_t req_schedule_cnt; /**< 请求调度的计数器 */
        xwsq_a req_chkpmpt_cnt; /**< 请求检查抢占的计数器 */
        xwsq_a dis_pmpt_cnt; /**< 关闭抢占的计数器 */
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
        xwsq_a req_bh_cnt; /**< 请求进入中断底半部的计数器 */
        xwsq_a dis_bh_cnt; /**< 关闭中断底半部的计数器 */
        struct xwmp_bh_cb bhcb; /**< 中断底半部控制块 */
        struct xwmp_skd_stack_info bh; /**< 中断底半部任务的栈信息 */
#endif
        struct xwmp_tt tt; /**< 时间树 */
        struct xwmp_splk cxlock; /**< 上下文切换的锁 */
        struct xwmp_skd_pm pm; /**< 调度器电源管理控制块 */

        struct xwlib_bclst_head thdlist; /**< 本调度器中所有线程的链表头 */
        xwsz_t thd_num; /**< 本调度器中的线程数量 */
        struct xwmp_splk thdlistlock; /**< 保护thdlist的锁 */
        struct xwlib_bclst_head thdelist; /**< 本调度器中所有待删除的线程的链表头 */
};

extern struct xwmp_skd xwmp_skd[CPUCFG_CPU_NUM];

struct xwmp_skd * xwmp_skd_get_lc(void);
xwer_t xwmp_skd_get_by_cpuid(xwid_t cpuid, struct xwmp_skd ** ptrbuf);
struct xwmp_thd * xwmp_skd_get_cthd(struct xwmp_skd * xwskd);
struct xwmp_thd * xwmp_skd_get_cthd_lc(void);
struct xwmp_skd * xwmp_skd_dspmpt(struct xwmp_skd * xwskd);
struct xwmp_skd * xwmp_skd_enpmpt(struct xwmp_skd * xwskd);

#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
struct xwmp_skd * xwmp_skd_dsbh(struct xwmp_skd * xwskd);
struct xwmp_skd * xwmp_skd_enbh(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_req_bh(struct xwmp_skd * xwskd);
bool xwmp_skd_tst_in_bh(struct xwmp_skd * xwskd);
#endif

void xwmp_skd_chkpmpt(struct xwmp_skd * xwskd);
void xwmp_skd_chkpmpt_all(void);
xwer_t xwmp_skd_req_swcx(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_inc_wklkcnt(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_dec_wklkcnt(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_wakelock_lock(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_wakelock_unlock(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_notify_allfrz_lic(struct xwmp_skd * xwskd);

struct xwmp_skd * xwmp_skd_pre_swcx_lic(struct xwmp_skd * xwskd);
struct xwmp_skd * xwmp_skd_post_swcx_lic(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_suspend_lic(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_resume_lic(struct xwmp_skd * xwskd);

xwer_t xwmp_skd_init_lc(void);
xwer_t xwmp_skd_start_lc(void);
xwer_t xwmp_skd_start_syshwt_lc(void);
xwer_t xwmp_skd_stop_syshwt_lc(void);
xwid_t xwmp_skd_id_lc(void);

#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
struct xwmp_skd * xwmp_skd_dsbh_lc(void);
struct xwmp_skd * xwmp_skd_enbh_lc(void);
bool xwmp_skd_tst_in_bh_lc(void);
#endif

struct xwmp_skd * xwmp_skd_dspmpt_lc(void);
struct xwmp_skd * xwmp_skd_enpmpt_lc(void);
xwer_t xwmp_skd_suspend(xwid_t cpuid);
xwer_t xwmp_skd_resume(xwid_t cpuid);
xwsq_t xwmp_skd_get_pm_state(struct xwmp_skd * xwskd);
void xwmp_skd_get_context_lc(xwsq_t * ctx, xwirq_t * irqn);
bool xwmp_skd_prio_tst_valid(xwpr_t prio);

#endif /* xwos/mp/skd.h */
