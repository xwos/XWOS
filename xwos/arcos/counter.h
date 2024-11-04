/**
 * @file
 * @brief xwos::autosar::cp::os::counter
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_arcos_counter_h__
#define __xwos_arcos_counter_h__

#include <xwos/arcos/error.h>
#include <xwos/lib/bclst.h>
#include <xwos/osal/lock/seqlock.h>
#include <xwos/osal/swt.h>

/**
 * @defgroup arcos_counter xwos::autosar::cp::os::counter
 * @ingroup arcos
 *
 * ## ISO:17356-3 Compliances
 *
 * N/A
 *
 * ## AUTOSAR CP OS Compliances
 *
 * ### AUTOSAR CP OS Counter
 *
 * + [x] [SWS_Os_00301] The Operating System module shall provide the ability to
 *       increment a software counter as an alternative action on alarm expiry
 *       (SRS_Os_11021). The Operating System module provides API service
 *       `IncrementCounter()` (see [SWS_Os_00399]) to increment a software Counter.
 * + [x] [SWS_Os_00374] The Operating System module shall handle all
 *       the initialization and configuration of timers used directly by the
 *       Operating System module and not handled by the GPT driver. ()
 * + [ ] [SWS_Os_00384] The Operating System module shall adjust the read out
 *       values of hardware timers (which drive counters) in such that the lowest value
 *       is zero and consecutive reads return an increasing count value until
 *       the timer wraps at its modulus.
 *
 * ### Multi-Core
 *
 * + [x] [SWS_Os_00629] A Counter belonging to an OS-Application shall be incremented
 *       by the core on which the OS-Application resides. The Counter shall not be
 *       incremented by other cores. (SRS_Os_80013)
 * + [x] [SWS_Os_00630] It shall not be allowed to drive a ScheduleTable from a
 *       Counter, which is assigned to a different core. (SRS_Os_80013)
 * + [x] [SWS_Os_00631] It shall not be allowed to drive an Alarm from a Counter, which
 *       is assigned to a different core. (SRS_Os_80013)
 *
 * ### API
 *
 * #### `IncrementCounter()`
 *
 * + [x] [SWS_Os_00399] Definition of API function `IncrementCounter()` .
 *   + **注解** ：XWOS提供后端实现 `xwarcos_counter_increment()` 。
 * + [x] [SWS_Os_00285] If the input parameter `CounterID` in a call of
 *       `IncrementCounter()` is not valid OR the Counter is a hardware Counter,
 *       `IncrementCounter()` shall return `E_OS_ID` . ()
 * + [x] [SWS_Os_00286] If the input parameter of `IncrementCounter()` is valid,
 *       `IncrementCounter()` shall increment the Counter `CounterID` by one
 *       (if any alarm connected to this Counter expires, the given action,
 *       e.g. Task activation, is done) and shall return `E_OK` . (SRS_Os_11020)
 * + [ ] [SWS_Os_00321] If in a call of `IncrementCounter()` an error happens
 *       during the execution of an alarm action, e.g. `E_OS_LIMIT` caused by a
 *       Task activation, `IncrementCounter()` shall call the error hook(s),
 *       but the `IncrementCounter()` service itself shall return `E_OK` . ()
 * + [x] [SWS_Os_00529] Caveats of `IncrementCounter()` : If called from a Task,
 *       rescheduling may take place. ()
 * + [ ] [SWS_Os_00530] Availability of `IncrementCounter()` : Available in
 *       all Scalability Classes. ()
 *
 * #### `GetCounterValue()`
 *
 * + [x] [SWS_Os_00383] Definition of API function `GetCounterValue()` . ()
 *   + **注解** ：XWOS提供后端实现 `xwarcos_counter_get_value()` 。
 * + [x] [SWS_Os_00376] If the input parameter `CounterID` in a call of
 *       `GetCounterValue()` is not valid,
 *       `GetCounterValue()` shall return `E_OS_ID` . ()
 * + [x] [SWS_Os_00377] If the input parameter `CounterID` in a call of
 *       `GetCounterValue()` is valid, `GetCounterValue()` shall return
 *       the current tick value of the Counter via `Value` and return `E_OK` . ()
 * + [ ] [SWS_Os_00531] Caveats of `GetCounterValue()` : Note that for counters of
 *       `OsCounterType = HARDWARE` the real timer value
 *       (the - possibly adjusted - hardware value, see [SWS_Os_00384]) is returned,
 *       whereas for counters of `OsCounterType = SOFTWARE` the current "software"
 *       tick value is returned. ()
 * + [ ] [SWS_Os_00532] Availability of `GetCounterValue()` : Available in all
 *       Scalability Classes. ()
 *
 * #### `GetElapsedValue()`
 *
 * + [x] [SWS_Os_00392] Definition of API function `GetElapsedValue()` . ()
 *   + **注解** ：XWOS提供后端实现 `xwarcos_counter_get_elapsed_value()` 。
 * + [x] [SWS_Os_00381] If the input parameter `CounterID` in a call of
 *       `GetElapsedValue()` is not valid `GetElapsedValue()` shall
 *       return `E_OS_ID` . ()
 * + [x] [SWS_Os_00391] If the `Value` in a call of `GetElapsedValue()`
 *       is larger than the max allowed value of the `CounterID` , `GetElapsedValue()`
 *       shall return `E_OS_VALUE` . ()
 * + [x] [SWS_Os_00382] If the input parameters in a call of `GetElapsedValue()`
 *       are valid, `GetElapsedValue()` shall return the number of elapsed ticks
 *       since the given `Value` value via `ElapsedValue` and shall return `E_OK` . ()
 * + [x] [SWS_Os_00460] `GetElapsedValue()` shall return the current tick value of the
 *       Counter in the `Value` parameter. ()
 * + [x] [SWS_Os_00533] Caveats of `GetElapsedValue()` : If the timer already passed
 *       the `Value` value a second (or multiple) time, the result returned is wrong.
 *       The reason is that the service can not detect such a relative overflow. ()
 * + [ ] [SWS_Os_00534] Availability of `GetElapsedValue()` : Available in all
 *       Scalability Classes. ()
 *
 * ### Configuration

 * + [x] [ECUC_Os_00026] OsCounter
 *   + [x] [ECUC_Os_00027] OsCounterMaxAllowedValue
 *   + [x] [ECUC_Os_00028] OsCounterMinCycle
 *   + [x] [ECUC_Os_00029] OsCounterTicksPerBase
 *   + [x] [ECUC_Os_00255] OsCounterType
 *   + [x] [ECUC_Os_00030] OsSecondsPerTick
 *   + [ ] [ECUC_Os_00031] OsCounterAccessingApplication
 * + [ ] [ECUC_Os_00371] OsDriver
 *   + [ ] [ECUC_Os_00032] OsGptChannelRef
 * @{
 */

