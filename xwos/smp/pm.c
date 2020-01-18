/**
 * @file
 * @brief XuanWuOS内核：内核电源管理
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/smp/cpu.h>
#include <xwos/smp/pm.h>
#include <xwos/smp/scheduler.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS PM API：初始化电源管理领域
 * @param pmdm: (I) 电源管理领域控制块指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
void xwos_pmdm_init(struct xwos_pmdm * pmdm)
{
        pmdm->lpmxwsd_cnt = 0;
}

/**
 * @brief XWOS PM API：得到当前CPU所属的电源管理领域控制块的指针
 * @param pmdm: (I) 电源管理领域控制块的指针
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
struct xwos_pmdm * xwos_pmdm_get_lc(void)
{
        struct xwos_scheduler * xwsd;

        xwsd = xwos_scheduler_get_lc();
        return xwsd->lpm.xwpmdm;
}

/**
 * @brief XWOS PM API：设置电源管理领域处于低功耗时的回调函数
 * @param pmdm: (I) 电源管理领域控制块指针
 * @param cb: (I) 回调函数指针
 * @param arg: (I) 回调函数调用时的参数
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 * @note
 * - 当所有CPU都已经进入低功耗模式，回调函数被调用。
 */
__xwos_api
void xwos_pmdm_set_lpmntf_cb(struct xwos_pmdm * pmdm, lpmntf_cb_f cb,
                             void * arg)
{
        pmdm->lpmntf_cb = cb;
        pmdm->arg = arg;
}

/**
 * @brief XWOS PM API：暂停电源管理领域中所有调度器
 * @param pmdm: (I) 电源管理领域控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 中断上下文：不可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
xwer_t xwos_pmdm_suspend(struct xwos_pmdm * pmdm)
{
        xwid_t cpuid;
        xwer_t rc;

        rc = OK;
        for (cpuid = 0; cpuid < pmdm->xwsd_num; cpuid++) {
                rc = xwos_scheduler_suspend(cpuid);
                if (__unlikely(rc < 0)) {
                        break;
                }
        }
        return rc;
}

/**
 * @brief XWOS PM API：继续电源管理领域中所有调度器
 * @param pmdm: (I) 电源管理领域控制块指针
 * @return 错误码
 * @note
 * - 同步/异步：异步
 * - 中断上下文：可以使用
 * - 中断底半部：不可以使用
 * - 线程上下文：不可以使用
 */
__xwos_api
xwer_t xwos_pmdm_resume(struct xwos_pmdm * pmdm)
{
        struct xwos_scheduler * xwsd;
        xwid_t cpuid;
        xwid_t localid;
        xwer_t rc;

        rc = OK;
        localid = xwos_cpu_get_id();
        for (cpuid = 0; cpuid < pmdm->xwsd_num; cpuid++) {
                if (cpuid == localid) {
                        xwsd = xwos_scheduler_get_lc();
                        rc = xwos_scheduler_resume_lic(xwsd);
                } else {
                        rc = xwos_scheduler_resume(cpuid);
                }
                if (__unlikely(rc < 0)) {
                        break;
                }/* else {} */
        }
        return rc;
}

/**
 * @brief XWOS PM API：测试电源管理领域中所有调度器是否都已经暂停
 * @param pmdm: (I) 电源管理领域控制块指针
 * @return 布尔值
 * @retval true: 是
 * @retval false: 否
 * @note
 * - 同步/异步：同步
 * - 中断上下文：可以使用
 * - 中断底半部：可以使用
 * - 线程上下文：可以使用
 */
__xwos_api
bool xwos_pmdm_tst_lpm(struct xwos_pmdm * pmdm)
{
        xwsz_t lpmxwsd_cnt;

        lpmxwsd_cnt = xwaop_load(xwsz_t, &pmdm->lpmxwsd_cnt, xwmb_modr_consume);
        return (lpmxwsd_cnt == pmdm->xwsd_num);
}

/**
 * @brief 增加已暂停的调度器的数量
 * @param pmdm: (I) 电源管理领域控制块指针
 */
__xwos_code
void xwos_pmdm_inc_lpmxwsd_cnt(struct xwos_pmdm * pmdm)
{
        xwsz_t lpmxwsd_cnt;

        xwaop_add(xwsz_t, &pmdm->lpmxwsd_cnt, 1, &lpmxwsd_cnt, NULL);
        if (pmdm->xwsd_num == lpmxwsd_cnt) {
                if (pmdm->lpmntf_cb) {
                        pmdm->lpmntf_cb(pmdm, pmdm->arg);
                }/* else {} */
        }
}

/**
 * @brief 减少已暂停的调度器的数量
 * @param pmdm: (I) 电源管理领域控制块指针
 */
__xwos_code
void xwos_pmdm_dec_lpmxwsd_cnt(struct xwos_pmdm * pmdm)
{
        xwaop_sub(xwsz_t, &pmdm->lpmxwsd_cnt, 1, NULL, NULL);
}
