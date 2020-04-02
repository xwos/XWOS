/**
 * @file
 * @brief XuanWuOS内核：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_smp_scheduler_h__
#define __xwos_smp_scheduler_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/rtrq.h>
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
  #include <xwos/smp/bh.h>
#endif /* XWSMPCFG_SD_BH */
#include <xwos/smp/tt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_SD_PRIORITY_RT_NUM                 ((xwpr_t)XWSMPCFG_SD_PRIORITY_RT_NUM)
#define XWOS_SD_PRIORITY_RT_MIN                 ((xwpr_t)0)
#define XWOS_SD_PRIORITY_RT_MAX                 (XWOS_SD_PRIORITY_RT_NUM - 1)
#define XWOS_SD_PRIORITY_INVALID                ((xwpr_t)-1)
#define XWOS_SD_PRIORITY_RAISE(base, inc)       ((base) + (inc))
#define XWOS_SD_PRIORITY_DROP(base, dec)        ((base) - (dec))

#define XWOS_SCHEDULER_IDLE_STK(xwsd)           (&((xwsd)->idle))
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
  #define XWOS_SCHEDULER_BH_STK(xwsd)           (&((xwsd)->bh))
#endif /* XWSMPCFG_SD_BH */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_pmdm;
struct xwos_scheduler;
struct xwos_tcb;

/**
 * @brief 调度策略枚举
 */
enum xwos_scheduler_policy_em {
        XWOS_SCHEDULER_POLICY_RT = BIT(0), /**< realtime */
};

/**
 * @brief 调度对象状态
 */
enum xwos_sdobj_state_em {
        XWSDOBJ_DST_UNKNOWN = 0U, /**< 未知状态 */
        XWSDOBJ_DST_STANDBY = (1U << 0U), /**< 待机 */
        XWSDOBJ_DST_RUNNING = (1U << 1U), /**< 运行 */
        XWSDOBJ_DST_READY = (1U << 2U), /**< 就绪 */
        XWSDOBJ_DST_SLEEPING = (1U << 3U), /**< 睡眠 */
        XWSDOBJ_DST_BLOCKING = (1U << 4U), /**< 阻塞 */
        XWSDOBJ_DST_FREEZABLE = (1U << 5U), /**< 可被冻结 */
        XWSDOBJ_DST_FROZEN = (1U << 6U), /**< 已经被冻结 */
        XWSDOBJ_DST_EXITING = (1U << 7U), /**< 正在退出 */
        XWSDOBJ_DST_MIGRATING = (1U << 8U), /**< 正在迁移 */
        XWSDOBJ_DST_UNINTERRUPTED = (1U << 15U), /**< 阻塞态不可中断 */
        XWSDOBJ_DST_MASK = (XWSDOBJ_DST_STANDBY |
                            XWSDOBJ_DST_RUNNING |
                            XWSDOBJ_DST_READY |
                            XWSDOBJ_DST_SLEEPING |
                            XWSDOBJ_DST_BLOCKING |
                            XWSDOBJ_DST_FREEZABLE |
                            XWSDOBJ_DST_FROZEN |
                            XWSDOBJ_DST_EXITING |
                            XWSDOBJ_DST_MIGRATING |
                            XWSDOBJ_DST_UNINTERRUPTED),
};

/**
 * @brief 调度对象属性
 */
enum xwos_sdobj_attr_em {
        XWSDOBJ_ATTR_PRIVILEGED = (1U << 0U), /**< 拥有超级权限 */
};

/**
 * @brief 线程主函数
 */
typedef xwer_t (*xwos_thrd_f)(void *);

/**
 * @brief 线程栈信息
 */
struct xwos_sdobj_stack_info {
        xwstk_t * sp; /**< 栈指针 */
        xwstk_t * base; /**< 栈基地址 */
        xwsz_t size; /**< 栈大小，单位：字节 */
        xwos_thrd_f main; /**< 主函数 */
        void * arg; /**< 主函数的参数 */
        const char * name; /**< 名字字符串 */
};

