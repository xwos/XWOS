/**
 * @file
 * @brief XWOS通用库：原子操作
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_lib_xwaop_h__
#define __xwos_lib_xwaop_h__

#include <xwos/standard.h>
#include <xwos/lib/xwbop.h>

/**
 * @defgroup xwos_lib_xwaop 原子操作
 * @ingroup xwos_lib
 * @{
 */

#include <xwos/ospl/soc/xwaop.h>

/**
 * @brief 调用原子操作函数，并指明内存序
 * @param[in] type: 原子变量的类型
 * @param[in] op: 操作函数
 * @param[in] memorder: 内存序
 * @param[in] a: 原子变量指针
 * @param[in,out] ...: 其他参数
 */
#define xwmoaop(type, op, a, memorder, ...) \
        xwaop__##type##__##op(a, memorder, ##__VA_ARGS__)

/**
 * @brief 对原子变量进行原子操作：加载
 * @param[in] type: 类型
 * @param[in] a: 数据地址的指针
 * @param[in] memorder: 内存顺序
 * @return 原子变量的值
 */
#define xwaop_load(type, a, memorder) \
        xwmoaop(type, load, (a), memorder)

/**
 * @brief 对原子变量进行原子操作：存储
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 新值
 * @param[in] memorder: 内存顺序
 * @return 写入后，原子变量的值
 */
#define xwaop_store(type, a, memorder, v) \
        xwmoaop(type, store, (a), memorder, (v))

/**
 * @brief 调用原子操作函数
 * @param[in] type: 原子变量的类型
 * @param[in] op: 操作函数
 * @param[in] a: 原子变量指针
 * @param[in,out] ...: 其他参数
 */
#define xwaop(type, op, a, ...) \
        xwaop__##type##__##op(a, ##__VA_ARGS__)

/**
 * @brief 对原子变量进行原子操作：读取
 * @param[in] type: 类型
 * @param[in] a: 数据地址的指针
 * @param[out] ov: 指向缓冲区的指针，此缓冲区用于返回原子变量的值
 * @note
 * + 内存序：acquire。
 */
#define xwaop_read(type, a, ov) \
        xwaop(type, read, (a), (ov))

/**
 * @brief 对原子变量进行原子操作：读取—写
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并将原子变量写为新值 `v` ；
 * + 内存序：acq_rel；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_write(type, a, v, ov) \
        xwaop(type, write, (a), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“相等”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否等于测试值，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_teq_then_write(type, a, t, v, ov) \
        xwaop(type, teq_then_write, (a), (t), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“不相等”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否不等于测试值，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tne_then_write(type, a, t, v, ov) \
        xwaop(type, tne_then_write, (a), (t), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否大于测试值，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tgt_then_write(type, a, t, v, ov) \
        xwaop(type, tgt_then_write, (a), (t), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于等于”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否大于等于测试值，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tge_then_write(type, a, t, v, ov) \
        xwaop(type, tge_then_write, (a), (t), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否小于测试值，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tlt_then_write(type, a, t, v, ov) \
        xwaop(type, tlt_then_write, (a), (t), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于等于”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否小于等于测试值，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tle_then_write(type, a, t, v, ov) \
        xwaop(type, tle_then_write, (a), (t), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否属于区间 `(l,r)` ，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tgtlt_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgtlt_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否属于区间 `[l,r)` ，
 *   - 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   - 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tgelt_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgelt_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否属于区间 `(l,r]` ，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tgtle_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgtle_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的值，并测试旧值是否属于区间 `[l,r]` ，
 *   + 如果结果为真，将原子变量更新为新值 `v` ，并返回原子变量的旧值；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：通过 `ov` 返回旧值。
 */
