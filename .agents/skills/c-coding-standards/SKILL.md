---
name: c-coding-standards
description: XWOS C 代码风格指南。用于在 XWOS（玄武操作系统）代码库中编写或审查 C 代码时遵循项目约定的编码规范。包括文件组织、命名约定、类型系统、代码格式化、错误处理、注释规范等。
---

# XWOS C 代码风格指南

本文档为在 XWOS（玄武操作系统）代码库中编写 C 代码提供指导。所有 C 代码必须遵循此编码标准以确保一致性、可读性和可维护性。

## 概述

XWOS 的 C 编码标准基于 MISRA-C:2012 标准，针对嵌入式实时操作系统环境进行了调整。主要特点包括：
- 使用自定义类型系统确保可移植性
- 严格的命名约定
- 静态内存分配为主，动态分配需谨慎使用
- 中文注释优先
- 必须通过 MISRA-C:2012 合规性检查

## 文件组织

### 文件扩展名
- C 头文件使用 `.h` 扩展名
- C 源文件使用 `.c` 扩展名
- C++ 头文件使用 `.hxx` 扩展名
- C++ 源文件使用 `.cxx` 扩展名

### 头文件保护宏
头文件保护宏格式为 `__xwos_文件路径_文件名_h__`，全部小写，用下划线分隔：

```c
#ifndef __xwos_mp_init_h__
#define __xwos_mp_init_h__

// ... 文件内容 ...

#endif /* xwos/mp/init.h */
```

### 文件头注释
每个文件必须以 Doxygen 风格的注释开头，包含文件描述、作者和版权信息：

```c
/**
 * @file
 * @brief XWOS MP内核：初始化
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */
```

### 包含顺序
头文件包含顺序应遵循以下规则：
1. XWOS 标准头文件（如 `xwos/standard.h`）
2. 系统头文件（如 `stdarg.h`）
3. 模块头文件

示例：
```c
#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/object.h>
#include <xwos/mm/common.h>
#include <xwos/mp/thd.h>
```

## 命名约定

### 类型和变量

#### 类型定义
- 使用 `xw` 前缀，如 `xwu32_t`、`xwer_t`
- 结构体类型使用 `struct xwmp_thd` 形式
- 枚举类型使用 `enum xwmp_sem_type_em` 形式

#### 全局变量
- 模块前缀 + 描述性名称，如 `xwlib_crc32tbl_0x04c11db7`
- 静态全局变量使用 `static` 关键字

#### 局部变量
- 小写字母，单词间用下划线分隔
- 示例：`data_size`、`temp_buffer`

#### 常量
- 全大写，下划线分隔，如 `XWOS_PTRSIZE`
- 枚举值使用帕斯卡命名法，如 `XWMP_SEM_TYPE_RT`

### 函数

#### 函数命名
- 模块前缀 + 动作 + 对象，如 `xwlib_crc32_calc()`
- 静态函数无需模块前缀
- 内联函数使用 `__xwcc_inline` 标记

#### 函数声明
函数声明应使用 Doxygen 格式注释：

```c
/**
 * @brief 计算 CRC32 值
 * @param[in] data: 数据指针
 * @param[in] size: 数据大小
 * @param[in] crc: 初始 CRC 值
 * @return CRC32 结果
 */
xwu32_t xwlib_crc32_calc(const xwu8_t * data, xwsz_t size, xwu32_t crc);
```

### 宏

#### 头文件保护宏
- 格式：`__xwos_模块名_文件名_h__`
- 示例：`__xwos_mp_sync_sem_h__`

#### 常量宏
- 全大写，下划线分隔
- 示例：`XWMP_SEM_NEGTIVE`

#### 配置宏
- 以 `CFG` 结尾，如 `XWLIBCFG_CRC32_0X04C11DB7`

#### 函数式宏
- 使用全大写，下划线分隔
- 多行宏使用 `do { ... } while (0)` 包裹
- 示例：
```c
#define XWOS_VALIDATE(exp, errstr, ...)       \
         if ((!(exp))) {                       \
             return __VA_ARGS__;               \
         }
```

## 类型系统

使用自定义类型定义确保可移植性：

| 类型 | 描述 |
|------|------|
| `xws8_t`, `xwu8_t` | 8 位有/无符号整数 |
| `xws16_t`, `xwu16_t` | 16 位有/无符号整数 |
| `xws32_t`, `xwu32_t` | 32 位有/无符号整数 |
| `xws64_t`, `xwu64_t` | 64 位有/无符号整数 |
| `xwptr_t` | 指针类型 |
| `xwer_t` | 错误码类型 |
| `xwsz_t` | 大小类型 |
| `xwsq_t` | 顺序类型 |
| `xwtm_t` | 时间类型 |
| `xwid_t` | ID 类型 |

### 结构体定义
结构体定义应包含详细注释：