/**
 * @brief 调度器唤醒锁状态枚举
 */
enum xwos_scheduler_wakelock_cnt_em {
        XWOS_SCHEDULER_WKLKCNT_LPM = 0, /**< 调度器正在暂停 */
        XWOS_SCHEDULER_WKLKCNT_UNLOCKED, /**< 唤醒锁：未加锁 */
        XWOS_SCHEDULER_WKLKCNT_ULOCKED, /**< 唤醒锁：已加锁 */
};

/**
 * @brief 调度器低功耗控制块
 */
struct xwos_scheduler_lpm {
        __atomic xwsq_t wklkcnt; /**< - == 0: 进入休眠
                                      - == 1: 可申请进入休眠
                                      - > 1: 不可休眠   */
        xwsz_t frz_thrd_cnt; /**< 已冻结的线程计数器 */
        struct xwlib_bclst_head frzlist; /**< 已冻结的线程链表 */
        struct xwlk_splk lock; /**< 保护链表和计数器的锁 */
        struct xwos_pmdm * xwpmdm; /**< 归属的电源管理领域 */
};

/**
 * @brief XWOS调度器
 */
struct __aligned_l1cacheline xwos_scheduler {
        struct xwos_sdobj_stack_info * cstk; /**< 当前正在运行的线程的栈信息的指针
                                                  偏移：0，
                                                  汇编代码中会使用这个成员 */
        struct xwos_sdobj_stack_info * pstk; /**< 前一个线程的栈信息的指针
                                                  偏移：sizeof(long)，
                                                  汇编代码中会使用这个成员 */
        xwid_t id; /**< CPU ID */
        struct {
                struct xwos_rtrq rt; /**< 实时就绪队列 */
        } rq; /**< 就绪队列 */
        struct xwos_sdobj_stack_info idle; /**< 空闲任务的栈信息 */
        xwsq_t req_schedule_cnt; /**< 请求调度的计数器 */
        __atomic xwsq_t req_chkpmpt_cnt; /**< 请求检查抢占的计数器 */
        __atomic xwsq_t dis_pmpt_cnt; /**< 关闭抢占的计数器 */
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
        __atomic xwsq_t req_bh_cnt; /**< 请求进入中断底半部的计数器 */
        __atomic xwsq_t dis_bh_cnt; /**< 关闭中断底半部的计数器 */
        struct xwos_bh_cb bhcb; /**< 中断底半部控制块 */
        struct xwos_sdobj_stack_info bh; /**< 中断底半部任务的栈信息 */
#endif /* XWSMPCFG_SD_BH */
        struct xwos_tt tt; /**< 时间树 */
        struct xwlk_splk cxlock; /**< 上下文切换的锁 */
        struct xwos_scheduler_lpm lpm; /**< 调度器低功耗控制块 */

