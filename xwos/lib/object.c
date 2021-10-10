/**
 * @file
 * @brief 玄武OS通用库：XWOS的对象
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at http://mozilla.org/MPL/2.0/.
 */

#include <xwos/standard.h>
#include <xwos/lib/object.h>
#include <xwos/osal/irq.h>
#include <xwos/osal/skd.h>

#define XWOS_OBJTIK_CHUNK        ((xwsq_t)(1 << 20)) /* 1M */
#define XWOS_OBJTIK_CHUNK_MSK    (XWOS_OBJTIK_CHUNK - 1)

/**
 * @brief 对象标签分配器，每次分配XWOS_OBJTIK_CHUNK个ID给CPU
 */
__xwlib_data xwsq_a xwos_objtik_dispatcher = (CPUCFG_CPU_NUM * XWOS_OBJTIK_CHUNK);

/**
 * @brief 每CPU的对象标签分配器
 */
__xwlib_data xwsq_a xwos_objtik[CPUCFG_CPU_NUM] __xwcc_alignl1cache = {0};

/**
 * @brief 初始化对象标签分配器
 */
__xwlib_code
void xwos_objtik_init(void)
{
        xwsq_t i;

        xwos_objtik_dispatcher = (CPUCFG_CPU_NUM * XWOS_OBJTIK_CHUNK);
        for (i = 0; i < CPUCFG_CPU_NUM; i++) {
                xwos_objtik[i] = (i * XWOS_OBJTIK_CHUNK);
        }
}

/**
 * @brief 从对象标签分配器获取一个标签
 * @return 标签
 * @note
 * - 为了避免频繁地访问多CPU共享原子变量xwos_objtik_dispatcher，每次获取标签
 *   都是从当前CPU的每CPU变量xwos_objtik中分配的，只有当xwos_objtik的分配额
 *   达到最大额度XWOS_OBJTIK_CHUNK时，才会访问xwos_objtik_dispatcher再次预先分配
 *   XWOS_OBJTIK_CHUNK个标签。
 */
__xwlib_code
xwsq_t xwos_objtik_get(void)
{
        xwid_t cpu;
        xwsq_a * objtik;
        xwsq_t curr, max, ov;
        xwer_t rc;

        cpu = xwos_skd_id_lc();
        objtik = &xwos_objtik[cpu];
        while (true) {
                curr = xwaop_load(xwsq, objtik, xwmb_modr_relaxed);
                max = (curr & (~XWOS_OBJTIK_CHUNK_MSK)) + XWOS_OBJTIK_CHUNK_MSK;
                rc = xwaop_tlt_then_add(xwsq, objtik, max, 1, NULL, &ov);
                if (rc < 0) {
                        xwreg_t cpuirq;
                        xwsq_t chunk;

                        xwos_cpuirq_save_lc(&cpuirq);
                        curr = xwaop_load(xwsq, objtik, xwmb_modr_relaxed);
                        if (curr == max) {
                                ov = curr;
                                xwaop_add(xwsq, &xwos_objtik_dispatcher,
                                          XWOS_OBJTIK_CHUNK, NULL, &chunk);
                                xwaop_store(xwsq, objtik, xwmb_modr_relaxed, chunk);
                                xwos_cpuirq_restore_lc(cpuirq);
                                break;
                        } else {
                                xwos_cpuirq_restore_lc(cpuirq);
                        }
                } else {
                        break;
                }
        }
        return ov;
}

/**
 * @brief XWOS对象的构造函数
 * @param[in] obj: 对象指针
 */
__xwlib_code
void xwos_object_construct(struct xwos_object * obj)
{
        obj->refcnt = 0;
        obj->gcfunc = NULL;
        obj->tik = 0;
}

/**
 * @brief XWOS对象的析构函数
 * @param[in] obj: 对象指针
 */
__xwlib_code
void xwos_object_destruct(struct xwos_object * obj)
{
        obj->gcfunc = NULL;
        obj->tik = 0;
        obj->magic = 0;
}

/**
 * @brief 激活一个XWOS对象（将其引用计数初始化位1）
 * @param[in] obj: 对象指针
 * @param[in] gcfunc: 垃圾回收函数：当对象应用计数为0，调用此函数回收资源
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJACTIVE: 对象已激活
 */
__xwlib_code
xwer_t xwos_object_activate(struct xwos_object * obj, xwobj_gc_f gcfunc)
{
        xwer_t rc;

        rc = xwaop_teq_then_add(xwsq, &obj->refcnt, 0, 1, NULL, NULL);
        if (__xwcc_likely(XWOK == rc)) {
                obj->tik = xwos_objtik_get();
                obj->magic = XWOS_OBJ_MAGIC;
                obj->gcfunc = gcfunc;
        } else {
                rc = -EOBJACTIVE;
        }
        return rc;
}

/**
 * @brief 设置XWOS对象的垃圾回收函数
 * @param[in] obj: 对象指针
 * @param[in] gcfunc: 垃圾回收函数：当对象应用计数为0，调用此函数回收资源
 */
__xwlib_code
void xwos_object_setgc(struct xwos_object * obj, xwobj_gc_f gcfunc)
{
        if (XWOS_OBJ_MAGIC == obj->magic) {
                obj->gcfunc = gcfunc;
        }
}

