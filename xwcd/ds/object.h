/**
 * @file
 * @brief 玄武设备栈：对象
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
 */

#ifndef __xwcd_ds_object_h__
#define __xwcd_ds_object_h__

#include <xwcd/ds/standard.h>
#include <xwos/lib/object.h>
#include <xwos/lib/bclst.h>

struct xwds;

/**
 * @brief 设备状态（引用计数）枚举
 */
enum xwds_object_refcnt_em {
        XWDS_OBJ_REF_REMOVE = 0,
        XWDS_OBJ_REF_SHUTDOWN,
        XWDS_OBJ_REF_SUSPEND,
        XWDS_OBJ_REF_RUNNING,
};

/**
 * @brief 设备栈对象
 */
struct xwds_object {
        struct xwos_object xwobj; /** 继承 struct xwos_object */

        /* private */
        struct xwlib_bclst_node node; /**< 链表节点 */
        struct xwds * ds; /**< 所属的设备栈 */
};

void xwds_obj_construct(struct xwds_object * obj);
void xwds_obj_destruct(struct xwds_object * obj);
xwsq_t xwds_obj_gettik(struct xwds_object * obj);
xwer_t xwds_obj_probe(struct xwds_object * obj, xwobj_gc_f gcfunc);
xwer_t xwds_obj_remove(struct xwds_object * obj);
xwer_t xwds_obj_start(struct xwds_object * obj);
xwer_t xwds_obj_stop(struct xwds_object * obj);
xwer_t xwds_obj_suspend(struct xwds_object * obj);
xwer_t xwds_obj_resume(struct xwds_object * obj);
xwer_t xwds_obj_grab(struct xwds_object * obj);
xwer_t xwds_obj_put(struct xwds_object * obj);
xwsq_t xwds_obj_get_refcnt(struct xwds_object * obj);
xwer_t xwds_obj_add(struct xwds * ds, struct xwds_object * obj);
xwer_t xwds_obj_del(struct xwds * ds, struct xwds_object * obj);

#endif /* xwcd/ds/object.h */