        struct xwlib_bclst_head tcblist; /**< 链接本调度器中所有线程的链表头 */
        xwsz_t thrd_num; /**< 本调度器中的线程数量 */
        struct xwlk_splk tcblistlock; /**< 保护tcblist的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
extern __xwos_data
struct xwos_scheduler xwos_scheduler[CPUCFG_CPU_NUM];

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
struct xwos_scheduler * xwos_scheduler_get_lc(void);

__xwos_code
xwer_t xwos_scheduler_get_by_cpuid(xwid_t cpuid, struct xwos_scheduler ** ptrbuf);

__xwos_code
struct xwos_tcb * xwos_scheduler_get_ctcb(struct xwos_scheduler * xwsd);

__xwos_code
struct xwos_tcb * xwos_scheduler_get_ctcb_lc(void);

__xwos_code
struct xwos_scheduler * xwos_scheduler_dspmpt(struct xwos_scheduler * xwsd);

__xwos_code
struct xwos_scheduler * xwos_scheduler_enpmpt(struct xwos_scheduler * xwsd);

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
__xwos_code
struct xwos_scheduler * xwos_scheduler_dsbh(struct xwos_scheduler * xwsd);

__xwos_code
struct xwos_scheduler * xwos_scheduler_enbh(struct xwos_scheduler * xwsd);

__xwos_code
xwer_t xwos_scheduler_req_bh(struct xwos_scheduler * xwsd);
#endif /* XWSMPCFG_SD_BH */

__xwos_code
void xwos_scheduler_chkpmpt(struct xwos_scheduler * xwsd);

__xwos_code
void xwos_scheduler_chkpmpt_all(void);

__xwos_code
xwer_t xwos_scheduler_req_swcx(struct xwos_scheduler * xwsd);

__xwos_code
xwer_t xwos_scheduler_inc_wklkcnt(struct xwos_scheduler * xwsd);

__xwos_code
xwer_t xwos_scheduler_dec_wklkcnt(struct xwos_scheduler * xwsd);

__xwos_code
xwer_t xwos_scheduler_notify_allfrz(struct xwos_scheduler * xwsd);

/******** XWOS Lib for BSP Adaptation Code ********/
__xwos_code
void xwos_scheduler_finish_swcx(struct xwos_scheduler * xwsd);

__xwos_code
xwer_t xwos_scheduler_suspend_lic(struct xwos_scheduler * xwsd);

__xwos_code
xwer_t xwos_scheduler_resume_lic(struct xwos_scheduler * xwsd);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 禁止调度器进入暂停模式
 * @param xwsd: (I) 调度器控制块指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwos_scheduler_wakelock_lock(struct xwos_scheduler * xwsd)
{
        return xwos_scheduler_inc_wklkcnt(xwsd);
}

/**
 * @brief 允许调度器进入暂停模式
 * @param xwsd: (I) 调度器控制块指针
 * @return 错误码
 */
static __xw_inline
xwer_t xwos_scheduler_wakelock_unlock(struct xwos_scheduler * xwsd)
{
        return xwos_scheduler_dec_wklkcnt(xwsd);
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_init_code
xwer_t xwos_scheduler_init_lc(struct xwos_pmdm * xwpmdm);

__xwos_init_code
xwer_t xwos_scheduler_start_lc(void);

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
__xwos_api
struct xwos_scheduler * xwos_scheduler_dsbh_lc(void);

__xwos_api
struct xwos_scheduler * xwos_scheduler_enbh_lc(void);
#endif /* XWSMPCFG_SD_BH */

__xwos_api
struct xwos_scheduler * xwos_scheduler_dspmpt_lc(void);

__xwos_api
struct xwos_scheduler * xwos_scheduler_enpmpt_lc(void);

__xwos_api
xwer_t xwos_scheduler_suspend(xwid_t cpuid);

__xwos_api
xwer_t xwos_scheduler_resume(xwid_t cpuid);

__xwos_api
bool xwos_scheduler_tst_lpm(struct xwos_scheduler * xwsd);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS API：测试优先级是否合法
 * @retval true: 是
 * @retval false: 否
 */
static __xwos_inline_api
bool xwos_scheduler_prio_tst_valid(xwpr_t prio)
{
        return (prio <= XWOS_SD_PRIORITY_INVALID);
}

#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
/**
 * @brief XWOS API：测试调度器的当前上下文是否为中断底半部上下文
 * @param xwsd: (I) XWOS调度器
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
static __xwos_inline_api
bool xwos_scheduler_tst_in_bh(struct xwos_scheduler * xwsd)
{
        return (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk);
}

/**
 * @brief XWOS API：测试当前上下文是否为本地中断底半部上下文
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
static __xwos_inline_api
bool xwos_scheduler_tst_in_bh_lc(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_get_lc();
        return !!(XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk);
}
#endif /* XWSMPCFG_SD_BH */

#endif /* xwos/smp/scheduler.h */
