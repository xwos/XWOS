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

#ifndef __xwos_up_scheduler_h__
#define __xwos_up_scheduler_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/lib/bclst.h>
#include <xwos/up/rtrq.h>
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
  #include <xwos/up/bh.h>
#endif /* XWUPCFG_SD_BH */
#include <xwos/up/tt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#define XWOS_SD_PRIORITY_RT_NUM                 ((xwpr_t)XWUPCFG_SD_PRIORITY_RT_NUM)
#define XWOS_SD_PRIORITY_RT_MIN                 ((xwpr_t)0)
#define XWOS_SD_PRIORITY_RT_MAX                 (XWOS_SD_PRIORITY_RT_NUM - 1)
#define XWOS_SD_PRIORITY_INVALID                ((xwpr_t) - 1)
#define XWOS_SD_PRIORITY_RAISE(base, inc)       ((base) + (inc))
#define XWOS_SD_PRIORITY_DROP(base, dec)        ((base) - (dec))

#define XWOS_SCHEDULER_IDLE_STK(xwsd)  (&((xwsd)->idle))
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
  #define XWOS_SCHEDULER_BH_STK(xwsd)    (&((xwsd)->bh))
#endif /* XWUPCFG_SD_BH */

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
        XWSDOBJ_DST_RESERVED5 = (1U << 5U), /**< 保留位5 */
        XWSDOBJ_DST_FROZEN = (1U << 6U), /**< 已经被冻结 */
        XWSDOBJ_DST_EXITING = (1U << 7U), /**< 正在退出 */
        XWSDOBJ_DST_RESERVED8 = (1U << 8U), /**< 保留位8 */
        XWSDOBJ_DST_UNINTERRUPTED = (1U << 15U), /**< 阻塞态不可中断 */
        XWSDOBJ_DST_MASK = (XWSDOBJ_DST_STANDBY |
                            XWSDOBJ_DST_RUNNING |
                            XWSDOBJ_DST_READY |
                            XWSDOBJ_DST_SLEEPING |
                            XWSDOBJ_DST_BLOCKING |
                            XWSDOBJ_DST_FROZEN |
                            XWSDOBJ_DST_EXITING |
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
typedef xwer_t (* xwos_thrd_f)(void *);

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
 * @brief wakelock counter enumerations
 */
enum xwos_scheduler_wakelock_cnt_em {
        XWOS_SCHEDULER_WKLKCNT_SUSPENDED = 0, /**< 调度器已暂停 */
        XWOS_SCHEDULER_WKLKCNT_SUSPENDING, /**< 调度器正在暂停 */
        XWOS_SCHEDULER_WKLKCNT_RESUMING = XWOS_SCHEDULER_WKLKCNT_SUSPENDING,
        XWOS_SCHEDULER_WKLKCNT_ALLFRZ, /**< 调度器所有线程已冻结 */
        XWOS_SCHEDULER_WKLKCNT_FREEZING, /**< 正在冻结线程 */
        XWOS_SCHEDULER_WKLKCNT_THAWING = XWOS_SCHEDULER_WKLKCNT_FREEZING,
        XWOS_SCHEDULER_WKLKCNT_RUNNING, /**< 正常运行 */
        XWOS_SCHEDULER_WKLKCNT_UNLOCKED = XWOS_SCHEDULER_WKLKCNT_RUNNING,
                                        /**< 唤醒锁：未加锁 */
        XWOS_SCHEDULER_WKLKCNT_LOCKED, /**< 唤醒锁：已加锁 */
};

#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
typedef void (* xwos_scheduler_pm_cb_f)(void *); /**< 电源管理领域回调函数 */

/**
 * @brief 电源管理回调函数集合
 */
struct xwos_scheduler_pm_callback {
        xwos_scheduler_pm_cb_f resume; /**< 电源管理领域从暂停模式恢复的回调函数 */
        xwos_scheduler_pm_cb_f suspend; /**< 电源管理领域进入暂停模式的回调函数 */
        xwos_scheduler_pm_cb_f wakeup; /**< 唤醒电源管理领域的回调函数 */
        xwos_scheduler_pm_cb_f sleep; /**< 电源管理领域休眠的回调函数 */
        void * arg; /**< 各回调函数的参数 */
};

/**
 * @brief 调度器电源管理控制块
 */
struct xwos_scheduler_pm {
        __xw_io xwsq_t wklkcnt; /**< 唤醒锁，
                                     取值@ref xwos_scheduler_wakelock_cnt_em */
        xwsz_t frz_thrd_cnt; /**< 已冻结的线程计数器 */
        struct xwlib_bclst_head frzlist; /**< 已冻结的线程链表 */
        struct xwos_scheduler_pm_callback cb; /**< 电源管理回调函数集合 */
};
#endif /* XWUPCFG_SD_PM */

/**
 * @brief xwos scheduler
 */
struct xwos_scheduler {
        struct xwos_sdobj_stack_info * cstk; /**< 当前正在运行的线程的栈信息的指针
                                                  偏移：0，
                                                  汇编代码中会使用这个成员。*/
        struct xwos_sdobj_stack_info * pstk; /**< 前一个线程的栈信息的指针
                                                  偏移：sizeof(long)，
                                                  汇编代码中会使用这个成员。*/
        struct {
                struct xwos_rtrq rt; /**< 实时就绪队列 */
        } rq; /**< 就绪队列 */
        struct xwos_sdobj_stack_info idle; /**< 空闲任务的栈信息 */
        xwsq_t req_schedule_cnt; /**< 请求调度的计数器 */
        xwsq_t req_chkpmpt_cnt; /**< 请求检查抢占的计数器 */
        xwsq_t dis_pmpt_cnt; /**< 关闭抢占的计数器 */
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
        xwsq_t req_bh_cnt; /**< 请求进入中断底半部的计数器 */
        xwsq_t dis_bh_cnt; /**< 关闭中断底半部的计数器 */
        struct xwos_bh_cb bhcb; /**< 中断底半部控制块 */
        struct xwos_sdobj_stack_info bh; /**< 中断底半部任务的栈信息 */
#endif /* XWUPCFG_SD_BH */
        struct xwos_tt tt; /**< 时间树 */
#if defined(XWUPCFG_SD_PM) && (1 == XWUPCFG_SD_PM)
        struct xwos_scheduler_pm pm; /**< 调度器低功耗控制块 */
#endif /* XWUPCFG_SD_PM */
        struct xwlib_bclst_head tcblist; /**< 链接本调度器中所有线程的链表头 */
        xwsz_t thrd_num; /**< 本调度器中的线程数量 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     internal function prototypes    ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_code
struct xwos_scheduler * xwos_scheduler_get_lc(void);

__xwos_code
struct xwos_tcb * xwos_scheduler_get_ctcb_lc(void);

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
__xwos_code
xwer_t xwos_scheduler_req_bh(void);
#endif /* XWUPCFG_SD_BH */

__xwos_code
void xwos_scheduler_chkpmpt(void);

__xwos_code
xwer_t xwos_scheduler_req_swcx(void);

__xwos_code
xwer_t xwos_scheduler_inc_wklkcnt(void);

__xwos_code
xwer_t xwos_scheduler_dec_wklkcnt(void);

__xwos_code
void xwos_scheduler_intr_all(void);

__xwos_code
xwer_t xwos_scheduler_notify_allfrz_lic(void);

/******** XWOS Lib for BSP Adaptation Code ********/
__xwos_code
xwer_t xwos_scheduler_start_syshwt(void);

__xwos_code
xwer_t xwos_scheduler_stop_syshwt(void);

__xwos_code
void xwos_scheduler_finish_swcx();

__xwos_code
xwer_t xwos_scheduler_suspend_lic(struct xwos_scheduler * xwsd);

__xwos_code
xwer_t xwos_scheduler_resume_lic(struct xwos_scheduler * xwsd);

/******** ******** ******** ******** ******** ******** ******** ********
 ********       internal inline function implementations        ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 禁止调度器进入暂停模式
 * @return 错误码
 */
static __xw_inline
xwer_t xwos_scheduler_wakelock_lock(void)
{
        return xwos_scheduler_inc_wklkcnt();
}

/**
 * @brief 允许调度器进入暂停模式
 * @return 错误码
 */
static __xw_inline
xwer_t xwos_scheduler_wakelock_unlock(void)
{
        return xwos_scheduler_dec_wklkcnt();
}

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********       API function prototypes       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_init_code
xwer_t xwos_scheduler_init(void);

__xwos_init_code
xwer_t xwos_scheduler_start_lc(void);

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
__xwos_api
struct xwos_scheduler * xwos_scheduler_dsbh_lc(void);

__xwos_api
struct xwos_scheduler * xwos_scheduler_enbh_lc(void);
#endif /* XWUPCFG_SD_BH */

__xwos_api
struct xwos_scheduler * xwos_scheduler_dspmpt_lc(void);

__xwos_api
struct xwos_scheduler * xwos_scheduler_enpmpt_lc(void);

__xwos_api
void xwos_scheduler_set_pm_cb(xwos_scheduler_pm_cb_f resume_cb,
                              xwos_scheduler_pm_cb_f suspend_cb,
                              xwos_scheduler_pm_cb_f wakeup_cb,
                              xwos_scheduler_pm_cb_f sleep_cb,
                              void * arg);

__xwos_api
xwer_t xwos_scheduler_suspend(void);

__xwos_api
xwer_t xwos_scheduler_resume(void);

__xwos_api
xwsq_t xwos_scheduler_get_pm_state(void);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      inline API implementations     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 测试调度优先级是否合法
 * @retval true: 是
 * @retval false: 否
 */
static __xwos_inline_api
bool xwos_scheduler_prio_tst_valid(xwpr_t prio)
{
        return (prio <= XWOS_SD_PRIORITY_INVALID);
}

#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
/**
 * @brief XWOS API：测试当前上下文是否为中断底半部上下文
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 */
static __xwos_inline_api
bool xwos_scheduler_tst_in_bh_lc(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_get_lc();
        return (XWOS_SCHEDULER_BH_STK(xwsd) == xwsd->cstk);
}
#endif /* XWUPCFG_SD_BH */

#endif /* xwos/up/scheduler.h */
