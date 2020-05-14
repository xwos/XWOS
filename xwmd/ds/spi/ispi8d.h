/**
 * @file
 * @brief xwmd设备栈：inter-SPI-8-bit从机通讯协议设备
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - protocol:
 * -- 1st byte: sync message, 0xAA
 * -- 2nd byte: communication data size (bit0 ~ bit5), + parity (bit6 ~ bit7)
 *              The parity is same as LIN protect ID
 * -- 3th ~ ...: signification data
 */

#ifndef __xwmd_ds_spi_ispi8d_h__
#define __xwmd_ds_spi_ispi8d_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwbop.h>
#include <xwos/osal/irq.h>
#include <xwos/osal/lock/spinlock.h>
#include <xwos/osal/sync/semaphore.h>
#include <xwos/osal/sync/condition.h>
#include <xwmd/ds/device.h>
#include <xwmd/ds/spi/common.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief iSPI8设备的时钟极性枚举
 */
enum xwds_ispi8d_cpol_em {
        SODS_ISPI8D_CPOL_LOW = 0,
        SODS_ISPI8D_CPOL_HIGH,
};

/**
 * @brief iSPI8设备的时钟相位枚举
 */
enum xwds_ispi8d_cpha_em {
        SODS_ISPI8D_CPHA_LCTS = 0, /**< 起始沿采集，结束沿设置 */
        SODS_ISPI8D_CPHA_LSTC, /**< 起始沿设置，结束沿采集 */
};

/**
 * @brief iSPI8设备的数据的顺序枚举
 */
enum xwds_ispi8d_frmodr_em {
        SODS_ISPI8D_FO_MSB_FIRST = 0, /**< MSB优先 */
        SODS_ISPI8D_FO_LSB_FIRST, /**< LSB优先 */
};

/**
 * @brief iSPI8设备的状态机枚举
 */
enum xwds_ispi8d_state_em {
        SODS_ISPI8D_STATE_IDLE, /**< 空闲 */
        SODS_ISPI8D_STATE_SYNC, /**< 已经同步 */
        SODS_ISPI8D_STATE_XDATA, /**< 正在传输数据 */
};

/**
 * @brief iSPI8设备的数据状态枚举
 */
enum xwds_ispi8d_data_state_em {
        SODS_ISPI8D_DATA_NON_AVAILABLE,
        SODS_ISPI8D_DATA_AVAILABLE,
};

/**
 * @brief iSPI8设备的片选模式枚举
 */
enum xwds_ispi8d_cs_mode_em {
        SODS_ISPI8D_CS_HWMODE = 0,
        SODS_ISPI8D_CS_SWMODE = BIT(0),
        SODS_ISPI8D_CS_INACTIVE_LOW = 0,
        SODS_ISPI8D_CS_INACTIVE_HIGH = BIT(1),
};

/**
 * @brief iSPI8设备的片选信号枚举
 */
enum xwds_ispi8d_cs_em {
        SODS_ISPI8D_CS_ACTIVE = 0,
        SODS_ISPI8D_CS_INACTIVE,
};

/**
 * @brief iSPI8设备的通知PIN信号枚举
 */
enum xwds_ispi8d_ntf_mode_em {
        SODS_ISPI8D_NTFM_LOW = 0,
        SODS_ISPI8D_NTFM_HIGH,

};

/**
 * @breif iSPI8设备的配置
 */
struct xwds_ispi8d_cfg {
        union {
                struct {
                        xwu32_t wire:1; /**< 3/4线模式 */
                        xwu32_t direction:2; /**< 方向 */
                        xwu32_t cpol:1; /**< 时钟极性 */
                        xwu32_t cpha:1; /**< 时钟相位 */
                        xwu32_t frmodr:1; /**< 是否LSB优先 */
                        xwu32_t csmode:2; /**< 片选模式 */
                        xwu32_t ntfmode:1; /**< 通知信号PIN模式 */
                } bit; /**< bit access */
                xwu32_t u32; /**< word access */
        } bus;
        void * xwccfg; /**< SOC芯片的私有配置 */
};

struct xwds_ispi8d;

/**
 * @breif BSP中需要提供的iSPI8设备的驱动函数表
 */
struct xwds_ispi8d_driver {
        struct xwds_driver base; /**< C语言面向对象：继承struct xwds_driver */
        xwer_t (* notify)(struct xwds_ispi8d *, xwsq_t); /**< 通知master启动传输 */
        xwer_t (* swcs)(struct xwds_ispi8d *, xwu8_t, xwtm_t *); /**< 软件片选 */
};

/**
 * @brief iSPI8设备的发送槽
 */
