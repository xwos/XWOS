/**
 * @file
 * @brief XuanWuOS内核：SMP中断
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
#include <string.h>
#include <xwos/object.h>
#include <xwos/irq.h>
#include <xwos/smp/cpu.h>
#include <xwos/smp/lock/spinlock.h>
#include <xwos/smp/scheduler.h>
#include <xwos/smp/irq.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwos_irqc_subsystem {
        struct xwos_irqc * percpu_irqc[CPUCFG_CPU_NUM];
        struct xwlk_splk percpu_irqcslot_lock;
        union {
                struct {
                        struct xwos_irq arch[ARCHCFG_IRQ_NUM];
                        struct xwos_irq cpu[SOCCFG_IRQ_NUM];
                } cfg;
                struct xwos_irq allcfg[XWOS_IRQ_NUM];
        } irq;
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_data struct xwos_irqc_subsystem xwos_irqc_subsystem;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********     static function prototypes      ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS INIT API：初始化XWOS中断控制器子系统
 */
__xwos_init_code
void xwos_irqc_subsys_init(void)
{
        xwsq_t i;

        for (i = 0; i < CPUCFG_CPU_NUM; i++) {
                xwos_irqc_subsystem.percpu_irqc[i] = NULL;
        }
        xwlk_splk_init(&xwos_irqc_subsystem.percpu_irqcslot_lock);

        for (i = 0; i < XWOS_IRQ_NUM; i++) {
                xwaop_store(xwid_t,
                            &xwos_irqc_subsystem.irq.allcfg[i].cpuid,
                            xwmb_modr_relaxed,
                            0);
        }
}

/**
 * @brief XWOS INIT API：中断控制器对象的构造函数
 * @param irqc: (I) 中断控制器对象的指针
 */
__xwos_init_code
void xwos_irqc_construct(struct xwos_irqc * irqc)
{
        xwos_object_construct(&irqc->xwobj);
        irqc->cpuid = XWID_MAX;
}

/**
 * @brief XWOS EXIT API：中断控制器对象的析构函数
 * @param irqc: (I) 中断控制器对象的指针
 */
__xwos_exit_code
void xwos_irqc_destruct(struct xwos_irqc * irqc)
{
        xwos_object_destruct(&irqc->xwobj);
}

/**
 * @brief XWOS INIT API：设置中断控制器的数据
 * @param irqc: (I) 中断控制器对象的指针
 * @param data: (I) 数据
 */
__xwos_init_code
void xwos_irqc_set_data(struct xwos_irqc * irqc, void * data)
{
        irqc->data = data;
}

/**
 * @brief XWOS IRQC BSP LIB：返回中断控制器的数据
 * @param irqc: (I) 中断控制器对象的指针
 * @return 数据的首地址指针
 */
__xwos_code
void * xwos_irqc_get_data(struct xwos_irqc * irqc)
{
        return irqc->data;
}

/**
 * @brief XWOS IRQC BSP LIB：注册中断控制器对象
 * @param irqc: (I) 中断控制器对象的指针
 * @param gcfunc: (I) 垃圾回收函数的指针
 * @return 错误码
 */
__xwos_init_code
xwer_t xwos_irqc_register(struct xwos_irqc * irqc, xwid_t cpuid, xwobj_gc_f gcfunc)
{
        xwer_t rc;
        xwreg_t cpuirq;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqc), "nullptr", -EFAULT);
        XWOS_VALIDATE((cpuid < CPUCFG_CPU_NUM), "out-of-range", -ERANGE);

        rc = xwos_object_activate(&irqc->xwobj, gcfunc);
        if (__unlikely(rc < 0)) {
                goto err_get_irqc;
        }

        xwlk_rawly_lock_cpuirqsv(&xwos_irqc_subsystem.percpu_irqcslot_lock, &cpuirq);
        if (xwos_irqc_subsystem.percpu_irqc[cpuid]) {
                rc = -EADDRINUSE;
                goto err_inused;
        }
        /* probe IRQ controller */
        drv = irqc->drv;
        if ((drv) && (drv->probe)) {
                rc = drv->probe(irqc);
                if (__unlikely(rc < 0)) {
                        goto err_drv_probe;
                }
        }
        xwos_irqc_subsystem.percpu_irqc[cpuid] = irqc;
        xwlk_rawly_unlock_cpuirqrs(&xwos_irqc_subsystem.percpu_irqcslot_lock, cpuirq);

        return XWOK;

err_drv_probe:
err_inused:
        xwlk_rawly_unlock_cpuirqrs(&xwos_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
err_get_irqc:
        return rc;
}

/**
 * @brief XWOS IRQC BSP LIB：注销中断控制器对象
 * @param irqc: (I) 中断控制器对象的指针
 * @return 错误码
 */
