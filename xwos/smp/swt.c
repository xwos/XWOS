/**
 * @file
 * @brief XuanWuOS内核：软件定时器
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
#include <xwos/object.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWSMPCFG_SD_SWT_MEMSLICE) && (1 == XWSMPCFG_SD_SWT_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWSMPCFG_SD_SWT_MEMSLICE */
#include <xwos/smp/cpu.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/tt.h>
#include <xwos/smp/swt.h>

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
void xwos_swt_construct(struct xwos_swt * swt);

static __xwos_code
void xwos_swt_destruct(struct xwos_swt * swt);

static __xwos_code
xwer_t xwos_swt_activate(struct xwos_swt * swt,
                         const char * name,
                         xwsq_t flag,
                         xwobj_gc_f gcfunc);

static __xwos_code
xwer_t xwos_swt_gc(void * swt);

static __xwos_code
void xwos_swt_ttn_cb(void * entry);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SD_SWT_MEMSLICE) && (1 == XWSMPCFG_SD_SWT_MEMSLICE)
/**
 * @brief 软件定时器对象缓存
 */
__xwos_data static struct xwmm_memslice * xwos_swt_cache = NULL;

/**
 * @brief 软件定时器对象缓存的名字
 */
__xwos_rodata const char xwos_swt_cache_name[] = "cache.swt.xwos";
#endif /* XWSMPCFG_SD_SWT_MEMSLICE */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWSMPCFG_SD_SWT_MEMSLICE) && (1 == XWSMPCFG_SD_SWT_MEMSLICE)
/**
 * @brief XWOS INIT CODE：初始化软件定时器对象缓存
 * @param zone_origin: (I) 内存区域首地址
 * @param zone_size: (I) 内存区域大小
 * @return 错误码
 * @note
 * - 只可在系统初始化时使用一次。
 */
__xwos_init_code
xwer_t xwos_swt_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwos_swt),
                                  xwos_swt_cache_name,
                                  (ctor_f)xwos_swt_construct,
                                  (dtor_f)xwos_swt_destruct);
        if (__unlikely(rc < 0)) {
                xwos_swt_cache = NULL;
        } else {
                xwos_swt_cache = msa;
        }
        return rc;
}
#endif /* XWSMPCFG_SD_SWT_MEMSLICE */

/**
 * @brief XWOS API：从软件定时器对象缓存中申请一个对象
 * @return 软件定时器对象的指针
 */
static __xwos_code
struct xwos_swt * xwos_swt_alloc(void)
{
#if defined(XWSMPCFG_SD_SWT_MEMSLICE) && (1 == XWSMPCFG_SD_SWT_MEMSLICE)
        union {
                struct xwos_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwos_swt_cache, &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(rc);
        }/* else {} */
        return mem.swt;
#else /* XWSMPCFG_SD_SWT_MEMSLICE */
        union {
                struct xwos_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwos_swt), XWMM_ALIGNMENT, &mem.anon);
        if (OK == rc) {
                xwos_swt_construct(mem.swt);
        } else {
                mem.swt = err_ptr(-ENOMEM);
        }
        return mem.swt;
#endif /* !XWSMPCFG_SD_SWT_MEMSLICE */
}

/**
 * @brief XWOS API：释放软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 */
static __xwos_code
void xwos_swt_free(struct xwos_swt * swt)
{
#if defined(XWSMPCFG_SD_SWT_MEMSLICE) && (1 == XWSMPCFG_SD_SWT_MEMSLICE)
        xwmm_memslice_free(xwos_swt_cache, swt);
#else /* XWSMPCFG_SD_SWT_MEMSLICE */
        xwos_swt_destruct(swt);
        xwmm_kma_free(swt);
#endif /* !XWSMPCFG_SD_SWT_MEMSLICE */
}

/**
 * @brief XWOS API：软件定时器对象的构造函数
 * @param swt: (I) 软件定时器对象的指针
 */
static __xwos_code
void xwos_swt_construct(struct xwos_swt * swt)
{
        xwos_object_construct(&swt->xwobj);
        swt->name = NULL;
        swt->xwsd = NULL;
        swt->flag = 0;
        xwos_ttn_init(&swt->ttn, (xwptr_t)swt, XWOS_TTN_TYPE_SWT);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
}

/**
 * @brief XWOS API：软件定时器对象的析构函数
 * @param swt: (I) 软件定时器对象的指针
 */
static __xwos_code
void xwos_swt_destruct(struct xwos_swt * swt)
{
        swt->name = NULL;
        swt->xwsd = NULL;
        swt->flag = 0;
        xwos_object_destruct(&swt->xwobj);
}

/**
 * @brief 软件定时器对象的垃圾回收函数
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 */
static __xwos_code
xwer_t xwos_swt_gc(void * swt)
{
        xwos_swt_free(swt);
        return OK;
}

