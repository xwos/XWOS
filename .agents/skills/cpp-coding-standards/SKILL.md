---
name: cpp-coding-standards
description: XWOS C++ 代码风格指南。用于在 XWOS（玄武操作系统）代码库中编写或审查 C++ 代码时遵循项目约定的编码规范。包括文件组织、命名约定、类型系统、代码格式化、错误处理、注释规范等。
---

# XWOS C++ 代码风格指南

本文档为在 XWOS（玄武操作系统）代码库中编写 C++ 代码提供指导。所有 C++ 代码必须遵循此编码标准以确保一致性、可读性和可维护性。

## 概述

XWOS 的 C++ 编码标准基于 Google C++ 风格指南，但针对嵌入式实时操作系统环境进行了调整。主要特点包括：
- 使用自定义类型系统确保可移植性
- 严格的命名约定
- RAII 资源管理
- 中文注释优先

## 文件组织

### 文件扩展名
- C++ 头文件使用 `.hxx` 扩展名
- C++ 源文件使用 `.cxx` 扩展名
- C 头文件使用 `.h` 扩展名
- C 源文件使用 `.c` 扩展名

### 头文件保护宏
头文件保护宏格式为 `__xwos_cxx_文件名_hxx__`，全部小写，用下划线分隔：

```cpp
#ifndef __xwos_cxx_Bmp_hxx__
#define __xwos_cxx_Bmp_hxx__

// ... 文件内容 ...

#endif /* xwos/cxx/Bmp.hxx */
```

### 文件头注释
每个文件必须以 Doxygen 风格的注释开头，包含文件描述、作者和版权信息：

```cpp
/**
 * @file
 * @brief xwos::Bmp
 * @author
 * + 隐星曜 (Roy Sun) <xwos@xwos.tech>
 * @copyright
 * + Copyright © 2015 xwos.tech, All Rights Reserved.
 * > This Source Code Form is subject to the terms of the Mozilla Public
 * > License, v. 2.0. If a copy of the MPL was not distributed with this
 * > file, You can obtain one at <http://mozilla.org/MPL/2.0/>.
 */
```

### 命名空间
使用嵌套命名空间风格：

```cpp
namespace xwos {
namespace lock {

class Spinlock
{
    // ...
};

} // namespace lock
} // namespace xwos
```

## 命名约定

### 类名
- 使用帕斯卡命名法（PascalCase）
- 示例：`Bmp`、`Sem`、`Spinlock`、`Pm`

### 成员变量
- 以 `m` 前缀开头，使用帕斯卡命名法
- 示例：`mBmp`、`mSpinlock`、`mStatus`、`mCpuirq`

### 局部变量
- 小写字母，单词间用下划线分隔
- 示例：`data_size`、`temp_buffer`

### 模板参数
- 以 `T` 前缀开头，使用帕斯卡命名法
- 示例：`TNum`、`TSelNum`、`TIrqList`

### 枚举值
- 使用帕斯卡命名法
- 示例：`SpinlockUnlocked`、`SpinlockLocked`、`PmStageSuspended`

### 函数

#### 成员函数
- 使用帕斯卡命名法
- 示例：`post()`、`wait()`、`freeze()`、`getXwosObj()`

#### 静态函数
- 以 `s` 前缀开头，使用帕斯卡命名法
- 示例：`sThdMainFunction`

#### 构造函数
- 支持指针和引用重载
- 示例：
  ```cpp
  explicit Bit(Bmp & bmp, xwsq_t index);
  explicit Bit(Bmp * bmp, xwsq_t index);
  ```

#### 析构函数
- 虚析构函数（如果需要）
- 示例：`~Bit()`

#### RAII 守卫类
- 嵌套类，以 `Grd` 结尾
- 示例：`LkGrd`、`TryLkGrd`、`LkThGrd`、`TryLkThGrd`

### 宏
- 头文件保护宏：`__xwos_cxx_文件名_hxx__`（全部小写）
- 常量宏：全大写，下划线分隔
- 示例：`XWOS_PM_STAGE_SUSPENDED`

## 代码格式化

项目使用 `.clang-format` 文件定义代码格式，基于 Google 风格但有以下关键调整：

### 关键设置
- **缩进**：4 空格（C++ 代码）
- **制表符**：不使用制表符（`UseTab: Never`）
- **行宽**：88 字符
- **指针对齐**：中间对齐（`PointerAlignment: Middle`）
- **大括号换行**：类、函数、命名空间后换行
- **命名空间缩进**：无（`NamespaceIndentation: None`）

### 访问修饰符
类成员访问修饰符缩进：

```cpp
class Spinlock
{
  public:
    // public members
  protected:
    // protected members
  private:
    // private members
};
```

