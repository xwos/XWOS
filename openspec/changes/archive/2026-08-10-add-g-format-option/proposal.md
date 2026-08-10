# 为 xwvsnpf() 增加 %g 格式化选项

## Why

`xwvsnpf()` 目前支持 `%f`、`%e`/`%E`，但缺少 C 标准 `vsnprintf()` 中的 `%g`/`%G`（自动选择定点或科学计数法的最短形式）。作为"功能与 `vsnprintf()` 相同"的格式化函数，需要补齐 `%g`/`%G` 支持，同时修复既有 `%e` 舍入进位后未重新归一化指数的 bug。

## What Changes

- 为 `xwvsnpf_format_decode()` 增加 `%g`/`%G` 转换说明符解析（`%g` 小写 e，`%G` 大写 E），支持 `L` 修饰符（`%Lg` 对应 long double）
- 为 `xwvsnpf_format_float()` 和 `xwvsnpf_format_long_double()` 增加 general（`%g`）模式：
  - 精度语义由"小数位数"变为"有效数字位数"（默认 6，显式 0 视为 1）
  - 根据舍入后的指数 X 选择风格：`P > X ≥ −4` 用定点（f）风格，否则用科学（e）风格
  - 非 `#` 标志时移除尾随零与小数点
- 修复既有 `%e`/`%E` 的舍入进位 bug：mantissa 进位后（如 9.999→10.0）重新归一化并调整指数（`10.000e+03` → `1.000e+01`）
- 扩大浮点格式化的临时缓冲区以支持大指数场景

## Capabilities

### New Capabilities

- `spf-g-format`: 在 XWOS 格式化字符串库（`xwos/lib/xwspf.c`）中提供 `%g`/`%G`/`%Lg`/`%LG` 格式化选项，行为遵循 C99 `vsnprintf()` 语义

### Modified Capabilities

- 无（`openspec/specs/` 中暂无既有能力；`%e` 修复作为新能力 `spf-g-format` 的需求之一，因共用格式化代码路径）

## Impact

- 代码文件：`xwos/lib/xwspf.c`（枚举、解码器、两个浮点格式化函数）
- 编译开关：`XWLIBCFG_SPF_FLOAT`（`%g` 与 `%f` 共享此开关）、`XWLIBCFG_SPF_LONG_DOUBLE`（`%Lg`）
- 调用方：`xwvsnpf` 的所有调用方（`xwmd/libc/newlibac/sprintf.c`、`xwmd/libc/picolibcac/sprintf.c`、`xwcd/soc/arm64/.../soc_debug.c` 等）无需改动，自动获得新能力
- 无 API 变更、无 ABI 变更、无破坏性变更
