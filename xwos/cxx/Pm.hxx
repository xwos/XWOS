/**
 * @file
 * @brief xwos::Pm
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/Pm.hxx
 *
 * @{
 */

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
    Pm();
    ~Pm();
    virtual void onResume() = 0;
    virtual void onSuspend() = 0;
    virtual void onWakeup() = 0;
    virtual void onSleep() = 0;
    xwer_t suspend() { return xwos_pm_resume(); }
    xwer_t resume() { return xwos_pm_resume(); }
    enum PmStage stage() { return (enum PmStage)xwos_pm_get_stage(); }

  private:
    static void sOnResume(void * obj);
    static void sOnSuspend(void * obj);
    static void sOnWakeup(void * obj);
    static void sOnSleep(void * obj);
};

/**
 * @} xwos_cxx_Pm
 */

} // namespace xwos

#endif /* xwos/cxx/Pm.hxx */
