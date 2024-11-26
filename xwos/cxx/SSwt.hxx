/**
 * @file
 * @brief xwos::SSwt
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_cxx_SSwt_hxx__
#define __xwos_cxx_SSwt_hxx__

#include <xwos/osal/swt.hxx>

namespace xwos {

/**
 * @defgroup xwos_cxx_SSwt 静态软件定时器
 * @ingroup xwos_cxx
 *
 * 静态软件定时器是指软件定时器所需要的内存在编译期由编译器分配。
 *
 *
 * ## C++ API
 *
 * 头文件： @ref xwos/cxx/SSwt.hxx
 *
 * @{
 */

/**
 * @brief 静态软件定时器
 */
class SSwt
{
  private:
    struct xwos_swt mSwt; /**< 软件定时器结构体 */
    xwer_t mCtorRc; /**< 软件定时器构造的结果 */

  public:
    /**
     * @brief 静态软件定时器构造函数
     * @param[in] flag: 软件定时器的标志，默认值：定时器为单次触发的
     */
    SSwt(xwsq_t flag = XWOS_SWT_FLAG_NULL);
    ~SSwt(); /**< 静态软件定时器析构函数 */
    xwer_t getCtorRc() { return mCtorRc; } /**< 获取静态软件定时器构造的结果 */
    /**
     * @brief 启动软件定时器
     * @param[in] origin: 软件定时器的初始时间
     * @param[in] period: 软件定时器的周期时间
     */
    xwer_t start(xwtm_t origin, xwtm_t period) {
        return xwos_swt_start(&mSwt, origin, period,
                              (xwos_swt_f)sSwtAlarmFunction, this);
    }
    xwer_t stop() { return xwos_swt_stop(&mSwt); } /**< 停止软件定时器 */

    /* 生命周期管理 */
    xwer_t grab() { return xwos_swt_grab(&mSwt); } /**< 增加引用计数 */
    xwer_t put() { return xwos_swt_put(&mSwt); } /**< 减少引用计数 */

  protected:
    virtual void swtAlarmFunction(); /**< 软件定时器报警函数 */

  private:
    static void sSwtAlarmFunction(struct xwos_swt * swt, SSwt * obj);
    static void * operator new(xwsz_t sz) = delete;
    void operator delete(void * obj) = delete;
};

/**
 * @} xwos_cxx_SSwt
 */

} // namespace xwos

#endif /* xwos/cxx/SSwt.hxx */
