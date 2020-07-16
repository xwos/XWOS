/**
 * @file
 * @brief xwmd设备栈：对象
 * @author
 * + 隐星魂 (Roy.Sun) <https://xwos.tech>
 * @copyright
 * + (c) 2015 隐星魂 (Roy.Sun) <https://xwos.tech>
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#ifndef __xwmd_ds_object_h__
#define __xwmd_ds_object_h__

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********      include      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
#include <xwmd/ds/standard.h>
#include <xwos/object.h>
#include <xwos/lib/bclst.h>

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       macros      ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ******** ********       types       ******** ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
struct xwds;

/**
 * @brief 设备栈对象
 */
struct xwds_object {
        struct xwos_object xwobj; /** 继承 struct xwos_object */

        /* private */
        struct xwlib_bclst_node node; /**< 链表节点 */
        struct xwds * ds; /**< 所属的设备栈 */
};

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********         function prototypes         ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
__xwds_code
void xwds_obj_construct(struct xwds_object * obj);

__xwds_code
void xwds_obj_destruct(struct xwds_object * obj);

__xwds_code
xwer_t xwds_obj_activate(struct xwds_object * obj, xwobj_gc_f gcfunc);

__xwds_code
xwer_t xwds_obj_add(struct xwds * ds, struct xwds_object * obj);

__xwds_code
xwer_t xwds_obj_remove(struct xwds * ds, struct xwds_object * obj);

/******** ******** ******** ******** ******** ******** ******** ********
 ******** ********   inline function implementations   ******** ********
 ******** ******** ******** ******** ******** ******** ******** ********/
/**
 * @brief 增加对象的引用计数
 * @param obj: (I) 对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 对象未被激活（引用计数小于1）
 */
static __xw_inline
xwer_t xwds_obj_grab(struct xwds_object * obj)
{
        return xwos_object_grab(&obj->xwobj);
}

/**
 * @brief 减少对象的引用计数
 * @param obj: (I) 对象的指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EINVAL: 对象未被激活（引用计数小于1）
 */
static __xw_inline
xwer_t xwds_obj_put(struct xwds_object * obj)
{
        return xwos_object_put(&obj->xwobj);
}

/**
 * @brief 得到对象的引用计数
 * @param obj: (I) 对象的指针
 * @return 引用计数
 */
static __xw_inline
xwsq_t xwds_obj_get_refcnt(struct xwds_object * obj)
{
        return xwos_object_get_refcnt(&obj->xwobj);
}

#endif /* xwmd/ds/object.h */