__xwos_exit_code
xwer_t xwos_irqc_deregister(struct xwos_irqc * irqc)
{
        xwer_t rc;
        xwreg_t cpuirq;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqc), "nullptr", -EFAULT);
        XWOS_VALIDATE((irqc->cpuid < CPUCFG_CPU_NUM), "out-of-range", -ERANGE);

        xwlk_rawly_lock_cpuirqsv(&xwos_irqc_subsystem.percpu_irqcslot_lock, &cpuirq);
        if (irqc != xwos_irqc_subsystem.percpu_irqc[irqc->cpuid]) {
                rc = -ENODEV;
                goto err_not_reg;
        }

        /* remove IRQ controller */
        drv = irqc->drv;
        if ((drv) && (drv->remove)) {
                rc = drv->remove(irqc);
                if (__unlikely(rc < 0)) {
                        goto err_drv_remove;
                }
        }

        xwos_irqc_subsystem.percpu_irqc[irqc->cpuid] = NULL;
        irqc->cpuid = XWID_MAX;
        xwlk_rawly_unlock_cpuirqrs(&xwos_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
        xwos_irqc_put(irqc);

        return XWOK;

err_drv_remove:
err_not_reg:
        xwlk_rawly_unlock_cpuirqrs(&xwos_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
        return rc;
}

/**
 * @brief XWOS IRQC BSP LIB：从中断号获取中断控制器对象
 * @param irqn: (I) 中断号
 * @return 中断控制器结构体的指针
 * @note
 * - irqn可为负数，表示ARCH IRQ
 */
__xwos_code
struct xwos_irqc * xwos_irq_get_irqc(xwirq_t irqn)
{
        struct xwos_irqc * irqc;
        xwreg_t cpuirq;
        xwid_t cpuid;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "nullptr", NULL);

        xwlk_rawly_lock_cpuirqsv(&xwos_irqc_subsystem.percpu_irqcslot_lock, &cpuirq);
        cpuid = xwaop_load(xwid_t, &xwos_irqc_subsystem.irq.cfg.cpu[irqn].cpuid,
                           xwmb_modr_relaxed);
        irqc = xwos_irqc_subsystem.percpu_irqc[cpuid];
        xwlk_rawly_unlock_cpuirqrs(&xwos_irqc_subsystem.percpu_irqcslot_lock, cpuirq);
        return irqc;
}

/**
 * @brief XWOS IRQC BSP LIB：通过描述得到中断资源
 * @param base: (I) 中断资源数组的首地址
 * @param num: (I) 中断资源数组中元素个数
 * @param description: (I) 描述
 * @param ptrbuf: (O) 指向用于返回结果的缓冲区的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -ENODEV: 没有找到资源
 */
__xwos_code
xwer_t xwos_irqc_get_irqrsc(const struct xwos_irq_resource base[], xwsz_t num,
                            const char * description,
                            const struct xwos_irq_resource ** ptrbuf)
{
        xwsz_t i;
        for (i = 0; i < num; i++) {
                if (!strcmp(base[i].description, description)) {
                        *ptrbuf = &base[i];
                        return XWOK;
                }
        }
        *ptrbuf = NULL;
        return -ENODEV;
}

/******** ******** IRQ APIs ******** ********/
__xwos_api
xwer_t xwos_irq_request(xwirq_t irqn, xwisr_f isr, xwsq_t flag, void * data)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwos_irq_get_irqc(irqn);
        rc = xwos_irqc_grab(irqc);
        if (__unlikely(rc < 0)) {
                rc = -EPERM;
        } else {
                drv = irqc->drv;
                if ((drv) && (drv->request)) {
                        rc = drv->request(irqc, irqn, isr, flag, data);
                } else {
                        rc = -ENOSYS;
                }
                if (__unlikely(rc < 0)) {
                        xwos_irqc_put(irqc);
                }
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_release(xwirq_t irqn)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->release)) {
                rc = drv->release(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        if (__likely(XWOK == rc)) {
                xwos_irqc_put(irqc);
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_enable(xwirq_t irqn)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->enable)) {
                rc = drv->enable(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_disable(xwirq_t irqn)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->disable)) {
                rc = drv->disable(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((flag), "nullptr", -EFAULT);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->save)) {
                rc = drv->save(irqc, irqn, flag);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->restore) {
                rc = drv->restore(irqc, irqn, flag);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_pend(xwirq_t irqn)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->pend) {
                rc = drv->pend(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_clear(xwirq_t irqn)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if ((drv) && (drv->clear)) {
                rc = drv->clear(irqc, irqn);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((cfg), "nullptr", -EFAULT);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->cfg) {
                rc = drv->cfg(irqc, irqn, cfg);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((cfgbuf), "nullptr", -EFAULT);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->get_cfg) {
                rc = drv->get_cfg(irqc, irqn, cfgbuf);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        xwer_t rc;
        struct xwos_irqc * irqc;
        const struct xwos_irqc_driver * drv;

        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);
        XWOS_VALIDATE((databuf), "nullptr", -EFAULT);

        irqc = xwos_irq_get_irqc(irqn);
        drv = irqc->drv;
        if (drv && drv->get_data) {
                rc = drv->get_data(irqc, irqn, databuf);
        } else {
                rc = -ENOSYS;
        }
        return rc;
}

__xwos_api
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = soc_irq_get_id(irqnbuf);
#if defined(XWSMPCFG_SD_BH) && (1 == XWSMPCFG_SD_BH)
        if (rc < 0) {
                if (xwos_scheduler_tst_in_bh_lc()) {
                        rc = -EINBH;
                } else {
                        rc = -EINTHRD;
                }
        }
#endif /* XWSMPCFG_SD_BH */
        return rc;
}
