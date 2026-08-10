# 设计：修复 xwvsnpf() 浮点特殊值输出

## Context

`xwos/lib/xwspf.c` 的 `xwvsnpf()` 是 XWOS 的 `vsnprintf()` 等价实现。浮点格式化由 `xwvsnpf_format_float()`（double）与 `xwvsnpf_format_long_double()`（long double）两个几乎逐字复制的函数承担，共享 `xwvsnpf_put_float_decimal()`（输出无符号整数，左填充零至至少 `digits` 位）。

宿主对比 glibc 测试暴露 2 个既有差异（均已人工确认 glibc 行为）：

1. **`%#.0f`/`%#.0e`/`%#.0g` 输出冗余 `0`**：精度 0 + `#` 标志时输出 `3.0`/`3.0e+00`/`3.0`，glibc 为 `3.`/`3.e+00`/`3.`（小数点出现但无数字）
2. **nan/inf 忽略符号标志**：`-nan` 输出 `nan`（glibc `-nan`）、`%+f` nan 输出 `nan`（glibc `+nan`）、`% f` nan 输出 `nan`（glibc ` nan`）、`%+f` inf 输出 `inf`（glibc `+inf`）

用户决策：
- 决策 1：方案 A——在 `xwvsnpf_put_float_decimal()` 内部修复（而非调用点）
- 决策 2：第 2 档——完整符号逻辑（signbit/`+`/空格）应用到 nan 与 inf 两个特殊值分支
- 决策 3：并入 `spf-g-format` 能力，更新主 spec

## Goals / Non-Goals

**Goals:**
- 修复 `%#.0f`/`%#.0e`/`%#.0g`（含 long double 变体）精度 0 时的冗余 `0` 输出，与 glibc 对齐
- nan/inf 特殊值应用完整符号逻辑（signbit 负号、`+` 标志、空格标志），与 glibc 对齐
- 不改变 `%g` 其他任何已通过测试的行为

**Non-Goals:**
- 不实现 `%G`/`%F`/`%E` 的 `NAN`/`INF` 大写输出（决策 2 止于符号档；大小写保持小写，对齐 musl，与现有 `%e` 行为一致）
- 不修复 nan/inf 的宽度/填充行为（glibc `%10f` nan → `       nan`，现有实现提前 return 无宽度处理——既有行为，不在范围）
- 不重构 nan/inf 提前 return 结构（MISRA 双 return 约束只针对新函数，既有函数保持 3 return）
- 不引入自动测试（沿用人工测试）

## Decisions

### D1：修复 1 落点——`xwvsnpf_put_float_decimal()` 内部（方案 A）

当前实现（xwspf.c:421）：

```c
if (num == 0) {
        tmp[i++] = '0';
} else {
        while (num > 0 && i < 29) { ... }
}
while (i < digits && i < 29) { tmp[i++] = '0'; }
```

`num == 0` 分支无条件输出 `'0'`，即使 `digits == 0`。修复为仅在 `digits > 0` 时输出：

```c
if (num == 0) {
        if (digits > 0) {
                tmp[i++] = '0';
        }
} else {
        ...
}
```

**安全论证**（所有调用点逐一核对）：

| 调用点 | digits | num | 受影响 |
|--------|--------|-----|--------|
| 整数部分（int_part） | 恒 1 | 任意 | 否 |
| 小数部分（frac_part） | ∈ [0, P−1] | 仅当 digits==0 时恒为 0 | 是（修复目标） |
| 指数部分（exp） | 恒 1 | 任意（含 0） | 否 |

小数部分在 `digits == 0` 时 `mult == 1`，`frac_part = (u64)(frac + 0.5)`；若 `frac ≥ 0.5` 则触发进位修正（`frac_part -= mult` 归零、`int_part++`），因此 `digits == 0` 时 `frac_part` 恒为 0。`num == 0 && digits == 0` → 输出 0 位，即"至少 digits 位"语义的精确落实。

修复效果（float 与 long double 共享此函数，一处修改全覆盖）：

| 场景 | 修复前 | 修复后 |
|------|--------|--------|
| `%#.0f` 3.0 | `3.0` | `3.` |
| `%#.0f` 0.0 | `0.0` | `0.` |
| `%#.0g` 3.0 | `3.0` | `3.` |
| `%#.0g` 1234567.0 | `1.0e+06` | `1.e+06` |
| `%#.0e` 3.0 | `3.0e+00` | `3.e+00` |