struct xwds_ispi8d_txslot {
        xwu8_t size; /**< 发送大小 */
        xwsq_t pos; /**< 待发送的字节位置 */
        xwu8_t * data; /**< 数据缓冲区的指针 */
        volatile xwer_t rc; /**< 发送结果 */
        void (*complete)(struct xwds_ispi8d_txslot *); /**< 完成回调函数 */
        struct xwosal_cdt cdt; /**< 条件量 */
};

/**
 * @brief iSPI8设备的接收槽
 */
struct xwds_ispi8d_rxslot {
        xwu8_t buf[XWMDCFG_ds_SPI_ISPI8C_MAX_COMM_BYTES]; /* 缓冲区 */
        xwsq_t pos; /**< 接收字节位置 */
        xwsz_t size; /* 有效自己的数量 */
};

/**
 * @breif iSPI8设备
 */
struct xwds_ispi8d {
        struct xwds_device dev; /**< 继承struct xwds_device */

        /* attributes */
        const struct xwds_ispi8d_cfg * cfg; /**< 配置 */

        /* private */
        struct {
                struct xwosal_splk lock; /**< 通讯状态机的锁 */
                xwsq_t state; /**< 通讯状态 */
                struct xwds_ispi8d_txslot * ctxslot; /**< 当前发送槽 */
                struct xwds_ispi8d_txslot * ntxslot; /**< 下一个发送槽 */
                struct xwds_ispi8d_rxslot * crxslot; /**< 当前的接收槽 */
        } comi; /* 通讯状态机 */
        struct {
                struct xwosal_smr smr; /**< 可用数据信号量 */
                struct xwosal_splk lock; /**< 保护队列的锁 */
                struct xwds_ispi8d_rxslot slot[XWMDCFG_ds_SPI_ISPI8C_RXQ_SIZE];
                xwssq_t pos; /**< 当前接收位置 */
                xwssz_t num; /**< 队列中有效数据的数量 */
        } rxq; /* 接收队列 */
        struct xwosal_mtx txmtx; /**< 提供多线程访问安全的锁 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_api
void xwds_ispi8d_construct(struct xwds_ispi8d * ispi8d);

__xwds_api
void xwds_ispi8d_destruct(struct xwds_ispi8d * ispi8d);

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_ispi8d_clear_rxq(struct xwds_ispi8d * ispi8d);

__xwds_api
xwer_t xwds_ispi8d_rx(struct xwds_ispi8d * ispi8d, xwu8_t * buf, xwsz_t * size,
                      xwtm_t * xwtm);

__xwds_api
xwer_t xwds_ispi8d_try_rx(struct xwds_ispi8d * ispi8d, xwu8_t * buf, xwsz_t * size);

__xwds_api
xwer_t xwds_ispi8d_tx(struct xwds_ispi8d * ispi8d, const xwu8_t * data, xwsz_t size,
                      xwtm_t * xwtm);

__xwds_api
xwer_t xwds_ispi8d_swcs(struct xwds_ispi8d * ispi8d, xwu8_t cs, xwtm_t * xwtm);

/******** ******** Libraries for BSP driver ******** ********/
__xwds_lib_code
xwer_t xwds_ispi8d_lib_sync(struct xwds_ispi8d * ispi8d, xwu8_t input,
                            struct xwds_ispi8d_txslot ** txslotbuf);

__xwds_lib_code
xwer_t xwds_ispi8d_lib_swapsize(struct xwds_ispi8d * ispi8d, xwu8_t input,
                                struct xwds_ispi8d_txslot ** txslotbuf);

__xwds_lib_code
xwer_t xwds_ispi8d_lib_swapdata(struct xwds_ispi8d * ispi8d,
                                xwu8_t * rxdata, xwsz_t rxsize,
                                struct xwds_ispi8d_txslot ** txslotbuf);


/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_grab()
 */
static __xwds_inline
xwer_t xwds_ispi8d_grab(struct xwds_ispi8d * ispi8d)
{
        return xwds_device_grab(&ispi8d->dev);
}

/**
 * @brief 减少对象的引用计数
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_put()
 */
static __xwds_inline
xwer_t xwds_ispi8d_put(struct xwds_ispi8d * ispi8d)
{
        return xwds_device_put(&ispi8d->dev);
}

/**
 * @brief 增加设备运行状态计数器
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_request()
 */
static __xwds_inline
xwer_t xwds_ispi8d_request(struct xwds_ispi8d * ispi8d)
{
        return xwds_device_request(&ispi8d->dev);
}

/**
 * @brief 减少设备运行状态计数器
 * @param ispi8d: (I) iSPI8设备对象指针
 * @return 错误码
 * @retval @ref xwds_device_release()
 */
static __xwds_inline
xwer_t xwds_ispi8d_release(struct xwds_ispi8d * ispi8d)
{
        return xwds_device_release(&ispi8d->dev);
}

#endif /* xwmd/ds/spi/ispi8d.h */
