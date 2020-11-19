/**
 * @file
 * @brief 玄武OS MP内核：MP中断
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <string.h>
#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/ospl/irq.h>
#include <xwos/mp/lock/spinlock.h>
#include <xwos/mp/skd.h>
#include <xwos/mp/irq.h>

/**
 * @brief 玄武OS MP内核中断控制器子系统结构体
 */
struct xwmp_irqc_subsystem {
        struct xwmp_irqc * percpu_irqc[CPUCFG_CPU_NUM];
        struct xwmp_splk percpu_irqcslot_lock;
        union {
                struct {
                        struct xwmp_irq arch[ARCHCFG_IRQ_NUM];
                        struct xwmp_irq cpu[SOCCFG_IRQ_NUM];
                } cfg;
                struct xwmp_irq allcfg[XWOS_IRQ_NUM];
        } irq;
};

/**
 * @brief 玄武OS MP内核中断控制器子系统
 */
__xwmp_data struct xwmp_irqc_subsystem xwmp_irqc_subsystem;

/**
 * @brief XWMP INIT CODE：初始化XWMP中断控制器子系统
 */
__xwmp_init_code
void xwmp_irqc_subsys_init(void)
{
        xwsq_t i;

        for (i = 0; i < CPUCFG_CPU_NUM; i++) {
                xwmp_irqc_subsystem.percpu_irqc[i] = NULL;
        }
        xwmp_splk_init(&xwmp_irqc_subsystem.percpu_irqcslot_lock);

        for (i = 0; i < XWOS_IRQ_NUM; i++) {
                xwaop_store(xwid,
                            &xwmp_irqc_subsystem.irq.allcfg[i].cpuid,
                            xwmb_modr_relaxed,
                            0);
        }
}

/**
 * @brief XWMP API：中断控制器对象的构造函数
 * @param irqc: (I) 中断控制器对象的指针
 */
__xwmp_code
void xwmp_irqc_construct(struct xwmp_irqc * irqc)
{
        xwos_object_construct(&irqc->xwobj, XWOS_OBJ_IRQC);
        irqc->cpuid = XWID_MAX;
}

/**
 * @brief XWMP API：中断控制器对象的析构函数
 * @param irqc: (I) 中断控制器对象的指针
 */
__xwmp_api
void xwmp_irqc_destruct(struct xwmp_irqc * irqc)
{
        xwos_object_destruct(&irqc->xwobj);
}

/**
 * @brief 增加对象的引用计数
 * @param irqc: (I) 中断控制器对象的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_irqc_grab(struct xwmp_irqc * irqc)
{
        return xwos_object_grab(&irqc->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param irqc: (I) 中断控制器对象的指针
 * @return 错误码
 */
__xwmp_code
xwer_t xwmp_irqc_put(struct xwmp_irqc * irqc)
{
        return xwos_object_put(&irqc->xwobj);
}

/**
 * @brief XWMP API：注册中断控制器对象
 * @param irqc: (I) 中断控制器对象的指针
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 */
__xwmp_api
xwer_t xwmp_irqc_register(struct xwmp_irqc * irqc, xwid_t cpuid, xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwreg_t cpuirq;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqc), "nullptr", -EFAULT);
        XWOS_VALIDATE((cpuid < CPUCFG_CPU_NUM), "out-of-range", -ERANGE);

        rc = xwos_object_activate(&irqc->xwobj, gcfunc);
        if (__xwcc_unlikely(rc < 0)) {
                goto err_get_irqc;
        }

        xwmp_rawly_lock_cpuirqsv(&xwmp_irqc_subsystem.percpu_irqcslot_lock, &cpuirq);
        if (xwmp_irqc_subsystem.percpu_irqc[cpuid]) {
                rc = -EADDRINUSE;
                goto err_inused;
        }
        /* probe IRQ controller */
        drv = irqc->drv;
        if ((drv) && (drv->probe)) {
                rc = drv->probe(irqc);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_drv_probe;
                }
        }
        xwmp_irqc_subsystem.percpu_irqc[cpuid] = irqc;
        xwmp_rawly_unlock_cpuirqrs(&xwmp_irqc_subsystem.percpu_irqcslot_lock, cpuirq);

        return XWOK;

err_drv_probe:
err_inused:
        xwmp_rawly_unlock_cpuirqrs(&xwmp_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
err_get_irqc:
        return rc;
}

/**
 * @brief XWMP API：注销中断控制器对象
 * @param irqc: (I) 中断控制器对象的指针
 * @return 错误码
 */