static __xwlib_code
xwer_t xwos_object_acquire_refaop_tst(const void * ov, void * arg)
{
        const xwsq_t * o;
        xwobj_d * objd;
        xwer_t rc;

        o = (const xwsq_t *)ov;
        objd = arg;
        if ((XWOS_OBJ_MAGIC != objd->obj->magic) ||
                   (objd->tik != objd->obj->tik)) {
                rc = -EACCES;
        } else if (*o < 1) {
                rc = -EOBJDEAD;
        } else {
                rc = XWOK;
        }
        return rc;
}

static __xwlib_code
void xwos_object_acquire_refaop_op(void * nv, const void * ov, void * arg)
{
        xwsq_t * n;
        const xwsq_t * o;

        XWOS_UNUSED(arg);
        n = (xwsq_t *)nv;
        o = (xwsq_t *)ov;
        *n = *o + 1;
}

/**
 * @brief 检查对象的标签并增加引用计数
 * @param[in] obj: 对象指针
 * @param[in] tik: 标签
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 */
__xwlib_code
xwer_t xwos_object_acquire(struct xwos_object * obj, xwsq_t tik)
{
        xwobj_d objd = {obj, tik};
        xwer_t rc;

        rc = xwaop_tst_then_op(xwsq, &obj->refcnt,
                               xwos_object_acquire_refaop_tst, &objd,
                               xwos_object_acquire_refaop_op, &objd,
                               NULL, NULL);
        return rc;
}

__xwlib_code
xwer_t xwos_object_release_refaop_tst(const void * ov, void * arg)
{
        const xwsq_t * o;
        xwobj_d * objd;
        xwer_t rc;

        o = (const xwsq_t *)ov;
        objd = arg;
        if ((XWOS_OBJ_MAGIC != objd->obj->magic) ||
                   (objd->tik != objd->obj->tik)) {
                rc = -EACCES;
        } else if (*o < 1) {
                rc = -EOBJDEAD;
        } else {
                rc = XWOK;
        }
        return rc;
}

__xwlib_code
void xwos_object_release_refaop_op(void * nv, const void * ov, void * arg)
{
        xwsq_t * n;
        const xwsq_t * o;

        XWOS_UNUSED(arg);
        n = (xwsq_t *)nv;
        o = (xwsq_t *)ov;
        *n = *o - 1;
}

/**
 * @brief 检查对象的标签并减少引用计数
 * @param[in] obj: 对象指针
 * @param[in] tik: 标签
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象无效
 * @retval -EACCES: 对象标签检查失败
 */
__xwlib_code
xwer_t xwos_object_release(struct xwos_object * obj, xwsq_t tik)
{
        xwobj_d objd = {obj, tik};
        xwsq_t nv;
        xwer_t rc;

        rc = xwaop_tst_then_op(xwsq, &obj->refcnt,
                               xwos_object_release_refaop_tst, &objd,
                               xwos_object_release_refaop_op, &objd,
                               &nv, NULL);
        if (__xwcc_likely(XWOK == rc)) {
                if (__xwcc_unlikely(0 == nv)) {
                        if (obj->gcfunc) {
                                rc = obj->gcfunc(obj);
                        }/* else {} */
                }/* else {} */
        }/* else {} */
        return rc;
}

/**
 * @brief 增加对象的引用计数
 * @param[in] obj: 对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象已销毁
 */
__xwlib_code
xwer_t xwos_object_grab(struct xwos_object * obj)
{
        xwer_t rc;

        rc = xwaop_tge_then_add(xwsq, &obj->refcnt, 1, 1, NULL, NULL);
        if (__xwcc_unlikely(rc < 0)) {
                rc = -EOBJDEAD;
        }/* else {} */
        return rc;
}

/**
 * @brief 减少对象的引用计数
 * @param[in] obj: 对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象已销毁
 */
__xwlib_code
xwer_t xwos_object_put(struct xwos_object * obj)
{
        xwer_t rc;
        xwsq_t nv;

        rc = xwaop_tgt_then_sub(xwsq, &obj->refcnt,
                                0, 1,
                                &nv, NULL);
        if (__xwcc_likely(XWOK == rc)) {
                if (__xwcc_unlikely(0 == nv)) {
                        if (obj->gcfunc) {
                                rc = obj->gcfunc(obj);
                        }/* else {} */
                }/* else {} */
        } else {
                rc = -EOBJDEAD;
        }
        return rc;
}

/**
 * @brief 减少对象的引用计数，不触发垃圾回收函数
 * @param[in] obj: 对象指针
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EOBJDEAD: 对象已销毁
 */
__xwlib_code
xwer_t xwos_object_rawput(struct xwos_object * obj)
{
        xwer_t rc;

        rc = xwaop_tgt_then_sub(xwsq, &obj->refcnt,
                                0, 1,
                                NULL, NULL);
        if (rc < 0) {
                rc = -EOBJDEAD;
        }
        return rc;
}

/**
 * @brief 获取XWOS对象的引用计数
 * @param[in] obj: 对象指针
 * @return 引用计数
 */
__xwlib_code
xwsq_t xwos_object_get_refcnt(struct xwos_object * obj)
{
        return xwaop_load(xwsq, &obj->refcnt, xwmb_modr_relaxed);
}
