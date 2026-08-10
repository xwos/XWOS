## Context

`xwos/lib/xwspf.c` 是 XWOS 自制 printf 实现。格式解码函数 `xwvsnpf_format_decode()`（第 877 行）解析 `%` 格式说明符，填充 `struct xwvsnpf_format_spec`。主函数 `xwvsnpf()`（第 1152 行）根据 `spec.type` 分派到对应的格式化函数。

当前支持的长度限定符（第 977 行）：`h`、`hh`（存为 `H`）、`l`、`ll`（存为 `L`）、`z`/`Z`、`t`。**缺少 `j`**（C99 intmax_t/uintmax_t）。

## Goals / Non-Goals

**Goals:**
- 在 `xwvsnpf_format_decode()` 中添加 `j` 限定符的解析
- 新增 `XWVSNPF_FT_INTMAX_T` 和 `XWVSNPF_FT_UINTMAX_T` 枚举值
- 在 `xwvsnpf()` 主分派循环中添加对应的 case
- 沿用现有 `xwvsnpf_format_number()` 格式化函数（复用 base/flags/precision 处理）
- 支持 `intmax_t`（注意：ARM64 LP64 ABI 上 `intmax_t` = `long`）

**Non-Goals:**
- 不新增 XWOS 类型系统别名（直接用工具链 `<stdint.h>` 的 `intmax_t`/`uintmax_t`）
- 不新增格式化函数（100% 复用 `xwvsnpf_format_number`）
- 不影响现有任何格式说明符的行为

## Decisions

### 决策 1：intmax_t/uintmax_t 类型来源

**选择**：使用工具链 `<stdint.h>` 提供的 `intmax_t` / `uintmax_t`。

**备选**：在 `xwos/lib/type.h` 中新增 `xwimax_t` / `xwuimax_t` 别名。

**理由**：
- `xwspf.c` 已通过 `<xwos/standard.h>` 间接使用标准库类型
- 现有 `t` 限定符（ptrdiff_t）和 `z` 限定符（xwsz_t）的混合模式表明无需新建别名
- `j` 限定符在 C99 标准中明确定义为 intmax_t，直接用标准名最清晰
- 减少改动面，不需要修改 `type.h`

### 决策 2：枚举值设计

**选择**：新增 2 个枚举值 — `XWVSNPF_FT_INTMAX_T` 和 `XWVSNPF_FT_UINTMAX_T`。

**备选**：新增 1 个值，复用 `XWVSNPF_F_SIGN` 标志区分有符号/无符号（类似 `FT_INT`/`FT_UINT`）。

**理由**：
- 与现有模式一致：`FT_LONG`/`FT_ULONG`、`FT_SHORT`/`FT_USHORT`、`FT_BYTE`/`FT_UBYTE` 各自独立
- 主分派循环中每个类型映射到唯一的 va_arg 调用，无需额外判断
- 代码清晰度优于节省 1 个枚举值

### 决策 3：数据流和控制流

```
  "%jd"
    │
    ▼
  xwvsnpf_format_decode()
    ├── 检测 'j' 限定符（第 977 行） → spec.qualifier = 'j'
    ├── 检测 'd' 类型字符（第 1105 行） → spec.flags |= F_SIGN
    └── 限定符分派（第 1116 行新增） → spec.type = FT_INTMAX_T
    │
    ▼
  xwvsnpf() main loop default: switch
    ├── case FT_INTMAX_T: num = (xwu64_t)va_arg(args, intmax_t)
    ├── case FT_UINTMAX_T: num = (xwu64_t)va_arg(args, uintmax_t)
    └── fallthrough → xwvsnpf_format_number(str, end, num, spec)
```

**关键点**：
- `intmax_t` / `uintmax_t` 转换为 `xwu64_t` 安全，因为 `xwu64_t` 是 64 位，
  C99 保证 `intmax_t` 至少 64 位，而 XWOS 目标平台（ARM64）上 `intmax_t` 恰好 64 位。
- 与现有 `FT_LONG_LONG` 等类型处理完全一致的代码路径。
- `xwvsnpf_format_number()` 内部通过 `F_SIGN` 标志正确处理负数。

### 决策 4：ARM64 ABI 兼容性

ARM64 LP64 ABI：
- `long` = 64 位
- `long long` = 64 位
- `intmax_t` = `long`（64 位）

`va_arg(args, intmax_t)` 和 `va_arg(args, long long)` 在二进制层面等价（都是 8 字节），
但使用正确的类型声明可避免编译警告和 MISRA 违规。

## Risks / Trade-offs

| 风险 | 影响 | 缓解措施 |
|------|------|----------|
| `intmax_t` 在某些目标平台上宽度超过 64 位 | 截断高位，输出错误 | 当前无此类平台；若未来出现，`xwu64_t` 需升级为 `xwu128_t` |
| `#include <stdint.h>` 重复引入 | 编译警告 | 由 `<xwos/standard.h>` 已间接引入，无需显式添加 |
| 与 `L` 限定符（ll）冲突 | `%jLf` 歧义 | `j` 限定符不用于浮点格式，无交集 |
