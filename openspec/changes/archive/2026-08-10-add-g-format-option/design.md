# 设计：为 xwvsnpf() 增加 %g 格式化选项

## Context

`xwos/lib/xwspf.c` 中的 `xwvsnpf()` 是 XWOS 的 `vsnprintf()` 等价实现（也是 newlib/picolibc 适配层 `vsnprintf()`/`sprintf()` 的后端）。当前浮点支持：

- `%f` → `XWVSNPF_FT_FLOAT`（double 定点）
- `%e`/`%E` → `XWVSNPF_FT_FLOAT_SCI`（double 科学计数法，`%e` 带 `XWVSNPF_F_SMALL` 标志决定指数符号大小写）
- `%Lf`、`%Le`/`%LE` → `XWVSNPF_FT_LONG_DOUBLE`/`_SCI`（受 `XWLIBCFG_SPF_LONG_DOUBLE` 控制）

实现集中在两个几乎逐字复制的大函数：`xwvsnpf_format_float()`（xwspf.c:448）与 `xwvsnpf_format_long_double()`（xwspf.c:582）。两者共享路径：nan/inf 特殊处理 → 符号处理 → （e 风格时）归一化指数 → 整数/小数分离 → `frac × 10^precision + 0.5` 四舍五入 → 拼 `tmp[100]` → 宽度/符号/零填充。

约束：
- MISRA-C:2012；函数至多尾部两个 return（一个正常、一个错误 goto）——**该约束仅适用于本次新增的函数**（决策点 1）
- 尽量不使用 `continue`
- 编译开关 `XWLIBCFG_SPF_FLOAT` 控制全部浮点格式，`XWLIBCFG_SPF_LONG_DOUBLE` 控制 long double 变体

## Goals / Non-Goals

**Goals:**
- 提供 `%g`/`%G`（及 `%Lg`/`%LG`）格式化，遵循 C99 `vsnprintf()` 语义（有效数字精度、风格选择、尾零移除、`#` 标志）
- 修复既有 `%e`/`%E` 舍入进位后未重新归一化指数的 bug
- 支持大指数场景（扩大临时缓冲区）
- 对现有 `%f`/`%e` 输出行为不产生除 bug 修复外的任何变化

**Non-Goals:**
- 不重构既有 `xwvsnpf_format_float()`/`xwvsnpf_format_long_double()` 的 return 结构（MISRA 双 return 约束只针对新函数）
- 不引入自动测试（测试由人工完成）
- 不实现 `%a`/`%A`（十六进制浮点）等其他缺失的转换说明符
- 不改变整数格式化的任何行为

## Decisions

### D1：方案 A——扩展现有浮点格式化函数（而非新建独立函数或重构抽公共）

`%g` 逻辑加入 `xwvsnpf_format_float()` 与 `xwvsnpf_format_long_double()` 内部，共享 nan/inf/符号/宽度/填充代码路径。

- **备选 B（独立函数）**：不采用——会再复制两份浮点格式化代码，总代码量膨胀 3 倍且行为容易漂移
- **备选 C（抽公共逻辑）**：不采用——需重构现有 `%f`/`%e` 路径，回归风险最高，与"MISRA 约束只针对新函数"的决策冲突

### D2：枚举与解码器扩展

枚举增加两个值（位于 `#if XWLIBCFG_SPF_FLOAT` 块内）：

```c
XWVSNPF_FT_FLOAT_GENERAL,        /* %g  double  */
XWVSNPF_FT_LONG_DOUBLE_GENERAL,  /* %Lg  long double */
```

`xwvsnpf_format_decode()` 在 `case 'f'` 附近增加 `case 'g'` 与 `case 'G'`，复用 `%e`/`%E` 的 `L` 修饰符分支结构：

- `case 'g'`：置 `XWVSNPF_F_SMALL`（指数用小写 `e`），`L` 修饰符 → `FT_LONG_DOUBLE_GENERAL`，否则 → `FT_FLOAT_GENERAL`
- `case 'G'`：同上但不置 `F_SMALL`（指数用大写 `E`）

主循环分发 switch 将 `FT_FLOAT_GENERAL` 并入 `FT_FLOAT`/`FT_FLOAT_SCI` 分支，`FT_LONG_DOUBLE_GENERAL` 并入 long double 分支。

### D3：%g 核心算法（在 format_float/format_long_double 内）

新增局部状态：

```c
int is_general = (spec.type == XWVSNPF_FT_FLOAT_GENERAL);
int is_sci_style;      /* 最终输出风格：true=e 风格，false=f 风格 */
int digits;            /* 实际小数位数（区别于 precision=有效位数 P） */
double abs_orig;       /* 归一化前的原值副本（f 风格使用） */
int exp = 0;
```

流程（仅 `is_general` 时介入，`%f`/`%e` 保持现有行为）：

```
P = precision（缺省 6；is_general && P==0 → P=1）
is_sci_style = (type == FLOAT_SCI)         # %e 直接 e 风格
if is_general:
    abs_orig = abs_num
    归一化循环 → exp                            # 与现有 %e 相同
    if (exp < -4 || exp >= P):  is_sci_style = true;  digits = P - 1
    else:                       is_sci_style = false; digits = P - exp - 1
                                abs_num = abs_orig      # 恢复原值走 f 逻辑
else:
    digits = precision
```

