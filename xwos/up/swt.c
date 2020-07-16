/**
 * @file
 * @brief XuanWuOS内核：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwos/standard.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/tt.h>
#include <xwos/up/swt.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      static function prototypes     ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
static __xwos_code
struct xwos_swt * xwos_swt_alloc(void);

static __xwos_code
void xwos_swt_free(struct xwos_swt * swt);

static __xwos_code
void xwos_swt_activate(struct xwos_swt * swt,
                       const char * name,
                       xwsq_t flag);

static __xwos_code
void xwos_swt_deactivate(struct xwos_swt * swt);

static __xwos_code
void xwos_swt_ttn_cb(void * entry);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 从软件定时器对象缓存中申请一个对象
 * @return 软件定时器对象的指针
 */
static __xwos_code
struct xwos_swt * xwos_swt_alloc(void)
{
        union {
                struct xwos_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwos_swt), XWMM_ALIGNMENT, &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(-ENOMEM);
        }/* else {} */
        return mem.swt;
}

/**
 * @brief XWOS API：释放软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 */
static __xwos_code
void xwos_swt_free(struct xwos_swt * swt)
{
        xwmm_kma_free(swt);
}

/**
 * @brief 激活软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 */
static __xwos_code
void xwos_swt_activate(struct xwos_swt * swt,
                       const char * name,
                       xwsq_t flag)
{
        flag &= (xwsq_t)(~(XWOS_SWT_FLAG_AUTORM));
        xwos_ttn_init(&swt->ttn, (xwptr_t)swt, XWOS_TTN_TYPE_SWT);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
        swt->name = name;
        swt->flag = flag;
}

/**
 * @brief 使得软件定时器对象无效
 * @param swt: (I) 软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 */
static __xwos_code
void xwos_swt_deactivate(struct xwos_swt * swt)
{
        xwos_swt_stop(swt);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
        swt->name = NULL;
        swt->flag = 0;
}

/**
 * @brief XWOS API：初始化软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *swt* ，不可重入
 */
__xwos_api
xwer_t xwos_swt_init(struct xwos_swt * swt,
                     const char * name,
                     xwsq_t flag)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        xwos_swt_activate(swt, name, flag);
        return XWOK;
}

/**
 * @brief XWOS API：销毁软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *swt* ，不可重入
 */
__xwos_api
xwer_t xwos_swt_destroy(struct xwos_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwos_swt_deactivate(swt);
        return XWOK;
}

/**
 * @brief XWOS API：动态创建软件定时器对象
 * @param ptrbuf: (O) 指向缓冲区的指针，通过此缓冲区返回软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_swt_create(struct xwos_swt ** ptrbuf,
                       const char * name,
                       xwsq_t flag)
{
        struct xwos_swt * swt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        if ((flag & XWOS_SWT_FLAG_RESTART) && (flag & XWOS_SWT_FLAG_AUTORM)) {
                flag &= (xwsq_t)(~(XWOS_SWT_FLAG_AUTORM));
        }

        *ptrbuf = NULL;
        swt = xwos_swt_alloc();
        if (is_err(swt)) {
                rc = ptr_err(swt);
                goto err_swt_alloc;
        }
        xwos_swt_activate(swt, name, flag);
        *ptrbuf = swt;
        return XWOK;

err_swt_alloc:
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：对于同一个 *swt* ，不可重入
 */
__xwos_api
xwer_t xwos_swt_delete(struct xwos_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwos_swt_deactivate(swt);
        xwos_swt_free(swt);
        return XWOK;
}

/**
 * @brief 软件定时器的时间树节点回调函数
 * @param entry: (I) 软件定时器对象的指针
 */
static __xwos_code
void xwos_swt_ttn_cb(void * entry)
{
        struct xwos_swt * swt;
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwer_t rc;

        swt = entry;
        xwsd = xwos_scheduler_get_lc();
        xwtt = &xwsd->tt;
        swt->cb(swt, swt->arg);
        if (XWOS_SWT_FLAG_RESTART & swt->flag) {
                expected = xwtm_add_safely(swt->ttn.wkup_xwtm, swt->period);
                xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
                swt->ttn.wkup_xwtm = expected;
                swt->ttn.wkuprs = XWOS_TTN_WKUPRS_UNKNOWN;
                swt->ttn.cb = xwos_swt_ttn_cb;
                rc = xwos_tt_add_locked(xwtt, &swt->ttn, cpuirq);
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                if (__unlikely(rc < 0)) {
                }
        }
}

/**
 * @brief XWOS API：开启软件定时器
 * @param swt: (I) 软件定时器对象的指针
 * @param base: (I) 定时器初始时间
 * @param period: (I) 周期
 * @param cb: (I) 定时器回调函数
 * @param arg: (I) 定时器回调函数的参数
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 无效参数
 * @retval -EALREADY: 定时器已经开启
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_swt_start(struct xwos_swt * swt,
                      xwtm_t base, xwtm_t period,
                      xwos_swt_f cb, void * arg)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        xwtm_t expected;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cb), "nullptr", -EFAULT);
        XWOS_VALIDATE(((xwtm_cmp(base, 0) > 0) && (xwtm_cmp(period, 0) > 0)),
                      "out-of-time", -EINVAL);

        if (__unlikely(swt->ttn.cb)) {
                rc = -EALREADY;
                goto err_already;
        }
        xwsd = xwos_scheduler_get_lc();
        xwtt = &xwsd->tt;
        expected = xwtm_add_safely(base, period);
        swt->cb = cb;
        swt->arg = arg;
        swt->period = period;
        xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        /* add to time tree */
        swt->ttn.wkup_xwtm = expected;
        swt->ttn.wkuprs = XWOS_TTN_WKUPRS_UNKNOWN;
        swt->ttn.cb = xwos_swt_ttn_cb;
        swt->ttn.xwtt = xwtt;
        rc = xwos_tt_add_locked(xwtt, &swt->ttn, cpuirq);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);

err_already:
        return rc;
}

/**
 * @brief XWOS API：停止软件定时器
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 定时器没有被开启
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_swt_stop(struct xwos_swt * swt)
{
        struct xwos_scheduler * xwsd;
        struct xwos_tt * xwtt;
        xwreg_t cpuirq;
        xwer_t rc;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwsd = xwos_scheduler_get_lc();
        xwtt = &xwsd->tt;
        xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwos_tt_remove_locked(xwtt, &swt->ttn);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}
