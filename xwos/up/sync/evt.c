/**
 * @file
 * @brief 玄武OS UP内核同步机制：事件
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <string.h>
#include <xwos/lib/xwbop.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/ospl/irq.h>
#include <xwos/up/thd.h>
#include <xwos/up/lock/fakespinlock.h>
#include <xwos/up/sync/cond.h>
#include <xwos/up/sync/obj.h>
#include <xwos/up/sync/evt.h>

static __xwup_code
struct xwup_evt * xwup_evt_alloc(void);

static __xwup_code
void xwup_evt_free(struct xwup_evt * evt);

static __xwup_code
void xwup_evt_activate(struct xwup_evt * evt, xwbmp_t initval[], xwsq_t attr);

static __xwup_code
xwer_t xwup_evt_trywait_level(struct xwup_evt * evt,
                              xwsq_t trigger, xwsq_t action,
                              xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_evt_trywait_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[]);

static __xwup_code
xwer_t xwup_evt_timedwait_level(struct xwup_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[],
                                xwtm_t * xwtm);

static __xwup_code
xwer_t xwup_evt_timedwait_edge(struct xwup_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[],
                               xwtm_t * xwtm);

/**
 * @brief 从事件对象缓存中申请对象
 * @return 事件对象的指针
 */
static __xwup_code
struct xwup_evt * xwup_evt_alloc(void)
{
        union {
                struct xwup_evt * evt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwup_evt), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.evt = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.evt;
}

/**
 * @brief 释放事件对象
 * @param evt: (I) 事件对象的指针
 */
static __xwup_code
void xwup_evt_free(struct xwup_evt * evt)
{
        xwmm_kma_free(evt);
}

/**
 * @brief 激活事件对象
 * @param evt: (I) 事件对象的指针
 * @param initval: (I) 事件的数组的初始态
 * @param attr: (I) 事件的属性，取值 @ref xwup_evt_attr_em
 */
static __xwup_code
void xwup_evt_activate(struct xwup_evt * evt, xwbmp_t initval[], xwsq_t attr)
{
        xwsz_t size;

        size = BITS_TO_BMPS(XWUP_EVT_MAXNUM);
        xwup_cond_activate(&evt->cond);
        evt->attr = attr;
        switch (attr & XWUP_EVT_TYPE_MASK) {
        case XWUP_EVT_TYPE_FLG:
                memset(evt->msk, 0xFF, size);
                break;
        case XWUP_EVT_TYPE_BR:
                memset(evt->msk, 0, size);
                break;
        case XWUP_EVT_TYPE_SEL:
                memset(evt->msk, 0, size);
                break;
        default:
                memset(evt->msk, 0xFF, size);
                break;
        }
        if (NULL != initval) {
                memcpy(evt->bmp, initval, size);
        } else {
                memset(evt->bmp, 0, size);
        }
        xwup_splk_init(&evt->lock);
}

/**
 * @brief 使得事件对象无效
 * @param evt: (I) 事件对象的指针
 */
static __xwup_code
void xwup_evt_deactivate(struct xwup_evt * evt)
{
        XWOS_UNUSED(evt);
}

