/**
 * @file
 * @brief 玄武OS MP内核：软件定时器
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/mm/common.h>
#include <xwos/mm/kma.h>
#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
  #include <xwos/mm/memslice.h>
#endif /* XWMPCFG_SKD_SWT_MEMSLICE */
#include <xwos/mp/skd.h>
#include <xwos/mp/tt.h>
#include <xwos/mp/swt.h>

static __xwmp_code
struct xwmp_swt * xwmp_swt_alloc(void);

static __xwmp_code
void xwmp_swt_free(struct xwmp_swt * swt);

static __xwmp_code
void xwmp_swt_construct(struct xwmp_swt * swt);

static __xwmp_code
void xwmp_swt_destruct(struct xwmp_swt * swt);

static __xwmp_code
xwer_t xwmp_swt_activate(struct xwmp_swt * swt,
                         const char * name,
                         xwsq_t flag,
                         xwobj_gc_f gcfunc);

static __xwmp_code
xwer_t xwmp_swt_gc(void * swt);

static __xwmp_code
void xwmp_swt_ttn_cb(void * entry);

#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
/**
 * @brief 软件定时器对象缓存
 */
__xwmp_data static struct xwmm_memslice * xwmp_swt_cache = NULL;

/**
 * @brief 软件定时器对象缓存的名字
 */
__xwmp_rodata const char xwmp_swt_cache_name[] = "xwos.mp.swt.cache";
#endif /* XWMPCFG_SKD_SWT_MEMSLICE */

#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
/**
 * @brief XWMP INIT CODE：初始化软件定时器对象缓存
 * @param zone_origin: (I) 内存区域首地址
 * @param zone_size: (I) 内存区域大小
 * @return 错误码
 * @note
 * - 重入性：只可在系统初始化时使用一次
 */
__xwmp_init_code
xwer_t xwmp_swt_cache_init(xwptr_t zone_origin, xwsz_t zone_size)
{
        struct xwmm_memslice * msa;
        xwer_t rc;

        rc = xwmm_memslice_create(&msa, zone_origin, zone_size,
                                  sizeof(struct xwmp_swt),
                                  xwmp_swt_cache_name,
                                  (ctor_f)xwmp_swt_construct,
                                  (dtor_f)xwmp_swt_destruct);
        if (__xwcc_unlikely(rc < 0)) {
                xwmp_swt_cache = NULL;
        } else {
                xwmp_swt_cache = msa;
        }
        return rc;
}
#endif /* XWMPCFG_SKD_SWT_MEMSLICE */

/**
 * @brief 从软件定时器对象缓存中申请一个对象
 * @return 软件定时器对象的指针
 */
static __xwmp_code
struct xwmp_swt * xwmp_swt_alloc(void)
{
#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
        union {
                struct xwmp_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_memslice_alloc(xwmp_swt_cache, &mem.anon);
        if (rc < 0) {
                mem.swt = err_ptr(rc);
        }/* else {} */
        return mem.swt;
#else /* XWMPCFG_SKD_SWT_MEMSLICE */
        union {
                struct xwmp_swt * swt;
                void * anon;
        } mem;
        xwer_t rc;

        rc = xwmm_kma_alloc(sizeof(struct xwmp_swt), XWMM_ALIGNMENT, &mem.anon);
        if (XWOK == rc) {
                xwmp_swt_construct(mem.swt);
        } else {
                mem.swt = err_ptr(-ENOMEM);
        }
        return mem.swt;
#endif /* !XWMPCFG_SKD_SWT_MEMSLICE */
}

/**
 * @brief 释放软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_free(struct xwmp_swt * swt)
{
#if defined(XWMPCFG_SKD_SWT_MEMSLICE) && (1 == XWMPCFG_SKD_SWT_MEMSLICE)
        xwmm_memslice_free(xwmp_swt_cache, swt);
#else /* XWMPCFG_SKD_SWT_MEMSLICE */
        xwmp_swt_destruct(swt);
        xwmm_kma_free(swt);
#endif /* !XWMPCFG_SKD_SWT_MEMSLICE */
}

/**
 * @brief 软件定时器对象的构造函数
 * @param swt: (I) 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_construct(struct xwmp_swt * swt)
{
        xwos_object_construct(&swt->xwobj, XWOS_OBJ_SWT);
        swt->name = NULL;
        swt->xwskd = NULL;
        swt->flag = 0;
        xwmp_ttn_init(&swt->ttn, (xwptr_t)swt, XWMP_TTN_TYPE_SWT);
        swt->cb = NULL;
        swt->arg = NULL;
        swt->period = 0;
}

/**
 * @brief 软件定时器对象的析构函数
 * @param swt: (I) 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_destruct(struct xwmp_swt * swt)
{
        swt->name = NULL;
        swt->xwskd = NULL;
        swt->flag = 0;
        xwos_object_destruct(&swt->xwobj);
}

/**
 * @brief 软件定时器对象的垃圾回收函数
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 */
static __xwmp_code
xwer_t xwmp_swt_gc(void * swt)
{
        xwmp_swt_free(swt);
        return XWOK;
}

