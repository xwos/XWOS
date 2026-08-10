# 任务：为 xwvsnpf() 增加 %g 格式化选项

## 1. 枚举与解码器

- [x] - [ ] 1.1 在 `xwos/lib/xwspf.c` 的 `enum xwvsnpf_format_type_em` 中新增 `XWVSNPF_FT_FLOAT_GENERAL` 与 `XWVSNPF_FT_LONG_DOUBLE_GENERAL`（位于 `#if XWLIBCFG_SPF_FLOAT` / `#if XWLIBCFG_SPF_LONG_DOUBLE` 块内，紧跟 `_SCI` 之后）
- [x] - [ ] 1.2 在 `xwvsnpf_format_decode()` 的 `#if XWLIBCFG_SPF_FLOAT` 块内新增 `case 'g'`：置 `XWVSNPF_F_SMALL`，按 `L` 修饰符解码为 `FT_LONG_DOUBLE_GENERAL` 或 `FT_FLOAT_GENERAL`
- [x] - [ ] 1.3 在 `xwvsnpf_format_decode()` 中新增 `case 'G'`：同 `'g'` 但不置 `XWVSNPF_F_SMALL`
- [x] - [ ] 1.4 在主循环分发 switch 中：`case XWVSNPF_FT_FLOAT_GENERAL:` 并入 `FT_FLOAT`/`FT_FLOAT_SCI` 分支；`case XWVSNPF_FT_LONG_DOUBLE_GENERAL:` 并入 long double 分支

## 2. 进位补丁（修复 %e bug，共用路径）

- [x] 2.1 在 `xwvsnpf_format_float()` 的四舍五入进位处（`frac_part >= mult` 分支）：e 风格时重归一化 mantissa（`int_part = 1; exp++`）
- [x] 2.2 在 `xwvsnpf_format_long_double()` 中执行同样修改
- [x] 2.3 编译验证：`%.3e 9999.9` 类场景经人工测试输出 `1.000e+04`

## 3. %g 核心实现（format_float）

- [x] 3.1 将 `is_sci` 扩展为 `is_sci_style` + `is_general`，新增 `digits`、`abs_orig`、`exp` 局部状态
- [x] 3.2 general 精度处理：P 缺省为 6，P==0 视为 1
- [x] 3.3 general 归一化与风格选择：归一化前保存 `abs_orig`；`exp < -4 || exp >= P` → e 风格（`digits = P - 1`），否则 f 风格（`digits = P - exp - 1`，`abs_num` 恢复为 `abs_orig`）
- [x] 3.4 将后续所有使用 `precision` 作为小数位数的地方替换为 `digits`（mult 连乘循环、`frac_part` 舍入、小数点输出条件、`put_float_decimal` 调用）
- [x] 3.5 指数段输出条件由 `is_sci` 改为 `is_sci_style`（指数符号大小写复用现有 `exp_char` 表达式，`%g` 由 `F_SMALL` 自动得到 `e`）
- [x] 3.6 f 风格进位补丁：进位后重算整数位数指数 `exp_new`，`exp_new >= P` 时转 e 风格（重归一化 mantissa、`digits = P - 1`），否则 `digits = P - exp_new - 1`
- [x] 3.7 `tmp[100]` 扩容为 `tmp[256]`

## 4. 尾随零移除辅助函数

- [x] 4.1 新增 `static inline char * xwvsnpf_format_strip_trailing_zeros(char * tmp, char * p)`：定位 `.` 与 `e`/`E`，从小数段尾部移除 `'0'`，全删则移除小数点，e 风格时 `memmove` 搬移指数段；仅尾部一个 return，不使用 `continue`
- [x] 4.2 在 `format_float` 中：general 且无 `XWVSNPF_F_SPECIAL` 时，于 `tmp[]` 拼装完成后、计算 `len` 之前调用该函数
- [x] 4.3 在 `format_long_double` 中执行同样的 tail-strip 调用

## 5. %g 核心实现（format_long_double）

- [x] 5.1 将 3.1-3.7 的全部修改同步到 `xwvsnpf_format_long_double()`（保持两函数行为一致）

## 6. 验证

- [x] 6.1 在 RPi4B 板级目录执行 `source env.sh && xwm` 编译整个工程，确认无警告无错误
- [x] 6.2 人工测试清单：`%g`/`%G`/`%Lg` 基础输出、有效数字舍入（123.4567→123.457）、风格选择边界（1234567→1.23457e+06、0.000012→1.2e-05、0.00012→0.00012）、尾零移除（1.500→1.5、# 保留）、进位补丁（%.3g 9999→1e+04、%.3e 9999.9→1.000e+04）、标志位（宽度/零填充/左对齐/正号）、nan/inf/-0.0
- [x] 6.3 运行 `make mc`（或 `make xwos.mc`）确认新代码通过 MISRA-C:2012 检查（尤其新辅助函数的 return 数量）