/**
 * @brief XWUP API：动态创建事件对象
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回对象的指针
 * @param initval: (I) 事件的数组的初始态，如果为NULL，初始值全部为0
 * @param attr: (I) 事件的属性，取值范围 @ref xwup_evt_attr_em
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_create(struct xwup_evt ** ptrbuf, xwbmp_t initval[], xwsq_t attr)
{
        struct xwup_evt * evt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);
        XWOS_VALIDATE(((attr & XWUP_EVT_TYPE_MASK) < XWUP_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        *ptrbuf = NULL;
        evt = xwup_evt_alloc();
        if (__xwcc_unlikely(is_err(evt))) {
                rc = ptr_err(evt);
        } else {
                xwup_evt_activate(evt, initval, attr);
                *ptrbuf = evt;
                rc = XWOK;
        }
        return rc;
}

/**
 * @brief XWUP API：删除动态创建的事件对象
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwup_api
xwer_t xwup_evt_delete(struct xwup_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwup_evt_deactivate(evt);
        xwup_evt_free(evt);
        return XWOK;
}

/**
 * @brief XWUP API：静态初始化事件对象
 * @param evt: (I) 事件对象的指针
 * @param initval: (I) 事件的数组的初始态，如果为NULL，初始值全部为0
 * @param attr: (I) 事件的属性，取值范围 @ref xwup_evt_attr_em
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -EINVAL: 无效参数
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwup_api
xwer_t xwup_evt_init(struct xwup_evt * evt, xwbmp_t initval[], xwsq_t attr)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE(((attr & XWUP_EVT_TYPE_MASK) < XWUP_EVT_TYPE_MAX),
                      "type-error", -EINVAL);

        xwup_evt_activate(evt, initval, attr);
        return XWOK;
}

/**
 * @brief XWUP API：销毁静态方式初始化的事件对象
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：异步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwup_api
xwer_t xwup_evt_destroy(struct xwup_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        xwup_evt_deactivate(evt);
        return XWOK;
}

/**
 * @brief XWUP API：绑定事件对象(evt)到另一个事件对象(sel)，
 *                  另一个事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param sel: (I) 类型为XWUP_EVT_TYPE_SEL的事件对象的指针
 * @param pos: (I) 事件对象对象映射到位图中的位置
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwup_api
xwer_t xwup_evt_bind(struct xwup_evt * evt, struct xwup_evt * sel, xwsq_t pos)
{
        return xwup_cond_bind(&evt->cond, sel, pos);
}

/**
 * @brief XWUP API：从另一个事件对象(sel)上解绑事件对象(evt)，
 *                  另一个事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param sel: (I) 类型为XWUP_EVT_TYPE_SEL的事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EFAULT: 空指针
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *evt* ，不可重入
 */
__xwup_api
xwer_t xwup_evt_unbind(struct xwup_evt * evt, struct xwup_evt * sel)
{
        return xwup_cond_unbind(&evt->cond, sel);
}