/**
 * @brief 增加对象的引用计数
 * @param swt: (I) 软件定时器控制块对象指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_swt_grab(struct xwmp_swt * swt)
{
        return xwos_object_grab(&swt->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param swt: (I) 软件定时器控制块对象指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_swt_put(struct xwmp_swt * swt)
{
        return xwos_object_put(&swt->xwobj);
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
static __xwmp_code
xwer_t xwmp_swt_activate(struct xwmp_swt * swt,
                         const char * name,
                         xwsq_t flag,
                         xwobj_gc_f gcfunc)
{
        xwer_t rc;

        if ((flag & XWMP_SWT_FLAG_RESTART) && (flag & XWMP_SWT_FLAG_AUTORM)) {
                flag &= (xwsq_t)(~(XWMP_SWT_FLAG_AUTORM));
        }
        rc = xwos_object_activate(&swt->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_xwobj_activate;
        }

        swt->xwskd = xwmp_skd_get_lc();
        swt->name = name;
        swt->flag = flag;
        return XWOK;

err_xwobj_activate:
        return rc;
}

/**
 * @brief XWMP API：静态方式初始化软件定时器
 * @param swt: (I) 软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_swt_init(struct xwmp_swt * swt,
                     const char * name,
                     xwsq_t flag)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        flag &= (xwsq_t)(~(XWMP_SWT_FLAG_AUTORM));
        xwmp_swt_construct(swt);
        return xwmp_swt_activate(swt, name, flag, NULL);
}

/**
 * @brief XWMP API：销毁静态方式初始化的软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_swt_destroy(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwmp_swt_stop(swt);
        return xwmp_swt_put(swt);
}

/**
 * @brief XWMP API：动态创建软件定时器对象
 * @param ptrbuf: (O) 指向缓冲区的指针，此缓冲区用于返回软件定时器对象的指针
 * @param name: (I) 名字
 * @param flag: (I) 标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @retval -ENOMEM: 内存不足
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_swt_create(struct xwmp_swt ** ptrbuf,
                       const char * name,
                       xwsq_t flag)
{
        struct xwmp_swt * swt;
        xwer_t rc;

        XWOS_VALIDATE((ptrbuf), "nullptr", -EFAULT);

        *ptrbuf = NULL;
        swt = xwmp_swt_alloc();
        if (is_err(swt)) {
                rc = ptr_err(swt);
                goto err_swt_alloc;
        }
        rc = xwmp_swt_activate(swt, name, flag, xwmp_swt_gc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_swt_activate;
        }
        *ptrbuf = swt;
        return XWOK;

err_swt_activate:
        xwmp_swt_free(swt);
err_swt_alloc:
        return rc;
}

/**
 * @brief XWMP API：删除动态创建的软件定时器对象
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EFAULT: 空指针
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_swt_delete(struct xwmp_swt * swt)
{
        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwmp_swt_stop(swt);
        return xwmp_swt_put(swt);
}

/**
 * @brief 软件定时器的时间树节点回调函数
 * @param entry: (I) 软件定时器对象的指针
 */
static __xwmp_code
void xwmp_swt_ttn_cb(void * entry)
{
        struct xwmp_swt * swt;
        struct xwmp_tt * xwtt;
        xwtm_t expected;
        xwreg_t cpuirq;
        xwer_t rc;

        swt = entry;
        xwtt = &swt->xwskd->tt;
        swt->cb(swt, swt->arg);
        if (XWMP_SWT_FLAG_RESTART & swt->flag) {
                expected = xwtm_add_safely(swt->ttn.wkup_xwtm, swt->period);
                xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
                swt->ttn.wkup_xwtm = expected;
                xwaop_store(xwsq, &swt->ttn.wkuprs,
                            xwmb_modr_release, XWMP_TTN_WKUPRS_UNKNOWN);
                swt->ttn.cb = xwmp_swt_ttn_cb;
                rc = xwmp_tt_add_locked(xwtt, &swt->ttn, cpuirq);
                xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
                XWOS_BUG_ON(rc < 0);
        } else if (XWMP_SWT_FLAG_AUTORM & swt->flag) {
                xwmp_swt_put(swt);
        }/* else {} */
}

/**
 * @brief XWMP API：开启软件定时器
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
__xwmp_api
xwer_t xwmp_swt_start(struct xwmp_swt * swt,
                      xwtm_t base, xwtm_t period,
                      xwmp_swt_f cb, void * arg)
{
        struct xwmp_tt * xwtt;
        xwtm_t expected;
        xwer_t rc;
        xwreg_t cpuirq;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);
        XWOS_VALIDATE((cb), "nullptr", -EFAULT);
        XWOS_VALIDATE(((xwtm_cmp(base, 0) > 0) && (xwtm_cmp(period, 0) > 0)),
                      "out-of-time", -EINVAL);

        xwtt = &swt->xwskd->tt;
        expected = xwtm_add_safely(base, period);
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        if (__xwcc_unlikely(NULL != swt->ttn.cb)) {
                rc = -EALREADY;
                goto err_already;
        }
        swt->cb = cb;
        swt->arg = arg;
        swt->period = period;
        /* 加入时间树 */
        swt->ttn.wkup_xwtm = expected;
        xwaop_store(xwsq, &swt->ttn.wkuprs,
                    xwmb_modr_release, XWMP_TTN_WKUPRS_UNKNOWN);
        swt->ttn.xwtt = xwtt;
        swt->ttn.cb = xwmp_swt_ttn_cb;
        rc = xwmp_tt_add_locked(xwtt, &swt->ttn, cpuirq);

err_already:
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}

/**
 * @brief XWMP API：停止软件定时器
 * @param swt: (I) 软件定时器对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ESRCH: 定时器没有被开启
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：不可重入
 */
__xwmp_api
xwer_t xwmp_swt_stop(struct xwmp_swt * swt)
{
        xwreg_t cpuirq;
        xwer_t rc;
        struct xwmp_tt * xwtt;

        XWOS_VALIDATE((swt), "nullptr", -EFAULT);

        xwtt = &swt->xwskd->tt;
        xwmp_sqlk_wr_lock_cpuirqsv(&xwtt->lock, &cpuirq);
        rc = xwmp_tt_remove_locked(xwtt, &swt->ttn);
        xwmp_sqlk_wr_unlock_cpuirqrs(&xwtt->lock, cpuirq);
        return rc;
}
