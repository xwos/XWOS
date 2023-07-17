/**
 * @file
 * @brief 玄武OS移植层：调度器
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_ospl_skd_h__
#define __xwos_ospl_skd_h__

#include <xwos/standard.h>

#if defined(XWOS_CFG_CORE__mp)
#  include <xwos/mp/skd.h>
#  include <xwos/mp/thd.h>

#  define xwospl_skdobj_stack           xwmp_skdobj_stack
#  define xwospl_skd                    xwmp_skd
#  define xwospl_thd                    xwmp_thd
#  define XWOSPL_SKDOBJ_FLAG_PRIVILEGED XWMP_SKDOBJ_FLAG_PRIVILEGED
#elif defined(XWOS_CFG_CORE__up)
#  include <xwos/up/skd.h>
#  include <xwos/up/thd.h>

#  define xwospl_skdobj_stack           xwup_skdobj_stack
#  define xwospl_skd                    xwup_skd
#  define xwospl_thd                    xwup_thd
#  define XWOSPL_SKDOBJ_FLAG_PRIVILEGED XWUP_SKDOBJ_FLAG_PRIVILEGED
#else
#  error "Can't find the configuration XWOS_CFG_CORE!"
#endif

struct xwospl_skdobj_stack;
struct xwospl_skd;
struct xwospl_thd;

struct xwospl_skd * xwosplcb_skd_get_lc(void);
struct xwospl_thd * xwosplcb_skd_get_cthd_lc(void);
struct xwospl_skd * xwosplcb_skd_pre_swcx_lic(struct xwospl_skd * xwskd);
struct xwospl_skd * xwosplcb_skd_post_swcx_lic(struct xwospl_skd * xwskd);
xwer_t xwosplcb_skd_suspend_lic(struct xwospl_skd * xwskd);
xwer_t xwosplcb_skd_resume_lic(struct xwospl_skd * xwskd);
xwer_t xwosplcb_thd_exit_lic(struct xwospl_thd * thd, xwer_t rc);
xwer_t xwosplcb_thd_freeze_lic(struct xwospl_thd * thd);
void xwosplcb_thd_immigrate_lic(struct xwospl_thd * thd);
xwer_t xwosplcb_thd_outmigrate_lic(struct xwospl_thd * thd, xwid_t dstcpu);

/**
 * @brief 玄武OS移植层：初始化调度调度器
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 */
xwer_t xwospl_skd_init(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS移植层：初始化调度对象的栈
 * @param[in] stk: 栈信息结构体指针
 * @param[in] attr: 标志
 */
void xwospl_skd_init_stack(struct xwospl_skdobj_stack * stk,
                           void (* exit)(xwer_t));

/**
 * @brief 玄武OS移植层：获取当前CPU的ID
 */
xwid_t xwospl_skd_id_lc(void);

/**
 * @brief 玄武OS移植层：启动调度器
 * @param[in] xwskd: 调度器的指针
 */
xwer_t xwospl_skd_start(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS移植层：暂停调度器，用于电源管理
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 */
xwer_t xwospl_skd_suspend(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS移植层：继续调度器，用于电源管理
 * @param[in] xwskd: 调度器的指针
 * @return 错误码
 */
xwer_t xwospl_skd_resume(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS移植层：触发切换上下文的软中断
 * @param[in] xwskd: 调度器的指针
 */
static __xwbsp_inline
void xwospl_skd_req_swcx(struct xwospl_skd * xwskd);

/**
 * @brief 玄武OS移植层：切换上下文中断的服务函数
 */
void xwospl_skd_isr_swcx(void);

/**
 * @brief 玄武OS移植层：当前CPU上的线程退出
 * @param[in] thd: 线程对象的指针
 * @param[in] rc: 线程退出抛出的返回值
 */
void xwospl_thd_exit_lc(struct xwospl_thd * thd, xwer_t rc);

/**
 * @brief 玄武OS移植层：冻结当前CPU中正在运行的线程
 * @param[in] thd: 线程对象的指针
 */
xwer_t xwospl_thd_freeze_lc(struct xwospl_thd * thd);

#if defined(XWOS_CFG_CORE__mp)
/**
 * @brief 玄武OS移植层：将线程迁出其他CPU，并准备迁入其他CPU
 * @param[in] thd: 线程对象的指针
 * @param[in] cpuid: 目的地CPU的ID
 * @return 错误码
 */
xwer_t xwospl_thd_outmigrate(struct xwospl_thd * thd, xwid_t cpuid);

/**
 * @brief 玄武OS移植层：迁移线程至目标CPU
 * @param[in] thd: 线程对象的指针
 * @param[in] cpuid: 目的地CPU的ID
 */
void xwospl_thd_immigrate(struct xwospl_thd * thd, xwid_t cpuid);
#endif

#include <xwosimpl_skd.h>

#endif /* xwos/ospl/skd.h */