/**
 * @brief XWUP API：中断事件对象等待队列中的所有节点
 * @param evt: (I) 事件对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 类型不匹配
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_intr_all(struct xwup_evt * evt)
{
        XWOS_VALIDATE((evt), "nullptr", -EFAULT);

        return xwup_cond_intr_all(&evt->cond);
}

/******** type:XWUP_EVT_TYPE_FLG ********/
/**
 * @brief XWUP API：在事件对象中同时设置多个位图的位，
 *                  事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 事件对象的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_s1m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1m(evt->bmp, msk, XWUP_EVT_MAXNUM);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

/**
 * @brief XWUP API：在事件对象中设置单个位图的位，
 *                  事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_s1i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWUP_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

/**
 * @brief XWUP API：在事件对象中同时清除多个位图的位，
 *                  事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 事件对象的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_c0m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0m(evt->bmp, msk, XWUP_EVT_MAXNUM);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

/**
 * @brief XWUP API：在事件对象中清除单个位图的位，
 *                  事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_c0i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWUP_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_c0i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

/**
 * @brief XWUP API：在事件对象中同时翻转多个位图的位，
 *                  事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 事件对象的位图掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_x1m(struct xwup_evt * evt, xwbmp_t msk[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1m(evt->bmp, msk, XWUP_EVT_MAXNUM);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

/**
 * @brief XWUP API：在事件对象中翻转单个位图的位，
 *                  事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_x1i(struct xwup_evt * evt, xwsq_t pos)
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWUP_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_x1i(evt->bmp, pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;
}

/**
 * @brief XWUP API：读取事件对象位图的值
 * @param evt: (I) 事件对象的指针
 * @param out: (O) 指向缓冲区的指针，通过此缓冲区返回事件对象位图的值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_read(struct xwup_evt * evt, xwbmp_t out[])
{
        xwreg_t cpuirq;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((out), "nullptr", -EFAULT);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(out, evt->bmp, XWUP_EVT_MAXNUM);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;
}

static __xwup_code
xwer_t xwup_evt_trywait_level(struct xwup_evt * evt,
                              xwsq_t trigger, xwsq_t action,
                              xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        bool triggered;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWUP_EVT_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWUP_EVT_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        if (origin) {
                xwbmpop_assign(origin, evt->bmp, XWUP_EVT_MAXNUM);
        }
        if (XWUP_EVT_ACTION_CONSUMPTION == action) {
                switch (trigger) {
                case XWUP_EVT_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                          XWUP_EVT_MAXNUM);
                        break;
                case XWUP_EVT_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                          XWUP_EVT_MAXNUM);
                        break;
                case XWUP_EVT_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                          XWUP_EVT_MAXNUM);
                        break;
                case XWUP_EVT_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                          XWUP_EVT_MAXNUM);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        } else {
                switch (trigger) {
                case XWUP_EVT_TRIGGER_SET_ALL:
                        triggered = xwbmpop_t1ma(evt->bmp, msk,
                                                 XWUP_EVT_MAXNUM);
                        break;
                case XWUP_EVT_TRIGGER_SET_ANY:
                        triggered = xwbmpop_t1mo(evt->bmp, msk,
                                                 XWUP_EVT_MAXNUM);
                        break;
                case XWUP_EVT_TRIGGER_CLR_ALL:
                        triggered = xwbmpop_t0ma(evt->bmp, msk,
                                                 XWUP_EVT_MAXNUM);
                        break;
                case XWUP_EVT_TRIGGER_CLR_ANY:
                        triggered = xwbmpop_t0mo(evt->bmp, msk,
                                                 XWUP_EVT_MAXNUM);
                        break;
                default:
                        triggered = true;
                        rc = -EINVAL;
                        break;
                }
        }
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        if (!triggered) {
                rc = -ENODATA;
        }
        return rc;
}

static __xwup_code
xwer_t xwup_evt_trywait_edge(struct xwup_evt * evt, xwsq_t trigger,
                             xwbmp_t origin[], xwbmp_t msk[])
{
        xwreg_t cpuirq;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwup_evt_declare_bitmap(cur);
        xwup_evt_declare_bitmap(tmp);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, XWUP_EVT_MAXNUM);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_assign(cur, evt->bmp, XWUP_EVT_MAXNUM);
        xwbmpop_and(cur, msk, XWUP_EVT_MAXNUM);
        if (XWUP_EVT_TRIGGER_TGL_ALL == trigger) {
                xwbmpop_assign(tmp, cur, XWUP_EVT_MAXNUM);
                xwbmpop_xor(tmp, origin, XWUP_EVT_MAXNUM);
                cmprc = xwbmpop_cmp(tmp, msk, XWUP_EVT_MAXNUM);
                if (0 == cmprc) {
                        triggered = true;
                        rc = XWOK;
                } else {
                        triggered = false;
                        rc = -ENODATA;
                }
        } else if (XWUP_EVT_TRIGGER_TGL_ANY == trigger) {
                cmprc = xwbmpop_cmp(origin, cur, XWUP_EVT_MAXNUM);
                if (0 == cmprc) {
                        triggered = false;
                        rc = -ENODATA;
                } else {
                        triggered = true;
                        rc = XWOK;
                }
        } else {
                triggered = true;
                rc = -EINVAL;
        }
        if (triggered) {
                xwbmpop_assign(origin, cur, XWUP_EVT_MAXNUM);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        } else {
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

/**
 * @brief XWUP API：检测一下事件对象中的触发信号，不会阻塞调用者，
 *                  事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwup_evt_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwup_evt_action_em
 *                    仅当trigger取值
 *                    @ref XWUP_EVT_TRIGGER_SET_ALL
 *                    @ref XWUP_EVT_TRIGGER_SET_ANY
 *                    @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                    @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWUP_EVT_TRIGGER_SET_ALL
 *                  @ref XWUP_EVT_TRIGGER_SET_ANY
 *                  @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                  @ref XWUP_EVT_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWUP_EVT_TRIGGER_TGL_ALL
 *                  @ref XWUP_EVT_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件对象的位图掩码，表示只关注掩码部分的位
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINVAL: 参数无效
 * @retval -ENODATA: 没有任何事件触发信号
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_trywait(struct xwup_evt * evt,
                        xwsq_t trigger, xwsq_t action,
                        xwbmp_t origin[], xwbmp_t msk[])
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);

        if (trigger <= XWUP_EVT_TRIGGER_CLR_ANY) {
                rc = xwup_evt_trywait_level(evt, trigger, action, origin, msk);
        } else {
                rc = xwup_evt_trywait_edge(evt, trigger, origin, msk);
        }
        return rc;
}

static __xwup_code
xwer_t xwup_evt_timedwait_level(struct xwup_evt * evt,
                                xwsq_t trigger, xwsq_t action,
                                xwbmp_t origin[], xwbmp_t msk[],
                                xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((trigger <= XWUP_EVT_TRIGGER_CLR_ANY),
                      "illegal-trigger", -EINVAL);
        XWOS_VALIDATE((action < XWUP_EVT_ACTION_NUM),
                      "illegal-action", -EINVAL);

        rc = XWOK;
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                if (origin) {
                        xwbmpop_assign(origin, evt->bmp, XWUP_EVT_MAXNUM);
                }
                if (XWUP_EVT_ACTION_CONSUMPTION == action) {
                        switch (trigger) {
                        case XWUP_EVT_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma_then_c0m(evt->bmp, msk,
                                                                  XWUP_EVT_MAXNUM);
                                break;
                        case XWUP_EVT_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo_then_c0m(evt->bmp, msk,
                                                                  XWUP_EVT_MAXNUM);
                                break;
                        case XWUP_EVT_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma_then_s1m(evt->bmp, msk,
                                                                  XWUP_EVT_MAXNUM);
                                break;
                        case XWUP_EVT_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo_then_s1m(evt->bmp, msk,
                                                                  XWUP_EVT_MAXNUM);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                } else {
                        switch (trigger) {
                        case XWUP_EVT_TRIGGER_SET_ALL:
                                triggered = xwbmpop_t1ma(evt->bmp, msk,
                                                         XWUP_EVT_MAXNUM);
                                break;
                        case XWUP_EVT_TRIGGER_SET_ANY:
                                triggered = xwbmpop_t1mo(evt->bmp, msk,
                                                         XWUP_EVT_MAXNUM);
                                break;
                        case XWUP_EVT_TRIGGER_CLR_ALL:
                                triggered = xwbmpop_t0ma(evt->bmp, msk,
                                                         XWUP_EVT_MAXNUM);
                                break;
                        case XWUP_EVT_TRIGGER_CLR_ANY:
                                triggered = xwbmpop_t0mo(evt->bmp, msk,
                                                         XWUP_EVT_MAXNUM);
                                break;
                        default:
                                triggered = true;
                                rc = -EINVAL;
                                break;
                        }
                }
                if (triggered) {
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwospl_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

static __xwup_code
xwer_t xwup_evt_timedwait_edge(struct xwup_evt * evt, xwsq_t trigger,
                               xwbmp_t origin[], xwbmp_t msk[],
                               xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        xwsq_t lkst;
        xwssq_t cmprc;
        bool triggered;
        xwer_t rc;
        xwup_evt_declare_bitmap(cur);
        xwup_evt_declare_bitmap(tmp);

        XWOS_VALIDATE((origin), "nullptr", -EFAULT);

        xwbmpop_and(origin, msk, XWUP_EVT_MAXNUM);
        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                xwbmpop_assign(cur, evt->bmp, XWUP_EVT_MAXNUM);
                xwbmpop_and(cur, msk, XWUP_EVT_MAXNUM);
                if (XWUP_EVT_TRIGGER_TGL_ALL == trigger) {
                        xwbmpop_assign(tmp, cur, XWUP_EVT_MAXNUM);
                        xwbmpop_xor(tmp, origin, XWUP_EVT_MAXNUM);
                        cmprc = xwbmpop_cmp(tmp, msk, XWUP_EVT_MAXNUM);
                        if (0 == cmprc) {
                                triggered = true;
                                rc = XWOK;
                        } else {
                                triggered = false;
                        }
                } else if (XWUP_EVT_TRIGGER_TGL_ANY == trigger) {
                        cmprc = xwbmpop_cmp(origin, cur, XWUP_EVT_MAXNUM);
                        if (0 == cmprc) {
                                triggered = false;
                        } else {
                                triggered = true;
                                rc = XWOK;
                        }
                } else {
                        triggered = true;
                        rc = -EINVAL;
                }
                if (triggered) {
                        xwbmpop_assign(origin, cur, XWUP_EVT_MAXNUM);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        break;
                } else {
                        rc = xwup_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwospl_cpuirq_restore_lc(cpuirq);
                                break;
                        }
                }
        }
        return rc;
}

/**
 * @brief XWUP API：等待事件对象中的触发信号，事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwup_evt_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwup_evt_action_em，
 *                    仅当trigger取值
 *                    @ref XWUP_EVT_TRIGGER_SET_ALL
 *                    @ref XWUP_EVT_TRIGGER_SET_ANY
 *                    @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                    @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWUP_EVT_TRIGGER_SET_ALL
 *                  @ref XWUP_EVT_TRIGGER_SET_ANY
 *                  @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                  @ref XWUP_EVT_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWUP_EVT_TRIGGER_TGL_ALL
 *                  @ref XWUP_EVT_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件对象的位图掩码，表示只关注掩码部分的位
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINVAL: 参数无效
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_wait(struct xwup_evt * evt,
                     xwsq_t trigger, xwsq_t action,
                     xwbmp_t origin[], xwbmp_t msk[])
{
        xwtm_t expected = XWTM_MAX;
        return xwup_evt_timedwait(evt, trigger, action, origin, msk, &expected);
}

/**
 * @brief XWUP API：限时等待事件对象中的触发信号，事件对象类型为XWUP_EVT_TYPE_FLG
 * @param evt: (I) 事件对象的指针
 * @param trigger: (I) 事件触发条件，取值 @ref xwup_evt_trigger_em
 * @param action: (I) 事件触发后的动作，取值 @ref xwup_evt_action_em
 *                    仅当trigger取值
 *                    @ref XWUP_EVT_TRIGGER_SET_ALL
 *                    @ref XWUP_EVT_TRIGGER_SET_ANY
 *                    @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                    @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                    时有效，其他情况不使用此参数，可填 @ref XWOS_UNUSED_ARGUMENT
 * @param origin: 指向缓冲区的指针：
 *                - 当trigger取值
 *                  @ref XWUP_EVT_TRIGGER_SET_ALL
 *                  @ref XWUP_EVT_TRIGGER_SET_ANY
 *                  @ref XWUP_EVT_TRIGGER_CLR_ALL
 *                  @ref XWUP_EVT_TRIGGER_CLR_ANY
 *                  (O) 返回事件对象中位图状态（action之前）
 *                - 当trigger取值
 *                  @ref XWUP_EVT_TRIGGER_TGL_ALL
 *                  @ref XWUP_EVT_TRIGGER_TGL_ANY
 *                  (I) 作为输入时，作为用于比较的初始值
 *                  (O) 作为输出时，返回事件对象中位图状态
 *                      （可作为下一次调用的初始值）
 * @param msk: (I) 事件对象的位图掩码，表示只关注掩码部分的位
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINVAL: 参数无效
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwup_api
xwer_t xwup_evt_timedwait(struct xwup_evt * evt,
                          xwsq_t trigger, xwsq_t action,
                          xwbmp_t origin[], xwbmp_t msk[],
                          xwtm_t * xwtm)
{
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_FLG),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        if (trigger <= XWUP_EVT_TRIGGER_CLR_ANY) {
                rc = xwup_evt_timedwait_level(evt, trigger, action,
                                              origin, msk, xwtm);
        } else {
                rc = xwup_evt_timedwait_edge(evt, trigger, origin, msk, xwtm);
        }
        return rc;
}

/******** type:XWUP_EVT_TYPE_SEL ********/
/**
 * @brief 绑定同步对象到事件对象，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @param pos: (I) 同步对象映射到位图中的位置
 * @param exclusive: (I) 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -EALREADY: 同步对象已经绑定到事件对象
 * @retval -EBUSY: 通道已经被其他同步对象独占
 */
