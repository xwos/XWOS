/**
 * @file
 * @brief 玄武OS UP内核同步机制：信号量的虚基类
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/ospl/irq.h>
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
#  include <xwos/up/sync/obj.h>
#  include <xwos/up/sync/evt.h>
#endif
#include <xwos/up/sync/vsem.h>

/**
 * @brief 激活信号量对象
 * @param[in] vsem: 信号量对象的指针
 */
__xwup_code
void xwup_vsem_activate(struct xwup_vsem * vsem)
{
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
        xwup_synobj_activate(&vsem->synobj);
#else
        XWOS_UNUSED(vsem);
#endif
}

#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
/**
 * @brief 绑定信号量到事件对象，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param[in] vsem: 信号量对象的指针
 * @param[in] evt: 事件对象的指针
 * @param[in] pos: 信号量对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个*vsem*，不可重入
 */
__xwup_code
xwer_t xwup_vsem_bind(struct xwup_vsem * vsem, struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_bind(evt, &vsem->synobj, pos, true);
        if ((XWOK == rc) && (vsem->count > 0)) {
                rc = xwup_sel_obj_s1i(evt, &vsem->synobj);
        }
        xwospl_cpuirq_restore_lc(cpuirq);

        return rc;
}

/**
 * @brief 从事件对象上解绑信号量，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param[in] sem: 信号量对象的指针
 * @param[in] evt: 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个*vsem*，不可重入
 */
__xwup_code
xwer_t xwup_vsem_unbind(struct xwup_vsem * vsem, struct xwup_evt * evt)
{
        xwreg_t cpuirq;
        xwer_t rc;

        xwospl_cpuirq_save_lc(&cpuirq);
        rc = xwup_sel_obj_unbind(evt, &vsem->synobj, true);
        if (XWOK == rc) {
                rc = xwup_sel_obj_c0i(evt, &vsem->synobj);
        }
        xwospl_cpuirq_restore_lc(cpuirq);

        return rc;
}
#endif

/**
 * @brief 冻结信号量（值设置为负）
 * @param[in] vsem: 信号量对象的基类指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EALREADY: 信号量已为负
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 已冻结的信号量不允许增加(V操作)，但可以被测试(P操作)，
 *   测试的线程会被加入到信号量的等待队列。
 */
__xwup_code
xwer_t xwup_vsem_freeze(struct xwup_vsem * vsem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_unlikely(vsem->count < 0)) {
                rc = -EALREADY;
        } else {
                vsem->count = XWUP_VSEM_NEGTIVE;
#if defined(XWUPCFG_SYNC_EVT) && (1 == XWUPCFG_SYNC_EVT)
                struct xwup_evt * evt;
                struct xwup_synobj * synobj;

                synobj = &vsem->synobj;
                evt = synobj->sel.evt;
                if (NULL != evt) {
                        xwup_sel_obj_c0i(evt, synobj);
                }
#endif
        }
        xwospl_cpuirq_restore_lc(cpuirq);

        return rc;
}

/**
 * @brief 解冻信号量
 * @param[in] vsem: 信号量对象的基类指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 参数无效
 * @retval -EALREADY: 信号量不为负
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 * @note
 * - 此函数只对已冻结的信号量起作用，对未冻结的信号量调用此函数将返回错误码。
 */
__xwup_code
xwer_t xwup_vsem_thaw(struct xwup_vsem * vsem)
{
        xwer_t rc;
        xwreg_t cpuirq;

        rc = XWOK;
        xwospl_cpuirq_save_lc(&cpuirq);
        if (__xwcc_unlikely(vsem->count >= 0)) {
                rc = -EALREADY;
        } else {
                vsem->count = 0;
        }
        xwospl_cpuirq_restore_lc(cpuirq);
        return rc;
}

/**
 * @brief 获取信号量计数器的值
 * @param[in] vsem: 信号量对象的基类指针
 * @param[out] sval: 指向缓冲区的指针，通过此缓冲区返回信号量计数器的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_code
xwer_t xwup_vsem_getvalue(struct xwup_vsem * vsem, xwssq_t * sval)
{
        *sval = vsem->count;
        return XWOK;
}
