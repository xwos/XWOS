/**
 * @file
 * @brief xwmd设备栈：设备基类
 * @author
 * + 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <www.starsoul.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 * @note
 * - 命名规则：
 *   + operations: 应用层可以调用的方法，“操作”设备；
 *   + driver: BSP中需要实现的驱动；
 *   + callback: 底层/服务对上层/客户端的回调。
 */

#ifndef __xwmd_ds_device_h__
#define __xwmd_ds_device_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/mutex.h>
#include <xwmd/ds/object.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief Iterate over all devices forward in a device stack
 */
#define xwds_itr_next_device(ds, p) \
        xwlib_bclst_itr_next_entry(p, &(ds)->devhead,  \
                                   struct xwds_device, obj.node)

/**
 * @brief Iterate over all devices backward in a device stack
 */
#define xwds_itr_prev_device(ds, p) \
        xwlib_bclst_itr_prev_entry(p, &(ds)->devhead,  \
                                   struct xwds_device, obj.node)

/**
 * @brief Iterate over all devices forward in a device stack safe against removal
 */
#define xwds_itr_next_device_safe(ds, p, n) \
        xwlib_bclst_itr_next_entry_safe(p, n, &(ds)->devhead,  \
                                        struct xwds_device, obj.node)

/**
 * @brief Iterate over all devices backward in a device stack safe against removal
 */
#define xwds_itr_prev_device_safe(ds, p, n) \
        xwlib_bclst_itr_prev_entry_safe(p, n, &(ds)->devhead,  \
                                        struct xwds_device, obj.node)

#define __IOC_NRBITS            8
#define __IOC_TYPEBITS          8
#define __IOC_SIZEBITS          14
#define __IOC_DIRBITS           2

#define __IOC_NRMASK            ((1 << __IOC_NRBITS) - 1)
#define __IOC_TYPEMASK          ((1 << __IOC_TYPEBITS) - 1)
#define __IOC_SIZEMASK          ((1 << __IOC_SIZEBITS) - 1)
#define __IOC_DIRMASK           ((1 << __IOC_DIRBITS) - 1)

#define __IOC_NRSHIFT           0
#define __IOC_TYPESHIFT         (__IOC_NRSHIFT + __IOC_NRBITS)
#define __IOC_SIZESHIFT         (__IOC_TYPESHIFT + __IOC_TYPEBITS)
#define __IOC_DIRSHIFT          (__IOC_SIZESHIFT + __IOC_SIZEBITS)

#define __IOC_CTRL              0U
#define __IOC_WRITE             1U
#define __IOC_READ              2U

#define __IOC(dir, type, nr, size)      \
        (((dir)  << __IOC_DIRSHIFT) |   \
         ((type) << __IOC_TYPESHIFT) |  \
         ((nr)   << __IOC_NRSHIFT) |    \
         ((size) << __IOC_SIZESHIFT))

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 设备状态枚举
 */
enum xwds_device_state_em {
        XWDS_DEVICE_STATE_INVALID = 0,
        XWDS_DEVICE_STATE_REMOVING,
        XWDS_DEVICE_STATE_PROBING,
        XWDS_DEVICE_STATE_STOPING,
        XWDS_DEVICE_STATE_STOPED,
        XWDS_DEVICE_STATE_SUSPENDING,
        XWDS_DEVICE_STATE_SUSPENDED,
        XWDS_DEVICE_STATE_RESUMING,
        XWDS_DEVICE_STATE_STARTING,
        XWDS_DEVICE_STATE_RUNNING,
};

struct xwds_device;

/**
 * @brief 基本操作的虚函数表（类似C++的虚函数表）
 */
struct xwds_base_virtual_operations {
        xwer_t (* probe)(struct xwds_device *); /**< 探测 */
        xwer_t (* remove)(struct xwds_device *); /**< 删除 */
        xwer_t (* start)(struct xwds_device *); /**< 启动 */
        xwer_t (* stop)(struct xwds_device *); /**< 停止 */
#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
        xwer_t (* suspend)(struct xwds_device *); /**< 暂停 */
        xwer_t (* resume)(struct xwds_device *); /**< 继续 */
#endif /* XWMDCFG_ds_PM */
};

/**
 * @brief 基本驱动函数表（所有设备驱动的基类）
 */
struct xwds_driver {
        /* public */
        const char * name;
        xwer_t (* probe)(struct xwds_device *); /**< 探测 */
        xwer_t (* remove)(struct xwds_device *); /**< 删除 */
        xwer_t (* start)(struct xwds_device *); /**< 启动 */
        xwer_t (* stop)(struct xwds_device *); /**< 停止 */
        xwer_t (* suspend)(struct xwds_device *); /**< 暂停 */
        xwer_t (* resume)(struct xwds_device *); /**< 继续 */
};

