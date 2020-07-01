/**
 * @file
 * @brief xwmd设备栈：SOC设备
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_soc_chip_h__
#define __xwmd_ds_soc_chip_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwmd/ds/device.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#if defined(XWMDCFG_ds_SOC_EIRQ_ROISRT) && (1 == XWMDCFG_ds_SOC_EIRQ_ROISRT)
  #define __xwds_soc_eirq_tbl_qualifier const
#else
  #define __xwds_soc_eirq_tbl_qualifier
#endif

#if defined(XWMDCFG_ds_SOC_DMA_ROCBT) && (1 == XWMDCFG_ds_SOC_DMA_ROCBT)
  #define __xwds_soc_dma_tbl_qualifier const
#else
  #define __xwds_soc_dma_tbl_qualifier
#endif

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwds_soc;
struct xwds_dma_cfg;

/**
 * @brief BSP中需要提供的SOC设备驱动函数表
 */
struct xwds_soc_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
#if defined(XWMDCFG_ds_SOC_CLK) && (1 == XWMDCFG_ds_SOC_CLK)
        xwer_t (* clk_req)(struct xwds_soc * /*soc*/,
                           xwid_t /*id*/); /**< 申请时钟 */
        xwer_t (* clk_rls)(struct xwds_soc * /*soc*/,
                           xwid_t /*id*/); /**< 释放时钟 */
        xwer_t (* clk_getfqcy)(struct xwds_soc * /*soc*/,
                               xwid_t /*id*/,
                               xwu32_t * /*buf*/,
                               xwsz_t * /*num*/); /**< 获取时钟频率 */
#endif /* XWMDCFG_ds_SOC_CLK */

#if defined(XWMDCFG_ds_SOC_PWR) && (1 == XWMDCFG_ds_SOC_PWR)
        xwer_t (* pwr_req)(struct xwds_soc * /*soc*/,
                           xwid_t /*id*/); /**< 申请电源 */
        xwer_t (* pwr_rls)(struct xwds_soc * /*soc*/,
                           xwid_t /*id*/); /**< 释放电源 */
        xwer_t (* pwr_getvltg)(struct xwds_soc * /*soc*/,
                               xwid_t /*id*/,
                               xwu32_t * /*buf*/,
                               xwsz_t * /*num*/); /**< 获取电源电压 */
#endif /* XWMDCFG_ds_SOC_PWR */

#if defined(XWMDCFG_ds_SOC_GPIO) && (1 == XWMDCFG_ds_SOC_GPIO)
        xwer_t (* gpio_req)(struct xwds_soc * /*soc*/,
                            xwid_t /*port*/,
                            xwsq_t /*pinmask*/); /**< 申请GPIO */
        xwer_t (* gpio_rls)(struct xwds_soc * /*soc*/,
                            xwid_t /*port*/,
                            xwid_t /*pinmask*/); /**< 释放GPIO */
        xwer_t (* gpio_cfg)(struct xwds_soc * /*soc*/,
                            xwid_t /*port*/,
                            xwsq_t /*pinmask*/,
                            void * /*cfg*/); /**< 配置GPIO */
        xwer_t (* gpio_output)(struct xwds_soc *,
                               xwid_t /*port*/,
                               xwsq_t /*pinmask*/,
                               xwsq_t); /**< 输出GPIO */
        xwer_t (* gpio_set)(struct xwds_soc * /*soc*/,
                            xwid_t /*port*/,
                            xwsq_t /*pinmask*/); /**< 将GPIO置为高电平 */
        xwer_t (* gpio_reset)(struct xwds_soc * /*soc*/,
                              xwid_t /*port*/,
                              xwsq_t /*pinmask*/); /**< 将GPIO置为低电平 */
        xwer_t (* gpio_toggle)(struct xwds_soc * /*soc*/,
                               xwid_t /*port*/,
                               xwsq_t /*pinmask*/); /**< 反转GPIO电平*/
        xwer_t (* gpio_input)(struct xwds_soc * /*soc*/,
                              xwid_t /*port*/,
                              xwsq_t /*pinmask*/,
                              xwsq_t * /*inbuf*/); /**< 读取GPIO上的输入 */