非 `#` 的 `%.0f`（无 SPECIAL）不进入小数点分支，输出 `3`，不受影响（glibc 同样 `3`）。

### D2：修复 2 落点——nan/inf 分支内联符号逻辑（第 2 档）

两个浮点函数各有 nan 与 inf 两个提前 return 分支，共 4 处（float: xwspf.c:504-519；long double: 688-703）。符号处理在 nan/inf 分支内内联（正常数值的符号逻辑位于 521-531 行，nan/inf 提前返回不可达）。

**nan 分支**（float 与 long double 各一处）：

```c
if (isnan(num)) {
        if (signbit(num)) {
                if (buf < end) *buf++ = '-';
        } else if (spec.flags & XWVSNPF_F_PLUS) {
                if (buf < end) *buf++ = '+';
        } else if (spec.flags & XWVSNPF_F_SPACE) {
                if (buf < end) *buf++ = ' ';
        }
        if (buf < end) *buf++ = 'n';
        ...
}
```

**inf 分支**：现有 `if (num < 0) { '-' }` 对 `-inf` 已生效（`num` 为负无穷）。补充 `+` 与空格标志（`signbit` 对 `-inf` 与 `num < 0` 等价，保留现有判断最小改动）：

```c
if (isinf(num)) {
        if (num < 0) {
                if (buf < end) *buf++ = '-';
        } else if (spec.flags & XWVSNPF_F_PLUS) {
                if (buf < end) *buf++ = '+';
        } else if (spec.flags & XWVSNPF_F_SPACE) {
                if (buf < end) *buf++ = ' ';
        }
        ...
}
```

修复效果（float 与 long double 共 4 个分支一致修改）：

| 场景 | 修复前 | 修复后 |
|------|--------|--------|
| `%f` −NaN | `nan` | `-nan` |
| `%+f` NaN | `nan` | `+nan` |
| `% f` NaN | `nan` | ` nan` |
| `%+f` +∞ | `inf` | `+inf` |
| `%f` −∞ | `-inf` | `-inf`（不变） |

注意 `signbit(num)` 对 NaN 的符号位判断正确（`num < 0` 对 NaN 恒假，故 nan 分支必须用 `signbit`）；nan 分支现有 `isnan()` 提前返回意味着正常符号逻辑（521-531 行）不可达，内联符号逻辑与之等价、无重复处理。

### D3：主 spec 基线落盘

上次归档时"已同步到主 specs"未落盘（`openspec/specs/` 为空）。本次将归档变更的 `specs/spf-g-format/spec.md` 原样复制为 `openspec/specs/spf-g-format/spec.md` 作为基线，再以 delta spec（MODIFIED 两个需求）描述本次行为变化。

### D4：特殊值大小写策略

nan/inf 固定小写（`nan`/`inf`），不随 `%G`/`%F`/`%E` 大写。理由：与现有 `%e`/`%E` 行为一致、对齐 musl；glibc 的 `%G` → `NAN` 大写属于另一类差异，用户决策 2 未包含。已在 spec 中明确记录该约束。

## Risks / Trade-offs

| 风险 | 缓解 |
|------|------|
| 修改共享函数 `xwvsnpf_put_float_decimal()` 影响其他调用点 | 已逐一核对：唯一 `digits == 0` 的调用点是小数字段，且此时 `frac_part` 恒为 0（进位修正保证）；int_part/exp 调用均为 `digits == 1` |
| nan/inf 符号输出改变既有 `%f`/`%e` 行为 | 向 glibc 对齐的修正，调用方无需改动；`-inf` 行为不变，新增符号仅出现在显式使用 `+`/空格标志或负 NaN 的场景 |
| 4 处特殊值分支的重复代码 | 与现有"复制两份大函数"风格一致，每个函数内 nan/inf 相邻，可读性可接受 |
| MISRA-C:2012 检查 | 不新增 return，不引入新循环/continue，逻辑为纯分支扩展，预期无违规 |

## Migration Plan

- 无外部 API 变化，无需迁移
- 验证：宿主 gcc 对比 glibc 测试（沿用上次的 `/tmp/opencode/test_spf_g.c` 测试框架，加入本次 8 个新场景）；RPi4B 板级编译（`source env.sh && xwm`）；MISRA（`make xwos.mc`）
- 回滚：单文件 `xwos/lib/xwspf.c` 改动，git revert 即可
