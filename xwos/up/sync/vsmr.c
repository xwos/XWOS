/**
 * @file
 * @brief XuanWuOS的同步机制：信号量的虚基类
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
#include <xwos/up/irq.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
  #include <xwos/up/sync/object.h>
  #include <xwos/up/sync/event.h>
#endif /* XWUPCFG_SYNC_EVT */
#include <xwos/up/sync/vsmr.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 激活信号量对象
 * @param vsmr: (I) 信号量对象的指针
 */
__xwos_code
void xwsync_vsmr_activate(struct xwsync_vsmr * vsmr)
{
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        xwsync_object_activate(&vsmr->xwsyncobj);
#endif /* XWUPCFG_SYNC_EVT */
}

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
/**
 * @brief 绑定信号量到事件对象，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param vsmr: (I) 信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *vsmr* ，不可重入
 */
__xwos_code
xwer_t xwsync_vsmr_bind(struct xwsync_vsmr * vsmr, struct xwsync_evt * evt,
                        xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_cpuirq_save_lc(&cpuirq);
        rc = xwsync_evt_obj_bind(evt, &vsmr->xwsyncobj, pos, true);
        if ((OK == rc) && (vsmr->count > 0)) {
                rc = xwsync_evt_obj_s1i(evt, &vsmr->xwsyncobj);
        }
        xwos_cpuirq_restore_lc(cpuirq);

        return rc;
}

/**
 * @brief 从事件对象上解绑信号量，事件对象类型为XWSYNC_EVT_TYPE_SELECTOR
 * @param smr: (I) 信号量对象的指针
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *vsmr* ，不可重入
 */
__xwos_code
xwer_t xwsync_vsmr_unbind(struct xwsync_vsmr * vsmr, struct xwsync_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwos_cpuirq_save_lc(&cpuirq);
        rc = xwsync_evt_obj_unbind(evt, &vsmr->xwsyncobj, true);
        if (OK == rc) {
                rc = xwsync_evt_obj_c0i(evt, &vsmr->xwsyncobj);
        }
        xwos_cpuirq_restore_lc(cpuirq);

        return rc;
}
#endif /* XWUPCFG_SYNC_EVT */

/**
 * @brief 冻结信号量（值设置为负）
 * @param vsmr: (I) 信号量对象的基类指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 信号量已为负
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)，
 *   测试的线程会被加入到信号量的等待队列。
 */
__xwos_code
xwer_t xwsync_vsmr_freeze(struct xwsync_vsmr * vsmr)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = OK;
        xwos_cpuirq_save_lc(&cpuirq);
        if (__unlikely(vsmr->count < 0)) {
                rc = -EALREADY;
        } else {
                vsmr->count = XWSDYNC_VSMR_NEGTIVE;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                struct xwsync_evt * evt;
                struct xwsync_object * xwsyncobj;

                xwsyncobj = &vsmr->xwsyncobj;
                evt = xwsyncobj->selector.evt;
                if (NULL != evt) {
                        xwsync_evt_obj_c0i(evt, xwsyncobj);
                }
#endif /* XWUPCFG_SYNC_EVT */
        }
        xwos_cpuirq_restore_lc(cpuirq);

        return rc;
}

/**
 * @brief 解冻信号量，并重新初始化
 * @param vsmr: (I) 信号量对象的基类指针
 * @param val: (I) 信号量的初始值
 * @param max: (I) 信号量的最大值
 * @return 错误码
 * @retval OK: OK
 * @retval -EINVAL: 参数无效
 * @retval -EALREADY: 信号量不为负
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用。
 */
__xwos_code
xwer_t xwsync_vsmr_thaw(struct xwsync_vsmr * vsmr, xwssq_t val, xwssq_t max)
{
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE(((val >= 0) && (max > 0) && (val <= max)),
                      "invalid-value", -EINVAL);

        rc = OK;
        xwos_cpuirq_save_lc(&cpuirq);
        if (__unlikely(vsmr->count >= 0)) {
                rc = -EALREADY;
        } else {
                vsmr->max = max;
                vsmr->count = val;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                if (vsmr->count > 0) {
                        struct xwsync_evt * evt;
                        struct xwsync_object * xwsyncobj;

                        xwsyncobj = &vsmr->xwsyncobj;
                        evt = xwsyncobj->selector.evt;
                        if (NULL != evt) {
                                xwsync_evt_obj_s1i(evt, xwsyncobj);
                        }
                }
#endif /* XWUPCFG_SYNC_EVT */
        }
        xwos_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief 获取信号量计数器的值
 * @param vsmr: (I) 信号量对象的基类指针
 * @param sval: (O) 指向缓冲区的指针，通过此缓冲区返回信号量计数器的值
 * @return 错误码
 * @retval OK: OK
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwos_code
xwer_t xwsync_vsmr_getvalue(struct xwsync_vsmr * vsmr, xwssq_t * sval)
{
        *sval = vsmr->count;
        return OK;
}