#endif /* XWMDCFG_ds_SOC_GPIO */

#if defined(XWMDCFG_ds_SOC_EIRQ) && (1 == XWMDCFG_ds_SOC_EIRQ)
        xwer_t (* eirq_req)(struct xwds_soc * /*soc*/,
                            xwid_t /*port*/, xwsq_t /*pinmask*/,
                            xwid_t /*eiid*/, xwsq_t /*eiflag*/); /**< 申请外部中断 */
        xwer_t (* eirq_rls)(struct xwds_soc * /*soc*/,
                            xwid_t /*port*/, xwsq_t /*pinmask*/,
                            xwid_t /*eiid*/); /**< 释放外部中断 */
#endif /* XWMDCFG_ds_SOC_EIRQ */

#if defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)
        xwer_t (* dma_req)(struct xwds_soc * /*soc*/,
                           xwid_t /*ch*/); /**< 申请DMA */
        xwer_t (* dma_rls)(struct xwds_soc * /*soc*/,
                           xwid_t /*ch*/); /**< 释放DMA */
        xwer_t (* dma_cfg)(struct xwds_soc * /*soc*/,
                           xwid_t /*ch*/,
                           void * /*cfg*/); /**< 配置DMA */
        xwer_t (* dma_enable)(struct xwds_soc * /*soc*/,
                              xwid_t /*ch*/); /**< 打开DMA传输 */
        xwer_t (* dma_disable)(struct xwds_soc * /*soc*/,
                               xwid_t /*ch*/); /**< 关闭DMA传输 */
        xwer_t (* dma_start)(struct xwds_soc * /*soc*/,
                             xwid_t /*ch*/); /**< 启动DMA传输 */
        xwer_t (* dma_stop)(struct xwds_soc * /*soc*/,
                            xwid_t /*ch*/); /**< 停止DMA传输 */
#endif /* XWMDCFG_ds_SOC_DMA */

#if defined(XWMDCFG_ds_SOC_ERAM) && (1 == XWMDCFG_ds_SOC_ERAM)
        xwer_t (* eram_tst)(struct xwds_soc * /*soc*/,
                            xwptr_t * /*erraddr*/); /**< 测试ERAM */
#endif /* XWMDCFG_ds_SOC_ERAM */

        xwer_t (* ioctl)(struct xwds_soc * /*soc*/,
                         xwsq_t /*cmd*/,
                         va_list /*varg*/); /**< 输入、输出和控制 */
};

/**
 * @brief 外部中断函数参数
 */
typedef void * xwds_eirq_arg_t;

/**
 * @brief 外部中断函数
 */
typedef void (*xwds_eirq_f)(struct xwds_soc *, xwid_t, xwds_eirq_arg_t);

/**
 * @brief DMA完成回调函数参数
 */
typedef void * xwds_dma_cbarg_t;

/**
 * @brief DMA完成回调函数类型
 */
typedef void (*xwds_dma_f)(struct xwds_soc * /* soc */,
                           xwid_t /* ch */,
                           xwu32_t /* rc */,
                           xwds_dma_cbarg_t /* arg */);

/**
 * @brief SOC设备
 */
