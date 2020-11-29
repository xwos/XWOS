/**
 * @file
 * @brief 玄武OS内核移植层：调度器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwos_ospl_skd_h__
#define __xwos_ospl_skd_h__

#include <xwos/standard.h>

#if defined(XuanWuOS_CFG_CORE__mp)
  #include <xwos/mp/skd.h>
  #include <xwos/mp/thrd.h>

  #define xwospl_skd_stack_info xwmp_skd_stack_info
  #define xwospl_skd xwmp_skd
  #define xwospl_tcb xwmp_tcb
#elif defined(XuanWuOS_CFG_CORE__up)
  #include <xwos/up/skd.h>
  #include <xwos/up/thrd.h>

  #define xwospl_skd_stack_info xwup_skd_stack_info
  #define xwospl_skd xwup_skd
  #define xwospl_tcb xwup_tcb
#else
  #error "Can't find the configuration XuanWuOS_CFG_CORE!"
#endif

struct xwospl_skd_stack_info;
struct xwospl_skd;
struct xwospl_tcb;

/**
 * @brief 玄武OS内核移植层：初始化调度调度器
 * @param xwskd: (I) 调度器的指针
 * @return 错误码
 */
xwer_t xwospl_skd_init(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS内核移植层：初始化调度对象的栈
 * @param stk: (I) 栈信息结构体指针
 * @param attr: (I) 标志
 */
void xwospl_skd_init_stack(struct xwospl_skd_stack_info * stk,
                           void (* exit)(xwer_t),
                           xwsq_t attr);

/**
 * @brief 玄武OS内核移植层：获取当前CPU的ID
 */
xwid_t xwospl_skd_get_id(void);

/**
 * @brief 玄武OS内核移植层：启动调度器
 * @param xwskd: (I) 调度器的指针
 */
xwer_t xwospl_skd_start_lc(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS内核移植层：暂停调度器，用于电源管理
 * @param xwskd: (I) 调度器的指针
 * @return 错误码
 */
xwer_t xwospl_skd_suspend(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS内核移植层：继续调度器，用于电源管理
 * @param xwskd: (I) 调度器的指针
 * @return 错误码
 */
xwer_t xwospl_skd_resume(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS内核移植层：发起切换上下文的软中断
 * @param xwskd: (I) 调度器的指针
 */
static __xwbsp_inline
void xwospl_skd_req_swcx(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS内核移植层：本地CPU上的线程退出
 * @param tcb: (I) 线程控制块对象的指针
 * @param rc: (I) 线程退出抛出的返回值
 */
void xwospl_thrd_exit_lc(struct xwospl_tcb * tcb, xwer_t rc);

/**
 * @brief 玄武OS内核移植层：冻结本地CPU中正在运行的线程
 * @param tcb: (I) 线程控制块对象的指针
 */
xwer_t xwospl_thrd_freeze_lc(struct xwospl_tcb * tcb);

#if defined(XuanWuOS_CFG_CORE__mp)
/**
 * @brief 玄武OS内核移植层：将线程迁出其他CPU，并准备迁入其他CPU
 * @param tcb: (I) 线程控制块对象的指针
 * @param cpuid: (I) 目的地CPU的ID
 * @return 错误码
 */
xwer_t xwospl_thrd_outmigrate(struct xwospl_tcb * tcb, xwid_t cpuid);

/**
 * @brief 玄武OS内核移植层：迁移线程至目标CPU
 * @param tcb: (I) 线程控制块对象的指针
 * @param cpuid: (I) 目的地CPU的ID
 */
void xwospl_thrd_immigrate(struct xwospl_tcb * tcb, xwid_t cpuid);
#endif /* XuanWuOS_CFG_CORE__mp */

#include <ospl_skd_impl.h>

#endif /* xwos/mp/ospl/skd.h */