__xwup_code
xwer_t xwup_evt_obj_bind(struct xwup_evt * evt,
                         struct xwup_synobj * synobj,
                         xwsq_t pos,
                         bool exclusive)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        bool existed;
        xwer_t rc;

        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((pos < XWUP_EVT_MAXNUM), "out-of-range", -ECHRNG);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (NULL != owner) {
                rc = -EALREADY;
                goto err_already;
        }
        existed = xwbmpop_t1i(evt->msk, pos);
        if (existed) {
                rc = -EBUSY;
                goto err_busy;
        }
        if (exclusive) {
                xwbmpop_s1i(evt->msk, pos);
        }
        synobj->sel.evt = evt;
        synobj->sel.pos = pos;
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_busy:
err_already:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief XWUP API：从事件对象上解绑同步对象，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @param exclusive: (I) 是否为独占绑定
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_evt_obj_unbind(struct xwup_evt * evt,
                           struct xwup_synobj * synobj,
                           bool exclusive)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        if (exclusive) {
                xwbmpop_c0i(evt->msk, synobj->sel.pos);
        }
        synobj->sel.evt = NULL;
        synobj->sel.pos = XWUP_EVT_MAXNUM;
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief 在事件对象上设置同步对象的标志位，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_evt_obj_s1i(struct xwup_evt * evt, struct xwup_synobj * synobj)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_s1i(evt->bmp, synobj->sel.pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        xwup_cond_broadcast(&evt->cond);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief 在事件对象上清除同步对象的标志位，事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param obj: (I) 同步对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENOTCONN: 同步对象没有绑定到事件对象上
 */
__xwup_code
xwer_t xwup_evt_obj_c0i(struct xwup_evt * evt, struct xwup_synobj * synobj)
{
        struct xwup_evt * owner;
        xwreg_t cpuirq;
        xwer_t rc;

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        owner = synobj->sel.evt;
        if (evt != owner) {
                rc = -ENOTCONN;
                goto err_notconn;
        }
        xwbmpop_c0i(evt->bmp, synobj->sel.pos);
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return XWOK;

err_notconn:
        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        return rc;
}

/**
 * @brief XWUP API：等待事件对象中绑定的同步对象，
 *                  事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 待触发的同步对象的位图掩码（表示只关注掩码部分的同步对象）
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象的位图
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_select(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwtm_t expected = XWTM_MAX;
        return xwup_evt_timedselect(evt, msk, trg, &expected);
}

/**
 * @brief XWUP API：检测一下事件对象中绑定的同步对象，不会阻塞调用者
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 待触发的同步对象的位图掩码，表示只关注掩码部分的位
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象的位图
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ENODATA: 没有任何信号或事件
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_tryselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[])
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        triggered = xwbmpop_t1mo(evt->bmp, msk, XWUP_EVT_MAXNUM);
        if (triggered) {
                if (NULL != trg) {
                        xwbmpop_assign(trg, evt->bmp, XWUP_EVT_MAXNUM);
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWUP_EVT_MAXNUM);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        xwbmpop_and(trg, msk, XWUP_EVT_MAXNUM);
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWUP_EVT_MAXNUM);
                        xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                }
                rc = XWOK;
        } else {
                rc = -ENODATA;
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}

/**
 * @brief XWUP API：限时等待事件对象中绑定的同步对象，
 *                  事件对象类型为XWUP_EVT_TYPE_SEL
 * @param evt: (I) 事件对象的指针
 * @param msk: (I) 待触发的同步对象的位图掩码（表示只关注掩码部分的同步对象）
 * @param trg: (O) 指向缓冲区的指针，通过此缓冲区返回已触发的同步对象的位图
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwup_api
xwer_t xwup_evt_timedselect(struct xwup_evt * evt, xwbmp_t msk[], xwbmp_t trg[],
                            xwtm_t * xwtm)
{
        xwer_t rc;
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((msk), "nullptr", -EFAULT);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_SEL),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        while (true) {
                triggered = xwbmpop_t1mo(evt->bmp, msk, XWUP_EVT_MAXNUM);
                if (triggered) {
                        if (NULL != trg) {
                                xwbmpop_assign(trg, evt->bmp, XWUP_EVT_MAXNUM);
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, XWUP_EVT_MAXNUM);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                                xwbmpop_and(trg, msk, XWUP_EVT_MAXNUM);
                        } else {
                                /* Clear non-exclusive bits */
                                xwbmpop_and(evt->bmp, evt->msk, XWUP_EVT_MAXNUM);
                                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                        }
                        rc = XWOK;
                        break;
                } else {
                        /* Clear non-exclusive bits */
                        xwbmpop_and(evt->bmp, evt->msk, XWUP_EVT_MAXNUM);
                        rc = xwup_cond_timedwait(&evt->cond,
                                                 &evt->lock, XWOS_LK_SPLK, NULL,
                                                 xwtm, &lkst);
                        if (XWOK == rc) {
                                if (XWOS_LKST_UNLOCKED == lkst) {
                                        xwup_splk_lock(&evt->lock);
                                }
                        } else {
                                if (XWOS_LKST_LOCKED == lkst) {
                                        xwup_splk_unlock(&evt->lock);
                                }
                                xwospl_cpuirq_restore_lc(cpuirq);
                                if (NULL != trg) {
                                        xwbmpop_c0all(trg, XWUP_EVT_MAXNUM);
                                }
                                break;
                        }
                }
        }
        return rc;
}

