/**
 * @file
 * @brief XWOS MP内核：调度器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_skd_h__
#define __xwos_mp_skd_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/rtrq.h>
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
#  include <xwos/mp/bh.h>
#endif
#include <xwos/mp/tt.h>

#define XWMP_CPU_NUM                            ((xwid_t)CPUCFG_CPU_NUM)
#define XWMP_SKD_PRIORITY_RT_NUM                ((xwpr_t)XWOSCFG_SKD_PRIORITY_RT_NUM)
#define XWMP_SKD_PRIORITY_RT_MIN                ((xwpr_t)0)
#define XWMP_SKD_PRIORITY_RT_MAX                (XWMP_SKD_PRIORITY_RT_NUM - (xwpr_t)1)
#define XWMP_SKD_PRIORITY_INVALID               ((xwpr_t)-1)
#define XWMP_SKD_PRIORITY_RAISE(base, inc)      ((base) + (inc))
#define XWMP_SKD_PRIORITY_DROP(base, dec)       ((base) - (dec))

#define XWMP_SKD_IDLE_STK(xwskd)                (&((xwskd)->idle))
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
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
        XWMP_SKDOBJ_ST_UNKNOWN = 0U, /**< 未知状态 */
        XWMP_SKDOBJ_ST_STANDBY = 1U, /**< 待机，BIT(0) */
        XWMP_SKDOBJ_ST_RUNNING = 2U, /**< 运行，BIT(1) */
        XWMP_SKDOBJ_ST_READY = 4U, /**< 就绪，BIT(2) */
        XWMP_SKDOBJ_ST_SLEEPING = 8U, /**< 睡眠，BIT(3) */
        XWMP_SKDOBJ_ST_BLOCKING = 16U, /**< 阻塞，BIT(4) */
        XWMP_SKDOBJ_ST_FREEZABLE = 32U, /**< 可被冻结，BIT(5) */
        XWMP_SKDOBJ_ST_FROZEN = 64U, /**< 已经被冻结，BIT(6) */
        XWMP_SKDOBJ_ST_EXITING = 128U, /**< 正在退出，BIT(7) */
        XWMP_SKDOBJ_ST_MIGRATING = 256U, /**< 正在迁移，BIT(8) */
        XWMP_SKDOBJ_ST_DETACHED = 512U, /**< 已分离，BIT(9) */
        XWMP_SKDOBJ_ST_JOINED = 1024U, /**< 已连接，BIT(10) */
        XWMP_SKDOBJ_ST_UNINTERRUPTED = 32768U, /**< 阻塞态不可中断，BIT(15) */
};

/**
 * @brief 调度对象标签
 */
enum xwmp_skdobj_flag_em {
        XWMP_SKDOBJ_FLAG_PRIVILEGED = 1U, /**< 拥有超级权限，BIT(0) */
        XWMP_SKDOBJ_FLAG_ALLOCATED_STACK = 2U, /**< 动态申请的栈，BIT(1) */
};

/**
 * @brief 线程主函数
 */
typedef xwer_t (* xwmp_thd_f)(void *);

/**
 * @brief 线程栈信息
 */
struct xwmp_skdobj_stack {
        xwstk_t * sp; /**< 栈指针 */
        void * tls; /**< TLS变量区的基地址 */
        xwstk_t * base; /**< 栈内存的基地址 */
        xwsz_t size; /**< 栈内存的大小，单位：字节 */
        xwstk_t * guard_base; /**< 栈内存警戒线的基地址 */
        xwsz_t guard; /**< 栈内存警戒线，单位：字节 */
        xwmp_thd_f main; /**< 主函数 */
        void * arg; /**< 主函数的参数 */
        const char * name; /**< 名字字符串 */
        atomic_xwsq_t flag; /**< 标签，取值 @ref xwmp_skdobj_flag_em */
};

/**
 * @brief 调度器上下文枚举
 */
enum xwmp_skd_context_em {
        XWMP_SKD_CONTEXT_BOOT = 0U, /**< 启动 */
        XWMP_SKD_CONTEXT_THD = 1U, /**< 线程 */
        XWMP_SKD_CONTEXT_ISR = 2U, /**< 中断 */
        XWMP_SKD_CONTEXT_BH = 3U, /**< 中断底半部 */
        XWMP_SKD_CONTEXT_IDLE = 4U, /**< 空闲任务 */
};

/**
 * @brief 调度器唤醒锁状态枚举
 */
enum xwmp_skd_wakelock_cnt_em {
        XWMP_SKD_WKLKCNT_SUSPENDED = 0U, /**< 调度器已暂停 */
        XWMP_SKD_WKLKCNT_ALLFRZ = 1U, /**< 调度器所有线程已冻结 */
        XWMP_SKD_WKLKCNT_FREEZING = 2U, /**< 调度器正在暂停 */
        XWMP_SKD_WKLKCNT_THAWING = XWMP_SKD_WKLKCNT_FREEZING, /**< 调度器正在恢复 */
        XWMP_SKD_WKLKCNT_RUNNING = 3U, /**< 正常运行 */
        XWMP_SKD_WKLKCNT_UNLOCKED = XWMP_SKD_WKLKCNT_RUNNING, /**< 唤醒锁：未加锁 */
        XWMP_SKD_WKLKCNT_LOCKED = 4U, /**< 唤醒锁：已加锁 */
};

/**
 * @brief 调度器电源管理控制块
 */