__xwmp_api
xwer_t xwmp_irqc_deregister(struct xwmp_irqc * irqc)
{
        xwer_t rc;
        xwreg_t cpuirq;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqc), "nullptr", -EFAULT);
        XWOS_VALIDATE((irqc->cpuid < CPUCFG_CPU_NUM), "out-of-range", -ERANGE);

        xwmp_rawly_lock_cpuirqsv(&xwmp_irqc_subsystem.percpu_irqcslot_lock, &cpuirq);
        if (irqc != xwmp_irqc_subsystem.percpu_irqc[irqc->cpuid]) {
                rc = -ENODEV;
                goto err_not_reg;
        }

        /* remove IRQ controller */
        drv = irqc->drv;
        if ((drv) && (drv->remove)) {
                rc = drv->remove(irqc);
                if (__xwcc_unlikely(rc < 0)) {
                        goto err_drv_remove;
                }
        }

        xwmp_irqc_subsystem.percpu_irqc[irqc->cpuid] = NULL;
        irqc->cpuid = XWID_MAX;
        xwmp_rawly_unlock_cpuirqrs(&xwmp_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
        xwmp_irqc_put(irqc);

        return XWOK;

err_drv_remove:
err_not_reg:
        xwmp_rawly_unlock_cpuirqrs(&xwmp_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
        return rc;
}

/**
 * @brief XWMP API：获取中断控制器对象
 * @return 中断控制器对象的指针
 * @note
 * + 当使用完毕后，需要调用xwmp_irqc_put(irqc)减少引用计数；
 * + 返回NULL表示获取失败。
 */
__xwmp_api
struct xwmp_irqc * xwmp_irqc_get_lc(void)
{
        struct xwmp_irqc * irqc;
        xwreg_t cpuirq;
        xwid_t cpuid;
        xwer_t rc;

        cpuid = xwmp_skd_get_id();
        xwmp_rawly_lock_cpuirqsv(&xwmp_irqc_subsystem.percpu_irqcslot_lock, &cpuirq);
        irqc = xwmp_irqc_subsystem.percpu_irqc[cpuid];
        if (irqc) {
                rc = xwmp_irqc_grab(irqc);
                if (rc < 0) {
                        irqc = NULL;
                }
        }
        xwmp_rawly_unlock_cpuirqrs(&xwmp_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
        return irqc;
}

/**
 * @brief XWMP API：设置中断控制器的数据
 * @param irqc: (I) 中断控制器对象的指针
 * @param data: (I) 数据
 */
__xwmp_api
void xwmp_irqc_set_data(struct xwmp_irqc * irqc, void * data)
{
        irqc->data = data;
}

/**
 * @brief XWMP API：返回中断控制器的数据
 * @param irqc: (I) 中断控制器对象的指针
 * @return 数据的首地址指针
 */
__xwmp_api
void * xwmp_irqc_get_data(struct xwmp_irqc * irqc)
{
        return irqc->data;
}

/**
 * @brief XWMP API：从中断号获取中断控制器对象
 * @param irqn: (I) 中断号
 * @return 中断控制器结构体的指针
 * @note
 * - irqn可为负数，表示ARCH IRQ
 */
__xwmp_api
struct xwmp_irqc * xwmp_irq_get_irqc(xwirq_t irqn)
{
        struct xwmp_irqc * irqc;
        xwreg_t cpuirq;
        xwid_t cpuid;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "nullptr", NULL);

        xwmp_rawly_lock_cpuirqsv(&xwmp_irqc_subsystem.percpu_irqcslot_lock, &cpuirq);
        cpuid = xwaop_load(xwid, &xwmp_irqc_subsystem.irq.cfg.cpu[irqn].cpuid,
                           xwmb_modr_relaxed);
        irqc = xwmp_irqc_subsystem.percpu_irqc[cpuid];
        xwmp_rawly_unlock_cpuirqrs(&xwmp_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
        return irqc;
}

/**
 * @brief XWMP API：申请中断
 * @param irqn: (I) 中断号
 * @param isr: (I) 中断处理函数
 * @param flag: (I) 中断标志
 * @param data: (I) 中断数据
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 中断号的枚举通常定义在芯片的头文件中；
 * - SOCCFG_RO_ISRTABLE配置为0时，参数isr无效，中断向量需预先写入vector.c
 *   的向量表中。
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_request(xwirq_t irqn, xwisr_f isr, xwsq_t flag, void * data)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwmp_irq_get_irqc(irqn);
        rc = xwmp_irqc_grab(irqc);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                drv = irqc->drv;
                if ((drv) && (drv->request)) {
                        rc = drv->request(irqc, irqn, isr, flag, data);
                } else {
                        rc = -ENOSYS;
                }
                if (__xwcc_unlikely(rc < 0)) {
                        xwmp_irqc_put(irqc);
                }
        }
        return rc;
}

/**
 * @brief XWMP API：释放中断
 * @param irqn: (I) 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_release(xwirq_t irqn)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->release)) {
                rc = drv->release(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        if (__xwcc_likely(XWOK == rc)) {
                xwmp_irqc_put(irqc);
        }
        return rc;
}

/**
 * @brief XWMP API：开启中断
 * @param irqn: (I) 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_enable(xwirq_t irqn)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->enable)) {
                rc = drv->enable(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：关闭中断
 * @param irqn: (I) 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_disable(xwirq_t irqn)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->disable)) {
                rc = drv->disable(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：保存中断的开关标志，然后将其关闭
 * @param irqn: (I) 中断号
 * @param flag: (O) 指向用于返回中断的开关标志的缓冲区的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((flag), "nullptr", -EFAULT);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->save)) {
                rc = drv->save(irqc, irqn, flag);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：恢复中断的开关标志
 * @param irqn: (I) 中断号
 * @param flag: (I) 中断的开关标志
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->restore) {
                rc = drv->restore(irqc, irqn, flag);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：挂起中断标志
 * @param irqn: (I) 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_pend(xwirq_t irqn)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->pend) {
                rc = drv->pend(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：清除中断标志
 * @param irqn: (I) 中断号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_clear(xwirq_t irqn)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->clear)) {
                rc = drv->clear(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：配置中断
 * @param irqn: (I) 中断号
 * @param cfg: (I) CPU私有的配置结构体指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwmp_api
xwer_t xwmp_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((cfg), "nullptr", -EFAULT);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->cfg) {
                rc = drv->cfg(irqc, irqn, cfg);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：获取中断的配置
 * @param irqn: (I) 中断号
 * @param cfgbuf: (I) 指向缓冲区的指针，此缓冲区用于返回SOC中断配置结构体
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((cfgbuf), "nullptr", -EFAULT);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->get_cfg) {
                rc = drv->get_cfg(irqc, irqn, cfgbuf);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：获取中断的数据
 * @param irqn: (I) 中断号
 * @param databuf: (I) 指向缓冲区的指针，此缓冲区用于返回SOC中断数据结构体
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ERANGE: 中断号超出范围
 * @retval -ENOSYS: 没有实现此功能
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        xwer_t rc;
        struct xwmp_irqc * irqc;
        const struct xwmp_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        irqc = xwmp_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->get_data) {
                rc = drv->get_data(irqc, irqn, databuf);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

/**
 * @brief XWMP API：判断当前的上下文是否为中断上下文，并取得当前中断的中断号
 * @param irqnbuf: (O) 指向缓冲区的指针，通过此缓冲区返回当前中断号：
 *                     - 返回结果仅当返回值为OK时有效
 *                     - 可为NULL，表示不需要返回中断号
 * @return 错误码
 * @retval OK: 当前上下文为中断
 * @retval -EINTHRD: 当前上下文为线程
 * @retval -EINBH: 当前上下文为中断底半部
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwmp_api
xwer_t xwmp_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = xwospl_irq_get_id(irqnbuf);
        if (rc < 0) {
#if defined(XWMPCFG_SKD_BH) && (1 == XWMPCFG_SKD_BH)
                if (xwmp_skd_tst_in_bh_lc()) {
                        rc = -EINBH;
                } else {
                        rc = -EINTHRD;
                }
#else
                rc = -EINTHRD;
#endif /* XWMPCFG_SKD_BH */
        }
        return rc;
}

/**
 * @brief XWMP API：开启本地CPU的中断
 */
__xwmp_api
void xwmp_cpuirq_enable_lc(void)
{
        xwospl_cpuirq_enable_lc();
}

/**
 * @brief XWMP API：关闭本地CPU的中断
 */
__xwmp_api
void xwmp_cpuirq_disable_lc(void)
{
        xwospl_cpuirq_disable_lc();
}

/**
 * @brief XWMP API：恢复本地CPU的中断
 * @param flag: (I) 本地CPU的中断开关标志
 */
__xwmp_api
void xwmp_cpuirq_restore_lc(xwreg_t cpuirq)
{
        xwospl_cpuirq_restore_lc(cpuirq);
}

/**
 * @brief XWMP API：保存然后关闭本地CPU的中断
 * @param flag: (O) 指向缓冲区的指针，此缓冲区用于返回本地CPU的中断开关标志
 */
__xwmp_api
void xwmp_cpuirq_save_lc(xwreg_t * cpuirq)
{
        xwospl_cpuirq_save_lc(cpuirq);
}
