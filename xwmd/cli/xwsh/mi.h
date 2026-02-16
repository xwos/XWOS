/**
 * @file
 * @brief XWSH：模块集成接口
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwmd_cli_xwsh_mi_h__
#define __xwmd_cli_xwsh_mi_h__

#include <xwos/standard.h>

/**
 * @brief 命令
 */
struct xwsh_cmd {
        char * name; /**< 命令的名称 */
        xwer_t (*func)(xwsz_t argc, char ** argv); /**< 命令的函数 */
        char * desc; /**< 命令的描述 */
};

#define XWSH_MAXINPUT 128U /**< 一行命令可输入的最大字符数 */

/**
 * @brief 创建一个新的线程运行XWSH
 * @param[in] stack: 线程运行的栈
 * @param[in] stack_size: 线程运行的栈大小
 * @return 错误码
 * @note
 * + 上下文：中断、中断底半部、线程
 * @details
 * 此CAPI不可和 @ref xwsh_init() 以及 @ref xwsh_loop() 一起使用。
 */
xwer_t xwsh_start(xwstk_t * stack, xwsz_t stack_size);

/**
 * @brief 初始化XWSH
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * 此CAPI不可和 @ref xwsh_start() 一起使用。
 */
void xwsh_init(void);

/**
 * @brief 在现成的线程中运行XWSH主循环
 * @return 错误码
 * @note
 * + 上下文：线程
 * @details
 * 此CAPI不可和 @ref xwsh_start() 一起使用。使用此CAPI前，需先调用 @ref xwsh_init() 。
 */
void xwsh_loop(char * buf);

/**
 * @brief 设置外部命令表
 * @param[in] cmd: 命令数组
 * @param[in] num: 命令的数量
 * @return 错误码
 * @retval XWOK: 成功
 * @retval -EINVAL: 参数无效（cmd为NULL但num不为0）
 * @retval -EEXIST: 外部命令与内部命令重名，或外部命令之间重名
 * @note
 * + 上下文：线程
 * @details
 * 此函数应在XWSH启动前调用，用于设置用户自定义的命令表。
 * 外部命令不能与内置命令重名，外部命令之间也不能重名。
 * 若cmd为NULL且num为0，则清空外部命令表。
 * 允许重复调用，新设置会覆盖旧的命令表。
 */
xwer_t xwsh_set_ext_cmd_table(const struct xwsh_cmd cmd[], xwsz_t num);

#endif /* xwmd/cli/xwsh/mi.h */
