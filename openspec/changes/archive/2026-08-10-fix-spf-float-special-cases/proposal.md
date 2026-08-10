# 修复 xwvsnpf() 浮点特殊值输出（%#.0f 冗余零与 nan/inf 符号）

## Why

`%g`/`%G` 支持（变更 2026-08-10-add-g-format-option）已实现并提交，宿主对比 glibc 测试 48/50 通过。剩余 2 个失败项此前判定为"既有 `%f`/`%e` 行为差异、不在范围"而保持现状。本次重新审视后决定修复：

1. `%#.0f` 输出 `3.0`（glibc 输出 `3.`）——精度 0 + `#` 标志时，小数点后不应输出数字。`%#.0g`（`%g` 新功能自身的边界行为）与 `%#.0e` 同样受影响
2. `-nan` 输出 `nan`（glibc 输出 `-nan`）——nan/inf 特殊值分支忽略符号标志（signbit/`+`/空格），glibc 对特殊值应用完整符号逻辑

这两个差异在既有 `%f`/`%e` 路径上已存在，但 `%#.0g` 是 `%g` 新功能自己的边界行为；且两者修复成本极低、向 glibc 对齐是正确的方向，故本次一并修复，并补充 `spf-g-format` 能力的 spec 场景。

## What Changes

- `xwvsnpf_put_float_decimal()` 的 `num == 0` 分支仅在 `digits > 0` 时输出 `'0'`（落实"至少 digits 位"语义），修复 `%#.0f`/`%#.0e`/`%#.0g` 及全部 long double 变体在精度 0 时的冗余 `0` 输出
- `xwvsnpf_format_float()` 与 `xwvsnpf_format_long_double()` 的 nan/inf 分支应用符号逻辑：`signbit` 为真输出 `-`，否则 `+` 标志输出 `+`，否则空格标志输出空格（与正常数值的符号处理一致）
- 主 spec `openspec/specs/spf-g-format/spec.md` 基线落盘（上次归档未同步成功），并更新"移除尾随零"与"特殊值与符号处理"需求

## Capabilities

### New Capabilities

- 无

### Modified Capabilities

- `spf-g-format`: 精度 0 + `#` 标志时小数点出现但小数点后不输出数字（`%#.0f`/`%#.0e`/`%#.0g`）；nan/inf 特殊值应用符号标志（signbit 负号、`+`、空格），符号大小写仍保持小写 `nan`/`inf`（对齐 musl，不随 `%G` 大写）

## Impact

- 代码文件：`xwos/lib/xwspf.c`（`xwvsnpf_put_float_decimal()` 与两个浮点格式化函数的 nan/inf 分支）
- 编译开关：无变化（仍受 `XWLIBCFG_SPF_FLOAT` / `XWLIBCFG_SPF_LONG_DOUBLE` 控制）
- 行为变更：`%f`/`%e`/`%g`（含 `%L` 变体）在 `#`+精度 0 与 nan/inf 场景的输出向 glibc 对齐；调用方（newlib/picolibc sprintf、soc_debug.c 等）自动继承，无需改动
- 无 API 变更、无 ABI 变更、无破坏性变更