其后整数/小数分离、`mult = 10^digits`、`frac_part = frac*mult + 0.5`、进位处理、`tmp[]` 拼装、指数段输出（条件由 `is_sci` 改为 `is_sci_style`）、宽度/符号/填充——全部复用现有代码，仅把 `precision` 替换为 `digits`。

**风格判断基于归一化前的指数，但需在四舍五入后复核（见 D4 进位补丁）**——这是与 C99 语义（"style E conversion would have exponent X"）对齐的关键。

### D4：进位补丁（共用路径，同时修复 %e bug）

四舍五入进位后，`int_part` 可能不再匹配当前指数。在现有进位代码处（xwspf.c:521-524 与 655-658 的等价位置）追加：

```c
if (frac_part >= (unsigned long long)mult) {
    frac_part -= (unsigned long long)mult;
    int_part++;
    if (is_sci_style) {
        /* e 风格：mantissa 9→10，重归一化为 1.0e(exp+1) */
        int_part = 1;
        exp++;
    } else {
        /* f 风格：进位后整数位数可能增加，重新求指数 */
        xwu64_t n = int_part;
        int exp_new = 0;
        while (n >= 10) { n /= 10; exp_new++; }
        if (exp_new >= precision) {
            /* 如 9999.9 %.4g → 10000，X'=4 ≥ P → 转 e 风格 */
            int_part = (unsigned long long)n;
            exp = exp_new;
            digits = precision - 1;
            is_sci_style = true;
        } else if (exp_new > exp) {
            /* 如 19.9 %.2g → 20，X'=1：小数位减一 */
            exp = exp_new;
            digits = precision - exp_new - 1;
        }
    }
}
```

此逻辑位于共用路径，`%e`/`%E`/`%Le`/`%LE` 的进位行为随之被修复（`%.3e 9999.9` 从 `10.000e+03` 变为 `1.000e+04`）。

### D5：尾随零移除（新辅助函数）

`tmp[]` 拼装完成后、计算 `len` 之前，对 general 模式且无 `#` 标志的情况移除尾随零。抽为新函数（本次唯一新增函数，遵守 MISRA 尾部双 return 约束，实际仅一个 return）：

```c
static inline
char * xwvsnpf_format_strip_trailing_zeros(char * tmp, char * p);
```

- 扫描 `tmp` 定位小数点 `.` 与指数起始 `e`/`E`（`tmp` 中不可能出现字母 e/E 于别处）
- 小数段 = `(dot, tail)`，`tail` 为指数位置（e 风格）或 `p`（f 风格）
- 从 `tail` 向 `dot` 方向移除 `'0'`；若小数段全部移除，则同时移除小数点
- e 风格时用 `memmove` 将指数段（`e±XX`）搬移到新位置
- 返回新的字符串尾部指针

`#` 标志（`XWVSNPF_F_SPECIAL`）时不调用此函数，保留尾零与小数点。

### D6：临时缓冲区扩容

`xwvsnpf_format_float()` 与 `xwvsnpf_format_long_double()` 的 `tmp[100]` 扩容为 `tmp[256]`。原因：general 模式 f 风格的最坏输出 = 整数部分（`int_part` 为 u64，实际最多 20 位）+ `.` + `digits`（可达 P−1 位），`%.50g 1e30` 之类场景超出 100 字节。`xwvsnpf_put_float_decimal()` 内部 `i < 29` 的位数上限保持不变（u64 整数上限 20 位，29 已足够）。

### D7：格式大小写与特殊值

- `%g` 置 `XWVSNPF_F_SMALL` → `exp_char = 'e'`；`%G` 不置 → `'E'`。现有 `exp_char` 表达式直接复用
- nan/inf/符号处理完全复用现有代码，无改动
- 负数、`-0.0`、`+`/空格标志行为与现有 `%f`/`%e` 一致

## Risks / Trade-offs

| 风险 | 缓解 |
|------|------|
| `format_float`/`format_long_double` 函数体进一步膨胀（两个 ~180 行函数） | 接受，与现有"复制两份"风格一致；归一化+风格选择逻辑尽量以少量局部变量内联，避免新增 return |
| 浮点归一化循环（除以/乘以 10）引入舍入误差，`%g` 风格判断边界值（X 恰为 −4 或 P）可能偏差 | 与现有 `%e` 同源，行为一致即可；边界样例（0.00012、1234567）列入人工测试清单 |
| 进位补丁中 f 风格的指数重算使用 u64 循环，对超大 `int_part`（>2^64）会先被 `(u64)` 转换截断 | 与现有 `%f` 行为一致（`(unsigned long long)abs_num` 截断），非本次引入的问题 |
| `tmp[256]` 栈上占用增加 156 字节 × 2 函数 | 可接受；若不希望增大，可保持 100 但文档化有效数字上限，实现时按实际回归测试情况定夺 |
| 修 `%e` 进位 bug 可能改变依赖旧行为的调用方输出 | 旧行为是错误的（违反 C99），修复后输出才正确；调用方无需改动 |

## Migration Plan

- 无外部 API 变化，无需迁移
- 实现后按 AGENTS.md 流程做编译验证（`xwm`/`xwmm`），由人工完成功能验证
- 回滚：单文件 `xwos/lib/xwspf.c` 改动，git revert 即可

## Open Questions

- `tmp` 扩容到 256 还是保持 100 并接受有效数字位数限制？（实现时按人工测试结果决定，倾向 256）
- `%e` bug 修复是否需要单独 commit？（建议与 `%g` 分两个 commit，便于追溯）