### 格式化命令
```bash
clang-format -i <文件>
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
| `xwst_t` | 状态类型 |

## 错误处理

### 错误码
- 函数返回 `xwer_t` 类型表示错误码
- 成功返回 `0`，错误返回负值或特定错误码
- 使用 `IS_ERR_VALUE()`、`err_ptr()`、`ptr_err()` 等宏处理错误指针

### 参数验证
- 使用 `XWOS_VALIDATE(exp, errstr, ...)` 宏验证参数
- 示例：
  ```cpp
  XWOS_VALIDATE((nullptr != data), -EFAULT, "data is NULL");
  ```

## 注释规范

### 文件头注释
Doxygen 格式，包含 `@file`、`@brief`、`@author`、`@copyright`

### 函数注释
Doxygen 格式，描述功能、参数、返回值：
```cpp
/**
 * @brief 计算 CRC32 值
 * @param[in] data: 数据指针
 * @param[in] size: 数据大小
 * @param[in] crc: 初始 CRC 值
 * @return CRC32 结果
 */
xwu32_t xwlib_crc32_calc(const xwu8_t * data, xwsz_t size, xwu32_t crc);
```

### 代码注释
- 中文或英文，清晰说明意图
- 避免过度注释，代码应自解释

### TODO/FIXME
使用标准标记：
```cpp
// TODO: 需要优化算法
// FIXME: 边界条件处理
```

## 中英文使用

### 注释
- 注释可以使用中文或英文
- 文档字符串优先使用中文

### 标识符
- 变量、函数名等标识符使用英文
- 避免使用拼音或中英文混合

## 模板使用

### 类模板
```cpp
template<xwsz_t TNum>
class Bmp
{
  public:
    xwbmp_t mData[BITS_TO_XWBMP_T(TNum)];
};
```

### 模板参数推导
支持指针和引用：
```cpp
template<xwsz_t TSelNum>
xwer_t bind(Sel<TSelNum> * src, long pos);

template<xwsz_t TSelNum>
xwer_t bind(Sel<TSelNum> & src, long pos);
```

## C 与 C++ 混编

使用 `extern "C"` 包裹 C 头文件：
```cpp
extern "C" {
#include <xwos/lib/xwbop.h>
#include <string.h>
}
```

## 继承和组合

### 守卫类继承
```cpp
class TryLkGrd : public LkGrd
{
  private:
    xwer_t mRc;

  public:
    // ...
};
```

## 示例代码

### 完整类定义示例
```cpp
/**
 * @brief 自旋锁
 */
class Spinlock
{
  public:
    /**
     * @brief 锁状态
     */
    enum LockStatus : xwu32_t {
        SpinlockUnlocked = 0,
        SpinlockLocked,
    };

    /**
     * @brief 普通上锁模式下的自旋锁RAII机制守卫
     */
    class LkGrd
    {
      protected:
        Spinlock * mSpinlock;
        enum LockStatus mStatus;

      public:
        /**
         * @brief 构造自旋锁的RAII机制守卫
         * @param[in] spinlock: 自旋锁对象指针
         */
        LkGrd(Spinlock * spinlock);

        /**
         * @brief 析构自旋锁的RAII机制守卫
         */
        ~LkGrd();

        /**
         * @brief 获取锁状态
         */
        enum LockStatus getStatus() { return mStatus; }
    };

  private:
    struct xwos_splk mLock;

  public:
    Spinlock();
    ~Spinlock();
    struct xwos_splk * getXwosObj() { return &mLock; }
};
```

### RAII 使用示例
```cpp
#include <xwos/cxx/lock/Spinlock.hxx>

xwos::lock::Spinlock gLock;

void foo()
{
    // 构造 `grd` 时会自动对自旋锁上锁
    xwos::lock::Spinlock::LkGrd grd(gLock);
    // 进入临界区
} // grd析构时会自动解锁自旋锁
```

### 类定义示例
```cpp
template<xwsz_t TNum>
class Bmp
{
  public:
    class Bit
    {
      private:
        Bmp * mBmp;
        xwsq_t mBit;

      public:
        explicit Bit(Bmp & bmp, xwsq_t index);
        explicit Bit(Bmp * bmp, xwsq_t index);
        ~Bit();
        // ... 其他成员函数 ...
    };

  public:
    xwbmp_t mData[BITS_TO_XWBMP_T(TNum)];

  public:
    Bmp();
    ~Bmp();
    void set();
    void set(xwsq_t pos);
    // ... 其他成员函数 ...
};
```

### 错误处理示例
```cpp
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

## 注意事项

1. **MISRA-C 合规性**：所有 C 代码必须通过 MISRA-C:2012 检查
2. **内存安全**：嵌入式环境需特别注意内存使用，应尽量避免使用 `new` 和 `delete` 以及 `malloc()` 和 `free()`
3. **可移植性**：避免使用平台特定特性
4. **性能考虑**：实时操作系统需保证确定性的执行时间
5. **RAII 模式**：优先使用 RAII 模式管理资源
6. **异常处理**：XWOS 不使用 C++ 异常，使用错误码返回值
7. **避免使用异常和 RTTI**
8. **使用显式构造函数避免隐式转换**

## 参考资料

- `.clang-format`：代码格式化配置
- `xwos/cxx/`：C++ 代码示例
- `AGENTS.md`：完整开发指南
- `xwos/lib/type.h`：类型定义
- `xwos/lib/error.h`：错误处理

---

*最后更新：2026-02-23*
