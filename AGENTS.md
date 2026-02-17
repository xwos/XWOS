# XWOS 开发指南

本文档为在 XWOS（玄武操作系统）代码库中工作的智能代理提供指导，包括构建命令、代码风格和项目约定。

## 构建系统

XWOS 使用基于 Makefile 的构建系统，支持多平台、多架构。构建系统位于 `xwbs/` 目录。

### 环境设置

1. 进入目标板级目录（例如 `xwbd/WeActMiniStm32H750/`）
2. 执行 `source env.sh` 设置环境变量（该脚本必须通过 `source` 执行）
3. 环境会自动创建 workspace 目录并生成配置文件

```bash
cd xwbd/WeActMiniStm32H750
source env.sh
```

执行后，会自动切换到项目根目录，并加载 shell 辅助函数。

### 常用构建命令

| 命令 | 描述 | 用法示例 |
|------|------|----------|
| `xwm` | 编译整个工程（在板级目录中执行） | `xwm` |
| `xwm -B` | 重新编译（先清理） | `xwm -B` |
| `xwmm` | 编译当前路径下的玄武模块 | 进入模块目录后执行 `xwmm` |
| `xwmmm PATH` | 编译指定路径下的玄武模块 | `xwmmm xwam/example/cxx` |
| `xwcbd` | 切换到电路板工程目录 | `xwcbd` |
| `xwcroot` | 切换到项目根目录 | `xwcroot` |

### Makefile 目标

在板级目录中，可以直接使用 `make -j8` 命令：

- `make` ：编译整个工程
- `make c` ：清理编译产物
- `make d` ：深度清理（包括 workspace 目录）
- `make cfg`：重新生成配置文件

目前有两个工程，板级目录分别是 `xwbd/RPi4B` 和 `xwbd/WeActMiniStm32H750`

### 单模块编译

每个模块目录包含 `xwmo.mk` 文件，可以通过以下方式编译单个模块：

```bash
# 在模块目录中
xwmm

# 或从任何位置
xwmmm 相对路径
```

## 代码质量检查

### MISRA-C:2012 合规性检查

项目遵循 MISRA-C:2012 标准，使用 cppcheck 进行检查：

| 命令 | 描述 |
|------|------|
| `make mc` | 对整个项目进行 MISRA-C 检查 |
| `make arch.mc` | 检查架构相关代码 |
| `make xwos.mc` | 检查 XWOS 内核代码 |
| `make xwmd.mc` | 检查中间件代码 |
| `make board.mc` | 检查板级支持包代码 |
| `make %.mc` | 检查指定目录 |

检查配置位于 `xwbs/misra.c.mk` 和 `xwbs/misra.c.rule`，抑制规则在 `xwbs/misra-c2012-suppressions.txt` 中定义。

### 代码格式化

项目使用 `.clang-format` 文件定义代码格式，基于 Google 风格但有以下关键调整：

- **缩进**：8 空格（C 代码），4 空格（C++ 代码）
- **制表符**：不使用制表符（`UseTab: Never`）
- **行宽**：88 字符
- **指针对齐**：中间对齐（`PointerAlignment: Middle`）
- **大括号换行**：类、函数、命名空间后换行

格式化命令：
```bash
clang-format -i <文件>
```

## 代码风格指南

### 文件组织

- 头文件使用 `.h` 扩展名，源文件使用 `.c` 扩展名
- C++ 头文件使用 `.hxx`，源文件使用 `.cxx`
- 每个文件以 Doxygen 风格的注释开头，包含文件描述、作者和版权信息

### 命名约定

#### C

##### 类型和变量
- **类型定义**：使用 `xw` 前缀，如 `xwu32_t`、`xwer_t`
- **全局变量**：模块前缀 + 描述性名称，如 `xwlib_crc32tbl_0x04c11db7`
- **局部变量**：小写字母，单词间用下划线分隔
- **常量**：全大写，下划线分隔，如 `XWOS_PTRSIZE`

##### 函数
- 模块前缀 + 动作 + 对象，如 `xwlib_crc32_calc()`
- 静态函数无需模块前缀
- 内联函数使用 `__xwcc_inline` 标记

##### 宏
- 全大写，下划线分隔，如 `XWOS_BUG_ON()`
- 头文件保护宏：`__xwos_lib_compiler_h__`
- 配置宏：以 `CFG` 结尾，如 `XWLIBCFG_CRC32_0X04C11DB7`


#### C++

