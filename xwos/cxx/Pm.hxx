/**
 * @file
 * @brief xwos::Pm
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_Pm_hxx__
#define __xwos_cxx_Pm_hxx__

#include <xwos/osal/pm.hxx>

namespace xwos {

/**
 * @defgroup xwos_cxx_Pm 电源管理库
 * @ingroup xwos_cxx
 *
 *
 * ## 电源管理操作函数
 *
 * `class Pm` 是抽象的类模板，用户必须为其提供实现，并定义4个基本操作函数：
 *
 * + `resumePeriph` ：恢复外设
 * + `suspendPeriph` ：暂停外设
 * + `wakeupCpu` ：唤醒CPU
 * + `sleepCpu` ：休眠CPU
 *
 * ## 模板参数
 *
 * `template<xwid_t TCpu> class Pm` 的模板参数 `TCpu` 表示电源管理类用于哪一个CPU。
 *
 * @{
 */

/**
 * @brief 电源管理类
 * @param[in] TCpu: 目标CPU
 */
template<xwid_t TCpu = 0UL>
class Pm
{
  public:
    enum PmStage : xwsq_t {
        PmStageSuspended = XWOS_PM_STAGE_SUSPENDED, /**< 已经暂停 */
        PmStageSuspending = XWOS_PM_STAGE_SUSPENDING, /**< 正在暂停 */
        PmStageResuming = XWOS_PM_STAGE_RESUMING, /**< 正在恢复 */
        PmStageFreezing = XWOS_PM_STAGE_FREEZING, /**< 正在冻结线程 */
        PmStageThawing = XWOS_PM_STAGE_THAWING, /**< 正在解冻线程 */
        PmStageRunning = XWOS_PM_STAGE_RUNNING, /**< 正常运行 */
    };

  protected:
    Pm() { xwos_pm_set_op(TCpu, sResumePeriph, sSuspendPeriph, sWakeupCpu, sSleepCpu, this); }
    virtual ~Pm() {}
    virtual void resumePeriph() = 0;
    virtual void suspendPeriph() = 0;
    virtual void wakeupCpu() = 0;
    virtual void sleepCpu() = 0;
    /**
     * @brief 进入低功耗模式
     * @note
     * 此C++API只能由 `class Pm` 的实现类调用，只会使得运行代码的CPU进入低功耗模式，不影响其他CPU。
     */
    void suspend() { xwos_pm_suspend(); }
  public:
    /**
     * @brief 从低功耗模式恢复
     * @note
     * 此C++API只能由唤醒中断调用，只会使得运行代码的CPU从低功耗模式恢复，不影响其他CPU。
     */
    void resume() { xwos_pm_resume(); }
    enum PmStage stage() { return (enum PmStage)xwos_pm_get_stage(); }

  private:
    static void sResumePeriph(void * obj)
    {
        Pm * mgr = reinterpret_cast<Pm *>(obj);
        mgr->resumePeriph();
    }
    static void sSuspendPeriph(void * obj)
    {
        Pm * mgr = reinterpret_cast<Pm *>(obj);
        mgr->suspendPeriph();
    }
    static void sWakeupCpu(void * obj)
    {
        Pm * mgr = reinterpret_cast<Pm *>(obj);
        mgr->wakeupCpu();
    }
    static void sSleepCpu(void * obj)
    {
        Pm * mgr = reinterpret_cast<Pm *>(obj);
        mgr->sleepCpu();
    }
};

/**
 * @} xwos_cxx_Pm
 */

} // namespace xwos

#endif /* xwos/cxx/Pm.hxx */