#define xwaop_tgele_then_write(type, a, l, r, v, ov) \
        xwaop(type, tgele_then_write, (a), (l), (r), (v), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－相加－回写
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的值，并增加加数 `v` ，结果回写到原子变量中；
 * + 内存序：acq_rel；
 * + 结果：通过 `nv` 返回相加后的新值，通过 `ov` 返回相加前的旧值。
 */
#define xwaop_add(type, a, v, nv, ov) \
        xwaop(type, add, (a), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“相等”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否等于测试值 `t` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_teq_then_add(type, a, t, v, nv, ov) \
        xwaop(type, teq_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“不相等”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否不等于测试值 `t` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tne_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tne_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否大于测试值 `t` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgt_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tgt_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于等于”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否大于等于测试值 `t` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tge_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tge_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否小于测试值 `t` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tlt_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tlt_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于等于”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否小于等于测试值 `t` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tle_then_add(type, a, t, v, nv, ov) \
        xwaop(type, tle_then_add, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `(l,r)` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgtlt_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtlt_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `[l,r)` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgelt_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgelt_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `(l,r]` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgtle_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtle_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相加－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 加数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `[l,r]` ，
 *   + 如果结果为真，旧值增加加数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgele_then_add(type, a, l, r, v, nv, ov) \
        xwaop(type, tgele_then_add, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－相减－回写
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并减去减数 `v` ，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_sub(type, a, v, nv, ov) \
        xwaop(type, sub, (a), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“相等”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否等于测试值 `t` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_teq_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, teq_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“不相等”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否不等于测试值 `t` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tne_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tne_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否大于测试值 `t` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgt_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tgt_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于等于”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否大于等于测试值 `t` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tge_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tge_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否小于测试值 `t` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tlt_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tlt_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于等于”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否小于等于测试值 `t` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tle_then_sub(type, a, t, v, nv, ov) \
        xwaop(type, tle_then_sub, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `(l,r)` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgtlt_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtlt_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `[l,r)` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgelt_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgelt_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `(l,r]` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgtle_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtle_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－相减－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `[l,r]` ，
 *   + 如果结果为真，旧值减去减数 `v` ，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgele_then_sub(type, a, l, r, v, nv, ov) \
        xwaop(type, tgele_then_sub, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－反向减法－回写
 * @param[in] type: 类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_rsb(type, a, v, nv, ov) \
        xwaop(type, rsb, (a), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“相等”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否等于测试值 `t` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_teq_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, teq_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“不相等”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否不等于测试值 `t` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tne_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tne_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否大于测试值 `t` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgt_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tgt_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“大于等于”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否大于等于测试值 `t` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tge_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tge_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否小于测试值 `t` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tlt_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tlt_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“小于等于”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量地址的指针
 * @param[in] t: 测试值
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否小于等于测试值 `t` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tle_then_rsb(type, a, t, v, nv, ov) \
        xwaop(type, tle_then_rsb, (a), (t), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `(l,r)` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgtlt_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtlt_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `[l,r)` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgelt_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgelt_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `(l,r]` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgtle_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgtle_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试“区间”－反向减法－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] l: 区间的左端点
 * @param[in] r: 区间的右端点
 * @param[in] v: 被减数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值是否属于区间 `[l,r]` ，
 *   + 如果结果为真，执行反向减法，从被减数 `v` 中减去旧值，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tgele_then_rsb(type, a, l, r, v, nv, ov) \
        xwaop(type, tgele_then_rsb, (a), (l), (r), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位与操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] v: 操作数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并和操作数 `v` 进行与操作，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_and(type, a, v, nv, ov) \
        xwaop(type, and, (a), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] v: 操作数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并和操作数 `v` 进行或操作，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_or(type, a, v, nv, ov) \
        xwaop(type, or, (a), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位异或操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] v: 操作数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并和操作数 `v` 进行异或操作，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_xor(type, a, v, nv, ov) \
        xwaop(type, xor, (a), (v), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位置1操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码位
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并对掩码 `m` 覆盖的部分进行置1操作，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_s1m(type, a, m, nv, ov) \
        xwaop(type, or, (a), (m), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位清0操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码位
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并对掩码 `m` 覆盖的部分进行清零操作，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_c0m(type, a, m, nv, ov) \
        xwaop(type, c0m, (a), (m), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位翻转操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码位
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @note
 * + 操作：读取原子变量的旧值，并对掩码 `m` 覆盖的部分进行位翻转操作，结果回写到原子变量中；
 * + 内存序：acq_rel。
 */
#define xwaop_x1m(type, a, m, nv, ov) \
        xwaop(type, xor, (a), (m), (nv), (ov))


/**
 * @brief 对原子变量进行原子操作：读取－位测试－位清零操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值中的掩码 `m` 覆盖的部分是否都为1，
 *   + 如果结果为真，将掩码 `m` 覆盖的部分全部清零，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_t1ma_then_c0m(type, a, m, nv, ov) \
        xwaop(type, t1ma_then_c0m, (a), (m), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位测试－位清零操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值中的掩码 `m` 覆盖的部分是否至少有一位为1，
 *   - 如果结果为真，将掩码 `m` 覆盖的部分全部清零，结果回写到原子变量中；
 *   - 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_t1mo_then_c0m(type, a, m, nv, ov) \
        xwaop(type, t1mo_then_c0m, (a), (m), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位测试－位置1操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值中的掩码 `m` 覆盖的部分是否都为0，
 *   + 如果结果为真，将掩码 `m` 覆盖的部分全部置1，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_t0ma_then_s1m(type, a, m, nv, ov) \
        xwaop(type, t0ma_then_s1m, (a), (m), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－位测试－位置1操作－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] m: 掩码
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并测试旧值中的掩码 `m` 覆盖的部分是否至少有一位为0，
 *   + 如果结果为真，将掩码 `m` 覆盖的部分全部置1，结果回写到原子变量中；
 *   + 如果结果为假，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_t0mo_then_s1m(type, a, m, nv, ov) \
        xwaop(type, t0mo_then_s1m, (a), (m), (nv), (ov))

/**
 * @brief 对原子变量进行原子操作：读取－测试－修改－回写
 * @param[in] type: 原子变量的类型
 * @param[in] a: 原子变量的地址
 * @param[in] tf: 测试函数的函数指针
 * @param[in] tfarg: 测试函数的参数
 * @param[in] of: 操作函数的函数指针
 * @param[in] ofarg: 操作函数的参数
 * @param[out] nv: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作后的新值
 * @param[out] ov: 指向缓冲区的指针，通过此缓冲区返回原子变量在原子操作前的旧值
 * @return 错误码
 * @retval OK: 测试成功或测试函数为空
 * @retval -EACCES: 测试失败
 * @note
 * + 操作：读取原子变量的旧值，并使用函数 `tf` 进行测试，
 *   + 如果结果为真或测试函数 `tf` 为空，使用函数 `of` 修改此值，结果回写到原子变量中；
 *   + 如果结果为假或操作函数 `tf` 为空，返回原子变量的旧值；
 * + 内存序：
 *   + 测试成功：acq_rel；
 *   + 测试失败：consume；
 * + 结果：
 *   + 测试成功时，通过 `nv` 返回原子操作后的新值，通过 `ov` 返回原子操作前的旧值；
 *   + 测试失败时，通过 `nv` 与 `ov` 同时返回旧值。
 */
#define xwaop_tst_then_op(type, a, tf, tfarg, of, ofarg, nv, ov) \
        xwaop(type, tst_then_op, (a), (tf), (tfarg), (of), (ofarg), (nv), (ov))

/******** ******** load ******** ********/
/**
 * @brief 定义原子操作模板：load
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_LOAD(type, btype) \
static __xwlib_inline \
type xwaop__##type##__load(atomic_##type * a, \
                           const enum xwaop_memory_order_em mo) \
{ \
        return (type)xwaop__##btype##__load((atomic_##btype *)a, mo); \
}

/******** ******** store ******** ********/
/**
 * @brief 定义原子操作模板：store
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_STORE(type, btype) \
static __xwlib_inline \
type xwaop__##type##__store(atomic_##type * a, \
                            const enum xwaop_memory_order_em mo, \
                            type v) \
{ \
        return (type)xwaop__##btype##__store((atomic_##btype *)a, mo, (btype)v); \
}

/******** ******** read ******** ********/
/**
 * @brief 定义原子操作模板：read
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_READ(type, btype) \
static __xwlib_inline \
void xwaop__##type##__read(atomic_##type * a, type * ov) \
{ \
        xwaop__##btype##__read((atomic_##btype *)a, \
                               (btype *)ov); \
}

/******** ******** write ******** ********/
/**
 * @brief 定义原子操作模板：write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_WRITE(type, btype) \
static __xwlib_inline \
void xwaop__##type##__write(atomic_##type * a, type v, type * ov) \
{ \
        xwaop__##btype##__write((atomic_##btype *)a, \
                                (btype)v, \
                                (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_write(atomic_##type * a, type t, type v, type * ov) \
{ \
        return xwaop__##btype##__teq_then_write((atomic_##btype *)a, \
                                                (btype)t, \
                                                (btype)v, \
                                                (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_write(atomic_##type * a, \
                                       type t, \
                                       type v, \
                                       type * ov) \
{ \
        return xwaop__##btype##__tne_then_write((atomic_##btype *)a, \
                                                (btype)t, \
                                                (btype)v, \
                                                (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_write(atomic_##type * a, \
                                       type t, \
                                       type v, \
                                       type * ov) \
{ \
        return xwaop__##btype##__tgt_then_write((atomic_##btype *)a, \
                                                (btype)t, \
                                                (btype)v, \
                                                (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_write(atomic_##type * a, \
                                       type t, \
                                       type v, \
                                       type * ov) \
{ \
        return xwaop__##btype##__tge_then_write((atomic_##btype *)a, \
                                                (btype)t, \
                                                (btype)v, \
                                                (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tlt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_write(atomic_##type * a, \
                                       type t, \
                                       type v, \
                                       type * ov) \
{ \
        return xwaop__##btype##__tlt_then_write((atomic_##btype *)a, \
                                                (btype)t, \
                                                (btype)v, \
                                                (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_write(atomic_##type * a, \
                                       type t, \
                                       type v, \
                                       type * ov) \
{ \
        return xwaop__##btype##__tle_then_write((atomic_##btype *)a, \
                                                (btype)t, \
                                                (btype)v, \
                                                (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_write(atomic_##type * a, \
                                         type l, type r, \
                                         type v, \
                                         type * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_write((atomic_##btype *)a, \
                                                  (btype)l, (btype)r, \
                                                  (btype)v, \
                                                  (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_write(atomic_##type * a, \
                                         type l, type r, \
                                         type v, \
                                         type * ov) \
{ \
        return xwaop__##btype##__tgelt_then_write((atomic_##btype *)a, \
                                                  (btype)l, (btype)r, \
                                                  (btype)v, \
                                                  (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_write(atomic_##type * a, \
                                         type l, type r, \
                                         type v, \
                                         type * ov) \
{ \
        return xwaop__##btype##__tgtle_then_write((atomic_##btype *)a, \
                                                  (btype)l, (btype)r, \
                                                  (btype)v, \
                                                  (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_write
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_WRITE(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_write(atomic_##type * a, \
                                         type l, type r, \
                                         type v, \
                                         type * ov) \
{ \
        return xwaop__##btype##__tgele_then_write((atomic_##btype *)a, \
                                                  (btype)l, (btype)r, \
                                                  (btype)v, \
                                                  (btype *)ov); \
}

/******** ******** add ******** ********/
/**
 * @brief 定义原子操作模板：add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_ADD(type, btype) \
static __xwlib_inline \
void xwaop__##type##__add(atomic_##type * a, \
                          type v, \
                          type * nv, type * ov) \
{ \
        xwaop__##btype##__add((atomic_##btype *)a, \
                              (btype)v, \
                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_add(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__teq_then_add((atomic_##btype *)a, \
                                              (btype)t, (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_add(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tne_then_add((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_add(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgt_then_add((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_add(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tge_then_add((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tlt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_add(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tlt_then_add((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_add(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tle_then_add((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_add(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_add((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_add(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgelt_then_add((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_add(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgtle_then_add((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_add
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_ADD(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_add(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgele_then_add((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/******** ******** subtract ******** ********/
/**
 * @brief 定义原子操作模板：sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_SUB(type, btype) \
static __xwlib_inline \
void xwaop__##type##__sub(atomic_##type * a, \
                          type v, \
                          type * nv, type * ov) \
{ \
        xwaop__##btype##__sub((atomic_##btype *)a, \
                              (btype)v, \
                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_sub(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__teq_then_sub((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_sub(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tne_then_sub((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_sub(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgt_then_sub((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_sub(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tge_then_sub((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tlt_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_sub(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tlt_then_sub((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_sub(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tle_then_sub((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_sub(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_sub((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_sub(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgelt_then_sub((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_sub(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgtle_then_sub((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_sub
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_SUB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_sub(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgele_then_sub((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/******** ******** reverse subtract ******** ********/
/**
 * @brief 定义原子操作模板：rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_RSB(type, btype) \
static __xwlib_inline \
void xwaop__##type##__rsb(atomic_##type * a, \
                          type v, \
                          type * nv, type * ov) \
{ \
        xwaop__##btype##__rsb((atomic_##btype *)a, \
                              (btype)v, \
                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：teq_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TEQ_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__teq_then_rsb(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__teq_then_rsb((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tne_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TNE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tne_then_rsb(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tne_then_rsb((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgt_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgt_then_rsb(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgt_then_rsb((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tge_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tge_then_rsb(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tge_then_rsb((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tlt_then_rsb(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tlt_then_rsb((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tle_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TLE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tle_then_rsb(atomic_##type * a, \
                                     type t, \
                                     type v, \
                                     type * nv, type * ov) \
{ \
        return xwaop__##btype##__tle_then_rsb((atomic_##btype *)a, \
                                              (btype)t, \
                                              (btype)v, \
                                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtlt_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtlt_then_rsb(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgtlt_then_rsb((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgelt_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELT_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgelt_then_rsb(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgelt_then_rsb((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgtle_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGTLE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgtle_then_rsb(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgtle_then_rsb((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：tgele_then_rsb
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TGELE_THEN_RSB(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tgele_then_rsb(atomic_##type * a, \
                                       type l, type r, \
                                       type v, \
                                       type * nv, type * ov) \
{ \
        return xwaop__##btype##__tgele_then_rsb((atomic_##btype *)a, \
                                                (btype)l, (btype)r, \
                                                (btype)v, \
                                                (btype *)nv, (btype *)ov); \
}

/******** ******** bit operations ******** ********/
/**
 * @brief 定义原子操作模板：or
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_OR(type, btype) \
static __xwlib_inline \
void xwaop__##type##__or(atomic_##type * a, \
                         type v, \
                         type * nv, type * ov) \
{ \
        xwaop__##btype##__or((atomic_##btype *)a, \
                             (btype)v, \
                             (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：and
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_AND(type, btype) \
static __xwlib_inline \
void xwaop__##type##__and(atomic_##type * a, \
                          type v, \
                          type * nv, type * ov) \
{ \
        xwaop__##btype##__and((atomic_##btype *)a, \
                              (btype)v, \
                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：xor
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_XOR(type, btype) \
static __xwlib_inline \
void xwaop__##type##__xor(atomic_##type * a, \
                          type v, \
                          type * nv, type * ov) \
{ \
        xwaop__##btype##__xor((atomic_##btype *)a, \
                              (btype)v, \
                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：c0m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_C0M(type, btype) \
static __xwlib_inline \
void xwaop__##type##__c0m(atomic_##type * a, \
                          type m, \
                          type * nv, type * ov) \
{ \
        xwaop__##btype##__c0m((atomic_##btype *)a, \
                              (btype)m, \
                              (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：t1ma_then_c0m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T1MA_THEN_C0M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t1ma_then_c0m(atomic_##type * a, \
                                      type m, \
                                      type * nv, type * ov) \
{ \
        return xwaop__##btype##__t1ma_then_c0m((atomic_##btype *)a, \
                                               (btype)m, \
                                               (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：t1mo_then_c0m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T1MO_THEN_C0M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t1mo_then_c0m(atomic_##type * a, \
                                      type m, \
                                      type * nv, type * ov) \
{ \
        return xwaop__##btype##__t1mo_then_c0m((atomic_##btype *)a, \
                                               (btype)m, \
                                               (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：t0ma_then_s1m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T0MA_THEN_S1M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t0ma_then_s1m(atomic_##type * a, \
                                      type m, \
                                      type * nv, type * ov) \
{ \
        return xwaop__##btype##__t0ma_then_s1m((atomic_##btype *)a, \
                                               (btype)m, \
                                               (btype *)nv, (btype *)ov); \
}

/**
 * @brief 定义原子操作模板：t0mo_then_s1m
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_T0MO_THEN_S1M(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__t0mo_then_s1m(atomic_##type * a, \
                                      type m, \
                                      type * nv, type * ov) \
{ \
        return xwaop__##btype##__t0mo_then_s1m((atomic_##btype *)a, \
                                               (btype)m, \
                                               (btype *)nv, (btype *)ov); \
}

/******** ******** test and operation ******** ********/
/**
 * @brief 定义原子操作模板：tst_then_op
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP_TST_THEN_OP(type, btype) \
static __xwlib_inline \
xwer_t xwaop__##type##__tst_then_op(atomic_##type * a, \
                                    xwaop_tst_f tst, void * tst_args, \
                                    xwaop_op_f op, void * op_args, \
                                    type * nv, type * ov) \
{ \
        return xwaop__##btype##__tst_then_op((atomic_##btype *)a, \
                                             tst, tst_args, \
                                             op, op_args, \
                                             (btype *)nv, (btype *)ov); \
}

/**
 * @brief 使用模板为某一类型定义原子操作函数集合
 * @param[in] type: 类型
 * @param[in] btype: 基础类型
 */
#define DEFINE_XWAOP(type, btype) \
        DEFINE_XWAOP_LOAD(type, btype) \
        DEFINE_XWAOP_STORE(type, btype) \
        DEFINE_XWAOP_READ(type, btype) \
        DEFINE_XWAOP_WRITE(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TNE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TLT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TLE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_WRITE(type, btype) \
        DEFINE_XWAOP_ADD(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TNE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TLT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TLE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_ADD(type, btype) \
        DEFINE_XWAOP_SUB(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TNE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TLT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TLE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_SUB(type, btype) \
        DEFINE_XWAOP_RSB(type, btype) \
        DEFINE_XWAOP_TEQ_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TNE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TLT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TLE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGTLT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGELT_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGTLE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_TGELE_THEN_RSB(type, btype) \
        DEFINE_XWAOP_OR(type, btype) \
        DEFINE_XWAOP_AND(type, btype) \
        DEFINE_XWAOP_XOR(type, btype) \
        DEFINE_XWAOP_C0M(type, btype) \
        DEFINE_XWAOP_T1MA_THEN_C0M(type, btype) \
        DEFINE_XWAOP_T1MO_THEN_C0M(type, btype) \
        DEFINE_XWAOP_T0MA_THEN_S1M(type, btype) \
        DEFINE_XWAOP_T0MO_THEN_S1M(type, btype) \
        DEFINE_XWAOP_TST_THEN_OP(type, btype)


#if (16 == BITS_PER_XWSZ_T)
DEFINE_XWAOP(xwsz_t, xwu16_t)
#elif (32 == BITS_PER_XWSZ_T)
DEFINE_XWAOP(xwsz_t, xwu32_t)
#elif (64 == BITS_PER_XWSZ_T)
DEFINE_XWAOP(xwsz_t, xwu64_t)
#endif

#if (16 == BITS_PER_XWSSZ_T)
DEFINE_XWAOP(xwssz_t, xws16_t)
#elif (32 == BITS_PER_XWSSZ_T)
DEFINE_XWAOP(xwssz_t, xws32_t)
#elif (64 == BITS_PER_XWSSZ_T)
DEFINE_XWAOP(xwssz_t, xws64_t)
#endif

#if (16 == BITS_PER_XWPTR_T)
DEFINE_XWAOP(xwptr_t, xwu16_t)
DEFINE_XWAOP(xwlfq_t, xwu16_t)
#elif (32 == BITS_PER_XWPTR_T)
DEFINE_XWAOP(xwlfq_t, xwu32_t)
DEFINE_XWAOP(xwptr_t, xwu32_t)
#elif (64 == BITS_PER_XWPTR_T)
DEFINE_XWAOP(xwlfq_t, xwu64_t)
DEFINE_XWAOP(xwptr_t, xwu64_t)
#endif

#if (16 == BITS_PER_XWER_T)
DEFINE_XWAOP(xwer_t, xws16_t)
#elif (32 == BITS_PER_XWER_T)
DEFINE_XWAOP(xwer_t, xws32_t)
#elif (64 == BITS_PER_XWER_T)
DEFINE_XWAOP(xwer_t, xws64_t)
#endif

#if (8 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid_t, xwu8_t)
#elif (16 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid_t, xwu16_t)
#elif (32 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid_t, xwu32_t)
#elif (64 == BITS_PER_XWID_T)
DEFINE_XWAOP(xwid_t, xwu64_t)
#endif

#if (8 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid_t, xws8_t)
#elif (16 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid_t, xws16_t)
#elif (32 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid_t, xws32_t)
#elif (64 == BITS_PER_XWSID_T)
DEFINE_XWAOP(xwsid_t, xws64_t)
#endif

#if (16 == BITS_PER_XWREG_T)
DEFINE_XWAOP(xwreg_t, xwu16_t)
#elif (32 == BITS_PER_XWREG_T)
DEFINE_XWAOP(xwreg_t, xwu32_t)
#elif (64 == BITS_PER_XWREG_T)
DEFINE_XWAOP(xwreg_t, xwu64_t)
#endif

#if (16 == BITS_PER_XWSREG_T)
DEFINE_XWAOP(xwsreg_t, xws16_t)
#elif (32 == BITS_PER_XWSREG_T)
DEFINE_XWAOP(xwsreg_t, xws32_t)
#elif (64 == BITS_PER_XWSREG_T)
DEFINE_XWAOP(xwsreg_t, xws64_t)
#endif

#if (16 == BITS_PER_XWSQ_T)
DEFINE_XWAOP(xwsq_t, xwu16_t)
#elif (32 == BITS_PER_XWSQ_T)
DEFINE_XWAOP(xwsq_t, xwu32_t)
#elif (64 == BITS_PER_XWSQ_T)
DEFINE_XWAOP(xwsq_t, xwu64_t)
#endif

#if (16 == BITS_PER_XWSSQ_T)
DEFINE_XWAOP(xwssq_t, xws16_t)
#elif (32 == BITS_PER_XWSSQ_T)
DEFINE_XWAOP(xwssq_t, xws32_t)
#elif (64 == BITS_PER_XWSSQ_T)
DEFINE_XWAOP(xwssq_t, xws64_t)
#endif

#if (8 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp_t, xwu16_t)
#elif (16 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp_t, xwu16_t)
#elif (32 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp_t, xwu32_t)
#elif (64 == BITS_PER_XWBMP_T)
DEFINE_XWAOP(xwbmp_t, xwu64_t)
#endif

DEFINE_XWAOP(xwtk_t, xwu64_t)
DEFINE_XWAOP(xwtm_t, xws64_t)

/**
 * @} xwos_lib_xwaop
 */

/**
 * @defgroup xwos_lib_xwbmpaop 位图原子操作
 * @ingroup xwos_lib
 * @{
 */

/**
 * @brief XWOS BOPLIB：声明原子位图
 * @param[in] name: 符号名
 * @param[in] bits: 位图中的位数
 */
// cppcheck-suppress [misra-c2012-20.7]
#define xwbmpaop_define(name, bits) atomic_xwbmp_t name[BITS_TO_XWBMP_T(bits)]

/**
 * @brief XWOS AOPLIB：测试位图中的某位是否被置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被测试的位的序号
 * @return 布尔值
 * @retval true: 置位
 * @retval false: 复位
 * @note
 * - 内存序：consume
 */
bool xwbmpaop_t1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：将位图中某位置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被置1的位的序号
 * @note
 * - 内存序：acq_rel
 */
void xwbmpaop_s1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：将位图中某位清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被清0的位的序号
 * @note
 * - 内存序：acq_rel
 */
void xwbmpaop_c0i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：将位图中某位翻转
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被翻转的位的序号
 * @note
 * - 内存序：acq_rel
 */
void xwbmpaop_x1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：测试位图中某位是否为0，如果是，就将它置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被测试位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwer_t xwbmpaop_t0i_then_s1i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：测试位图中某位是否为1，如果是，就将它清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] idx: 被测试位的序号
 * @return 错误码
 * @retval XWOK: 没有错误
 * @retval -EACCES: 测试失败
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwer_t xwbmpaop_t1i_then_c0i(atomic_xwbmp_t * bmp, xwsq_t idx);

/**
 * @brief XWOS AOPLIB：从最高位起查找位图中第一个为1的位并将它清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为1
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_fls_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：从最高位起查找位图中第一个为0的位并将它置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为0
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_flz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：从最低位起查找位图中第一个为1的位并将它清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为1
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_ffs_then_c0i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：从最低位起查找位图中第一个为0的位并将它置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @return 错误码
 * @retval >=0: 位的序号
 * @retval -ENODATA: 没有任何一个位为0
 * @note
 * - 内存序：测试成功，acq_rel；测试失败，consume；
 */
xwssq_t xwbmpaop_ffz_then_s1i(atomic_xwbmp_t * bmp, xwsz_t num);

/**
 * @brief XWOS AOPLIB：将位图所有位置1
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @note
 * - 内存序：acq_rel
 */
static __xwlib_inline
void xwbmpaop_s1all(atomic_xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwaop_s1m(xwbmp_t, &bmp[i], ~((xwbmp_t)0), NULL, NULL);
        }
}

/**
 * @brief XWOS AOPLIB：将位图所有位清0
 * @param[in] bmp: 位图的起始地址指针
 * @param[in] num: 位图中总的位数
 * @note
 * - 内存序：acq_rel
 */
static __xwlib_inline
void xwbmpaop_c0all(atomic_xwbmp_t * bmp, xwsq_t num)
{
        xwsz_t n = BITS_TO_XWBMP_T(num);
        xwsz_t i;

        for (i = 0; i < n; i++) {
                xwaop_c0m(xwbmp_t, &bmp[i], ~((xwbmp_t)0), NULL, NULL);
        }
}

/**
 * @} xwos_lib_xwbmpaop
 */

#endif /* xwos/lib/xwaop.h */