##### 类型和变量
- **类名**：帕斯卡命名法，如 `Bmp`、`Sem`、`Spinlock`、`Pm`
- **成员变量**：以 `m` 前缀开头，帕斯卡命名法，如 `mSemPtr`、`mSpinlock`、`mStatus`、`mCpuirq`
- **局部变量**：小写字母，单词间用下划线分隔
- **模板参数**：以 `T` 前缀开头，帕斯卡命名法，如 `TNum`、`TSelNum`、`TIrqList`
- **枚举值**：帕斯卡命名法，如 `SpinlockUnlocked`、`SpinlockLocked`、`PmStageSuspended`

##### 函数
- **成员函数**：帕斯卡命名法，如 `post()`、`wait()`、`freeze()`、`getXwosObj()`
- **静态函数**：以 `s` 前缀开头，如 `sThdMainFunction`
- **构造函数**：支持指针和引用重载
- **析构函数**：虚析构函数（如果需要）
- **RAII守卫类**：嵌套类，以 `Grd` 结尾，如 `LkGrd`、`TryLkGrd`

##### 宏
- **头文件保护宏**：格式为 `__xwos_cxx_文件名_hxx__`，全部小写，用下划线分隔
- **常量宏**：全大写，下划线分隔，如 `XWOS_PM_STAGE_SUSPENDED`

##### 命名空间
- 主命名空间：`xwos`
- 子命名空间：`sync`、`lock` 等模块分类
- 使用 `namespace xwos::sync` 嵌套风格

##### 文件组织
- C++ 头文件使用 `.hxx` 扩展名，源文件使用 `.cxx` 扩展名
- 每个文件以 Doxygen 风格的注释开头，包含文件描述、作者和版权信息

### 类型系统

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

### 错误处理

- 函数返回 `xwer_t` 类型表示错误码
- 成功返回 `0`，错误返回负值或特定错误码
- 使用 `IS_ERR_VALUE()`、`err_ptr()`、`ptr_err()` 等宏处理错误指针
- 参数验证使用 `XWOS_VALIDATE(exp, errstr, ...)` 宏

### 内存管理

- 使用 `__xwlib_code`、`__xwlib_rodata`、`__xwlib_data` 等段属性
- 静态内存分配为主，动态分配需谨慎使用

### 注释规范

1. **文件头注释**：Doxygen 格式，包含 `@file`、`@brief`、`@author`、`@copyright`
2. **函数注释**：Doxygen 格式，描述功能、参数、返回值
3. **代码注释**：中文或英文，清晰说明意图
4. **TODO/FIXME**：使用标准标记

示例：
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

### 中英文使用

- 注释可以使用中文或英文
- 标识符（变量、函数名）使用英文
- 文档字符串优先使用中文

## 提交规范

项目使用约定式提交，提交信息格式如下：

```
<type>(<scope>): :emoji: <subject>

[正文]

[脚注]
```

### 类型（type）
- `feat`：新功能
- `fix`： bug 修复
- `perf`：性能优化
- `refactor`：重构（不改变功能）
- `style`：代码风格调整
- `revert`：回滚
- `docs`：文档更新
- `test`：测试相关
- `ci`：持续集成
- `chore`：维护任务
- `milestone`：里程碑
- `release`：发布
- `revision`：修订
- `alpha`、`beta`、`rc`：预发布版本

### 格式要求
- 标题行不超过 72 字符（中文字符计 2 字符）
- 正文每行不超过 72 字符
- 使用动词开头，结尾无句号
- 可选的 emoji 前缀（非必须）

## 测试

项目运行在嵌入式目标板中，目前没有统一的单元测试框架，只能做编译测试。

## 工具要求

- **编译器**：只能使用xwtc工具箱内的编译器，通过 `xwtcwd` 命令获取其根目录
- **构建工具**：GNU Make 3.81+
- **静态分析**：cppcheck 2.7+
- **代码格式化**：clang-format 15+
- **版本控制**：Git

## 注意事项

1. **MISRA-C 合规性**：所有 C 代码必须通过 MISRA-C:2012 检查
2. **内存安全**：嵌入式环境需特别注意内存使用
3. **可移植性**：避免使用平台特定特性
4. **性能考虑**：实时操作系统需保证确定性的执行时间
5. **文档同步**：代码修改需更新相关注释和文档

## 参考资料

- `.clang-format`：代码格式化配置
- `xwbs/`：构建系统定义
- `xwos/lib/type.h`：类型定义
- `xwos/lib/error.h`：错误处理
- `.gitemplate`：提交信息模板
- `xwbs/misra-c2012-suppressions.txt`：MISRA-C 抑制规则

---

*最后更新：2026-02-18*