```c
/**
 * @brief XWOS MP线程属性
 */
struct xwmp_thd_attr {
        const char * name; /**< 线程的名字 */
        xwstk_t * stack; /**< 栈内存的基地址 */
        xwsz_t stack_size; /**< 线程栈的大小 */
        xwsz_t stack_guard_size; /**< 栈内存警戒线 */
        xwpr_t priority; /**< 优先级 */
        bool detached; /**< 是否为分离态 */
        bool privileged; /**< 是否为特权线程 */
};
```

### 枚举定义
枚举定义应明确指定类型：

```c
/**
 * @brief 信号量类型枚举
 */
enum xwmp_sem_type_em {
        XWMP_SEM_TYPE_UNKNOWN = 0U,

#if defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
        XWMP_SEM_TYPE_PIPELINE = 1U, /**< 管道信号量：先进先出 */
#endif

#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
        XWMP_SEM_TYPE_RT = 2U, /**< 实时信号量：高优先级先出 */
#endif
};
```

## 错误处理

### 错误码
- 函数返回 `xwer_t` 类型表示错误码
- 成功返回 `0`，错误返回负值或特定错误码
- 使用 `IS_ERR_VALUE()`、`err_ptr()`、`ptr_err()` 等宏处理错误指针

### 参数验证
使用 `XWOS_VALIDATE` 宏验证参数：

```c
xwer_t function_example(const xwu8_t * data, xwsz_t size)
{
        xwer_t rc;

        XWOS_VALIDATE((nullptr != data), -EFAULT, "data is NULL");
        XWOS_VALIDATE((size > 0), -EINVAL, "size must be positive");

        // 处理逻辑
        rc = process_data(data, size);
        if (rc < 0) {
                goto err_process;
        }

        return XWOK;

err_process:
        // 错误处理
        return rc;
}
```

### 错误传播
- 使用 `goto` 语句进行错误处理
- 错误标签以 `err_` 为前缀
- 资源释放顺序与分配顺序相反

## 内存管理

### 内存段属性
使用段属性指定数据存放位置：

| 属性 | 描述 |
|------|------|
| `__xwmp_code` | MP内核代码段 |
| `__xwmp_rodata` | MP内核只读数据段 |
| `__xwmp_data` | MP内核数据段 |
| `__xwmp_bss` | MP内核BSS段 |

示例：
```c
static __xwmp_data struct xwmm_mempool_objcache xwmp_thd_cache;
const __xwmp_rodata char xwmp_thd_cache_name[] = "xwmp.thd.cache";
```

### 静态分配为主
- 优先使用静态内存分配
- 动态分配需谨慎使用，确保资源释放

## 注释规范

### 文件头注释
Doxygen 格式，包含 `@file`、`@brief`、`@author`、`@copyright`

### 函数注释
Doxygen 格式，描述功能、参数、返回值：

```c
/**
 * @brief 初始化自旋锁
 * @param[in] spl: 自旋锁对象的指针
 * @note
 * - 此函数不会关闭CPU中断
 * - 此函数不会关闭抢占
 */
void xwmp_splk_init(struct xwmp_splk * spl);
```

### 结构体/枚举注释
每个成员都应添加注释：

```c
/**
 * @brief 信号量对象
 */
struct xwmp_sem {
        struct xwmp_synobj synobj; /**< C语言面向对象：继承 `struct xwmp_synobj` */
        xwsq_t type; /**< 类型 */
        xwssq_t count; /**< 信号量计数器：<0，信号量处于负状态 */
        xwssq_t max; /**< 信号量计数器的最大值 */
        // ...
};
```

### 代码注释
- 中文或英文，清晰说明意图
- 避免过度注释，代码应自解释
- 复杂算法需详细注释

### TODO/FIXME
使用标准标记：
```c
// TODO: 未完成的工作
// FIXME: 需要优化算法
```

## 代码格式化

项目使用 `.clang-format` 文件定义代码格式，基于 Google 风格但有以下关键调整：

### 关键设置
- **缩进**：8 空格（C 代码），4 空格（C++ 代码）
- **制表符**：不使用制表符（`UseTab: Never`）
- **行宽**：88 字符
- **指针对齐**：中间对齐（`PointerAlignment: Middle`）
- **大括号换行**：函数后换行，结构体/枚举后不换行
- **访问修饰符**：无特殊缩进

### 结构体/枚举格式化
结构体和枚举的大括号不换行：

```c
struct xwmp_thd_attr {
        const char * name;
        xwstk_t * stack;
        xwsz_t stack_size;
        // ...
};

enum xwmp_sem_type_em {
        XWMP_SEM_TYPE_UNKNOWN = 0U,
        XWMP_SEM_TYPE_PIPELINE = 1U,
        XWMP_SEM_TYPE_RT = 2U,
};
```

### 函数格式化
函数的大括号换行：

```c
xwer_t xwmp_thd_init(struct xwmp_thd * thd,
                     const struct xwmp_thd_attr * inattr,
                     xwmp_thd_f thdfunc, void * arg)
{
        xwer_t rc;
        // 函数体
        return rc;
}
```