struct xwmp_skd_pm {
        atomic_xwsq_t wklkcnt; /**< 唤醒锁，取值 @ref xwmp_skd_wakelock_cnt_em */
        xwsz_t frz_thd_cnt; /**< 已冻结的线程计数器 */
        struct xwlib_bclst_head frzlist; /**< 已冻结的线程链表 */
        struct xwmp_splk lock; /**< 保护链表和计数器的锁 */
        struct xwmp_pmdm * xwpmdm; /**< 归属的电源管理领域 */
};

/**
 * @brief 调度器状态
 */
enum xwmp_skd_state_em {
        XWMP_SKD_STATE_UNINIT, /**< 未初始化 */
        XWMP_SKD_STATE_INIT, /**< 已初始化 */
        XWMP_SKD_STATE_START, /**< 开始调度 */
};

/**
 * @brief XWOS MP 调度器
 */
struct __xwcc_alignl1cache xwmp_skd {
        struct xwmp_skdobj_stack * cstk; /**< 当前正在运行的线程的栈信息的指针
                                              偏移：0，
                                              汇编代码中会使用这个成员 */
        struct xwmp_skdobj_stack * pstk; /**< 前一个线程的栈信息的指针
                                              偏移：sizeof(void *)，
                                              汇编代码中会使用这个成员 */
        xwid_t id; /**< CPU ID */
        xwsq_t state; /**< 调度器状态，取值： @ref xwmp_skd_state_em */
        struct {
                struct xwmp_rtrq rt; /**< 实时就绪队列 */
        } rq; /**< 就绪队列 */
        struct xwmp_skdobj_stack idle; /**< 空闲任务的栈信息 */
        xwsq_t req_schedule_cnt; /**< 请求调度的计数器 */
        atomic_xwsq_t req_chkpmpt_cnt; /**< 请求检查抢占的计数器 */
        atomic_xwsq_t dis_pmpt_cnt; /**< 关闭抢占的计数器 */
#if defined(XWOSCFG_SKD_BH) && (1 == XWOSCFG_SKD_BH)
        atomic_xwsq_t req_bh_cnt; /**< 请求进入中断底半部的计数器 */
        atomic_xwsq_t dis_bh_cnt; /**< 关闭中断底半部的计数器 */
        struct xwmp_bh_cb bhcb; /**< 中断底半部控制块 */
        struct xwmp_skdobj_stack bh; /**< 中断底半部任务的栈信息 */
#endif
        xwsq_t dis_irq_cnt; /**< 关闭中断的计数器 */
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
struct xwmp_skd * xwmp_skd_svpmpt(struct xwmp_skd * xwskd, xwsq_t * dis_pmpt_cnt);
struct xwmp_skd * xwmp_skd_rspmpt(struct xwmp_skd * xwskd, xwsq_t dis_pmpt_cnt);
bool xwmp_skd_tstpmpt(struct xwmp_skd * xwskd);

struct xwmp_skd * xwmp_skd_dsbh(struct xwmp_skd * xwskd);
struct xwmp_skd * xwmp_skd_enbh(struct xwmp_skd * xwskd);
struct xwmp_skd * xwmp_skd_svbh(struct xwmp_skd * xwskd, xwsq_t * dis_bh_cnt);
struct xwmp_skd * xwmp_skd_rsbh(struct xwmp_skd * xwskd, xwsq_t dis_bh_cnt);
bool xwmp_skd_tstbh(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_req_bh(struct xwmp_skd * xwskd);
bool xwmp_skd_tst_in_bh(struct xwmp_skd * xwskd);

void xwmp_skd_chkpmpt(struct xwmp_skd * xwskd);
void xwmp_skd_chkpmpt_all(void);
xwer_t xwmp_skd_req_swcx(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_inc_wklkcnt(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_dec_wklkcnt(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_wakelock_lock(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_wakelock_unlock(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_notify_allfrz_lic(struct xwmp_skd * xwskd);

struct xwmp_skd * xwmp_skd_post_start_lic(struct xwmp_skd * xwskd);
struct xwmp_skd * xwmp_skd_pre_swcx_lic(struct xwmp_skd * xwskd);
struct xwmp_skd * xwmp_skd_post_swcx_lic(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_suspend_lic(struct xwmp_skd * xwskd);
xwer_t xwmp_skd_resume_lic(struct xwmp_skd * xwskd);

xwer_t xwmp_skd_init_lc(void);
xwer_t xwmp_skd_start_lc(void);
xwer_t xwmp_skd_start_syshwt_lc(void);
xwer_t xwmp_skd_stop_syshwt_lc(void);
xwid_t xwmp_skd_get_cpuid_lc(void);

struct xwmp_skd * xwmp_skd_dsbh_lc(void);
struct xwmp_skd * xwmp_skd_enbh_lc(void);
bool xwmp_skd_tst_in_bh_lc(void);

struct xwmp_skd * xwmp_skd_dspmpt_lc(void);
struct xwmp_skd * xwmp_skd_enpmpt_lc(void);
xwer_t xwmp_skd_suspend(xwid_t cpuid);
xwer_t xwmp_skd_resume(xwid_t cpuid);
xwsq_t xwmp_skd_get_pm_state(struct xwmp_skd * xwskd);
void xwmp_skd_get_context_lc(xwsq_t * ctx, xwirq_t * irqn);
bool xwmp_skd_prio_tst_valid(xwpr_t prio);

#endif /* xwos/mp/skd.h */