/**
 * @brief 设备（所有设备的基类）
 */
struct xwds_device {
        struct xwds_object obj; /**< C语言面向对象：继承struct xwds_object */
        /* attribute */
        const char * name; /**< 名字 */
        xwid_t id; /**< ID */
        const struct xwds_resources * resources; /**< 资源 */
        const struct xwds_driver * drv; /**< 基本驱动集合 */
        void * data; /**< 私有数据 */

        /* private */
        __atomic xwsq_t state; /**< 设备状态 */
        struct xwds * ds; /**< 设备所属的设备栈的指针 */
        const struct xwds_base_virtual_operations * cvops; /**< 通用操作的虚函数表 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_code
xwer_t xwds_get_regrsc(const struct xwds_resource_reg base[], xwsz_t num,
                       const char * descay[], xwsz_t descnum,
                       const struct xwds_resource_reg ** ret);

/******** ******** ******** constructor & destructor ******** ******** ********/
__xwds_code
void xwds_device_construct(struct xwds_device * dev);

__xwds_code
void xwds_device_destruct(struct xwds_device * dev);

/******** ******** base virtual operations ******** ********/
__xwds_vop
xwer_t xwds_device_cvop_probe(struct xwds_device * dev);

__xwds_vop
xwer_t xwds_device_cvop_remove(struct xwds_device * dev);

__xwds_vop
xwer_t xwds_device_cvop_start(struct xwds_device * dev);

__xwds_vop
xwer_t xwds_device_cvop_stop(struct xwds_device * dev);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
__xwds_vop
xwer_t xwds_device_cvop_suspend(struct xwds_device * dev);

__xwds_vop
xwer_t xwds_device_cvop_resume(struct xwds_device * dev);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** APIs ******** ******** ********/
__xwds_api
xwer_t xwds_device_probe(struct xwds * ds, struct xwds_device * dev,
                         xwobj_gc_f gcfunc);

__xwds_api
xwer_t xwds_device_remove(struct xwds_device * dev);

__xwds_api
xwer_t xwds_device_start(struct xwds_device * dev);

__xwds_api
xwer_t xwds_device_stop(struct xwds_device * dev);

#if defined(XWMDCFG_ds_PM) && (1 == XWMDCFG_ds_PM)
__xwds_api
xwer_t xwds_device_suspend(struct xwds_device * dev);

__xwds_api
xwer_t xwds_device_resume(struct xwds_device * dev);

__xwds_api
xwer_t xwds_device_suspend_all(struct xwds * ds, bool ign_err);

__xwds_api
xwer_t xwds_device_resume_all(struct xwds * ds, bool ign_err);
#endif /* XWMDCFG_ds_PM */

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 * @retval @ref xwds_obj_grab()
 */
static __xwds_inline
xwer_t xwds_device_grab(struct xwds_device * dev)
{
        return xwds_obj_grab(&dev->obj);
}

/**
 * @brief 减少对象的引用计数
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 * @retval @ref xwds_obj_put()
 */
static __xwds_inline
xwer_t xwds_device_put(struct xwds_device * dev)
{
        return xwds_obj_put(&dev->obj);
}

/**
 * @brief 得到对象的引用计数
 * @param dev: (I) 设备对象的指针
 * @return 引用计数
 */
static __xwds_inline
xwsq_t xwds_device_get_refcnt(struct xwds_device * dev)
{
        return xwds_obj_get_refcnt(&dev->obj);
}

/**
 * @brief 增加设备运行状态计数器
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -ESHUTDOWN: 设备已经停止运行
 */
static __xwds_inline
xwer_t xwds_device_request(struct xwds_device * dev)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq_t, &dev->state,
                                XWDS_DEVICE_STATE_RUNNING, 1,
                                NULL, NULL);
        if (__unlikely(rc < 0)) {
                rc = -ESHUTDOWN;
        }
        return rc;
}

/**
 * @brief 减少设备运行状态计数器
 * @param dev: (I) 设备对象的指针
 * @return 错误码
 * @retval OK: OK
 * @retval -EALREADY: 设备已经停止运行
 */
static __xwds_inline
xwer_t xwds_device_release(struct xwds_device * dev)
{
        xwer_t rc;

        rc = xwaop_tgt_then_sub(xwsq_t, &dev->state,
                                XWDS_DEVICE_STATE_RUNNING, 1,
                                NULL, NULL);
        if (__unlikely(rc < 0)) {
                rc = -EALREADY;
        }
        return rc;
}

#endif /* xwmd/ds/device.h */
