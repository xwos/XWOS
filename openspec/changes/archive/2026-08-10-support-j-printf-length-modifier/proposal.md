## Why

XWOS 自定义 printf 实现（`xwos/lib/xwspf.c`）已支持 `h`、`hh`、`l`、`ll`、`z`、`t` 等 C99 长度限定符，但缺少 `j` 限定符（对应 `intmax_t` / `uintmax_t`）。这导致使用 `intmax_t` 类型（C99 标准定义的最大宽度整数类型）的代码无法通过 XWOS 的 printf 系列函数正确格式化输出。

## What Changes

- 新增 C99 标准长度限定符 `j` 的支持，对应 `intmax_t` / `uintmax_t` 类型
- `%jd`、`%ji` 输出有符号 `intmax_t` 十进制
- `%ju` 输出无符号 `uintmax_t` 十进制
- `%jo` 输出 `uintmax_t` 八进制
- `%jx` / `%jX` 输出 `uintmax_t` 十六进制
- `%jb` / `%jB` 输出 `uintmax_t` 二进制（XWOS 扩展）

## Capabilities

### New Capabilities
- `j-printf-modifier`: 支持 C99 `j` 长度限定符，用于格式化 `intmax_t` 和 `uintmax_t` 类型

### Modified Capabilities
（无）

## Impact

- 受影响文件：`xwos/lib/xwspf.c`（约 4 处修改，~30 行新代码）
- 无 breaking change，现有功能完全不受影响
- 新增类型 `intmax_t`/`uintmax_t` 由工具链 `<stdint.h>` 提供，无需在 XWOS 类型系统中新增类型