struct xwds_soc {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const void * xwccfg; /**< SOC芯片私有配置 */
#if (defined(XWMDCFG_ds_SOC_GPIO) && (1 == XWMDCFG_ds_SOC_GPIO))
        struct {
                void * cfg; /**< GPIO配置 */
                __atomic xwsq_t * pins; /**< GPIO PIN状态图数组 */
                xwsz_t port_num; /**< GPIO端口数量 */
                xwsz_t pin_num; /**< 每个GPIO端口中的PIN数量 */
        } gpio; /**< GPIO描述 */
#endif /* #if (defined(XWMDCFG_ds_SOC_GPIO) && (1 == XWMDCFG_ds_SOC_GPIO)) */
#if (defined(XWMDCFG_ds_SOC_CLK) && (1 == XWMDCFG_ds_SOC_CLK))
        struct {
                xwsz_t num; /**< 时钟数量 */
        } clk; /**< 时钟描述 */
#endif /* #if (defined(XWMDCFG_ds_SOC_CLK) && (1 == XWMDCFG_ds_SOC_CLK)) */
#if (defined(XWMDCFG_ds_SOC_PWR) && (1 == XWMDCFG_ds_SOC_PWR))
        struct {
                xwsz_t num; /**< 电源数量 */
        } pwr; /**< 电源描述 */
#endif /* #if (defined(XWMDCFG_ds_SOC_PWR) && (1 == XWMDCFG_ds_SOC_PWR)) */
#if (defined(XWMDCFG_ds_SOC_EIRQ) && (1 == XWMDCFG_ds_SOC_EIRQ))
        struct {
                __xwds_soc_eirq_tbl_qualifier xwds_eirq_f * isrs;
                                                        /**< 外部中断的ISR表 */
                __xwds_soc_eirq_tbl_qualifier xwds_eirq_arg_t * isrargs;
                                                        /**< 外部中断的参数表 */
                xwsz_t num; /**< 外部中断的数量 */
        } eirq; /**< 外部中断的描述 */
#endif /* #if (defined(XWMDCFG_ds_SOC_EIRQ) && (1 == XWMDCFG_ds_SOC_EIRQ)) */
#if (defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA))
        struct {
                xwsz_t ch_num; /**< DMA通道数量 */
                void * ccfg; /**< SOC DMA控制器配置 */
                __atomic xwbmp_t * chstatus; /**< DMA通道状态图 */
                __xwds_soc_dma_tbl_qualifier xwds_dma_f * chcbs;
                                                /**< DMA通道传输结束回调函数表 */
                __xwds_soc_dma_tbl_qualifier xwds_dma_cbarg_t * chcbargs;
                                                /**< DMA通道传输结束参数表 */
        } dma; /**< DMA描述 */
#endif /* #if (defined(XWMDCFG_ds_SOC_DMA) && (1 == XWMDCFG_ds_SOC_DMA)) */

#if defined(XWMDCFG_ds_SOC_ERAM) && (1 == XWMDCFG_ds_SOC_ERAM)
        struct {
                xwptr_t origin; /**< 基地址 */
                xwsz_t size; /**< 大小 */
        } eram;
#endif /* XWMDCFG_ds_SOC_ERAM */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_soc_construct(struct xwds_soc * soc);

__xwds_api
void xwds_soc_destruct(struct xwds_soc * soc);

/******** ******** ******** APIs ******** ******** ********/
/**
 * @brief XWDS API：SOC输入、输出、控制
 * @param soc: (I) SOC对象指针
 * @param cmd: (I) 命令
 * @param ...: (I) 参数表
 * @return 错误码
 * @note
 * - 同步/异步：依赖于CMD的实现
 * - 上下文：依赖于CMD的实现
 * - 重入性：依赖于CMD的实现
 */
__xwds_api
xwer_t xwds_soc_ioctl(struct xwds_soc * soc, xwsq_t cmd, ...);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_soc_grab(struct xwds_soc * soc)
{
        return xwds_device_grab(&soc->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_soc_put(struct xwds_soc * soc)
{
        return xwds_device_put(&soc->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_soc_request(struct xwds_soc * soc)
{
        return xwds_device_request(&soc->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param soc: (I) SOC对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_soc_release(struct xwds_soc * soc)
{
        return xwds_device_release(&soc->dev);
}

#endif /* xwmd/ds/soc/chip.h */