/******** type:XWUP_EVT_TYPE_BR ********/
/**
 * @brief XWUP API：等待所有线程到达栅栏，事件对象类型为XWUP_EVT_TYPE_BR
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETYPE: 事件对象类型错误
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 */
__xwup_api
xwer_t xwup_evt_sync(struct xwup_evt * evt, xwsq_t pos, xwbmp_t sync[])
{
        xwtm_t expected = XWTM_MAX;
        return xwup_evt_timedsync(evt, pos, sync, &expected);
}

/**
 * @brief XWUP API：等待所有线程到达栅栏，事件对象类型为XWUP_EVT_TYPE_BR
 * @param evt: (I) 事件对象的指针
 * @param pos: (I) 当前线程的位图位置
 * @param sync: (I) 当前线程需要同步的线程掩码
 * @param xwtm: 指向缓冲区的指针，此缓冲区：
 *              (I) 作为输入时，表示期望的阻塞等待时间
 *              (O) 作为输出时，返回剩余的期望时间
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ETYPE: 事件对象类型错误
 * @retval -ECHRNG: 位置超出范围
 * @retval -ETIMEDOUT: 超时
 * @retval -EINTR: 等待被中断
 * @retval -ENOTINTHD: 不在线程上下文中
 * @note
 * - 同步/异步：同步
 * - 上下文：线程
 * - 重入性：可重入
 * @note
 * - 函数返回返回-ETIMEDOUT时，*xwtm* 指向的缓冲区内的期望时间会减为0。
 */