struct xwarcos_counter;
struct xwarcos_counter_node;

/**
 * @brief AUTOSAR CP OS 计数器节点函数
 */
typedef bool (* xwarcos_counter_operation_f)(struct xwarcos_counter *,
                                             struct xwarcos_counter_node *);

/**
 * @brief AUTOSAR CP OS 计数器节点
 */
struct xwarcos_counter_node {
        xwid_t core_id;
        struct xwlib_bclst_node bcln;
        xwarcos_counter_operation_f operation;
        struct xwarcos_counter * counter;
};

/**
 * @brief AUTOSAR CP OS 计数器类型枚举
 */
enum xwarcos_counter_type_em {
        XWARCOS_COUNTER_TYPE_HARDWARE,
        XWARCOS_COUNTER_TYPE_SOFTWARE,
};

/**
 * @brief AUTOSAR CP OS 计数器配置
 */
struct xwarcos_counter_config {
        xwid_t core_id;
        xwtk_t ticks_per_base; /**< [ECUC_Os_00029] */
        xwtk_t max_allowed_value; /**< [ECUC_Os_00027] */
        xwtk_t min_cycle; /**< [ECUC_Os_00028] */
        xwtk_t nanoseconds_per_tick; /**< [ECUC_Os_00030] */
        enum xwarcos_counter_type_em type; /**< [ECUC_Os_00255] */
};

/**
 * @brief AUTOSAR CP OS 计数器
 */
struct xwarcos_counter {
        const struct xwarcos_counter_config * cfg;
        xwtk_t value;
        struct xwlib_bclst_head head;
        struct xwos_sqlk lock;
};