### 格式化命令
```bash
clang-format -i <文件>
```

## 中英文使用

### 注释
- 注释可以使用中文或英文
- 文档字符串优先使用中文
- 中文注释需清晰准确

### 标识符
- 变量、函数名等标识符使用英文
- 避免使用拼音或中英文混合
- 标识符应具有描述性

## 预处理指令

### 条件编译
- 使用 `#if defined(MACRO) && (1 == MACRO)` 格式
- 嵌套条件编译应有明确注释
- 示例：

```c
#if defined(XWOSCFG_SYNC_RTSEM) && (1 == XWOSCFG_SYNC_RTSEM)
#  define XWMP_SEM_API(api, ...) xwmp_rtsem_##api(__VA_ARGS__)
#elif defined(XWOSCFG_SYNC_PLSEM) && (1 == XWOSCFG_SYNC_PLSEM)
#  define XWMP_SEM_API(api, ...) xwmp_plsem_##api(__VA_ARGS__)
#endif
```

### 头文件包含保护
- 使用 `#ifndef`、`#define`、`#endif` 标准格式
- 注释中注明头文件路径

## 代码示例

### 完整头文件示例
```c
/**
 * @file
 * @brief XWOS MP内核：自旋锁
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#ifndef __xwos_mp_lock_spinlock_h__
#define __xwos_mp_lock_spinlock_h__

#include <xwos/standard.h>
#include <xwos/ospl/soc/spinlock.h>

/**
 * @brief 自旋锁
 */
struct xwmp_splk {
        struct soc_splk socsplk; /**< 架构描述层中的自旋锁结构体 */
};

/**
 * @brief 自旋锁的初始值
 */
#define XWMP_SPLK_INITIALIZER { .socsplk = SOC_SPLK_INITIALIZER, }

void xwmp_rawly_lock(struct xwmp_splk * spl);
xwer_t xwmp_rawly_trylock(struct xwmp_splk * spl);
void xwmp_rawly_unlock(struct xwmp_splk * spl);

#endif /* xwos/mp/lock/spinlock.h */
```

### 完整源文件示例
```c
/**
 * @file
 * @brief XWOS MP内核：线程
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */

#include <xwos/standard.h>
#include <xwos/lib/xwlog.h>
#include <xwos/lib/object.h>
#include <xwos/mp/thd.h>

/* #define XWOS_THDLOGF */ /**< 调试日志开关 */
#ifdef XWOS_THDLOGF
#  define xwos_thdlogf(lv, thd, fmt, ...) \
           xwlogf(lv, "Thd:%s", fmt, thd->stack.name, ##__VA_ARGS__)
#else
#  define xwos_thdlogf(lv, thd, fmt, ...)
#endif

#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
/**
 * @brief 结构体 `xwmp_thd` 的对象缓存
 */
static __xwmp_data struct xwmm_mempool_objcache xwmp_thd_cache;

/**
 * @brief 结构体 `xwmp_thd` 的对象缓存的名字
 */
const __xwmp_rodata char xwmp_thd_cache_name[] = "xwmp.thd.cache";
#endif

/**
 * @brief 初始化线程缓存
 */
#if defined(XWOSCFG_SKD_THD_MEMPOOL) && (1 == XWOSCFG_SKD_THD_MEMPOOL)
xwer_t xwmp_thd_cache_init(struct xwmm_mempool * mp, xwsq_t page_order)
{
        xwer_t rc;

        XWOS_VALIDATE((nullptr != mp), -EFAULT, "mp is NULL");

        rc = xwmm_mempool_objcache_init(&xwmp_thd_cache, mp,
                                        sizeof(struct xwmp_thd),
                                        page_order,
                                        xwmp_thd_cache_name);
        if (rc < 0) {
                goto err_mempool_objcache_init;
        }

        return XWOK;

err_mempool_objcache_init:
        return rc;
}
#endif
```

## 注意事项

- **MISRA-C 合规性**：所有 C 代码必须通过 MISRA-C:2012 检查
- **内存安全**：嵌入式环境需特别注意内存使用，避免内存泄漏和越界访问
- **可移植性**：避免使用平台特定特性，使用 XWOS 自定义类型
- **性能考虑**：实时操作系统需保证确定性的执行时间，避免动态内存分配
- **线程安全**：多核环境下需使用适当的同步机制
- **错误处理**：所有可能失败的函数必须返回错误码
- **资源管理**：遵循谁分配谁释放的原则
- **注释质量**：注释应准确反映代码意图，及时更新

## 参考资料

- `.clang-format`：代码格式化配置
- `AGENTS.md`：完整开发指南
- `xwos/lib/type.h`：类型定义
- `xwos/lib/error.h`：错误处理
- `xwbs/misra-c2012-suppressions.txt`：MISRA-C 抑制规则
- `xwos/mp/`：MP内核代码示例

---

*最后更新：2026-04-11*