__xwup_api
xwer_t xwup_evt_timedsync(struct xwup_evt * evt, xwsq_t pos, xwbmp_t sync[],
                          xwtm_t * xwtm)
{
        xwreg_t cpuirq;
        bool triggered;
        xwsq_t lkst;
        xwer_t rc;

        XWOS_VALIDATE((evt), "nullptr", -EFAULT);
        XWOS_VALIDATE((sync), "nullptr", -EFAULT);
        XWOS_VALIDATE((pos < XWUP_EVT_MAXNUM), "out-of-range", -ECHRNG);
        XWOS_VALIDATE(((evt->attr & XWUP_EVT_TYPE_MASK) == XWUP_EVT_TYPE_BR),
                      "type-error", -ETYPE);
        XWOS_VALIDATE((xwtm), "nullptr", -EFAULT);
        XWOS_VALIDATE((-EINTHD == xwup_irq_get_id(NULL)),
                      "not-in-thd", -ENOTINTHD);

        xwup_splk_lock_cpuirqsv(&evt->lock, &cpuirq);
        xwbmpop_s1i(evt->bmp, pos);
        triggered = xwbmpop_t1ma(evt->bmp, sync, XWUP_EVT_MAXNUM);
        if (triggered) {
                xwbmpop_c0i(evt->bmp, pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
                xwup_cond_broadcast(&evt->cond);
                xwup_cthd_yield();
                rc = XWOK;
        } else {
                rc = xwup_cond_timedwait(&evt->cond,
                                         &evt->lock, XWOS_LK_SPLK, NULL,
                                         xwtm, &lkst);
                if (XWOS_LKST_UNLOCKED == lkst) {
                        xwup_splk_lock(&evt->lock);
                }
                xwbmpop_c0i(evt->bmp, pos);
                xwup_splk_unlock_cpuirqrs(&evt->lock, cpuirq);
        }
        return rc;
}