/**
 * @brief 校验计数器节点是否被正确配置
 * @param[in] node: 计数器节点的指针
 * @return 错误码
 * @retval E_XWARCOS_OK: 没有错误
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: 无效地址
 * @retval -E_XWARCOS_CORE: CPU ID不匹配
 */
xwer_t xwarcos_counter_node_validate(struct xwarcos_counter_node * node);

/**
 * @brief 向计数器的队列插入一个节点
 * @param[in] counter: 计数器的指针
 * @param[in] node: 节点的指针
 * @param[in] post_add: 节点加入后的回调函数
 * @return 错误码
 * @retval E_XWARCOS_OK: 无错误
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: 无效指针
 * @retval -E_XWARCOS_CORE: CPU ID不匹配
 * @details
 * + `post_add()` 在 `node` 插入到 `counter` 的队列之后被调用。
 * + `post_add()` 在锁 `counter->lock` 内调用。
 */
xwer_t xwarcos_counter_add(struct xwarcos_counter * counter,
                           struct xwarcos_counter_node * node,
                           void (* post_add)(struct xwarcos_counter_node *));

/**
 * @brief 从计数器的队列中删除一个节点
 * @param[in] counter: 计数器的指针
 * @param[in] node: 节点的指针
 * @param[in] pre_del: 节点删除前的回调函数
 * @return 错误码
 * @retval E_XWARCOS_OK: 无错误
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: 无效指针
 * @retval -E_XWARCOS_CORE: CPU ID不匹配
 * @details
 * + `pre_del()` 在 `node` 从 `counter` 的队列删除之前被调用。
 * + `pre_del()` 在锁 `counter->lock` 内调用。
 */
xwer_t xwarcos_counter_del(struct xwarcos_counter * counter,
                           struct xwarcos_counter_node * node,
                           void (* pre_del)(struct xwarcos_counter_node *));

/**
 * @brief 校验计数器是否被正确配置
 * @param[in] counter: 计数器的指针
 * @return 错误码
 * @retval E_XWARCOS_OK: 没有错误
 */
xwer_t xwarcos_counter_validate_and_init(struct xwarcos_counter * counter);

/**
 * @brief 递增软件计数器
 * @param[in] counter: 计数器的指针
 * @return 错误码
 * @retval E_XWARCOS_OK: 无错误
 * @retval -E_XWARCOS_ID: `counter` 无效
 * @retval -E_XWARCOS_CORE: CPU ID不匹配
 * @note
 * + 同步/异步：同步
 * + 重入性：可重入
 */
xwer_t xwarcos_counter_increment(struct xwarcos_counter * counter);

/**
 * @brief 读取计数器的当前计数值
 * @param[in] counter: 计数器的指针
 * @param[out] value: 指向缓冲区的指针，通过此缓冲区返回计数器的当前计数值
 * @return 错误码
 * @retval E_XWARCOS_OK: 无错误
 * @retval -E_XWARCOS_ID: `counter` 指针无效
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: `value` 指针无效
 * @note
 * + 同步/异步：同步
 * + 重入性：可重入
 */
xwer_t xwarcos_counter_get_value(struct xwarcos_counter * counter,
                                 xwtk_t * value);

/**
 * @brief 获取计数器的当前计数值，并计算以前计数值与当前计数值之间的间隔
 * @param[in] counter: 计数器的指针
 * @param[in,out] value: (in) 以前从计数器读取的计数值
 *                       (out) 计数器的当前计数值
 * @param[out] elapsed: 指向缓冲区的指针，通过此缓冲区返回
 *                      以前计数值与当前计数值之间的间隔
 * @return 错误码
 * @retval E_XWARCOS_OK: 无错误
 * @retval -E_XWARCOS_ID: `counter` 指针无效
 * @retval -E_XWARCOS_ILLEGAL_ADDRESS: `value` 或 `elapsed` 指针无效
 * @retval -E_XWARCOS_VALUE: `value` 缓冲区内的值超出范围
 * @note
 * + 同步/异步：同步
 * + 重入性：可重入
 */
xwer_t xwarcos_counter_get_elapsed_value(struct xwarcos_counter * counter,
                                         xwtk_t * value, xwtk_t * elapsed);

/**
 * @} arcos_counter
 */

#endif /* xwos/arcos/counter.h */
