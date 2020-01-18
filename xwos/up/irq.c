/**
 * @file
 * @brief XuanWuOS内核：中断控制器
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
#include <xwos/lib/string.h>
#include <xwos/irq.h>
#include <xwos/up/scheduler.h>
#include <xwos/up/irq.h>
#include <soc_up_irqc.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       .data       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwos_data struct xwos_irqc xwos_irqc;

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********      function implementations       ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief XWOS INIT LIB：初始化中断控制器
 * @param name: (I) 名字字符串
 * @param irqs_num: (I) 中断的数量
 * @param soc_cfg: (I) SOC私有配置
 * @param isr_table: (I) 中断向量表
 * @param data_table: (I) 中断数据表
 * @return 错误码
 */
__xwos_init_code
xwer_t xwos_irqc_init(const char * name, xwsz_t irqs_num,
                      __soc_isr_table_qualifier struct soc_isr_table * isr_table,
                      __soc_isr_table_qualifier struct soc_irq_data_table * data_table,
                      const void * soc_cfg)
{
        xwer_t rc;

        xwos_irqc.name = name;
        xwos_irqc.irqs_num = irqs_num;
        xwos_irqc.isr_table = isr_table;
        xwos_irqc.irq_data_table = data_table;
        xwos_irqc.soc_cfg = soc_cfg;
        rc = soc_irqc_init();
        return rc;
}

/**
 * @brief XWOS INIT LIB：设置中断控制器的数据
 * @param data: (I) 数据
 */
__xwos_init_code
void xwos_irqc_set_data(void * data)
{
        xwos_irqc.data = data;
}

/**
 * @brief XWOS IRQC BSP LIB：返回中断控制器的数据
 * @return 数据的首地址指针
 */
__xwos_code
void * xwos_irqc_get_data(void)
{
        return xwos_irqc.data;
}

/**
 * @brief XWOS IRQC BSP LIB：通过描述得到中断资源
 * @param base: (I) 中断资源数组的首地址
 * @param num: (I) 中断资源数组中元素个数
 * @param description: (I) 描述
 * @param ptrbuf: (O) 指向用于返回结果的缓冲区的指针
 * @return 错误码
 * @retval OK: OK
 * @retval <0: 没有找到资源
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
                        return OK;
                }
        }
        *ptrbuf = NULL;
        return -ENODEV;
}

/******** ******** IRQ APIs ******** ********/
/**
 * @brief XWOS API：申请一个中断
 * @param irqn: (I) 中断号
 * @param isr: (I) 中断处理函数
 * @param flag: (I) 中断标志
 * @param data: (I) 中断数据
 * @return 错误码
 * @note
 * - 中断号的枚举通常定义在芯片的头文件中；
 * - SOCCFG_RO_ISRTABLE配置为0时，参数isr无效，中断向量需预先写入vector.c
 *   的向量表中。
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_request(xwirq_t irqn, xwisr_f isr, xwsq_t flag, void * data)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_request_irq(irqn, isr, flag, data);
}

/**
 * @brief XWOS API：释放一个中断
 * @param irqn: (I) 中断号
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_release(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_release_irq(irqn);
}

/**
 * @brief XWOS API：开启一个中断
 * @param irqn: (I) 中断号
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_enable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_enable_irq(irqn);
}

/**
 * @brief XWOS API：关闭一个中断
 * @param irqn: (I) 中断号
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_disable(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_disable_irq(irqn);;
}

/**
 * @brief XWOS API：保存一个中断的开关标志，然后将其关闭
 * @param irqn: (I) 中断号
 * @param flag: (O) 指向用于返回中断的开关标志的缓冲区的指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_save(xwirq_t irqn, xwreg_t * flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_save_irq(irqn, flag);
}

/**
 * @brief XWOS API：恢复一个中断的开关标志
 * @param irqn: (I) 中断号
 * @param flag: (I) 中断的开关标志
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_restore(xwirq_t irqn, xwreg_t flag)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_restore_irq(irqn, flag);
}

/**
 * @brief XWOS API：挂起一个中断标志
 * @param irqn: (I) 中断号
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_pend(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_pend_irq(irqn);
}

/**
 * @brief XWOS API：清除一个中断标志
 * @param irqn: (I) 中断号
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_clear(xwirq_t irqn)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_clear_irq(irqn);
}

/**
 * @brief XWOS API：配置中断
 * @param irqn: (I) 中断号
 * @param cfg: (I) CPU私有的配置结构体指针
 * @return 错误码
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：同一中断号，不可重入
 */
__xwos_api
xwer_t xwos_irq_cfg(xwirq_t irqn, const struct soc_irq_cfg * cfg)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_cfg_irq(irqn, cfg);
}

/**
 * @brief XWOS API：获取中断的配置
 * @param irqn: (I) 中断号
 * @param cfgbuf: (I) 指向缓冲区的指针，此缓冲区用于返回SOC中断配置结构体
 * @return 错误码
 * @retval OK: OK
 * @retval -ERANGE: 无效的IRQ号
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_irq_get_cfg(xwirq_t irqn, struct soc_irq_cfg * cfgbuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_get_cfg_irq(irqn, cfgbuf);
}

/**
 * @brief XWOS API：获取中断的数据
 * @param irqn: (I) 中断号
 * @param databuf: (I) 指向缓冲区的指针，此缓冲区用于返回SOC中断数据结构体
 * @return 错误码
 * @retval OK: OK
 * @retval -ERANGE: 无效的IRQ号
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_irq_get_data(xwirq_t irqn, struct soc_irq_data * databuf)
{
        XWOS_VALIDATE((irqn < (xwirq_t)SOCCFG_IRQ_NUM), "out-of-range", -ERANGE);

        return soc_irqc_get_data_irq(irqn, databuf);
}

/**
 * @brief XWOS API：判断当前的上下文是否为中断上下文，
 *                  如果是，就取得当前中断的中断号。
 * @param irqnbuf: (O) 指向缓冲区的指针，通过此缓冲区返回当前中断号：
 *                     - 返回结果仅当返回值为OK时有效；
 *                     - 此指针可为NULL，表示不需要返回中断号。
 * @return 错误码
 * @retval OK: 当前上下文为中断
 * @retval -EINTHRD: 当前上下文为线程
 * @retval -EINBH: 当前上下文为中断底半部
 * @note
 * - 同步/异步：同步
 * - 上下文：中断、中断底半部、线程
 * - 重入性：可重入
 */
__xwos_api
xwer_t xwos_irq_get_id(xwirq_t * irqnbuf)
{
        xwer_t rc;

        rc = soc_irq_get_id(irqnbuf);
#if defined(XWUPCFG_SD_BH) && (1 == XWUPCFG_SD_BH)
        if (rc < 0) {
                if (xwos_scheduler_tst_in_bh_lc()) {
                        rc = -EINBH;
                } else {
                        rc = -EINTHRD;
                }
        }
#endif /* XWUPCFG_SD_BH */
        return rc;
}