/**
 * @brief 激活软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @param gcfunc: (I) 垃圾回收函数：当对象应用计数为0，调用此函数回收资源。
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
static __xwos_code
xwer_t xwos_swt_activate(struct xwos_swt * swt,
                         const char * name,
                         xwsq_t flag,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        if ((flag & XWOS_SWT_FLAG_RESTART) && (flag & XWOS_SWT_FLAG_AUTORM)) {
                flag &= (xwsq_t)(~(XWOS_SWT_FLAG_AUTORM));
        }
        rc = xwos_object_activate(&swt->xwobj, gcfunc);
        if (__unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }

        swt->xwsd = xwos_scheduler_get_lc();
        swt->name = name;
        swt->flag = flag;
        return OK;

err_xwobj_activate:
        return rc;
}

/**
 * @brief XWOS API：静态方式初始化软件定时器
 * @param swt: (I) 软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_swt_init(struct xwos_swt * swt,
                     const char * name,
                     xwsq_t flag)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        flag &= (xwsq_t)(~(XWOS_SWT_FLAG_AUTORM));
        xwos_swt_construct(swt);
        return xwos_swt_activate(swt, name, flag, NULL);
}

/**
 * @brief XWOS API：销毁静态方式初始化的软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_swt_destroy(struct xwos_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwos_swt_stop(swt);
        return xwos_swt_put(swt);
}

/**
 * @brief XWOS API：动态创建软件定时器对象
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_swt_create(struct xwos_swt ** ptrbuf,
                       const char * name,
                       xwsq_t flag)
{
        struct xwos_swt * swt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        swt = xwos_swt_alloc();
        if (is_err(swt)) {
                rc = ptr_err(swt);
                goto err_swt_alloc;
        }
        rc = xwos_swt_activate(swt, name, flag, xwos_swt_gc);
        if (__unlikely(rc < 0)) {
                goto err_swt_activate;
        }
        *ptrbuf = swt;
        return OK;

err_swt_activate:
        xwos_swt_free(swt);
err_swt_alloc:
        return rc;
}

/**
 * @brief XWOS API：删除动态创建的软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_swt_delete(struct xwos_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwos_swt_stop(swt);
        return xwos_swt_put(swt);
}

/**
 * @brief 软件定时器的时间树节点回调函数
 * @param entry: (I) 软件定时器对象的指针
 */
static __xwos_code
void xwos_swt_ttn_cb(void * entry)
{
        struct xwos_swt * swt;
        struct xwos_tt * xwtt;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwer_t rc;

        swt = entry;
        xwtt = &swt->xwsd->tt;
        swt->cb(swt, swt->arg);
        if (XWOS_SWT_FLAG_RESTART & swt->flag) {
                expected = xwtm_add_safely(swt->ttn.wkup_xwtm, swt->period);
                xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
                swt->ttn.wkup_xwtm = expected;
                swt->ttn.wkuprs = XWOS_TTN_WKUPRS_UNKNOWN;
                swt->ttn.cb = xwos_swt_ttn_cb;
                rc = xwos_tt_add_locked(xwtt, &swt->ttn, cpuirq);
                xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                XWOS_BUG_ON(rc < 0);
        } else if (XWOS_SWT_FLAG_AUTORM & swt->flag) {
                xwos_swt_put(swt);
        }/* else {} */
}

/**
 * @brief XWOS API：开启软件定时器
 * @param swt: (I) 软件定时器对象的指针
 * @param base: (I) 定时器初始时间
 * @param period: (I) 周期
 * @param cb: (I) 定时器回调函数
 * @param arg: (I) 定时器回调函数的参数
 * @return 错误码
 * @retval OK: OK
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
        struct xwos_tt * xwtt;
        xwtm_t expected;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cb), "nullptr", -EFAULT);
        XWOS_VALIDATE(((xwtm_cmp(base, 0) > 0) && (xwtm_cmp(period, 0) > 0)),
                      "out-of-time", -EINVAL);

        xwtt = &swt->xwsd->tt;
        expected = xwtm_add_safely(base, period);
        xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        if (__unlikely(NULL != swt->ttn.cb)) {
                rc = -EALREADY;
                goto err_already;
        }
        swt->cb = cb;
        swt->arg = arg;
        swt->period = period;
        /* 加入时间树 */
        swt->ttn.wkup_xwtm = expected;
        swt->ttn.wkuprs = XWOS_TTN_WKUPRS_UNKNOWN;
        swt->ttn.xwtt = xwtt;
        swt->ttn.cb = xwos_swt_ttn_cb;
        rc = xwos_tt_add_locked(xwtt, &swt->ttn, cpuirq);

err_already:
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}

/**
 * @brief XWOS API：停止软件定时器
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ESRCH: 定时器没有被开启
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwos_api
xwer_t xwos_swt_stop(struct xwos_swt * swt)
{
        xwreg_t cpuirq;
        xwer_t rc;
        struct xwos_tt * xwtt;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwtt = &swt->xwsd->tt;
        xwlk_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwos_tt_remove_locked(xwtt, &swt->ttn);
        xwlk_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}
