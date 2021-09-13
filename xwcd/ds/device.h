/**
 * @file
 * @brief 玄武设备栈：设备基类
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > Licensed under the Apache License, Version 2.0 (the "License");
 * > you may not use this file except in compliance with the License.
 * > You may obtain a copy of the License at
 * >
 * >         http://www.apache.org/licenses/LICENSE-2.0
 * >
 * > Unless required by applicable law or agreed to in writing, software
 * > distributed under the License is distributed on an "AS IS" BASIS,
 * > WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * > See the License for the specific language governing permissions and
 * > limitations under the License.
 * @note
 * - 命名规则：
 *   + operations: 应用层可以调用的方法，“操作”设备；
 *   + driver: BSP中需要实现的驱动；
 *   + callback: 底层/服务对上层/客户端的回调。
 */

#ifndef __xwcd_ds_device_h__
#define __xwcd_ds_device_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/xwaop.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/mtx.h>
#include <xwcd/ds/object.h>

/**
 * @brief 向前迭代每个设备
 */
#define xwds_itr_next_device(ds, p) \
        xwlib_bclst_itr_next_entry(p, &(ds)->devhead,  \
                                   struct xwds_device, obj.node)

/**
 * @brief 向后迭代每个设备
 */
#define xwds_itr_prev_device(ds, p) \
        xwlib_bclst_itr_prev_entry(p, &(ds)->devhead,  \
                                   struct xwds_device, obj.node)

/**
 * @brief 向前迭代每个设备（可安全删除设备）
 */
#define xwds_itr_next_device_safe(ds, p, n) \
        xwlib_bclst_itr_next_entry_safe(p, n, &(ds)->devhead,  \
                                        struct xwds_device, obj.node)

/**
 * @brief 向后迭代每个设备（可安全删除设备）
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

struct xwds_device;

/**
 * @brief 基本操作的虚函数表（类似C++的虚函数表）
 */
struct xwds_virtual_operation {
        xwer_t (* probe)(struct xwds_device *); /**< 探测 */
        xwer_t (* remove)(struct xwds_device *); /**< 删除 */
        xwer_t (* start)(struct xwds_device *); /**< 启动 */
        xwer_t (* stop)(struct xwds_device *); /**< 停止 */
#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
        xwer_t (* suspend)(struct xwds_device *); /**< 暂停 */
        xwer_t (* resume)(struct xwds_device *); /**< 继续 */
#endif
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
        struct xwds * ds; /**< 设备所属的设备栈的指针 */
        const struct xwds_virtual_operation * vop; /**< 通用操作的虚函数表 */
};

xwer_t xwds_device_vop_probe(struct xwds_device * dev);
xwer_t xwds_device_vop_remove(struct xwds_device * dev);
xwer_t xwds_device_vop_start(struct xwds_device * dev);
xwer_t xwds_device_vop_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_device_vop_suspend(struct xwds_device * dev);
xwer_t xwds_device_vop_resume(struct xwds_device * dev);
#endif

void xwds_device_construct(struct xwds_device * dev);
void xwds_device_destruct(struct xwds_device * dev);

xwer_t xwds_device_probe(struct xwds * ds, struct xwds_device * dev,
                         xwobj_gc_f gcfunc);
xwer_t xwds_device_remove(struct xwds_device * dev);
xwer_t xwds_device_start(struct xwds_device * dev);
xwer_t xwds_device_stop(struct xwds_device * dev);

#if defined(XWCDCFG_ds_PM) && (1 == XWCDCFG_ds_PM)
xwer_t xwds_device_suspend(struct xwds_device * dev);
xwer_t xwds_device_resume(struct xwds_device * dev);
xwer_t xwds_device_suspend_all(struct xwds * ds, bool ign_err);
xwer_t xwds_device_resume_all(struct xwds * ds, bool ign_err);
#endif

/**
 * @brief XWDS API：获取设备对象的标签
 * @param[in] dev: 设备对象的指针
 * @return 标签
 */
static __xwds_inline_api
xwsq_t xwds_device_gettik(struct xwds_device * dev)
{
        return xwds_obj_gettik(&dev->obj);
}

#if defined(XWCDCFG_ds_LITE) && (1 == XWCDCFG_ds_LITE)

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @retval @ref xwds_obj_grab()
 */
static __xwds_inline_api
xwer_t xwds_device_grab(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @retval @ref xwds_obj_put()
 */
static __xwds_inline_api
xwer_t xwds_device_put(struct xwds_device * dev)
{
        XWOS_UNUSED(dev);
        return XWOK;
}

/**
 * @brief XWDS API：得到对象的引用计数
 * @param[in] dev: 设备对象的指针
 * @return 引用计数
 */
static __xwds_inline_api
xwsq_t xwds_device_get_refcnt(struct xwds_device * dev)
{
        return xwds_obj_get_refcnt(&dev->obj);
}

#else

/**
 * @brief XWDS API：增加对象的引用计数
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @retval @ref xwds_obj_grab()
 */
static __xwds_inline_api
xwer_t xwds_device_grab(struct xwds_device * dev)
{
        return xwds_obj_grab(&dev->obj);
}

/**
 * @brief XWDS API：减少对象的引用计数
 * @param[in] dev: 设备对象的指针
 * @return 错误码
 * @retval @ref xwds_obj_put()
 */
static __xwds_inline_api
xwer_t xwds_device_put(struct xwds_device * dev)
{
        return xwds_obj_put(&dev->obj);
}

/**
 * @brief XWDS API：得到对象的引用计数
 * @param[in] dev: 设备对象的指针
 * @return 引用计数
 */
static __xwds_inline_api
xwsq_t xwds_device_get_refcnt(struct xwds_device * dev)
{
        return xwds_obj_get_refcnt(&dev->obj);
}
#endif

#endif /* xwcd/ds/device.h */
