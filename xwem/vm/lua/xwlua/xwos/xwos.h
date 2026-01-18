/**
 * @file
 * @brief XWLUA库：XWOS内核
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
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

#ifndef __xwem_vm_lua_xwlua_xwos_h__
#define __xwem_vm_lua_xwlua_xwos_h__

#include <xwos/standard.h>
#include <xwos/osal/lock/mtx.h>
#include <xwos/osal/sync/sem.h>
#include <xwos/osal/sync/cond.h>
#include <xwos/osal/sync/flg.h>
#include <xwos/osal/sync/br.h>
#include <xwos/osal/sync/sel.h>
#include "xwem/vm/lua/src/lauxlib.h"

/**
 * @ingroup xwlua_xwos
 * @{
 */

/**
 * @brief 注册互斥锁对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: 互斥锁对象在Lua中的符号名
 * @param[in] mtx: 互斥锁对象的指针
 */
void xwlua_mtx_register(lua_State * L, const char * name,
                        struct xwos_mtx * mtx);

/**
 * @brief 注册信号量对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: 信号量对象在Lua中的符号名
 * @param[in] sem: 信号量对象的指针
 */
void xwlua_sem_register(lua_State * L, const char * name,
                        struct xwos_sem * sem);

/**
 * @brief 注册条件量对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: 条件量对象在Lua中的符号名
 * @param[in] cond: 条件量对象的指针
 */
void xwlua_cond_register(lua_State * L, const char * name,
                         struct xwos_cond * cond);

/**
 * @brief 注册事件标志对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: 事件标志对象在Lua中的符号名
 * @param[in] flg: 事件标志对象的指针
 */
void xwlua_flg_register(lua_State * L, const char * name,
                        struct xwos_flg * flg);

/**
 * @brief 注册线程栅栏对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: 线程栅栏对象在Lua中的符号名
 * @param[in] br: 线程栅栏对象的指针
 */
void xwlua_br_register(lua_State * L, const char * name,
                       struct xwos_br * br);

/**
 * @brief 注册信号选择器对象
 * @param[in] L: 虚拟机指针
 * @param[in] name: 信号选择器对象在Lua中的符号名
 * @param[in] sel: 信号选择器对象的指针
 */
void xwlua_sel_register(lua_State * L, const char * name,
                        struct xwos_sel * sel);

/**
 * @} xwlua_xwos
 */

#endif /* xwem/vm/lua/xwlua/xwos.h */
