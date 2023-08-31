/**
 * @file
 * @brief 外部模块配置
 * @author
 * + 隐星魂 (Roy Sun) <xwos@xwos.tech>
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

#ifndef __cfg_xwem_h__
#define __cfg_xwem_h__

#define XWEMCFG_serializing_nanopb                      0
#define XWEMCFG_fs_fatfs                                0
#define XWEMCFG_fs_littlefs                             0
#define XWEMCFG_fs_spiffs                               0
#define XWEMCFG_fs_dhara                                0

#define XWEMCFG_vm_lua                                  0
#define XWEMCFG_vm_lua_INT_TYPE                         LUA_INT_LONG
#define XWEMCFG_vm_lua_FLOAT_TYPE                       LUA_FLOAT_DOUBLE
#define XWEMCFG_vm_lua_THD_STACK_SIZE                   16384
#define XWEMCFG_vm_lua_BRDLIBS                          1

#endif /* cfg/xwem.h */
