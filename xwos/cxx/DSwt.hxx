/**
 * @file
 * @brief xwos::DSwt
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_DSwt_hxx__
#define __xwos_cxx_DSwt_hxx__

#include <xwos/osal/swt.hxx>

namespace xwos {

/**
 * @defgroup xwos_cxx_DSwt 动态软件定时器
 * @ingroup xwos_cxx
 *
 * 动态软件定时器是指软件定时器所需要的内存空间是在运行时动态申请的。
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/DSwt.hxx
 *
 * @{
 */

/**
 * @brief 动态软件定时器
 */
class DSwt
{
  private:
    xwos_swt_d mSwtDesc; /**< 软件定时器对象描述符 */
    xwer_t mCtorRc; /**< 软件定时器构造的结果 */

  public:
    /**
     * @brief 动态软件定时器构造函数
     * @param[in] flag: 软件定时器的标志，默认值：定时器为单次触发的
     */
    DSwt(xwsq_t flag = XWOS_SWT_FLAG_NULL);
    ~DSwt(); /**< 动态软件定时器析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取动态软件定时器构造的结果 */
    /**
     * @brief 启动软件定时器
     * @param[in] origin: 软件定时器的初始时间
     * @param[in] period: 软件定时器的周期时间
     */
    xwer_t start(xwtm_t origin, xwtm_t period) {
        return xwos_swt_start(mSwtDesc.swt, origin, period,
                              (xwos_swt_f)sSwtAlarmFunction, this);
    }
    xwer_t stop() { return xwos_swt_stop(mSwtDesc.swt); } /**< 停止软件定时器 */
    struct xwos_swt * getXwosObj() { return mSwtDesc.swt; } /**< 获取XWOS对象指针 */

    /* 生命周期管理 */
    xwer_t acquire() { return xwos_swt_acquire(mSwtDesc); } /**< 增加引用计数 */
    xwer_t release() { return xwos_swt_release(mSwtDesc); } /**< 减少引用计数 */

  protected:
    virtual void swtAlarmFunction(); /**< 软件定时器报警函数 */

  private:
    static void sSwtAlarmFunction(struct xwos_swt * swt, DSwt * obj);
};

/**
 * @} xwos_cxx_DSwt
 */

} // namespace xwos

#endif /* xwos/cxx/DSwt.hxx */
